/*
 * Filesystem.c
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#include "Filesystem.h"

void *CrearFileSystem(){
	printf("Creando filesystem...\n");

	ruta_tables = string_new();
	string_append(&ruta_tables, lfs_conf.punto_montaje);
	string_append(&ruta_tables, "Tables/");
	printf("ruta tablas: %s\n", ruta_tables);
	cargar_metadata();
	cargar_bitmap();

	return (void*)1;
}

void cargar_metadata(){
	ruta_metadata = string_new();
	string_append(&ruta_metadata, lfs_conf.punto_montaje);
	string_append(&ruta_metadata, "Metadata/");
	string_append(&ruta_metadata, "Metadata.bin");
	printf("Ruta Metadata: %s \n", ruta_metadata);

	/*config_metadata = cargarConfiguracion(ruta_metadata, logger);

			if(config_has_property(config_metadata,"BLOCK_SIZE")) {
				tamanio_bloques= config_get_int_value(config_metadata, "BLOCK_SIZE");
				loggear(logger, LOG_LEVEL_INFO, "Tamanio de bloques: %d", tamanio_bloques);
			}

			if(config_has_property(config_metadata,"BLOCKS")) {
				cantidad_bloques= config_get_int_value(config_metadata, "BLOCKS");
				loggear(logger, LOG_LEVEL_INFO, "Cantidad de bloques: %d", cantidad_bloques);
						}

	config_destroy(config_metadata);*/

}

void cargar_bitmap(){
	ruta_bitmap = string_new();
	string_append(&ruta_bitmap, lfs_conf.punto_montaje);
	string_append(&ruta_bitmap, "Metadata/");
	string_append(&ruta_bitmap, "Bitmap.bin");
	printf("Ruta bitmap: %s \n", ruta_bitmap);

	/*int bm = open(ruta_bitmap, O_RDWR);
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


	char *ruta_tabla = string_from_format("%s%s", ruta_tables, tabla);
	log_info(logger, "Ruta tabla: %s", ruta_tabla);
	FILE *fp = fopen(ruta_tabla, "r");
	free(ruta_tabla);

	if(fp)
	{
		fclose(fp);
		loggear(logger, LOG_LEVEL_WARNING, "La tabla %s ya existe", tabla);
		return 1;
	}else
	{
		return 0;
	}
}

int ObtenerMetadata(char *tabla)
{
	loggear(logger, LOG_LEVEL_INFO, "Obteniendo metadata de tabla: %s", tabla);

	char *rutaMetadata = string_from_format("%s%s/Metadata", ruta_tables, tabla);

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
	char *rutaTabla = string_from_format("%s%s", ruta_tables, tabla);


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
	char *rutaMetadataTabla = string_from_format("%s%s/Metadata", ruta_tables, tabla);

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

