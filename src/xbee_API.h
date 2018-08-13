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
#include "xbee_log.h"

#ifndef xbee_API_H
#define xbee_API_H

/* A variable to get error code */
xbee_err ret;

/*
 * xbee_initial
 *     For initialize zigbee, include loading config.
 * Parameter:
 *     xbee_mode: we use xbeeZB as our device, this parameter is for setting
 *                libxbee3 work mode.
 *     xbee_device: This parameter is to define where is our zigbee device path.
 *     xbee_baudrate: This parameter is to define what our zigbee working
 *                    baudrate.
 *     xbee: A pointer to catch zigbee pointer.
 *     pkt_Queue: A structure store the packet we decide to send.
 *     Received_Queue: A structure store the packet we received.
 * Return Value:
 *     xbee_err: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 */
xbee_err xbee_initial(char* xbee_mode, char* xbee_device, int xbee_baudrate
                    , struct xbee** xbee, pkt_ptr pkt_Queue
                    , pkt_ptr Received_Queue);


/*
 * xbee_connector
 *     For connect to zigbee and assign it's destnation address.
 * Parameter:
 *     xbee: A pointer to catch zigbee pointer.
 *     con: A pointer of the connector of zigbee.
 *     pkt_Queue: A structure store the packet we decide to send.
 *     Received_Queue: A structure store the packet we received.
 * Return Value:
 *     xbee_err: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 */
xbee_err xbee_connector(struct xbee** xbee, struct xbee_con** con
                      , pkt_ptr pkt_Queue, pkt_ptr Received_Queue);

/* xbee_send_pkt
 *      A function for sending pkt to dest address.
 * Parameter:
 *      xbee: A pointer to catch zigbee pointer.
 *      con: A pointer of the connector of zigbee.
 *      pkt_Queue: A structure store the packet we decide to send.
 *      Received_Queue: A structure store the packet we received.
 * Return Value:
 *      xbee_err: If return 0, everything work successfully.
 *                If not 0, something wrong.
 */
xbee_err xbee_send_pkt(struct xbee_con* con, pkt_ptr pkt_Queue);

/*
 * xbee_check_CallBack
 *      Check if CallBack is disabled and pkt_Queue is NULL.
 * Parameter:
 *      con : a pointer for xbee connector.
 *      pkt_Queue : A pointer point to the packet queue we use.
 * Return Value:
 *      True if CallBack is disabled and pkt_Queue is NULL, else false.
 *
 */
bool xbee_check_CallBack(struct xbee_con* con, pkt_ptr pkt_Queue
                                             , bool exclude_pkt_Queue);

xbee_err xbee_release(struct xbee* xbee, struct xbee_con* con
                      , pkt_ptr pkt_Queue, pkt_ptr Received_Queue);

/* ---------------------------callback Section------------------------------  */
/* It will be executed once for each packet that is received on               */
/* an associated connection                                                   */
/* -------------------------------------------------------------------------  */

void CallBack(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt
, void **data);

#endif
