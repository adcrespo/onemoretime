/*
 * Conexion.c
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */
#include "Conexion.h"
#include "Configuracion.h"
#include "LFS.h"
#include "Error.h"
#include "errno.h"

void *listen_connexions(){

	int socket_lfs = definirSocket(logger);
	if(bindearSocketYEscuchar(socket_lfs,"127.0.0.1",atoi(lfs_conf.puerto),logger)<= 0)
		_exit_with_error("BIND",NULL);

	fd_set set_master, set_copia;
	FD_ZERO(&set_master);
	FD_SET(socket_lfs, &set_master);
	int descriptor_mas_alto = socket_lfs;

	while (true) {
		set_copia = set_master;
		int i = select(descriptor_mas_alto + 1, &set_copia, NULL, NULL, NULL);
		if (i == -1 && errno != EINTR)
			_exit_with_error("SELECT",NULL);

		int n_descriptor = 0;
		while (n_descriptor <= descriptor_mas_alto) {
			if (FD_ISSET(n_descriptor,&set_copia)) {
				//ACEPTAR CONXIONES
				if (n_descriptor == socket_lfs) {
					aceptar(socket_lfs, &descriptor_mas_alto, &set_master);
				}
				//PROCESAR MENSAJE
				else {
					procesar(n_descriptor,&set_master);
				}
			}
			n_descriptor++;
		}
	}

	//return socket_lfs;
}

void aceptar(int socket_lfs, int* descriptor_mas_alto, fd_set* set_master) {
	int client_socket;
	if ((client_socket = aceptarConexiones(socket_lfs,logger)) == -1) {
		loggear(logger,LOG_LEVEL_ERROR,"Error en el accept");
		return;
	}
	FD_SET(client_socket, set_master);
	if (client_socket > *descriptor_mas_alto) {
		*descriptor_mas_alto = client_socket;
	}
}

void procesar(int n_descriptor, fd_set* set_master) {
	t_mensaje* msg;

	loggear(logger, LOG_LEVEL_INFO, "Recibiendo mensaje...");

	if((msg = recibirMensaje(n_descriptor, logger))== NULL) {
			close(n_descriptor);
			FD_CLR(n_descriptor, set_master);
			destruirMensaje(msg);
			return;
		}

		loggear(logger, LOG_LEVEL_INFO, "Proceso: %d",msg->header.tipoProceso);
		loggear(logger, LOG_LEVEL_INFO, "Mensaje: %d",msg->header.tipoMensaje);

		switch(msg->header.tipoProceso) {
			case mem:;
			switch(msg->header.tipoMensaje){

				case handshake:
					loggear(logger, LOG_LEVEL_INFO, "Handshake.");
					enviarMensaje(socket_lfs, handshake, 0, NULL, n_descriptor, logger, mem);
					break;
			}
			destruirMensaje(msg);
			break;
			default:;
		}

}
