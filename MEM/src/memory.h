/*
 * memory.h
 *
 *  Created on: 31 oct. 2018
 *      Author: utnso
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <shared.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <commons/collections/list.h>

typedef struct {
	int frame;
} t_paginas_spa;

typedef struct {
	t_list* pag_lista;
} t_segmentos_spa;

typedef struct {
	int pid;
	t_list* seg_lista;
} t_adm_tabla_segmentos_spa;

typedef struct {
	int pid;
	int segmento;
} t_adm_tabla_frames_spa;

char* frames_spa;
int frames_spa_count;
int frame_spa_size;

t_list* adm_spa_lista;
t_list* adm_frame_lista_spa;

void liberar_memory_spa();
void init_memory_spa();
char* leer_bytes_spa(int pid, int segmento, int offset, int size);
int escribir_bytes_spa(int pid, int segmento, int offset, int size, char* buffer);
int add_spa(int pid, int n_frames);
void free_spa(int pid, int segmento);
void update_administrative_register_adm_table_spa(t_adm_tabla_segmentos_spa* adm_table);
void dump_memory_spa(int pid);

#endif /* MEMORY_H_ */
