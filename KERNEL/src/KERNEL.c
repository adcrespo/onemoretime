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
	logger = configurar_logger("../log/KERNEL.log", "Kernel");

	/* 2. Carga de configuración desde archivo */
	cargar_configuracion_kernel();

	/* 3. Inicializar variables */
	inicializar();

	/* 4. Conexión con Memoria */
	//socket_memoria = conectar_a_servidor(kernel_conf.ip_memoria, kernel_conf.puerto_memoria, mem);

	/* 5. Creación de hilo para consola */
	crear_hilo_consola();


	pthread_join(thread_consola, NULL);

	puts("Fin proceso Kernel"); /* prints Proceso Kernel */
	return EXIT_SUCCESS;
}

void inicializar() {

	cont_id_procesos = 0;

	lista_new = list_create();
	lista_ready = list_create();
	lista_exec = list_create();
	lista_exit = list_create();

}

void crear_hilo_consola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola.");
	}
	log_info(logger, "Se generó el hilo para la consola.");
}
