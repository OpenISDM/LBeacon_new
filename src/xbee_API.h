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
 *   	This file contains the header of  function declarations and variable
 *      used in xbee_API.c
 *
 * File Name:
 *
 *      xbee_API.h
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xbee.h>
#include "pkt_Queue.h"

#ifndef xbee_API_H
#define xbee_API_H

/* A variable to get error code from zigbee library*/
xbee_err ret;


/* The function for xbee initialization */
xbee_err xbee_initial(char* xbee_mode, char* xbee_device, int xbee_baudrate
                        , int LogLevel, struct xbee** xbee, pkt_ptr pkt_Queue);

/* A function for setting up xbee connection */
xbee_err xbee_connector(struct xbee** xbee, struct xbee_con** con
                                                , pkt_ptr pkt_Queue);

/* CallBack for Data Received */
void CallBack(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt
, void **data);

#endif
