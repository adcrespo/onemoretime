/*
 * consola.c
 *
 *  Created on: 20 oct. 2018
 *      Author: utnso
 */

#include "MEM.h"

#include "commons/string.h"
#include "error.h"
#include "memory.h"
#include "consola.h"

t_tipoComando buscar_enum(char *sval) {
	t_tipoComando result = select_;
	int i = 0;
	char* comandos_str[] = { "select", "insert", "create", "describe", "drop", "journal", "dump", "salir", NULL };
	if (sval == NULL)
		return -2;
	for (i = 0; comandos_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, comandos_str[i]))
			return result;
	return -1;
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "select", "insert", "create", "describe", "drop", "journal", "dump", "salir", NULL };

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

char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

void *crearConsola() {

	char * line;
	rl_attempted_completion_function = character_name_completion;

	while (1) {
		line = readline("Ingrese un comando> ");
		if (line) {
			add_history(line);
		}

		char** comando = string_split(line, " ");

		t_tipoComando comando_e = buscar_enum(comando[0]);

		if(comando_e == -2)
			continue;

		switch (comando_e) {
		case select_:
			if (comando[1] == NULL || comando[2] == NULL) {
				printf("error: select {tabla} {key}.\n");
				break;
			}
			printf("select...\n");
			//TODO: select_
			break;
		case insert:
			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL) {
				printf("error: insert {tabla} {key} {\"value\"}.\n");
				break;
			}
			printf("insert...\n");
			//TODO: insert
			break;
		case create:
			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL || comando[4] == NULL) {
				printf("error: create {tabla} {tipo_consistencia} {numero_particiones} {compaction_time}.\n");
				break;
			}
			printf("create...\n");
			//TODO: create
			break;
		case describe:
			if (comando[1] == NULL ) {
				printf("error: describe {tabla} .\n");
				break;
			}
			printf("describe...\n");
			//TODO: describe
			break;
		case drop:
			if (comando[1] == NULL ) {
				printf("error: drop {tabla} .\n");
				break;
			}
			printf("drop...\n");
			//TODO: drop
			break;
		case journal:
			printf("journal...\n");
			//TODO: journal
			break;
		case dump:
			if (comando[1] == NULL) {
				printf("error: dump {id}.\n");
				break;
			}
			printf("Dump (process id: %s)...\n",comando[1]);
			int id_proceso = atoi(comando[1]);
			dump_memory_spa(id_proceso);
			break;
		case salir:
			exit_gracefully(EXIT_SUCCESS);
			break;
		default:
			printf("No se reconoce el comando %s .\n", comando[0]);
		}
	}
	return 0;
}

void crearHiloConsola() {

	sigset_t set;
	int s;
	int hilo_consola;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_consola = pthread_create(&consola, NULL, crearConsola, (void *) &set);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola.");
	}
	log_info(logger, "Se generó el hilo para la consola.");
}
