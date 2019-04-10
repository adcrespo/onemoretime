/*
 * memory.c
 *
 *  Created on: 31 oct. 2018
 *      Author: utnso
 */
#include "MEM.h"

#include "memory.h"
#include "file_conf.h"
/*
 * ---------------AUXILIARES--------------
 */

bool exists_pid_spa(int pid){
	int i;
	for(i=0; i<list_size(adm_frame_lista_spa);i++){
		t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa, i);
		if (adm_table->pid == pid) return true;
	}
	return false;
}

int get_free_frame() {
	int i;
	for(i=0; i<list_size(adm_frame_lista_spa); i++) {
		t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa,i);
		if(adm_table->pid<0)
			return i;
	}
	return -1;
}

bool has_available_frames_spa(int n_frames) {
	bool find(void* element) {
		t_adm_tabla_frames_spa* adm_table = element;
		return adm_table->pid < 0;
	}
	t_list* available_pages_list = list_filter(adm_frame_lista_spa, find);
	int available_pages = list_size(available_pages_list);
	list_destroy(available_pages_list);
	return available_pages >= n_frames;
}

/*
unsigned int hash(int pid, int page) {
	char* str = malloc(strlen(string_itoa(pid)));
	strcpy(str, string_itoa(pid));
	strcat(str, string_itoa(page));
	unsigned int indice = atoi(str) % frames_seg_count;
	free(str);
	return indice;
}
*/

void clean_frame_spa(int frame) {
	int start = frame * frame_spa_size;
	int end = start + frame_spa_size;
	int i;
	for (i = start; i < end; i++) {
		frames_spa[i] = '#';
	}
}

void mem_allocate_fullspace_spa() {

	int mem_size = MEM_CONF.TAM_MEM;
	frames_spa = malloc(mem_size);
	memset(frames_spa, '#', mem_size);
}

void mem_desallocate_fullspace_spa() {

	free(frames_spa);
}
/*
 * -----------------------------------------
 */

void liberar_memory_spa() {

	mem_desallocate_fullspace_spa();
	int i,j,k;
	for (i = 0; i < frames_spa_count; i++) {
		free(list_remove(adm_frame_lista_spa,0));
	}
	if(adm_frame_lista_spa!=NULL) list_destroy(adm_frame_lista_spa);

	int countSegLista = (adm_spa_lista==NULL)?0:list_size(adm_spa_lista);
	for (i = 0; i < countSegLista; i++) {
		t_adm_tabla_segmentos_spa* adm_table = list_remove(adm_spa_lista,0);
		int countSeg = list_size(adm_table->seg_lista);
		for (j = 0; j < countSeg; j++) {
			t_segmentos_spa* adm_table_seg = list_remove(adm_table->seg_lista,0);
			int countPag = list_size(adm_table_seg->pag_lista);
			for (k = 0; k < countPag; k++) {
				free(list_remove(adm_table_seg->pag_lista,0));
			}
			list_destroy(adm_table_seg->pag_lista);
			free(adm_table_seg);
		}
		list_destroy(adm_table->seg_lista);
		free(adm_table);
	}
	if(adm_spa_lista!=NULL) list_destroy(adm_spa_lista);

	frames_spa = NULL;
	frames_spa_count = 0;
	frame_spa_size = 0;

	adm_spa_lista = NULL;
	adm_frame_lista_spa = NULL;
}

void init_memory_spa() {

	int i;
	frames_spa_count = MEM_CONF.TAM_MEM / MAX_LINEA;
	frame_spa_size = MAX_LINEA;
	//TODO: frames_spa_count = FM9_CONF.TAMANIO / FM9_CONF.TAM_PAGINA;
	//TODO: frame_spa_size = FM9_CONF.TAM_PAGINA;

	mem_allocate_fullspace_spa();

	adm_frame_lista_spa = list_create();

	for (i = 0; i < frames_spa_count; i++) {
		t_adm_tabla_frames_spa* adm_table = malloc(sizeof(t_adm_tabla_frames_spa));
		adm_table->pid = -1;
		adm_table->segmento = 0;
		list_add(adm_frame_lista_spa, adm_table);
	}

	adm_spa_lista = list_create();

	/*for (i = 0; i < frames_seg_count; i++) {
		t_adm_tabla_segmentos* adm_table = malloc(sizeof(t_adm_tabla_segmentos));
		adm_table->pid = -1;
		adm_table->seg_lista = list_create();

		list_add(adm_frame_lista, adm_table);
	}*/
}

