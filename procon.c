#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define PRODUCE_COUNT 10

int buffer[BUFFER_SIZE];
int in = 0; // Index for the next produced item
int out = 0; // Index for the next consumed item

sem_t empty; // Semaphore to count empty slots
sem_t full;  // Semaphore to count full slots
pthread_mutex_t mutex; // Mutex for mutual exclusion

void* producer(void* arg) {
    for (int i = 0; i < PRODUCE_COUNT; i++) {
        // Produce an item
        int item = rand() % 100;

        // Wait for an empty slot
        sem_wait(&empty);
        
        // Enter critical section
        pthread_mutex_lock(&mutex);

        // Add item to the buffer
        buffer[in] = item;
        printf("Produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        // Exit critical section
        pthread_mutex_unlock(&mutex);

        // Signal that a new item is available
        sem_post(&full);
        sleep(1); // Simulate time taken to produce
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < PRODUCE_COUNT; i++) {
        // Wait for a full slot
        sem_wait(&full);

        // Enter critical section
        pthread_mutex_lock(&mutex);

        // Remove item from the buffer
        int item = buffer[out];
        printf("Consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        // Exit critical section
        pthread_mutex_unlock(&mutex);

        // Signal that a new empty slot is available
        sem_post(&empty);
        sleep(1); // Simulate time taken to consume
    }
    return NULL;
}

int main() {
    pthread_t prod_thread, cons_thread;

    // Initialize semaphores and mutex
    sem_init(&empty, 0, BUFFER_SIZE); // All slots are empty initially
    sem_init(&full, 0, 0);            // No slots are full initially
    pthread_mutex_init(&mutex, NULL); // Initialize mutex

    // Create producer and consumer threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Clean up
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    printf("All done!\n");
    return 0;
}

