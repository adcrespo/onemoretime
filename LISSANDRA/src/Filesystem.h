/*
 * Filesystem.h
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "LFS.h"

void *CrearFileSystem();
void cargar_metadata();
void cargar_bitmap();
int ExisteTabla(const char*tabla);
int ObtenerMetadata(char *tabla);
int CalcularParticion(int clave, int particiones);
void CrearDirectorioTabla(char *tabla);
void CrearMetadataTabla(char *tabla, char *consistencia, int particiones, int tiempoCompactacion);

t_config *config_metadata;
t_bitarray *bitmap;

int tamanio_bloques;
int cantidad_bloques;
char *ruta_metadata;
char *ruta_bitmap;
char *ruta_tables;
char *bmap;



#endif /* FILESYSTEM_H_ */
