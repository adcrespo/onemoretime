/*
 * proceso.c
 *
 *  Created on: 7 jun. 2019
 *      Author: utnso
 */

#include "MEM.h"

#include "proceso.h"
#include "memory.h"
#include "connection.h"
#include "journaling.h"
#include "file_conf.h"

int solicitarPagina(char *tabla, unsigned long long timestamp) {
	int paginaNueva = add_spa(tabla,1,timestamp);
	if(paginaNueva<0) {
		t_adm_tabla_frames_spa frame_reg = getPaginaMenorTimestamp();
		paginaNueva = frame_reg.pagina;
		free(frame_reg.path_tabla);
		if(paginaNueva<0) {
			loggear(logger,LOG_LEVEL_DEBUG,"MEMORY FULL!!");
			return MEMORY_FULL;
			//process_journaling();
			//paginaNueva = add_spa(tabla,1,timestamp);
		}
	}
	if(paginaNueva<0){
		loggear(logger,LOG_LEVEL_ERROR,"Error en solitarPagina: %d", paginaNueva);
		return -1;
	}
	return paginaNueva;
}
int proceso_select(char* tabla, int clave, char** buffer, int* largo_buffer) {
	pthread_mutex_lock(&journalingMutexSelect);
	*largo_buffer = 0;
	int paginaTabla = getPaginaForKey(tabla, clave);
	if(paginaTabla>=0){
		*buffer = leer_bytes_spa(tabla,0,paginaTabla*frame_spa_size,frame_spa_size);
		if(*buffer[0]!=0x00){
			pthread_mutex_unlock(&journalingMutexSelect);
			return 1;
		}
		pthread_mutex_unlock(&journalingMutexSelect);
		return -1;
	}

	int largo_content = MAX_PATH +sizeof(int);
	char *content = malloc(largo_content);
	memset(content, 0x00, largo_content);
	memcpy(content,tabla,strlen(tabla)<MAX_PATH?strlen(tabla):MAX_PATH);
	content[MAX_PATH-1] = 0x00;
	memcpy(content+MAX_PATH,&clave,sizeof(int));

	sleep(MEM_CONF.RETARDO_FS/1000);
	enviarMensaje(mem,selectMsg,largo_content,content,socket_lis,logger,lis);
	t_mensaje* mensaje = recibirMensaje(socket_lis, logger);
	free(content);

	if(mensaje == NULL) {
		loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
		pthread_mutex_unlock(&journalingMutexSelect);
		return -1;
	}

	*largo_buffer = mensaje->header.longitud;
	*buffer = malloc(*largo_buffer);
	memcpy(*buffer, mensaje->content, mensaje->header.longitud);
	destruirMensaje(mensaje);

	t_registro* reg = descomponer_registro(*buffer);
	unsigned long long timestamp = reg->timestamp;
	destruir_registro(reg);

	int paginaNueva = solicitarPagina(tabla, timestamp);
	if(paginaNueva<0){
		free(*buffer);
		loggear(logger,LOG_LEVEL_ERROR,"Error en solicitar pagina: %d", paginaNueva);
		pthread_mutex_unlock(&journalingMutexSelect);
		return paginaNueva;
	}

	int escrito = escribir_bytes_spa(tabla,paginaNueva,0,frame_spa_size,*buffer,0);
	if(escrito<0){
		free(*buffer);
		loggear(logger,LOG_LEVEL_ERROR,"Error en escribir_bytes: %d", escrito);
		pthread_mutex_unlock(&journalingMutexSelect);
		return -1;
	}

	pthread_mutex_unlock(&journalingMutexSelect);
	return 1;
}


int proceso_insert(char* tabla, int clave, char* value, unsigned long long tstamp) {
	pthread_mutex_lock(&journalingMutexInsert);
	int escrito = -1;
	unsigned long long timestamp = tstamp;
	if(timestamp==0)
		timestamp = obtenerTimeStamp();
	char *buffer = componer_registro(timestamp,clave, value, strlen(value));
	int paginaTabla = getPaginaForKey(tabla, clave);
	if(paginaTabla>=0){
		loggear(logger,LOG_LEVEL_INFO,"Encontre la pagina en memoria (%d)",paginaTabla);
		escrito = escribir_bytes_spa(tabla,paginaTabla,0,frame_spa_size,buffer,1);
		if(escrito<0)
			loggear(logger,LOG_LEVEL_ERROR,"Error en escribir_bytes: %d", escrito);
		free(buffer);
		pthread_mutex_unlock(&journalingMutexInsert);
		return escrito;
	}

	int paginaNueva = solicitarPagina(tabla, timestamp);
	if(paginaNueva<0){
		free(buffer);
		loggear(logger,LOG_LEVEL_ERROR,"Error en escribir_bytes: %d", paginaNueva);
		pthread_mutex_unlock(&journalingMutexInsert);
		return paginaNueva;
	}

	escrito = escribir_bytes_spa(tabla,0,paginaNueva*frame_spa_size,frame_spa_size,buffer,1);
	if(escrito<0){
		free(buffer);
		loggear(logger,LOG_LEVEL_ERROR,"Error en escribir_bytes: %d", escrito);
		pthread_mutex_unlock(&journalingMutexInsert);
		return escrito;
	}

	free(buffer);
	pthread_mutex_unlock(&journalingMutexInsert);
	return escrito;
}


