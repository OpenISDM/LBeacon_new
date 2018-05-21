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
 *      This file contains the implementation of a queue data structure. It has
 *      the feature of enqueueing a node to the front of the queue and
 *      dequeueing a node to the end of the queue. It can also peek at the
 *      node in the front of the queue and print a list of all the nodes in the
 *      queue. The purpose of this queue implementation is to allow scanned MAC
 *      addresses to wait
 *
 * File Name:
 *
 *      Queue.c
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

#include "Queue.h"

/* Initialize pointer to the QueueNode at the head of the queue */
QueueNode *queue_head = NULL;

/* Initialize pointer to the QueueNode at the tail of the queue */
QueueNode *queue_tail = NULL;

/*
 *  enqueue:
 *
 *  This function will allocate a QueueNode with the passed in data and at the
 *  tail, insert the node at the end of the queue. This will allow the queue to
 *  be FIFO.
 *
 *  Parameters:
 *
 *  data - scanned MAC address
 *
 *  Return value:
 *
 *  None
 */
void enqueue(char data[]) {
    int mac_address_iterator;

    /* Create a temporary node */
    struct QueueNode *temp =
        (struct QueueNode *)malloc(sizeof(struct QueueNode));

    /* Copy data passed into the function into the new node */
    for (mac_address_iterator = 0; mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
         mac_address_iterator++) {
        temp->data[mac_address_iterator] = data[mac_address_iterator];
    }
    temp->next = NULL;

    /* If queue is empty, set both head and tail to the new node */
    if (queue_head == NULL && queue_tail == NULL) {
        queue_head = queue_tail = temp;
        return;
    }

    /* If queue is not empty, only set the tail to the new node */
    queue_tail->next = temp;
    queue_tail = temp;
    free(temp);
}

/*
 *  dequeue:
 *
 *  This function will remove the head node of the queue. Since the head will be
 *  removed, we need to rename the next node as the head.
 *
 *  Parameters:
 *
 *  None
 *
 *  Return value:
 *
 *  None
 */
void dequeue() {
    /* Start from the first node */
    struct QueueNode *temp = queue_head;

    /* If head if empty, exit */
    if (queue_head == NULL) {
        return;
    }

    /* If found a node with matching data, update node */
    if (queue_head == queue_tail) {
        queue_head = queue_tail = NULL;
    } else {
        queue_head = queue_head->next;
    }
    free(temp);
}

/*
 *  peek:
 *
 *  This function will peek at the head of the queue. If the queue is empty,
 *  peek() will return NULL because it doesn't exist. Otherwise, this function
 *  will return the MAC address of the node at the front of the queue.
 *
 *  Parameters:
 *
 *  None
 *
 *  Return value:
 *
 *  return_value - MAC address of the head QueueNode
 */
char *peek() {
    /* If head if empty, exit */
    if (queue_head == NULL) {
        return NULL;
    }

    /* Return MAC address of the first QueueNode */
    char *return_value = &queue_head->data[0];
    return return_value;
}

/*
 *  print_queue:
 *
 *  This function will print all the MAC addresses in the queue. When printing,
 *  the MAC addresses will be in order from head to tail.
 *
 *  Parameters:
 *
 *  None
 *
 *  Return value:
 *
 *  None
 */
void print_queue() {
    /* Start from the first node */
    struct QueueNode *temp = queue_head;

    /* Start from the beginning and stop when the next node doesn't exist */
    printf("%s", "Queue: ");
    while (temp != NULL) {
        printf("%s ", &temp->data[0]);
        temp = temp->next;
    }
    printf("\n");
}