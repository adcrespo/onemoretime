/*
 ============================================================================
 Name        : KERNEL.c
 Author      : Osquic
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "KERNEL.h"

int main() {

	/* 1. Logger */
	logger = configurar_logger("../log/KERNEL.log", "Kernel");

	/* 2. Configuración */
	cargar_configuracion_kernel("../config/KERNEL.config");

	/* 3. Inicializar variables */
	inicializar();

	/* 4. Consola */
	crear_hilo_consola();

	/* 5. Inotify */
	crear_hilo_inotify();

	/* 6. Refresh metadata */
	crear_hilo_refresh();

	/* 7. Gossiping */
	init_gossiping();
	crear_hilo_gossiping();

	/* 8. Planificador */
	crear_hilo_planificador();

	pthread_join(thread_consola, NULL);

	puts("Fin proceso Kernel"); /* prints Proceso Kernel */
	return EXIT_SUCCESS;
}

void inicializar() {

	cont_id_procesos = 0;
	memoria_sc = -1;
	lista_new = list_create();
	lista_ready = list_create();
	lista_exec = list_create();
	lista_exit = list_create();
	lista_criterio_shc = list_create();
	lista_criterio_ev = list_create();
	lista_metadata = list_create();
	LISTA_CONN = list_create();

	inicializar_semaforos();
}

void inicializar_semaforos() {

	sem_init(&sem_new, 0, 0);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_exit, 0, 0);

	sem_init(&sem_multiprog, 0, kernel_conf.multiprocesamiento);
}

void aplicar_tiempo_refresh() {
	int segundos_refresh = (kernel_conf.metadata_refresh / 1000);
	log_info(logger, "REFRESH|Iniciando refresh de metadata en %d segundos",
			segundos_refresh);
	sleep(segundos_refresh);
}



void actualizar_metadata() {
	int largo_content = MAX_PATH;
	char *content = malloc(largo_content);

	log_info(logger, "METADATA| Iniciando refresh.");

	memset(content, 0x00, largo_content);
	content[MAX_PATH - 1] = 0x00;

	sleep(kernel_conf.sleep_ejecucion / 1000);
	int cantidad = 1;
	//aca obtiene de criterio EV, hay que cambiar por cualquiera conectada
	t_tipoSeeds *memoria;
	memoria = obtener_memoria_random();

	log_info(logger, "METADATA| Memoria asignada: %d", memoria->numeroMemoria);
	int puerto = atoi(memoria->puerto);

	log_info(logger, "METADATA| Conectando a memoria %d", memoria->numeroMemoria);
	int client_socket = conectar_a_servidor(memoria->ip, puerto, kernel);

	enviarMensaje(kernel, countTables, 0, NULL, client_socket, logger, mem);
	t_mensaje* mensajeCantidad = recibirMensaje(client_socket, logger);
	cantidad = mensajeCantidad->header.error;
	destruirMensaje(mensajeCantidad);
	loggear(logger, LOG_LEVEL_DEBUG, "La cantidad es: %d", cantidad);

	enviarMensaje(kernel, describe, largo_content, content, client_socket,
			logger, mem);
	free(content);
	int longAcum = 0;

	while (cantidad-- > 0) {
		t_mensaje* mensaje = recibirMensaje(client_socket, logger);
		if (mensaje == NULL) {
			loggear(logger, LOG_LEVEL_ERROR,
					"No se pudo recibir mensaje de mem");
			return;
		}
		char* buffer_describe= string_new();
		longAcum += mensaje->header.longitud;
		memcpy(buffer_describe, mensaje->content, mensaje->header.longitud);
		guardar_metadata(buffer_describe);
		destruirMensaje(mensaje);

		loggear(logger, LOG_LEVEL_DEBUG, "Data: %s", *buffer_describe);
	}
}

void guardar_metadata(char *buffer) {
	//nombre_tabla;tipoConsistencia;particiones;compactationTime
	char **elementos;

	elementos = string_split(buffer, ";");

	t_metadata *metadata = malloc(sizeof(t_metadata));
	strcpy(metadata->nombreTabla, elementos[0]);
	strcpy(metadata->tipoConsistencia, elementos[1]);
	metadata->particiones = atoi(elementos[2]);
	metadata->compactationTime = atoi(elementos[3]);
	list_add(lista_metadata, metadata);

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
