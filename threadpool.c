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
        jobqueue_pop(queue);
    }
    free(queue);
}

// Worker functions

// actual thread function
void *worker_pthread_work(void *own_worker) {
    worker *work = (worker *) own_worker;
    while(work->working) {
        if(jobqueue_num_jobs(work->queue) > 0) {
            pthread_mutex_lock(&(work->queue_mutex));
            job next_job = jobqueue_pop(&(work->queue));
            pthread_mutex_unlock(&(work->queue_mutex));
            job_exec(&next_job);
            job_free(&next_job);
        }
    }
    pthread_exit(NULL);
}

worker worker_new() {
    worker work;
    work.queue = jobqueue_new();
    pthread_mutex_init(&(work.queue_mutex), NULL);
    work.working = true;
    return work;
}

void worker_start(worker *work) {
    int retval = pthread_create(&(work->thread), NULL, worker_pthread_work, (void *) work);
    if(retval) {
        printf("Error: Could not initialize thread in worker_new");
        exit(EXIT_FAILURE);
    }
}

bool worker_get_job_count(worker *work) {
    pthread_mutex_lock(&(work->queue_mutex));
    return jobqueue_num_jobs(work->queue);
    pthread_mutex_unlock(&(work->queue_mutex));
}

void worker_push_job(worker *work, job *new_job) {
    pthread_mutex_lock(&(work->queue_mutex));
    jobqueue_push(&(work->queue), new_job);
    pthread_mutex_unlock(&(work->queue_mutex));
}

void worker_close(worker *work) {
    work->working = false;      // no lock needed, only modification point
    pthread_join(work->thread, NULL);
}

void worker_free(worker *work) {
    worker_close(work);
    jobqueue_free(&(work->queue));
    free(work);
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


