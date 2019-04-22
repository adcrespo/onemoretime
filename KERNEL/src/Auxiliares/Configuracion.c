/*
 * Configuracion.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#include "Configuracion.h"

void cargar_configuracion_kernel() {

//	config = cargar_configuracion("../config/KERNEL.config", logger);
	config = cargarConfiguracion("../config/KERNEL.config", logger);

	if (config_has_property(config, "IP_MEMORIA")) {
			kernel_conf.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
			remove_quotes(kernel_conf.ip_memoria);
			loguear_carga_propiedad("IP_MEMORIA", kernel_conf.ip_memoria);
		} else {
			loguear_error_carga_propiedad("IP_MEMORIA");
		}

		if (config_has_property(config, "PUERTO_MEMORIA")) {
			kernel_conf.puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
			loguear_carga_propiedad("PUERTO_MEMORIA", string_itoa(kernel_conf.puerto_memoria));
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
			kernel_conf.multiprocesamiento = config_get_int_value(config, "MULTIPROCESAMIENTO");
//			char* valor = string_new();
//			valor = string_itoa(kernel_conf.multiprocesamiento);
			loguear_carga_propiedad("MULTIPROCESAMIENTO", string_itoa(kernel_conf.multiprocesamiento));
		} else {
			loguear_error_carga_propiedad("MULTIPROCESAMIENTO");
		}

		if (config_has_property(config, "METADATA_REFRESH")) {
			kernel_conf.metadata_refresh = config_get_int_value(config, "METADATA_REFRESH");
//			char* valor = string_new();
//			valor = string_itoa(kernel_conf.metadata_refresh);
			loguear_carga_propiedad("METADATA_REFRESH", string_itoa(kernel_conf.metadata_refresh));
		} else {
			loguear_error_carga_propiedad("METADATA_REFRESH");
		}

		if (config_has_property(config, "SLEEP_EJECUCION")) {
			kernel_conf.sleep_ejecucion = config_get_int_value(config, "SLEEP_EJECUCION");
//			char* valor = string_new();
//			valor = string_itoa(kernel_conf.sleep_ejecucion);
			loguear_carga_propiedad("SLEEP_EJECUCION", string_itoa(kernel_conf.sleep_ejecucion));
		} else {
			loguear_error_carga_propiedad("SLEEP_EJECUCION");
		}

		// No hace falta
		mostrar_propiedades();
}

void mostrar_propiedades() {

	printf("IP Memoria: %s\n", kernel_conf.ip_memoria);
	printf("Puerto Memoria: %d\n", kernel_conf.puerto_memoria);
	printf("Quantum: %d\n", kernel_conf.quantum);
	printf("Multiprocesamiento: %d\n", kernel_conf.multiprocesamiento);
	printf("Metadata Refresh: %d\n", kernel_conf.metadata_refresh);
	printf("Sleep Ejecución: %d\n", kernel_conf.sleep_ejecucion);
}
