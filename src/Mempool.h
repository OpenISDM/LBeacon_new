/*
* Copyright (c) 2016 Academia Sinica, Institute of Information Science
*
* License:
*
*      GPL 3.0 : The content of this file is subject to the terms and
*      cnditions defined in file 'COPYING.txt', which is part of this source
*      code package.
*
* Project Name:
*
*      BeDIPS
*
* File Description:
*
*      This file contains the function declarations and
*      variables used in the Mempool.c file.
*
*      Note: The code is referred to the site:
*      https://codereview.stackexchange.com/questions/48919/simple-memory-
*      pool-%20using-no-extra-memory 
*       
*
* File Name:
*
*      Mempool.h
*
* Version:
* 
*       1.2
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
*
*      Han Wang, hollywang@iis.sinica.edu.tw

*      
*/

#include <stdlib.h>

#define MEMORY_POOL_SUCCESS 1
#define MEMORY_POOL_ERROR 0
#define MEMORY_POOL_MINIMUM_SIZE sizeof(void *)

/* The structure of the memory pool */
typedef struct {
    void **head;
    void *memory;
} Memory_Pool;



/*
*  mp_init:
*
*  This function initializes the memory pool and links the slots in the pool.
*
*  Parameters:
*
*  mp - the specific memory pool to be utlized
*  size - the size of the one slots 
*  slots - the number or the size of the slots 
*
*  Return value:
*
*  Status integer - the error code or the successful message
*/
int mp_init(Memory_Pool *mp, size_t size, size_t slots);


/*
*  mp_destroy:
*
*  This function reads the specified config file line by line until the
*  end of file, and stores the data in the lines into the global variable of a
*  Config struct.
*
*  Parameters:
*
*  mp - the specific memory pool to be destroied 
*
*  Return value:
*
*  None
*
*/
void mp_destroy(Memory_Pool *mp);


/*
*  mp_alloc:
*
*  This function gets the space of the head in the memory pool and relinks
*  the head to the next node in the pool.
*
*  Parameters:
*
*  mp - the specific memory pool to be utlized
*
*  Return value:
*
*  temp - returns the pointer to the specific element 
*/
void *mp_alloc(Memory_Pool *mp);


/*
*  mp_free:
*
*  This function release the unused element back to the memory pool and place 
*  it in the head of the list.

*  Parameters:
*
*  mp - the specific memory pool to be utlized
*  mem - the specific element to be released
*
*  Return value:
*
*  none
*/
void mp_free(Memory_Pool *mp, void *mem);
