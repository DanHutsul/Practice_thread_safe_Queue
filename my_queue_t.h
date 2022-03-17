//
// Created by Dandelonium on 3/12/2022.
//

#ifndef L3_MY_QUEUE_T_H
#define L3_MY_QUEUE_T_H

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
    int size;
    int write;
    int read;
    pthread_mutex_t lock;
    sem_t semaphore; //! Indicates free space
    int* array;
} *my_queue_t;

int my_queue_init(int size, my_queue_t *q);

int my_queue_uninit(my_queue_t q);

int my_queue_write(my_queue_t q, int val);

int my_queue_read(my_queue_t q, int *val);

#endif //L3_MY_QUEUE_T_H
