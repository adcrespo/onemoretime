/*
 * Filesystem.c
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#include "Filesystem.h"

void *CrearFileSystem(){
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

	return (void*)1;
}

void CargarMetadata(){
	rutaMetadata = string_new();
	string_append(&rutaMetadata, lfs_conf.punto_montaje);
	string_append(&rutaMetadata, "Metadata/");
	string_append(&rutaMetadata, "Metadata.bin");
	loggear(logger, LOG_LEVEL_INFO, "Ruta Metadata: %s \n", rutaMetadata);

	config_metadata = cargarConfiguracion(rutaMetadata, logger);

	if(config_has_property(config_metadata,"BLOCK_SIZE"))
	{
		tamanio_bloques= config_get_int_value(config_metadata, "BLOCK_SIZE");
		loggear(logger, LOG_LEVEL_INFO, "Tamanio de bloques: %d", tamanio_bloques);
	}

	if(config_has_property(config_metadata,"BLOCKS"))
	{
		cantidad_bloques= config_get_int_value(config_metadata, "BLOCKS");
		loggear(logger, LOG_LEVEL_INFO, "Cantidad de bloques: %d", cantidad_bloques);
	}

	config_destroy(config_metadata);

}

void CargarBitmap(){
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

int ExisteTabla(const char *tabla)
{
	loggear(logger, LOG_LEVEL_INFO, "Validando tabla: %s", tabla);


	char *ruta_tabla = string_from_format("%s%s", rutaTablas, tabla);
	log_info(logger, "Ruta tabla: %s", ruta_tabla);
	FILE *fp = fopen(ruta_tabla, "r");
	free(ruta_tabla);

	if(fp)
	{
		fclose(fp);
		loggear(logger, LOG_LEVEL_WARNING, "La tabla %s existe en el FileSystem", tabla);
		return 1;
	}else
	{
		return 0;
	}
}

t_metadata* ObtenerMetadataTabla(char *tabla)
{
	loggear(logger, LOG_LEVEL_INFO, "Obteniendo metadata de tabla: %s", tabla);

	t_metadata *metadata = malloc(sizeof(t_metadata));
	char *rutaMetadata = string_from_format("%s%s/Metadata", rutaTablas, tabla);

	t_config *metadataFile = cargarConfiguracion(rutaMetadata, logger);

	metadata->particiones = config_get_int_value(metadataFile, "PARTITIONS");
	metadata->compactationTime = config_get_int_value(metadataFile, "COMPACTATION_TIME");
	char *consistencia = string_new();
	consistencia = config_get_string_value(metadataFile, "CONSISTENCY");
	strcpy(metadata->tipoConsistencia, consistencia);
	free(rutaMetadata);
	free(consistencia);
	//config_destroy(metadataFile);


	return metadata;
}

void ObtenerMetadataCompleto()
{

	DIR *dir;
	struct dirent *entry;

	if((dir=opendir(rutaTablas)) == NULL)
	{
		perror("openndir() error");
	} else
	{
		while((entry = readdir(dir)) != NULL)
		{
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			{

			} else
			{
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

int CalcularParticion(int clave, int particiones)
{
	int particion = clave % particiones;
	return particion;
}

void CrearDirectorioTabla(char *tabla)
{
	char *rutaTabla = string_from_format("%s%s", rutaTablas, tabla);


	if(mkdir(rutaTabla, 0777) == -1)
	{
		//perror("mkdir");
		loggear(logger, LOG_LEVEL_ERROR, "Error creando directorio para: %s", tabla);
	} else
	{
		loggear(logger, LOG_LEVEL_INFO, "Directorio de %s creado", tabla);
	}

	free(rutaTabla);
}

void CrearMetadataTabla(char *tabla, char *consistencia, int particiones, int tiempoCompactacion)
{
	loggear(logger, LOG_LEVEL_INFO, "Creando metadata para: %s", tabla);
	char *rutaMetadataTabla = string_from_format("%s%s/Metadata", rutaTablas, tabla);

	FILE *file = fopen(rutaMetadataTabla, "w");

	char *tipoConsistencia = string_from_format("CONSISTENCY=%s\n", consistencia);
	fputs(tipoConsistencia, file);
	char *partitions = string_from_format("PARTITIONS=%d\n", particiones);
	fputs(partitions, file);
	char *compactationTime = string_from_format("COMPACTATION_TIME=%d\n", tiempoCompactacion);
	fputs(compactationTime, file);


	free(tipoConsistencia);
	free(partitions);
	free(compactationTime);
	fclose(file);
}

t_tabla* BuscarTablaMemtable(char *nombre)
{

	int EsLaTabla(t_tabla *tabla)
	{
		return string_equals_ignore_case(nombre,tabla->nombre_tabla);
	}

	loggear(logger, LOG_LEVEL_INFO, "Buscando %s en Memtable", nombre);
	return list_find(memtable, (void*)EsLaTabla);
}

void AlocarTabla(char *tabla, t_registro *registro)
{
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

int InsertarTabla(t_request *request)
{
	t_registro *registro = malloc(sizeof(t_registro));

	registro->key = atoi(request->parametro2);
	memcpy(registro->value, request->parametro3, strlen(request->parametro3)+1);
	registro->timestamp = atoll(request->parametro4);

	printf("Registro key %d\n", registro->key);
	printf("Registro value %s\n", registro->value);
	printf("Registro timestamp %llu\n", registro->timestamp);

	//Verifico existencia en el file system
	if(!ExisteTabla(request->parametro1))
	{
		loggear(logger, LOG_LEVEL_WARNING, "%s no existe en el file system", request->parametro1);
		return 1;
	}


	//Verifico si no tiene datos a dumpear
	t_tabla *tabla = malloc(sizeof(t_tabla));
	tabla = BuscarTablaMemtable(request->parametro1);

	if(tabla == NULL)
	{
		//Aloco en memtable como nueva tabla
		loggear(logger, LOG_LEVEL_INFO, "%s no posee datos a dumpear", request->parametro1);
		AlocarTabla(request->parametro1, registro);
	}else
	{
		//Alocar en su posicion
		loggear(logger, LOG_LEVEL_INFO, "Alocando en su pos correspondiente");
		list_add(tabla->lista, registro);
	}

	return 0;
}



void CrearBloque(int numero, int bytes)
{
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

void GuardarEnBloque(char *linea, char *path)
{
	loggear(logger, LOG_LEVEL_INFO, "Guardando linea :%s en: %s", linea, path);
	FILE *file = fopen(path, "a+");
	fseek(file, 0L, SEEK_END);
	//int len = ftell(file);
	//fseek(file, len, SEEK_SET);

	fwrite(linea, 1, strlen(linea), file);
	free(linea);
	fclose(file);
}

void BuscarKey(int key, char *tabla)
{

	//char *nombreTabla = string_new();
	//strcpy(nombreTabla,request->parametro1);
	//int key = atoi(request->parametro2);

	//Verifico existencia en el file system
	if(!ExisteTabla(tabla))
	{
		loggear(logger, LOG_LEVEL_ERROR, "%s no existe en el file system", tabla);;
	}

	//Obtengo metadata
	t_metadata *metadata = ObtenerMetadataTabla(tabla);
	int particiones = metadata->particiones;
	free(metadata);

	//Calculo particion de la key
	int particion = CalcularParticion(key, particiones);

	//Obtengo bloques de la particion
	char *rutaParticion = (string_from_format("%s%s/%d.bin", rutaTablas, tabla, particion));

	t_config *configFile = cargarConfiguracion(rutaParticion, logger);
	int sizeArchivo = config_get_int_value(configFile, "SIZE");
	char **blocksArray = config_get_array_value(configFile, "BLOCKS");

	//Escaneo la particion
	int j = 0;
	while(blocksArray[j] != NULL)
	{
		//BuscarKeyBloque(key, blocksArray[j]);
		j++;
	}
	//Escaneo temporales
	//Escaneo memtable


}

void BuscarKeyMemtable(int key, char *nombre)
{
	printf("Buscando key en memtable de %s\n", nombre);
	sleep(10);

	t_tabla *tabla = malloc(sizeof(t_tabla));
	tabla = BuscarTablaMemtable(nombre);

	if(tabla == NULL)
	{
	} else
	{
		int sizeLista = list_size(tabla->lista);

		t_registro *registro = malloc(sizeof(t_registro));
		registro = list_get(tabla->lista, 0);


		for(int i = 0; i < sizeLista; i++)
		{
			t_registro *registroAux = malloc(sizeof(t_registro));
			registroAux = list_get(tabla->lista, i);


			printf("Registro\n key: %d\nvalue: %s\n", registro->key, registro->value);
			printf("RegistroAux\n key: %d\nvalue: %s\n", registroAux->key, registroAux->value);

		}
	}

}


void BuscarKeyBloque(int key, char *archivo)
{
	char *rutaArchivo = string_from_format("%s/%s", rutaBloques, archivo);
	char linea[50];
	char **elementos;

	FILE *file = fopen(rutaArchivo, "r");

	if(file==NULL)
	{
		loggear(logger, LOG_LEVEL_ERROR, "Error abriendo archivo %s", file);
	}

	while(!feof(file))

	{
		fgets(linea, 50, file);
		elementos = string_split(linea, ";");
		int cantElementos = ContarElementosArray(elementos);

		if(atoi(elementos[1]) == key)
		{
			t_registro *registro = malloc(sizeof(t_registro));
			char *value = string_new();
			string_append(&value, elementos[2]);
			registro->timestamp = atoll(elementos[0]);
			registro->key = atoi(elementos[1]);
			strcpy(registro->value, value);

			printf("Timestamp:%d\n", registro->timestamp);
			printf("Key:%d\n", registro->key);
			printf("Value:%s\n", registro->value);
			printf("\n");

			free(value);
		}

		for(int i = 0; i < cantElementos; i++)
		{
			free(elementos[i]);
		}

	}


	free(rutaArchivo);
	fclose(file);

}


int ContarElementosArray(char **cadena)
{
	int contador = 0;

	while(cadena[contador] != '\0')
	{
		contador++;
	}

	return contador;
}


int CrearTabla(t_create *msgCreate)
{

	//Verifico existencia en el file system
	if(ExisteTabla(msgCreate->nombreTabla))
	{
		loggear(logger, LOG_LEVEL_ERROR, "%s ya existe en el file system", msgCreate->nombreTabla);
		return -1;
	}

	int particiones = msgCreate->num_part;

	//Creo Directorio
	CrearDirectorioTabla(msgCreate->nombreTabla);

	//Creo archivo metadata
	CrearMetadataTabla(msgCreate->nombreTabla, msgCreate->tipo_cons, particiones, msgCreate->comp_time);

	//Creo archivo binarios
	int particionInicial = 1;
	for(int i = 0; i < particiones; i++)
	{

		char *rutaParticion = string_from_format("%s%s/%s%d.bin", rutaTablas, msgCreate->nombreTabla, "part",particionInicial);
		printf("Abriendo particiones %s\n", rutaParticion);
		FILE *file = fopen(rutaParticion, "w");

		if(file==NULL)
		{
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
	crearHiloCompactacion(msgCreate->comp_time, msgCreate->nombreTabla);


	return 0;
}

int AgregarBloque()
{
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

int DropearTabla(char *nombre)
{
	//Verifico existencia en el file system
	if(!ExisteTabla(nombre))
	{
		loggear(logger, LOG_LEVEL_ERROR, "%s no puede ser dropeada", nombre);
		return 0;
	}

	DIR *dir;
	struct dirent *entry;

	char *path = string_from_format("%s%s", rutaTablas, nombre);
	char *pathMetadata = string_from_format("%s/Metadata", path);


	if((dir=opendir(path)) == NULL)
		{
			perror("openndir() error");
		} else
		{
			while((entry = readdir(dir)) != NULL)
			{
				if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") || !strcmp(entry->d_name, "Metadata"))
				{

				} else
				{
					printf("Archivo: %s\n", entry->d_name);
					char *pathFile = string_from_format("%s/%s", path, entry->d_name);
					printf("Abriendo file %s\n", pathFile);
					t_config *config_file = cargarConfiguracion(pathFile,logger);


					int size = config_get_int_value(config_file, "SIZE");

					int cantBloques = calcularBloques(size);
					char **bloques = malloc(sizeof(int) * cantBloques);
					bloques = config_get_array_value(config_file, "BLOCKS");



					liberarBloques(bloques, cantBloques);
					liberarMetadata(bloques, cantBloques);


					printf("PATHFILE %s\n", pathFile);
					remove(pathFile);

					for(int i = 0; i<= cantBloques; i++)
					{
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
	return 1;
}


int calcularBloques(int bytes)
{
	int a = (bytes/tamanio_bloques);
	a++;
	return a;
}

void liberarBloques(char **bloques, int cantBloques)
{
	for(int i = 0; i < cantBloques; i++)
	{
		printf("Eliminando bloque %d\n", atoi(bloques[i]));
		//int pos = atoi(bloques[i]);
		//bitarray_clean_bit(bitmap, pos);

	}
}

void liberarMetadata(char **bloques, int cant)
{
	loggear(logger, LOG_LEVEL_INFO, "Eliminando metadata asociada");
	for(int i = 0; i < cant; i++)
	{
		char *pathMeta = string_from_format("%s%d.bin", rutaBloques, atoi(bloques[i]));
		loggear(logger, LOG_LEVEL_INFO, "Eliminando %s", pathMeta);
		remove(pathMeta);
		free(pathMeta);

	}
}

void LevantarHilosCompactacionFS()
{
	DIR *dir;
	struct dirent *entry;

	if((dir=opendir(rutaTablas)) == NULL)
		{
			perror("openndir() error");
		} else
		{
			while((entry = readdir(dir)) != NULL)
			{
				if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
				{

				} else
				{
					printf("%s\n", entry->d_name);
					char *metadataFile = string_from_format("%s%s/Metadata", rutaTablas, entry->d_name);
					printf("Ruta de metadata: %s\n", metadataFile);
					t_config *config_file = cargarConfiguracion(metadataFile,logger);

					int compactationTime = config_get_int_value(config_file, "COMPACTATION_TIME");
					printf("Tiempo de compactacion tabla %s: %d\n", entry->d_name, compactationTime);
					//crearHiloCompactacion(compactationTime, entry->d_name);
				}
			}
		}
}

t_list *obtenerRegistroBin(char *tabla) {

	DIR *dir;
	struct dirent *entry;

	//vamos guardando cada tmp en el array

	if ((dir = opendir(tabla)) == NULL) {
		perror("openndir() error");
	} else {
		t_list *registrosBin = list_create();
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

			} else {
				if (string_ends_with(entry->d_name, ".bin")) {
					printf("Archivo: %s\n", entry->d_name);
					char *pathFile = string_from_format("%s/%s", tabla,
							entry->d_name);
					printf("Abriendo file %s\n", pathFile);
					t_config *config_file = cargarConfiguracion(pathFile,
							logger);

					int size = config_get_int_value(config_file, "SIZE");

					int cantBloques = calcularBloques(size);
					char **bloques = malloc(sizeof(int) * cantBloques);
					bloques = config_get_array_value(config_file, "BLOCKS");

					for (int i = 0; cantBloques > i; i++) {
						char *bloque = string_from_format("%s/%s.bin",
								rutaBloques, bloques[i]);
						//abrirArchivo
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
							list_add(registrosBin, registro);
							for (int i = 0; cantElementos > i; i++) {
								free(elementos[i]);
							}
						}
						fclose(archivo);
					}
				}
			}
		}
		return registrosBin;
	}
	return NULL;
}



