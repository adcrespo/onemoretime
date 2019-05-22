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
							break;

							/*La operación Insert permite la creación y/o actualización del valor de una key dentro de una tabla.
							 * Para esto, se utiliza la siguiente nomenclatura:

							INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]

							Ejemplos:
							INSERT TABLA1 3 “Mi nombre es Lissandra” 1548421507

							Ó
							INSERT TABLA1 3 “Mi nombre es Lissandra” 1548421507


							Esta operación incluye los siguientes pasos:
							-Verificar que la tabla exista en el file system. En caso que no exista, informa el error y continúa su ejecución.
							-Obtener la metadata asociada a dicha tabla.
							-Verificar si existe en memoria una lista de datos a dumpear. De no existir, alocar dicha memoria.
							-El parámetro Timestamp es opcional. En caso que un request no lo provea (por ejemplo insertando un valor desde la consola), se usará el valor actual del Epoch UNIX.
							-Insertar en la memoria temporal del punto anterior una nueva entrada que contenga los datos enviados en la request.

							*/

						case _create:



							printf("CONSOLA: Se ingresó comando CREATE \n");
							break;

							/*
							 La operación Create permite la creación de una nueva tabla dentro del file system.
							  Para esto, se utiliza la siguiente nomenclatura:

							 CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]

							 Ej:
							 CREATE TABLA1 SC 4 60000

							 Esta operación incluye los siguientes pasos:
							 -Verificar que la tabla no exista en el file system.
							 	 Por convención, una tabla existe si ya hay otra con el mismo nombre.
							 	 Para dichos nombres de las tablas siempre tomaremos sus valores en UPPERCASE (mayúsculas).
							 	 En caso que exista, se guardará el resultado en un archivo .log y se retorna un error indicando dicho resultado.
							 -Crear el directorio para dicha tabla.
							 -Crear el archivo Metadata asociado al mismo.
							 -Grabar en dicho archivo los parámetros pasados por el request.
							 -Crear los archivos binarios asociados a cada partición de la tabla
							 	 y asignar a cada uno un bloque
							 */

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






