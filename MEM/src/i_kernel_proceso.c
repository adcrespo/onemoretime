/*
 * i_kernel_proceso.c
 *
 *  Created on: 8 jun. 2019
 *      Author: utnso
 */

#include "MEM.h"
#include "i_kernel_proceso.h"

#include "proceso.h"

void procesar_KER(t_mensaje* msg, int socketKER, fd_set* set_master) {
	int largo_tabla; char* tabla;
	int clave; int largo_value; char* value;
	char tipo_cons[3]; int num_part; int comp_time;
	char* buffer; int largo_buffer;

	switch(msg->header.tipoMensaje) {
		case handshake:;
			if(enviarMensaje(mem, handshake, 0, NULL, socketKER, logger, kernel)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			break;
		case selectMsg:;
			memcpy(&largo_tabla,msg->content,sizeof(int));
			tabla = malloc(largo_tabla);
			memcpy(tabla,msg->content+sizeof(int),largo_tabla);
			memcpy(&clave,&msg->content+sizeof(int)+largo_tabla,sizeof(int));

			int select_result = proceso_select(tabla,clave,&buffer);

			if(enviarMensajeConError(mem, selectMsg, 0, NULL, socketKER,
					logger, kernel, select_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(tabla);
			free(buffer);
		//TODO: select
			break;
		case insert:;
			memcpy(&largo_tabla,msg->content,sizeof(int));
			tabla = malloc(largo_tabla);
			memcpy(tabla,msg->content+sizeof(int),largo_tabla);
			memcpy(&clave,&msg->content+sizeof(int)+largo_tabla,sizeof(int));
			memcpy(&largo_value,&msg->content+sizeof(int)+largo_tabla+sizeof(int),sizeof(int));
			value = malloc(largo_value);
			memcpy(value,&msg->content+sizeof(int)+largo_tabla+sizeof(int)+sizeof(int),largo_value);

			int insert_result = proceso_insert(tabla,clave,value);

			if(enviarMensajeConError(mem, insert, 0, NULL, socketKER,
					logger, kernel, insert_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(tabla);
			free(value);
		//TODO: insert
			break;
		case create:;
			memcpy(&largo_tabla,msg->content,sizeof(int));
			tabla = malloc(largo_tabla);
			memcpy(tabla,msg->content+sizeof(int),largo_tabla);
			memcpy(&tipo_cons,&msg->content+sizeof(int)+largo_tabla,3);
			memcpy(&num_part,&msg->content+sizeof(int)+largo_tabla+3,sizeof(int));
			memcpy(&comp_time,&msg->content+sizeof(int)+largo_tabla+3+sizeof(int),sizeof(int));

			int create_result = proceso_create(tabla,tipo_cons,num_part,comp_time);

			if(enviarMensajeConError(mem, create, 0, NULL, socketKER,
					logger, kernel, create_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
		//TODO: create
			break;
		case describe:;
			memcpy(&largo_tabla,msg->content,sizeof(int));
			tabla = malloc(largo_tabla);
			memcpy(tabla,msg->content+sizeof(int),largo_tabla);

			int describe_result = proceso_describe(tabla,&buffer,&largo_buffer);

			if(enviarMensajeConError(mem, describe, 0, NULL, socketKER,
					logger, kernel, describe_result)<=0){
				close(socketKER);
				FD_CLR(socketKER, set_master);;
			}
			free(buffer);
		//TODO: describe
			break;
		case drop:;
			memcpy(&largo_tabla,msg->content,sizeof(int));
			tabla = malloc(largo_tabla);
			memcpy(tabla,msg->content+sizeof(int),largo_tabla);

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
