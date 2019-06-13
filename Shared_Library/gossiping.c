/*
 * gossiping.c
 *
 *  Created on: 8 jun. 2019
 *      Author: utnso
 */

#include "gossiping.h"

int loggearLista(t_list *LISTA_CONN,t_log *logger)
{
	char *cadena;
	t_tipoSeeds *seed;
	int i=0;

	while(i<LISTA_CONN->elements_count)
	{
		seed = list_get(LISTA_CONN, i);

		cadena = string_from_format("%s", seed->numeroMemoria);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_NUMERO[%d]: %s",i, cadena);
		cadena = string_from_format("%s", seed->ip);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_IP[%d]: %s",i, cadena);
		cadena = string_from_format("%s", seed->puerto);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_PUERTO[%d]: %s",i, cadena);
		cadena = string_from_format("%s", seed->estado);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_ESTADO[%d]: %s",i, cadena);

		i++;
	}
	return 1;
}

int loggearElementosListaConectados(t_list *LISTA_CONECTADOS, t_log *logger)
{
	int i;
	i=0;
	char *ipLista;

	while(i < LISTA_CONECTADOS->elements_count)
	{
		ipLista = list_get(LISTA_CONECTADOS,i);

		loggear(logger,LOG_LEVEL_INFO,"LOG - LISTA_SEEDS_CONECTADOS_NUMERO: %d",i+1);

		loggear(logger,LOG_LEVEL_INFO,"LOG - LISTA_IP_PUERTO_SEEDS_CONECTADOS: %s", ipLista);
		i++;
	}
	return 1;
}

t_list *obtenerListaConectados(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT,t_log *logger)
{
	//t_list *LISTA_CONECTADOS;
	char *ip;
	char *puerto;
	int j=0;
	char *nuevoElementoLista;
	/*
	char *numeroMemoria;
	char conectado;
	t_tipoSeeds seed;
	*/

	//LISTA_CONECTADOS =list_create();

	while(j<LISTA_CONECTADOS->elements_count)
	{
		free(list_remove(LISTA_CONECTADOS,j));
		j++;
	}

	j=0;
	while(j<LISTA_CONN->elements_count)
	{
		if(j!=0 && BITMAP_CONN_STATUS[j] == CONECTADO)
		{
			nuevoElementoLista = string_new();


			/*
			seed = list_get(LISTA_CONN,j);
			numeroMemoria = string_from_format("%s", seed->numeroMemoria);
			ip = string_from_format("%s", seed->ip);
			puerto = string_from_format("%s", seed->puerto);
			conectado = string_from_format("%s", seed->estado);
			*/


			ip = list_get(LISTA_CONN,j);
			puerto = list_get(LISTA_CONN_PORT,j);


			nuevoElementoLista = string_from_format("%s:%s",ip,puerto);
			list_add(LISTA_CONECTADOS,nuevoElementoLista);
			//free(nuevoElementoLista);
		}
		j++;
	}

	loggearElementosListaConectados(LISTA_CONECTADOS,logger);

	return LISTA_CONECTADOS;
}

