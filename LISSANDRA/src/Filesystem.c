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
	//CargarMetadata();
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

int ObtenerMetadata(char *tabla)
{
	loggear(logger, LOG_LEVEL_INFO, "Obteniendo metadata de tabla: %s", tabla);

	char *rutaMetadata = string_from_format("%s%s/Metadata", rutaTablas, tabla);

	t_config *metadataFile = cargarConfiguracion(rutaMetadata, logger);

	int partitions = config_get_int_value(metadataFile, "PARTITIONS");

	free(rutaMetadata);
	config_destroy(metadataFile);

	return partitions;
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

void AlocarTabla(char *tabla)
{
	t_tabla *listaTabla = malloc(sizeof(t_tabla));
	char *nombre = string_new();
	string_append(&nombre, tabla);
	strcpy(listaTabla->nombre_tabla, nombre);
	listaTabla->lista = list_create();
	list_add(memtable, listaTabla);

}

void InsertarTabla(t_request *request)
{
	//char *nombre = string_new();
	//string_append(&nombre, request->parametro1);

	//Verifico existencia en el file system
	if(!ExisteTabla(request->parametro1))
	{
		loggear(logger, LOG_LEVEL_ERROR, "%s no existe en el file system", request->parametro1);;
	}

	//Obtengo metadata
	int particiones = ObtenerMetadata(request->parametro1);

	//Verifico si no tiene datos a dumpear
	t_tabla *tabla = malloc(sizeof(t_tabla));
	tabla = BuscarTablaMemtable(request->parametro1);

	if(tabla == NULL)
	{
		//Aloco en memtable como nueva tabla
		loggear(logger, LOG_LEVEL_INFO, "%s no posee datos a dumpear", request->parametro1);
		AlocarTabla(request->parametro1);
	}else
	{
		//Alocar en su posicion
		loggear(logger, LOG_LEVEL_INFO, "Alocando en su pos correspondiente");
	}


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

