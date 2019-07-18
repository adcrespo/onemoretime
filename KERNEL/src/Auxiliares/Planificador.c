/*
 * Planificador.c
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#include "Planificador.h"


void* planificar() {

	while(1) {

		sem_wait(&sem_ready);

		log_info(logger, " --------- Planificando -------- ");

	}
}

int asignar_id_proceso() {
	cont_id_procesos += 1;
	return cont_id_procesos;
}

//t_pcb* crear_proceso(char* line,t_request* request) {
void crear_proceso(char* line,t_request* request) {

	/* 1. Creación de PCB */
	t_pcb* proceso = malloc(sizeof(t_pcb));
	int id_proceso = asignar_id_proceso();

	/* 2. Agregar a NEW */
	log_info(logger, "PLANIFICADOR|Proceso %d generado. Agregado a NEW.", id_proceso);
	int valor;
	sem_getvalue(&sem_new, &valor);
	log_info(logger, "PLANIFICADOR| Sem new: %d", valor);
	agregar_proceso(proceso, lista_new, &sem_new);
	sem_getvalue(&sem_new, &valor);
	log_info(logger, "PLANIFICADOR| Sem new: %d", valor);

	/* 3. Carga de PCB */
	log_info(logger, "PLANIFICADOR|Generando estructura de planificación.");
	proceso->script = string_new();

	if( request->request == _run) {
		proceso->ruta_archivo = request->parametro1;
	} else {
		proceso->ruta_archivo = string_new();
	}
	proceso->id_proceso = id_proceso;
	proceso->program_counter = 0;
	string_append(&proceso->script, line);
	proceso->cantidad_request = cantidad_request(line);

	/* 4. Pasar PCB de NEW a READY */
	log_info(logger, "PLANIFICADOR|Pasando proceso %d de NEW a READY.", id_proceso);
	proceso = sacar_proceso(id_proceso, lista_new, &sem_new);
	agregar_proceso(proceso, lista_ready, &sem_ready);

	imprimir_pcb(proceso);

}

void agregar_proceso(t_pcb* proceso,t_list* lista, sem_t* sem) {

	sem_post(sem);
	list_add(lista, proceso);
}

t_pcb* sacar_proceso(int id, t_list* lista, sem_t* sem) {

	int buscar_pcb_por_id(t_pcb* pcb) {
		return pcb->id_proceso == id;
	}

	sem_wait(sem);
	return (t_pcb *) list_remove_by_condition(lista, (void *)buscar_pcb_por_id);
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


t_tipoSeeds* get_memoria_por_criterio(char *criterio) {
	t_tipoSeeds *memory;
	int tipo_criterio = criterio_to_enum(criterio);
	switch (tipo_criterio) {
	case SC:
		//devolver memoria_sc
		memory = obtener_memoria_sc();
		break;
	case SHC:
		break;
	case EV:
		//memoria random
		memory = obtener_memoria_random();
		break;

	}
	return memory;
}

t_tipoSeeds* obtener_memoria_random() {
	int n;
	int size_ev = list_size(lista_criterio_ev);
	log_info(logger, "REFRESH| Size EV: %d", size_ev);
	n = rand() % size_ev;
	log_info(logger, "REFRESH| Random n: %d", n);

	t_tipoSeeds *memory;
	memory = (t_tipoSeeds*)list_get(lista_criterio_ev, n);
	log_info(logger, "REFRESH| Numero Memoria: %d", memory->numeroMemoria);
	return memory;
}

t_tipoSeeds* obtener_memoria_sc(){
	bool findSC(void* element) {
			t_tipoSeeds *memoria = element;
			return memoria->numeroMemoria == memoria_sc;
		}
	return list_find(LISTA_CONN, &findSC);
}