char* getLocalIp(char *MEM_CONF_IP)
{
	char* localIp = string_new();
	struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void* tmpAddrPtr=NULL;

    FILE *f;
    char line[100] , *p , *c;

    if(MEM_CONF_IP != NULL)
    	return MEM_CONF_IP;

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

int actualizaListaSeedConfig(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT, char *ipNueva,char *puertoNuevo)
{
	int j=0;
	int existe=0;
	char *ipCompara;
	char *portCompara;

	/*
	char *numeroCompara;
	char conectado;
	t_tipoSeeds seed;
	*/

	while(j<LISTA_CONN->elements_count)
	{

		//loggearLista(LISTA_CONN, logger);
		/*
		seed = list_get(LISTA_CONN,j);
		numeroCompara = string_from_format("%s", seed->numeroMemoria)	;
		ipCompara = string_from_format("%s", seed->ip);
		portCompara = string_from_format("%s", seed->puerto);
		conectado = string_from_format("%s", seed->estado);
		*/

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
		/*
		seed->numeroMemoria = string_from_format("%s", numeroMemoria);
		seed->ip = string_from_format("%s", ipNueva);
		seed->puerto = string_from_format("%s", puertoNuevo);
		seed->estado = DESCONECTADO;
		list_add(LISTA_CONN,seed);
		*/

		list_add(LISTA_CONN,ipNueva);
		list_add(LISTA_CONN_PORT,puertoNuevo);
	}

	return 1;
}

int incializarBitMapConnStatus()
{
	BITMAP_CONN_STATUS[0]=CONECTADO;

	for (int i = 1; i< NUM_CONEX; i++)
		BITMAP_CONN_STATUS[i]=0;

	return 1;
}

int crearListaSeeds(char *MEM_CONF_IP,char *MEM_CONF_PUERTO, char **MEM_CONF_IP_SEEDS, char **MEM_CONF_PUERTO_SEEDS, t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT)
{
	char *cadena;
	int i=0;

	//t_tipoSeeds seed;

	incializarBitMapConnStatus();

	loggear(logger,LOG_LEVEL_INFO,"Creando lista de SEEDS/PUERTOS...");

	actualizaListaSeedConfig(LISTA_CONN,LISTA_CONN_PORT,getLocalIp(MEM_CONF_IP),MEM_CONF_PUERTO);

	//loggearLista(LISTA_CONN, logger);

	cadena = list_get(LISTA_CONN, i);
	loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS[%d]: %s",i, cadena);
	cadena = list_get(LISTA_CONN_PORT, i);
	loggear(logger,LOG_LEVEL_INFO,"LISTA_PUERTO_SEEDS[%d]: %s",i, cadena);

	while(MEM_CONF_IP_SEEDS[i] != NULL)
	{
		actualizaListaSeedConfig(LISTA_CONN,LISTA_CONN_PORT,MEM_CONF_IP_SEEDS[i],MEM_CONF_PUERTO_SEEDS[i]);

		//loggearLista(LISTA_CONN, logger);

		cadena = list_get(LISTA_CONN, i+1);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS[%d]: %s",i+1, cadena);
		cadena = list_get(LISTA_CONN_PORT, i+1);
		loggear(logger,LOG_LEVEL_INFO,"LISTA_PUERTO_SEEDS[%d]: %s",i+1, cadena);

		i++;
	}
	return 1;
}

int connect_to_server_goss(char* IP, char* PUERTO, int proceso, int flag, t_log *logger) {

        int socket;

        if((socket = definirSocket(logger))<= 0)
        	return-1;

        if(conectarseAServidor_w_to(socket, IP, atoi(PUERTO), logger)<=0)
        	return -1;

        loggear(logger, LOG_LEVEL_INFO, "INICIO Handshake(%d)...", proceso);
        enviarMensaje(mem, handshake, 0, NULL, socket, logger, proceso);
        t_mensaje* msg = recibirMensaje(socket, logger);
        destruirMensaje(msg);
        loggear(logger, LOG_LEVEL_INFO, "FIN Handshake(%d)", proceso);
        return socket;
}

char *armarMensajeListaSEEDS(t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT)
{
	char *msj;
	char *ip;
	char *puerto;
	/*
	char *numeroMemoria;
	t_tipoSeeds seed;
	*/


	int j=0;

	msj = string_new();

	while(j<LISTA_CONN->elements_count)
	{

		/*
		seed = list_get(LISTA_CONN,j);
		numeroMemoria = string_from_format("%s", seed->numeroMemoria);
		ip = string_from_format("%s", seed->ip);
		puerto = string_from_format("%s", seed->puerto);
		*/

		ip = list_get(LISTA_CONN,j);
		puerto = list_get(LISTA_CONN_PORT,j);

		if(j==0)
		{
			//msj = string_from_format("%s:%s:%s",numeroMemoria,ip,puerto);

			msj = string_from_format("%s:%s",ip,puerto);
		}
		else
		{
			msj = string_from_format("%s-",msj);
			msj = string_from_format("%s%s:%s",msj,ip,puerto);

			//msj = string_from_format("%s%s:%s:%s",msj,numeroMemoria,ip,puerto);
		}

		j++;
	}

	loggear(logger,LOG_LEVEL_INFO,"MSJ_GOSSIPING: %s", msj);

	return msj;
}

int loggearElementosLista(t_list *LISTA_CONN,t_list *LISTA_CONN_PORT, t_log *logger)
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

int procesarMsjGossiping(char *mensaje, char *primerParser, char *segundoParser, t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT)
{
	int i;
	char** parser;
	char** parserIpPuerto;
	char* ip;
	char* puerto;
	//char* numeroMemoria;

	if (!string_is_empty(mensaje))
	{

		parser = string_split(mensaje, primerParser);
		i=0;
		while(parser[i]!=NULL)
		{
			parserIpPuerto = string_split(parser[i], segundoParser);

			/*
			if(parserIpPuerto[0] != NULL)
			{
				numeroMemoria = string_new();
				numeroMemoria = parserIpPuerto[0];
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"ERROR PROCESO MSJ GOSSIPING");
				return -1;
			}
			*/

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
		loggearElementosLista(LISTA_CONN,LISTA_CONN_PORT,logger);
	}
	else
	{
		loggear(logger,LOG_LEVEL_INFO,"ERROR PROCESO MSJ GOSSIPING");
		return-1;
	}

	return 1;
}

void processGossiping(t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT) {
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
		loggear(logger,LOG_LEVEL_INFO,"ERROR_MUTEX_GOSSIPING");
		return;
	}

	loggear(logger,LOG_LEVEL_INFO,"Se inicio proceso Gossiping...");

	incializarBitMapConnStatus();
	mensaje = string_new();

	pthread_mutex_lock(&mutexGossiping);
	mensaje= string_from_format("%s",armarMensajeListaSEEDS(logger,LISTA_CONN,LISTA_CONN_PORT));
	//mensaje = armarMensajeListaSEEDS(logger);
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

		socketReceptor=connect_to_server_goss(ipLista,puertoLista,mem,gossiping,logger);

		if(socketReceptor>0)
		{
			int envioMsj;

			//ENVIAR_LISTA_SEEDS
			envioMsj = enviarMensaje(mem,gossipingMsg,strlen(mensaje)+1,mensaje,socketReceptor,logger,mem);

			if(envioMsj < 1 )
				loggear(logger,LOG_LEVEL_INFO,"NO SE PUDO ENVIAR MSJ %d",envioMsj);
			else
				loggear(logger,LOG_LEVEL_INFO,"MSJ ENVIADO CON EXITO %d",envioMsj);

			//RECIBIR_LISTA_SEEDS
			msjRecibido = recibirMensaje(socketReceptor,logger);

			//PROCESAR_LISTA_SEEDS
			if(msjRecibido != NULL)
			{
				loggear(logger,LOG_LEVEL_INFO,"MSJ RECIBIDO CON EXITO %d",envioMsj);
				procesarMsjGossiping(msjRecibido->content,"-",":",logger,LISTA_CONN,LISTA_CONN_PORT);
				destruirMensaje(msjRecibido);
				BITMAP_CONN_STATUS[i]=CONECTADO;
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"ERROR MSJ %d",envioMsj);
				BITMAP_CONN_STATUS[i]=DESCONECTADO;
			}

			close(socketReceptor);
		}
		else
		{
			loggear(logger,LOG_LEVEL_INFO,"FALLÓ_CONEXION: %d", socketReceptor);
			BITMAP_CONN_STATUS[i]=DESCONECTADO;
		}
		i++;
	}
	free (mensaje);


	pthread_mutex_lock(&mutexGossiping);
	obtenerListaConectados(LISTA_CONN,LISTA_CONN_PORT,logger);
	pthread_mutex_unlock(&mutexGossiping);

	pthread_mutex_unlock(&mutexprocessGossiping);

}

