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


/*
 * job: A single job to be executed by one thread in a threadpool.
 * Contains its constituent function and a lock.
 */
typedef struct {
    //
} job;


/*
 * jobqueue: A linked list of jobs to execute.
 * Access to jobs before they are removed from the queue is rare anyway,
 * so the linear search time of a linked list is irrelevant.
 */
typedef struct {
    //
} jobqueue ;


/*
 * worker: A single thread working in a threadpool.
 * Contains the thread it is using as well as a private jobqueue,
 * which it takes from in its own time.
 */
typedef struct {
    //
} worker;


/*
 * threadpool: A collection of workers and a public jobqueue,
 * which it sends jobs from to each of its workers' jobqueues.
 */
typedef struct {
    //
} threadpool;


#endif // THREADPOOL_H

