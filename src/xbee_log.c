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
 *   	This file contains the program to store all log to log file.
 *
 * File Name:
 *
 *      xbee_log.c
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

#include "xbee_log.h"

int init_log_file(pxbee_log xbee_log, char* log_location, char* log_filename){

    xbee_log -> locker = false;

    bool status;

    do{

        status = xbee_log -> locker;

        xbee_log -> locker = true;

    } while(status == true);

    xbee_log -> log_location = malloc((strlen(log_location) + 1) * sizeof(char));

    memset(xbee_log -> log_location, 0
         , sizeof((strlen(log_location) + 1) * sizeof(char)));

    strncpy(xbee_log -> log_location, log_location
          , strlen(log_location) * sizeof(char));

    xbee_log -> log_filename = malloc((strlen(log_filename) + 1) * sizeof(char));

    memset(xbee_log -> log_filename, 0
         , (strlen(log_filename) + 1) * sizeof(char));

    strncpy(xbee_log -> log_filename, log_filename
                                , (strlen(log_filename) + 1) * sizeof(char));

    xbee_log -> log_ptr = NULL;

    add_log(xbee_log, collect_info, "init_log_file Success", true);

    char concat_location[1000];

    sprintf(concat_location,"%s/%s\0", xbee_log -> log_location
          , xbee_log -> log_filename);

    add_log(xbee_log, collect_info, "::Log_Location::", true);

    add_log(xbee_log, collect_info, concat_location, true);

    xbee_log -> locker = false;

    return 0;

}

int add_log(pxbee_log xbee_log, const char* filename, const int line, const char* funcname
          , char* content, bool in_lock){

    bool status;

    do{

        status = xbee_log -> locker;

        xbee_log -> locker = true;

    } while(status == true && !in_lock);

    bool opend = false;

    time_t timep;

    time (&timep);

    if(xbee_log -> log_ptr == NULL){

        open_log_file(xbee_log, "a", true);

    }

    else{

        opend = true;

    }

    char sline_num[10];

    sprintf(sline_num, "%d", line );

    char DateTime[100];

    strncpy(DateTime, ctime(&timep), (strlen(ctime(&timep)) -1)*sizeof(char));

    DateTime[strlen(ctime(&timep)) -1] = '\0';

    fputs( DateTime, xbee_log->log_ptr);
    fputs(" | ", xbee_log->log_ptr);
    fputs(funcname, xbee_log->log_ptr);
    fputs("() in Line ", xbee_log->log_ptr);
    fputs(sline_num, xbee_log->log_ptr);
    fputs(" (In File: ",xbee_log->log_ptr);
    fputs(filename, xbee_log->log_ptr);
    fputs(") | ", xbee_log->log_ptr);
    fputs(content, xbee_log->log_ptr);
    fputs("\n", xbee_log->log_ptr);

    if(!opend)

        close_log_file(xbee_log);

    if(!in_lock)

        xbee_log -> locker = false;

    return 0;

}

int open_log_file(pxbee_log xbee_log, char* type, bool in_lock){

    bool status;

    do{

        status = xbee_log -> locker;

        xbee_log -> locker = true;

    } while(status == true && !in_lock);

    char concat_location[1000];

    sprintf(concat_location,"%s/%s\0", xbee_log -> log_location
          , xbee_log -> log_filename);

    xbee_log -> log_ptr = fopen(concat_location, type);

    xbee_log -> locker = false;

    return 0;

}

int close_log_file(pxbee_log xbee_log){

    bool status;

    do{

        status = xbee_log -> locker;

        xbee_log -> locker = true;

    } while(status == true);

    if(xbee_log -> log_ptr == NULL){

        xbee_log -> locker = false;

        return 0;

    }

    fclose(xbee_log -> log_ptr);

    xbee_log -> log_ptr = NULL;

    xbee_log -> locker = false;

    return 0;

}

int release_log_struct(pxbee_log xbee_log){

    bool status;

    do{

        status = xbee_log -> locker;

        xbee_log -> locker = true;

    } while(status == true);

    add_log(xbee_log, collect_info, "release_log_struct ... END", true);

    free(xbee_log -> log_filename );

    free(xbee_log -> log_location);

    xbee_log -> locker = false;

    return 0;
}
