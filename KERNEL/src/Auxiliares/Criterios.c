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

t_tipoSeeds* obtener_memoria_lista_SHC(int numero){

	int findMemory(t_tipoSeeds * memoria) {
		log_info(logger, "CRITERIOS BUSQUEDA SHC | Memoria %d en LISTA_CONN", memoria->numeroMemoria);
		log_info(logger, "CRITERIOS BUSQUEDA SHC | Numero buscado %d", numero);
		log_info(logger, "CRITERIOS BUSQUEDA SHC | Igualdad: %d", memoria->numeroMemoria == numero);
		return memoria->numeroMemoria == numero;
	}

	log_info(logger, "Buscando memoria %d en lista_criterio_shc", numero);
//	return list_find(LISTA_CONN, &findMemory);
	pthread_mutex_lock(&mutex_memoria_shc);
	t_tipoSeeds* mem = list_get(lista_criterio_shc,numero);
//	t_tipoSeeds* mem = list_find(lista_criterio_shc, (void *)findMemory);
	pthread_mutex_unlock(&mutex_memoria_shc);

	if (mem != NULL) {
		log_info(logger, "CRITERIOS BUSQUEDA SHC | Memoria devuelta por FIND %d", mem->numeroMemoria);
	}
	else
	{
		mem = malloc(sizeof(t_tipoSeeds));
		memset(mem, 0x00, sizeof(t_tipoSeeds));
		mem->numeroMemoria = -1;
		log_info(logger, "CRITERIOS BUSQUEDA SHC NO ENCONTRADA | Memoria devuelta por FIND %d", mem->numeroMemoria);
	}
	return mem;

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
	pthread_mutex_lock(&mutex_LISTA_CONN);
	t_tipoSeeds* mem = list_find(LISTA_CONN, (void *)findMemory);
	pthread_mutex_unlock(&mutex_LISTA_CONN);

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

				t_tipoSeeds* memoriaSHC = malloc(sizeof(t_tipoSeeds));
				memcpy(memoriaSHC, memoria,sizeof(t_tipoSeeds));

				list_add(lista_criterio_shc, memoriaSHC);
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

				t_tipoSeeds* memoriaEV = malloc(sizeof(t_tipoSeeds));
				memcpy(memoriaEV, memoria,sizeof(t_tipoSeeds));

				list_add(lista_criterio_ev, memoriaEV);
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
			if (memoria_sc == NULL) {
				memory = malloc(sizeof(t_tipoSeeds));
				memset(memory, 0x00, sizeof(t_tipoSeeds));
				memory->numeroMemoria = -1;
				return memory;
			}
			memory = memoria_sc;
			pthread_mutex_unlock(&mutex_memoria_sc);
			break;

		case SHC:;
			int nro_memoria = get_memory_hash_SHC(key);

			memory = obtener_memoria_lista_SHC(nro_memoria);

			//TODO: HACER JOURNALING A TODAS LAS MEMORIAS

			break;

		case EV:
			// TODO
			//memoria random
			memory = obtener_memoria_random();
			if (memory->numeroMemoria <0)
			{
				log_info(logger, "PLANIFIC| MEMORIA: %d",memory->numeroMemoria);
				return memory;
			}
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

		pthread_mutex_lock(&mutex_LISTA_CONN);
		t_list *conectadas = list_filter(LISTA_CONN, (void *)buscar);
		pthread_mutex_unlock(&mutex_LISTA_CONN);

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

int get_memory_hash_SHC(int key) {

	char *key_convert = string_itoa(key);
	log_info(logger, "Hash GET| Key GET %s", key_convert);
	int * memoria_dic = dictionary_get(hashdictionary, key_convert);

	if (memoria_dic == NULL) {

		pthread_mutex_lock(&mutex_memoria_shc);
		int size_conectadas = list_size(lista_criterio_shc);
		pthread_mutex_unlock(&mutex_memoria_shc);

		log_info(logger, "Hash| tamaño lista_criterio_shc  %d", size_conectadas);

		if (size_conectadas == 0)
			return 0;

		int * memoria = malloc(sizeof(int));

		*memoria = key % size_conectadas;

		log_info(logger, "Hash Dictionary PUT| Key %d Memoria %d", key, *memoria);

		dictionary_put(hashdictionary, key_convert, memoria);
		memoria_dic = memoria;
	} else {

		log_info(logger, "Hash Dictionary GET| Key %d Memoria %d", key, *memoria_dic);
	}

	log_info(logger, "Hash Result| Key %d Memoria %d", key, *memoria_dic);
	free(key_convert);
	return *memoria_dic;
}
