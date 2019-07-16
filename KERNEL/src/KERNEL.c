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

	/* 1. Creación de logger */
	logger = configurar_logger("../log/KERNEL.log", "Kernel");

	/* 2. Carga de configuración desde archivo */
	cargar_configuracion_kernel("../config/KERNEL.config");

	/* 3. Inicializar variables */
	inicializar();

	/* 4. Conexión con Memoria */
//	socket_memoria = conectar_a_servidor(kernel_conf.ip_memoria, kernel_conf.puerto_memoria, mem);

	/* 5. Creación de hilo para consola */
	crear_hilo_consola();

	/* 6. Creación de hilo para inotify */
	crearHiloInotify();

	/* 7. Gossiping */
	init_gossiping();
	crear_hilo_gossiping();

	pthread_join(thread_consola, NULL);

	puts("Fin proceso Kernel"); /* prints Proceso Kernel */
	return EXIT_SUCCESS;
}

void inicializar() {

	cont_id_procesos = 0;
	memoria_sc = -1;
	lista_new = list_create();
	lista_ready = list_create();
	lista_exec = list_create();
	lista_exit = list_create();
	lista_criterio_shc = list_create();
	lista_criterio_ev = list_create();

	LISTA_CONN = list_create();
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
	crearListaSeedsStruct(kernelGoss, kernel_conf.ip, kernel_conf.puerto, 1000, kernel_conf.ip_memoria, kernel_conf.puerto_memoria, logger, LISTA_CONN);
//	crearListaSeedsStruct(gossiping, kernel_conf.ip, kernel_conf.puerto, 1000, kernel_conf.ip_memoria, kernel_conf.puerto_memoria, logger, LISTA_CONN);
}

void *hiloGossiping()
{
	int tiempo = 10000;
//	sleep (kernel_conf./1000);
	sleep (tiempo/1000);

	while (1)
	{
		loggear(logger,LOG_LEVEL_INFO,"INIT_GOSSIPING");
		processGossipingStruct(logger,LISTA_CONN, kernelGoss);
//		processGossipingStruct(logger,LISTA_CONN);
		loggear(logger,LOG_LEVEL_INFO,"END_GOSSIPING");

//		sleep (MEM_CONF.RETARDO_GOSSIPING/1000);
		sleep (tiempo/1000);
	}
}

void crear_hilo_gossiping() {

	int hilo_gossiping = pthread_create(&thread_gossiping, NULL, hiloGossiping, NULL);
	if (hilo_gossiping == -1) {
		log_error(logger, "No se pudo generar el hilo para el gossiping.");
	}
	log_info(logger, "Se generó el hilo para el gossiping.");
}

void *crearInotify() {
	int inotifyFd = inotifyInit("../config/KERNEL.config");

	/*if (inotifyFd == -1)
		_exit_with_error("inotify_init", NULL);*/

	while (1) {/* Read events forever */
		if (inotifyEvent(inotifyFd))
			cargar_configuracion_kernel_inotify("../config/KERNEL.config");
	}
}

int crearHiloInotify() {
	sigset_t set;
	int s;
	int hilo_inotify;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	/*if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",
		NULL);*/

	hilo_inotify = pthread_create(&inotify, NULL, crearInotify, (void *) &set);

	if (hilo_inotify == -1) {
		log_error(logger, "No se pudo generar el hilo para el I-NOTIFY.");
	}
	log_info(logger, "Se generó el hilo para el I-NOTIFY.");

	return 1;
}


