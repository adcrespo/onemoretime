/*
 * Compactacion.c
 *
 *  Created on: 15 jun. 2019
 *      Author: utnso
 */
#include "LFS.h"

#include "Compactacion.h"

int process_compactacion(char* path_tabla)
{
	int res = 0;
	loggear(logger, LOG_LEVEL_INFO,"Se esta por realizar la compactacion de %s",path_tabla);

	//TODO: listar archivos .tmp, dentro del directorio que le corresponde a la tabla

	//Por cada .tmp:
		//Renombrar a .tmpc
		//Analizar registro por registro y compararlos contra el .bin (en memoria)
			//Si la key no existe -> agregarlo
			//Si la key existe pero el timestamp del .tmp es mas reciente -> agregarlo
			//Si no no hacer nada
		//Realizar la reasignacion de bloques
			//Bloquear la tabla y tomar timestamp
			//Liberar los bloques que contengan el archivo “.tmpc”
			//Liberar los bloques que contengan el archivo “.bin”
			//Solicitar los bloques necesarios para el nuevo archivo “.bin”
			//Grabar los datos en el nuevo archivo “.bin”
			//Desbloquer la tabla y tomar el tiempo cuanto estuvo bloqueada

	return res;
}

void *crearCompactacion(void *pDatos_compactacion) {
	t_datos_compactacion datos_compactacion = *((t_datos_compactacion *) pDatos_compactacion);

	sleep(datos_compactacion.retardo/1000);
	while (1) {
		process_compactacion(datos_compactacion.path_tabla);
		sleep(datos_compactacion.retardo/1000);
	}
}

int crearHiloCompactacion(int retardo, char* path_tabla)
{
	t_datos_compactacion datos_compactacion = {.retardo = retardo, .path_tabla = string_from_format(path_tabla)};

	hilo_compactacion = pthread_create(&compactacion, NULL, crearCompactacion, (void *) &datos_compactacion);

	if (hilo_compactacion == -1)
		loggear(logger,LOG_LEVEL_INFO,"ERROR_HILO_COMPACTACION: %d", hilo_compactacion);
	log_info(logger, "Se generó el hilo para la COMPACTACION.");

	return 1;
}
