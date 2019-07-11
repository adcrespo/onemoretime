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

int main(int argc, char *argv[]) {

	initArgumentos(argc, argv);

	/* 1. Creación de logger */
	//logger = configurar_logger("../log/KERNEL.log", "Kernel");
	logger = configurar_logger_verbose("../log/KERNEL.log", "MEM", string_equals_ignore_case(args_verbose,"true")?true:false);

	/* 2. Carga de configuración desde archivo */
	cargar_configuracion_kernel(string_equals_ignore_case(args_configfile,"false")?
			"/home/utnso/Repositorios/tp-2019-1c-One-more-time/KERNEL/config/KERNEL.config":
			args_configfile);

	/* 3. Inicializar variables */
	inicializar();

	/* 4. Conexión con Memoria */
	socket_memoria = conectar_a_servidor(kernel_conf.ip_memoria, kernel_conf.puerto_memoria, mem);

	/* 5. Creación de hilo para consola */
	crear_hilo_consola();

	/* 6. Gossiping */
	init_gossiping();


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

void init_gossiping() {

//	crearListaSeeds(kernel_conf.MEM_CONF.IP,MEM_CONF.PUERTO,MEM_CONF.IP_SEEDS,MEM_CONF.PUERTO_SEEDS,logger);
}
