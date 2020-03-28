/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     Mempool.c

  File Description:

     This file contains the program to allow memory allocation for structs of
     identical size.

     Note: The code is referred to the site:
     https://codereview.stackexchange.com/questions/48919/simple-memory-pool-
     %20using-no-extra-memory

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

#include "Mempool.h"


size_t get_current_size_mempool(Memory_Pool *mp){

    size_t mem_size;

    pthread_mutex_lock(&mp->mem_lock);

    mem_size = mp->alloc_time * mp->size * mp->slots;

    pthread_mutex_unlock(&mp->mem_lock);

    return mem_size;
}


int mp_init(Memory_Pool *mp, size_t size, size_t slots){

    char *end;
    char *ite;
    void *temp;
    int return_value;

    /* initialize and set parameters */
    mp->head = NULL;
    mp->size = size;
    mp->slots = slots;
    mp->used_slots = 0;
    mp->alloc_time = 0;
    mp->blocks = 0;

    pthread_mutex_init( &mp->mem_lock, 0);

    return_value = mp_expand(mp);

#ifdef debugging
    zlog_info(category_debug, 
              "[Mempool] Current MemPool [%d]\n[Mempool] Remain blocks [%d]", 
              mp, mp->blocks);
#endif

    return return_value;
}


int mp_expand(Memory_Pool *mp){

    int alloc_count;
    char *end;
    void *temp;
    char *ite;

    alloc_count = mp->alloc_time;

    if(alloc_count == MAX_EXP_TIME)
        return MEMORY_POOL_ERROR;

    mp->memory[alloc_count] = malloc(mp->size * mp->slots);
    
    if(mp->memory[alloc_count] == NULL )
        return MEMORY_POOL_ERROR;

    memset(mp->memory[alloc_count], 0, mp->size * mp->slots);

    /* add every slot to the free list */
    end = (char *)mp->memory[alloc_count] + mp->size * mp->slots;

    for(ite = mp->memory[alloc_count]; ite < end; ite += mp->size){

        /* store first address */
        temp = mp->head;

        /* link the new node */
        mp->head = (void *)ite;

        /* link to the list from new node */
        *mp->head = temp;

        mp->blocks ++;

    }

    mp->alloc_time ++;

#ifdef debugging
    zlog_info(category_debug, 
              "[Mempool] Current MemPool [%d]\n[Mempool] Remain blocks [%d]", 
              mp, mp->blocks);
#endif

    return MEMORY_POOL_SUCCESS;
}


void mp_destroy(Memory_Pool *mp){

    int i;

    pthread_mutex_lock( &mp->mem_lock);

    for(i = 0; i < MAX_EXP_TIME; i++){

        mp->memory[i] = NULL;
        free(mp->memory[i]);
    }

    mp->head = NULL;
    mp->size = 0;
    mp->slots = 0;
    mp->alloc_time = 0;
    mp->blocks = 0;

#ifdef debugging
    zlog_info(category_debug, 
              "[Mempool] Current MemPool [%d]\n[Mempool] Remain blocks [%d]", 
              mp, mp->blocks);
#endif

    pthread_mutex_unlock( &mp->mem_lock);

    pthread_mutex_destroy( &mp->mem_lock);

}


void *mp_alloc(Memory_Pool *mp){

    void *temp;

    /*zlog_info(category_debug, "[mp_alloc] Attemp to mp_alloc, current " \
                "blocks = [%d], current alloc times = [%d]", mp->blocks, 
                mp->alloc_time);
                */
    pthread_mutex_lock(&mp->mem_lock);

    if(mp->head == NULL){

        /* If the next position which mp->head is pointing to is NULL,
           expand the memory pool. */
        if(mp_expand(mp) == MEMORY_POOL_ERROR){

            pthread_mutex_unlock(&mp->mem_lock);
            return NULL;
        }
    }

    /* store first address, i.e., address of the start of first element */
    temp = mp->head;

    /* link one past it */
    mp->head = *mp->head;
    
    // count the slots usage
    mp->used_slots = mp->used_slots + 1;

    mp->blocks --;

#ifdef debugging
    zlog_info(category_debug, 
              "[Mempool] Current MemPool [%d]\n[Mempool] Remain blocks [%d]", 
              mp, mp->blocks);
#endif

    memset(temp, 0, mp->size);

    pthread_mutex_unlock( &mp->mem_lock);

    /* return the first address */
    return temp;

}


int mp_free(Memory_Pool *mp, void *mem){

    int closest = -1;
    int i;
    int differenceinbyte;
    void *temp;

    pthread_mutex_lock(&mp->mem_lock);

    /* Check all the expanded memory space, to find the closest and
    most relevant mem_head for the current freeing memory. */
    for(i = 0; i < mp->alloc_time; i++){

        /* Calculate the offset from mem to mp->memory */
        differenceinbyte = (int)mem - (int)mp->memory[i];
        /* Only consider the positive offset */
        if((differenceinbyte > 0) && ((differenceinbyte < closest) ||
           (closest == -1)))
            closest = differenceinbyte;
    }
    /* check if mem is correct, i.e. is pointing to the struct of a slot */
    if((closest % mp->size) != 0){
        pthread_mutex_unlock(&mp->mem_lock);
        return MEMORY_POOL_ERROR;
    }

    memset(mem, 0, mp->size);

    /* store first address */
    temp = mp->head;
    /* link new node */
    mp->head = mem;
    /* link to the list from new node */
    *mp->head = temp;

    mp->blocks ++;

#ifdef debugging
    zlog_info(category_debug, 
              "[Mempool] Current MemPool [%d]\n[Mempool] Remain blocks [%d]", 
              mp, mp->blocks);
#endif
    // count the slots usage
    mp->used_slots = mp->used_slots - 1;
    
    pthread_mutex_unlock(&mp->mem_lock);

    return MEMORY_POOL_SUCCESS;
}

float mp_slots_usage_percentage(Memory_Pool *mp){
    float usage_percentage = 0;
    
    usage_percentage = (mp->used_slots*1.0) / (mp->alloc_time * mp->slots);

    return usage_percentage;
}

