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
#include <parser.h>
#include <string.h>

#include "Configuracion.h"
#include "Filesystem.h"
#include "Consola.h"
#include "Conexion.h"
#include "Dump.h"
#include "Error.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

t_log *logger;
t_list *memtable;

typedef struct {
	char nombre_tabla[20];
     t_list *lista;
}__attribute__((packed)) t_tabla;


typedef struct {
	int timestamp;
	int key;
	char value[50];
} t_registro;


pthread_t thread_consola;
pthread_t thread_conexiones;
pthread_t thread_dump;


void CrearHiloConsola();
void CrearHiloConexiones();
void CrearHiloDump();
void Inicializar();
t_tabla* ObtenerTabla(char *nombre);
void AlocarTabla(char *tabla, t_registro *registro);



#endif /* LFS_H_ */
