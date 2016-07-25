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
#include <glib.h>


// Job functions

job job_new(void *params, void (* work_func)(void *)) {
    //
}

void job_free(job job) {
    //
}

void job_exec(job job) {
    //
}

// Jobqueue functions

jobqueue jobqueue_new() {
    //
}

void jobqueue_push(jobqueue *queue, job new_job){
    //
}

job jobqueue_pop(jobqueue *queue) {
    //
}

int jobqueue_num_jobs(jobqueue queue) {
    //
}

void jobqueue_free(jobqueue *queue) {
    //
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


