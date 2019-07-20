/*
 * Compactacion.c
 *
 *  Created on: 15 jun. 2019
 *      Author: utnso
 */
#include "LFS.h"

#include "Compactacion.h"

static int myCompare(const void* a, const void* b)
{

    // setting up rules for comparison
    return strcmp(*(const char**)a, *(const char**)b);
}

// Function to sort the array
void sort(const char* arr[], int n)
{
    // calling qsort function to sort the array
    // with the help of Comparator
    qsort(arr, n, sizeof(const char*), myCompare);
}

int process_compactacion(char* path_tabla)
{
	loggear(logger, LOG_LEVEL_INFO,"Se esta por realizar la compactacion de %s",path_tabla);
	char* listasTmp[100];
	char* listasBin[100];
	int i,j,res = 0;
	char* rutaTabla = string_new();

	DIR *dir;
	struct dirent *entry;

	string_append_with_format(&rutaTabla,"%s%s",rutaTablas,path_tabla);

	loggear(logger, LOG_LEVEL_INFO,"Ruta %s",rutaTabla);

	//creamos una array vacio
	for (i = 0; 100 > i; i++) listasTmp[i] = string_new();
	for (j = 0; 100 > j; j++) listasBin[j] = string_new();

	//vamos guardando cada tmp en el array

	if ((dir = opendir(rutaTabla)) == NULL) {
		perror("openndir() error");
	} else {
		i = 0;
		j= 0;
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

			} else {
				if (string_ends_with(entry->d_name, ".tmp") ||
						string_ends_with(entry->d_name, ".tmpc")) {
					string_append(&listasTmp[i],entry->d_name);
					i++;
				}else if(string_ends_with(entry->d_name, ".bin")) {
					string_append(&listasBin[j],entry->d_name);
					j++;

				}
			}
		}
	}
	closedir(dir);

	sort((const char**)listasBin, j);

	//Renombrar a .tmpc
	i=0;
	if(string_is_empty(listasTmp[i])){
		log_info(logger, "No hay tmp! %s",listasTmp[i]);
		return -1;
	}
	while(!string_is_empty(listasTmp[i])){
		if(string_ends_with(listasTmp[i],".tmpc")){
			i++;
			continue;
		}
		char* rutaTmp = string_new();
		char* rutaTmpc = string_new();
		string_append(&rutaTmp,rutaTabla);
		string_append(&rutaTmp, "/");
		string_append(&rutaTmp, listasTmp[i]);
		string_append(&rutaTmpc, rutaTmp);
		string_append(&rutaTmpc, "c");
		/*if(access( rutaTmp, F_OK ) != -1 ){
			free(rutaTmp);
			free(rutaTmpc);
			i++;
			continue;
		}*/
		rename(rutaTmp, rutaTmpc);
		string_append(&listasTmp[i],"c");
		free(rutaTmp);
		free(rutaTmpc);
		i++;
	}
	ReiniciarContadorTmp(path_tabla);

	//Por cada .tmpc:
		//Analizar registro por registro y compararlos contra el .bin (en memoria)
	t_list *listaTmp = ObtenerRegistros(rutaTabla, ".tmpc");
	int cantListaTmp = list_size(listaTmp);
	log_info(logger, "list size de registros tmp %d", cantListaTmp);
	t_list *listaBin = list_create();
	j=0;
	while(!string_is_empty(listasBin[j])){
		listasBin[j][strlen(listasBin[j])-4] = 0x00;
		t_list *listaBinReg = ObtenerRegistrosArchivo(rutaTabla, listasBin[j],".bin");
		log_info(logger, "list size de registros de %s %d", listasBin[j], list_size(listaBinReg));
		list_add(listaBin,listaBinReg);
		j++;
	}
	log_info(logger, "list size de registros bin %d", list_size(listaBin));

	for (int z = 0; cantListaTmp > z; z++){
		t_registro* registroTmp = list_get(listaTmp, z);
		t_list* listReg = list_get(listaBin,registroTmp->key%list_size(listaBin));
		t_registro* registroBin = NULL;
		int pos = 0;
		log_info(logger, "List count %d", listReg->elements_count);
		for(int z = 0; listReg!=NULL && z<listReg->elements_count; z++){
			log_info(logger, "Buscando Registro %d %llu", registroTmp->key,
					registroTmp->timestamp);
			registroBin = list_get(listReg,z);
			log_info(logger, "BIN Registro %d %llu", registroBin->key,
					registroBin->timestamp);
			if(registroTmp->key == registroBin->key){
				pos = z;
				log_info(logger, "Registro encontado pos: %d", pos);
				break;
			}
		}
		if (registroBin == NULL) {
			log_info(logger, "Insertando Registro %llu", registroTmp->timestamp);
			list_add(list_get(listaBin,registroTmp->key%list_size(listaBin)), registroTmp);//Si la key no existe -> agregarlo
		} else {
			log_info(logger, "Reemplazando Registro %llu", registroTmp->timestamp);
			if (registroTmp->key == registroBin->key &&
					registroTmp->timestamp > registroBin->timestamp) {
				//Si la key existe pero el timestamp del .tmp es mas reciente -> agregarlo
				log_info(logger, "Reemplazando Registro %llu (pos %d)", registroTmp->timestamp, pos);
				registroBin->key = registroTmp->key;
				registroBin->timestamp = registroTmp->timestamp;
				strcpy(registroBin->value,registroTmp->value);
				list_replace(listReg,pos,registroBin);
			}
			//Si no no hacer nada
		}
	}

		//Calcular la cantidad de bloques a asignar, teniendo en cuenta los que se van a liberar
	int bloquesLiberar = 0;
	i = 0;
	while (!string_is_empty(listasTmp[i])) {
		char *path = string_new();
		string_append_with_format(&path,"%s/%s",rutaTabla,listasTmp[i]);
		log_info(logger, "Abriendo tmp de %s", path);
		t_config *config_file = cargarConfiguracion(path, logger);
		int size = config_get_int_value(config_file, "SIZE");
		int cantBloques = CalcularBloques(size);
		bloquesLiberar+=cantBloques;
		free(path);
		i++;
	}
	int sizeListaBin = 0;
	j = 0;
	while (!string_is_empty(listasBin[j])) {
		char *path = string_new();
		string_append_with_format(&path,"%s/%s.bin",rutaTabla,listasBin[j]);
		log_info(logger, "Abriendo bin de %s", path);
		t_config *config_file = cargarConfiguracion(path, logger);
		int size = config_get_int_value(config_file, "SIZE");
		int cantBloques = CalcularBloques(size);
		bloquesLiberar+=cantBloques;
		t_list* listaBinRegistro = list_get(listaBin,j);
		for (int z = 0; listaBinRegistro != NULL && list_size(listaBinRegistro)> z; z++){
			t_registro* registroTmp = list_get(listaBinRegistro, z);
			char* timestamp = malloc(20);
			sprintf(timestamp, "%llu", registroTmp->timestamp);

			sizeListaBin += strlen(timestamp) + strlen(string_itoa(registroTmp->key))
					+ strlen(registroTmp->value) + 3;
			log_info(logger, "timestamp %s", timestamp);
			free(timestamp);
		}
		free(path);
		j++;
	}
	log_info(logger, "sizeListaBin %d", sizeListaBin);
	int bloquesAsignar = (sizeListaBin + tamanio_bloques - 1) / tamanio_bloques;
	int bloquesLibres = GetFreeBlocks();

	log_info(logger, "(Libres: %d, a_liberar: %d, a_asignar)",
					bloquesLibres, bloquesLiberar, bloquesAsignar);
	if(bloquesLibres + bloquesLiberar - bloquesAsignar < 0){
		log_info(logger, "No hay bloques libres ",
				bloquesLibres, bloquesLiberar, bloquesAsignar);
		return -1;
	}

		//Realizar la reasignacion de bloques
			//Bloquear la tabla y tomar timestamp
	unsigned long long timeStamp = obtenerTimeStamp();
	bool findBloqueado(void* element) {
		t_tcb* registroBusqueda = element;
		return string_equals_ignore_case(registroBusqueda->nombre_tabla, path_tabla);
	}
	t_tcb* registroEncontrado = list_find(tablasGlobal,&findBloqueado);
	registroEncontrado->bloqueado = 1;

	//*****************************
	//ESTO TIENE QUE ESTAR EN UN MUTEX?
			//TODO: Preguntar si hay bloquesdisponibles
			//Liberar los bloques que contengan el archivo “.tmpc”
	i = 0;
	while (!string_is_empty(listasTmp[i])) {
		char *path = string_new();
		string_append_with_format(&path,"%s/%s",rutaTabla,listasTmp[i]);
		log_info(logger, "Liberando tmp de %s", path);
		t_config *config_file = cargarConfiguracion(path, logger);
		int size = config_get_int_value(config_file, "SIZE");
		int cantBloques = CalcularBloques(size);
		char **bloques = malloc(sizeof(int) * cantBloques);
		bloques = config_get_array_value(config_file, "BLOCKS");
		LiberarBloques(bloques,cantBloques);
		remove(path);
		free(path);
		i++;
	}
			//Liberar los bloques que contengan el archivo “.bin”
	j = 0;
	while (!string_is_empty(listasBin[j])) {
		char *path = string_new();
		string_append_with_format(&path,"%s/%s.bin",rutaTabla,listasBin[j]);
		log_info(logger, "Liberando bin de %s", path);
		t_config *config_file = cargarConfiguracion(path, logger);
		int size = config_get_int_value(config_file, "SIZE");
		int cantBloques = CalcularBloques(size);
		char **bloques = malloc(sizeof(int) * cantBloques);
		bloques = config_get_array_value(config_file, "BLOCKS");
		LiberarBloques(bloques,cantBloques);
		remove(path);
		free(path);
		j++;
	}
			//TODO:Solicitar los bloques necesarios para el nuevo archivo “.bin”
			//TODO:Grabar los datos en el nuevo archivo “.bin”
	j = 0;
	while (!string_is_empty(listasBin[j])) {
		char *path = string_new();
		string_append_with_format(&path,"%s/%s.bin",rutaTabla,listasBin[j]);
		log_info(logger, "Asignando bin de %s", path);
		int sizeBin = 0;
		t_list* listaBinRegistro = list_get(listaBin,j);
		for (int z = 0; listaBinRegistro != NULL && list_size(listaBinRegistro)> z; z++){
			t_registro* registroTmp = list_get(listaBinRegistro, z);
			char* timestamp = malloc(20);
			sprintf(timestamp, "%llu", registroTmp->timestamp);
			sizeBin += strlen(timestamp) + strlen(string_itoa(registroTmp->key))
					+ strlen(registroTmp->value) + 3;
			free(timestamp);
		}

		log_info(logger, "Size Bin %d", sizeBin);

		t_list *lbloques = list_create();
		int size = sizeBin;
		int nroBLoque;
		if(size == 0){
			nroBLoque = AgregarBloque(lbloques);
			list_add(lbloques,(int *)nroBLoque);
			size = size - tamanio_bloques;
			log_info(logger, "Size Bin %d", sizeBin);
		}
		else {
			while(size>0){
				int nroBLoque= AgregarBloque(lbloques);
				list_add(lbloques,(int *)nroBLoque);
				size = size - tamanio_bloques;
				log_info(logger, "Size Bin %d", sizeBin);
			}
		}

		FILE *file = fopen(path, "w");

		if (file == NULL) {
			//loggear(logger, LOG_LEVEL_ERROR, "Error abriendo archivo %s", file);
			printf("Error abriendo archivo %s\n", path);
		}

		char *stringSize = string_from_format("SIZE=%d\n", sizeBin);
		fputs(stringSize, file);
		char *stringBlocks = string_from_format("BLOCKS=[");
		fputs(stringBlocks, file);
		for(int i= 0; lbloques!=NULL && i<lbloques->elements_count;i++){
			int bloque = list_get(lbloques,i);
			fputs(string_itoa(bloque), file);
			if(i==lbloques->elements_count-1)
				fputs("]", file);
			else
				fputs(",", file);
		}
		fclose(file);
		free(stringSize);
		free(stringBlocks);

		int disponibleActual = tamanio_bloques;

		for (int z = 0; listaBinRegistro != NULL && list_size(listaBinRegistro)> z; z++){
			t_registro *registro = list_get(listaBinRegistro, z);
			char *linea = string_new();
			char *key = string_new();
			string_append(&key, (string_itoa(registro->key)));
			//char *timestamp = string_new();
			char *timestamp = malloc(20);
			sprintf(timestamp, "%llu", registro->timestamp);
			string_append(&linea, timestamp);
			string_append(&linea, ";");
			string_append(&linea, key);
			string_append(&linea, ";");
			string_append(&linea, registro->value);
			string_append(&linea, "\n");
			free(key);
			free(timestamp);
			int lenLinea = strlen(linea);
			log_info(logger, "strlen de linea %s es %d", linea, lenLinea);

			int nBloque = 0;
			char *rutaActual;
			if(lenLinea < disponibleActual)
			{
				int bloque = (int)list_get(lbloques,nBloque);
				rutaActual = string_from_format("%s%d.bin", rutaBloques, bloque);
				disponibleActual -= lenLinea;
				GuardarEnBloque(linea, rutaActual);
				loggear(logger, LOG_LEVEL_INFO, "El disponible es: %d", disponibleActual);
			} else
			{
				nBloque++;
				int bloque = (int)list_get(lbloques,nBloque);
				rutaActual = string_from_format("%s%d.bin", rutaBloques,bloque);
				disponibleActual -= lenLinea;
				GuardarEnBloque(linea, rutaActual);
				loggear(logger, LOG_LEVEL_INFO, "El disponible es: %d", disponibleActual);
			}
			free(rutaActual);
		}

		j++;
	}
	//********************************

			//Desbloquer la tabla y tomar el tiempo cuanto estuvo bloqueada
	registroEncontrado->bloqueado = 0;
	unsigned long long tiempo = obtenerTimeStamp() - timeStamp;
	log_info(logger, "Tiempo consumido %d", tiempo);

	//Liberar
	i=0;
	while (!string_is_empty(listasTmp[i])) {
		free(listasTmp[i]);
		i++;
	}
	j = 0;
	while (!string_is_empty(listasBin[j])) {
		t_list* listaBinRegistro = list_get(listaBin,j);
		for (int z = 0; listaBinRegistro != NULL && list_size(listaBinRegistro)> z; z++){
			free(list_remove(listaBinRegistro,0));
		}
		list_destroy(listaBinRegistro);
		free(listasBin[j]);
		j++;
	}

	free(rutaTabla);

	return res;
}

