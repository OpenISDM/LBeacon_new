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
*      Han Wang, hollywang@iis.sinica.edu.tw
*	   Jake Lee, jakelee@iis.sinica.edu.tw
*      Johnson Su, johnsonsu@iis.sinica.edu.tw
*      Shirley Huang, shirley.huang.93@gmail.com
*      Han Hu, hhu14@illinois.edu
*      Jeffrey Lin, lin.jeff03@gmail.com
*      Howard Hsu, haohsu0823@gmail.com
*	   
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
#define ListEntry(ptr,type,member)	\
		((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*Macro for the method going through the list structure */
#define list_for_each(pos, head)	\
		for (pos = (head)->next; pos != (head); pos = pos->next)




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

/*
*  init_list:
*
*  This function initializes the list.
*
*  Parameters:
*
*  entry - the head of the list for determining which list is goning to be 
*  initialized.
*   
*  Return value:
*
*  None
*/
inline void init_list(List_Entry *entry);

/*
*  list_insert_:
*
*  This function changes the links between node and the added 
*  new node.
*
*  Parameters:
*
*  new_node - the struct of list entry for the node be added into the list.
*  prev - the struct of list entry which the new node points to previously.
*  next - the struct of list entry which the new node points to next.
*   
*  Return value:
*
*  None
*/
inline void list_insert_(List_Entry *new_node, List_Entry *prev,
						 List_Entry *next);

/*
*  list_insert_first:
*
*  This function calls the function of list_insert_ to add a new node at the 
*  first of the list.
*
*  Parameters:
*
*  new_node - the struct of list entry for the node be added into the list.
*  head - the struct of list entry which is the head of the list.
*   
*  Return value:
*
*  None
*/
inline void list_insert_first(List_Entry *new_node, List_Entry *head);

/*
*  list_insert_tail:
*
*  This function calls the function of list_insert_ to add a new node at the 
*  last of the list.
*
*  Parameters:
*
*  new_node - the struct of list entry for the node be added into the list.
*  head - the struct of list entry which is the head of the list.
*   
*  Return value:
*
*  None
*/
inline void list_insert_tail(List_Entry *new_node, List_Entry *head);

/*
*  list_remove_:
*
*  This function changes the links between the node and the node which 
*  is going to be removed.
*
*  Parameters:
*
*  prev - the struct of list entry for the node which is going to be deleted 
*  points to previously.
*  next - the struct of list entry for the node which is going to be deleted 
*  points to next.
*   
*  Return value:
*
*  None
*/
inline void list_remove_(List_Entry *prev, List_Entry *next);


/*
*  list_remove_node:
*
*  This function calls the function of remove_node__ to delete a node in the 
*  list.
*
*  Parameters:
*
*  removed_node_ptrs - the struct of list entry for the node is going to be
*  removed.
*  
*   
*  Return value:
*
*  None
*/
inline void list_remove_node(List_Entry *removed_node_ptrs);


/*
 *  get_list_length:
 *
 *  This function returns the length of the list. 
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  length - number of nodes in the list.
 */
inline int get_list_length(List_Entry *entry);

/*
 *  get_list_head:
 *
 *  This function peeks at the head of the list. If the list is empty, it 
 *  returns NULL because it doesn't exist. Otherwise, this function returns
 *  the data of the node at the head of the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  return_value - data of the fist node.
 */
inline void *get_list_head(List_Entry *entry);

/*
 *  get_list_tail:
 *
 *  This function peeks at the tail of the list. If the list is empty, it 
 *  returns NULL because it doesn't exist. Otherwise, this function returns
 *  the data of the node at the tail of the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  return_value - data of the last node.
 */
inline void *get_list_tail(List_Entry *entry);

/*
 *  print_list:
 *
 *  This function prints the data in the specified list in the order of head 
 *  to tail. fpitr is used to access the function to be used for printing 
 *  current node data.
 *  Note that different data types need different specifier in printf().
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  None
 */
inline void print_list(List_Entry *entry, void (*fptr)(void *));

/*
 *  free_list:
 *
 *  This function frees the resources of the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  None
 */
inline void free_list(List_Entry *entry);

