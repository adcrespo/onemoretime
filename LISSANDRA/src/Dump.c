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
}
