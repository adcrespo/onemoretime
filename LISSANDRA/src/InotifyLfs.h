/*
 * InotifyLfs.h
 *
 *  Created on: 14 jul. 2019
 *      Author: utnso
 */

#ifndef INOTIFYLFS_H_
#define INOTIFYLFS_H_

#include "LFS.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

pthread_t inotify;

int crearHiloInotify();


#endif /* INOTIFYLFS_H_ */
