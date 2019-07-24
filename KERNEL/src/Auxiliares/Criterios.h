/*
 * Criterios.h
 *
 *  Created on: 22 jul. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_CRITERIOS_H_
#define AUXILIARES_CRITERIOS_H_

#include "../KERNEL.h"

typedef enum tipoCriterio {
	SC,SHC,EV
} t_tipoCriterio;


t_tipoCriterio criterio_to_enum(char *sval);
t_tipoSeeds* obtener_memoria_lista(int numero);
int agregar_memoria_a_criterio(int nro_memoria, char* criterio);
t_tipoSeeds* get_memoria_por_criterio(char *criterio);
int existe_memoria(t_tipoSeeds* memoria, t_list* lista);

#endif /* AUXILIARES_CRITERIOS_H_ */