void free_spa(int pid, int segmento) {
	int i,j,k;

	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return adm_table->pid == pid;
	}

	bool find2(void* element) {

		bool find21(void* element) {
			t_paginas_spa* adm_table = element;
			return adm_table->frame != -1;
		}

		t_segmentos_spa* adm_table = element;
		return list_size(list_filter(adm_table->pag_lista,&find21));
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	t_segmentos_spa* adm_table_seg = (adm_table!=NULL)?list_get(adm_table->seg_lista, segmento):NULL;


	loggear(logger,LOG_LEVEL_DEBUG, "%s", (adm_table != NULL) && (adm_table_seg != NULL) ?
				"ENCONTRE SEGMENTOS PARA LIBERAR!" : "NO ENCONTRE SEGMMENTO PARA LIBERAR.");

	if(adm_table == NULL || adm_table_seg == NULL)
		return;

	int countPaginas = list_size(adm_table_seg->pag_lista);
	for (i = 0; i < countPaginas; i++) {
		t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista, i);
		if (adm_table_pag->frame > -1) { // TODO: porque chequeo el numero de frame
			t_adm_tabla_frames_spa* adm_table_frame = list_get(adm_frame_lista_spa,adm_table_pag->frame);

			adm_table_frame->pid = -1;
			adm_table_frame->segmento = 0;
			clean_frame_spa(adm_table_pag->frame);

			adm_table_pag->frame = -1;
		}
	}

	if(list_size(list_filter(adm_table->seg_lista,&find2))==0) {
		loggear(logger,LOG_LEVEL_DEBUG, "%s", "SE LIBERA EL ID DEL PROCESO.");
		int countSegLista = list_size(adm_spa_lista);
		for (i = 0; i < countSegLista; i++) {
			t_adm_tabla_segmentos_spa* adm_table = list_get(adm_spa_lista,0);
			if(adm_table->pid == pid){
				int countSeg = list_size(adm_table->seg_lista);
				for (j = 0; j < countSeg; j++) {
					t_segmentos_spa* adm_table_spa = list_get(adm_table->seg_lista,0);
					int countPag = list_size(adm_table_spa->pag_lista);
					for (k = 0; k < countPag; k++) {
						free(list_remove(adm_table_spa->pag_lista,0));
					}
					free(list_remove(adm_table->seg_lista,0));
				}
				free(list_remove(adm_spa_lista,0));
				break;
			}
		}
	}
}

int add_spa(int pid, int n_frames) {
	loggear(logger,LOG_LEVEL_DEBUG, "segments-pages, pid = '%d', n_frames = '%d'", pid, n_frames);

	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return adm_table->pid == pid;
	}

	int known_segmentos = 0, known_paginas = 0, i;

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);
	known_segmentos = (adm_table == NULL)?0:list_size(adm_table->seg_lista);

	//t_segmentos_spa* adm_table_seg = adm_table->seg_lista;
	//known_paginas = list_size(adm_table_seg->pag_lista);

	if(!has_available_frames_spa(n_frames)) {
		loggear(logger,LOG_LEVEL_DEBUG, "%s", "NO TENGO FRAMES DISPONIBLES.");
		return -1;
	}

	loggear(logger,LOG_LEVEL_DEBUG, "%s", "TENGO FRAMES DISPONIBLES!");

	t_adm_tabla_segmentos_spa* adm_table_new;

	if(known_segmentos==0) {
		adm_table_new =  malloc(sizeof(t_adm_tabla_segmentos_spa));
		adm_table_new->pid = pid;
		adm_table_new->seg_lista = list_create();
		list_add(adm_spa_lista, adm_table_new);
	}

	adm_table = list_find(adm_spa_lista, &find);

	t_segmentos_spa* adm_table_seg_new;

	adm_table_seg_new = malloc(sizeof(t_paginas_spa));
	adm_table_seg_new->pag_lista = list_create();
	list_add(adm_table->seg_lista,adm_table_seg_new);

	known_segmentos = (adm_table == NULL)?0:list_size(adm_table->seg_lista);

	adm_table_seg_new = list_get(adm_table->seg_lista, known_segmentos-1);
	known_paginas = (adm_table_seg_new == NULL)?0:list_size(adm_table_seg_new->pag_lista);

	//t_paginas_spa* adm_table_pag = list_get(adm_table_spa_new->pag_lista, known_paginas-1);
	loggear(logger,LOG_LEVEL_DEBUG, "%s", "adm_table_seg_new!");
	for(i=0; i<n_frames; i++) {
		t_paginas_spa* adm_table_pag_new = malloc(sizeof(t_paginas_spa));
		loggear(logger,LOG_LEVEL_DEBUG, "get_free_frame!");
		adm_table_pag_new->frame = get_free_frame();
		loggear(logger,LOG_LEVEL_DEBUG, "get_free_frame! %d",adm_table_pag_new->frame);
		list_add(adm_table_seg_new->pag_lista, adm_table_pag_new);
		known_paginas++;
		loggear(logger,LOG_LEVEL_DEBUG, "known_paginas %d",known_paginas);
		t_adm_tabla_frames_spa* adm_table_frames = list_get(adm_frame_lista_spa,adm_table_pag_new->frame);
		adm_table_frames->pid = pid;
		adm_table_frames->segmento = known_segmentos;
		loggear(logger,LOG_LEVEL_DEBUG, "%s", "adm_table_pag_new!");
	}

	return known_segmentos-1;
}

