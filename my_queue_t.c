//
// Created by Dandelonium on 3/12/2022.
//
#include "my_queue_t.h"

int my_queue_init(int size, my_queue_t *q) {
    *q = malloc(sizeof(**q));
    (*q)->size = size;
    (*q)->array = (int*) malloc((*q)->size * sizeof(int));
    sem_init(&(*q)->semaphore, 0, (*q)->size); //TODO broken
    pthread_mutex_init(&(*q)->lock, NULL);
    (*q)->write = -1; //!Indicates that queue is empty
    (*q)->read = -1; //!Indicates that queue is empty
    return 0;
}

int my_queue_uninit(my_queue_t q) {
    sem_destroy(&q->semaphore);
    pthread_mutex_destroy(&q->lock);
    free(q->array);
    free(q);
    return 0;
}


int my_queue_write(my_queue_t q, int val) { //! Only lock semaphore, unlocking is done when read is performed
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

int my_queue_read(my_queue_t q, int *val) {
    int count = 0;

    //TODO mutex for while and if
    while(1) { //! Empty Queue
        pthread_mutex_lock(&q->lock); //! Lock to prevent unwanted change to indexes
        sem_getvalue(&q->semaphore, &count);
        printf("%d \n", count);
        if (count != q->size) {
            pthread_mutex_unlock(&q->lock); //! Unlock and repeat loop
            break;
        }
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