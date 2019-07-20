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


void procesar_KER(t_mensaje* msg, int socketKER, fd_set* set_master) {
	char tabla[MAX_PATH];
	int clave; char value[VALUE]; unsigned long long timestamp;
	char tipo_cons[4]; int num_part; int comp_time;
	char* buffer; int largo_buffer;

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
			memcpy(msgselect, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ******* ");
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgselect->nombreTabla);
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgselect->id_proceso);

			memcpy(&tabla,msg->content,MAX_PATH);
			tabla[MAX_PATH-1] = 0x00;
			memcpy(&clave,&msg->content+MAX_PATH,sizeof(int));

			loggear(logger, LOG_LEVEL_DEBUG, "[KERNEL] Select...");
			loggear(logger, LOG_LEVEL_DEBUG, "[KERNEL] Tabla: %s", tabla);
			loggear(logger, LOG_LEVEL_DEBUG, "[KERNEL] Clave: %s", clave);

			int select_result = proceso_select(tabla,clave,&buffer,&largo_buffer);

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
		//TODO: select
			break;
		case insert:;
			memcpy(&tabla,msg->content,MAX_PATH);
			tabla[MAX_PATH-1] = 0x00;
			memcpy(&clave,&msg->content+MAX_PATH,sizeof(int));
			memcpy(&value,&msg->content+MAX_PATH+sizeof(int),VALUE);
			value[VALUE-1] = 0x00;
			memcpy(&timestamp,&msg->content+MAX_PATH+sizeof(int)+VALUE,sizeof(unsigned long long));

			int insert_result = proceso_insert(tabla,clave,value,timestamp);

			if(enviarMensajeConError(mem, insert, 0, NULL, socketKER,
					logger, kernel, insert_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
		//TODO: insert
			break;
		case create:;
			t_create *msgCreate = malloc(sizeof(t_create));
			loggear(logger, LOG_LEVEL_INFO, "Malloc ok, msg header long :%d",msg->header.longitud);
			memcpy(msgCreate, msg->content, msg->header.longitud);
			loggear(logger, LOG_LEVEL_INFO, "******MSJ******* ");
			loggear(logger, LOG_LEVEL_INFO, "NOMBRE_TABLA_MSJ :%s",msgCreate->nombreTabla);
			loggear(logger, LOG_LEVEL_INFO, "ID_PROCESO_MSJ :%d",msgCreate->id_proceso);

			int create_result = proceso_create(msgCreate->nombreTabla,msgCreate->tipo_cons,msgCreate->num_part,msgCreate->comp_time);

			if(enviarMensajeConError(mem, create, 0, NULL, socketKER,
					logger, kernel, create_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
		//TODO: create
			break;
		case describe_global_:;
			pthread_mutex_lock(&journalingMutexDescribe);

			//COUNT-TABLE
			loggear(logger,LOG_LEVEL_INFO, "INICIANDO PROCESO DESCRIBE KERNEL");
			enviarMensaje(mem,countTables,0,NULL,socket_lis,logger,lis);
			t_mensaje* mensajeCantidad = recibirMensaje(socket_lis, logger);
			loggear(logger,LOG_LEVEL_INFO, "Cantidad TABLAS DESCRIBE: %d",mensajeCantidad->header.error);
			if(enviarMensajeConError(mem,countTables,mensajeCantidad->header.longitud,mensajeCantidad->content,socketKER,logger,kernel,mensajeCantidad->header.error)<=0)
			{
				close(socketKER);
				FD_CLR(socketKER, set_master);
			}

			loggear(logger,LOG_LEVEL_INFO, "RESULTADO MSJ-KERNEL");

			//DESCRIBE
			int largo_content = MAX_PATH;
			char *content = malloc(largo_content);
			memset(content, 0x00, largo_content);
			content[MAX_PATH-1] = 0x00;
			enviarMensaje(mem,describe,largo_content,content,socket_lis,logger,lis);
			loggear(logger,LOG_LEVEL_INFO, "MENSAJE ENVIADO LFS");
			free(content);

			int cantidad = mensajeCantidad->header.error;
			int longAcum = 0;
			char* bufferMsjDescribe=NULL;

			while(cantidad-->0)
			{
				loggear(logger,LOG_LEVEL_INFO, "INICIO DE RECIVE %d", cantidad);
				t_mensaje* mensaje = recibirMensaje(socket_lis, logger);
				if(mensaje == NULL) {
					loggear(logger,LOG_LEVEL_ERROR,"No se pudo recibir mensaje de lis");
				}
				longAcum += mensaje->header.longitud;
				bufferMsjDescribe = realloc(bufferMsjDescribe,longAcum);
				memset(bufferMsjDescribe,0x00,mensaje->header.longitud);
				memcpy(bufferMsjDescribe,mensaje->content,mensaje->header.longitud);
				//*largo_buffer = mensaje->header.longitud;
				if(enviarMensajeConError(mem,describe,mensaje->header.longitud,mensaje->content,socketKER,logger,kernel,mensaje->header.error)<=0)
				{
					close(socketKER);
					FD_CLR(socketKER, set_master);
				}
				destruirMensaje(mensaje);
				loggear(logger,LOG_LEVEL_DEBUG,"Data: %s",bufferMsjDescribe);
			}
			destruirMensaje(mensajeCantidad);
			free(bufferMsjDescribe);
			pthread_mutex_unlock(&journalingMutexDescribe);
		//TODO: describe
			break;
		case drop:;
			memcpy(&tabla,msg->content,MAX_PATH);
			tabla[MAX_PATH-1] = 0x00;

			int drop_result = proceso_drop(tabla);

			if(enviarMensajeConError(mem, drop, 0, NULL, socketKER,
					logger, kernel, drop_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
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
			//enviarMensaje(mem,countTables,0,NULL,socket_lis,logger,lis);
			//t_mensaje* mensajeCantidad = recibirMensaje(socket_lis, logger);
			//enviarMensajeConError(mem,countTables,mensajeCantidad->header.longitud,mensajeCantidad->content,socketKER,logger,kernel,mensajeCantidad->header.error);
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
