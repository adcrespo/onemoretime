/*
 * gossipingMemoria.h
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#ifndef GOSSIPINGMEMORIA_H_
#define GOSSIPINGMEMORIA_H_


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

#define NUM_CONEX 100
#define DESCONECTADO 0
#define CONECTADO 1

pthread_t cliente;
//pthread_mutex_t mutexGossiping;
//pthread_mutex_t mutexprocessGossiping;
//t_list *LISTA_CONN;
//t_list *LISTA_CONECTADOS;
//char BITMAP_CONN_STATUS[NUM_CONEX];

void *hiloGossipingMemoria();

int crearHiloGossipingMemoria();

#endif /* GOSSIPINGMEMORIA_H_ */
