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
