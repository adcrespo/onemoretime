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

							/*La operación Select permite la obtención del
							valor de una key dentro de una tabla.

							SELECT [NOMBRE_TABLA] [KEY]

							Ej:
							SELECT TABLA1 3

							Esta operación incluye los siguientes pasos:
							-Verificar que la tabla exista en el file system.
							-Obtener la metadata asociada a dicha tabla.
							-Calcular cual es la partición que contiene dicho KEY.
							-Escanear la partición objetivo, todos los archivos temporales y la memoria temporal de dicha tabla (si existe) buscando la key deseada.
							-Encontradas las entradas para dicha Key, se retorna el valor con el Timestamp más grande.
							*/

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
							break;

							/*
							 La operación Describe permite obtener la Metadata de una tabla en particular o
							 de todas las tablas que el File System tenga.
							 Para esto, se utiliza la siguiente nomenclatura:

							DESCRIBE [NOMBRE_TABLA]

							Ej:
							DESCRIBE
							DESCRIBE TABLA1

							Para el primer caso la operación incluye los siguientes pasos:
							-Recorrer el directorio de árboles de tablas y
								descubrir cuales son las tablas que dispone el sistema.
							-Leer los archivos Metadata de cada tabla.
							-Retornar el contenido de dichos archivos Metadata


							Para el segundo caso la operación incluye los siguientes pasos:
							-Verificar que la tabla exista en el file system.
							-Leer el archivo Metadata de dicha tabla.
							-Retornar el contenido del archivo.

							  */

						case _drop:
							printf("CONSOLA: Se ingresó comando DROP \n");
							break;

							/*La operación Drop permite la eliminación de una tabla del file system.
							 Para esto, se utiliza la siguiente nomenclatura:
								DROP [NOMBRE_TABLA]

								Ej:
								DROP TABLA1


							Esta operación incluye los siguientes pasos:
							-Verificar que la tabla exista en el file system.
							-Eliminar directorio y todos los archivos de dicha tabla.


							*/

						default:;
					}
				}

				return 0;
}






