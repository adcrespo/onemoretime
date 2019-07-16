/*
 * Consola.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_CONSOLA_H_
#define AUXILIARES_CONSOLA_H_

#include "shared.h"
#include "parser.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "Logueo.h"
#include "Planificador.h"
#include "../KERNEL.h"


typedef enum tipoCriterio {
	SC,SHC,EV
} t_tipoCriterio;


// Definición de funciones
void *crear_consola();
int procesar_comando(char *line);
void abrir_archivo_LQL(t_request* request);
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);
void imprimir_pcb(t_pcb* pcb);
t_tipoCriterio criterio_to_enum(char *sval);


#endif /* AUXILIARES_CONSOLA_H_ */
