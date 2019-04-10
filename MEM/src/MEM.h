/*
 * FM9.h
 *
 *  Created on: 20 sep. 2018
 *      Author: utnso
 */

#ifndef MEM_H_
#define MEM_H_

#include <shared.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <sys/select.h>
#include <readline/readline.h> // Para usar readline
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

t_log* logger;

int recvMensaje(int client_socket, t_mensaje** msg);
int sendMensaje(int client_socket);
void aceptar(int socket_fm9, int* descriptor_mas_alto, fd_set* set_master);
void procesar(int n_descriptor, fd_set* set_master);
char* intToChar4(int num);
#endif /* MEM_H_ */
