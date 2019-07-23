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

int agregar_memoria_a_criterio(int nro_memoria, char* criterio) {

	int resultado = -1;

	// Valido si existe la memoria -- CHECKEAR SI SE NECESITA QUE ESTE CONECTADA
	t_tipoSeeds* memoria = obtener_memoria_lista(nro_memoria);

	if (memoria == NULL) {
		log_info(logger, "CRITERIO| La memoria %d no existe.", nro_memoria);
		return resultado;
	}

	int e_criterio = criterio_to_enum(criterio);

	switch(e_criterio){

		case SC:

			if(memoria_sc == NULL){
				memoria_sc = memoria;
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
				resultado = 1;
			}else{
				log_info(logger, "CRITERIO| %s ya tiene memoria asignada.", criterio);
			}
			break;

		case SHC:

			if (!existe_memoria(memoria, lista_criterio_shc)) {
				list_add(lista_criterio_shc, memoria);
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
			} else {
				log_info(logger, "CRITERIO| Memoria %d ya existe en %s.", nro_memoria, criterio);
			}

			resultado = 1;
			break;

		case EV:

			if (!existe_memoria(memoria, lista_criterio_ev)) {
				list_add(lista_criterio_ev, memoria);
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
			} else {
				log_info(logger, "CRITERIO| Memoria %d ya existe en %s.", nro_memoria, criterio);
			}

			resultado = 1;
			break;

		default:
			printf("CRITERIO| Criterio no reconocido.\n");
	}

	return resultado;
}

int existe_memoria(t_tipoSeeds* memoria, t_list* lista_criterio) {

	int existe_memoria(t_tipoSeeds* mem) {
		return mem->numeroMemoria == memoria->numeroMemoria;
	}

	return list_any_satisfy(lista_criterio, (void *) existe_memoria);
}
