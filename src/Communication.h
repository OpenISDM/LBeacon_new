/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      conditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIS

 File Description:

      This header file contains the declarations of the function  and
      variables used in the Communication.c file.

 File Name:

      Communication.h

 Version:

       2.0,  20190103

 Abstract:

      BeDIS uses LBeacons to deliver 3D coordinates and textual
      descriptions of their locations to users' devices. Basically, a LBeacon
      is an inexpensive, Bluetooth Smart Ready device. The 3D coordinates and
      location description of every LBeacon are retrieved from BeDIS
      (Building/environment Data and Information System) and stored locally
      during deployment and maintenance times. Once initialized, each LBeacon
      broadcasts its coordinates and location description to Bluetooth
      enabled user devices within its coverage area.

 Authors:

      Holly Wang, hollywang@iis.sinica.edu.tw
      Gary Xiao, garyh0205@hotmail.com
	  Chun Yu Lai, chunyu1202@gmail.com

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


/*
  Wifi_init:

     This function initilizes the Wifi's necessory object.

  Parameters:

     udp_config - The struct of UDP configuration setting

  Return value:

      int - The error code for the corresponding error or successful

 */
int Wifi_init(sudp_config_beacon *udp_config);

/*
  receive_data:

    This is the entry function of worker thread. In LBeacon, we create 
    multiple worker threads within thread pool to start from this function
    to receive data from gateway. 
    This function receives the packet sent by the gateway via Wifi UDP 
    connection, creates a temporary node of queue to store the content and 
    inserts this node to receive packet queue.

  Parameters:

    udp_config - The struct of UDP configuration setting


  Return value:
    
     int - The error code for the corresponding error or successful

*/

int receive_data(void *udp_config);

/*
  send_data:

    This is the entry function of worker thread. In LBeacon, we create 
    multiple worker threads within thread pool to start from this function 
    to send data to gateway. 
    This function prepares a socket, retrieves one packet from send packet
    queue and sends this packet to gateway via Wifi UDP connection.

  Parameters:

    udp_config - The struct of UDP configuration setting

  Return value:

    None

*/
void *send_data(void *udp_config);

/*
  Wifi_free:

     When called, this function frees the necessory element.

  Parameters:

    udp_config - The struct of UDP configuration setting

  Return value:

     None
 */
void Wifi_free(sudp_config_beacon *udp_config);



#endif
