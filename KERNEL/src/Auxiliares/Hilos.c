/*
 * Hilos.c
 *
 *  Created on: 17 jul. 2019
 *      Author: utnso
 */

#include "Hilos.h"

void crear_hilo_consola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "THREAD|No se pudo generar el hilo para la consola.");
	}
	log_info(logger, "THREAD|Se generó el hilo para la consola.");
}

void init_gossiping() {

	log_info(logger, "GOSSIPING|Iniciando lista seeds.");
	crearListaSeedsStruct(kernelGoss, kernel_conf.ip, kernel_conf.puerto, 1000, kernel_conf.ip_memoria, kernel_conf.puerto_memoria, logger, LISTA_CONN);
}

void *hiloGossiping()
{
	int tiempo = 5000;
	sleep (tiempo/1000);

	while (1)
	{
		loggear(logger,LOG_LEVEL_INFO,"GOSSIPING|Inicio Gossiping");
		processGossipingStruct(logger,LISTA_CONN, kernelGoss);
		loggear(logger,LOG_LEVEL_INFO,"GOSSIPING|Fin Gossiping");

		sleep (tiempo/1000);
	}
}

void crear_hilo_gossiping() {

	int hilo_gossiping = pthread_create(&thread_gossiping, NULL, hiloGossiping, NULL);
	if (hilo_gossiping == -1) {
		log_error(logger, "THREAD|No se pudo generar el hilo para el gossiping.");
	}
	log_info(logger, "THREAD|Se generó el hilo para el gossiping.");
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

int crear_hilo_inotify() {
	sigset_t set;
//	int s;
	int hilo_inotify;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
//	int s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	/*if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",
		NULL);*/

	hilo_inotify = pthread_create(&inotify, NULL, crearInotify, (void *) &set);

	if (hilo_inotify == -1) {
		log_error(logger, "THREAD|No se pudo generar el hilo para el I-NOTIFY.");
	}
	log_info(logger, "THREAD|Se generó el hilo para el I-NOTIFY.");

	return 1;
}

void crear_hilo_refresh() {

	int hiloDump = pthread_create(&thread_refresh, NULL, inicializar_refresh,
			NULL);
	if (hiloDump == -1) {
		log_error(logger, "THREAD|No se pudo generar el hilo para refresh");
	}
	log_info(logger, "THREAD|Se generó el hilo para refresh");
}

void *inicializar_refresh() {
	//actualizo metadata una vez al principio
//	actualizar_metadata();  // Se rompe por no tener memorias conectadas.
	//luego por hilo
	while (1) {
		aplicar_tiempo_refresh();
		limpiar_metadata();
		if (hay_memorias_disponibles()) {
			log_info(logger, "REFRESH| HAY memoria disponible");
			actualizar_metadata();
		} else {
			log_info(logger, "REFRESH| NO Hay memoria disponible");
		}
	}

}

void crear_hilo_planificador() {

	int hilo_planificador = pthread_create(&thread_planificacion, NULL, planificar, NULL);
	if (hilo_planificador == -1) {
		log_error(logger, "THREAD|No se pudo generar el hilo para el planificador.");
	}
	log_info(logger, "THREAD|Se generó el hilo para el planificador.");
}


//busco si ya tengo metadata para esa tabla
t_metadata* validar_metadata(char *nombre){
	bool findMd(void* element) {
			t_metadata *metadata = element;
			return string_equals_ignore_case(nombre, metadata->nombreTabla);
		}
	return list_find(lista_metadata, &findMd);
}

int hay_memorias_disponibles() {

	int memoria_conectada = 0;

	for(int i = 0; i < LISTA_CONN->elements_count; i++) {
		t_tipoSeeds* mem = list_get(LISTA_CONN, i);

		if(string_equals_ignore_case(&mem->estado,"1")) {
			memoria_conectada = 1;
		}
	}

	return LISTA_CONN->elements_count > 0 && memoria_conectada > 0;
}



