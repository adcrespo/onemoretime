/*
 * Filesystem.h
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "LFS.h"

void *crear_filesystem();
void cargar_metadata();
void cargar_bitmap();

t_config *config_metadata;
t_bitarray *bitmap;

int tamanio_bloques;
int cantidad_bloques;
char *ruta_metadata;
char *ruta_bitmap;



#endif /* FILESYSTEM_H_ */
