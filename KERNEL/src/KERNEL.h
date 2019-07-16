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
#include "parser.h"
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
pthread_t thread_refresh;

/* Structs */
typedef struct {
	char nombreTabla[50];
	char tipoConsistencia[4];
	int particiones;
	int compactationTime;
} t_metadata;


int socket_memoria;
int memoria_sc;
t_list *lista_criterio_shc;
t_list *lista_criterio_ev;
t_list *lista_metadata;
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
int crear_hilo_inotify();
void crear_hilo_refresh();
void *inicializar_refresh();
void aplicar_tiempo_refresh();
t_metadata* buscar_tabla(char *nombre);



#endif /* KERNEL_H_ */
