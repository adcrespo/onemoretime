/*
 ============================================================================
 Name        : LISSANDRA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "LFS.h"

int main(void) {

	logger = configurar_logger("../log/LIS.log", "Lissandra");

	CargarConfigLFS();


	Inicializar();
	CrearFileSystem();
	CrearHiloConsola();
	CrearHiloConexiones();
	CrearHiloDump();

	pthread_join(thread_consola, NULL);
	pthread_join(thread_dump, NULL);

	return EXIT_SUCCESS;
}



void CrearHiloConsola() {

	int hilo_consola = pthread_create(&thread_consola, NULL, crear_consola, NULL);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola");
	}
	log_info(logger, "Se generó el hilo para la consola");
}

void CrearHiloConexiones() {

	int hilo_conexiones = pthread_create(&thread_conexiones, NULL, listen_connexions, NULL);
		if (hilo_conexiones == -1) {
			log_error(logger, "No se pudo generar el hilo para las conexiones");
		}
		log_info(logger, "Se generó el hilo para las conexiones");
}

void CrearHiloDump() {

	int hiloDump = pthread_create(&thread_dump, NULL, InicializarDump, NULL);
	if (hiloDump == -1) {
		log_error(logger, "No se pudo generar el hilo para proceso dump");
	}
	log_info(logger, "Se generó el hilo para el dump");
}

void Inicializar()
{
	memtable = list_create();
}
