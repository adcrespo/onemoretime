/*
 * Filesystem.c
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#include "Filesystem.h"

void *CrearFileSystem() {
	loggear(logger, LOG_LEVEL_INFO, "Creando fileSystem");

	rutaTablas = string_new();
	string_append(&rutaTablas, lfs_conf.punto_montaje);
	string_append(&rutaTablas, "Tables/");
	loggear(logger, LOG_LEVEL_INFO, "Ruta de tablas: %s", rutaTablas);
	rutaBloques = string_new();
	string_append(&rutaBloques, lfs_conf.punto_montaje);
	string_append(&rutaBloques, "Bloques/");
	loggear(logger, LOG_LEVEL_INFO, "Ruta de bloques: %s", rutaBloques);
	CargarMetadata();
	CargarBitmap();

	return (void*) 1;
}

void CargarMetadata() {
	rutaMetadata = string_new();
	string_append(&rutaMetadata, lfs_conf.punto_montaje);
	string_append(&rutaMetadata, "Metadata/");
	string_append(&rutaMetadata, "Metadata.bin");
	loggear(logger, LOG_LEVEL_INFO, "Ruta Metadata: %s", rutaMetadata);

	config_metadata = cargarConfiguracion(rutaMetadata, logger);

	if (config_has_property(config_metadata, "BLOCK_SIZE")) {
		tamanio_bloques = config_get_int_value(config_metadata, "BLOCK_SIZE");
		loggear(logger, LOG_LEVEL_INFO, "Tamanio de bloques: %d",
				tamanio_bloques);
	}

	if (config_has_property(config_metadata, "BLOCKS")) {
		cantidad_bloques = config_get_int_value(config_metadata, "BLOCKS");
		loggear(logger, LOG_LEVEL_INFO, "Cantidad de bloques: %d",
				cantidad_bloques);
	}

	config_destroy(config_metadata);

}

void CargarBitmap() {
	rutaBitmap = string_new();
	string_append(&rutaBitmap, lfs_conf.punto_montaje);
	string_append(&rutaBitmap, "Metadata/");
	string_append(&rutaBitmap, "Bitmap.bin");
	loggear(logger, LOG_LEVEL_INFO, "Ruta Bitmap: %s", rutaBitmap);

	/*int bm = open(rutaBitmap, O_RDWR);
	 struct stat mystat;

	 if(fstat(bm, &mystat) < 0){
	 loggear(logger, LOG_LEVEL_ERROR, "Error al establecer fstat");
	 }
	 fstat(bm, &mystat);
	 bmap = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, bm, 0);

	 bitmap = bitarray_create_with_mode(bmap, cantidad_bloques/8, MSB_FIRST);

	 loggear(logger, LOG_LEVEL_INFO, "Bitmap generado");

	 close(bm);
	 free(ruta_bitmap)*/
}

int ExisteTabla(const char *tabla) {
	loggear(logger, LOG_LEVEL_INFO, "Validando tabla: %s", tabla);

	char *ruta_tabla = string_from_format("%s%s", rutaTablas, tabla);
	log_info(logger, "Ruta tabla: %s", ruta_tabla);
	FILE *fp = fopen(ruta_tabla, "r");
	free(ruta_tabla);

	if (fp) {
		fclose(fp);
		loggear(logger, LOG_LEVEL_WARNING,
				"La tabla %s existe en el FileSystem", tabla);
		return 1;
	} else {
		return 0;
	}
}

