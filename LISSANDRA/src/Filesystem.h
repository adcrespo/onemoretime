/*
 * Filesystem.h
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "LFS.h"

void *CrearFileSystem();
void CargarMetadata();
void CargarBitmap();
int ExisteTabla(const char*tabla);
int CalcularParticion(int clave, int particiones);
void CrearDirectorioTabla(char *tabla);
void CrearMetadataTabla(char *tabla, char *consistencia, int particiones, int tiempoCompactacion);
int InsertarTabla(t_request *request);
void CrearBloque(int numero, int bytes);
void BuscarKeyBloque(int key, char *archivo);
int ContarElementosArray(char **cadena);
void BuscarKeyBloque(int key, char *tabla);
void BuscarKey(int key, char *tabla);
int AgregarBloque();
int CrearTabla(t_create *msgCreate);
void ObtenerMetadataCompleto();
int DropearTabla(char *nombre);
int calcularBloques(int bytes);
void liberarBloques(char **bloques, int cantBloques);
void liberarMetadata(char **bloques, int cant);
void BuscarKeyMemtable(int key, char *nombre);
void LevantarHilosCompactacionFS();

t_config *config_metadata;
t_bitarray *bitmap;



int tamanio_bloques;
int cantidad_bloques;
char *rutaMetadata;
char *rutaBitmap;
char *rutaTablas;
char *rutaBloques;
char *bmap;



#endif /* FILESYSTEM_H_ */
