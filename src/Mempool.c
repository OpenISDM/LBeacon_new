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
*      This file contains the program to allow the necessory memory 
*      allocation for the nodes in the linked list. 
*
*      Note: The code is referred to the site:
*      https://codereview.stackexchange.com/questions/48919/simple-memory-
*      pool-%20using-no-extra-memory 
*       
*
* File Name:
*
*      Mempool.c
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
*/#include "Mempool.h"

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

int mp_init(Memory_Pool *mp, size_t size, size_t slots)
{
  

    //allocate memory
    if((mp->memory = malloc(size * slots)) == NULL)
        return MEMORY_POOL_ERROR;

    //initialize
    mp->head = NULL;

    //add every slot to the list
     char *ptr;
     for (ptr = mp->memory; --slots; ptr+=size) {
        *(void **)ptr = ptr+size;
     }
     *(void **)ptr = NULL;
     mp->head = mp->memory;
    

    return MEMORY_POOL_SUCCESS;
}



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

void mp_destroy(Memory_Pool *mp)
{   

    mp->memory = NULL;
    free(mp->memory);

}



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

void *mp_alloc(Memory_Pool *mp)
{
    if(mp->head == NULL)
        return NULL;

    //store first address, i.e., address of the start of first element
    void *temp = mp->head;

    //link one past it
    mp->head = *mp->head;

    //return the first address
    return temp;
}



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

void mp_free(Memory_Pool *mp, void *mem)
{
    //store first address
    void *temp = mp->head;

    //link new node
    mp->head = mem;

    //link to the list from new node
    *mp->head = temp;
}

