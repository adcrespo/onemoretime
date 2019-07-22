/*
 * i_kernel_proceso.c
 *
 *  Created on: 8 jun. 2019
 *      Author: utnso
 */

#include "MEM.h"

#include "i_kernel_proceso.h"
#include "memory.h"
#include "proceso.h"
#include "connection.h"
#include "journaling.h"

int proceso_describe_global(char* tabla,int socketKER, fd_set* set_master){
	//DECLARACIONES - INICIALIZACIONES
	t_mensaje* mensajeCantidad=NULL;
	int largo_content = 0;
	char *content = NULL;
	int cantidad;
	int longAcum = 0;
	char* bufferMsjDescribe=NULL;
	t_mensaje* mensajeDescribe=NULL;

	//SEM_MUTEX
	pthread_mutex_lock(&journalingMutexDescribe);

	//PROCESO_COUNT-TABLE
	cantidad=1;
	if(string_is_empty(tabla)){
		loggear(logger,LOG_LEVEL_INFO, "INICIANDO PROCESO DESCRIBE KERNEL");
		enviarMensaje(mem,countTables,0,NULL,socket_lis,logger,lis);
		mensajeCantidad = recibirMensaje(socket_lis, logger);
		cantidad=mensajeCantidad->header.error;
		//loggear(logger,LOG_LEVEL_INFO, "CANTIDAD DE TABLAS DESCRIBE: %d",cantidad);
		if(enviarMensajeConError(mem,countTables,mensajeCantidad->header.longitud,mensajeCantidad->content,socketKER,logger,kernel,mensajeCantidad->header.error)<=0)
		{
			close(socketKER);
			FD_CLR(socketKER, set_master);
		}
		loggear(logger,LOG_LEVEL_INFO, " MSJ ENVIADO KERNEL");
	}
	loggear(logger,LOG_LEVEL_INFO, "CANTIDAD DE TABLAS DESCRIBE: %d",cantidad);

	//PROCESO_DESCRIBE
	largo_content = MAX_PATH;
	content = malloc(largo_content);
	memset(content, 0x00, largo_content);
	content[MAX_PATH-1] = 0x00;
	enviarMensaje(mem,describe,largo_content,content,socket_lis,logger,lis);
	loggear(logger,LOG_LEVEL_INFO, "MENSAJE ENVIADO LFS");

	while(cantidad-->0)
	{
		loggear(logger,LOG_LEVEL_INFO, "INICIO DE RECIVE DESCRIBE_GLOBAL %d", cantidad);
		mensajeDescribe = recibirMensaje(socket_lis, logger);

		if(mensajeDescribe == NULL) {
			loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
		}

		longAcum = mensajeDescribe->header.longitud;
		bufferMsjDescribe = malloc(longAcum);
		memset(bufferMsjDescribe,0x00,mensajeDescribe->header.longitud);
		memcpy(bufferMsjDescribe,mensajeDescribe->content,mensajeDescribe->header.longitud);

		if(enviarMensajeConError(mem,describe,mensajeDescribe->header.longitud,mensajeDescribe->content,socketKER,logger,kernel,mensajeDescribe->header.error)<=0)
		{
			close(socketKER);
			FD_CLR(socketKER, set_master);
		}

		loggear(logger,LOG_LEVEL_DEBUG,"Data: %s",bufferMsjDescribe);

		//LIBERO_MEM_ITERACION
		free(bufferMsjDescribe);
		destruirMensaje(mensajeDescribe);
	}

	//LIBERAR_MEMORIA
	destruirMensaje(mensajeCantidad);
	//destruirMensaje(mensajeDescribe);
	free(content);

	//SEM_MUTEX
	pthread_mutex_unlock(&journalingMutexDescribe);

	return 1;
}

