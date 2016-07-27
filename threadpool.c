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
#include <unistd.h>


// Job functions

job *job_new(void *params, void (* work_func)(void *)) {
    job *new_job = malloc(sizeof(job));
    if(new_job == NULL) {
        printf("Error: could not allocate new job");
        exit(EXIT_FAILURE);
    }
    new_job->params = params;
    new_job->work = work_func;
    return new_job;
}

void job_free(job *job) {
    free(job->params);
    free(job);
}

void job_exec(job *job) {
    (*(job->work))(job->params);
}

// Jobqueue functions

jobqueue *jobqueue_new() {
    jobqueue *queue = malloc(sizeof(jobqueue));
    if(queue == NULL) {
        printf("Error: could not allocate new jobqueue");
        exit(EXIT_FAILURE);
    }
    queue->num_jobs = 0;
    queue->head = queue->tail = NULL;
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

job *jobqueue_pop(jobqueue *queue) {
    if(queue->num_jobs > 0) {
        job *next_job = (job *) (queue->head->data);
        queue->head = g_slist_delete_link(queue->head, queue->head);
        queue->num_jobs--;
        if(queue->num_jobs == 0)
            queue->head = queue->tail = NULL;
        return next_job;
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
            job *next_job = jobqueue_pop(&(work->queue));
            pthread_mutex_unlock(&(work->queue_mutex));
            job_exec(next_job);
            job_free(next_job);
        }
        usleep(1000);
    }
    pthread_exit(NULL);
}

worker *worker_new() {
    worker *work = malloc(sizeof(worker));
    if(work == NULL) {
        printf("Error: could not allocate new worker");
        exit(EXIT_FAILURE);
    }
    work->queue = *jobqueue_new();
    pthread_mutex_init(&(work->queue_mutex), NULL);
    work->working = true;
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
    int count = jobqueue_num_jobs(work->queue);
    pthread_mutex_unlock(&(work->queue_mutex));
    return count;
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
    while(work->queue.num_jobs > 0) {
        jobqueue_pop(&(work->queue));
    }
    free(work);
}

// Threadpool functions

// actual threadpool dispatcher thread function
void *dispatch_jobs(void *tpool) {
    threadpool *pool = (threadpool *) tpool;
    // loop through all workers in circular fashion, giving jobs to each
    for(int i = 0; pool->working; i = (i+1) % pool->num_workers) {
        if(jobqueue_num_jobs(pool->queue) > 0) {
            pthread_mutex_lock(&(pool->queue_mutex));
            job *new_job = jobqueue_pop(&(pool->queue));
            pthread_mutex_unlock(&(pool->queue_mutex));
            pthread_mutex_lock(&(pool->workers[i].queue_mutex));
            jobqueue_push(&(pool->workers[i].queue), new_job);
            pthread_mutex_unlock(&(pool->workers[i].queue_mutex));
        }
        usleep(1000);
    }
    pthread_exit(NULL);
}

threadpool *threadpool_new(int num_workers) {
    threadpool *pool = malloc(sizeof(threadpool));
    if(pool == NULL) {
        printf("Error: could not allocate new threadpool");
        exit(EXIT_FAILURE);
    }
    pool->num_workers = num_workers;
    pool->workers = malloc(sizeof(worker) * pool->num_workers);
    if(pool->workers == NULL) {
        printf("Error: could not allocate memory for threadpool workers in threadpool_new");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < pool->num_workers; i++) {
        pool->workers[i] = *worker_new();
    }
    pthread_mutex_init(&(pool->queue_mutex), NULL);
    pool->queue = *jobqueue_new();
    pool->working = false;
    return pool;
}

void threadpool_add_job(threadpool *pool, job *job) {
    pthread_mutex_lock(&(pool->queue_mutex));
    jobqueue_push(&(pool->queue), job);
    pthread_mutex_unlock(&(pool->queue_mutex));
}

void threadpool_start(threadpool *pool) {
    pool->working = true;
    for(int i = 0; i < pool->num_workers; i++) {
        worker_start(&(pool->workers[i]));
    }
    int retval = pthread_create(&(pool->dispatcher), NULL, dispatch_jobs, (void *) pool);
    if(retval) {
        printf("Error: Could not initialize thread in worker_new");
        exit(EXIT_FAILURE);
    }
}

int threadpool_jobs_left(threadpool *pool) {
    pthread_mutex_lock(&(pool->queue_mutex));
    int jobs_left = jobqueue_num_jobs(pool->queue);
    pthread_mutex_unlock(&(pool->queue_mutex));
    return jobs_left;
}

void threadpool_finish(threadpool *pool) {
    pool->working = false;
    pthread_join(pool->dispatcher, NULL);
}

void threadpool_free(threadpool *pool) {
    for(int i = 0; i < pool->num_workers; i++) {
        // cannot use worker_free here bc need to free entire array together
        worker_close(&(pool->workers[i]));
        while(pool->workers[i].queue.num_jobs > 0) {
            jobqueue_pop(&(pool->workers[i].queue));
        }
    }
    free(pool->workers);
    free(pool);
}

