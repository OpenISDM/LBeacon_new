/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      cnditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIPS

 File Description:

      This file contains the programs to allow data transmission between
       LBeacon and Gateway.

 File Name:

      Communication.c

 Version:

       1.2, 20181114

 Abstract:

      BeDIPS uses LBeacons to deliver 3D coordinates and textual
      descriptions of their locations to users' devices. Basically, a
      LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
      coordinates and location description of every LBeacon are retrieved
      from BeDIS (Building/environment Data and Information System) and
      stored locally during deployment and maintenance times. Once
      initialized, each LBeacon broadcasts its coordinates and location
      description to Bluetooth enabled user devices within its coverage
      area.

 Authors:

      Holly Wang, hollywang@iis.sinica.edu.tw
      Gary Xiao, garyh0205@hotmail.com

*/

#include "Communication.h"
#define Debugging

int Wifi_init(){


    /* Initialize the Wifi cinfig file */
    if(udp_initial(&udp_config) != WORK_SUCCESSFULLY){

        /* Error handling TODO */
        return E_WIFI_INIT_FAIL;
    }
    return WORK_SUCCESSFULLY;
}


int receive_call_back(){

    sPkt temp;
    temp = udp_getrecv(&udp_config);

    return NOT_YET_POLLED;

}

void *send_data(char *message){

    printf("Message:~~~~~~~~~~~~~~~~~~~~~~~ %s \n", message);

    int i = receive_call_back();

    udp_addpkt(&udp_config, "140.109.22.248", message, 1024);


   return;
}


void Wifi_free(){

    /* Release the Wifi elements and close the connection. */
    udp_release(&udp_config);
    return;
}
