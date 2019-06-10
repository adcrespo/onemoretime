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
							printf("Tabla: %s\n", request->parametro1);
							printf("Key: %s\n", request->parametro2);
							printf("Value: %s\n", request->parametro3);
							printf("Timestamp: %s\n", request->parametro4);
							InsertarTabla(request);

							break;



						case _create:

							printf("CONSOLA: Se ingresó comando CREATE \n");
							printf("Tabla: %s\n", request->parametro1);
							printf("Tipo consistencia: %s\n", request->parametro2);
							printf("Numero particiones: %s\n", request->parametro3);
							printf("Compactation time: %s\n", request->parametro4);
							CrearTabla(request);
							break;


						case _describe:
							printf("CONSOLA: Se ingresó comando DESCRIBE \n");

							if(request->parametro1 != NULL)
							{
								printf("Metadata de tabla %s\n", request->parametro1);
								t_metadata *metadata;
								metadata = ObtenerMetadataTabla(request->parametro1);
								int particiones = metadata->particiones;
								int tiempoCompactacion = metadata->compactationTime;
								char *consistencia = string_new();
								strcpy(consistencia, metadata->tipoConsistencia);
								printf("CONSISTENCY=%s\n", consistencia);
								printf("PARTITIONS=%d\n", particiones);
								printf("COMPACTATION_TIME=%d\n", tiempoCompactacion);
								free(consistencia);
								free(metadata);
							}

							break;


						case _drop:
							printf("CONSOLA: Se ingresó comando DROP \n");
							break;

						default:;
					}
				}

				return 0;
}






