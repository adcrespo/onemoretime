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

int listen_connexions(char* PUERTO){

	int socket_lfs = definirSocket(logger);
	if(bindearSocketYEscuchar(socket_lfs,"127.0.0.1",atoi(PUERTO),logger)<= 0)
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

	return socket_lfs;
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


	loggear(logger, LOG_LEVEL_INFO, "Recibiendo mensaje...");
}
