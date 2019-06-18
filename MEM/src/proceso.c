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
	loggear(logger,LOG_LEVEL_INFO,"Busco pagina");
	int paginaNueva = add_spa(tabla,1,timestamp);
	if(paginaNueva<0) {
		t_adm_tabla_frames_spa frame_reg = getPaginaMenorTimestamp();
		paginaNueva = frame_reg.pagina;
		if(paginaNueva<=-1) {
			process_journaling();
			paginaNueva = add_spa(tabla,1,timestamp);
		}
	}
	if(paginaNueva<0){
		loggear(logger,LOG_LEVEL_ERROR,"Error en pagina: %d", paginaNueva);
		return -1;
	}
	return paginaNueva;
}
int proceso_select(char* tabla, int clave, char** buffer) {
	pthread_mutex_lock(&journalingMutexSelect);
	int paginaTabla = getPaginaForKey(tabla, clave);
	if(paginaTabla>=0){
		*buffer = leer_bytes_spa(tabla,0,paginaTabla*frame_spa_size,frame_spa_size);
		if(*buffer[0]!=0x00){
			loggear(logger,LOG_LEVEL_ERROR,"NO encontre la pagina en memoria (%d)",paginaTabla);
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
	content[MAX_PATH] = 0x00;
	memcpy(content+MAX_PATH,&clave,sizeof(int));

	sleep(MEM_CONF.RETARDO_FS/1000);
	enviarMensaje(mem,selectMsg,largo_content,content,socket_lis,logger,lis);
	t_mensaje* mensaje = recibirMensaje(socket_lis, logger);

	if(mensaje == NULL) {
		loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
		pthread_mutex_unlock(&journalingMutexSelect);
		return -1;
	}

	*buffer = malloc(mensaje->header.longitud);
	memcpy(*buffer, mensaje->content, mensaje->header.longitud);
	destruirMensaje(mensaje);

	t_registro* reg = descomponer_registro(*buffer);
	unsigned long long timestamp = reg->timestamp;
	destruir_registro(reg);

	int paginaNueva = solicitarPagina(tabla, timestamp);
	if(paginaNueva<0){
		free(*buffer);
		loggear(logger,LOG_LEVEL_ERROR,"Error en escribir_bytes: %d", paginaNueva);
		pthread_mutex_unlock(&journalingMutexSelect);
		return -1;
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


int proceso_insert(char* tabla, int clave, char* value) {
	pthread_mutex_lock(&journalingMutexInsert);
	int escrito = -1;
	unsigned long long timestamp = obtenerTimeStamp();
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
		return -1;
	}

	escrito = escribir_bytes_spa(tabla,0,paginaNueva*frame_spa_size,frame_spa_size,buffer,1);
	if(escrito<0){
		free(buffer);
		loggear(logger,LOG_LEVEL_ERROR,"Error en escribir_bytes: %d", escrito);
		pthread_mutex_unlock(&journalingMutexInsert);
		return escrito;
	}

	pthread_mutex_unlock(&journalingMutexInsert);
	return escrito;
}


int proceso_create(char* tabla,char* tipo_cons, int num_part, int compact_time){
	pthread_mutex_lock(&journalingMutexCreate);
	int creado = -1;
	int largo_content = MAX_PATH + MAX_CONS + sizeof(int) + sizeof(int);
	char *content = malloc(largo_content);
	tipo_cons[2] = 0;

	memset(content, 0x00, largo_content);
	memcpy(content,tabla,strlen(tabla)<MAX_PATH?strlen(tabla):MAX_PATH);
	content[MAX_PATH] = 0x00;
	memcpy(content+MAX_PATH,tipo_cons,MAX_CONS);
	memcpy(content+MAX_PATH+MAX_CONS,&num_part,sizeof(int));
	memcpy(content+MAX_PATH+MAX_CONS+sizeof(int),&compact_time,sizeof(int));

	sleep(MEM_CONF.RETARDO_FS/1000);
	enviarMensaje(mem,create,largo_content,content,socket_lis,logger,lis);
	t_mensaje* mensaje = recibirMensaje(socket_lis, logger);

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
	int describe = -1;
	int largo_content = MAX_PATH;
	char *content = malloc(largo_content);

	memset(content, 0x00, largo_content);
	memcpy(content,tabla,strlen(tabla)<MAX_PATH?strlen(tabla):MAX_PATH);
	content[MAX_PATH] = 0x00;

	sleep(MEM_CONF.RETARDO_FS/1000);
	enviarMensaje(mem,create,largo_content,content,socket_lis,logger,lis);
	t_mensaje* mensaje = recibirMensaje(socket_lis, logger);

	if(mensaje == NULL) {
		loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
		pthread_mutex_unlock(&journalingMutexDescribe);
		return -1;
	}
	describe = mensaje->header.error;
	*buffer = malloc(mensaje->header.longitud);
	memset(*buffer,0x00,mensaje->header.longitud);
	memcpy(*buffer,mensaje->content,mensaje->header.longitud);
	*largo_buffer = mensaje->header.longitud;
	destruirMensaje(mensaje);

	pthread_mutex_unlock(&journalingMutexDescribe);
	return describe;
}

int proceso_drop(char* tabla){
	pthread_mutex_lock(&journalingMutexDrop);
	int drop = -1;

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
	content[MAX_PATH] = 0x00;

	sleep(MEM_CONF.RETARDO_FS/1000);
	enviarMensaje(mem,create,largo_content,content,socket_lis,logger,lis);
	t_mensaje* mensaje = recibirMensaje(socket_lis, logger);

	if(mensaje == NULL) {
		loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
		pthread_mutex_unlock(&journalingMutexDrop);
		return -1;
	}
	drop = mensaje->header.error;
	destruirMensaje(mensaje);

	pthread_mutex_unlock(&journalingMutexDrop);
	return drop;
}


int proceso_journal(){
	return process_journaling();
}
