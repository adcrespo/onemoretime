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
				printf("Generando nuevo proceso.\n");
				t_pcb* proceso_nuevo = crear_proceso(linea_auxiliar, request);

				printf("Proceso generado.\n");
				printf("ID: %d .\n", proceso_nuevo->id_proceso);
				printf("Program Counter: %d .\n", proceso_nuevo->program_counter);
				printf("Script: %s .\n", proceso_nuevo->script);

				printf("Validando existencia de Tabla.\n");
				printf("Seleccionando Memoria según Criterio.\n");
				printf("Enviando SELECT a una Memoria.\n");
				break;

			case _insert:
				printf("Validando existencia de Tabla.\n");
				printf("Seleccionando Memoria según Criterio.\n");
				printf("Enviando INSERT a una Memoria.\n");
				break;

			case _create:
				printf("CONSOLA: Se ingresó comando CREATE \n");
				break;

			case _describe:
				printf("Enviando DESCRIBE a una Memoria \n"); /* CAMBIAR SOCKET DE ABAJO */
				// elegir memoria
				//enviarMensaje(kernel, describe, sizeof(request), &request, 10/* socket memoria*/, logger, mem);
				break;

			case _drop:
				printf("Validando existencia de Tabla.\n");
				printf("Seleccionando Memoria según Criterio.\n");
				printf("Enviando DROP a una Memoria.\n");
				break;

			case _journal:
				printf("CONSOLA: Se ingresó comando JOURNAL \n");
				break;

			case _add:
				printf("CONSOLA: Se ingresó comando ADD \n");
				break;

			case _run:
				printf("CONSOLA: Se ingresó comando RUN \n");
				printf("Parámetro: %s \n", request->parametro1);

				generar_nuevo_proceso(request);
				abrir_archivo_LQL(request);
				break;

			case _metrics:
				printf("CONSOLA: Se ingresó comando METRICS \n");
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
