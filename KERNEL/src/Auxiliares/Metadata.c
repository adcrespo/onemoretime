/*
 * Metadata.c
 *
 *  Created on: 20 jul. 2019
 *      Author: utnso
 */
#include "Metadata.h"

void aplicar_tiempo_refresh() {
	int segundos_refresh = (kernel_conf.metadata_refresh / 1000);
//	log_info(logger, "REFRESH|Iniciando refresh de metadata en %d segundos", segundos_refresh);
	sleep(segundos_refresh);
}



void actualizar_metadata() {

	log_info(logger, "METADATA| Iniciando refresh.");

	t_tipoSeeds *memoria = get_memoria_conectada();

	log_info(logger, "METADATA| Memoria asignada: %d", memoria->numeroMemoria);
	int puerto = atoi(memoria->puerto);
	int cliente = conectar_a_servidor(memoria->ip, puerto, mem);

	// Solicitud
	describe_global(cliente);
}

void guardar_metadata(char *buffer) {
	//nombre_tabla;tipoConsistencia;particiones;compactationTime
	// PERSONAS;SHC;5;1000

	log_info(logger, "METADATA| Guardo Metadata");
	char **elementos = string_split(buffer, ";");

	t_metadata *metadata = malloc(sizeof(t_metadata));
	strcpy(metadata->nombreTabla, elementos[0]);
	strcpy(metadata->tipoConsistencia, elementos[1]);
	metadata->particiones = atoi(elementos[2]);
	metadata->compactationTime = atoi(elementos[3]);
	list_add(lista_metadata, metadata);

	log_info(logger, "METADATA| Cantidad de Metadatas: %d", lista_metadata->elements_count);

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
	enviarMensaje(kernel, describe_global, 0, NULL, cliente, logger, mem);
	t_mensaje* mensajeCantidad = recibirMensaje(cliente, logger);
	log_info(logger, "METADATA| Respuesta de DESCRIBE");
	cantidad = mensajeCantidad->header.error;
	log_info(logger, "METADATA| La cantidad de tablas es: %d", cantidad);
	destruirMensaje(mensajeCantidad);

	int longAcum = 0;

	while (cantidad-- > 0) {

		log_info(logger, "METADATA| Empiezo a recibir metadata: %d", cantidad);
		t_mensaje* mensaje = recibirMensaje(cliente, logger);
		if (mensaje == NULL) {
			loggear(logger, LOG_LEVEL_ERROR,
					"No se pudo recibir mensaje de mem");
			return;
		}
		log_info(logger, "METADATA| Mensaje recibido: %d", cantidad);
		char* buffer_describe= string_new();
		longAcum += mensaje->header.longitud;
//		memcpy(buffer_describe, mensaje->content, mensaje->header.longitud);
		string_append(&buffer_describe, mensaje->content);

		log_info(logger, "METADATA| Metadata: %s", buffer_describe);

		guardar_metadata(buffer_describe);
		destruirMensaje(mensaje);

//		loggear(logger, LOG_LEVEL_DEBUG, "Metadata: %s", *buffer_describe);
		free(buffer_describe);
	}
}
