/*
 * gossiping.h
 *
 *  Created on: 8 jun. 2019
 *      Author: utnso
 */

#ifndef GOSSIPING_H_
#define GOSSIPING_H_

#include "shared.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <pthread.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define NUM_CONEX 100
#define DESCONECTADO 0
#define CONECTADO 1

pthread_t cliente;
pthread_mutex_t mutexGossiping;
pthread_mutex_t mutexprocessGossiping;
t_list *LISTA_CONECTADOS;
//t_list *LISTA_CONN;
//t_list *LISTA_CONN_PORT;
//t_list *LISTA_CONN_STATUS;
char BITMAP_CONN_STATUS[NUM_CONEX];

typedef struct tipoSeeds {
	char numeroMemoria[15];
	char ip[30];
	char puerto[15];
	char estado;
} t_tipoSeeds;

typedef enum tipoServidor {
	memoria,gossiping
} t_tipoServidor;

void processGossiping(t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT);

/*
 * Devuelve la lista de las MEMORIAS conectadas despues del GOSSIPING
 */
t_list *obtenerListaConectados(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT,t_log *logger);

/*
 * Arma la lista de seeds que envia por msj
 */
char *armarMensajeListaSEEDS(t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT);

/*
 * Actualiza la lista de seeds que recibe por msj
 */
int procesarMsjGossiping(char *mensaje, char *primerParser, char *segundoParser, t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT);

/*
 * Obtiene IP local si no esta en archivo de configuracion
 */
char* getLocalIp(char *MEM_CONF_IP);

/*
 * Crea la lista de SEEDS al prinicipio del proceso
 */
int crearListaSeeds(char *MEM_CONF_IP,char *MEM_CONF_PUERTO, char **MEM_CONF_IP_SEEDS, char **MEM_CONF_PUERTO_SEEDS, t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT);

/*
 * Crea el hilo encargado del GOSSIPING
 */
int crearHiloGossiping(t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT,int *hilo_cliente);

#endif /* GOSSIPING_H_ */