t_metadata* ObtenerMetadataTabla(char *tabla) {
	loggear(logger, LOG_LEVEL_INFO, "Obteniendo metadata de tabla: %s", tabla);

	t_metadata *metadata = malloc(sizeof(t_metadata));
	char *rutaMetadata = string_from_format("%s%s/Metadata", rutaTablas, tabla);
	loggear(logger, LOG_LEVEL_INFO, "Obteniendo metadata en ruta %s",
			rutaMetadata);

	t_config *metadataFile = cargarConfiguracion(rutaMetadata, logger);

	metadata->particiones = config_get_int_value(metadataFile, "PARTITIONS");
	metadata->compactationTime = config_get_int_value(metadataFile,
			"COMPACTATION_TIME");
	char *consistencia = string_new();
	consistencia = config_get_string_value(metadataFile, "CONSISTENCY");
	strcpy(metadata->tipoConsistencia, consistencia);
	free(rutaMetadata);
	free(consistencia);
	//config_destroy(metadataFile);

	return metadata;
}

void ObtenerMetadataCompleto() {

	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(rutaTablas)) == NULL) {
		perror("openndir() error");
	} else {
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

			} else {
				printf("Metadata tabla: %s\n", entry->d_name);
				t_metadata *metadata;
				metadata = ObtenerMetadataTabla(entry->d_name);
				int particiones = metadata->particiones;
				int tiempoCompactacion = metadata->compactationTime;
				char *consistencia = string_new();
				strcpy(consistencia, metadata->tipoConsistencia);
				printf("CONSISTENCY=%s\n", consistencia);
				printf("PARTITIONS=%d\n", particiones);
				printf("COMPACTATION_TIME=%d\n", tiempoCompactacion);
				free(consistencia);
				free(metadata);
			}
		}

		closedir(dir);
	}
}

int CalcularParticion(int clave, int particiones) {
	int particion = clave % particiones;
	return particion;
}

void CrearDirectorioTabla(char *tabla) {
	char *rutaTabla = string_from_format("%s%s", rutaTablas, tabla);

	if (mkdir(rutaTabla, 0777) == -1) {
		//perror("mkdir");
		loggear(logger, LOG_LEVEL_ERROR, "Error creando directorio para: %s",
				tabla);
	} else {
		loggear(logger, LOG_LEVEL_INFO, "Directorio de %s creado", tabla);
	}

	free(rutaTabla);
}

void CrearMetadataTabla(char *tabla, char *consistencia, int particiones,
		int tiempoCompactacion) {
	loggear(logger, LOG_LEVEL_INFO, "Creando metadata para: %s", tabla);
	char *rutaMetadataTabla = string_from_format("%s%s/Metadata", rutaTablas,
			tabla);

	FILE *file = fopen(rutaMetadataTabla, "w");

	char *tipoConsistencia = string_from_format("CONSISTENCY=%s\n",
			consistencia);
	fputs(tipoConsistencia, file);
	char *partitions = string_from_format("PARTITIONS=%d\n", particiones);
	fputs(partitions, file);
	char *compactationTime = string_from_format("COMPACTATION_TIME=%d\n",
			tiempoCompactacion);
	fputs(compactationTime, file);

	free(tipoConsistencia);
	free(partitions);
	free(compactationTime);
	fclose(file);
}

t_tabla* BuscarTablaMemtable(char *nombre) {

	int EsLaTabla(t_tabla *tabla) {
		return string_equals_ignore_case(nombre, tabla->nombre_tabla);
	}

	loggear(logger, LOG_LEVEL_INFO, "Buscando %s en Memtable", nombre);
	return list_find(memtable, (void*) EsLaTabla);
}

void AlocarTabla(char *tabla, t_registro *registro) {
	t_tabla *listaTabla = malloc(sizeof(t_tabla));
	char *nombre = string_new();
	string_append(&nombre, tabla);
	strcpy(listaTabla->nombre_tabla, nombre);
	listaTabla->lista = list_create();
	list_add(memtable, listaTabla);
	list_add(listaTabla->lista, registro);
	int a = list_size(memtable);
	printf("Tamanio memtable :%d\n", a);
	free(nombre);

}

