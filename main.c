/*
 * File: main.c
 *
 * Main entry point for program,
 * as well as an example of what the threadpool can do.
 *
 * Author: Jack Romo <sharrackor@gmail.com>
 *
 */

#include "threadpool.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void print_hello(void *args) {
    printf("Hello out there!\n");
}


int main(int argc, char **argv) {
    // TODO: make _new methods give allocated pointer
    threadpool *pool = malloc(sizeof(threadpool));
    *pool = threadpool_new(10);
    threadpool_start(pool);
    job *first_job = malloc(sizeof(job));
    *first_job = job_new(NULL, print_hello);
    threadpool_add_job(pool, first_job);
    job *second_job = malloc(sizeof(job));
    *second_job = job_new(NULL, print_hello);
    threadpool_add_job(pool, second_job);
    usleep(1000*100);
    printf("done waiting\n");
    threadpool_finish(pool);
    threadpool_free(pool);
}

