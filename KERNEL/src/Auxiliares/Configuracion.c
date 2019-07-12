/*
 * Configuracion.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#include "Configuracion.h"

void cargar_configuracion_kernel(char * path_config) {

//	config = cargar_configuracion("../config/KERNEL.config", logger);
	config = cargarConfiguracion(path_config,logger);

	if (config_has_property(config, "IP")) {
			kernel_conf.ip = config_get_string_value(config, "IP");
			remove_quotes(kernel_conf.ip);
		} else {
			loguear_error_carga_propiedad("IP");
		}

		if (config_has_property(config, "PUERTO")) {
			kernel_conf.puerto = config_get_string_value(config, "PUERTO");
		} else {
			loguear_error_carga_propiedad("PUERTO");
		}

		if (config_has_property(config, "IP_MEMORIA")) {
			char* arr = config_get_string_value(config, "IP_MEMORIA");
			puts(arr);
			kernel_conf.ip_memoria = string_get_string_as_array(arr);
		} else {
			loguear_error_carga_propiedad("IP_MEMORIA");
		}

		if (config_has_property(config, "PUERTO_MEMORIA")) {
			char* arr2 = config_get_string_value(config, "PUERTO_MEMORIA");
			puts(arr2);
			kernel_conf.puerto_memoria = string_get_string_as_array(arr2);
			puts("cargado ip memoria");
		} else {
			loguear_error_carga_propiedad("PUERTO_MEMORIA");
		}

		if (config_has_property(config, "QUANTUM")) {
			kernel_conf.quantum = config_get_int_value(config, "QUANTUM");
	//			char* valor = string_new();
	//			valor = string_itoa(kernel_conf.quantum);
			loguear_carga_propiedad("QUANTUM", string_itoa(kernel_conf.quantum));
		} else {
			loguear_error_carga_propiedad("QUANTUM");
		}

		if (config_has_property(config, "MULTIPROCESAMIENTO")) {
			kernel_conf.multiprocesamiento = config_get_int_value(config,
					"MULTIPROCESAMIENTO");
	//			char* valor = string_new();
	//			valor = string_itoa(kernel_conf.multiprocesamiento);
			loguear_carga_propiedad("MULTIPROCESAMIENTO",
					string_itoa(kernel_conf.multiprocesamiento));
		} else {
			loguear_error_carga_propiedad("MULTIPROCESAMIENTO");
		}

		if (config_has_property(config, "METADATA_REFRESH")) {
			kernel_conf.metadata_refresh = config_get_int_value(config,
					"METADATA_REFRESH");
	//			char* valor = string_new();
	//			valor = string_itoa(kernel_conf.metadata_refresh);
			loguear_carga_propiedad("METADATA_REFRESH",
					string_itoa(kernel_conf.metadata_refresh));
		} else {
			loguear_error_carga_propiedad("METADATA_REFRESH");
		}

		if (config_has_property(config, "SLEEP_EJECUCION")) {
			kernel_conf.sleep_ejecucion = config_get_int_value(config,
					"SLEEP_EJECUCION");
	//			char* valor = string_new();
	//			valor = string_itoa(kernel_conf.sleep_ejecucion);
			loguear_carga_propiedad("SLEEP_EJECUCION",
					string_itoa(kernel_conf.sleep_ejecucion));
		} else {
			loguear_error_carga_propiedad("SLEEP_EJECUCION");
		}

		// No hace falta
		mostrar_propiedades();
}

// Auxiliares
void mostrar_propiedades() {

	//	printf("IP Memoria: %s\n", kernel_conf.ip_memoria);
	//	printf("Puerto Memoria: %d\n", kernel_conf.puerto_memoria);
		printf("Quantum: %d\n", kernel_conf.quantum);
		printf("Multiprocesamiento: %d\n", kernel_conf.multiprocesamiento);
		printf("Metadata Refresh: %d\n", kernel_conf.metadata_refresh);
		printf("Sleep Ejecución: %d\n", kernel_conf.sleep_ejecucion);

		int i=0;
		while(kernel_conf.ip_memoria[i] != NULL)	{

			remove_quotes(kernel_conf.ip_memoria[i]);
			loggear(logger,LOG_LEVEL_INFO,"IP_SEEDS: %s", kernel_conf.ip_memoria[i]);
			loggear(logger,LOG_LEVEL_INFO,"PUERTOS_SEEDS: %s", kernel_conf.puerto_memoria[i]);
			i++;
		}
}
