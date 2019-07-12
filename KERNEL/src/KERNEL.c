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
	crear_hilo_gossiping();

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

	LISTA_CONN = list_create();
//	char* lista_ips = string_new();
//	char* lista_puertos = string_new();

//	puts("Metiendo memoria en lista");
//	string_append(&lista_ips, kernel_conf.ip_memoria);
//	puts("listo el primero");
//	string_append(&lista_puertos, string_itoa(kernel_conf.puerto_memoria));
//	puts("Metido en lista");
}

void crear_hilo_consola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola.");
	}
	log_info(logger, "Se generó el hilo para la consola.");
}

void init_gossiping() {

	puts("Creando lista seeds struct");
	crearListaSeedsStruct(gossiping, kernel_conf.ip, kernel_conf.puerto, 1000, kernel_conf.ip_memoria, kernel_conf.puerto_memoria, logger, LISTA_CONN);
}

void *hiloGossiping()
{
	int tiempo = 30000;
//	sleep (kernel_conf./1000);
	sleep (tiempo/1000);

	while (1)
	{
		loggear(logger,LOG_LEVEL_INFO,"INIT_GOSSIPING");
		processGossipingStruct(logger,LISTA_CONN);
		loggear(logger,LOG_LEVEL_INFO,"END_GOSSIPING");

//		sleep (MEM_CONF.RETARDO_GOSSIPING/1000);
		sleep (tiempo/1000);
	}
}

void crear_hilo_gossiping() {

//	int hilo_gossiping = pthread_create(&thread_gossiping, NULL, crear_consola, NULL);
	int hilo_gossiping = pthread_create(&thread_gossiping, NULL, hiloGossiping, NULL);
	if (hilo_gossiping == -1) {
		log_error(logger, "No se pudo generar el hilo para el gossiping.");
	}
	log_info(logger, "Se generó el hilo para el gossiping.");
}
