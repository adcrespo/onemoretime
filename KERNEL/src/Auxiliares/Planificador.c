/*
 * Planificador.c
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#include "Planificador.h"


void* planificar() {

	while(1) {

		// Encuentro nuevo proceso en READY
		sem_wait(&sem_multiprog);
		sem_wait(&sem_ready);

//		aplicar_algoritmo_rr();


		pthread_create(&thread_planificacion, NULL, aplicar_algoritmo_rr(), NULL);
//		int hilo_algoritmo = pthread_create(&thread_planificacion, NULL, aplicar_algoritmo_rr(), NULL);

//		if (hilo_algoritmo == -1) {
//			log_error(logger, "THREAD|No se pudo generar el hilo para el algoritmo.");
//		}
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
	imprimir_listas(); // SACAR

	/* 3. Carga de PCB */
	log_info(logger, "PLANIFIC| Cargando PCB.");
	proceso->script = string_new();
	proceso->ruta_archivo = string_new();
	proceso->id_proceso = id_proceso;
	proceso->program_counter = 0;
	string_append(&proceso->script, line);
	proceso->cantidad_request = cantidad_request(line);

	if( request->request == _run) {
		string_append(&proceso->ruta_archivo, request->parametro1);
	}

	imprimir_pcb(proceso);

	/* 4. Pasar PCB de NEW a READY */
	proceso = sacar_proceso(id_proceso, lista_new, &sem_new);
	agregar_proceso(proceso, lista_ready, &sem_ready);
	log_info(logger, "PLANIFIC| Proceso %d de NEW a READY.", id_proceso);

}

void agregar_proceso(t_pcb* proceso,t_list* lista, sem_t* sem) {

	list_add(lista, proceso);
	sem_post(sem);
}

t_pcb* sacar_proceso(int id, t_list* lista, sem_t* sem) {

	int buscar_pcb_por_id(t_pcb* pcb) {
		return pcb->id_proceso == id;
	}
	t_pcb* proceso = list_remove_by_condition(lista, (void *)buscar_pcb_por_id);
	sem_wait(sem);

	return proceso;
}

t_pcb* sacar_proceso_rr(t_list* lista) {

	log_info(logger, "PLANIFIC| Buscando PCB por ROUND ROBIN");
	return (t_pcb *) list_remove(lista_ready, 0);

//	int buscar_primer_elemento(t_pcb* pcb) {
//		return pcb != NULL;
//	}
//
//	return (t_pcb*) list_remove_by_condition(lista, (void *)buscar_primer_elemento);
}

