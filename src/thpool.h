/**********************************
 * @author      Johan Hanssen Seferidis
 * License:     MIT
 *
 **********************************/

#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include "Mempool.h"


#define err(str) fprintf(stderr, str)


static volatile int threads_keepalive;
static volatile int threads_on_hold;



/* ========================== STRUCTURES ============================ */


/* Binary semaphore */
typedef struct bsem {
	pthread_mutex_t mutex;
	pthread_cond_t   cond;
	int v;
} bsem;


/* Job */
typedef struct job{
	struct job*  prev;                   /* pointer to previous job   */
	void   (*function)(void* arg);       /* function pointer          */
	void*  arg;                          /* function's argument       */
} job;


/* Job queue */
typedef struct jobqueue{
	pthread_mutex_t rwmutex;             /* used for queue r/w access */
	job  *front;                         /* pointer to front of queue */
	job  *rear;                          /* pointer to rear  of queue */
	bsem *has_jobs;                      /* flag as binary semaphore  */
	int   len;                           /* number of jobs in queue   */
} jobqueue;


/* Thread */
typedef struct thread{
	int       id;                        /* friendly id               */
	pthread_t pthread;                   /* pointer to actual thread  */
	struct thpool_* thpool_p;            /* access to thpool          */
} thread;


/* Threadpool */
typedef struct thpool_{
	thread**   threads;                  /* pointer to threads        */
	volatile int num_threads_alive;      /* threads currently alive   */
	volatile int num_threads_working;    /* threads currently working */
	pthread_mutex_t  thcount_lock;       /* used for thread count etc */
	pthread_cond_t  threads_all_idle;    /* signal to thpool_wait     */
	jobqueue  jobqueue;                  /* job queue                 */
} thpool_;







/* ========================== PROTOTYPES ============================ */


static int  thread_init(thpool_* thpool_p, struct thread** thread_p, int id);
static void* thread_do(struct thread* thread_p);
static void  thread_hold(int sig_id);
static void  thread_destroy(struct thread* thread_p);

static int   jobqueue_init(jobqueue* jobqueue_p);
static void  jobqueue_clear(jobqueue* jobqueue_p);
static void  jobqueue_push(jobqueue* jobqueue_p, struct job* newjob_p);
static struct job* jobqueue_pull(jobqueue* jobqueue_p);
static void  jobqueue_destroy(jobqueue* jobqueue_p);

static void  bsem_init(struct bsem *bsem_p, int value);
static void  bsem_reset(struct bsem *bsem_p);
static void  bsem_post(struct bsem *bsem_p);
static void  bsem_post_all(struct bsem *bsem_p);
static void  bsem_wait(struct bsem *bsem_p);

/* =================================== API ======================================= */


typedef struct thpool_* Threadpool;


/**
 * @brief  Initialize threadpool
 *
 * Initializes a threadpool. This function will not return untill all
 * threads have initialized successfully.
 *
 * @example
 *
 *    ..
 *    threadpool thpool;                     //First we declare a threadpool
 *    thpool = thpool_init(4);               //then we initialize it to 4 threads
 *    ..
 *
 * @param  num_threads   number of threads to be created in the threadpool
 * @return threadpool    created threadpool on success,
 *                       NULL on error
 */
Threadpool thpool_init(int num_threads);


/**
 * @brief Add work to the job queue
 *
 * Takes an action and its argument and adds it to the threadpool's job queue.
 * If you want to add to work a function with more than one arguments then
 * a way to implement this is by passing a pointer to a structure.
 *
 * NOTICE: You have to cast both the function and argument to not get warnings.
 *
 * @example
 *
 *    void print_num(int num){
 *       printf("%d\n", num);
 *    }
 *
 *    int main() {
 *       ..
 *       int a = 10;
 *       thpool_add_work(thpool, (void*)print_num, (void*)a);
 *       ..
 *    }
 *
 * @param  threadpool    threadpool to which the work will be added
 * @param  function_p    pointer to function to add as work
 * @param  arg_p         pointer to an argument
 * @return 0 on successs, -1 otherwise.
 */
int thpool_add_work(Threadpool threadpool, void (*function_p)(void*), void* arg_p);


/**
 * @brief Destroy the threadpool
 *
 * This will wait for the currently active threads to finish and then 'kill'
 * the whole threadpool to free up memory.
 *
 * @example
 * int main() {
 *    threadpool thpool1 = thpool_init(2);
 *    threadpool thpool2 = thpool_init(2);
 *    ..
 *    thpool_destroy(thpool1);
 *    ..
 *    return 0;
 * }
 *
 * @param threadpool     the threadpool to destroy
 * @return nothing
 */
void thpool_destroy(Threadpool);

/* The number of slots for the memory pool */
#define SLOTS_FOR_MEM_POOL 100

/* The number of slots for the memory pool */
#define SIZE_FOR_MEM_POOL 256