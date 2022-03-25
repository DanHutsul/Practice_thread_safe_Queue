//
// Created by Dandelonium on 3/12/2022.
//
#include "my_queue.h"
#define LENGTH 2

my_queue_t queues[LENGTH];
int my_queue_init(int size, my_queue_t *q) {
    if (size <= 0) {
        return -1;
    }
    for (int i = 0; i < LENGTH; i++) {
        if (!queues[i]) {
            queues[i] = malloc(sizeof(**q));
            *q = queues[i];
            queues[i]->size = size;
            queues[i]->array = (int*) malloc(queues[i]->size * sizeof(int));
            sem_init(&queues[i]->semaphore, 0, queues[i]->size);
            pthread_mutex_init(&queues[i]->lock, NULL);
            queues[i]->write = -1; //!Indicates that queue is empty
            queues[i]->read = -1; //!Indicates that queue is empty
            return 0;
        }
    }
    return -1;
}

int my_queue_uninit(my_queue_t q) {
    if (!q) {
        return -1;
    }
    for (int i = 0; i < LENGTH; i++) {
        if (queues[i] == q) {
            sem_destroy(&q->semaphore);
            pthread_mutex_destroy(&q->lock);
            q->size = 0;
            free(q->array);
            free(q);
            queues[i] = NULL; ///Set to null to signify freed element
            return 0;
        }
    }

    return -1;
}


int my_queue_write(my_queue_t q, int val) { //! Only lock semaphore, unlocking is done when read is performed
    if (!q) {
        return -1;
    }
    for (int i = 0; i < LENGTH; i++) {
        if (queues[i] == q) {
            sem_wait(&q->semaphore); //! Decrement semaphore - One less empty space in Queue
            pthread_mutex_lock(&q->lock); //! Lock to prevent unwanted change to indexes
            if (q->write == -1 && q->read == -1) { //! Empty Queue
                q->write++;
                q->read++;
            }
            q->array[q->write] = val;
            q->write = (q->write + 1) % q->size;
            pthread_mutex_unlock(&q->lock); //! Unlock because indexes are no longer needed
            return 0;
        }
    }
    return -1;
}

int my_queue_read(my_queue_t q, int *val) {
    if (!q) {
        return -1;
    }
    for (int i = 0; i < LENGTH; i++) {
        if (&*queues[i] == &*q) {
            int count = q->size;
            while(1) { //! Empty Queue
                pthread_mutex_lock(&q->lock); //! Lock to prevent unwanted change to indexes
                sem_getvalue(&q->semaphore, &count);
                if (count != q->size && q->read != -1) {
                    pthread_mutex_unlock(&q->lock); //! Unlock and repeat loop
                    break;
                }
                pthread_mutex_unlock(&q->lock); //! Unlock and repeat loop
            }
            *val = q->array[q->read];
            q->read = (q->read + 1) % q->size;
            if (q->read == q->write) { //! No items left in Queue
                q->write = -1; //!Indicates that queue is empty
                q->read = -1; //!Indicates that queue is empty
            }
            pthread_mutex_unlock(&q->lock); //! Unlock because indexes are no longer needed
            sem_post(&q->semaphore); //! Increment semaphore - One more empty space in Queue
            return 0;
        }
    }
    return -1;
}