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

     thpool.c

  File Description:

     This file contains the program to providing a threading pool where we
     can use for add work.

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

#include "thpool.h"

/* ========================== THREADPOOL ============================ */


/* Initialise thread pool */
struct thpool_ *thpool_init(int num_threads){

    int return_value, n;

    thpool_ *thpool_p;

    if (num_threads < 0){
        num_threads = 0;
    }

    /* Make new thread pool */
    thpool_p = (thpool_ *)malloc(sizeof(thpool_));
    if (thpool_p == NULL){
        err("thpool_init(): Could not allocate memory for thread pool\n");
        return NULL;
    }

    thpool_p->num_threads_alive   = 0;
    thpool_p->num_threads_working = 0;

    thpool_p->threads_keepalive = 1;

    thpool_p->mempool_size = SIZE_OF_SLOT;

    /* Initialize the memory pool */
    if(mp_init(&thpool_p->mempool, SIZE_OF_SLOT,
       num_threads * SLOTS_FOR_MEM_POOL_PER_THREAD) != MEMORY_POOL_SUCCESS)
        return NULL;


    /* Initialise the job queue */
    if (jobqueue_init(thpool_p, &thpool_p -> jobqueue) == -1){
        err("thpool_init(): Could not allocate memory for job queue\n");

        mp_destroy(&thpool_p->mempool);

        free(thpool_p);
        return NULL;
    }

    /* Make threads in pool */
    thpool_p ->threads = (thread **)malloc((sizeof(struct thread *) * 
                         num_threads));

    if (thpool_p -> threads == NULL){
        err("thpool_init(): Could not allocate memory for threads\n");
        jobqueue_destroy(thpool_p, &thpool_p -> jobqueue);

        mp_destroy(&thpool_p->mempool);

        free(thpool_p);

        return NULL;
    }

    pthread_mutex_init(&(thpool_p -> thcount_lock), 0);
    pthread_cond_init(&(thpool_p->thcount_lock), 0);

    /* Thread init */
    for (n = 0; n < num_threads; n ++){
        thread_init(thpool_p, &thpool_p -> threads[n], n);

    }

    /* Wait for threads to initialize */
    while (thpool_p -> num_threads_alive != num_threads) {
        sleep_t(WAITING_TIME);
    }

    return thpool_p;
}


/* Add work to the thread pool */
int thpool_add_work(thpool_ *thpool_p, void (*function_p)(void *),
                    void *arg_p, int priority){
    job *newjob = NULL;

    newjob = (job *)mp_alloc(&thpool_p->mempool);

    if (newjob == NULL){
        err("thpool_add_work(): Could not allocate memory for new job\n");
        return -1;
    }

    /* add function and argument */
    newjob -> function = function_p;
    newjob -> arg = arg_p;
    newjob -> priority = priority;

    /* add job to queue */
    jobqueue_push(&thpool_p -> jobqueue, newjob);

    return 0;
}


/* Destroy the threadpool */
void thpool_destroy(thpool_ *thpool_p){

    volatile int threads_total;

    int n;

    double TIMEOUT, tpassed;

    time_t start, end;

    /* No need to destory if it's NULL */
    if (thpool_p == NULL) return ;

    threads_total = thpool_p ->  num_threads_alive;

    /* End each thread 's infinite loop */
    thpool_p->threads_keepalive = 0;

    /* Give one second to kill idle threads */
    TIMEOUT = 1.0;
    tpassed = 0.0;
    time (&start);
    while (tpassed < TIMEOUT && thpool_p -> num_threads_alive){
        bsem_post_all(thpool_p -> jobqueue.has_jobs);
        time (&end);
        tpassed = difftime(end, start);
    }

    /* Poll remaining threads */
    while (thpool_p -> num_threads_alive){
        bsem_post_all(thpool_p -> jobqueue.has_jobs);
        sleep_t(WAITING_TIME);
    }

    /* Job queue cleanup */
    jobqueue_destroy(thpool_p, &thpool_p -> jobqueue);
    /* Deallocs */
    for (n = 0; n < threads_total; n ++){
        thread_destroy(thpool_p -> threads[n]);
    }

    free(thpool_p -> threads);

    mp_destroy(&thpool_p->mempool);

    free(thpool_p -> threads);
    free(thpool_p);

    thpool_p = NULL;
}


int thpool_num_threads_working(thpool_ *thpool_p){
    return thpool_p -> num_threads_working;
}


/* ============================ THREAD ============================== */


static int thread_init (thpool_ *thpool_p, thread **thread_p, int id){

    *thread_p = NULL;
    
    *thread_p = (thread *)mp_alloc(&thpool_p -> mempool);

    if (thread_p == NULL){
        err("thread_init(): Could not allocate memory for thread\n");
        return -1;
    }

    (*thread_p)->thpool_p = thpool_p;
    (*thread_p)->id       = id;

    pthread_create(&(*thread_p) -> pthread, NULL, (void *)thread_do,
                   (*thread_p));
    pthread_detach((*thread_p) -> pthread);

    return 0;
}


