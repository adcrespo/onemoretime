/*
 * Conexion.c
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */
#include "Conexion.h"
#include "Configuracion.h"

int levantar_servidor() {

	socket_lfs = definirSocket(logger);
	char* ip = lfs_conf.ip;
	int puerto = lfs_conf.puerto;
	int bind = bindearSocketYEscuchar(socket_lfs, ip, puerto, logger);

	if (bind <= 0) {
		return EXIT_FAILURE;
	}
