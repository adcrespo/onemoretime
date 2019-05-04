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

t_log *logger;



// Variables globales

pthread_t thread_filesystem;

void crear_hilo_filesystem();



#endif /* LFS_H_ */