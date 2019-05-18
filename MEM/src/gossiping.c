/*
 * cliente.c
 *
 *  Created on: 24 abr. 2019
 *      Author: utnso
 */
//#include <shared.h>
//#include <signal.h>

#include "MEM.h"
#include "connection.h"
#include "file_conf.h"
#include "error.h"
#include "argparse.h"
#include "commons/string.h"
#include "memory.h"
#include "gossiping.h"
#include "commons/collections/list.h"

/*volatile sig_atomic_t bandera_conexion = false;

void handle_alarm( int sig ) {
	bandera_conexion = true;
}*/
int loggearElementosLista(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT)
{
	int i;
	i=0;
	char *ipLista;
	char *puertoLista;

	while(i < LISTA_CONN_LOC->elements_count)
	{
		ipLista = list_get(LISTA_CONN_LOC,i);
		puertoLista =list_get(LISTA_CONN_PORT_LOC,i);

		loggear(logger,LOG_LEVEL_INFO,"CONEXION LISTA SEEDS NUMERO: %d",i+1);

		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS: %s", ipLista);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_PUERTO_SEEDS: %s", puertoLista);
		i++;
	}
	return 1;
}


char *armarMensajeListaSEEDS()
{
	char *msj;
	char *ip;
	char *puerto;

	int j=0;

	msj = string_new();

	//msj = "GOSSIPING|";

	while(j<LISTA_CONN->elements_count)
	{

		ip = list_get(LISTA_CONN_LOC,j);
		puerto = list_get(LISTA_CONN_PORT_LOC,j);

		if(j==0)
		{
			msj = string_from_format("%s%s:%s",ip,msj,puerto);
		}
		else
		{
			msj = string_from_format("%s-",msj);
			msj = string_from_format("%s%s:%s",msj,ip,puerto);
		}
		//msj = string_from_format("%s|",msj);

		j++;
	}

	loggear(logger,LOG_LEVEL_INFO,"MSJ_GOSSIPING: %s", msj);

	return msj;

}


int actualizaListaSeedConfig(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT, char *ipNueva,char *puertoNuevo)
{
	int i=0;
	int existe=0;
	int control_servidor_lista;
	int control_puerto_lista;
	char *ipCompara;

	//while(MEM_CONF.IP_SEEDS[i] != NULL)
	//{
		int j=0;

		while(j<LISTA_CONN->elements_count)
		{

			ipCompara = list_get(LISTA_CONN,i);
			if(string_equals_ignore_case(ipCompara, ipNueva))
			{
				existe=1;
				break;
			}
			j++;
		}
		if(existe!=1)
		{
			control_servidor_lista = list_add(LISTA_CONN,ipNueva);
			control_puerto_lista = list_add(LISTA_CONN_PORT,puertoNuevo);
		}
		//i++;
	//}

	return 1;
}

int crearListaSeeds()
{
	char *cadena;
	int i=0;

	LISTA_CONN = list_create();
	LISTA_CONN_PORT = list_create();

	loggear(logger,LOG_LEVEL_INFO,"Creando lista de SEEDS");

	while(MEM_CONF.IP_SEEDS[i] != NULL)
	{
		actualizaListaSeedConfig(LISTA_CONN,LISTA_CONN_PORT,MEM_CONF.IP_SEEDS[i],MEM_CONF.PUERTO_SEEDS[i]);
		cadena = list_get(LISTA_CONN, i);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS: %s", cadena);
		cadena = list_get(LISTA_CONN_PORT, i);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_PUERTO_SEEDS: %s", cadena);

		i++;
	}
	return 1;
}


int armarListaLocal(t_list *LISTA_CONN_LOC,t_list *LISTA_CONN_PORT_LOC)
{
	int j=0;
	char *ipCompara;
	char *puertoCompara;

	while(j<LISTA_CONN->elements_count)
	{
		ipCompara = list_get(LISTA_CONN,j);
		puertoCompara = list_get(LISTA_CONN_PORT,j);

		actualizaListaSeedConfig(LISTA_CONN_LOC , LISTA_CONN_PORT_LOC , ipCompara , puertoCompara);

		j++;
	}

	return 1;
}

int procesarMsjGossiping(char *mensaje, char *primerParser, char *segundoParser)
{
	int i;
	char** parser;
	char** parserIpPuerto;
	char* ip;
	char* puerto;

	if (!string_is_empty(mensaje))
	{

		parser = string_split(mensaje, primerParser);
		i=0;
		while(parser[i]!=NULL)
		{
			parserIpPuerto = string_split(parser[i], segundoParser);


			if(parserIpPuerto[0] != NULL)
			{
				ip = string_new();
				ip = parserIpPuerto[0];
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"ERROR PROCESO MSJ GOSSIPING");
				return -1;
			}
			if(parserIpPuerto[1] != NULL)
			{
				puerto = string_new();
				puerto = parserIpPuerto[1];
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"ERROR PROCESO MSJ GOSSIPING");
				return -1;
			}

			//AGREGAR A LISTA LOCAL DE PROCESO DE MSJ
			actualizaListaSeedConfig(LISTA_CONN_LOC_MSJ,LISTA_CONN_PORT_LOC_MSJ, ip,puerto);

			i++;
		}
		loggearElementosLista(LISTA_CONN_LOC_MSJ,LISTA_CONN_PORT_LOC_MSJ);
	}
	else
	{
		loggear(logger,LOG_LEVEL_INFO,"ERROR PROCESO MSJ GOSSIPING");
		return-1;
	}

	return 1;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------