void *crearCompactacion(void *pDatos_compactacion) {
	t_datos_compactacion datos_compactacion = *((t_datos_compactacion *) pDatos_compactacion);
	log_info(logger, "Compactando tabla %s en %d", datos_compactacion.path_tabla, datos_compactacion.retardo);
	while (1) {
		sleep(datos_compactacion.retardo/1000);
		process_compactacion(datos_compactacion.path_tabla);
	}

	return NULL;
}

int crearHiloCompactacion(int retardo, char* path_tabla)
{
//	t_datos_compactacion datos_compactacion = {.retardo = retardo, .path_tabla = string_from_format(path_tabla)};
	t_datos_compactacion *datos_compactacion = malloc(sizeof(t_datos_compactacion));
	datos_compactacion->retardo = retardo;
	strcpy(datos_compactacion->path_tabla, path_tabla);

//	hilo_compactacion = pthread_create(&compactacion, NULL, crearCompactacion, (void *) &datos_compactacion);
	hilo_compactacion = pthread_create(&compactacion, NULL, crearCompactacion, datos_compactacion);

	if (hilo_compactacion == -1)
		loggear(logger,LOG_LEVEL_INFO,"ERROR_HILO_COMPACTACION: %d", hilo_compactacion);
	log_info(logger, "Se generó el hilo para la COMPACTACION de tabla:%s.", path_tabla);

	return 1;
}
