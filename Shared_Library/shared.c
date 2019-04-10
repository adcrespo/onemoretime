/*
 * shared.c
 *
 *  Created on: 22 sep. 2018
 *      Author: utnso
 */

#include "shared.h"

#define CONEXIONES_MAXIMAS 10

pthread_mutex_t lock_logger;

t_log* configurar_logger(char* nombreLog, char* nombreProceso) {
	t_log* logger = log_create(nombreLog, nombreProceso, false, LOG_LEVEL_INFO);
	log_info(logger, "*************** NUEVO LOG ***************");
	return logger;
}

//t_log* configurar_logger(char* nombreLog, char* nombreProceso) {
//	return configurar_logger_verbose(nombreLog, nombreProceso, true);
//}

t_log* configurar_logger_verbose(char* nombreLog, char* nombreProceso, bool verbose) {
	if (pthread_mutex_init(&lock_logger, NULL) != 0)
		return NULL;
	t_log* logger = log_create(nombreLog, nombreProceso, verbose, LOG_LEVEL_TRACE);
	pthread_mutex_lock(&lock_logger);
	log_info(logger, "*************** NUEVO LOG ***************");
	pthread_mutex_unlock(&lock_logger);
	return logger;
}

void loggear(t_log* logger, t_log_level level, const char* message_format, ...) {

	va_list arguments;
	va_start(arguments, message_format);

	char *message;

	message = string_from_vformat(message_format, arguments);

	switch (level) {
	case LOG_LEVEL_TRACE:
		;
		pthread_mutex_lock(&lock_logger);
		log_trace(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_DEBUG:
		;
		pthread_mutex_lock(&lock_logger);
		log_debug(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_INFO:
		;
		pthread_mutex_lock(&lock_logger);
		log_info(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_WARNING:
		;
		pthread_mutex_lock(&lock_logger);
		log_warning(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	case LOG_LEVEL_ERROR:
		;
		pthread_mutex_lock(&lock_logger);
		log_error(logger, message);
		pthread_mutex_unlock(&lock_logger);
		break;
	default:
		break;
	}

	free(message);
	va_end(arguments);
}
void destruir_logger(t_log* logger) {

	if(logger!=NULL)
		log_destroy(logger);
	pthread_mutex_destroy(&lock_logger);
}

t_config* cargarConfiguracion(char *nombreArchivo, t_log* logger) {
	t_config* config = config_create(nombreArchivo);
	if (!config) {
		loggear(logger, LOG_LEVEL_ERROR,
				"No se pudo abrir el archivo de configuración.");
		return NULL;
	}
	loggear(logger, LOG_LEVEL_INFO,
			"Se abrió el archivo de configuración correctamente.");
	return config;
}

void remove_quotes(char *line) {
	int j = 0;
	for (int i = 0; i < strlen(line); i++) {
		if (line[i] != '"' && line[i] != '\\') {
			line[j++] = line[i];
		} else if (line[i + 1] == '"' && line[i] == '\\') {
			line[j++] = '"';
		} else if (line[i + 1] != '"' && line[i] == '\\') {
			line[j++] = '\\';
		}
	}
	if (j > 0)
		line[j] = 0;
}

void destroy_config(t_config* config) {

	if(config!=NULL)
		config_destroy(config);
}

int definirSocket(t_log* logger) {

	int nuevoSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (nuevoSocket == -1) {
		loggear(logger, LOG_LEVEL_ERROR, "No se pudo abrir el socket.");
		return -1;
	}
	loggear(logger, LOG_LEVEL_INFO, "Socket creado correctamente.");
	return nuevoSocket;
}

int bindearSocketYEscuchar(int socket, char *ip, int puerto, t_log* logger) {
	struct sockaddr_in direccion;
	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = htonl(INADDR_ANY);
	direccion.sin_port = htons(puerto);
	memset(&(direccion.sin_zero), '\0', 8);

	/* Bindeo */
	if (bind(socket, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
		loggear(logger, LOG_LEVEL_ERROR, "No se pudo bindear el socket.");
		return -1;
	}
	loggear(logger, LOG_LEVEL_INFO, "Socket bindeado correctamente.");

	/* Escucha */
	if (listen(socket, CONEXIONES_MAXIMAS) < 0) {
		loggear(logger, LOG_LEVEL_ERROR, "No se puede escuchar conexiones.");
		return 0;
	}

	log_info(logger, "Escuchando conexiones.");
	return 1;
}

int aceptarConexiones(int socket, t_log* logger) {
	int client_socket;
	struct sockaddr_in server_address;
	unsigned int len = sizeof(struct sockaddr);

	if ((client_socket = accept(socket, (struct sockaddr*) &server_address,
			&len)) == -1) {
		loggear(logger, LOG_LEVEL_ERROR, "No se puede aceptar conexiones.");
		return -1;
	}
	loggear(logger, LOG_LEVEL_INFO, "Conexion aceptada (sock_id: %d)",
			client_socket);
	return client_socket;
}

int conectarseAServidor(int socket, char* ip, int puerto, t_log* logger) {

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_port = htons(puerto);
	direccionServidor.sin_addr.s_addr = inet_addr(ip);

	if (connect(socket, (void *) &direccionServidor, sizeof(direccionServidor))
			!= 0) {
		loggear(logger, LOG_LEVEL_ERROR, "No se pudo conectar al Servidor.");
		return 0;
	}

	loggear(logger, LOG_LEVEL_INFO, "Conectado al Servidor correctamente.");
	return 1;
}

int enviarMensaje(int tipoProcesoEmisor, int tipoMensaje, int len, void* content,
		int socketReceptor, t_log* logger, int tipoProcesoReceptor) {

	void* buffer = serializar(tipoProcesoEmisor, tipoMensaje, len, content);

	if (len > 0 && content != NULL)
		if (buffer == NULL) {
			loggear(logger, LOG_LEVEL_ERROR,
					"No se pudo serializar mensaje (proceso emisor %d).", tipoProcesoEmisor);
			return -1;
		}

	if (send(socketReceptor, buffer, sizeof(t_header) + len, 0) <= 0) {
		free(buffer);
		loggear(logger, LOG_LEVEL_ERROR,
				"No se pudo enviar mensaje (proceso emisor %d - proceso receptor %d).",
				tipoProcesoEmisor, tipoProcesoReceptor);
		return 0;
	}
	/* Cambiar esto porque no está bien */
	loggear(logger, LOG_LEVEL_INFO, "Se envió mensaje (proceso emisor %d - proceso receptor %d).",
			tipoProcesoEmisor, tipoProcesoReceptor);

	free(buffer); // Sirver para t_mensaje
	return 1;
}

t_mensaje* recibirMensaje(int socketEmisor, t_log* logger) {

	void* buffer = malloc(sizeof(t_header));

	if (recv(socketEmisor, buffer, sizeof(t_header), 0) <= 0) {
		free(buffer);
		loggear(logger, LOG_LEVEL_ERROR,
				"Se desconecto o ocurrio un error con el socket (sock_id: %d).",
				socketEmisor);
		return NULL;
	}

	t_mensaje* msg = deserializar(buffer);

	msg->content = NULL;

	if (msg->header.longitud > 0) {
		//buffer = calloc(sizeof(char), msg->header.longitud + 1);
		void* c_buffer = malloc(msg->header.longitud + 1);
		if (recv(socketEmisor, c_buffer, msg->header.longitud, MSG_WAITALL) <= 0) {
			free(c_buffer);
			loggear(logger, LOG_LEVEL_ERROR, "Se desconecto o ocurrio un error con el socket (sock_id: %d).", socketEmisor);
			return NULL;
	}

		msg->content = c_buffer;

	}

	return msg;
}

void destruirMensaje(t_mensaje* msg) {

	if(msg == NULL)
		return;
	if (msg != NULL && msg->header.longitud > 0 && msg->content != NULL)
		free(msg->content);
	if(msg != NULL)
		free(msg);
}

void destruirBuffer(void* buffer) {
	if (buffer != NULL)
		free(buffer);
}

/* Auxiliares */
void* serializar(int tipoProceso, int tipoMensaje, int len, void* content) {

	t_mensaje mensaje;
	mensaje.header.tipoProceso = tipoProceso;
	mensaje.header.tipoMensaje = tipoMensaje;
	mensaje.header.longitud = len;
	mensaje.content = content;

	void* buffer = malloc(sizeof(t_header) + len);
	memcpy(buffer, &(mensaje.header), sizeof(t_header));
	memcpy(buffer + sizeof(t_header), mensaje.content, len);

	return buffer;
}

t_mensaje* deserializar(void* buffer) {
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
	memcpy(&(mensaje->header), buffer, sizeof(t_header));

	free(buffer);

	return mensaje;
}
