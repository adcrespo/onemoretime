/*
 * Dump.c
 *
 *  Created on: 4 jun. 2019
 *      Author: utnso
 */
#include "Dump.h"

void *InicializarDump() {
	dumpRealizados = 0;
	while (1) {
		AplicarTiempoDump();
		RealizarDumpeo();
		LimpiarMemtable();
	}
	return 0;
}

void AplicarTiempoDump()
{
	int segundosDump = (lfs_conf.tiempo_dump/1000);
	log_debug(logger, "El dumpeo se realizará en %d segundos", segundosDump);
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
			string_append(&nombre, listaTabla->nombre_tabla);
			DumpearTabla(listaTabla->lista, nombre);
			AumentarContadorTmp(nombre);
		}

	}

	log_debug(logger, "Dumpeo %d terminado", dumpRealizados);

}


void DumpearTabla(t_list *lista, char *nombre)
{
	loggear(logger, LOG_LEVEL_INFO, "Dumpeando tabla: %s", nombre);
	int numeroDump = GetContadorTmp(nombre);
	int longitudTabla = list_size(lista);
	loggear(logger, LOG_LEVEL_INFO,"Longitud tabla %s es %d", nombre, longitudTabla);
	char *temporal = string_from_format("%s%s/%d.tmp", rutaTablas, nombre, numeroDump);
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
		//char *timestamp = string_new();
		char *timestamp = malloc(20);
		sprintf(timestamp, "%llu", registro->timestamp);
		string_append(&linea, timestamp);
		string_append(&linea, ";");
		string_append(&linea, key);
		string_append(&linea, ";");
		string_append(&linea, registro->value);
		string_append(&linea, "\n");
		free(key);
		free(timestamp);
		int lenLinea = strlen(linea);
		log_info(logger, "strlen de linea %s es %d", linea, lenLinea);

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
			//bloqueActual ++;
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

void LimpiarMemtable() {
	int sizeMemTable = list_size(memtable);
	//recorro memtable
	for (int i = 0; i < sizeMemTable; i++) {
		t_tabla *tabla = list_get(memtable, i);
		int sizeTabla = list_size(tabla->lista);
		//recorro tabla dentro de memtable
		for (int j = 0; j < sizeTabla; j++) {
			t_registro *registro = list_get(memtable, i);
			free(registro);
		}
		free(tabla);
	}
	list_clean(memtable);
	loggear(logger, LOG_LEVEL_INFO, "Memtable vacia");
}

void AumentarContadorTmp(char *nombre){

	bool findTable(void* element) {
		t_tcb* tabla = element;
		return string_equals_ignore_case(tabla->nombre_tabla, nombre);
	}

	t_tcb* tcbBusqueda = list_find(tablasGlobal, &findTable);
	tcbBusqueda->contadorTmp ++;
//	log_info(logger, "contador tabla %s vale %d", tcbBusqueda->nombre_tabla, tcbBusqueda->contadorTmp);
}

int GetContadorTmp(char *nombre){

	bool findTable(void* element) {
		t_tcb* tabla = element;
		return string_equals_ignore_case(tabla->nombre_tabla, nombre);
	}

	t_tcb* tcbBusqueda = list_find(tablasGlobal, &findTable);
	log_info(logger, "Contador tabla %s es %d", nombre, tcbBusqueda->contadorTmp);
	return tcbBusqueda->contadorTmp;
}

void ReiniciarContadorTmp(char *nombre){
	bool findTable(void* element) {
		t_tcb* tabla = element;
		return string_equals_ignore_case(tabla->nombre_tabla, nombre);
	}

	t_tcb* tcbBusqueda = list_find(tablasGlobal, &findTable);
	tcbBusqueda->contadorTmp = 1;
//	log_info(logger, "contador tabla %s vale %d", tcbBusqueda->nombre_tabla, tcbBusqueda->contadorTmp);
}

