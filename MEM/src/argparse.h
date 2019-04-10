/*
 * argparse.h
 *
 *  Created on: 28 oct. 2018
 *      Author: utnso
 */

#ifndef ARGPARSE_H_
#define ARGPARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* args_configfile;
char* args_verbose;

char *getArg(int argc, char** argv, char* key);
void initArgumentos(int argc, char** argv);
void destroy_args();
#endif /* ARGPARSE_H_ */
