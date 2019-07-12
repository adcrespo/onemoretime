/*
 * Compactacion.c
 *
 *  Created on: 15 jun. 2019
 *      Author: utnso
 */
#include "LFS.h"

#include "Compactacion.h"

int process_compactacion(char* path_tabla)
{
	int res = 0;
	loggear(logger, LOG_LEVEL_INFO,"Se esta por realizar la compactacion de %s",path_tabla);
	char* listasTmp[100];
	char* listasBin[100];
	int i;
	int j;
	char* rutaTabla = string_new();
	char* rutaTmp = string_new();
	char* rutaTmpc = string_new();

	DIR *dir;
	struct dirent *entry;

	//creamos una array vacio
	for (i = 0; 100 > i; i++) {
		listasTmp[i] = string_new();
	}

	for (j = 0; 100 > j; j++) {
			listasBin[j] = string_new();
		}

	//vamos guardando cada tmp en el array
	string_append(&rutaTabla, rutaTablas);
	string_append(&rutaTabla, path_tabla);


	if ((dir = opendir(rutaTabla)) == NULL) {
		perror("openndir() error");
	} else {
		i = 0;
		j= 0;
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

			} else {
				if (string_ends_with(entry->d_name, ".tmp")) {
					listasTmp[i] = entry->d_name;
					i++;
				}else if(string_ends_with(entry->d_name, ".bin")) {
					listasBin[j] = entry->d_name;
					j++;

				}
			}
		}
	}

	//Renombrar a .tmpc

	i=0;
	while(!string_is_empty(listasTmp[i])){
		string_append(&rutaTmp,rutaTabla);
		string_append(&rutaTmp, "/");
		string_append(&rutaTmp, listasTmp[i]);
		string_append(&rutaTmpc, rutaTmp);
		string_append(&rutaTmpc, "c");
		rename(rutaTmp, rutaTmpc);
		i++;
	}


	//Por cada .tmp:
		//Analizar registro por registro y compararlos contra el .bin (en memoria)
	t_list *listaBin = ObtenerRegistros(rutaTabla, ".bin");
	t_list *listaTmp = ObtenerRegistros(rutaTabla, ".tmpc");
	int cantListaTmp = list_size(listaTmp);
	int cantListaBin = list_size(listaBin);
	log_info(logger, "list size de registros tmp %d", cantListaTmp);
	log_info(logger, "list size de registros bin %d", cantListaBin);

	for (int i = 0; cantListaTmp > i; i++) {

		t_registro* registroTmp = list_get(listaTmp, i);

		bool find(void* element) {
			t_registro* registroBusqueda = element;
			return registroBusqueda->key == registroTmp->key;
		}

		t_registro* registroBin = list_find(listaBin, &find);
		if (registroBin == NULL) {
			list_add(listaBin, registroTmp);//Si la key no existe -> agregarlo
		} else {
			if (registroTmp->timestamp > registroBin->timestamp) {
				//Si la key existe pero el timestamp del .tmp es mas reciente -> agregarlo
				registroBin = registroTmp;
			}
			//Si no no hacer nada
		}
	}

		//Realizar la reasignacion de bloques
			//Bloquear la tabla y tomar timestamp
	bool findBloqueado(void* element) {
				t_tcb* registroBusqueda = element;
				return string_equals_ignore_case(registroBusqueda->nombre_tabla, path_tabla);
			}
	t_tcb* registroEncontrado = list_find(tablasGlobal,&findBloqueado);
	registroEncontrado->bloqueado = 1;
	unsigned long long timeStamp = obtenerTimeStamp();

			//Liberar los bloques que contengan el archivo “.tmpc”

	i = 0;
	while (!string_is_empty(listasTmp[i])) {
		log_info(logger, "Abriendo config de %s", listasTmp[i]);
		t_config *config_file = cargarConfiguracion(listasTmp[i], logger);
		int size = config_get_int_value(config_file, "SIZE");
		int cantBloques = CalcularBloques(size);
		char **bloques = malloc(sizeof(int) * cantBloques);
		bloques = config_get_array_value(config_file, "BLOCKS");
		for (int i = 0; cantBloques > i; i++) {
			char *bloque = string_from_format("%s%s.bin", rutaBloques,
					bloques[i]);
		}
		LiberarBloques(bloques,cantBloques);
		remove(listaTmp[i]);
		i++;
	}
			//Liberar los bloques que contengan el archivo “.bin”

	j = 0;
	while (!string_is_empty(listasBin[j])) {
		log_info(logger, "Abriendo config de %s", listasBin[j]);
		t_config *config_file = cargarConfiguracion(listasBin[j], logger);
		int size = config_get_int_value(config_file, "SIZE");
		int cantBloques = CalcularBloques(size);
		char **bloques = malloc(sizeof(int) * cantBloques);
		bloques = config_get_array_value(config_file, "BLOCKS");
		for (int j = 0; cantBloques > j; j++) {
			char *bloque = string_from_format("%s%s.bin", rutaBloques,
					bloques[j]);
		}
		LiberarBloques(bloques, cantBloques);
		remove(listaTmp[j]);
		j++;
	}
			//Solicitar los bloques necesarios para el nuevo archivo “.bin”
			//Grabar los datos en el nuevo archivo “.bin”
			//Desbloquer la tabla y tomar el tiempo cuanto estuvo bloqueada
	registroEncontrado->bloqueado = 0;
	closedir(dir);
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
