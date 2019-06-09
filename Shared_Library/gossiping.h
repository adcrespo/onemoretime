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

pthread_t cliente;
pthread_mutex_t mutexGossiping;
pthread_mutex_t mutexprocessGossiping;
t_list *LISTA_CONN;
t_list *LISTA_CONN_PORT;

typedef enum tipoServidor {
	memoria,gossiping
} t_tipoServidor;

int procesarMsjGossiping(char *mensaje, char *primerParser, char *segundoParser, t_log *logger);
char* getLocalIp(char *MEM_CONF_IP);
int crearListaSeeds(char *MEM_CONF_IP,char *MEM_CONF_PUERTO, char **MEM_CONF_IP_SEEDS, char **MEM_CONF_PUERTO_SEEDS, t_log *logger);
int crearHiloGossiping(t_log *logger);

#endif /* GOSSIPING_H_ */
