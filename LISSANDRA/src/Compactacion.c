/*
 * Compactacion.c
 *
 *  Created on: 15 jun. 2019
 *      Author: utnso
 */
#include "LFS.h"

#include "Compactacion.h"

void *crearCompactacion(int retardo) {

	sleep( retardo/1000);
	while (1) {
		//process_journaling();
		sleep( retardo/1000);
	}
}

int crearHiloGossiping()
{
	sigset_t set;
	int s;
	int hilo_compactacion;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		return -1;
		//_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_compactacion = pthread_create(&compactacion, NULL, crearCompactacion, (void *) &set);

	if (hilo_compactacion == -1)
		loggear(logger,LOG_LEVEL_INFO,"ERROR_HILO_COMPACTACION: %d", hilo_compactacion);
	log_info(logger, "Se generó el hilo para la COMPACTACION.");

	return 1;
}
