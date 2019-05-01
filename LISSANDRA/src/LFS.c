/*
 ============================================================================
 Name        : LISSANDRA.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "LFS.h"

int main(void) {

	logger = configurar_logger("../log/LIS.log", "Lissandra");

	cargar_conf_lis();


	return EXIT_SUCCESS;
}
