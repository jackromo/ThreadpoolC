/*
 * File: threadpool.h
 *
 * Main threadpool header.
 *
 * Author: Jack Romo <sharrackor@gmail.com>
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdlib.h>
#include <glib.h>
#include <stdbool.h>


/*
 * job: A single job to be executed by one thread in a threadpool.
 * Contains its constituent function and a lock.
 */
typedef struct {
    void *params;               // params to feed to job
    void (* work)(void *params);    // work function to be executed
} job;

// Job functions

job job_new(void *params, void (* work_func)(void *));
void job_free(job *);
void job_exec(job *);

/*
 * jobqueue: A linked list of jobs to execute.
 * Access to jobs before they are removed from the queue is rare anyway,
 * so the linear search time of a linked list is irrelevant.
 */
typedef struct {
    int num_jobs;   // number of jobs
    GSList *head;  // linked list of jobs
    GSList *tail;  // final entry in queue, to append new jobs to
} jobqueue;

// Jobqueue functions

jobqueue jobqueue_new();
void jobqueue_push(jobqueue *, job *new_job);
job jobqueue_pop(jobqueue *);
int jobqueue_num_jobs(jobqueue);
void jobqueue_free(jobqueue *);

/*
 * worker: A single thread working in a threadpool.
 * Contains the thread it is using as well as a private jobqueue,
 * which it takes from in its own time.
 */
typedef struct {
    jobqueue queue;     // private queue of jobs
    pthread_t thread;   // id of worker thread
    pthread_mutex_t queue_mutex;    // mutex on job queue
    bool working;       // whether worker is active or not
} worker;

// Worker functions

worker worker_new();
void worker_start(worker *);
bool worker_get_job_count(worker *);
void worker_push_job(worker *, job *);
void worker_close(worker *);
void worker_free(worker *);

/*
 * threadpool: A collection of workers and a public jobqueue,
 * which it sends jobs from to each of its workers' jobqueues.
 */
typedef struct {
    int num_workers;
    worker *workers;    // array of workers
    jobqueue queue;     // public jobqueue
    pthread_t dispatcher;   // thread to dispatch jobs
} threadpool;

// Threadpool functions

threadpool threadpool_new(int num_workers);
void threadpool_add_job(threadpool *pool, job job);
void threadpool_start(threadpool *pool);
int threadpool_jobs_left(threadpool *pool);
void threadpool_finish(threadpool *);
void threadpool_free(threadpool *);

#endif // THREADPOOL_H

