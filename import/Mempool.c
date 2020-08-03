/*
  Copyright (c) BiDaE Technology Inc. All rights reserved.

  License:

    BiDaE SHAREWARE LICENSE
    Version 1.0, 31 July 2020

    Copyright (c) BiDaE Technology Inc. All rights reserved.
    The SOFTWARE implemented in the product is copyrighted and protected by 
    all applicable intellectual property laws worldwide. Any duplication of 
    the SOFTWARE other than for archiving or resetting purposes on the same 
    product without the written agreement from BiDaE Technology could be a 
    violation of law. For the avoidance of doubt, redistribution of this 
    SOFTWARE in source or binary form is prohibited. You agree to prevent 
    any unauthorized copying and redistribution of the SOFTWARE. 

    BiDaE Technology Inc. is the license steward. No one other than the 
    license steward has the right to modify or publish new versions of this 
    License. However, You may distribute the SOFTWARE under the terms of the 
    version of the License under which You originally received the Covered 
    Software, or under the terms of any subsequent version published by the 
    license steward.

    LIMITED WARRANTY:

    BiDaE Technology Inc. or its distributors, depending on which party sold 
    the SOFTWARE, warrants that the media on which the SOFTWARE is installed 
    will be free from defects in materials under normal and purposed use.

    BiDaE Technology Inc. or its distributor warrants, for your benefit alone, 
    that during the Warranty Period the SOFTWARE, shall operate substantially 
    in accordance with the functional specifications in the User's Manual. If, 
    during the Warranty Period, a defect in the SOFTWARE appears, You may 
    obtain a replacement of the SOFTWARE. Any replacement SOFTWARE will be 
    warranted for the remainder of the Warranty Period attached to the product.

    WHEN THE WARRANTY PERIOD HAS BEEN EXPIRED, THIS SOFTWARE IS PROVIDED 
    ''AS IS,'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. HENCEFORTH, IN NO EVENT SHALL BiDaE 
    TECHNOLOGY INC. OR ITS COLLABORATOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

