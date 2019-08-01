/*
 * Conexion.c
 *
 *  Created on: 22 abr. 2019
 *      Author: utnso
 */

#include "Conexion.h"

int conectar_a_servidor(char* ip, int puerto, int proceso) {

	int socket;

	if((socket = definirSocket(logger))<= 0)
		log_error(logger, "No se pudo definir socket.");

	// sacar esto
	log_info(logger, "Socket creado: %d", socket);
	log_info(logger, "ip: %s", ip);
	log_info(logger, "puerto: %d", puerto);

	if(conectarseAServidor(socket, ip, puerto, logger)<=0)
		log_error(logger, "No se pudo conectar a servidor.");

	loggear(logger,LOG_LEVEL_INFO, "INICIO Handshake(%d)...", proceso);
	enviarMensaje(kernel, handshake, 0, NULL, socket, logger, proceso);
	t_mensaje* msg = recibirMensaje(socket, logger);
	destruirMensaje(msg);
	loggear(logger,LOG_LEVEL_INFO, "FIN Handshake(%d)", proceso);
	return socket;
}

void enviar_journal_memorias() {
	enviar_journal_sc();
	enviar_journal_shc();
	enviar_journal_ev();
}

void enviar_journal_sc() {
	if(memoria_sc == NULL) return;
//	t_tipoSeeds *memoria;
//	memoria = obtener_memoria_lista(memoria_sc->numeroMemoria);
	if (memoria_sc != NULL) {
		//TODO: MUTEX
		pthread_mutex_lock(&mutex_memoria_sc);
		enviar_mensaje_journal(memoria_sc);
		pthread_mutex_lock(&mutex_memoria_sc);
	} else {
		log_info(logger, "Memoria criterio SC no encontrada");
	}

}

void enviar_journal_shc() {
	pthread_mutex_lock(&mutex_memoria_shc);
	int size_shc = list_size(lista_criterio_shc);
//	pthread_mutex_unlock(&mutex_memoria_shc);
	t_tipoSeeds *memoria;
	for (int i = 0; i < size_shc; i++) {
//		pthread_mutex_lock(&mutex_memoria_shc);
		memoria = list_get(lista_criterio_shc, i);
//		pthread_mutex_unlock(&mutex_memoria_shc);
		enviar_mensaje_journal(memoria);
	}
	pthread_mutex_unlock(&mutex_memoria_shc);
}

void enviar_journal_ev() {
	pthread_mutex_unlock(&mutex_memoria_ev);
	int size_ev = list_size(lista_criterio_ev);
//	pthread_mutex_unlock(&mutex_memoria_ev);
	t_tipoSeeds *memoria;
	for (int i = 0; i < size_ev; i++) {
//		pthread_mutex_unlock(&mutex_memoria_ev);
		memoria = list_get(lista_criterio_ev, i);
//		pthread_mutex_unlock(&mutex_memoria_ev);
		enviar_mensaje_journal(memoria);

	}
	pthread_mutex_unlock(&mutex_memoria_ev);
}

void enviar_mensaje_journal(t_tipoSeeds *memoria) {
	int puerto = atoi(memoria->puerto);
	int client_socket = conectar_a_servidor(memoria->ip, puerto, kernel);

	if (socket > 0) {
		enviarMensaje(kernel, journal, 0, NULL, client_socket, logger, mem);
		//TODO: RECIBIR MSJ
		t_mensaje* mensaje = recibirMensaje(client_socket, logger);
		if(mensaje == NULL) {
			loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje");
		}
		int insert_error = mensaje->header.error;
		destruirMensaje(mensaje);
		if(insert_error != 0) {
			loggear(logger,LOG_LEVEL_ERROR,"No se pudo insertar en lis correctamente");
		}

		close(client_socket);
	}
}

int conectar_a_memoria(t_tipoSeeds* memoria) {

	log_info(logger, "Conectando a Memoria: %d", memoria->numeroMemoria);
	int puerto = atoi(memoria->puerto);
	return conectar_a_servidor(memoria->ip, puerto, mem);
}
