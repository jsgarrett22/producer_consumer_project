/*
Comp323 - Proj 2
Description - A program that demonstrates the Producer-Consumer problem, illustrating synchronization of a bounded buffer resource
Author - Joshua S. Garrett
Date - 04/02/2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include "buffer.h"

// Global declarations
pthread_mutex_t mutex;
int buffer[BUFFER_SIZE];
sem_t empty;
sem_t full;
int in = 0;
int out = 0;
int argS;

int insert_item(buffer_item item) {
    /* Insert item into buffer - FIGURE 6.1*/
    sem_wait(&empty);                                           // semaphore version of while (count == BUFFER_SIZE)
    pthread_mutex_lock(&mutex);
    
    buffer[in] = item;
    printf("Producer produced %d at %d\n", buffer[in], in);
    in = (in + 1) % BUFFER_SIZE;
    /* UNLOCK MUTEX & INCREMENT SEMAPHORE */
    pthread_mutex_unlock(&mutex);
    sem_post(&full);                                            // semaphore version of count++
    return 0;
}

int remove_item() {
    /* Remove item from buffer - FIGURE 6.1 */
    int consumed;
   
    sem_wait(&full);                                            // semaphore version of while (count == 0)
    pthread_mutex_lock(&mutex);
    
    consumed = buffer[out];
    printf("Consumer consumed %d from %d\n", consumed, out);
    out = (out + 1) % BUFFER_SIZE;
    
    /* UNLOCK MUTEX & DECREMENT SEMAPHORE */
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);                                           // semaphore version of count--
    return 0;
}

void* producer(void* param) {
    buffer_item item;
    while(true) {
        /* sleep for a random period of time */
        sleep(rand() % argS);
        
        /* generate a random number and assign it to item*/
        item = rand() % 100;
        
        /* Add item to buffer */
        insert_item(item);
    }
}

void* consumer(void* param) {
    while(true) {
        /* sleep for a random period of time */
        sleep(rand() % argS);
        
        /* Remove item from buffer */
        remove_item();
    }
}

void printName() {
    printf("Proj 2 - Written by: Joshua S. Garrett\n");
    printf("=====================\n");
}

int main(int argc, char* argv[])
{
    /* Validate number of command line arguments */
    if (argc != 4) {
        printf("Three command line arguments required!");
        exit(-1);
    }
    
    /* Convert command line arguments to integers
        argS - How long to sleep before terminating
        argP - The number of producer threads
        argC - The number of consumer threads
    */
    argS = atoi(argv[1]);
    int argP = atoi(argv[2]);
    int argC = atoi(argv[3]);
    
    // declare thread variables
    pthread_t prod[argP], con[argC];
    
    // initialize mutex
    pthread_mutex_init(&mutex, NULL);
    
    // initialize semaphores
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    
    // seed rand() with sys time
    srand(time(NULL));

    // print full name and project number to terminal
    printName();
    
    /* Create Producer Threads */
    for (int i = 0; i < argP; i++) {
        if (pthread_create(&prod[i], NULL, producer, NULL) != 0) {
            printf("Failed to create producer.");
        } else {
            printf("Starting producer %d\n", i);
        }
    }
    
    /* Create Consumer Threads */
    for (int i = 0; i < argC; i++) {
        if (pthread_create(&con[i], NULL, consumer, NULL) != 0) {
            printf("Failed to create consumer.");
        } else {
            printf("Starting consumer %d\n", i);
        }
    }
    
    /* Join Producer Threads */
    for (int i = 0; i < argP; i++) {
        pthread_join(prod[i], NULL);
        printf("Terminating producer %d\n", i);
    }
    
    /* Join Consumer Threads */
    for (int i = 0; i < argC; i++) {
        pthread_join(con[i], NULL);
        printf("Terminating consumer %d\n", i);
    }
    
    /* Destroy semaphores and mutex */
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    
    sleep(argS);
    
    /* EXIT */
    return 0;
}