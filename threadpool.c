/*
 * File: threadpool.c
 *
 * Contains all definitions of threadpool.h functions.
 *
 * Author: Jack Romo <sharrackor@gmail.com>
 */

#include "threadpool.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>


// Job functions

job job_new(void *params, void (* work_func)(void *)) {
    job job;
    job.params = params;
    job.work = work_func;
    return job;
}

void job_free(job *job) {
    free(job->params);
    free(job);
}

void job_exec(job *job) {
    job->work(job->params);
}

// Jobqueue functions

jobqueue jobqueue_new() {
    jobqueue queue;
    queue.num_jobs = 0;
    queue.head = queue.tail = NULL;
    return queue;
}

void jobqueue_push(jobqueue *queue, job *new_job) {
    queue->tail = g_slist_append(queue->tail, (void *) new_job);
    (queue->num_jobs)++;
    if(queue->num_jobs == 1)
        queue->head = queue->tail;
    else
        queue->tail = queue->tail->next;
}

job jobqueue_pop(jobqueue *queue) {
    if(queue->num_jobs > 0) {
        job *next_job = (job *) (queue->head->data);
        queue->head = g_slist_delete_link(queue->head, queue->head);
        queue->num_jobs--;
        if(queue->num_jobs == 0)
            queue->head = queue->tail = NULL;
        return *next_job;
    } else {
        printf("Error: Attempted to pop value from empty queue");
        exit(EXIT_FAILURE);
    }
}

int jobqueue_num_jobs(jobqueue queue) {
    return queue.num_jobs;
}

void jobqueue_free(jobqueue *queue) {
    while(queue->num_jobs > 0) {
        job next_job = jobqueue_pop(queue);
    }
    free(queue);
}

// Worker functions

void worker_pthread_work(worker *work) {
    // actual thread function
}

worker worker_new() {
    //
}

void worker_start(worker *work) {
    //
}

bool worker_get_job_count(worker work) {
    //
}

void worker_push_job(worker *work) {
    //
}

void worker_close(worker *work) {
    //
}

void worker_free(worker *work) {
    //
}

// Threadpool functions

threadpool threadpool_new(int num_workers) {
    //
}

void threadpool_add_job(threadpool *pool, job job) {
    //
}

void threadpool_start(threadpool *pool) {
    //
}

int threadpool_jobs_left(threadpool *pool) {
    //
}

void threadpool_finish(threadpool *pool) {
    //
}

void threadpool_free(threadpool *pool) {
    //
}


