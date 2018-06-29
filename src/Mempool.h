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

//size must be greater than or equal to MEMORY_POOL_MINIMUM_SIZE

int mp_init(Memory_Pool *mp, size_t size, size_t slots);
void mp_destroy(Memory_Pool *mp);

/* [NB: For Lbeacon, each slot needs to hold a ScannedDevice struct. 
    Clearly, no struct should straddle two cache lines. So, slot size
    should be size of the struct rounded up to an integer multiple of
    cache line size. Cache line size of RPI W0 is supposedly 32 bytes.]
*/

void *mp_get(Memory_Pool *mp);
void mp_release(Memory_Pool *mp, void *mem);

// [NB: We should call the function mp_alloc and mp_free.]