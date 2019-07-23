/*
 * Criterios.c
 *
 *  Created on: 22 jul. 2019
 *      Author: utnso
 */

#include "Criterios.h"


char* criterio_str[] = {
		"SC"
		, "SHC"
		, "EV"
		, NULL
};

t_tipoCriterio criterio_to_enum(char *sval) {
	t_tipoCriterio result = SC;
	int i = 0;
	for (i = 0; criterio_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, criterio_str[i]))
			return result;
	return -1;
}

t_tipoSeeds* obtener_memoria_lista(int numero){

	int findMemory(t_tipoSeeds * memoria) {
		log_info(logger, "Memoria %d en LISTA_CONN", memoria->numeroMemoria);
		log_info(logger, "Numero buscado %d", numero);
		log_info(logger, "Igualdad: %d", memoria->numeroMemoria == numero);
		return memoria->numeroMemoria == numero;
	}


	log_info(logger, "Buscando memoria %d en LISTA_CONN", numero);
//	return list_find(LISTA_CONN, &findMemory);
	return list_find(LISTA_CONN, (void *)findMemory);

}
