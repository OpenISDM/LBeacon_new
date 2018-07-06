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
*  slots - the number of slots in the memory pool
*
*  Return value:
*
*  Status - the error code or the successful message
*/

int mp_init(Memory_Pool *mp, size_t size, size_t slots)
{
  

    //allocate memory
    if((mp->memory = malloc(size * slots)) == NULL)
        return MEMORY_POOL_ERROR;

    //initialize
    mp->head = NULL;
    mp->size = size;

     //add every slot to the list
    char *end = (char *)mp->memory + size * slots;
    
    for(char *ite = mp->memory; ite < end; ite += size){

            //store first address
            void *temp = mp->head;

            //link the new node
            mp->head = ite;

            //link to the list from new node
            *mp->head = temp;
  
    }
    

    return MEMORY_POOL_SUCCESS;
}



/*
*  mp_destroy:
*
*  This function frees te memory occupied by the specified memory pool.
*
*  Parameters:
*
*  mp - the specific memory pool to be destroyed 
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
*  This function gets a free slot from the memory pool when a free slot
*  is available and return NULL when no free slot is available.
*
*  Parameters:
*
*  mp - the specific memory pool to be utlized
*
*  Return value:
*
*  void - the pointer to the struct of a free slot or NULL 
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
*  This function release an unused slot back to the memory pool and place 
*  it in the head of the free list.

*  Parameters:
*
*  mp - the pointer to the specific memory pool
*  mem - the pointer to the strting address of the slot to be freed
*
*  Return value:
*
*  Errorcode - error code or sucessful message 
*/

int mp_free(Memory_Pool *mp, void *mem)
{
    

    int offset = &mp->memory - &mem;
    int diffrenceinbyte = offset * sizeof(int);
    printf("Offset: %d \n", diffrenceinbyte);
    
    if(diffrenceinbyte / mp->size != 0){  
        printf("Error \n");
        return MEMORY_POOL_ERROR;

    }

    //store first address
    void *temp = mp->head;

    //link new node
    mp->head = mem;

    //link to the list from new node
    *mp->head = temp;

    return MEMORY_POOL_SUCCESS; 
}

