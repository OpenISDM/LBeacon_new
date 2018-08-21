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
#include <errno.h>
#include "zlog.h"
#include "xbee_API.h"



/* Length of timeout in number of milliseconds */
#define XBEE_TIMEOUT 2000000

#define XBEE_MODE "xbeeZB"

#define XBEE_DEVICE "/dev/ttyAMA0"

#define XBEE_DATASTREAM -1

#define XBEE_CONFIG_PATH "../config/xbee_config.conf"

/* The pointer to the category of the log file */
zlog_category_t *category_health_report, *category_debug;

/* Struct for storing necessary objects for zigbee connection */
sxbee_config xbee_config;


/* The enumeration of the polled data */
typedef enum PolledDataType {

    NOT_YET_POLLED = 0,
    TRACK_OBJECT_DATA = 1,
    HEALTH_REPORT = 2,
    MAX_NO_DATA_TYPES = 3

} PolledDataType;




/* The enumeration of the error code */
typedef enum ErrorCode_XBee {

    XBEE_SUCCESSFULLY = 0,
    E_XBEE_VALIDATE = 1,
    E_CALL_BACK = 2,
    E_CONNECT = 3,
    E_SHUT_DOWN = 4

} ErrorCode_Xbee;

struct _errordesc_xbee {
    int code;
    char *message;
} errord_xbee[] = {

    {XBEE_SUCCESSFULLY, "The xbee works successfullly"},
    {E_XBEE_VALIDATE, "Error validating xbee"},
    {E_CALL_BACK, "Error enabling call back function for xbee"},
    {E_CONNECT, "Error buildiing xbee connection"},
    {E_SHUT_DOWN, "Error shutting down xbee"}

    
};


/*
  zigbee_init:

    This function initilizes the zigbee's necessory object.  

  Parameters:

    zigbee - the struct of necessary parameter and data


  Return value:

    ErrorCode: The error code for the corresponding error or successful

*/
ErrorCode_Xbee zigbee_init();


/*
  receive_call_back:

    This function receives a pointer to the packet sent by the gateway and 
    return an indicatior for the polled data type of the packet.   

  Parameters:

    zigbee - the struct of necessary parameter and data


  Return value:

    polled_data: An indicator of the polled data

*/

int receive_call_back();

/*
  zigbee_send_file:

    When called, this function sends a packet containing the specified 
    message to the gateway via xbee module.

  Parameters:

    zigbee - the struct of necessary parameter and data

  Return value:

    None

*/
void zigbee_send_file(char *zig_message);

/*
  zigbee_free:

    When called, this function frees the necessory element.

  Parameters:

    zigbee - the struct of necessary parameter and data

  Return value:

    ErrorCode: The error code for the corresponding error

*/
ErrorCode_Xbee zigbee_free();