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
	t_tipoSeeds* mem = list_find(LISTA_CONN, (void *)findMemory);

	if (mem != NULL) {
		log_info(logger, "MEMMMMMMM | Memoria devuelta por FIND %d", mem->numeroMemoria);
	}
	return mem;

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

				pthread_mutex_lock(&mutex_memoria_sc);
				memoria_sc = malloc(sizeof(t_tipoSeeds));
				memcpy(memoria_sc, memoria,sizeof(t_tipoSeeds));
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
				pthread_mutex_unlock(&mutex_memoria_sc);

				resultado = 1;

			} else {
				log_info(logger, "CRITERIO| %s ya tiene memoria asignada.", criterio);
			}
			break;

		case SHC:

			pthread_mutex_lock(&mutex_memoria_shc);
			if (!existe_memoria(memoria, lista_criterio_shc)) {
				list_add(lista_criterio_shc, memoria);
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
			} else {
				log_info(logger, "CRITERIO| Memoria %d ya existe en %s.", nro_memoria, criterio);
			}
			pthread_mutex_unlock(&mutex_memoria_shc);

			resultado = 1;
			break;

		case EV:

			pthread_mutex_lock(&mutex_memoria_ev);
			if (!existe_memoria(memoria, lista_criterio_ev)) {
				list_add(lista_criterio_ev, memoria);
				log_info(logger, "CRITERIO| Memoria %d asignada a %s.", nro_memoria, criterio);
			} else {
				log_info(logger, "CRITERIO| Memoria %d ya existe en %s.", nro_memoria, criterio);
			}
			pthread_mutex_unlock(&mutex_memoria_ev);

			resultado = 1;
			break;

		default:
			printf("CRITERIO| Criterio no reconocido.\n");
	}

	return resultado;
}

t_tipoSeeds* get_memoria_por_criterio(char *criterio, int key) {
	t_tipoSeeds *memory;
	int tipo_criterio = criterio_to_enum(criterio);
	switch (tipo_criterio) {
		case SC:
			pthread_mutex_lock(&mutex_memoria_sc);
			memory = memoria_sc;
			pthread_mutex_unlock(&mutex_memoria_sc);
			break;

		case SHC:;
			int nro_memoria = get_memory_hash(key);
			memory = obtener_memoria_lista(nro_memoria);

			break;

		case EV:
			// TODO
			//memoria random
			memory = obtener_memoria_random();
			break;

	}
	return memory;
}

int existe_memoria(t_tipoSeeds* memoria, t_list* lista_criterio) {

	int existe_memoria(t_tipoSeeds* mem) {
		return mem->numeroMemoria == memoria->numeroMemoria;
	}

	return list_any_satisfy(lista_criterio, (void *) existe_memoria);
}

int get_memory_hash(int key) {

	char *key_convert = string_itoa(key);
	int * memoria_dic = dictionary_get(hashdictionary, key_convert);
	int memoria;

	if (memoria_dic == NULL) {

		int buscar(t_tipoSeeds *seed) {
			return (seed->estado == 1);
		}

		t_list *conectadas = list_filter(LISTA_CONN, (void *)buscar);

		int size_conectadas = (conectadas == NULL) ? 0 : list_size(conectadas);

		if (size_conectadas == 0)
			return 0;
		memoria = key % size_conectadas;

		dictionary_put(hashdictionary, key_convert, &memoria);

	} else
		memoria = *memoria_dic;

	log_info(logger, "Hash | Key %d Memoria %d", key, memoria);
	free(key_convert);
	return memoria;
}