void procesar_KER(t_mensaje* msg, int socketKER, fd_set* set_master) {
	char tabla[MAX_PATH];
	int clave;
	char* buffer;
	int largo_buffer;

	switch(msg->header.tipoMensaje) {
		case handshake:;

			if(enviarMensaje(mem, handshake, 0, NULL, socketKER, logger, kernel)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			break;

		case selectMsg:;

			t_select *msgselect = malloc(sizeof(t_select));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",msg->header.longitud);
			memset(msgselect, 0x00, sizeof(t_select));
			memcpy(msgselect, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ_SELECT******* ");
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgselect->id_proceso);
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgselect->nombreTabla);
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgselect->key);

			memcpy(&tabla,msg->content,MAX_PATH);
			tabla[MAX_PATH-1] = 0x00;
			memcpy(&clave,&msg->content+MAX_PATH,sizeof(int));

			int select_result = proceso_select(msgselect->nombreTabla,msgselect->key,&buffer,&largo_buffer);

			if(select_result>0){
				t_registro* reg = descomponer_registro(buffer);
				printf("[KERNEL] [OK] [timestamp:%llu][key:%d] %s\n",reg->timestamp, reg->key, reg->value);
				destruir_registro(reg);
			}

			if(enviarMensajeConError(mem, selectMsg, largo_buffer, buffer, socketKER,
					logger, kernel, select_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(buffer);
			free(msgselect);
		//TODO: select
			break;

		case insert:;

			t_insert *msgInsert = malloc(sizeof(t_insert));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",msg->header.longitud);
			memset(msgInsert, 0x00, sizeof(t_insert));
			memcpy(msgInsert, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ_INSERT******* ");
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgInsert->id_proceso);
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgInsert->nombreTabla);
			loggear(logger, LOG_LEVEL_INFO, "TIME_STAMP_MSJ :%l",msgInsert->timestamp);
			loggear(logger, LOG_LEVEL_INFO, "VALUE_MSJ :%s",msgInsert->value);
			loggear(logger, LOG_LEVEL_INFO, "KEY_MSJ :%d",msgInsert->key);

			int insert_result = proceso_insert(msgInsert->nombreTabla,msgInsert->key,msgInsert->value,msgInsert->timestamp);

			if(enviarMensajeConError(mem, insert, 0, NULL, socketKER,
					logger, kernel, insert_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(msgInsert);
		//TODO: insert
			break;

		case create:;

			t_create *msgCreate = malloc(sizeof(t_create));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",msg->header.longitud);
			memset(msgCreate, 0x00, sizeof(t_create));
			memcpy(msgCreate, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ_CREATE******* ");
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgCreate->id_proceso);
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgCreate->nombreTabla);
			loggear(logger, LOG_LEVEL_INFO, "TIPO_CON_MSJ :%s",msgCreate->tipo_cons);
			loggear(logger, LOG_LEVEL_INFO, "NUM_PROC_MSJ :%d",msgCreate->num_part);
			loggear(logger, LOG_LEVEL_INFO, "COMP_TIME_MSJ :%d",msgCreate->comp_time);

			int create_result = proceso_create(msgCreate->nombreTabla,msgCreate->tipo_cons,msgCreate->num_part,msgCreate->comp_time);

			if(enviarMensajeConError(mem, create, 0, NULL, socketKER,
					logger, kernel, create_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(msgCreate);
		//TODO: create
			break;

		case describe:;

			t_describe *msgDescribe = malloc(sizeof(t_describe));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",msg->header.longitud);
			memset(msgDescribe, 0x00, sizeof(t_describe));
			memcpy(msgDescribe, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ_DESCRIBE_TABLA******* ");
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgDescribe->id_proceso);
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgDescribe->nombreTabla);

			int describe = proceso_describe_global(msgDescribe->nombreTabla,socketKER,set_master);
			loggear(logger, LOG_LEVEL_INFO, "RESULTADO_DESCRIBE_GLOBAL :%d",describe);
			free(msgDescribe);
		//TODO: describe
			break;

		case describe_global_:;

			t_describe *msgDescribeGlobal = malloc(sizeof(t_describe));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",msg->header.longitud);
			memset(msgDescribeGlobal, 0x00, sizeof(t_describe));
			memcpy(msgDescribeGlobal, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ_DESCRIBE_GLOBAL******* ");
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgDescribeGlobal->id_proceso);
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgDescribeGlobal->nombreTabla);

			int describe_global_result = proceso_describe_global(msgDescribeGlobal->nombreTabla,socketKER,set_master);
			loggear(logger, LOG_LEVEL_INFO, "RESULTADO_DESCRIBE_GLOBAL :%d",describe_global_result);
			free(msgDescribeGlobal);
		//TODO: describe_global
			break;

		case drop:;

			t_drop *msgDrop = malloc(sizeof(t_drop));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",msg->header.longitud);
			memset(msgDrop, 0x00, sizeof(t_drop));
			memcpy(msgDrop, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ_CREATE******* ");
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgDrop->id_proceso);
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgDrop->nombreTabla);

			int drop_result = proceso_drop(msgDrop->nombreTabla);

			if(enviarMensajeConError(mem, drop, 0, NULL, socketKER,
					logger, kernel, drop_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(msgDrop);
		//TODO: drop
			break;

		case journal:;

			int journal_result = proceso_journal();

			if(enviarMensajeConError(mem, journal, 0, NULL, socketKER,
					logger, kernel, journal_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			break;

		//TODO: journal
		case countTables:;

			if(enviarMensajeConError(mem, journal, msg->header.longitud, msg->content, socketKER,
					logger, kernel, msg->header.error)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
		//TODO: countTables
			break;

		default:
			loggear(logger,LOG_LEVEL_ERROR, "No se reconoce el Comando: %d",msg->header.tipoMensaje);
			break;
	}
	return;
}
