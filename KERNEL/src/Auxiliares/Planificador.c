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

		t_pcb* pcb = sacar_proceso_rr(lista_ready);
		imprimir_pcb(pcb);

		log_info(logger, "PLANIFICADOR| Agregando proceso %d a EXEC", pcb->id_proceso);
		agregar_proceso(pcb, lista_exec, &sem_exec);

		sem_wait(&sem_multiprog);
		procesar_pcb(pcb);
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

t_pcb* sacar_proceso_rr(t_list* lista) {

//	sem_wait(sem);
	return (t_pcb *) list_remove(lista_ready, 0);
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

void retardo_ejecucion() {

	sleep(kernel_conf.sleep_ejecucion/1000);
}

void procesar_pcb(t_pcb* pcb) {

	int quantum = kernel_conf.quantum;
	int requests_restantes = pcb->cantidad_request - pcb->program_counter;
	log_info(logger, "PLANIFICADOR| Faltan procesar %d request", requests_restantes);

	int quantum_restante = (pcb->program_counter % quantum) == 0 ? quantum: quantum - (pcb->program_counter % quantum);

	if(requests_restantes < quantum_restante) {
		quantum_restante = requests_restantes;
	}

	log_info(logger, "PLANIFICADOR| Quantum a procesar: %d", quantum_restante);

	for (int i=0; quantum_restante >= i; i++ ) {
		log_info(logger, "PLANIFICADOR| Proceso consumiendo quantum");
		retardo_ejecucion();

		// Parsear request y procesarlo
		char **linea = string_split(pcb->script, "\n");
//		log_info(logger, "PLANIFICADOR| %s", linea[pcb->program_counter]);

		int resultado = ejecutar_request(linea[pcb->program_counter]);

		log_info(logger, "PLANIFICADOR| Resultado del request: %d", resultado);

		log_info(logger, "PLANIFICADOR| Nuevo Program Counter: %d", pcb->program_counter);
		log_info(logger, "PLANIFICADOR| Quantum restante: %d", quantum_restante);

		pcb->program_counter++;
		quantum_restante--;

	}

	// Saco proceso de EXEC y evaluo si finalizó o vuelve a READY
	sacar_proceso(pcb->id_proceso, lista_exec, &sem_exec);
	sem_post(&sem_multiprog);

	if (pcb->program_counter == pcb->cantidad_request) {

		agregar_proceso(pcb, lista_exit, &sem_exit);
		log_info(logger, "PLANIFICADOR| Fin de proceso %d", pcb->id_proceso);
	} else {
		agregar_proceso(pcb, lista_ready, &sem_ready);
		log_info(logger, "PLANIFICADOR| Proceso %d vuelve a READY", pcb->id_proceso);
	}

}

int ejecutar_request(char* linea) {

	log_info(logger, "PLANIFICADOR| Linea a ejecutar:");
	log_info(logger, "PLANIFICADOR| %s", linea);

	t_request* request = parsear(linea, logger);

	switch (request->request) {

		case _select:
			// SELECT [NOMBRE_TABLA] [KEY]
			// SELECT TABLA1 3
			log_info(logger, "PLANIFICADOR|Preparando SELECT");
			log_info(logger, "PLANIFICADOR|Parámetro 1: %s", request->parametro1);
			log_info(logger, "PLANIFICADOR|Parámetro 2: %s", request->parametro2);
			break;

		case _insert:
			// INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
			// INSERT TABLA1 3 “Mi nombre es Lissandra”

			log_info(logger, "PLANIFICADOR|Preparando INSERT");
			log_info(logger, "PLANIFICADOR|Parámetro 1: %s", request->parametro1);
			log_info(logger, "PLANIFICADOR|Parámetro 2: %s", request->parametro2);
			log_info(logger, "PLANIFICADOR|Parámetro 3: %s", request->parametro3);
			log_info(logger, "PLANIFICADOR|Parámetro 4: %s", request->parametro4);
			break;

		case _create:
			// CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
			// CREATE TABLA1 SC 4 60000

			log_info(logger, "PLANIFICADOR|Preparando CREATE");
			log_info(logger, "PLANIFICADOR|Parámetro 1: %s", request->parametro1);
			log_info(logger, "PLANIFICADOR|Parámetro 2: %s", request->parametro2);
			log_info(logger, "PLANIFICADOR|Parámetro 3: %s", request->parametro3);
			log_info(logger, "PLANIFICADOR|Parámetro 4: %s", request->parametro4);
			break;

		case _describe:
			// DESCRIBE [NOMBRE_TABLA]
			// DESCRIBE
			// DESCRIBE TABLA1

			log_info(logger, "PLANIFICADOR|Preparando DESCRIBE");
			if(request->parametro1 != NULL) {
				log_info(logger, "PLANIFICADOR|Parámetro 1: %s", request->parametro1);
			}

			break;

		case _drop:
			// DROP [NOMBRE_TABLA]
			// DROP TABLA1

			log_info(logger, "PLANIFICADOR|Preparando DROP");
			log_info(logger, "PLANIFICADOR|Parámetro 1: %s", request->parametro1);
			break;

		default:
			// No entra por acá porque se valida antes el enum != -1
			log_info(logger, "PLANIFICADOR|No se reconoce operación: %s", request->parametro1);
			break;
	}

	return request->es_valido;
}
