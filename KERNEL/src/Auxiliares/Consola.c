/*
 * Consola.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#include "Consola.h"

void *crear_consola() {

	char * line;
	rl_attempted_completion_function = character_name_completion;

	int estado;

	while (1) {
		line = readline("Ingrese un comando > ");
		if (line) {
			add_history(line);
		}

		estado = procesar_comando(line);

		if (estado != 0) {
			return (void*)EXIT_FAILURE;
		}
	}
}

int procesar_comando(char *line) {

	log_info(logger, "CONSOLA: %s.", line);

	char* linea_auxiliar = string_new();
	string_append(&linea_auxiliar, line);
	t_request* request = parsear(line, logger);
	t_pcb* proceso_nuevo;

	if (request->request == -1) {

		log_error(logger, "CONSOLA: Se ingresó un comando desconocido: %s.", line);
		printf("Se ingresó un comando desconocido: %s.\n", line);

	} else if (request->es_valido == -1) {

		log_error(logger, "CONSOLA: No se ingresaron los parámetros necesarios: %s.\n", line);
		printf("No se ingresaron los parámetros necesarios: %s.\n", line);

	} else {

		switch (request->request) {

			case _salir:
				puts("Muchas gracias por utilizar el proceso KERNEL. Vuelva pronto!");
				return -1;

			case _select:
				// Generar nuevo proceso.
				proceso_nuevo = crear_proceso(linea_auxiliar, request);

				// Loguear nuevo proceso
				log_info(logger, "Proceso generado.");
				imprimir_pcb(proceso_nuevo);

				printf("Validando existencia de Tabla.\n");
				printf("Seleccionando Memoria según Criterio.\n");
				printf("Enviando SELECT a una Memoria.\n");
				break;

			case _insert:
				// Generar nuevo proceso.
				proceso_nuevo = crear_proceso(linea_auxiliar, request);

				// Loguear nuevo proceso
				log_info(logger, "Proceso generado.");
				imprimir_pcb(proceso_nuevo);

				printf("Validando existencia de Tabla.\n");
				printf("Seleccionando Memoria según Criterio.\n");
				printf("Enviando INSERT a una Memoria.\n");
				break;

			case _create:
				// Generar nuevo proceso.
				proceso_nuevo = crear_proceso(linea_auxiliar, request);

				// Loguear nuevo proceso
				log_info(logger, "Proceso generado.");
				imprimir_pcb(proceso_nuevo);
				break;

			case _describe:
				printf("Enviando DESCRIBE a una Memoria \n"); /* CAMBIAR SOCKET DE ABAJO */
				// elegir memoria
				//enviarMensaje(kernel, describe, sizeof(request), &request, 10/* socket memoria*/, logger, mem);
				break;

			case _drop:

				// Generar nuevo proceso.
				proceso_nuevo = crear_proceso(linea_auxiliar, request);

				// Loguear nuevo proceso
				log_info(logger, "Proceso generado.");
				imprimir_pcb(proceso_nuevo);

				printf("Validando existencia de Tabla.\n");
				printf("Seleccionando Memoria según Criterio.\n");
				printf("Enviando DROP a una Memoria.\n");
				break;

			case _journal:
				printf("CONSOLA: Se ingresó comando JOURNAL \n");
				break;

			case _add:
				printf("CONSOLA: Se ingresó comando ADD \n");
				char *criterio = string_new();
				string_append(&criterio, request->parametro4);
				int numeroMemoria = atoi(request->parametro2);
				int comando = criterio_to_enum(request->parametro4);

				switch(comando){
				case SC:
					printf("Agregando memoria %d a strong consistency.\n", numeroMemoria);
					memoria_sc = numeroMemoria;
					break;
				case SHC:
					printf("Agregando memoria %d a strong hash consistency.\n", numeroMemoria);
					break;
				case EV:
					printf("Agregando memoria %d a eventual consistency.\n", numeroMemoria);
					break;
				default:
					printf("Criterio no reconocido.\n");
				}


				break;

			case _run:
				printf("CONSOLA: Se ingresó comando RUN \n");
				printf("Parámetro: %s \n", request->parametro1);

				generar_nuevo_proceso(request);
				abrir_archivo_LQL(request);
				break;

			case _metrics:

				log_info(logger, "Lista NEW: %d elementos.", list_size(lista_new));
				log_info(logger, "Lista READY: %d elementos.", list_size(lista_ready));
				log_info(logger, "Lista EXEC: %d elementos.", list_size(lista_exec));
				log_info(logger, "Lista EXIT: %d elementos.", list_size(lista_exit));
				break;

			default:;
				// No entra por acá porque se valida antes el enum != -1
				//printf("No se reconoce el comando %s .\n", comando);
		}

	}

	return 0;
}

void abrir_archivo_LQL(t_request* request) {

	FILE *file;
	char* linea = string_new();
	size_t len = 0;
	int cantidad_lineas = 0;

	file = fopen(request->parametro1, "r");

	if (file == NULL) {
		log_error(logger, "CONSOLA: No se puede abrir el archivo: %s", request->parametro1);
		printf("No se puede abrir el archivo: %s", request->parametro1);
	}

	while (getline(&linea, &len, file) != -1) {
		printf("Contenido de línea: %s",linea);
		cantidad_lineas++;
	}

	printf("Cantidad de líneas: %d \n",cantidad_lineas);

	fclose(file);
	if(linea)
		free(linea);
}

//void generar_nuevo_proceso(t_request* request) {
//
//
//}


char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "SELECT"
			, "INSERT"
			, "CREATE"
			, "DESCRIBE"
			, "DROP"
			, "JOURNAL"
			, "ADD"
			, "RUN"
			, "METRICS"
			, "SALIR"
			, NULL };

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = character_names[list_index++])) {
		if (strncmp(name, text, len) == 0) {
			return strdup(name);
		}
	}
	return NULL;
}

void imprimir_pcb(t_pcb* pcb) {

	log_info(logger, "Proceso N°: %d.", pcb->id_proceso);
	log_info(logger, "Ruta archivo: %s.", pcb->ruta_archivo);
	log_info(logger, "Program Counter: %d.", pcb->program_counter);
	log_info(logger, "Cantidad request: %d.", pcb->cantidad_request);
	log_info(logger, "Script: %s.", pcb->script);

}


char* criterio_str[] = {
		"SC"
		, "SHC"
		, "EV"
		, NULL
};

t_tipoCriterio criterio_to_enum(char *sval) {
	t_tipoCriterio result = SC;
	int i = 0;
	for (i = 0; criterio_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, criterio_str[i]))
			return result;
	return -1;
}

