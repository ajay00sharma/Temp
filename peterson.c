#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define NUM_ITERATIONS 5

bool flag[2] = {false, false}; // Flags for each process
int turn; // Indicates whose turn it is

void *process(void *arg) {
    int id = *((int *)arg); // Process ID (0 or 1)

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // Entry section
        flag[id] = true; // Indicate the process is ready
        turn = 1 - id; // Give turn to the other process

        // Wait until the other process is not ready or it's our turn
        while (flag[1 - id] && turn == 1 - id);

        // Critical section
        printf("Process %d is in the critical section.\n", id);
        sleep(1); // Simulate some work in the critical section

        // Exit section
        flag[id] = false; // Indicate the process is leaving the critical section
        printf("Process %d is leaving the critical section.\n", id);
    }

    return NULL;
}

int main() {
    pthread_t threads[2];
    int process_ids[2] = {0, 1}; // Process IDs

    // Create two threads
    pthread_create(&threads[0], NULL, process, &process_ids[0]);
    pthread_create(&threads[1], NULL, process, &process_ids[1]);

    // Wait for both threads to finish
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    printf("All processes completed.\n");
    return 0;
}

