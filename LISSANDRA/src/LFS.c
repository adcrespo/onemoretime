/*
 ============================================================================
 Name        : LISSANDRA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "LFS.h"

int main(void) {

	logger = configurar_logger("../log/LIS.log", "Lissandra");

	cargar_conf_lfs();

	crear_hilo_consola();
	crear_filesystem();
	crear_hilo_conexiones();

	pthread_join(thread_consola, NULL);


	return EXIT_SUCCESS;
}




void crear_hilo_consola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola");
	}
	log_info(logger, "Se generó el hilo para la consola");
}

void crear_hilo_conexiones() {

	int hilo_conexiones = pthread_create(&thread_conexiones, NULL, listen_connexions, NULL);
		if (hilo_conexiones == -1) {
			log_error(logger, "No se pudo generar el hilo para las conexiones");
		}
		log_info(logger, "Se generó el hilo para las conexiones");
}