int cantidad_request(char* buffer) {

	char ** arrays_linea = string_split(buffer,"\n" );
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


t_tipoSeeds* obtener_memoria_random() {

	t_tipoSeeds *memory = get_memoria_conectada();;
	log_info(logger, "REFRESH| Numero Memoria: %d", memory->numeroMemoria);
	return memory;
}

//t_tipoSeeds* obtener_memoria_sc(){
//	bool findSC(void* element) {
//			t_tipoSeeds *memoria = element;
//			return memoria->numeroMemoria == memoria_sc->numeroMemoria;
//		}
//	return list_find(LISTA_CONN, &findSC);
//}

void retardo_ejecucion() {

	sleep(kernel_conf.sleep_ejecucion/1000);
}

int procesar_pcb(t_pcb* pcb) {

	int quantum_restante = calcular_quantum(pcb);
	log_info(logger, "PLANIFIC| --- Quantum restante: %d ---", quantum_restante);

	for (int i=0; quantum_restante > i; i++ ) {
		log_info(logger, "PLANIFIC| --- Consumiendo Quantum ---");
		retardo_ejecucion();

		// Parsear request y procesarlo
		char **linea = string_split(pcb->script, "\n");

		log_info(logger, "Linea a ejecutar: %s", linea[pcb->program_counter]);
		int resultado = ejecutar_request(linea[pcb->program_counter], pcb->id_proceso);

		// Si el request falla, se termina el proceso
		if(resultado < 0) {
			sacar_proceso(pcb->id_proceso, lista_exec, &sem_exec);
			sem_post(&sem_multiprog);
			agregar_proceso(pcb, lista_exit, &sem_exit);
			log_info(logger, "PLANIFIC| Proceso %d pasa a EXIT", pcb->id_proceso);
			return EXIT_FAILURE;
		}

		log_info(logger, "PLANIFIC| Resultado del request: %d", resultado);

		pcb->program_counter++;
//		quantum_restante--;
		log_info(logger, "PLANIFIC| Nuevo Program Counter: %d", pcb->program_counter);
//		log_info(logger, "PLANIFIC| Quantum restante: %d", quantum_restante);

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

	return EXIT_SUCCESS;

}

t_registro* descomponer_registro(char *buffer)
{
	t_registro* registro = malloc(sizeof(t_registro));
	memcpy(&registro->timestamp,buffer,sizeof(unsigned long long));
	memcpy(&registro->key,buffer+sizeof(unsigned long long),sizeof(int));
	memcpy(&registro->value[0],buffer+sizeof(unsigned long long)+sizeof(int),VALUE);
	return registro;
}

int ejecutar_request(char* linea, int id_proceso) {

	int t_inicio, t_fin;
	log_info(logger, "PLANIFIC| Request a ejecutar: %s", linea);
	t_request* request = parsear(linea, logger);
	int resultado, cliente;
	t_tipoSeeds* memoria;
	t_metadata* tabla;

	switch (request->request) {

		case _select:;

			t_inicio = obtenerTimeStamp();

			// VALIDAR EXISTENCIA DE TABLA
			tabla = buscar_tabla(request->parametro1);

			if(tabla == NULL) {
				log_info(logger, "PLANIFIC| La tabla no existe.");
				t_fin = obtenerTimeStamp();
				log_info(logger, "PLANIFIC| DURACION SELECT: %d segundos", t_fin - t_inicio);
				return -1;
			}

			// PREPARANDO REQUEST
			log_info(logger, "PLANIFIC|Preparando SELECT");
			t_select* req_select = malloc(sizeof(t_select));

			req_select->id_proceso = id_proceso;
			strcpy(req_select->nombreTabla, request->parametro1);
			req_select->key = atoi(request->parametro2);

			log_info(logger, "PLANIFIC| SELECT: %s, %d", req_select->nombreTabla, req_select->key);

			// BUSCANDO MEMORIA
			log_info(logger, "Buscando memoria del criterio %s", tabla->tipoConsistencia);
			memoria = get_memoria_por_criterio(tabla->tipoConsistencia);
			cliente = conectar_a_memoria(memoria);

			// REQUEST
			log_info(logger, "PLANIFIC| Enviando SELECT a MEMORIA");
			int resultado_mensaje_select = enviarMensajeConError(kernel, selectMsg, sizeof(t_select), req_select, cliente, logger, mem, 0);
			log_info(logger, "Resultado de enviar mensaje SELECT: %d", resultado_mensaje_select);

			// RESPUESTA
			log_info(logger, "PLANIFIC| RECIBIENDO SELECT");
			t_mensaje* resultado_req_select = recibirMensaje(cliente, logger);


			int largo_buffer = resultado_req_select->header.longitud;
			char *buffer = malloc(largo_buffer);
			memcpy(buffer, resultado_req_select->content, resultado_req_select->header.longitud);

			t_registro* reg = descomponer_registro(buffer);

			log_info(logger, "PLANIFIC| RESPUESTA SELECT: KEY:%d, VALOR:%s TIMESTAMP:%llu", reg->key, reg->value,reg->timestamp);

			resultado = resultado_mensaje_select;

			// METRICAS
			t_fin = obtenerTimeStamp();
			int duracion = (t_fin - t_inicio) / 1000;
			log_info(logger, "PLANIFIC| DURACION SELECT: %d segundos", duracion);

			// LIBERAR MEMORIA
			free(req_select);
			destruirMensaje(resultado_req_select);
			free(reg);

//			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.

			break;

		case _insert:
			// INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
			// INSERT TABLA1 3 “Mi nombre es Lissandra”

			log_info(logger, "VALIDACION| MEMORIA SC: %d", memoria_sc->numeroMemoria);

			tabla = buscar_tabla(request->parametro1);

			if(tabla == NULL) {
				log_info(logger, "PLANIFIC| La tabla no existe.");
				return -1;
			}

			log_info(logger, "PLANIFIC| La tabla %s.", tabla->nombreTabla);

			log_info(logger, "PLANIFIC|Preparando INSERT");
			t_insert* req_insert = malloc(sizeof(t_insert));

			req_insert->id_proceso = id_proceso;
			strcpy(req_insert->nombreTabla, request->parametro1);
			req_insert->timestamp = obtenerTimeStamp();
			req_insert->key = atoi(request->parametro2);
			strcpy(req_insert->value, request->parametro3);

			log_info(logger, "PLANIFIC| INSERT: %s, %llu, %d, %s", req_insert->nombreTabla, req_insert->timestamp, req_insert->key, req_insert->value);

			//TODO: FALTA BUSCAR MEMORIA POR CRITERIO Y CONECTARSE
			memoria = get_memoria_por_criterio(tabla->tipoConsistencia);

			log_info(logger, "VALIDACION| MEMORIA SC: %d", memoria_sc->numeroMemoria);
//			memoria = obtener_memoria_random(); // cambiar para hacerlo dinamico para los criterios
			cliente = conectar_a_memoria(memoria);

			log_info(logger, "PLANIFIC| Enviando INSERT a MEMORIA");
			log_info(logger, "PLANIFC - INSERT| Memoria SC tiene: %d", memoria_sc->numeroMemoria);
			log_info(logger, "PLANIFC - INSERT| Memoria Asignada tiene: %d", memoria->numeroMemoria);
			int resultado_mensaje_insert = enviarMensajeConError(kernel, insert, sizeof(t_insert), req_insert, cliente, logger, mem, 0);
			log_info(logger, "Resultado de enviar mensaje INSERT: %d", resultado_mensaje_insert);

			t_mensaje* resultado_req_insert = recibirMensaje(cliente, logger);
			resultado = resultado_req_insert->header.error;
			log_info(logger, "PLANIFIC| Resultado de INSERT: %d", resultado);

			free(req_insert);
			destruirMensaje(resultado_req_insert);

//			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.
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

			log_info(logger, "PLANIFIC| CREATE: %s, %s, %d, %d", req_create->nombreTabla, req_create->tipo_cons, req_create->num_part, req_create->comp_time);

			//TODO: FALTA BUSCAR MEMORIA POR CRITERIO Y CONECTARSE
			memoria = obtener_memoria_random(); // cambiar para hacerlo dinamico para los criterios
			cliente = conectar_a_memoria(memoria);

			log_info(logger, "PLANIFIC| Enviando CREATE a MEMORIA");
//			int resultado_mensaje = enviarMensaje(kernel, create, sizeof(t_create), req_create, cliente, logger, mem);
			int resultado_mensaje = enviarMensajeConError(kernel, create, sizeof(t_create), req_create, cliente, logger, mem, 0);
			log_info(logger, "Resultado de enviar mensaje: %d", resultado_mensaje);

			t_mensaje* resultado_req_create = recibirMensaje(cliente, logger);
			resultado = resultado_req_create->header.error;
			log_info(logger, "PLANIFIC| Resultado de CREATE: %d", resultado);

			if (resultado == 0) {
//				char** buff = string_from_format("DESCRIBE %s", req_create->nombreTabla);
//				log_info(logger, "PLANIFIC| Solicitando: %s", buff);
				pthread_mutex_lock(&mutex_metadata);
				describe_global(cliente);
				pthread_mutex_unlock(&mutex_metadata);
			}

			free(req_create);
			destruirMensaje(resultado_req_create);

			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.

			break;

		case _describe:
			// DESCRIBE
			// DESCRIBE TABLA1

			log_info(logger, "PLANIFIC|Preparando DESCRIBE");

			t_describe* req_describe = malloc(sizeof(t_describe));
			req_describe->id_proceso = id_proceso;
			strcpy(req_describe->nombreTabla, "");

			memoria = obtener_memoria_random(); // ----- cambiar para hacerlo dinamico para los criterios
			cliente = conectar_a_memoria(memoria);

			if (string_is_empty(request->parametro1)) {
				log_info(logger, "PLANIFIC| Describe GLOBAL.");
				describe_global(cliente);
			} else {
				log_info(logger, "PLANIFIC| DESCRIBE: %s", req_describe->nombreTabla);
				log_info(logger, "PLANIFIC| Describe %s.", request->parametro1);
				log_info(logger, "PLANIFIC| Parámetro 1: %s", request->parametro1);
				strcpy(req_describe->nombreTabla, request->parametro1);


				log_info(logger, "PLANIFIC| Enviando DESCRIBE a MEMORIA");
				enviarMensaje(kernel, describe, sizeof(t_describe), req_describe, cliente, logger, mem);

				t_mensaje* msg_describe = recibirMensaje(cliente, logger);
				char* buffer_describe= string_new();
				string_append(&buffer_describe, msg_describe->content);

				log_info(logger, "RESULTADO METADATA TABLA %s| Metadata: %s",req_describe->nombreTabla, buffer_describe);

				//guardar_metadata(buffer_describe);
				destruirMensaje(msg_describe);
				free(buffer_describe);
			}

			free(req_describe);

			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.
			break;

		case _drop:
			// DROP [NOMBRE_TABLA]
			// DROP TABLA1

			tabla = buscar_tabla(request->parametro1);

			if(tabla == NULL) {
				log_info(logger, "PLANIFIC| La tabla no existe.");
				return -1;
			}

			log_info(logger, "PLANIFIC|Preparando DROP");
			t_drop* req_drop = malloc(sizeof(t_drop));

			req_drop->id_proceso = id_proceso;
			strcpy(req_drop->nombreTabla, request->parametro1);

			log_info(logger, "PLANIFIC| DROP OK. %s", req_drop->nombreTabla);

			//TODO: FALTA BUSCAR MEMORIA POR CRITERIO Y CONECTARSE
			memoria = obtener_memoria_random(); // cambiar para hacerlo dinamico para los criterios
			cliente = conectar_a_memoria(memoria);

			log_info(logger, "PLANIFIC| Enviando DROP a MEMORIA");
			int resultado_mensaje_drop = enviarMensajeConError(kernel, drop, sizeof(t_drop), req_drop, cliente, logger, mem, 0);
			log_info(logger, "Resultado de enviar mensaje: %d", resultado_mensaje_drop);

			t_mensaje* resultado_req_drop = recibirMensaje(cliente, logger);
			resultado = resultado_req_drop->header.error;
			log_info(logger, "PLANIFIC| Resultado de DROP: %d", resultado);

			free(req_drop);
			destruirMensaje(resultado_req_drop);

			resultado = request->es_valido; // Cambiar por lo que devuelve la memoria.
			break;

		default:
			// No entra por acá porque se valida antes el enum != -1
			log_info(logger, "PLANIFIC|No se reconoce operación: %s", request->request);
			break;
	}

	return resultado;
}


void* aplicar_algoritmo_rr() {
	t_pcb* pcb = sacar_proceso_rr(lista_ready);

	if (pcb == NULL) {
		log_info(logger, "PLANIFIC| No se encontró el PCB");
	} else {
		log_info(logger, "PLANIFIC| Proceso %d removido", pcb->id_proceso);
	}

	agregar_proceso(pcb, lista_exec, &sem_exec);
	imprimir_listas(); // SACAR

	log_info(logger, "PLANIFIC| Proceso %d pasa a EXEC", pcb->id_proceso);

	int resultado = procesar_pcb(pcb);

	log_info(logger, "PLANIFIC| Fin hilo planificación proceso N° %d", pcb->id_proceso);

	imprimir_listas();
//	return NULL;
//	return (void*)EXIT_SUCCESS;
	return (void*)resultado;
}

void imprimir_listas() {

	log_info(logger, "LISTA NEW: %d", lista_new->elements_count);
	log_info(logger, "LISTA READY: %d", lista_ready->elements_count);
	log_info(logger, "LISTA EXEC: %d", lista_exec->elements_count);
	log_info(logger, "LISTA EXIT: %d", lista_exit->elements_count);
}

int calcular_quantum(t_pcb* pcb) {

	int quantum = kernel_conf.quantum;
	int requests_restantes = pcb->cantidad_request - pcb->program_counter;
//	log_info(logger, "PLANIFIC| Request restantes: %d", requests_restantes);

	int quantum_restante = (pcb->program_counter % quantum) == 0 ? quantum: quantum - (pcb->program_counter % quantum);

	if(requests_restantes < quantum_restante) {
		quantum_restante = requests_restantes;
	}

	log_info(logger, "PLANIFIC| Quantum disponible: %d", quantum_restante);

	return quantum_restante;

}
