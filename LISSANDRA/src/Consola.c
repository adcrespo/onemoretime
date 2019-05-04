/*
 * Consola.c
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */

#include "Consola.h"

void *crear_consola(){
	printf("Creando consola...\n");

	char *linea;

	while(1){
		linea = readline(">");

		if(linea)
			add_history(linea);
	}

}
