/*
 * LISSANDRA.h
 *
 *  Created on: 29 abr. 2019
 *      Author: utnso
 */

#ifndef LFS_H_
#define LFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <shared.h>

t_log *logger;
t_config *config;

typedef struct {
	int puerto;
	char* punto_montaje;
	int retardo;
	int tamano_value;
	int tiempo_dump;
} t_lis_config;


// Variables globales
t_config *config;
t_lis_config lis_conf;

void cargar_conf_lis();

void cargar_conf_lis(){
	config = cargarConfiguracion("../config/Lissandra.config", logger);

		if (config_has_property(config, "PUERTO"))
				lis_conf.puerto = config_get_int_value(config, "PUERTO");
				printf("Puerto: %d", lis_conf.puerto);

}

#endif /* LFS_H_ */
