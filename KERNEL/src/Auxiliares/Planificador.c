/*
 * Planificador.c
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#include "Planificador.h"


void* planificar() {

	while(1) {

		int valor= 0;
		int valor_2= 0;

		sem_wait(&sem_ready);
		sem_wait(&sem_multiprog);

		sem_getvalue(&sem_ready, &valor);
		sem_getvalue(&sem_multiprog, &valor_2);

		log_info(logger, "Sem Ready: %d", valor);
		log_info(logger, "Sem Multiprog: %d", valor_2);


		imprimir_listas();

//		t_pcb* pcb = sacar_proceso_rr(lista_ready);
//		log_info(logger, "PLANIFIC| Proceso N°: %d.", pcb->id_proceso);
//
//		log_info(logger, "PLANIFIC| Proceso %d pasa a EXEC", pcb->id_proceso);
//		agregar_proceso(pcb, lista_exec, &sem_exec);
//
////		sem_wait(&sem_multiprog);
//		procesar_pcb(pcb);

		int hilo_algoritmo = pthread_create(&thread_planificacion, NULL, aplicar_algoritmo_rr(), NULL);
		if (hilo_algoritmo == -1) {
			log_error(logger, "THREAD|No se pudo generar el hilo para el algoritmo.");
		}
		log_info(logger, "THREAD|Se generó el hilo para el algoritmo.");
	}
}

int asignar_id_proceso() {
	cont_id_procesos += 1;
	return cont_id_procesos;
}

void crear_proceso(char* line,t_request* request) {

	/* 1. Creación de PCB */
	t_pcb* proceso = malloc(sizeof(t_pcb));
	int id_proceso = asignar_id_proceso();

	/* 2. Agregar a NEW */
	log_info(logger, "PLANIFIC| Proceso %d generado", id_proceso);
	agregar_proceso(proceso, lista_new, &sem_new);
	log_info(logger, "PLANIFIC| Proceso %d agregado a NEW", id_proceso);

	/* 3. Carga de PCB */
	log_info(logger, "PLANIFIC| Generando PCB.");
	proceso->script = string_new();
	proceso->ruta_archivo = string_new();

	if( request->request == _run) {
		string_append(&proceso->ruta_archivo, request->parametro1);
	}

	proceso->id_proceso = id_proceso;
	proceso->program_counter = 0;
	string_append(&proceso->script, line);
	proceso->cantidad_request = cantidad_request(line);

	/* 4. Pasar PCB de NEW a READY */
	log_info(logger, "PLANIFIC|Pasando proceso %d de NEW a READY.", id_proceso);
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
	log_info(logger, "PLANIFIC| Buscando PCB por ROUND ROBIN");
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

	t_tipoSeeds *memory = get_memoria_conectada();;
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
	log_info(logger, "PLANIFIC| Request restantes: %d", requests_restantes);

	int quantum_restante = (pcb->program_counter % quantum) == 0 ? quantum: quantum - (pcb->program_counter % quantum);

	if(requests_restantes < quantum_restante) {
		quantum_restante = requests_restantes;
	}

	log_info(logger, "PLANIFIC| Quantum disponible: %d", quantum_restante);

	for (int i=0; quantum_restante > i; i++ ) {
		log_info(logger, "PLANIFIC| --- Consumiendo Quantum ---");
		retardo_ejecucion();

		// Parsear request y procesarlo
		char **linea = string_split(pcb->script, "\n");

		int resultado = ejecutar_request(linea[pcb->program_counter], pcb->id_proceso);

		log_info(logger, "PLANIFIC| Resultado del request: %d", resultado);

		pcb->program_counter++;
		quantum_restante--;
		log_info(logger, "PLANIFIC| Nuevo Program Counter: %d", pcb->program_counter);
		log_info(logger, "PLANIFIC| Quantum restante: %d", quantum_restante);

	}

	// Saco proceso de EXEC y evaluo si finalizó o vuelve a READY
	sacar_proceso(pcb->id_proceso, lista_exec, &sem_exec);
	sem_post(&sem_multiprog);

	if (pcb->program_counter == pcb->cantidad_request) {

		agregar_proceso(pcb, lista_exit, &sem_exit);
		log_info(logger, "PLANIFIC| Proceso %d pasa a EXIT", pcb->id_proceso);
	} else {
		agregar_proceso(pcb, lista_ready, &sem_ready);
		log_info(logger, "PLANIFIC| Proceso %d vuelve a READY", pcb->id_proceso);
	}

}

