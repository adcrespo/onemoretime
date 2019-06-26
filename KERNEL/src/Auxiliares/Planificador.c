/*
 * Planificador.c
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#include "Planificador.h"

int asignar_id_proceso() {
	cont_id_procesos += 1;
	return cont_id_procesos;
}

void generar_nuevo_proceso(t_request* request) {

//	t_pcb* proceso = malloc(sizeof(t_pcb));

}

t_pcb* crear_proceso(char* line,t_request* request) {

	t_pcb* proceso = malloc(sizeof(t_pcb));
	proceso->script = string_new();
	proceso->id_proceso = asignar_id_proceso();
	proceso->program_counter = 0;
	string_append(&proceso->script, line);
	return proceso;
}


