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
//#include "Auxiliares/Planificador.h"
#include "Auxiliares/Configuracion.h"
#include "Auxiliares/Conexion.h"
#include "Auxiliares/Hilos.h"
#include <semaphore.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

/* Variables globales */
//t_log* logger;
pthread_t thread_consola;
pthread_t thread_gossiping;
pthread_t inotify;
pthread_t thread_refresh;
pthread_t thread_planificacion;

/* Definición de tipos */
sem_t sem_new, sem_ready, sem_exec, sem_exit, sem_multiprog;

int memoria_sc;
t_list *lista_criterio_shc;
t_list *lista_criterio_ev;
t_list *lista_metadata;
t_list *LISTA_CONN;
char** lista_ips;
char** lista_puertos;


/* Structs */
typedef struct {
	char nombreTabla[50];
	char tipoConsistencia[4];
	int particiones;
	int compactationTime;
} t_metadata;

/* Declaración de Procesos*/
//void cargar_configuracion_kernel();
void inicializar();
void inicializar_semaforos();
void aplicar_tiempo_refresh();
t_metadata* buscar_tabla(char *nombre);



#endif /* KERNEL_H_ */
