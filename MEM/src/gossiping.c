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

volatile sig_atomic_t bandera_conexion = false;

void handle_alarm( int sig ) {
	bandera_conexion = true;
}

void *crearHiloCliente() {
	int i =0;
	int control_servidor_lista;
	int control_puerto_lista;
	int conexion=0;
	//t_list lista;

	LISTA_CONN = list_create();
	LISTA_CONN_PORT = list_create();

	signal( SIGALRM, handle_alarm ); // Install handler first,
	alarm( 1 ); // before scheduling it to be called.
	loggear(logger,LOG_LEVEL_INFO,"HILO CLIENTE");
	loggear(logger,LOG_LEVEL_INFO,"SEÑAL INICIAL %d",bandera_conexion);

	while (1) {
		//esperarSeñalTiempo;
		loggear(logger,LOG_LEVEL_INFO,"BANDERA %d",bandera_conexion);
		if(bandera_conexion)
		{
			loggear(logger,LOG_LEVEL_INFO,"ELEMENTO %d BANDERA %d",i,bandera_conexion);
			//ACTUALIZA LISTA DE SEEDS DEL ARCHIVO CONFIG
			while(MEM_CONF.IP_SEEDS[i] != NULL)
			{
				conexion=connect_to_server(MEM_CONF.IP_SEEDS[i],MEM_CONF.PUERTO_SEEDS[i],mem);

				if(conexion!=1)
				{
					control_servidor_lista = list_add(LISTA_CONN,MEM_CONF.IP_SEEDS[i]);
					control_puerto_lista = list_add(LISTA_CONN_PORT,MEM_CONF.PUERTO_SEEDS[i]);
				}
				else
				{
					int j=0;
					char *ipCompara;
					while(j<=LISTA_CONN->elements_count)
					{
						//ipCompara = string_new();
						ipCompara = list_get(LISTA_CONN,i);
						if(string_equals_ignore_case(ipCompara, MEM_CONF.IP_SEEDS[i]))
						{
							free(list_remove(LISTA_CONN,i));
							free(list_remove(LISTA_CONN_PORT,i));
							break;
						}
						j++;
					}

				}
				i++;
			}


			bandera_conexion = false;
			alarm( MEM_CONF.RETARDO_GOSSIPING );
		}

	}
}

int crearCliente()
{
	sigset_t set;
	int s;
	int hilo_cliente;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_cliente = pthread_create(&cliente, NULL, crearHiloCliente, (void *) &set);


	return 1;
}
