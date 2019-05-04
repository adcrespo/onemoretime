/*
 * Config.h
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include "LFS.h"
#include <shared.h>

typedef struct {
	char* ip;
	int puerto;
	char* punto_montaje;
	int retardo;
	int tamano_value;
	int tiempo_dump;
} t_lfs_config;


t_lfs_config lfs_conf;
t_config *config;

void cargar_conf_lfs();


#endif /* CONFIGURACION_H_ */
