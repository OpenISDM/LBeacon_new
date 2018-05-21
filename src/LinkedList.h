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
 *      This header file contains the function declarations and
 *      variables used in the LinkedList.c file.
 *
 * File Name:
 *
 *      LinkedList.h
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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Length of a Bluetooth MAC address */
#define LENGTH_OF_MAC_ADDRESS 18

/* Struct for storing scanned timestamp and MAC address of the user's device */
typedef struct ScannedDevice {
    long long initial_scanned_time;
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
} ScannedDevice;

/* Struct for each node in the linked list; data is a ScannedDevice struct */
typedef struct LinkedListNode {
    ScannedDevice data;
    struct LinkedListNode *next;
    struct LinkedListNode *prev;
} LinkedListNode;

/* Pointer to the LinkedListNode at the head of the linked list */
extern LinkedListNode *linked_list_head;

/* Pointer to the LinkedListNode at the end of the linked list */
extern LinkedListNode *linked_list_tail;

/* Pointer to current LinkedListNode of the linked list */
extern LinkedListNode *linked_list_current;

/*
 * FUNCTIONS
 */

void insert_first(ScannedDevice data, int mac_address_length);
void insert_last(ScannedDevice data, int mac_address_length);
void delete_node(ScannedDevice data);
int get_linked_list_length();
void print_linked_list();