int InsertarTabla(t_request *request) {
	t_registro *registro = malloc(sizeof(t_registro));

	registro->key = atoi(request->parametro2);
	memcpy(registro->value, request->parametro3,
			strlen(request->parametro3) + 1);
	registro->timestamp = atoll(request->parametro4);

//	printf("Registro key %d\n", registro->key);
//	printf("Registro value %s\n", registro->value);
//	printf("Registro timestamp %llu\n", registro->timestamp);

	//Verifico existencia en el file system
	if (!ExisteTabla(request->parametro1)) {
		loggear(logger, LOG_LEVEL_WARNING, "%s no existe en el file system",
				request->parametro1);
		return 1;
	}

	//Verifico si no tiene datos a dumpear
	t_tabla *tabla = malloc(sizeof(t_tabla));
	tabla = BuscarTablaMemtable(request->parametro1);

	if (tabla == NULL) {
		//Aloco en memtable como nueva tabla
		loggear(logger, LOG_LEVEL_INFO, "%s no posee datos a dumpear",
				request->parametro1);
		AlocarTabla(request->parametro1, registro);
	} else {
		//Alocar en su posicion
		loggear(logger, LOG_LEVEL_INFO, "Alocando en su pos correspondiente");
		list_add(tabla->lista, registro);
	}

	return 0;
}

void CrearBloque(int numero, int bytes) {
	loggear(logger, LOG_LEVEL_INFO, "Creando bloque %d.bin", numero);
	char *rutaBloque = string_from_format("%s/%d.bin", rutaBloques, numero);

	FILE *binFile = fopen(rutaBloque, "w");

	char *bytesAEscribir = malloc(bytes);
	memset(bytesAEscribir, '\n', bytes);
	fwrite(bytesAEscribir, bytes, 1, binFile);

	free(rutaBloque);
	free(bytesAEscribir);
	fflush(binFile);

}

void GuardarEnBloque(char *linea, char *path) {
	loggear(logger, LOG_LEVEL_INFO, "Guardando linea :%s en: %s", linea, path);
	FILE *file = fopen(path, "a+");
	fseek(file, 0L, SEEK_END);
	//int len = ftell(file);
	//fseek(file, len, SEEK_SET);

	fwrite(linea, 1, strlen(linea), file);
	free(linea);
	fclose(file);
}

