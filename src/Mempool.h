/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      cnditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIPS

 File Description:

      This file contains the declarations and definition of variables
      used in the Mempool.c file.

      Note: This code is referred from a post by 2013Asker on 20140504 
      on the stackexchange website here:
      https://codereview.stackexchange.com/questions/48919/simple-memory-
      pool-using-no-extra-memory


 File Name:

      Mempool.h

 Version:

       1.2

 Abstract:

      BeDIPS uses LBeacons to deliver 3D coordinates and textual
      descriptions of their locations to users' devices. Basically, a
      LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
      coordinates and location description of every LBeacon are retrieved
      from BeDIS (Building/environment Data and Information System) and
      stored locally during deployment and maintenance times. Once
      initialized, each LBeacon broadcasts its coordinates and location
      description to Bluetooth enabled user devices within its coverage
      area.

 Authors:

      Han Wang, hollywang@iis.sinica.edu.tw


*/

#include <stdlib.h>
#include <pthread.h>

#ifndef MEMPOOL_H
#define MEMPOOL_H

#define MEMORY_POOL_SUCCESS 1
#define MEMORY_POOL_ERROR 0
/* The specified number of slots to be expanded in the memory pool */
#define EXPAND_SLOT 1024
#define MEMORY_POOL_MINIMUM_SIZE sizeof(void *)

/* The structure of the memory pool */
typedef struct {
    void **head;
    void *memory;
    /* A per list lock */
    pthread_mutex_t mem_lock;
    int size;
} Memory_Pool;



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

    This function expands the slots and allocates more memory to the memory
    pool.

  Parameters:

    mp - pointer to a specific memory pool
    slots - the number of slots in the memory pool

  Return value:

    Status - the error code or the successful message
*/

int mp_expand(Memory_Pool *mp, size_t slots);


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
  	to a slot when a free slot is available and return NULL when no free slot
  	is available.

  Parameters:

  	mp - pointer to the specific memory pool to be used

  Return value:

  	void - the pointer to the struct of a free slot or NULL
*/

void *mp_alloc(Memory_Pool *mp);


/*
  mp_free:

  	This function releases an unused slot back to the memory pool and places
  	it in the head of the free list.

  Parameters:

  	mp - the pointer to the specific memory pool
  	mem - the pointer to the strting address of the slot to be freed

  Return value:

  	Errorcode - error code or sucessful message
*/
int mp_free(Memory_Pool *mp, void *mem);

#endif