void *hiloGossiping(t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT)
{
	//sleep (MEM_CONF.RETARDO_GOSSIPING/1000);

	while (1)
	{
		loggear(logger,LOG_LEVEL_INFO,"INIT_HILO_GOSSIPING");
		processGossiping(logger,LISTA_CONN,LISTA_CONN_PORT);
		loggear(logger,LOG_LEVEL_INFO,"END_HILO_GOSSIPING");

		//sleep (MEM_CONF.RETARDO_GOSSIPING/1000);
		sleep (15);
	}
}

int crearHiloGossiping(t_log *logger,t_list *LISTA_CONN,t_list *LISTA_CONN_PORT,int *hilo_cliente)
{
	sigset_t set;
	int s;
	//int hilo_cliente;
	LISTA_CONECTADOS =list_create();

	pthread_mutex_init(&mutexGossiping, NULL);
	pthread_mutex_init(&mutexprocessGossiping, NULL);

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		return -1;
		//_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	*hilo_cliente = pthread_create(&cliente, NULL, hiloGossiping(logger,LISTA_CONN,LISTA_CONN_PORT), (void *) &set);

	if (*hilo_cliente == -1)
		loggear(logger,LOG_LEVEL_INFO,"ERROR_HILO_GOSSIPING: %s", hilo_cliente);
	log_info(logger, "Se generó el hilo para el GOSSIPING.");

	return 1;
}

