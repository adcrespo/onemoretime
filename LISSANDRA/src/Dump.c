/*
 * Dump.c
 *
 *  Created on: 4 jun. 2019
 *      Author: utnso
 */
#include "Dump.h"

void *InicializarDump()
{
	dumpRealizados = 0;
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
	dumpRealizados ++;
	loggear(logger, LOG_LEVEL_INFO, "Realizando dumpeo numero %d", dumpRealizados);

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
	loggear(logger, LOG_LEVEL_INFO,"Longitud tabla %s es %d", nombre, longitudTabla);
	char *temporal = string_from_format("%s%s/%d.tmp", rutaTablas, nombre, dumpRealizados);
	loggear(logger, LOG_LEVEL_INFO,"Creando archivo %s", temporal);
	FILE *file = fopen(temporal, "w+");

	int bloqueActual = AgregarBloque();
	char *rutaActual = string_from_format("%s%d.bin", rutaBloques, bloqueActual);
	int sizeTotal = 0;
	int disponibleActual = tamanio_bloques;
	t_list *bloques = list_create();
	list_add(bloques, bloqueActual);
	//for para recorrer cada tabla dentro de memtable
	for(int j = 0; j < longitudTabla; j++)
	{

		int len = sizeof(t_registro);
		t_registro *registro = malloc(len);
		registro = list_get(lista, j);
		char *linea = string_new();
		char *key = string_itoa(registro->key);
		char *timestamp = string_itoa(registro->timestamp);

		string_append(&linea, key);
		string_append(&linea, ";");
		string_append(&linea, registro->value);
		string_append(&linea, ";");
		string_append(&linea, timestamp);
		string_append(&linea, "\n");
		free(key);
		free(timestamp);
		int lenLinea = strlen(linea);

		if(lenLinea < disponibleActual)
		{
			disponibleActual -= lenLinea;
			GuardarEnBloque(linea, rutaActual);
			sizeTotal += lenLinea;
			loggear(logger, LOG_LEVEL_INFO, "El disponible es: %d", disponibleActual);
		} else
		{
			bloqueActual = AgregarBloque();
			disponibleActual = tamanio_bloques;
			bloqueActual ++;
			disponibleActual -= lenLinea;
			rutaActual = string_from_format("%s%d.bin", rutaBloques, bloqueActual);
			GuardarEnBloque(linea, rutaActual);
			list_add(bloques, bloqueActual);
			sizeTotal += lenLinea;
			loggear(logger, LOG_LEVEL_INFO, "El disponible es: %d", disponibleActual);


		}



		loggear(logger, LOG_LEVEL_WARNING, "Registro key: %d value: %s timestamp %d", registro->key, registro->value, registro->timestamp);

	}
	char *sizeAEscribir = string_from_format("SIZE=%d\n", sizeTotal);
	fputs(sizeAEscribir, file);
	fputs("BLOCKS=[",file);
	int longitudBloques = list_size(bloques);
	for(int i = 0; i < longitudBloques; i++)
	{
		int idBloque = list_get(bloques, i);
		if(i < (longitudBloques - 1))
		{
			fprintf(file, "%d", idBloque);
			fputs(",", file);
		} else
		{
			fprintf(file, "%d", idBloque);
		}

	}

	fputs("]\n",file);
	free(sizeAEscribir);
	list_clean(bloques);
	free(bloques);
	list_clean(lista);
	free(lista);
	fclose(file);

}

void LimpiarMemtable()
{
	list_clean(memtable);
	loggear(logger, LOG_LEVEL_INFO, "Memtable vacia");
}
