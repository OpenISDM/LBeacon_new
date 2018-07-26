/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      conditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIPS

 File Description:

      This header file contains the function declarations and
      variables used in the Communication.c file.

 File Name:

      Communication.h

 Version:
 
       1.2

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
#include "xbee_API.h"

/* The length of the message to be sent to the gateway */
#define MESSAGE_LENGTH 512

#define XBEE_TIMEOUT 2000000

#define XBEE_SUCCESSFULLY 1
#define XBEE_ERROR 0

/* Struct of parameters for Zigbee Initialization */
typedef struct Zigbee {

    /* Struct of xbee main part which is defined in "libxbee" library */
    struct xbee *xbee; 

    /* Struct of xbee connector which is defined in "libxbee" library */
    struct xbee_con *con;

    /* Struct of queue of packet which is defined in pkt_Queue.h */
    pkt_ptr pkt_Queue;

    /* The message to be sent to the gateway. */
    char zig_message[MESSAGE_LENGTH];
    
} Zigbee;



/*
  zigbee_init:

  This function initilizes the zigbee's necessory object.  

  Parameters:

  zigbee - the struct of necessary parameter and data


  Return value:

  ErrorCode: The error code for the corresponding error

*/
int zigbee_init(Zigbee zigbee);


int receive_call_back(Zigbee zigbee);

/*
  zigbee_send_file:

  When called, this function sends a containing the specified message packet 
  to the gateway via xbee module and and receives command or data from the 
  gateway. 

  Parameters:

  zigbee - the struct of necessary parameter and data

  Return value:

  None

*/
void *zigbee_send_file(Zigbee zigbee);

/*
  zigbee_free:

  When called, this function frees the necessory element.

  Parameters:

  zigbee - the struct of necessary parameter and data

  Return value:

  ErrorCode: The error code for the corresponding error

*/
void zigbee_free(Zigbee zigbee);