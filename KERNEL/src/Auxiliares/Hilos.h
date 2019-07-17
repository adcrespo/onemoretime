/*
 * Hilos.h
 *
 *  Created on: 17 jul. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_HILOS_H_
#define AUXILIARES_HILOS_H_

#include "../KERNEL.h"
#include <stdlib.h>
#include "Planificador.h"


void crear_hilo_consola();
void crear_hilo_gossiping();
void init_gossiping();
void *hiloGossiping();
int crear_hilo_inotify();
void crear_hilo_refresh();
void *inicializar_refresh();
void crear_hilo_planificador();
void limpiar_metadata();

#endif /* AUXILIARES_HILOS_H_ */
