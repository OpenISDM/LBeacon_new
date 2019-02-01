/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and cnditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     BeDIS.c

  File Description:

     This file contains code of functions used in both Gateway and LBeacon.

  Version:

     2.0, 20190201

  Abstract:

     BeDIS uses LBeacons to deliver 3D coordinates and textual descriptions of
     their locations to users' devices. Basically, a LBeacon is an inexpensive,
     Bluetooth Smart Ready device. The 3D coordinates and location description
     of every LBeacon are retrieved from BeDIS (Building/environment Data and
     Information System) and stored locally during deployment and maintenance
     times. Once initialized, each LBeacon broadcasts its coordinates and
     location description to Bluetooth enabled user devices within its coverage
     area.

  Authors:

     Gary Xiao     , garyh0205@hotmail.com
     Joey Zhou     , joeyzhou5566@gmail.com
     Holly Wang    , hollywang@iis.sinica.edu.tw
     Jake Lee      , jakelee@iis.sinica.edu.tw
     Chun Yu Lai   , chunyu1202@gmail.com

 */

#include "BeDIS.h"


unsigned int *uuid_str_to_data(char *uuid) {
    char conversion[] = "0123456789ABCDEF";
    int uuid_length = strlen(uuid);
    unsigned int *data =
        (unsigned int *)malloc(sizeof(unsigned int) * uuid_length);

    if (data == NULL) {
        /* Error handling */
        perror("Failed to allocate memory");
        return NULL;
    }

    unsigned int *data_pointer = data;
    char *uuid_counter = uuid;

    for (; uuid_counter < uuid + uuid_length;

         data_pointer++, uuid_counter += 2) {
        *data_pointer =
            ((strchr(conversion, toupper(*uuid_counter)) - conversion) * 16) +
            (strchr(conversion, toupper(*(uuid_counter + 1))) - conversion);

    }

    return data;
}


unsigned int twoc(int in, int t) {

    return (in < 0) ? (in + (2 << (t - 1))) : in;
}


void trim_string_tail(char *message) {

    int idx = 0;

    /* discard the whitespace, newline, carry-return characters at the end */
    if(strlen(message) > 0){

        idx = strlen(message) - 1;
        while(10 == message[idx] ||
                13 == message[idx] ||
                32 == message[idx]){

           message[idx] = '\0';
           idx--;
        }
    }
}


void ctrlc_handler(int stop) { ready_to_work = false; }


ErrorCode startThread(pthread_t *thread, void *( *start_routine)(void *),
                      void *arg){

    pthread_attr_t attr;

    if ( pthread_attr_init( &attr) != 0 ||
         pthread_create(thread, &attr, start_routine, arg) != 0 ||
         pthread_detach( *thread)){

          printf("Start Thread Error.\n");
          return E_START_THREAD;
    }

    return WORK_SUCCESSFULLY;

}


int get_system_time() {
    /* A struct that stores the time */
    struct timeb t;

    /* Return value as a long long type */
    int system_time;

    /* Convert time from Epoch to time in milliseconds of a long long type */
    ftime(&t);

    /* millisecond ver. */
    /* system_time = 1000 * (long long)t.time + (long long)t.millitm; */

    /* second ver. */
    system_time = (int)t.time;

    return system_time;
}
