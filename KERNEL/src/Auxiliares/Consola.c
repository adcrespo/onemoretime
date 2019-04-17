/*
 * Consola.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#include "Consola.h"

char* comandos_str[] = {
		"select"
		, "insert"
		, "create"
		, "describe"
		, "drop"
		, "journal"
		, "add"
		, "run"
		, "metrics"
		, "salir"
		, NULL };

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

	char* comando = strtok(line, " ");
	char* parametro;

	t_comando_enum comando_e = string_to_enum(comando);

	if (comando_e == -1) {
		log_error(logger, "CONSOLA: Se ingresó un comando desconocido: %s.", comando);
	} else {
		log_info(logger, "CONSOLA: Se ejecutó el comando %s.", enum_to_string(comando_e));
	}

	switch (comando_e) {

		case _salir:
			puts("Muchas gracias por utilizar el proceso SAFA. Vuelva pronto!\n");
			return -1;

		case _select:
			break;

		case _insert:
			break;

		case _create:
			break;

		case _describe:
			break;

		case _drop:
			break;

		case _journal:
			break;

		case _add:
			break;

		case _run:
			break;

		case _metrics:
			break;

		default:
			printf("No se reconoce el comando %s .\n", comando);
	}

	printf("Se ingresó el comando: %s. \n", comando);

	return 0;
}


// Auxiliares
t_comando_enum string_to_enum(char *sval) {
	t_comando_enum result = _select;
	int i = 0;
	for (i = 0; comandos_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, comandos_str[i]))
			return result;
	return -1;
}

char* enum_to_string(t_comando_enum comando) {
	return comandos_str[comando];
}

char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "select"
			, "insert"
			, "create"
			, "describe"
			, "drop"
			, "journal"
			, "add"
			, "run"
			, "metrics"
			, "salir"
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
