/*
 * Error.c
 *
 *  Created on: 4 may. 2019
 *      Author: utnso
 */

#include "LFS.h"
#include "Error.h"
#include "Configuracion.h"

void _exit_with_error(char* error_msg, void * buffer) {

	if (buffer != NULL)
		free(buffer);
	if (error_msg != NULL)
		loggear(logger, LOG_LEVEL_ERROR, error_msg);

	exit_gracefully(EXIT_FAILURE);
}


void exit_gracefully(int return_nr) {


  loggear(logger, LOG_LEVEL_INFO, "********* FIN DEL PROCESO LISSANDRA *********");
  list_clean(tablasGlobal);
  destruir_logger(logger);
  destroy_config(config);
  bitarray_destroy(bitmap);

  exit(return_nr);
}
