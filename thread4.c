#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define COUNT_LIMIT 5

void *count(void *arg) {
    int thread_num = *((int *)arg);  // Get the thread number
    for (int i = 1; i <= COUNT_LIMIT; i++) {
        printf("Thread %d: Count %d\n", thread_num, i);
        sleep(1);  // Simulate work with a sleep
    }
    printf("Thread %d finished.\n", thread_num);  // Indicate thread completion
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    int thread1_num = 1;
    int thread2_num = 2;

    pthread_create(&thread1, NULL, count, &thread1_num);
    pthread_create(&thread2, NULL, count, &thread2_num);


    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("All threads completed.\n");
    return 0;
}

