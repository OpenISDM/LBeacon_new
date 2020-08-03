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