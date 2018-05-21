/*
 * Copyright (c) 2016 Academia Sinica, Institute of Information Science
 *
 * License:
 *
 *      GPL 3.0 : The content of this file is subject to the terms and
 *      conditions defined in file 'COPYING.txt', which is part of this source
 *      code package.
 *
 * Project Name:
 *
 *      BeDIPS
 *
 * File Description:
 *
 *      This is the header file containing the function declarations and
 *      variables used in the Queue.c file.
 *
 * File Name:
 *
 *      Queue.h
 *
 * Abstract:
 *
 *      BeDIPS uses LBeacons to deliver 3D coordinates and textual
 *      descriptions of their locations to users' devices. Basically, a LBeacon
 *      is an inexpensive, Bluetooth Smart Ready device. The 3D coordinates and
 *      location description of every LBeacon are retrieved from BeDIS
 *      (Building/environment Data and Information System) and stored locally
 *      during deployment and maintenance times. Once initialized, each LBeacon
 *      broadcasts its coordinates and location description to Bluetooth
 *      enabled user devices within its coverage area.
 *
 * Authors:
 *
 *      Jake Lee, jakelee@iis.sinica.edu.tw
 *      Johnson Su, johnsonsu@iis.sinica.edu.tw
 *      Shirley Huang, shirley.huang.93@gmail.com
 *      Han Hu, hhu14@illinois.edu
 *      Jeffrey Lin, lin.jeff03@gmail.com
 *      Howard Hsu, haohsu0823@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>

/* Length of a Bluetooth MAC address*/
#define LENGTH_OF_MAC_ADDRESS 18

/* Struct for each node in the queue; data is the MAC address */
typedef struct QueueNode {
    char data[LENGTH_OF_MAC_ADDRESS];
    struct QueueNode *next;
} QueueNode;

/* Pointer to the QueueNode at the head of the queue */
extern QueueNode *queue_head;

/* Pointer to the QueueNode at the tail of the queue */
extern QueueNode *queue_tail;

/*
 * FUNCTIONS
 */

void enqueue(char data[]);
void dequeue();
char *peek();
void print_queue();