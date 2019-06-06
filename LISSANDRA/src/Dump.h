/*
 * Dump.h
 *
 *  Created on: 4 jun. 2019
 *      Author: utnso
 */

#ifndef DUMP_H_
#define DUMP_H_

#include "LFS.h"

void *InicializarDump();
void AplicarTiempoDump();
void RealizarDumpeo();
void LimpiarMemtable();
void DumpearTabla(t_list *lista, char *nombre);

#endif /* DUMP_H_ */
