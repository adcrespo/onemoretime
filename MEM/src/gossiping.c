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

char* getLocalIp()
{
	char* localIp = string_new();
	struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void* tmpAddrPtr=NULL;

    FILE *f;
    char line[100] , *p , *c;

	f = fopen("/proc/net/route", "r");

	while(fgets(line , 100 , f)) {
		p = strtok(line , " \t");
		c = strtok(NULL , " \t");
		if(p!=NULL && c!=NULL && strcmp(c , "00000000") == 0) break;
	}

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET && strcmp( ifa->ifa_name , p) == 0) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            localIp = string_from_format(addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return localIp;
}

int loggearElementosLista(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT)
{
	int i;
	i=0;
	char *ipLista;
	char *puertoLista;

	while(i < LISTA_CONN->elements_count)
	{
		ipLista = list_get(LISTA_CONN,i);
		puertoLista =list_get(LISTA_CONN_PORT,i);

		loggear(logger,LOG_LEVEL_INFO,"LOG - LISTA SEEDS NUMERO: %d",i+1);

		loggear(logger,LOG_LEVEL_INFO,"LOG - LISTA_IP_SEEDS: %s", ipLista);
		loggear(logger,LOG_LEVEL_INFO,"LOG - LISTA_PUERTO_SEEDS: %s", puertoLista);
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

		ip = list_get(LISTA_CONN,j);
		puerto = list_get(LISTA_CONN_PORT,j);

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
	int j=0;
	int existe=0;
	char *ipCompara;
	char *portCompara;

	while(j<LISTA_CONN->elements_count)
	{

		ipCompara = list_get(LISTA_CONN,j);
		portCompara = list_get(LISTA_CONN_PORT,j);
		if(string_equals_ignore_case(ipCompara, ipNueva) && string_equals_ignore_case(portCompara, puertoNuevo))
		{
			existe=1;
			break;
		}
		j++;
	}
	if(existe!=1)
	{
		list_add(LISTA_CONN,ipNueva);
		list_add(LISTA_CONN_PORT,puertoNuevo);
	}

	return 1;
}

int crearListaSeeds()
{
	char *cadena;
	int i=0;

	LISTA_CONN = list_create();
	LISTA_CONN_PORT = list_create();

	loggear(logger,LOG_LEVEL_INFO,"Creando lista de SEEDS");

	actualizaListaSeedConfig(LISTA_CONN,LISTA_CONN_PORT,getLocalIp(),MEM_CONF.PUERTO);

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
			pthread_mutex_lock(&mutexGossiping);
			actualizaListaSeedConfig(LISTA_CONN,LISTA_CONN_PORT, ip,puerto);
			pthread_mutex_unlock(&mutexGossiping);

			i++;
		}
		loggearElementosLista(LISTA_CONN,LISTA_CONN_PORT);
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
	int i;
	int contadorLista;
	int socketReceptor=0;
	char *ipLista;
	char *puertoLista;
	char *mensaje;
	t_mensaje* msjRecibido;

	i=1;

	if(pthread_mutex_trylock(&mutexprocessGossiping))
	{
		loggear(logger,LOG_LEVEL_INFO,"ERROR GOSSIPING");
		return;
	}

	loggear(logger,LOG_LEVEL_INFO,"Se inicio proceso Gossiping");

	mensaje = string_new();

	pthread_mutex_lock(&mutexGossiping);
	mensaje = armarMensajeListaSEEDS();
	pthread_mutex_unlock(&mutexGossiping);

	pthread_mutex_lock(&mutexGossiping);
	contadorLista = LISTA_CONN->elements_count;
	pthread_mutex_unlock(&mutexGossiping);

	//CONEXION_CON_CADA_MEMORIA
	while(i < contadorLista)
	{
		pthread_mutex_lock(&mutexGossiping);
		ipLista = list_get(LISTA_CONN,i);
		puertoLista =list_get(LISTA_CONN_PORT,i);
		pthread_mutex_unlock(&mutexGossiping);

		loggear(logger,LOG_LEVEL_INFO,"CONEXION LISTA SEEDS NUMERO: %d",i+1);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS: %s", ipLista);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_PUERTO_SEEDS: %s", puertoLista);

		socketReceptor=connect_to_server(ipLista,puertoLista,mem,gossiping);

		if(socketReceptor>0)
		{
			int envioMsj;

			//ENVIAR_LISTA_SEEDS
			envioMsj = enviarMensaje(mem,gossipingMsg,strlen(mensaje)+1,mensaje,socketReceptor,logger,mem);

			if(envioMsj < 1 )
				loggear(logger,LOG_LEVEL_INFO,"NO SE PUDO ENVIAR MSJ %d",envioMsj);
			else
				loggear(logger,LOG_LEVEL_INFO,"MSJ ENVIADO CON EXITO %d",envioMsj);
			free (mensaje);

			//RECIBIR_LISTA_SEEDS
			msjRecibido = recibirMensaje(socketReceptor,logger);

			//PROCESAR_LISTA_SEEDS
			if(msjRecibido != NULL)
			{
				loggear(logger,LOG_LEVEL_INFO,"MSJ RECIBIDO CON EXITO %d",envioMsj);
				procesarMsjGossiping(msjRecibido->content,"-",":");
				destruirMensaje(msjRecibido);
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"ERROR MSJ %d",envioMsj);
			}

			close(socketReceptor);
		}
		else
		{
			loggear(logger,LOG_LEVEL_INFO,"FALLÓ_CONEXION: %d", socketReceptor);
		}
		i++;
	}

	pthread_mutex_unlock(&mutexprocessGossiping);

}
void *hiloGossiping()
{
	//sleep (MEM_CONF.RETARDO_GOSSIPING/1000);

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












