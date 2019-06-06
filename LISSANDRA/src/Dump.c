/*
 * Dump.c
 *
 *  Created on: 4 jun. 2019
 *      Author: utnso
 */
#include "Dump.h"

void *InicializarDump()
{

	AplicarTiempoDump();
	RealizarDumpeo();
	LimpiarMemtable();
	return 0;
}

void AplicarTiempoDump()
{
	int segundosDump = (lfs_conf.tiempo_dump/1000);
	loggear(logger, LOG_LEVEL_INFO, "El dumpeo se realizará en %d segundos", segundosDump);
	sleep(segundosDump);

}

void RealizarDumpeo()
{
	loggear(logger, LOG_LEVEL_INFO, "Realizando dumpeo");

	int longitudMemtable = list_size(memtable);

	//for para recorrer memtable
	for(int i = 0; i < longitudMemtable; i++)
	{
		t_tabla *listaTabla = malloc(sizeof(t_tabla));
		listaTabla = list_get(memtable, i);

		if(listaTabla != NULL)
		{

			char *nombre = string_new();
			strcpy(nombre, listaTabla->nombre_tabla);
			DumpearTabla(listaTabla->lista, nombre);

		}

	}

	loggear(logger, LOG_LEVEL_INFO, "Dumpeo terminado");

}


void DumpearTabla(t_list *lista, char *nombre)
{
	loggear(logger, LOG_LEVEL_INFO, "Dumpeando tabla: %s", nombre);

	int longitudTabla = list_size(lista);
	//for para recorrer cada tabla dentro de memtable
	for(int j = 0; j < longitudTabla; j++)
	{

	}
}

void LimpiarMemtable()
{
	list_clean(memtable);
	loggear(logger, LOG_LEVEL_INFO, "Memtable vacia");
}
