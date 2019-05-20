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
#include "Conexion.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

t_log *logger;
t_list *memtable;

typedef struct {
	char nombre_tabla[20];
     t_list *lista;
}__attribute__((packed)) t_tabla;



pthread_t thread_consola;
pthread_t thread_conexiones;



void CrearHiloConsola();
void CrearHiloConexiones();



#endif /* LFS_H_ */
