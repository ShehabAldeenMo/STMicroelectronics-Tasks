#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem; // Declare an unnamed semaphore

// Producer thread function
void *producer(void *arg)
{
    printf("Producer: Doing some work...\n");
    sleep(2); // Simulate some work with sleep
    printf("Producer: Signaling semaphore to consumer...\n");

    // Signal the semaphore to indicate work is done
    sem_post(&sem);

    return NULL;
}

// Consumer thread function
void *consumer(void *arg)
{
    printf("Consumer: Waiting for producer to signal...\n");

    // Wait on the semaphore before proceeding
    sem_wait(&sem);

    printf("Consumer: Got the signal, continuing work...\n");
    return NULL;
}

int main()
{
    pthread_t producer_thread, consumer_thread;

    // Initialize the semaphore with an initial value of 0, shared between threads
    if (sem_init(&sem, 0, 0) != 0)
    {
        perror("sem_init failed");
        return 1;
    }

    // Create the producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Wait for the threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Destroy the semaphore
    sem_destroy(&sem);

    return 0;
}
