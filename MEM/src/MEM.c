/*
 ============================================================================
 Name        : FM9.c
 Author      : Emiliano Ibarrola
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "MEM.h"

#include <shared.h>

#include "connection.h"
#include "file_conf.h"
#include "consola.h"
#include "error.h"
#include "argparse.h"
#include "memory.h"
#include "journaling.h"
#include "gossiping.h"
#include "gossipingMemoria.h"
#include "inotifyMemoria.h"

char* intToChar4(int num){
	//RECORDAR: liberar el puntero con free()
	char* n_char = string_new();//malloc(sizeof(int));
	memcpy(n_char, &num, sizeof(int));
	return n_char;
}

int main(int argc, char *argv[]) {

	LISTA_CONN = list_create();
	LISTA_CONN_PORT = list_create();
	initArgumentos(argc, argv);
	logger = configurar_logger_verbose("MEM.log", "MEM", string_equals_ignore_case(args_verbose,"true")?true:false);
	get_config(string_equals_ignore_case(args_configfile,"false")?"../MEM.conf":args_configfile);

	//TODO: Inicializar la lista de SEEDS - USAR EN KERNEL
	crearListaSeedsStruct(gossiping,MEM_CONF.IP,MEM_CONF.PUERTO,MEM_CONF.MEMORY_NUMBER,MEM_CONF.IP_SEEDS,MEM_CONF.PUERTO_SEEDS,logger,LISTA_CONN);


	/*init_memory_spa();
	int paginas1 = add_spa("TABLA1",1,obtenerTimeStamp()); //Siempre devuelve pag; si es 0 no reservo nada
	int paginas2 = add_spa("TABLA1",1,obtenerTimeStamp());
	getPaginaMenorTimestamp();
	int paginas4 = add_spa("TABLA2",1,obtenerTimeStamp());
	int paginas3 = add_spa("TABLA1",1,obtenerTimeStamp());
	getPaginaMenorTimestamp();
	int escrito1 = escribir_bytes_spa("TABLA1",1,1*MAX_LINEA,8+strlen("DATO")+1,componer_registro(100,1,"DATO",strlen("DATO")+1),0); //offset siempre deberia ser multiplo de max_linea
	int escrito2 = escribir_bytes_spa("TABLA1",0,0,8+strlen("HOLA")+1,componer_registro(100,2,"HOLA",strlen("HOLA")+1),0);
	int escrito3 = escribir_bytes_spa("TABLA1",0,1*MAX_LINEA,8+strlen("COMO")+1,componer_registro(100,3,"COMO",strlen("COMO")+1),0);
	int escrito4 = escribir_bytes_spa("TABLA1",2,2*MAX_LINEA,8+strlen("FAIL")+1,componer_registro(100,4,"FAIL",strlen("FAIL")+1),0); //si es -1 no encontre la pagina
	int escrito5 = escribir_bytes_spa("TABLA1",1,8*MAX_LINEA,8+strlen("FAIL")+1,componer_registro(100,5,"FAIL",strlen("FAIL")+1),0); //TODO probar cuando la linea supera la pagina
	char* buffer1 = leer_bytes_spa("TABLA1",0,0,MAX_LINEA); //size siempre deberia ser max_liena y offset siempre deberia ser multiplo de max_linea
	char* buffer2 = leer_bytes_spa("TABLA1",0,MAX_LINEA,MAX_LINEA);
	char* buffer3 = leer_bytes_spa("TABLA1",1,0,MAX_LINEA);
	char* buffer4 = leer_bytes_spa("TABLA1",2,2*MAX_LINEA,MAX_LINEA); //si es \0 no encontre la pagina
	char* buffer5 = leer_bytes_spa("TABLA1",1,8*MAX_LINEA,MAX_LINEA); //TODO probar cuando la linea supera la pagina
	dump_memory_spa("TABLA1");
	//free_spa("TABLA3", 1);
	//free_spa("TABLA1", 1);
	//free_spa("TABLA1", 2);
	//free_spa("TABLA1", 0);
	add_spa("TABLA2",1,obtenerTimeStamp());
	getPaginaMenorTimestamp();
	//free_spa("TABLA2", 0);
	//free_spa("TABLA2", 1);

	//free_spa("TABLA2", 0);//tantos free como add y siempre indice tiene que ser el numero de pagina (tener en cuenta la re-indexacion
	//dump_memory_spa("TABLA1");
	//liberar_memory_spa();*/

	tamanio_value = VALUE;
	socket_lis = connect_to_server(MEM_CONF.IP_FS, MEM_CONF.PUERTO_FS, lis,memoria);

	init_memory_spa();

	crearHiloConsola();
	crearHiloJournaling();
	crearHiloInotify();
	//TODO: Crea el hilo encargado del Gossiping - USAR EN KERNEL
	crearHiloGossipingMemoria();

	listen_connexions(MEM_CONF.PUERTO);

	exit_gracefully(EXIT_SUCCESS);
}
