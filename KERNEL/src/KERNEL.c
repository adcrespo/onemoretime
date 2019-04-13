/*
 ============================================================================
 Name        : KERNEL.c
 Author      : Osquic
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "KERNEL.h"

int main(void) {

	/* 1. Creación de logger */
	logger = configurar_logger("../log/KERNEL.log", logger);

	/* 2. Carga de configuración desde archivo */

	/* 3. Inicializar variables */

	/* 4. Creación de hilo para consola */
	crear_hilo_consola();

	puts("Dale que vaaaa"); /* prints Dale que vaaaa */
	return EXIT_SUCCESS;
}

void crear_consola() {

}

void crear_hilo_consola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola.");
	}
	log_info(logger, "Se generó el hilo para la consola.");
}