static void *thread_do(thread *thread_p){

    thpool_ *thpool_p;

    /* Assure all threads have been created before starting serving */
    thpool_p = thread_p -> thpool_p;

    /* Mark thread as alive (initialized) */
    pthread_mutex_lock(&thpool_p -> thcount_lock);
    thpool_p->num_threads_alive += 1;
    pthread_mutex_unlock(&thpool_p -> thcount_lock);

    while(thpool_p->threads_keepalive){

        void ( *func_buff)(void *);
        void *arg_buff;
        job *job_p;

        bsem_wait(thpool_p -> jobqueue.has_jobs);

        if (thpool_p->threads_keepalive){

            pthread_mutex_lock(&thpool_p -> thcount_lock);
            thpool_p->num_threads_working ++;
            pthread_mutex_unlock(&thpool_p -> thcount_lock);

            /* Read job from queue and execute it */

            job_p = jobqueue_pull(&thpool_p -> jobqueue);
            if (job_p) {
                func_buff = job_p -> function;
                arg_buff  = job_p -> arg;

                func_buff(arg_buff);
                mp_free(&thread_p->thpool_p->mempool ,job_p);
            }

            pthread_mutex_lock(&thpool_p -> thcount_lock);
            thpool_p -> num_threads_working --;
            pthread_mutex_unlock(&thpool_p -> thcount_lock);

        }
    }
    pthread_mutex_lock(&thpool_p -> thcount_lock);
    thpool_p -> num_threads_alive --;
    pthread_mutex_unlock(&thpool_p -> thcount_lock);

    return NULL;
}


/* Frees a thread  */
static void thread_destroy (thread *thread_p){
    
    mp_free(&thread_p->thpool_p->mempool, thread_p);

    thread_p = NULL;
}


/* ============================ JOB QUEUE =========================== */


/* Initialize queue */
static int jobqueue_init(thpool_ *thpool_p, jobqueue *jobqueue_p){
    jobqueue_p -> len = 0;
    jobqueue_p -> front = NULL;
    jobqueue_p -> rear  = NULL;

    jobqueue_p -> has_jobs = (bsem *)mp_alloc(&thpool_p-> mempool);

    if (jobqueue_p -> has_jobs == NULL){
        return -1;
    }

    pthread_mutex_init(&(jobqueue_p -> rwmutex), 0);
    bsem_init(jobqueue_p -> has_jobs, 0);

    return 0;
}


/* Clear the queue */
static void jobqueue_clear(thpool_ *thpool_p, jobqueue *jobqueue_p){

    while(jobqueue_p -> len){
        mp_free(&thpool_p->mempool, jobqueue_pull(jobqueue_p));
    }

    jobqueue_p -> front = NULL;
    jobqueue_p -> rear  = NULL;
    bsem_reset(jobqueue_p -> has_jobs);
    jobqueue_p -> len = 0;

}


/* Add (allocated) job to queue */
static void jobqueue_push(jobqueue *jobqueue_p, job *newjob){

    pthread_mutex_lock(&jobqueue_p -> rwmutex);
    newjob -> prev = NULL;

    switch(jobqueue_p -> len){

        case 0:  /* if no jobs in queue */

            jobqueue_p -> front = newjob;
            jobqueue_p -> rear  = newjob;

            break;

        default: /* if jobs in queue */

            jobqueue_p -> rear -> prev = newjob;
            jobqueue_p -> rear = newjob;

    }

    jobqueue_p -> len ++;

    bsem_post(jobqueue_p -> has_jobs);
    pthread_mutex_unlock(&jobqueue_p -> rwmutex);
}


static job *jobqueue_pull(jobqueue *jobqueue_p){

    job *job_p;

    pthread_mutex_lock(&jobqueue_p -> rwmutex);
    job_p = jobqueue_p -> front;

    switch(jobqueue_p -> len){

        case 0:  /* if no jobs in queue */

            break;

        case 1:  /* if one job in queue */

            jobqueue_p -> front = NULL;
            jobqueue_p -> rear  = NULL;
            jobqueue_p -> len = 0;

            break;

        default: /* if >1 jobs in queue */

            jobqueue_p -> front = job_p -> prev;
            jobqueue_p -> len --;
            /* more than one job in queue -> post it */
            bsem_post(jobqueue_p -> has_jobs);

    }

    pthread_mutex_unlock(&jobqueue_p -> rwmutex);
    return job_p;
}


/* Free all queue resources back to the system */
static void jobqueue_destroy(thpool_ *thpool_p, jobqueue *jobqueue_p){
    jobqueue_clear(thpool_p, jobqueue_p);
    mp_free(&thpool_p->mempool, jobqueue_p -> has_jobs);
}


/* ======================== SYNCHRONISATION ========================= */


/* Init semaphore to 1 or 0 */
static void bsem_init(bsem *bsem_p, int value) {
    if (value < 0 || value > 1) {
        err("bsem_init(): Binary semaphore can take only values 1 or 0");
        exit(1);
    }
    pthread_mutex_init(&(bsem_p -> mutex), 0);
    pthread_cond_init(&(bsem_p -> cond), 0);
    bsem_p -> v = value;
}


/* Reset semaphore to 0 */
static void bsem_reset(bsem *bsem_p) {
    bsem_init(bsem_p, 0);
}


/* Post to at least one thread */
static void bsem_post(bsem *bsem_p) {
    pthread_mutex_lock(&bsem_p -> mutex);
    bsem_p -> v = 1;
    pthread_cond_signal(&bsem_p -> cond);
    pthread_mutex_unlock(&bsem_p -> mutex);
}


/* Post to all threads */
static void bsem_post_all(bsem *bsem_p) {
    pthread_mutex_lock(&bsem_p -> mutex);
    bsem_p -> v = 1;
    pthread_cond_broadcast(&bsem_p -> cond);
    pthread_mutex_unlock(&bsem_p -> mutex);
}


/* Wait on semaphore until semaphore has value 0 */
static void bsem_wait(bsem *bsem_p) {
    pthread_mutex_lock(&bsem_p ->  mutex);
    while (bsem_p -> v != 1) {
        pthread_cond_wait(&bsem_p -> cond, &bsem_p -> mutex);
    }
    bsem_p -> v = 0;
    pthread_mutex_unlock(&bsem_p -> mutex);
}