void processGossiping() {
	int i =0;
	int socketReceptor=0;
	char *ipLista;
	char *puertoLista;
	char *mensaje;
	t_mensaje* msjRecibido;

	if(pthread_mutex_trylock(&mutexprocessGossiping))
	{
		loggear(logger,LOG_LEVEL_INFO,"ERROR GOSSIPING");
		return;
	}

	//LISTA_CONN_LOC=list_create();
	//LISTA_CONN_PORT_LOC=list_create();

	loggear(logger,LOG_LEVEL_INFO,"Se inicio proceso Gossiping");

	//sleep(MEM_CONF.RETARDO_GOSSIPING);
	//sleep(15);
	//while (1)
	//{
		i=0;

		mensaje = string_new();

		//mensaje = armarMensajeListaSEEDS();

		//loggear(logger,LOG_LEVEL_INFO,"CREANDO LISTA LOCAL");
		pthread_mutex_lock(&mutexGossiping);
		armarListaLocal(LISTA_CONN_LOC,LISTA_CONN_PORT_LOC);
		pthread_mutex_unlock(&mutexGossiping);
		mensaje = armarMensajeListaSEEDS();
		procesarMsjGossiping(mensaje,"|","-");
		//ipLista = list_get(LISTA_CONN_LOC,i);
		//puertoLista =list_get(LISTA_CONN_PORT_LOC,i);
		//loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS: %s", ipLista);
		//loggear(logger,LOG_LEVEL_INFO,"LISTA_PUERTO_SEEDS: %s", puertoLista);


		//CONEXION_CON_CADA_MEMORIA
		while(i < LISTA_CONN_LOC->elements_count)
		{
			ipLista = list_get(LISTA_CONN_LOC,i);
			puertoLista =list_get(LISTA_CONN_PORT_LOC,i);

			loggear(logger,LOG_LEVEL_INFO,"CONEXION LISTA SEEDS NUMERO: %d",i+1);

			loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS: %s", ipLista);
			loggear(logger,LOG_LEVEL_INFO,"LISTA_PUERTO_SEEDS: %s", puertoLista);

			socketReceptor=connect_to_server(ipLista,puertoLista,mem,gossiping);

			if(socketReceptor>0)
			{
				int envioMsj;
				///int largoMsj=0;
				//largoMsj = strlen(mensaje);
				//ENVIAR LISTA SEEDS
				envioMsj = enviarMensaje(mem,gossipingMsg,strlen(mensaje)+1,mensaje,socketReceptor,logger,mem);
				if(envioMsj < 1 )
					loggear(logger,LOG_LEVEL_INFO,"NO SE PUDO ENVIAR MSJ %d",envioMsj);
				else
					loggear(logger,LOG_LEVEL_INFO,"MSJ ENVIADO CON EXITO %d",envioMsj);

				//RECIBIR_LISTA_SEEDS
				free (mensaje);
				//mensaje = string_new();

				msjRecibido = recibirMensaje(socketReceptor,logger);
				//PROCESAR_LISTA_SEEDS
				if(msjRecibido != NULL)
				{
					//mensaje = string_new();
					loggear(logger,LOG_LEVEL_INFO,"MSJ ENVIADO CON EXITO %d",envioMsj);
					procesarMsjGossiping(msjRecibido->content,"|","-");

				}
				else
				{
					loggear(logger,LOG_LEVEL_INFO,"ERROR MSJ %d",envioMsj);
				}



			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"FALLÓ_CONEXION: %d", socketReceptor);
			}
			i++;
		}
	pthread_mutex_unlock(&mutexprocessGossiping);
		//sleep(MEM_CONF.RETARDO_GOSSIPING);
	//	sleep(15);

//	}
}
void *hiloGossiping()
{
	//sleep (MEM_CONF.RETARDO_GOSSIPING/1000);
	LISTA_CONN_LOC=list_create();
	LISTA_CONN_PORT_LOC=list_create();

	LISTA_CONN_LOC_MSJ=list_create();
	LISTA_CONN_PORT_LOC_MSJ=list_create();

	//sleep (15);
	while (1)
	{
		loggear(logger,LOG_LEVEL_INFO,"INIT_HILO_GOSSIPING");
		processGossiping();
		loggear(logger,LOG_LEVEL_INFO,"END_HILO_GOSSIPING");
		//sleep (MEM_CONF.RETARDO_GOSSIPING/1000);
		sleep (15);
	}
}

int crearHiloGossiping()
{
	sigset_t set;
	int s;
	int hilo_cliente;

	pthread_mutex_init(&mutexGossiping, NULL);
	pthread_mutex_init(&mutexprocessGossiping, NULL);

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_cliente = pthread_create(&cliente, NULL, hiloGossiping, (void *) &set);
	loggear(logger,LOG_LEVEL_INFO,"ERROR_HILO_GOSSIPING: %s", hilo_cliente);

	return 1;
}