char* leer_bytes_spa(int pid, int segmento, int offset, int size) {
	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return adm_table->pid == pid;
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	t_segmentos_spa* adm_table_seg;

	do {
		adm_table_seg = list_get(adm_table->seg_lista,segmento);
		if(adm_table_seg==NULL)
			break;
		segmento += (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? 1:0;
		offset = (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? offset-(list_size(adm_table_seg->pag_lista)*frame_spa_size):offset;
	} while(offset>(list_size(adm_table_seg->pag_lista)*frame_spa_size));

	adm_table_seg = list_get(adm_table->seg_lista,segmento);

	loggear(logger,LOG_LEVEL_DEBUG, "%s",
				(adm_table_seg != NULL) ? "ENCONTRE EL SEGMENTO!" : "NO ENCONTRE ESL SEGMENTO.");

	char* buffer = string_new();
	if (adm_table_seg != NULL) {
		int page = offset/frame_spa_size, page_offset =  offset%frame_spa_size, start = 0;

		//for(i = page; i<list_size(adm_table_seg->pag_lista); i++) {
		t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista,page);
		start = adm_table_pag->frame*frame_spa_size+page_offset;
		buffer = string_substring(frames_spa, start, size);
		//}
	}
	return buffer;
}

int escribir_bytes_spa(int pid, int segmento, int offset, int size, char* buffer) {
	bool find(void* element) {
		t_adm_tabla_segmentos_spa* adm_table = element;
		return adm_table->pid == pid;
	}

	t_adm_tabla_segmentos_spa* adm_table = list_find(adm_spa_lista, &find);

	t_segmentos_spa* adm_table_seg;

	do {
		adm_table_seg = list_get(adm_table->seg_lista,segmento);
		if(adm_table_seg==NULL)
			break;
		segmento += (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? 1:0;
		offset = (offset/(list_size(adm_table_seg->pag_lista)*frame_spa_size))>=1? offset-(list_size(adm_table_seg->pag_lista)*frame_spa_size):offset;
	} while(offset>(list_size(adm_table_seg->pag_lista)*frame_spa_size));

	adm_table_seg = list_get(adm_table->seg_lista,segmento);

	loggear(logger,LOG_LEVEL_DEBUG, "%s",
				(adm_table_seg != NULL) ? "ENCONTRE EL SEGMENTO!" : "NO ENCONTRE ESL SEGMENTO.");

	int start = 0;
	if (adm_table_seg != NULL) {
		int i, page = offset/frame_spa_size, page_offset =  offset%frame_spa_size, end = 0, b = 0;

		//for (i = page; i < list_size(adm_table_seg->pag_lista); i++) {
		t_paginas_spa* adm_table_pag = list_get(adm_table_seg->pag_lista,page);
		start = adm_table_pag->frame*frame_spa_size+page_offset;
		end = start + size;
		for (i = start; i < end; i++) {
			frames_spa[i] = buffer[b];
			b++;
		}
		//}
	}
	else
		return -1;

	return start;
}

void dump_memory_spa(int pid) {
	int i;

	if(frames_spa == NULL) {
		printf("FAIL: No esta inicializada la memoria!\n");
		return;
	}

	if(!exists_pid_spa(pid)) {
		printf("FAIL: No existe el ID del proceso!\n");
		return;
	}

	char* dump_mem_struct = string_new();
	char* dump_act_process = string_new();
	char* dump_mem_content = string_new();

	if (pid == -1) {
		string_append(&dump_mem_content, frames_spa);
	} else {
		for (i = 0; i < list_size(adm_frame_lista_spa); i++) {
			t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa, i);
			if (adm_table->pid == pid) {
				char* frame = string_substring(frames_spa,
						i * frame_spa_size, frame_spa_size);
				string_append_with_format(&dump_mem_content,
						"FRAME: %d | PID: %d | SEGMENTO: %d\n%s\n",
						i, adm_table->pid, adm_table->segmento,
						frame);
			}
		}
	}

	for (i = 0; i < list_size(adm_spa_lista); i++) {
		t_adm_tabla_segmentos_spa* adm_table = list_get(adm_spa_lista, i);
		if (adm_table->pid > 0) {
			string_append_with_format(&dump_act_process, "ACTIVE PID: %d\n",
					adm_table->pid);
		}
	}

	for (i = 0; i < frames_spa_count; i++) {
		t_adm_tabla_frames_spa* adm_table = list_get(adm_frame_lista_spa, i);
		if (adm_table->pid == pid || pid < 0) {
			string_append_with_format(&dump_mem_struct,
					"FRAME: %d | PID: %d | SEGMENTO: %d\n", i,
					adm_table->pid, adm_table->segmento);
		}
	}

	char* dump_total = string_new();
	string_append_with_format(&dump_total, "TABLA DE SEGMENTOS:\n%s\n",
			dump_mem_struct);
	string_append_with_format(&dump_total, "LISTADO DE PROCESOS ACTIVOS:\n%s\n",
			dump_act_process);
	string_append_with_format(&dump_total, "CONTENIDO MEMORIA:\n%s\n\n",
			dump_mem_content);


	printf("%s\n", dump_total);
	loggear(logger,LOG_LEVEL_DEBUG,"\n>PID: %d\n\n%s\n", pid, dump_total);

	free(dump_act_process);
	free(dump_mem_content);
	free(dump_mem_struct);
	free(dump_total);
}