t_registro* BuscarKey(t_select *selectMsg) {

	//Verifico existencia en el file system
	if (!ExisteTabla(selectMsg->nombreTabla)) {
		loggear(logger, LOG_LEVEL_ERROR, "%s no existe en el file system",
				selectMsg->nombreTabla);
	}

	//Obtengo metadata
	t_metadata *metadata = ObtenerMetadataTabla(selectMsg->nombreTabla);
	int particiones = metadata->particiones;
	free(metadata);

	//Calculo particion de la key
	int particion = CalcularParticion(selectMsg->key, particiones);

	//Obtengo bloques de la particion
	char *rutaParticion = string_from_format("%s%s/part%d.bin", rutaTablas,
			selectMsg->nombreTabla, particion);
	loggear(logger, LOG_LEVEL_INFO, "configFile %s", rutaParticion);
	t_config *configFile = cargarConfiguracion(rutaParticion, logger);

	int sizeArchivo = config_get_int_value(configFile, "SIZE");
	int cantBloques = CalcularBloques(sizeArchivo);
	char **blocksArray = malloc(sizeof(int) * cantBloques);
	blocksArray = config_get_array_value(configFile, "BLOCKS");

	//Inicializo lista donde se concatenaran las restantes
	t_list *listaBusqueda = list_create();

	//Escaneo la particion
	int j = 0;
//	t_registro *registro;
//	while ((blocksArray[j] != NULL) && (registro->timestamp != 0)) {
	while (blocksArray[j] != NULL) {

		t_registro *registro = BuscarKeyParticion(selectMsg->key,
				blocksArray[j]);
		if (registro->timestamp != 0) {
			log_info(logger, "Registro encontrado en particion");
			list_add(listaBusqueda, registro);
		}
		j++;
	}

	//Si se encontro en particion agrego a la lista de busqueda
//	if (registro->timestamp != 0) {
//		log_info(logger, "Registro encontrado en particion");
//		list_add(listaBusqueda, registro);
//	}
	int sizeList = list_size(listaBusqueda);
	loggear(logger, LOG_LEVEL_INFO, "sizeLista %d", sizeList);

//	//Escaneo memtable
	t_list *listaMemtable = list_create();
	listaMemtable = BuscarKeyMemtable(selectMsg->key, selectMsg->nombreTabla);
	if (listaMemtable != NULL) {
		list_add_all(listaBusqueda, listaMemtable);
	} else {
		log_info(logger, "Lista de memtable vacia");
	}

	int sizeSelect = list_size(listaBusqueda);
	loggear(logger, LOG_LEVEL_INFO, "Lista de select tiene size %d",
			sizeSelect);

	//Escaneo temporales
	t_list *listaTemp;
	listaTemp = BuscarKeyTemporales(selectMsg->key, selectMsg->nombreTabla);
	list_add_all(listaBusqueda, listaTemp);

	int count = list_size(listaTemp);
	log_info(logger, "Coincidencias en temp :%d", count);

	for (int i = 0; i < count; i++) {
		t_registro *registro = malloc(sizeof(t_registro));
		log_info(logger, "Obteniendo registro %d", i);
		registro = list_get(listaTemp, i);
		log_info(logger, "***Registro %d de Temp***", i);
		log_info(logger, "Timestamp %llu", registro->timestamp);
		log_info(logger, "Key %d", registro->key);
		log_info(logger, "Value: %s", registro->value);
	}

	//Busco registro con mayor timestamp
	t_registro *registroInit = malloc(sizeof(t_registro));
	registroInit = list_get(listaBusqueda, 0);
	t_registro *registroAux = malloc(sizeof(t_registro));

	int sizeLista = list_size(listaBusqueda);
	for (int i = 0; i < sizeLista; i++) {
		registroAux = list_get(listaBusqueda, i);
		loggear(logger, LOG_LEVEL_INFO,
				"Elemento %d tiene value %s y timestamp %llu", i,
				registroAux->value, registroAux->timestamp);
		if (registroInit->timestamp < registroAux->timestamp) {
			registroInit = registroAux;
		}

	}

	loggear(logger, LOG_LEVEL_INFO, "El timestamp mayor es %llu",
			registroInit->timestamp);
	free(registroAux);
	free(selectMsg);
	if (listaMemtable != NULL)
		list_clean(listaMemtable);
	if (listaTemp != NULL)
		list_clean(listaTemp);
	list_clean(listaBusqueda);
	return registroInit;
}

t_list *BuscarKeyMemtable(int key, char *nombre) {
	loggear(logger, LOG_LEVEL_INFO, "Buscando key:%d en memtable de: %s", key,
			nombre);

	t_tabla *tabla = malloc(sizeof(t_tabla));
	tabla = BuscarTablaMemtable(nombre);

	if (tabla == NULL) {
		loggear(logger, LOG_LEVEL_WARNING,
				"La tabla %s no posee datos en memtable", nombre);
		return NULL;
	}

	int findKey(t_registro *registro) {
		return (registro->key == key);
	}

	return list_filter(tabla->lista, (void*) findKey);
}

