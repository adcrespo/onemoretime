/*
 * Parser.c
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */

//#include "Parser.h"
//
//char* request_str[] = {
//		"SELECT"
//		, "INSERT"
//		, "CREATE"
//		, "DESCRIBE"
//		, "DROP"
//		, "JOURNAL"
//		, "ADD"
//		, "RUN"
//		, "METRICS"
//		, "SALIR"
//		, NULL };
//
//t_request* parsear(char* linea) {
//
//	t_request* request = malloc(sizeof(t_request));
//
//	// Inicializo los strings
//	request->parametro1 = string_new();
//	request->parametro2 = string_new();
//	request->parametro3 = string_new();
//	request->parametro4 = string_new();
//
//	// 1. CHEQUEAR PORQUE CUANDO RECIBO PARAMETRO ENTRE "" NO LO TOMA BIEN -----------------------
//	// 2. CHEQUEAR LOS PARAMETROS NECESARIOS PARA QUE NO FALLE CON SEG FAULT ---------------------
//	char* comando = strtok(linea, " ");
//
//	request->request = string_to_enum(comando);
//
//	if (request->request == -1) {
//
//		log_error(logger, "Comando desconocido: %s.", comando);
//
//	} else {
//
//		log_info(logger, "Comando: %s.", comando);
//
//		switch (request->request) {
//
//				case _salir:
//					// No hacer nada
//					break;
//
//				case _select:
//					string_append(&request->parametro1, strtok(NULL, " "));
//					string_append(&request->parametro2, strtok(NULL, " "));
//
//					log_info(logger, "Parámetro 1: %s", request->parametro1);
//					log_info(logger, "Parámetro 2: %s", request->parametro2);
//					break;
//
//				case _insert:
//					string_append(&request->parametro1, strtok(NULL, " "));
//					string_append(&request->parametro2, strtok(NULL, " "));
//					string_append(&request->parametro3, strtok(NULL, " "));
//
//					log_info(logger, "Parámetro 1: %s", request->parametro1);
//					log_info(logger, "Parámetro 2: %s", request->parametro2);
//					log_info(logger, "Parámetro 3: %s", request->parametro3);
//					break;
//
//				case _create:
//					string_append(&request->parametro1, strtok(NULL, " "));
//					string_append(&request->parametro2, strtok(NULL, " "));
//					string_append(&request->parametro3, strtok(NULL, " "));
//					string_append(&request->parametro4, strtok(NULL, " "));
//
//					log_info(logger, "Parámetro 1: %s", request->parametro1);
//					log_info(logger, "Parámetro 2: %s", request->parametro2);
//					log_info(logger, "Parámetro 3: %s", request->parametro3);
//					log_info(logger, "Parámetro 4: %s", request->parametro4);
//					break;
//
//				case _describe:
//					string_append(&request->parametro1, strtok(NULL, " "));
//
//					log_info(logger, "Parámetro 1: %s", request->parametro1);
//					break;
//
//				case _drop:
//					string_append(&request->parametro1, strtok(NULL, " "));
//
//					log_info(logger, "Parámetro 1: %s", request->parametro1);
//					break;
//
//				case _journal:
//					// No tiene parámetros
//					break;
//
//				case _add:
//					string_append(&request->parametro1, strtok(NULL, " "));
//					string_append(&request->parametro2, strtok(NULL, " "));
//					string_append(&request->parametro3, strtok(NULL, " "));
//					string_append(&request->parametro4, strtok(NULL, " "));
//
//					log_info(logger, "Parámetro 1: %s", request->parametro1);
//					log_info(logger, "Parámetro 2: %s", request->parametro2);
//					log_info(logger, "Parámetro 3: %s", request->parametro3);
//					log_info(logger, "Parámetro 4: %s", request->parametro4);
//					break;
//
//				case _run:
//					string_append(&request->parametro1, strtok(NULL, " "));
//
//					log_info(logger, "Parámetro 1: %s", request->parametro1);
//					break;
//
//				case _metrics:
//					// No tiene parámetros
//					break;
//
//				default:
//					// No entra por acá porque se valida antes el enum != -1
//					break;
//			}
//
//	}
//
//	return request;
//
//}
//
//
//// Auxiliares
//t_request_enum string_to_enum(char *sval) {
//	t_request_enum result = _select;
//	int i = 0;
//	for (i = 0; request_str[i] != NULL; ++i, ++result)
//		if (0 == strcmp(sval, request_str[i]))
//			return result;
//	return -1;
//}
//
//char* enum_to_string(t_request_enum comando) {
//	return request_str[comando];
//}
