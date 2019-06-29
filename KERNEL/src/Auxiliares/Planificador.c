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
	proceso->ruta_archivo = string_new();
	proceso->id_proceso = asignar_id_proceso();
	proceso->program_counter = 0;
	string_append(&proceso->script, line);
	proceso->cantidad_request = cantidad_request(line);

	// Agregar proceso a NEW
	list_add(lista_new, proceso);

	return proceso;
}





int cantidad_request(char* buffer) {

	char ** arrays_linea = strlineassplit(buffer,"\n" );
	int i = 0;
	int cantidad_lineas = 0;

	while(arrays_linea[i] != NULL){
		cantidad_lineas++;
		i++;
	}

	if (cantidad_lineas == 0) {
		cantidad_lineas = 1;
	}

	return cantidad_lineas;
}
