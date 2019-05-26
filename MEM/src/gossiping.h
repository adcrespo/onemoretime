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
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

char* getLocalIp();

int loggearElementosLista(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT);

int procesarMsjGossiping(char *mensaje, char *primerParser, char *segundoParser);

int crearListaSeeds();

int crearHiloGossiping();

pthread_t cliente;

t_list *LISTA_CONN;
t_list *LISTA_CONN_PORT;


typedef enum tipoServidor {
	memoria,gossiping
} t_tipoServidor;

pthread_mutex_t mutexGossiping;
pthread_mutex_t mutexprocessGossiping;

#endif /* GOSSIPING_H_ */
