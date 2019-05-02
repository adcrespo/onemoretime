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

	crear_hilo_filesystem();

	pthread_join(thread_filesystem, NULL);


	return EXIT_SUCCESS;
}


void crear_hilo_filesystem() {

	int hilo_filesystem = pthread_create(&thread_filesystem, NULL, crear_filesystem, NULL);
	if (hilo_filesystem == -1) {
		log_error(logger, "No se pudo generar el hilo para el fs.");
	}
	log_info(logger, "Se generó el hilo para el fs.");
}



