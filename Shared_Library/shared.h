/*
 * shared.h
 *
 *  Created on: 22 sep. 2018
 *      Author: utnso
 */

#ifndef SHARED_H_
#define SHARED_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // Para close
#include <stdint.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>


/* Estructuras de Hackers */
typedef enum tipoMensaje {
	handshake
}t_tipoMensaje;

typedef enum tipoProceso {
	lis, mem, knl
} t_tipoProceso;

typedef struct {
	t_tipoProceso 	tipoProceso;
	t_tipoMensaje 	tipoMensaje;
	int32_t 		longitud;
}__attribute__((packed)) t_header;

typedef struct {
	t_header 		header;
	void* 			content;
}__attribute__((packed)) t_mensaje;


//typedef struct {
//	int id_proceso;
//	char* ruta_archivo;
//	int program_counter; // no se que onda esto ja
//	int iniciado; // esto es un flag, se podría usar un semáforo también.
//	char* tabla_archivos_abiertos; // de momento lo pongo como un char* pero seguro va a array
//}__attribute__((packed)) t_dtb;

typedef struct {
	int32_t id_proceso;
	char ruta_archivo[100];
	int32_t program_counter; // no se que onda esto ja
	int32_t iniciado; // esto es un flag, se podría usar un semáforo también.
	int32_t tabla_archivos_abiertos[50]; // de momento lo pongo como un char* pero seguro va a array
}__attribute__((packed)) t_dtb;

typedef struct{
	int32_t id_proceso;
	char ruta_archivo[100];
}__attribute__((packed)) t_idRuta;


//Structs protocolo CPU-DAM-MDJ


typedef struct {
	char path[50];
	int cantBytes;
} t_newFile;

typedef struct {
	char path[50];
	int offset;
	int size;
} t_dataFile;

typedef struct {
	char path[50];
	int offset;
	int size;
	void *buffer;
} t_saveFile;

/* Definición */

/*funciones de log*/
t_log* configurar_logger(char* nombreLog, char* nombreProceso);
t_log* configurar_logger_verbose(char* nombreLog, char* nombreProceso, bool verbose);
void loggear(t_log* logger, t_log_level level, const char* message_format, ...);
void destruir_logger(t_log* logger);

/*funciones de configuracion*/
t_config* cargarConfiguracion(char *nombreArchivo, t_log* logger);
void remove_quotes(char *line);
void destroy_config(t_config* config);

/* funciones de socket */
int definirSocket(t_log* logger);
int bindearSocketYEscuchar(int socket, char *ip, int puerto, t_log* logger);
int aceptarConexiones(int socket, t_log* logger);
int conectarseAServidor(int socket, char* ip, int puerto, t_log* logger);
int enviarMensaje(int tipoProcesoEmisor, int tipoMensaje, int len, void* content, int socketReceptor, t_log* logger, int tipoProcesoReceptor);
t_mensaje* recibirMensaje(int socketEmisor, t_log* logger);
void destruirMensaje(t_mensaje* msg);
void destruirBuffer(void* buffer);

/* Auxiliares */
void* serializar(int tipoProceso, int tipoMensaje, int len, void* content);
t_mensaje* deserializar(void* buffer);

#endif /* SHARED_H_ */
