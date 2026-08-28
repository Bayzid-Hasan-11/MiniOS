#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "sync.h"

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

// OS Synchronization Primitives
sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void* producer(void* arg) {
    (void)arg; // suppress unused warning
    for(int i = 1; i <= NUM_ITEMS; i++) {
        sem_wait(&empty);               // Wait if buffer is full
        pthread_mutex_lock(&mutex);     // Lock critical section

        buffer[in] = i;
        printf("[Producer] Created item %2d and placed it at index %d\n", i, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);   // Unlock critical section
        sem_post(&full);                // Signal that a new item is available
        
        usleep(100000); // Sleep for 100ms to simulate work
    }
    return NULL;
}

void* consumer(void* arg) {
    (void)arg;
    for(int i = 1; i <= NUM_ITEMS; i++) {
        sem_wait(&full);                // Wait if buffer is empty
        pthread_mutex_lock(&mutex);     // Lock critical section

        int item = buffer[out];
        printf("[Consumer] Removed item %2d from index %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);   // Unlock critical section
        sem_post(&empty);               // Signal that space freed up
        
        usleep(150000); // Sleep 150ms to simulate slower consumer processing
    }
    return NULL;
}

void run_sync_demo() {
    pthread_t prod_thread, cons_thread;

    // Initialize semaphores and mutex
    sem_init(&empty, 0, BUFFER_SIZE); // Initially, all slots are empty
    sem_init(&full, 0, 0);            // Initially, 0 slots are full
    pthread_mutex_init(&mutex, NULL);

    printf("\n--- Starting Producer-Consumer Synchronization Demo ---\n");

    // Spawn OS Threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Cleanup
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    printf("--- Synchronization Demo Completed Safely ---\n\n");
}
