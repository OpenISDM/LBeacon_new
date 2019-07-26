/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and cnditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     global_variable.h

  File Description:

     This file include the global variables and definition that used in BeDIS 
     and Server but not always commonly used in Gateway and LBeacon.

  Version:

     2.0, 20190718

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
      
     Chun-Yu Lai  , chunyu1202@gmail.com
 */

#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H

/* Number of times to retry opening socket, because socket openning operation
   may have transient failure. */
#define SOCKET_OPEN_RETRY 5

/* Timeout in seconds for UDP recevie socket */
#define TIMEOUT_UDP_RECEIVCE_IN_SEC 5

/* Time interval in micro seconds for busy-wait checking in threads */
#define INTERVAL_FOR_BUSY_WAITING_CHECK_IN_MICRO_SECONDS 100000

/* The configuration file structure */
/* The following BeaconConfig structure is not used by LBeacon, and it is 
   here for compatible with BeDIS common library. */
typedef struct {

    int number_worker_threads;
    int time_critical_priority;
} BeaconConfig;

/* Global variables */
/* A Gateway config struct for storing config parameters from the config file */
BeaconConfig config;


#endif
