/*
 * Copyright (c) 2016 Academia Sinica, Institute of Information Science
 *
 * License:
 *
 *      GPL 3.0 : The content of this file is subject to the terms and
 *      cnditions defined in file 'COPYING.txt', which is part of this
 *      source code package.
 *
 * Project Name:
 *
 *      BeDIPS
 *
 * File Description:
 *
 *   	This file contains the header of  function declarations and variable
 *      used in xbee_log.c
 *
 * File Name:
 *
 *      xbee_log.h
 *
 * Abstract:
 *
 *      BeDIPS uses LBeacons to deliver 3D coordinates and textual
 *      descriptions of their locations to users' devices. Basically, a
 *      LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
 *      coordinates and location description of every LBeacon are retrieved
 *      from BeDIS (Building/environment Data and Information System) and
 *      stored locally during deployment and maintenance times. Once
 *      initialized, each LBeacon broadcasts its coordinates and location
 *      description to Bluetooth enabled user devices within its coverage
 *      area.
 *
 * Authors:
 *      Gary Xiao		, garyh0205@hotmail.com
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <string.h>
 #include <unistd.h>
 #include <time.h>


#ifndef pkt_log_H
#define pkt_log_H

#define Max_Log_Length 1024

#define collect_info __FILE__, __LINE__, __func__

typedef struct xbee_log_header{

    FILE* log_ptr;

    char* log_location;

    char* log_filename;

    bool locker;

} sxbee_log;

typedef sxbee_log* pxbee_log;

int init_log_file(pxbee_log xbee_log, char* log_location, char* log_filename);

int add_log(pxbee_log xbee_log, const char* filename, const int line
          , const char* funcname, char* content, bool in_lock);

int del_older_log(pxbee_log xbee_log);

int open_log_file(pxbee_log xbee_log, char* type, bool in_lock);

int close_log_file(pxbee_log xbee_log);

int release_log_struct(pxbee_log xbee_log);

#endif
