/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      conditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIPS

 File Description:

      This header file contains the declarations of the function  and
      variables used in the Communication.c file.

 File Name:

      Communication.h

 Version:

       1.2,  20181114

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

      Han Wang, hollywang@iis.sinica.edu.tw
      Gary Xiao, garyh0205@hotmail.com

*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#ifndef BEDIS_H
#include "BeDIS.h"
#endif

#ifndef COMMUNICATION_H
#define COMMUNICATION_H



/* The pointer to the category of the log file */
zlog_category_t *category_health_report, *category_debug;



/* The enumeration of the polled data */
typedef enum PolledDataType {

    NOT_YET_POLLED = 5,
    TRACK_OBJECT_DATA = 6,
    HEALTH_REPORT = 7,
    MAX_NO_DATA_TYPES = 8

} PolledDataType;




/*
  receive_call_back:

    This function receives the packet sent by the gateway and return an
    indicator for the data as the polled type of the packet.

  Parameters:

    None


  Return value:

    polled_data: An indicator of the polled data

*/

int receive_call_back();

/*
  send_data:

    When called, this function sends a packet that containing the specified
    message to the gateway via xbee module.

  Parameters:

    message - the message to be sent via xbee module

  Return value:

    None

*/
void *send_data(char *message);



#endif
