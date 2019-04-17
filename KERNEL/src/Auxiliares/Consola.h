/*
 * Consola.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_CONSOLA_H_
#define AUXILIARES_CONSOLA_H_

#include "shared.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "Logueo.h"


typedef enum t_comando_enum {
	_select
	, _insert
	, _create
	, _describe
	, _drop
	, _journal
	, _add
	, _run
	, _metrics
	, _salir
} t_comando_enum;


// Definición de funciones
void *crear_consola();
t_comando_enum string_to_enum(char*);
char* enum_to_string(t_comando_enum);
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

#endif /* AUXILIARES_CONSOLA_H_ */
