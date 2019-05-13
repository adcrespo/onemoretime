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

char* intToChar4(int num){
	//RECORDAR: liberar el puntero con free()
	char* n_char = string_new();//malloc(sizeof(int));
	memcpy(n_char, &num, sizeof(int));
	return n_char;
}

int main(int argc, char *argv[]) {

	initArgumentos(argc, argv);

	logger = configurar_logger_verbose("MEM.log", "MEM", string_equals_ignore_case(args_verbose,"true")?true:false);

	get_config(string_equals_ignore_case(args_configfile,"false")?"../MEM.conf":args_configfile);

	init_memory_spa();

	int paginas1 = add_spa("TABLA1",10,time(NULL)); //Siempre devuelve pag o seg + 1 ; si es 0 no reservo nada
	int paginas2 = add_spa("TABLA1",1,time(NULL));
	getPaginaMenorTimestamp();
	int paginas4 = add_spa("TABLA2",2,time(NULL));
	int paginas3 = add_spa("TABLA1",1,time(NULL));
	getPaginaMenorTimestamp();
	int escrito1 = escribir_bytes_spa("TABLA1",1,1*MAX_LINEA,5,"CHAU\n",0); //offset siempre deberia ser multiplo de max_linea
	int escrito2 = escribir_bytes_spa("TABLA1",0,0,5,"HOLA\n",0);
	int escrito3 = escribir_bytes_spa("TABLA1",0,1*MAX_LINEA,5,"COMO\n",0);
	int escrito4 = escribir_bytes_spa("TABLA1",2,2*MAX_LINEA,5,"FAIL\n",0); //si es -1 no encontre la pagina
	int escrito5 = escribir_bytes_spa("TABLA1",1,8*MAX_LINEA,5,"FAIL\n",0); //TODO probar cuando la linea supera la pagina
	char* buffer1 = leer_bytes_spa("TABLA1",0,0,MAX_LINEA); //size siempre deberia ser max_liena y offset siempre deberia ser multiplo de max_linea
	char* buffer2 = leer_bytes_spa("TABLA1",0,MAX_LINEA,MAX_LINEA);
	char* buffer3 = leer_bytes_spa("TABLA1",1,0,MAX_LINEA);
	char* buffer4 = leer_bytes_spa("TABLA1",2,2*MAX_LINEA,MAX_LINEA); //si es \0 no encontre la pagina
	char* buffer5 = leer_bytes_spa("TABLA1",1,8*MAX_LINEA,MAX_LINEA); //TODO probar cuando la linea supera la pagina
	dump_memory_spa("TABLA1");
	/*free_spa("TABLA3", 1);
	free_spa("TABLA1", 1);
	free_spa("TABLA1", 2);
	free_spa("TABLA1", 0);
	add_spa("TABLA2",1,time(NULL));
	getPaginaMenorTimestamp();
	free_spa("TABLA2", 0);
	free_spa("TABLA2", 1);
	free_spa("TABLA2", 0);//tantos free como add y siempre indice tiene que ser el numero de pagina (tener en cuenta la re-indexacion
	dump_memory_spa("TABLA1");
	liberar_memory_spa();*/

	crearHiloConsola();
	crearHiloJournaling();
//	connect_to_server(MEM_CONF.IP_FS, MEM_CONF.PUERTO_FS, lis);

	listen_connexions(MEM_CONF.PUERTO);

	exit_gracefully(EXIT_SUCCESS);
}
