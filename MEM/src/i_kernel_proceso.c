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
			memcpy(&tabla,msg->content,MAX_PATH);
			tabla[MAX_PATH-1] = 0x00;
			memcpy(&tipo_cons,&msg->content+MAX_PATH,4);
			memcpy(&num_part,&msg->content+MAX_PATH+4,sizeof(int));
			memcpy(&comp_time,&msg->content+MAX_PATH+4+sizeof(int),sizeof(int));

			int create_result = proceso_create(tabla,tipo_cons,num_part,comp_time);

			if(enviarMensajeConError(mem, create, 0, NULL, socketKER,
					logger, kernel, create_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
		//TODO: create
			break;
		case describe:;
			memcpy(&tabla,msg->content,MAX_PATH);
			tabla[MAX_PATH-1] = 0x00;
			int describe_result = proceso_describe(tabla[0]!=0x00?tabla:"",&buffer,&largo_buffer);

			if(enviarMensajeConError(mem, describe, 0, NULL, socketKER,
					logger, kernel, describe_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(buffer);
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
		//TODO: journal
			break;
		default:
			loggear(logger,LOG_LEVEL_ERROR, "No se reconoce el Comando: %d",msg->header.tipoMensaje);
			break;
	}
	return;
}
