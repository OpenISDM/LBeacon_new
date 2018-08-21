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
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <xbee.h>
#include "pkt_Queue.h"
#include "xbee_Serial.h"

#ifndef xbee_API_H
#define xbee_API_H

typedef struct xbee_config {

    //Universal
    char*   xbee_device;
    char    Local_Address[17];

    //Serial
    int    xbee_datastream;
    char*   config_location;

    //API
    char* xbee_mode;
    struct xbee *xbee;
    struct xbee_con *con;
    spkt_ptr pkt_Queue, Received_Queue;

} sxbee_config;

typedef sxbee_config* pxbee_config;

enum{File_OPEN_ERROR = -1,};

/*
 *
 * xbee_initial
 *
 *     For initialize zigbee, include loading config.
 *
 * Parameter:
 *
 *     xbee_config: A structure contain all variables for xbee.
 *
 * Return Value:
 *
 *     xbee_err: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 *
 */
xbee_err xbee_initial(pxbee_config xbee_config);

/*
 * xbee_LoadConfig
 *
 *     For initialize zigbee, include loading config to xbee.
 *
 * Parameter:
 *
 *     xbee_config: A structure contain all variables for xbee.
 *
 * Return Value:
 *     xbee_err: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 */
int xbee_LoadConfig(pxbee_config xbee_config);

/*
 * xbee_connector
 *
 *     For connect to zigbee and assign it's destnation address.
 *
 * Parameter:
 *
 *     xbee_config: A structure contain all variables for xbee.
 *
 * Return Value:
 *
 *     xbee_err: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 *
 */
xbee_err xbee_connector(pxbee_config xbee_config);

/* xbee_send_pkt
 *
 *      A function for sending pkt to dest address.
 *
 * Parameter:
 *
 *     xbee_config: A structure contain all variables for xbee.
 *
 * Return Value:
 *
 *      xbee_err: If return 0, everything work successfully.
 *                If not 0, something wrong.
 *
 */
xbee_err xbee_send_pkt(pxbee_config xbee_config);

/*
 * xbee_check_CallBack
 *
 *      Check if CallBack is disabled and pkt_Queue is NULL.
 *
 * Parameter:
 *
 *     xbee_config: A structure contain all variables for xbee.
 *     exclude_pkt_Queue : If true, ignore pkt_Queue.
 *
 * Return Value:
 *
 *      True if CallBack is disabled and pkt_Queue is NULL(if exclude_pkt_Queue
 *      is false), else false.
 *
 */
bool xbee_check_CallBack(pxbee_config xbee_config, bool exclude_pkt_Queue);

/*
 * xbee_release
 *
 *      Release All pkt and mutex.
 *
 * Parameter:
 *
 *     xbee_config: A structure contain all variables for xbee.
 *
 * Return Value:
 *
 *      xbee_err: If return 0, everything work successfully.
 *                If not 0, something wrong.
 *
 */
xbee_err xbee_release(pxbee_config xbee_config);

/* ---------------------------callback Section------------------------------  */
/* It will be executed once for each packet that is received on               */
/* an associated connection                                                   */
/* -------------------------------------------------------------------------  */

void CallBack(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt
, void **data);

#endif
