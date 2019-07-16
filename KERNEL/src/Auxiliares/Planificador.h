/*
 * Planificador.h
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_PLANIFICADOR_H_
#define AUXILIARES_PLANIFICADOR_H_


#include "../KERNEL.h"

/* Definición de tipos */

// Estructura planificable
typedef struct {
	int32_t id_proceso;
	char* ruta_archivo;
	char* script;
	int32_t cantidad_request;
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
int cantidad_request(char* buffer);
void imprimir_pcb(t_pcb* pcb);
t_tipoSeeds* get_memoria_por_criterio(char *criterio);
t_tipoSeeds* obtener_memoria_random();
t_tipoSeeds* obtener_memoria_sc();

#endif /* AUXILIARES_PLANIFICADOR_H_ */
