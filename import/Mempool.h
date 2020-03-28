/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     Mempool.h

  File Description:

     This file contains the declarations and definition of variables used in
     the Mempool.c file.

     Note: This code is referred from a post by 2013Asker on 20140504 on the
     stackexchange website here:
     https://codereview.stackexchange.com/questions/48919/simple-memory-pool-
     using-no-extra-memory

  Version:

      2.0, 20190415

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

     Holly Wang, hollywang@iis.sinica.edu.tw
 */

#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* When debugging is needed */
//#define debugging

#define MEMORY_POOL_SUCCESS 1
#define MEMORY_POOL_ERROR 0
#define MEMORY_POOL_MINIMUM_SIZE sizeof(void *)
#define MAX_EXP_TIME 10

/* The structure of the memory pool */
typedef struct {
    /* The head of the unused slots */
    void **head;

    /* An array stores the head of each malloced memory */
    void *memory[MAX_EXP_TIME];

    /* Counting current malloc times */
    int alloc_time;

    /* A per list lock */
    pthread_mutex_t mem_lock;

    /* The size of each slots in byte */
    int size;

    /* The number of slots is made each time the mempool expand */
    int slots;

    int blocks;
    
    /* counter for calculating the slots usage */
    int used_slots;

} Memory_Pool;


/*
  get_current_size_mempool:

     This function returns the current size of the memory pool.

  Parameters:

     mp - pointer to a specific memory pool

  Return value:

     mem_size- the current size of the memory pool
 */
size_t get_current_size_mempool(Memory_Pool *mp);


/*
  mp_init:

     This function allocates memory and initializes the memory pool and links
     the slots in the pool.

  Parameters:

     mp - pointer to a specific memory pool
     size - the size of slots in the pool
     slots - the number of slots in the memory pool

  Return value:

     Status - the error code or the successful message
 */
int mp_init(Memory_Pool *mp, size_t size, size_t slots);


/*
  mp_expand:

     This function expands the number of slots and allocates more memory to the
     memory pool.

  Parameters:

     mp - pointer to a specific memory pool

  Return value:

     Status - the error code or the successful message
 */
int mp_expand(Memory_Pool *mp);


/*
  mp_destroy:

     This function frees the memory occupied by the specified memory pool.

  Parameters:

     mp - pointer to the specific memory pool to be destroyed

  Return value:

     None

 */
void mp_destroy(Memory_Pool *mp);


/*
  mp_alloc:

     This function gets a free slot from the memory pool and returns a pointer
     to the slot when a free slot is available and return NULL when no free slot
     is available.

  Parameters:

     mp - pointer to the specific memory pool to be used

  Return value:

     void - the pointer to the struct of a free slot or NULL
 */
void *mp_alloc(Memory_Pool *mp);


/*
  mp_free:

     This function releases a slot back to the memory pool.

  Parameters:

     mp - the pointer to the specific memory pool
     mem - the pointer to the strting address of the slot to be freed

  Return value:

     Errorcode - error code or sucessful message
 */
int mp_free(Memory_Pool *mp, void *mem);

/*
  mp_slots_usage_percentage:

     This function calculates the memory pool slots usage in percentage

  Parameters:

     mp - the pointer to the specific memory pool

  Return value:

     float - the memory pool slots usage in percentage
*/
float mp_slots_usage_percentage(Memory_Pool *mp);

#endif