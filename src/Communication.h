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
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "BeDIS.h"

/*
  Wifi_init:

     This function initializes the Wifi's necessory object.

  Parameters:

     udp_config - The struct of UDP configuration setting

  Return value:

      ErrorCode - The error code for the corresponding error or successful

 */
ErrorCode Wifi_init(sudp_config_beacon *udp_config);

/*
  receive_data:

    This is the entry function of worker threads. In LBeacon, we create
    multiple worker threads within thread pool to start from this function
    to receive data from gateway.
    This function receives packets sent by the gateway via Wifi UDP
    connection, creates a node to store the content and inserts the node
    to the receive packet queue.

  Parameters:

    udp_config - The struct of UDP configuration setting


  Return value:

     ErrorCode - The error code for the corresponding error or successful

*/

ErrorCode receive_data(void *udp_config);

/*
  send_data:

    This is the entry function of worker threads. In LBeacon, we create
    multiple worker threads within thread pool to start from this function
    to send data to gateway.
    This function prepares a socket, retrieves a packet from the send packet
    queue and sends the packet to the gateway via Wifi UDP connection.

  Parameters:

    udp_config - The struct of UDP configuration setting

  Return value:

    ErrorCode - The error code for the corresponding error or successful

*/
ErrorCode *send_data(void *udp_config);

/*
  Wifi_free:

     When called, this function frees the elements specified by the input.

  Parameters:

    udp_config - The struct of UDP configuration setting

  Return value:

    ErrorCode - The error code for the corresponding error or successful
 */
ErrorCode Wifi_free(sudp_config_beacon *udp_config);



#endif
