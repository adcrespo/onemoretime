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
#include "journaling.h"
#include "consola.h"
#include "memory.h"
#include "connection.h"

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
		case select_:;
			if (comando[1] == NULL || comando[2] == NULL) {
				printf("error: select {tabla} {key}.\n");
				break;
			}
			printf("select...\n");
			loggear(logger,LOG_LEVEL_INFO,"Buscando la tabla");
			int paginaTabla = getPaginaForKey(comando[1], atoi(comando[2]));
			if(paginaTabla>=0){
				loggear(logger,LOG_LEVEL_INFO,"Encontre la pagina en memoria (%d)",paginaTabla);
				char* buffer = leer_bytes_spa(comando[1],paginaTabla,0,MAX_LINEA);
				t_registro* reg = descomponer_registro(buffer);
				printf("[mem][timestamp:%d][key:%d] %s\n",reg->timestamp, reg->key, reg->value);
				free(buffer);
				destruir_registro(reg);
			}else{
				loggear(logger,LOG_LEVEL_INFO,"Tengo que buscar en lissandra");

				int largo_content = sizeof(int) + strlen(comando[1]) + 1 +sizeof(int);
				int key = atoi(comando[2]);
				void *content = malloc(largo_content);
				memset(content, 0x00, largo_content);
				memcpy(content,&largo_content,sizeof(int));
				memcpy(content+sizeof(int),comando[1],strlen(comando[1])+1);
				memcpy(content+sizeof(int)+strlen(comando[1])+1,&key,sizeof(int));
				enviarMensaje(mem,selectMsg,largo_content,content,socket_lis,logger,lis);
				t_mensaje* mensaje = recibirMensaje(socket_lis, logger);
				if(mensaje!=NULL && mensaje->header.error != 0) {
					char* buffer = malloc(mensaje->header.longitud);
					t_registro* reg = descomponer_registro(buffer);
					printf("[lis][timestamp:%d][key:%d] %s\n",reg->timestamp, reg->key, reg->value);
					int segmentoNuevo = add_spa(comando[1],1,reg->timestamp);
					if(segmentoNuevo<=0) {
						t_adm_tabla_frames_spa frame_reg = getPaginaMenorTimestamp();
						segmentoNuevo = frame_reg.pagina;
						if(segmentoNuevo<=-1) {
							process_journaling();
							segmentoNuevo = add_spa(comando[1],1,reg->timestamp);
						}
					}
					if(segmentoNuevo>0)
						escribir_bytes_spa(comando[1],segmentoNuevo,0,frame_spa_size,buffer,0);
					else
						printf("[ERR] No se pudo grabar el registro \n");

					free(buffer);
					destruir_registro(reg);
				}else
					printf("[ERR] No se pudo conectar a lis \n");
			}
			break;
		case insert_:;
			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL) {
				printf("error: insert {tabla} {key} {\"value\"}.\n");
				break;
			}
			printf("insert...\n");
			//TODO: insert
			break;
		case create_:;
			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL || comando[4] == NULL) {
				printf("error: create {tabla} {tipo_consistencia} {numero_particiones} {compaction_time}.\n");
				break;
			}
			printf("create...\n");
			//TODO: create
			break;
		case describe_:;
			if (comando[1] == NULL ) {
				printf("error: describe {tabla} .\n");
				break;
			}
			printf("describe...\n");
			//TODO: describe
			break;
		case drop_:;
			if (comando[1] == NULL ) {
				printf("error: drop {tabla} .\n");
				break;
			}
			printf("drop...\n");
			//TODO: drop
			break;
		case journal_:;
			printf("journal...\n");
			process_journaling();
			break;
		case dump_:;
			if (comando[1] == NULL) {
				printf("error: dump {id}.\n");
				break;
			}
			printf("Dump (process id: %s)...\n",comando[1]);
			dump_memory_spa(comando[1]);
			break;
		case salir_:;
			exit_gracefully(EXIT_SUCCESS);
			break;
		default:;
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
