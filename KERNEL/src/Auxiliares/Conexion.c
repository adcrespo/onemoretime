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
	//enviar_journal_shc();
	//enviar_journal_ev();
}

void enviar_journal_sc() {
	if(memoria_sc == NULL) return;
//	t_tipoSeeds *memoria;
//	memoria = obtener_memoria_lista(memoria_sc->numeroMemoria);
	if (memoria_sc != NULL) {
		enviar_mensaje_journal(memoria_sc);
	} else {
		log_info(logger, "Memoria criterio SC no encontrada");
	}

}

void enviar_journal_shc() {
	int size_shc = list_size(lista_criterio_shc);
	t_tipoSeeds *memoria;
	for (int i = 0; i < size_shc; i++) {
		memoria = list_get(lista_criterio_shc, i);
		enviar_mensaje_journal(memoria);

	}
}

void enviar_journal_ev() {
	int size_ev = list_size(lista_criterio_ev);
	t_tipoSeeds *memoria;
	for (int i = 0; i < size_ev; i++) {
		memoria = list_get(lista_criterio_ev, i);
		enviar_mensaje_journal(memoria);

	}
}

void enviar_mensaje_journal(t_tipoSeeds *memoria) {
	int puerto = atoi(memoria->puerto);
	int client_socket = conectar_a_servidor(memoria->ip, puerto, kernel);

	if (socket > 0) {
		enviarMensaje(kernel, journal, 0, NULL, client_socket, logger, mem);
		close(client_socket);
	}
}

int conectar_a_memoria(t_tipoSeeds* memoria) {

	log_info(logger, "Conectando a Memoria: %d", memoria->numeroMemoria);
	int puerto = atoi(memoria->puerto);
	return conectar_a_servidor(memoria->ip, puerto, mem);
}
