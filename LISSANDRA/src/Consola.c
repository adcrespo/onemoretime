/*
 * Consola.c
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */

#include "Consola.h"
#include "parser.h"
#include "parser.c"
#include "commons/string.h"

void *crear_consola(){


	char *line;

	int estado;

	while (1) {
			line = readline("Ingrese un comando> ");
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

				t_request* request = parsear(line, logger);

				if (request->request == -1) {

					log_error(logger, "CONSOLA: Se ingresó un comando desconocido: %s.", line);
					printf("Se ingresó un comando desconocido: %s.\n", line);

				} else {

					switch (request->request) {

						case _select:
							printf("CONSOLA: Se ingresó comando SELECT \n");
							break;

						case _insert:
							printf("CONSOLA: Se ingresó comando INSERT \n");
							break;

						case _create:
							printf("CONSOLA: Se ingresó comando CREATE \n");
							break;

						case _describe:
							printf("CONSOLA: Se ingresó comando DESCRIBE \n");
							break;

						case _drop:
							printf("CONSOLA: Se ingresó comando DROP \n");
							break;

						default:;
					}
				}

				return 0;
}






