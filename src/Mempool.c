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

int mp_init(Memory_Pool *mp, size_t size, size_t slots)
{
    /* [NB] should check whether mp is NULL, return error if it is.]
       parameter check code ....
    */

    //allocate memory
    if((mp->memory = malloc(size * slots)) == NULL)
        return MEMORY_POOL_ERROR;

    //initialize
    mp->head = NULL;

    //add every slot to the list
    char *end = (char *)mp->memory + size * slots;
    /*
    [NB: Below cleverly uses the release function to link up the slots
    but an expert says it is more complicated than necessary and suggested
    the following way. */
     char *ptr;
     for (ptr = mp->memory; --slots; ptr+=size) {
        *(void **)ptr = ptr+size;
     }
     *(void **)ptr = NULL;
     mp->head = mp->memory;
    
/*
    for(char *ite = mp->memory; ite < end; ite += size)
        mp_release(mp, ite);
*/

    return MEMORY_POOL_SUCCESS;
}

void mp_destroy(Memory_Pool *mp)
{   

    mp->memory = NULL;
    free(mp->memory);

}

void *mp_get(Memory_Pool *mp)
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

/* [NB: As commented by a blogger, the function should have error check
   to make sure that mem actually points to slot.] */

void mp_release(Memory_Pool *mp, void *mem)
{
    //store first address
    void *temp = mp->head;

    //link new node
    mp->head = mem;

    //link to the list from new node
    *mp->head = temp;
}

