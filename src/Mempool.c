/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      cnditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIPS

 File Description:

      This file contains the program to allow the necessory memory 
      allocation for the nodes in the linked list. 

      Note: The code is referred to the site:
      https://codereview.stackexchange.com/questions/48919/simple-memory-
      pool-%20using-no-extra-memory 
       

 File Name:

      Mempool.c

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

#include "Mempool.h"



int mp_init(Memory_Pool *mp, size_t size, size_t slots)
{
  
    
    //allocate memory
    if((mp->memory = malloc(size * slots)) == NULL){    
      
      return MEMORY_POOL_ERROR;
    
    }
 
    //initialize
    mp->head = NULL;
    mp->size = size;

    //add every slot to the free list
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


int mp_expand(Memory_Pool *mp, size_t slots)
{
    void *new_mem;
   
    new_mem = malloc(mp->size * slots);
    if(new_mem == NULL ){
        
        return MEMORY_POOL_ERROR;
        
    }
    
    //add every slot to the free list
    char *end = (char *)new_mem + mp->size * slots;
    
    for(char *ite = new_mem; ite < end; ite += mp->size){
        
       
        //store first address
        void *temp = mp->head;
       
        //link the new node
        mp->head = ite;
       
        //link to the list from new node
        *mp->head = temp;
       
        
    }
    
    return MEMORY_POOL_SUCCESS;
    
    
}

void mp_destroy(Memory_Pool *mp)
{   

    mp->memory = NULL;
    free(mp->memory);

}



void *mp_alloc(Memory_Pool *mp)
{
         
    if( *mp->head == NULL){
       
      if(mp_expand(mp, EXPAND_SLOT) == MEMORY_POOL_ERROR){

        return NULL;

      }
           
    }
        
    //store first address, i.e., address of the start of first element
    void *temp = mp->head;

    //link one past it
    mp->head = *mp->head;

    //return the first address
    return temp;
}



int mp_free(Memory_Pool *mp, void *mem)
{
  
    //check if mem is correct, i.e. is pointing to the struct of a slot
    //calculate the offset from mem to mp->memory
    int diffrenceinbyte = (mem - mp->memory) * sizeof(mem);
    
    if((diffrenceinbyte % mp->size) != 0){  
        
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

