/*
 * Metadata.h
 *
 *  Created on: 20 jul. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_METADATA_H_
#define AUXILIARES_METADATA_H_

#include "../KERNEL.h"

void aplicar_tiempo_refresh();
t_metadata* buscar_tabla(char *nombre);
void actualizar_metadata();
void guardar_metadata(char *buffer);
//t_metadata* validar_metadata(char *nombre);
void describe_global(int cliente);
int validar_tabla(char *nombre);
t_metadata* buscar_tabla(char *nombre);


#endif /* AUXILIARES_METADATA_H_ */
