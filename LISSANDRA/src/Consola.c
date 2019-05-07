/*
 * Consola.c
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */

#include "Consola.h"



void *crear_consola(){
	printf("Creando consola...\n");

	char *line;

	while (1) {
			line = readline("Ingrese un comando> ");
			if (line) {
				add_history(line);
			}

		}
}





