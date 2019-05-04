/*
 * LISSANDRA.h
 *
 *  Created on: 29 abr. 2019
 *      Author: utnso
 */

#ifndef LFS_H_
#define LFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <shared.h>

#include "Configuracion.h"
#include "Filesystem.h"
#include "Consola.h"

t_log *logger;



// Variables globales

pthread_t thread_filesystem;
pthread_t thread_consola;

void crear_hilo_filesystem();
void crear_hilo_consola();



#endif /* LFS_H_ */
