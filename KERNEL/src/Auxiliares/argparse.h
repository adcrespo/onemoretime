/*
 * argparse.h
 *
 *  Created on: 10 jul. 2019
 *      Author: utnso
 */

#ifndef AUXILIARES_ARGPARSE_H_
#define AUXILIARES_ARGPARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

char* args_configfile;
char* args_verbose;

void initArgumentos(int argc, char** argv);
void destroy_args();

#endif /* AUXILIARES_ARGPARSE_H_ */
