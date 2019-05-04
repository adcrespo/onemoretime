/*
 * gossiping.h
 *
 *  Created on: 24 abr. 2019
 *      Author: utnso
 */

#ifndef GOSSIPING_H_
#define GOSSIPING_H_

#include <signal.h>
#include <shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>

int crearCliente();

pthread_t cliente;

t_list *LISTA_CONN;
t_list *LISTA_CONN_PORT;

#endif /* GOSSIPING_H_ */
