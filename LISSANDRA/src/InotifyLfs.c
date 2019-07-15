/*
 * InotifyLissandra.c
 *
 *  Created on: 14 jul. 2019
 *      Author: utnso
 */
#include "InotifyLfs.h"

void *crearInotify() {
	int inotifyFd = inotifyInit("../config/Lissandra.config");

	if (inotifyFd == -1)
		_exit_with_error("inotify_init", NULL);

	while (1) {/* Read events forever */
		if (inotifyEvent(inotifyFd))
			get_modify_config("../config/Lissandra.config");
	}
}

int crearHiloInotify() {
	sigset_t set;
	int s;
	int hilo_inotify;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",
		NULL);

	hilo_inotify = pthread_create(&inotify, NULL, crearInotify, (void *) &set);

	if (hilo_inotify == -1) {
		log_error(logger, "No se pudo generar el hilo para el I-NOTIFY.");
	}
	log_info(logger, "Se generó el hilo para el I-NOTIFY.");

	return 1;
}
