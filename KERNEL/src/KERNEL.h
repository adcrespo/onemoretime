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


/* Variables globales */
t_log* logger;
pthread_t thread_consola;


/* Declaración de Procesos*/
void crear_consola();
void crear_hilo_consola();


#endif /* KERNEL_H_ */
