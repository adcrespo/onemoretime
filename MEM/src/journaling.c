/*
 * journaling.c
 *
 *  Created on: 12 may. 2019
 *      Author: utnso
 */

#include "MEM.h"

#include "connection.h"
#include "file_conf.h"
#include "error.h"
#include "memory.h"

#include "journaling.h"

void process_journaling(){
	if(pthread_mutex_trylock(&journalingMutex) != 0){
		loggear(logger,LOG_LEVEL_ERROR,"ERR JOURNALING...");
		loggear(logger,LOG_LEVEL_INFO,"JOURNALING en ejecución!");
		return;
	}

	loggear(logger,LOG_LEVEL_INFO,"Init JOURNALING...");
	int i,j;
	for (i = 0; adm_spa_lista!=NULL && i < list_size(adm_spa_lista); i++) {
		t_adm_tabla_segmentos_spa* adm_table = list_get(adm_spa_lista, i);
		loggear(logger,LOG_LEVEL_INFO,"Analizando Tabla: %s", adm_table->path_tabla);
		t_segmentos_spa* adm_table_seg = list_get(adm_table->seg_lista, 0);
		for (j = 0; adm_table_seg!=NULL && j < list_size(adm_table_seg->pag_lista); j++) {
			t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista,j);
			loggear(logger,LOG_LEVEL_INFO,"Pagina: %d, Frame: %d, Modificado: %d, TS: %d",
					j, adm_table_pag->frame, adm_table_pag->modificado, adm_table_pag->timestamp);
			if(adm_table_pag->modificado==1) {
				loggear(logger,LOG_LEVEL_INFO,"Enviando INSERT");
				//TODO: enviar INSERT
			}
		}
	}
	loggear(logger,LOG_LEVEL_INFO,"End JOURNALING...");
	pthread_mutex_unlock (&journalingMutex);
	return;
}

void *crearJournaling() {

	sleep( MEM_CONF.RETARDO_JOURNAL/1000);
	while (1) {
		loggear(logger,LOG_LEVEL_INFO,"Init Automatic-JOURNALING...");
		process_journaling();
		loggear(logger,LOG_LEVEL_INFO,"End Automatic-JOURNALING");
		sleep( MEM_CONF.RETARDO_JOURNAL/1000);
	}
}

int crearHiloJournaling()
{
	pthread_mutex_init (&journalingMutex, NULL);

	sigset_t set;
	int s;
	int hilo_journaling;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_journaling = pthread_create(&journaling, NULL, crearJournaling, (void *) &set);

	if (hilo_journaling == -1) {
			log_error(logger, "No se pudo generar el hilo para la consola.");
		}
		log_info(logger, "Se generó el hilo para la consola.");

	return 1;
}
