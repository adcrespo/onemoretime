/*
 * journaling.h
 *
 *  Created on: 12 may. 2019
 *      Author: utnso
 */

#ifndef JOURNALING_H_
#define JOURNALING_H_

#include <signal.h>
#include <shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>

void process_journaling();

void *crearHiloJournaling();

pthread_t journaling;
pthread_mutex_t journalingMutex;

#endif /* JOURNALING_H_ */