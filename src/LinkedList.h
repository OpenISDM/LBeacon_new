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
*	   Han Wang, hollywang@iis.sinica.edu.tw
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
* CONSTANTS
*/

/*Macro for calculating the offset of two addresses*/
#define offsetof(type, member) ((size_t) &((type *)0)->member)

/*Macro for geting the master struct from the sub struct */
#define ListEntry(ptr,type,member)((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*Macro for the method going through the list structure */
#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)




/*Strcut for the head of list containing two pointers: next and prev */
typedef struct List_Entry {	
	struct List_Entry *next;
	struct List_Entry *prev;
	
}List_Entry;

/* Struct for each node in the list with any type of data */
typedef struct Node {
	void *data;
	struct List_Entry ptrs;
	
}Node;


/*
* FUNCTIONS
*/


/* The function changes the links between node and the added new node.*/
inline void list_insert_(List_Entry *new_node, List_Entry *prev, List_Entry *next);

/* The function calls the function of list_insert_ to add a new node at the 
 * first of the list.*/
inline void list_insert_head(List_Entry *new_node, List_Entry *head);

/* The function calls the function of list_insert_ to add a new node at the 
 * last of the list.*/
inline void list_insert_tail(List_Entry *new_node, List_Entry *head);

/* The function changes the links between the node and the node which 
 * is going to be removed.*/
inline void list_remove_(List_Entry *prev, List_Entry *next);

/* The function calls the function of remove_node__ to delete a node in the 
 * list.*/
inline void list_remove_node(List_Entry *removed_node_ptrs);

/* The function returns the length of the list. */
inline int get_list_length(List_Entry *entry);