t_list *BuscarKeyTemporales(int key, char *tabla) {
	t_list *listaTmp = list_create();
	char *pathTemps = string_from_format("%s%s", rutaTablas, tabla);
	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(pathTemps)) == NULL) {
		perror("openndir() error");
	}

	t_list *tempBlocksCollection = list_create();

	//busco todos los bloques de los tmp
	while ((entry = readdir(dir)) != NULL) {
		if (string_ends_with(entry->d_name, ".tmp")) {

			char *pathFile = string_from_format("%s/%s", pathTemps,
					entry->d_name);
			t_config *config_file = cargarConfiguracion(pathFile, logger);
			int size = config_get_int_value(config_file, "SIZE");
			int cantBloques = CalcularBloques(size);
			char **bloques = malloc(sizeof(int) * cantBloques);
			bloques = config_get_array_value(config_file, "BLOCKS");

			for (int i = 0; cantBloques > i; i++) {
				list_add(tempBlocksCollection, atoi(bloques[i]));
				free(bloques[i]);
			}

			free(pathFile);
			free(bloques);
		}
	}

	closedir(dir);

	//leo bloque por bloque y agrego registro si es la key buscada
	int lenghtCollection = list_size(tempBlocksCollection);
	for (int j = 0; lenghtCollection > j; j++) {
		int block = list_get(tempBlocksCollection, j);
		char *pathBlock = string_from_format("%s%d.bin", rutaBloques, block);
		log_info(logger, "Leyendo bloque: %s", pathBlock);

		//leer bloque y hacer add en listaTmp si es la key buscada
		char linea[100];
		char **elementos;
		FILE *file = fopen(pathBlock, "r");

		while (!feof(file))

		{
			fgets(linea, 100, file);
			elementos = string_split(linea, ";");
			int cantElementos = ContarElementosArray(elementos);

			if (atoi(elementos[1]) == key) {
				t_registro *registro = malloc(sizeof(t_registro));
				registro->timestamp = atoll(elementos[0]);
				registro->key = atoi(elementos[1]);
				char *value = string_new();
				string_append(&value, elementos[2]);
				value[strcspn(value, "\n")] = 0;
				strcpy(registro->value, value);
				list_add(listaTmp, registro);
				log_info(logger, "Elemento guardado con value %s",
						registro->value);
			}

			for (int i = 0; i < cantElementos; i++) {
				free(elementos[i]);
			}
			free(elementos);

		}

		free(pathBlock);
	}

	free(pathTemps);
	list_clean(tempBlocksCollection);
	return listaTmp;
}

t_registro* BuscarKeyParticion(int key, char *bloque) {
	loggear(logger, LOG_LEVEL_INFO, "Buscando key : %d en bloque: %s", key,
			bloque);
	char *pathBlock = string_from_format("%s%s.bin", rutaBloques, bloque);
	loggear(logger, LOG_LEVEL_INFO, "Ruta bloque : %s", pathBlock);
	char linea[100];
	char **elementos;

	FILE *file = fopen(pathBlock, "r");

	if (file == NULL) {
		loggear(logger, LOG_LEVEL_ERROR, "Error abriendo archivo %s", file);
	}
	loggear(logger, LOG_LEVEL_INFO, "Archivo %s abierto correctamente",
			pathBlock);
	t_registro *registro = malloc(sizeof(t_registro));
	while (!feof(file))

	{
		fgets(linea, 100, file);
		elementos = string_split(linea, ";");
		int cantElementos = ContarElementosArray(elementos);

		if (atoi(elementos[1]) == key) {
			registro->timestamp = atoll(elementos[0]);
			registro->key = atoi(elementos[1]);
			char *value = string_new();
			string_append(&value, elementos[2]);
			value[strcspn(value, "\n")] = 0;
			strcpy(registro->value, value);

			loggear(logger, LOG_LEVEL_INFO, "Timestamp:%llu",
					registro->timestamp);
			loggear(logger, LOG_LEVEL_INFO, "Key:%d", registro->key);
			loggear(logger, LOG_LEVEL_INFO, "Value:%s", registro->value);
			return registro;
		} else {
			registro->timestamp = 0;
		}

		for (int i = 0; i < cantElementos; i++) {
			free(elementos[i]);
		}
		free(elementos);

	}

	free(pathBlock);
	fclose(file);
	return registro;
}

int ContarElementosArray(char **cadena) {
	int contador = 0;

	while (cadena[contador] != '\0') {
		contador++;
	}

	return contador;
}

