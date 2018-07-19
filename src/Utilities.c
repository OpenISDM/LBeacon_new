/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science
 
  License:
 
      GPL 3.0 : The content of this file is subject to the terms and
       conditions defined in file 'COPYING.txt', which is part of this source
       code package.
 
  Project Name:
 
       BeDIPS
 
  File Description:
 
       This file contains various utilities functions included in BlueZ, the
       official Linux Bluetooth protocol stack.
 
  File Name:
 
       Utilities.c
 
  Abstract:
 
       BeDIPS uses LBeacons to deliver 3D coordinates and textual
       descriptions of their locations to users' devices. Basically, a LBeacon
       is an inexpensive, Bluetooth Smart Ready device. The 3D coordinates and
       location description of every LBeacon are retrieved from BeDIS
       (Building/environment Data and Information System) and stored locally
       during deployment and maintenance times. Once initialized, each LBeacon
       broadcasts its coordinates and location description to Bluetooth
       enabled user devices within its coverage area.
 
  Authors:
 
       Jake Lee, jakelee@iis.sinica.edu.tw
       Johnson Su, johnsonsu@iis.sinica.edu.tw
       Shirley Huang, shirley.huang.93@gmail.com
       Han Hu, hhu14@illinois.edu
       Jeffrey Lin, lin.jeff03@gmail.com
       Howard Hsu, haohsu0823@gmail.com
 */

#include "Utilities.h"

/* Initialize flag that is used to check if CTRL-C is pressed */
bool g_done = false;


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


void ctrlc_handler(int stop) { g_done = true; }