int ejecutar_request(char* linea, int id_proceso) {

//	log_info(logger, "PLANIFIC| Request a ejecutar:");
	log_info(logger, "PLANIFIC| Request a ejecutar: %s", linea);
	t_request* request = parsear(linea, logger);
	int resultado;

	switch (request->request) {

		case _select:;
			// SELECT [NOMBRE_TABLA] [KEY]
			// SELECT TABLA1 3

			int existe = validar_tabla(request->parametro1);

			log_info(logger, "Existe: %d", existe);
			if (existe == 0) {
				log_info(logger, "PLANIFIC| La tabla no existe.");
			} else {
				log_info(logger, "PLANIFIC| La tabla existe.");
			}


			log_info(logger, "PLANIFIC| Preparando SELECT");
			t_select* req_select = malloc(sizeof(t_create));

			req_select->id_proceso = id_proceso;
			strcpy(req_select->nombreTabla, request->parametro1);
			req_select->key = atoi(request->parametro2);

			log_info(logger, "PLANIFIC| SELECT OK. %s, %d", req_select->nombreTabla, req_select->key);
			log_info(logger, "PLANIFIC| Enviando SELECT a MEMORIA");

			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.
			free(req_select);
			break;

		case _insert:
			// INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
			// INSERT TABLA1 3 “Mi nombre es Lissandra”

			log_info(logger, "PLANIFIC|Preparando INSERT");
			t_insert* req_insert = malloc(sizeof(t_insert));

			req_insert->id_proceso = id_proceso;
			strcpy(req_insert->nombreTabla, request->parametro1);
			req_insert->timestamp = atoi(request->parametro2);
			req_insert->key = atoi(request->parametro3);
			strcpy(req_insert->value, request->parametro4);

			log_info(logger, "PLANIFIC| INSERT OK. %s, %d, %d, %s", req_insert->nombreTabla, req_insert->timestamp, req_insert->key, req_insert->value);
			log_info(logger, "PLANIFIC| Enviando INSERT a MEMORIA");

			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.
			free(req_insert);
			break;

		case _create:;
			// CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
			// CREATE TABLA1 SC 4 60000

			log_info(logger, "PLANIFIC|Preparando CREATE");

			t_create* req_create = malloc(sizeof(t_create));

			req_create->id_proceso = id_proceso;
			strcpy(req_create->nombreTabla, request->parametro1);
			strcpy(req_create->tipo_cons, request->parametro2);
			req_create->num_part = atoi(request->parametro3);
			req_create->comp_time = atoi(request->parametro4);

			log_info(logger, "PLANIFIC| CREATE OK. %s, %s, %d, %d", req_create->nombreTabla, req_create->tipo_cons, req_create->num_part, req_create->comp_time);
			log_info(logger, "PLANIFIC| Enviando CREATE a MEMORIA");

			t_tipoSeeds* memoria = obtener_memoria_sc(); // ----- cambiar para hacerlo dinamico para los criterios
			int cliente = conectar_a_memoria(memoria);

//			int resultado_mensaje = enviarMensaje(kernel, create, sizeof(t_create), req_create, cliente, logger, mem);
			int resultado_mensaje = enviarMensajeConError(kernel, create, sizeof(t_create), req_create, cliente, logger, mem, 0);

			log_info(logger, "Resultado de enviar mensaje: %d", resultado_mensaje);

			t_mensaje* resultado_req = recibirMensaje(cliente, logger);
			resultado = resultado_req->header.error;
			log_info(logger, "PLANIFIC| Resultado de CREATE: %d", resultado);

//			close(cliente);
//			free(req_create);
			break;

		case _describe:
			// DESCRIBE
			// DESCRIBE TABLA1

			log_info(logger, "PLANIFIC|Preparando DESCRIBE");

			t_describe* req_describe = malloc(sizeof(t_describe));
			req_describe->id_proceso = id_proceso;
			strcpy(req_describe->nombreTabla, "");
			if(request->parametro1 != NULL) {
				log_info(logger, "PLANIFIC|Parámetro 1: %s", request->parametro1);
				strcpy(req_describe->nombreTabla, request->parametro1);
			}

			log_info(logger, "PLANIFIC| DESCRIBE OK. %s", req_describe->nombreTabla);
			log_info(logger, "PLANIFIC| Enviando DESCRIBE a MEMORIA");

			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.
			free(req_describe);
			break;

		case _drop:
			// DROP [NOMBRE_TABLA]
			// DROP TABLA1

			log_info(logger, "PLANIFIC|Preparando DROP");
			t_drop* req_drop = malloc(sizeof(t_drop));

			req_drop->id_proceso = id_proceso;
			strcpy(req_drop->nombreTabla, request->parametro1);

			log_info(logger, "PLANIFIC| DROP OK. %s", req_drop->nombreTabla);
			log_info(logger, "PLANIFIC| Enviando DROP a MEMORIA");

			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.
			free(req_drop);
			break;

		default:
			// No entra por acá porque se valida antes el enum != -1
			log_info(logger, "PLANIFIC|No se reconoce operación: %s", request->request);
			break;
	}

	return resultado;
}


void* aplicar_algoritmo_rr() { // @suppress("No return")
	t_pcb* pcb = sacar_proceso_rr(lista_ready);
	log_info(logger, "PLANIFIC| Proceso N°: %d.", pcb->id_proceso);

	log_info(logger, "PLANIFIC| Proceso %d pasa a EXEC", pcb->id_proceso);
	agregar_proceso(pcb, lista_exec, &sem_exec);

//		sem_wait(&sem_multiprog);
	procesar_pcb(pcb);

	log_info(logger, "PLANIFIC| Fin hilo planificación proceso N° %d", pcb->id_proceso);
	return NULL;
}

void imprimir_listas() {

	log_info(logger, "LISTA NEW: %d", lista_new->elements_count);
	log_info(logger, "LISTA READY: %d", lista_ready->elements_count);
	log_info(logger, "LISTA EXEC: %d", lista_exec->elements_count);
	log_info(logger, "LISTA EXIT: %d", lista_exit->elements_count);
}