int CrearTabla(t_create *msgCreate) {

	//Verifico existencia en el file system
	if (ExisteTabla(msgCreate->nombreTabla)) {
		loggear(logger, LOG_LEVEL_ERROR, "%s ya existe en el file system",
				msgCreate->nombreTabla);
		return -1;
	}

	int particiones = msgCreate->num_part;

	//Creo Directorio
	CrearDirectorioTabla(msgCreate->nombreTabla);

	//Creo archivo metadata
	CrearMetadataTabla(msgCreate->nombreTabla, msgCreate->tipo_cons,
			particiones, msgCreate->comp_time);

	//Creo archivo binarios
	int particionInicial = 1;
	for (int i = 0; i < particiones; i++) {

		char *rutaParticion = string_from_format("%s%s/%s%d.bin", rutaTablas,
				msgCreate->nombreTabla, "part", particionInicial);
		printf("Abriendo particiones %s\n", rutaParticion);
		FILE *file = fopen(rutaParticion, "w");

		if (file == NULL) {
			//loggear(logger, LOG_LEVEL_ERROR, "Error abriendo archivo %s", file);
			printf("Error abriendo archivo %s\n", rutaParticion);
		}

		char *stringSize = string_from_format("SIZE=%d\n", 0);
		fputs(stringSize, file);
		int bloque = AgregarBloque();
		char *stringBlocks = string_from_format("BLOCKS=[%d]\n", bloque);
		fputs(stringBlocks, file);
		fclose(file);
		free(stringSize);
		free(stringBlocks);
		free(rutaParticion);
		particionInicial++;
	}

	//CreoHiloCompactacion
	//crearHiloCompactacion(msgCreate->comp_time, msgCreate->nombreTabla);

	return 0;
}

int AgregarBloque() {
	//Descomentar cuando se encuentre el bitmap del fs disponible
	/*size_t sizeBitmap = bitarray_get_max_bit(bitmap);
	 int bloque = 1;

	 log_info(logger, "Agregando bloque");

	 int count = 0;

	 while(count < sizeBitmap && bloque != -1)
	 {

	 if(bitarray_test_bit(bitmap, count ) == 0)
	 {
	 bitarray_set_bit(bitmap, count);
	 log_info(logger,"Bloque %d asignado", count);
	 bloque = count;
	 return bloque;
	 }
	 count ++;
	 }*/

	//return bloque;
	return 1;
}

int DropearTabla(char *nombre) {
	//Verifico existencia en el file system
	if (!ExisteTabla(nombre)) {
		loggear(logger, LOG_LEVEL_ERROR, "%s no puede ser dropeada", nombre);
		return 1;
	}

	DIR *dir;
	struct dirent *entry;

	char *path = string_from_format("%s%s", rutaTablas, nombre);
	char *pathMetadata = string_from_format("%s/Metadata", path);

	if ((dir = opendir(path)) == NULL) {
		perror("openndir() error");
	} else {
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")
					|| !strcmp(entry->d_name, "Metadata")) {

			} else {
//				printf("Archivo: %s\n", entry->d_name);
				char *pathFile = string_from_format("%s/%s", path,
						entry->d_name);
//				printf("Abriendo file %s\n", pathFile);
				t_config *config_file = cargarConfiguracion(pathFile, logger);

				int size = config_get_int_value(config_file, "SIZE");

				int cantBloques = CalcularBloques(size);
				char **bloques = malloc(sizeof(int) * cantBloques);
				bloques = config_get_array_value(config_file, "BLOCKS");

				LiberarBloques(bloques, cantBloques);
				LiberarMetadata(bloques, cantBloques);

//				printf("PATHFILE %s\n", pathFile);
				remove(pathFile);

				for (int i = 0; i <= cantBloques; i++) {
					free(bloques[i]);
				}
				free(bloques);

			}
		}

		closedir(dir);
	}

	remove(pathMetadata);
	remove(path);
	free(pathMetadata);
	free(path);
	return 0;
}

