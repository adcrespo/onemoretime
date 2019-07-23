/*
 * Metadata.c
 *
 *  Created on: 20 jul. 2019
 *      Author: utnso
 */
#include "Metadata.h"

void aplicar_tiempo_refresh() {

	sleep(kernel_conf.metadata_refresh / 1000);
}

void actualizar_metadata() {

	log_info(logger, "REFRESH| Iniciando.");

	t_tipoSeeds *memoria = get_memoria_conectada();

	log_info(logger, "REFRESH| Memoria asignada: %d", memoria->numeroMemoria);
	int cliente = conectar_a_memoria(memoria);

	// Solicitud
	describe_global(cliente);
	log_info(logger, "REFRESH| Finalizado.");
}

void guardar_metadata(char *buffer) {
	//nombre_tabla;tipoConsistencia;particiones;compactationTime
	// PERSONAS;SHC;5;1000

	log_info(logger, "REFRESH| Guardo Metadata");
	char **elementos = string_split(buffer, ";");

	t_metadata *metadata = malloc(sizeof(t_metadata));
	strcpy(metadata->nombreTabla, elementos[0]);
	strcpy(metadata->tipoConsistencia, elementos[1]);
	metadata->particiones = atoi(elementos[2]);
	metadata->compactationTime = atoi(elementos[3]);
	list_add(lista_metadata, metadata);

	log_info(logger, "REFRESH| Cantidad de Metadatas: %d", lista_metadata->elements_count);

	int i = 0;
	while (elementos[i] != NULL) {
		free(elementos[i]);
		i++;
	}
	free(elementos);

}

void limpiar_metadata() {
	if (lista_metadata != NULL) {
		int size_metadata = list_size(lista_metadata);
		for (int i = 0; i < size_metadata; i++) {
			t_metadata *metadata = list_get(lista_metadata, i);
			free(metadata);
		}
		list_clean(lista_metadata);
	}
}

void describe_global(int cliente) {
	int cantidad = 0;
	log_info(logger, "METADATA| Inicio de DESCRIBE");
	enviarMensaje(kernel, describe_global_, 0, NULL, cliente, logger, mem);
	t_mensaje* mensajeCantidad = recibirMensaje(cliente, logger);
	cantidad = mensajeCantidad->header.error;
	log_info(logger, "METADATA| La cantidad de tablas es: %d", cantidad);
	destruirMensaje(mensajeCantidad);

	int longAcum = 0;

	while (cantidad-- > 0) {

		t_mensaje* mensaje = recibirMensaje(cliente, logger);
		if (mensaje == NULL) {
			loggear(logger, LOG_LEVEL_ERROR,
					"No se pudo recibir mensaje de mem");
			return;
		}
		log_info(logger, "METADATA| Mensaje recibido: %d", cantidad);
		char* buffer_describe= string_new();
		longAcum += mensaje->header.longitud;
		string_append(&buffer_describe, mensaje->content);

		log_info(logger, "METADATA| Metadata: %s", buffer_describe);

		guardar_metadata(buffer_describe);
		destruirMensaje(mensaje);
		free(buffer_describe);
	}
}

int validar_tabla(char *nombre){
	bool findMd(void* element) {
			t_metadata *metadata = element;
			return string_equals_ignore_case(nombre, metadata->nombreTabla);
		}
	return list_any_satisfy(lista_metadata, &findMd);
}

t_metadata* buscar_tabla(char *nombre) {

	int esLaTabla(t_metadata *tabla) {
		return string_equals_ignore_case(nombre, tabla->nombreTabla);
	}

	loggear(logger, LOG_LEVEL_INFO, "Buscando %s en Metadata", nombre);
	return list_find(lista_metadata, (void*) esLaTabla);
}
