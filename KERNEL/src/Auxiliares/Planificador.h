/*
 * Planificador.h
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_PLANIFICADOR_H_
#define AUXILIARES_PLANIFICADOR_H_

#include "parser.h"

/* Definición de tipos */

// Estructura planificable
typedef struct {
	int32_t id_proceso;
//	char ruta_archivo[100];
//	char script[1000];
	char* ruta_archivo;
	char* script;
	int32_t program_counter;
}__attribute__((packed)) t_pcb;


int cont_id_procesos;

/* Colas de estados */
t_list* lista_new;
t_list* lista_ready;
t_list* lista_exec;
t_list* lista_exit;


int asignar_id_proceso();
void generar_nuevo_proceso(t_request* request);
t_pcb* crear_proceso(char* line,t_request* request);

#endif /* AUXILIARES_PLANIFICADOR_H_ */
