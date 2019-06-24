/*
 * Compactacion.h
 *
 *  Created on: 15 jun. 2019
 *      Author: utnso
 */

#ifndef COMPACTACION_H_
#define COMPACTACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <shared.h>
#include <parser.h>
#include <string.h>
#include <signal.h>

typedef struct {
	int retardo;
	char* path_tabla;
}t_datos_compactacion;

pthread_t compactacion;
int hilo_compactacion;

int crearHiloCompactacion();

#endif /* COMPACTACION_H_ */