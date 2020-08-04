/*
  2020 © Copyright (c) BiDaE Technology Inc. 
  Provided under BiDaE SHAREWARE LICENSE-1.0 in the LICENSE.

  Project Name:

     BeDIS

  File Name:

     thpool.h

  File Description:

     This file the definitions and declarations of constants, structures, and
     functions used in the thpool.c file.

     Note: This code is forked from https://github.com/Pithikos/C-Thread-Pool
           Author: Johan Hanssen Seferidis
           License: MIT

  Version:

     2.0, 20190617

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

     Holly Wang    , hollywang@iis.sinica.edu.tw
     Gary Xiao     , garyh0205@hotmail.com

 */
//TODO thread priority

#ifndef THPOOL_H
#define THPOOL_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include "Mempool.h"


/* The number of slots for the memory pool */
#define SLOTS_FOR_MEM_POOL_PER_THREAD 20
/* The size of the slot for the memory pool */
#define SIZE_OF_SLOT 512

#define WAITING_TIME 50

#define err(str) fprintf(stderr, str)

/* ========================== STRUCTURES ============================ */


/* Binary semaphore */
typedef struct bsem {
    pthread_mutex_t mutex;
    pthread_cond_t   cond;
    int v;
} bsem;


/* Job */
typedef struct job{
    /* A pointer pointer to the previous job */
    struct job *prev;

    /* A pointer point to the function to be called */
    void (*function)(void *arg);

    /* A argument for the function pointer */
    void *arg;

    /* The priority nice level of this job */
    int priority;
} job;


/* Job queue */
typedef struct jobqueue{

    /* A mutex use for controling the job queue read/write access */
    pthread_mutex_t rwmutex;

    /* A pointer point to the head of the job queue */
    job *front;

    /* A pointer point to the tail of the job queue  */
    job *rear;

    /* A binary semaphore flag to identify if there has jobs */
    bsem *has_jobs;

    /* The number of jobs in the job queue */
    int len;

} jobqueue;


/* Thread */
typedef struct thread{

    /* The number defined by the thread pool */
    int id;

    pthread_t pthread;

    /* A pointer points to the curret thread pool */
    struct thpool_ *thpool_p;

} thread;


/* Threadpool */
typedef struct thpool_{
    /* A pointer point to threads */
    thread **threads;

    /* The number of threads currently alive */
    volatile int num_threads_alive;

    /* The nnumber of threads currently working */
    volatile int num_threads_working;

    /* A mutex use for counting threads */
    pthread_mutex_t  thcount_lock;

    jobqueue  jobqueue;

    volatile int threads_keepalive;

    /* Memory pools for the allocation of all variable in the thpool
       including thread, bsem and job */
    Memory_Pool mempool;

    int mempool_size;

} thpool_;

typedef thpool_ *Threadpool;


/* ========================== PROTOTYPES ============================ */


static int   thread_init(thpool_ *thpool_p, thread **thread_p, int id);
static void *thread_do(thread *thread_p);
static void  thread_destroy(thread *thread_p);

static int   jobqueue_init(thpool_ *thpool_p, jobqueue *jobqueue_p);
static void  jobqueue_clear(thpool_ *thpool_p, jobqueue *jobqueue_p);
static void  jobqueue_push(jobqueue *jobqueue_p, job *newjob_p);
static job *jobqueue_pull(jobqueue *jobqueue_p);
static void  jobqueue_destroy(thpool_ *thpool_p, jobqueue *jobqueue_p);

static void  bsem_init(bsem *bsem_p, int value);
static void  bsem_reset(bsem *bsem_p);
static void  bsem_post(bsem *bsem_p);
static void  bsem_post_all(bsem *bsem_p);
static void  bsem_wait(bsem *bsem_p);

/* ================================= API ==================================== */

/*
  thpool_init

     Initializes a threadpool. This function will not return untill all
     threads have initialized successfully.

  Parameters:

     num_threads - The number of threads to be created in the threadpool.

  Return Value:

     Created threadpool on success, NULL on error.

 */
Threadpool thpool_init(int num_threads);


/*
  thpool_add_work

     Takes an action and its argument and adds it to the threadpool's job queue.
     If you want to add to work a function with more than one arguments then
     a way to implement this is by passing a pointer to a structure.

     NOTICE: You have to cast both the function and argument
             to not get warnings.

  Parameters:

     threadpool - threadpool to which the work will be added
     function_p - The pointer point to the function to be added as work.
     arg_p      - The pointer point to the argument use for function_p.
     priority   - This priority nice of this work.

      @example

          void print_num(int num){
             printf("%d\n", num);
          }

          int main() {
             ..
             int a = 10;
             thpool_add_work(thpool, (void*)print_num, (void*)a);
             ..
          }

  Return_Value:

     0 on successs, -1 otherwise.

 */
int thpool_add_work(Threadpool threadpool, void (*function_p)(void *),
                    void *arg_p, int priority);


/*
  thpool_destroy

     This will wait for the currently active threads to finish and then 'kill'
     the whole threadpool to free up memory.

  Parameters:

     thpool - The threadpool to be destroyed.

       @example
       int main() {
          threadpool thpool1 = thpool_init(2);
          threadpool thpool2 = thpool_init(2);
          ..
          thpool_destroy(thpool1);
          ..
          return 0;
       }

  Return_value:

     None

 */
void thpool_destroy(thpool_ *thpool_p);


/*
  thpool_num_threads_working

     Working threads are the threads that are performing work (not idle).

  Parameters:

     thpool - The threadpool that we want to know the number of working threads.

       @example
       int main() {
          threadpool thpool1 = thpool_init(2);
          threadpool thpool2 = thpool_init(2);
          ..
          printf("Working threads: %d\n", thpool_num_threads_working(thpool1));
          ..
          return 0;
       }

  Return_Value:

     The number of threads is working currently.

 */
int thpool_num_threads_working(thpool_ *thpool_p);


#endif