int CalcularBloques(int bytes) {
	int count;
	int aux = (bytes / tamanio_bloques);
	count = (bytes % tamanio_bloques == 0) ? aux : (aux + 1);

	return count;
}

void LiberarBloques(char **bloques, int cantBloques) {
	for (int i = 0; i < cantBloques; i++) {
		printf("Eliminando bloque %d\n", atoi(bloques[i]));
		//int pos = atoi(bloques[i]);
		//bitarray_clean_bit(bitmap, pos);

	}
}

void LiberarMetadata(char **bloques, int cant) {
	loggear(logger, LOG_LEVEL_INFO, "Eliminando metadata asociada");
	for (int i = 0; i < cant; i++) {
		char *pathMeta = string_from_format("%s%d.bin", rutaBloques,
				atoi(bloques[i]));
		loggear(logger, LOG_LEVEL_INFO, "Eliminando %s", pathMeta);
		remove(pathMeta);
		free(pathMeta);

	}
}

void LevantarHilosCompactacionFS() {
	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(rutaTablas)) == NULL) {
		perror("openndir() error");
	} else {
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

			} else {
				//printf("%s\n", entry->d_name);
				char *metadataFile = string_from_format("%s%s/Metadata",
						rutaTablas, entry->d_name);
				//printf("Ruta de metadata: %s\n", metadataFile);
				t_config *config_file = cargarConfiguracion(metadataFile,
						logger);

				int compactationTime = config_get_int_value(config_file,
						"COMPACTATION_TIME");
				//printf("Tiempo de compactacion tabla %s: %d\n", entry->d_name, compactationTime);
				//crearHiloCompactacion(compactationTime, entry->d_name);
			}
		}
	}
}

t_list *ObtenerRegistros(char *tabla, char *extension) {

	DIR *dir;
	struct dirent *entry;

	log_info(logger, "Obteniendo registros %s de %s", extension, tabla);
	if ((dir = opendir(tabla)) == NULL) {
		perror("openndir() error");
	} else {
		t_list *registros = list_create();
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
				//no hacer nada
			} else {
				if (string_ends_with(entry->d_name, extension)) {
					char *pathFile = string_from_format("%s/%s", tabla,
							entry->d_name);
					log_info(logger, "Abriendo config de %s", pathFile);
					t_config *config_file = cargarConfiguracion(pathFile,
							logger);

					int size = config_get_int_value(config_file, "SIZE");

					int cantBloques = CalcularBloques(size);
					char **bloques = malloc(sizeof(int) * cantBloques);
					bloques = config_get_array_value(config_file, "BLOCKS");
					for (int i = 0; cantBloques > i; i++) {
						char *bloque = string_from_format("%s%s.bin",
								rutaBloques, bloques[i]);
						//abrirArchivo
						log_info(logger, "ABRIENDO %s", bloque);
						FILE *archivo = fopen(bloque, "r+");
						char linea[100];
						char **elementos;

						//recorrer
						while (!feof(archivo)) {
							fgets(linea, 100, archivo);
							elementos = string_split(linea, ";");
							int cantElementos = ContarElementosArray(elementos);
							t_registro *registro = malloc(sizeof(t_registro));
							registro->timestamp = atoll(elementos[0]);
							registro->key = atoi(elementos[1]);
							strcpy(registro->value, elementos[2]);
							list_add(registros, registro);
							for (int i = 0; cantElementos > i; i++) {
								free(elementos[i]);
							}
							free(elementos);
						}
						free(bloque);
						fclose(archivo);
					}
					free(pathFile);
				}
			}
		}
		return registros;
	}
	closedir(dir);
	return NULL;
}

int ContarTablas() {
	DIR *dir;
	struct dirent *entry;
	int count = 0;

	char *path = string_from_format("%s", rutaTablas);

	if ((dir = opendir(path)) == NULL) {
		perror("openndir() error");
	}

	while ((entry = readdir(dir)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

		} else {
			count++;
		}
	}

	free(entry);
	free(dir);
	free(path);
	return count;
}

