/*
 * Consola.h
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_CONSOLA_H_
#define AUXILIARES_CONSOLA_H_

#include "../KERNEL.h"
#include <readline/readline.h>
#include <readline/history.h>




typedef enum tipoCriterio {
	SC,SHC,EV
} t_tipoCriterio;


// Definición de funciones
void *crear_consola();
int procesar_comando(char *line);
void abrir_archivo_LQL(t_request* request);
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);
t_tipoCriterio criterio_to_enum(char *sval);
t_tipoSeeds* obtener_memoria_lista(int numero);



#endif /* AUXILIARES_CONSOLA_H_ */
