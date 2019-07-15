/*
 * KERNEL.h
 *
 *  Created on: 13 abr. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <shared.h>
#include <gossiping.h>
#include "Auxiliares/Logueo.h"
#include "Auxiliares/Consola.h"
#include "Auxiliares/Planificador.h"
#include "Auxiliares/Configuracion.h"
#include "Auxiliares/Conexion.h"
#include "Auxiliares/argparse.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* Variables globales */
//t_log* logger;
pthread_t thread_consola;
pthread_t thread_gossiping;
pthread_t inotify;

int socket_memoria;
t_list *LISTA_CONN;
char** lista_ips;
char** lista_puertos;


/* Declaración de Procesos*/
//void cargar_configuracion_kernel();
void inicializar();
void crear_hilo_consola();
void crear_hilo_gossiping();
void init_gossiping();
void *hiloGossiping();
int crearHiloInotify();


#endif /* KERNEL_H_ */