int proceso_create(char* tabla,char* tipo_cons, int num_part, int compact_time){
	pthread_mutex_lock(&journalingMutexCreate);
	int creado = -1;
	int largo_content = MAX_PATH + MAX_CONS + sizeof(int) + sizeof(int);
	char *content = malloc(largo_content);

	memset(content, 0x00, largo_content);
	memcpy(content,tabla,strlen(tabla)<MAX_PATH?strlen(tabla):MAX_PATH);
	content[MAX_PATH-1] = 0x00;
	memcpy(content+MAX_PATH,tipo_cons,strlen(tipo_cons));
	memcpy(content+MAX_PATH+MAX_CONS,&num_part,sizeof(int));
	memcpy(content+MAX_PATH+MAX_CONS+sizeof(int),&compact_time,sizeof(int));

	sleep(MEM_CONF.RETARDO_FS/1000);
	enviarMensaje(mem,create,largo_content,content,socket_lis,logger,lis);
	t_mensaje* mensaje = recibirMensaje(socket_lis, logger);

	free(content);

	if(mensaje == NULL) {
		loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
		pthread_mutex_unlock(&journalingMutexCreate);
		return -1;
	}
	creado = mensaje->header.error;
	destruirMensaje(mensaje);

	pthread_mutex_unlock(&journalingMutexCreate);
	return creado;
}


int proceso_describe(char* tabla, char** buffer, int* largo_buffer){
	pthread_mutex_lock(&journalingMutexDescribe);
	int desc = -1;
	int largo_content = MAX_PATH;
	char *content = malloc(largo_content);

	memset(content, 0x00, largo_content);
	memcpy(content,tabla,strlen(tabla)<MAX_PATH?strlen(tabla):MAX_PATH);
	content[MAX_PATH-1] = 0x00;

	sleep(MEM_CONF.RETARDO_FS/1000);
	int cantidad = 1;

	if(string_is_empty(tabla)){
		enviarMensaje(mem,countTables,0,NULL,socket_lis,logger,lis);
		t_mensaje* mensajeCantidad = recibirMensaje(socket_lis, logger);
		cantidad=mensajeCantidad->header.error;
		destruirMensaje(mensajeCantidad);
		loggear(logger,LOG_LEVEL_DEBUG,"La cantidad es: %d",cantidad);
	}

	enviarMensaje(mem,describe,largo_content,content,socket_lis,logger,lis);
	free(content);
	int longAcum = 0;
	while(cantidad-->0)
	{
		t_mensaje* mensaje = recibirMensaje(socket_lis, logger);
		if(mensaje == NULL) {
			loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
			pthread_mutex_unlock(&journalingMutexDescribe);
			return -1;
		}
		desc = mensaje->header.error;
		longAcum += mensaje->header.longitud;
		*buffer = realloc(*buffer,longAcum);
		memset(*buffer,0x00,mensaje->header.longitud);
		memcpy(*buffer,mensaje->content,mensaje->header.longitud);
		*largo_buffer = mensaje->header.longitud;
		destruirMensaje(mensaje);

		loggear(logger,LOG_LEVEL_DEBUG,"Data: %s",*buffer);
	}
	pthread_mutex_unlock(&journalingMutexDescribe);
	return desc;
}

int proceso_drop(char* tabla){
	pthread_mutex_lock(&journalingMutexDrop);
	int dropRes = -1;

	int i;
	int totalPaginas = getSizePagesForTable(tabla);

	if(totalPaginas<0){
		loggear(logger,LOG_LEVEL_ERROR,"No se pudo obtener la cantidad de paginas");
		pthread_mutex_unlock(&journalingMutexDrop);
		return -1;
	}

	for(i=0;i<totalPaginas;i++) {
		free_spa(tabla,0);
	}

	int largo_content = MAX_PATH;
	char *content = malloc(largo_content);

	memset(content, 0x00, largo_content);
	memcpy(content,tabla,strlen(tabla)<MAX_PATH?strlen(tabla):MAX_PATH);
	content[MAX_PATH-1] = 0x00;

	sleep(MEM_CONF.RETARDO_FS/1000);
	enviarMensaje(mem,drop,largo_content,content,socket_lis,logger,lis);
	t_mensaje* mensaje = recibirMensaje(socket_lis, logger);

	free(content);

	if(mensaje == NULL) {
		loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
		pthread_mutex_unlock(&journalingMutexDrop);
		return -1;
	}
	dropRes = mensaje->header.error;
	destruirMensaje(mensaje);

	pthread_mutex_unlock(&journalingMutexDrop);
	return dropRes;
}


int proceso_journal(){
	return process_journaling();
}
