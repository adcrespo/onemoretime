/*
 * Planificador.h
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_PLANIFICADOR_H_
#define AUXILIARES_PLANIFICADOR_H_

/* Definición de tipos */

// Estructura planificable
typedef struct {
	int32_t id_proceso;
	char ruta_archivo[100];
	int32_t program_counter;
}__attribute__((packed)) t_pcb;


/* Colas de estados */
t_list* lista_new;
t_list* lista_ready;
t_list* lista_exec;
t_list* lista_exit;


#endif /* AUXILIARES_PLANIFICADOR_H_ */
