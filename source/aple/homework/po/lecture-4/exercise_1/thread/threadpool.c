#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

struct ThreadPool *pool;
pthread_t parent_t;
int tid = 0;

void pool_init(int num_threads) {
    pool = (struct ThreadPool*) malloc(sizeof(struct ThreadPool));
    pool->head = NULL;
    pool->tail = NULL;
    pool->total_threads = num_threads;
    // float *count = (float *)malloc(sizeof(float));
    pool->count = 0;
    pthread_mutex_init(&pool->mutex, NULL);
    sem_init(&pool->semaphore, 0, 0);
    pool->threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
    for (int i=0; i < num_threads; i++){
        pthread_create(&(pool->threads[i]), NULL, worker, (void *)i);
    }
}

// insert a task to the end of the dequeue
int enqueue(struct Task * t){
    if (pool->head == NULL) {
        pool->head = t;
        pool->tail = t;
    } else {
        pool->tail->next = t;
        pool->tail = t;
    }
    return 0;
}

// pop a task from the head of the dequeue
struct Task * dequeue() {
    struct Task *curr_task;
    curr_task = pool->head;
    if (curr_task == NULL) {
        return NULL;
    }
    pool->head = curr_task->next;
    return curr_task;
}


void pool_submit(FN_PT fn_pt, void *param) {
    pthread_mutex_lock(&pool->mutex);
    struct Task *t = (struct Task*) malloc(sizeof(struct Task));
    t->tid = tid;
    t->fn_pt = fn_pt;
    t->next = NULL;
    t->param = param;

    tid++;

    enqueue(t);
    pthread_mutex_unlock(&pool->mutex);
    sem_post(&pool->semaphore);
}

void pool_destroy(char *path) {
    for (int i=0; i < pool->total_threads ; i++) {
        sem_post(&pool->semaphore);
    }
    for (int i=0; i < pool->total_threads ; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    printf("All work is done!\nTotal file size in dir:%s is:%f\n", path, pool->count);
}

float execute(FN_PT fn_pt, void *param) {
    return (* fn_pt)(param);
}


// data structures
void *worker(void *param) {
    int thread_id = (int) param;
    int num_tasks = 0;
    float count_in_thread = 0;
    printf("starting thread %u...\n", thread_id);
    while (1)
    {
        printf("thread %u waiting===================>\n", thread_id);
        sem_wait(&pool->semaphore);
        printf("thread %u executing------------------>\n", thread_id);
        pthread_mutex_lock(&pool->mutex);
        struct Task *t;
        t = dequeue();
        pthread_mutex_unlock(&pool->mutex);
        if (t == NULL) {
            break;
        } 
        printf("thread %u get task: %d\n", thread_id, num_tasks);
        ++num_tasks;
        count_in_thread += execute(t->fn_pt, t->param);
        // sem_post can't be put here, 
        // because if job hasn't been submitted, then this process would be early stopped
    }
    pthread_mutex_lock(&pool->mutex);
    pool->count += count_in_thread;
    pthread_mutex_unlock(&pool->mutex);
    printf("thread %u exits--------------->\n", thread_id);
    pthread_exit(0);
}