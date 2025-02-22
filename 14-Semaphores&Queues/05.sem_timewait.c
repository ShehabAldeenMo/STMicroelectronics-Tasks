#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int main()
{
    // Open or create a named semaphore with initial value 0 (locked state)
    sem_t *sem = sem_open("/my_semaphore", O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    // Set the timeout period (e.g., 5 seconds)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5; // Wait for up to 5 seconds

    // Attempt to acquire the semaphore with a timeout
    printf("Waiting for semaphore with timeout...\n");
    int result = sem_timedwait(sem, &ts);

    if (result == -1)
    {
        if (errno == ETIMEDOUT)
        {
            printf("Semaphore timed out! Could not acquire the semaphore.\n");
        }
        else
        {
            perror("sem_timedwait");
        }
        return 1;
    }

    // Semaphore acquired
    printf("Semaphore acquired! Performing operation...\n");
    sleep(2); // Simulate work

    // Release the semaphore
    if (sem_post(sem) == -1)
    {
        perror("sem_post");
        return 1;
    }

    // Close the semaphore
    if (sem_close(sem) == -1)
    {
        perror("sem_close");
        return 1;
    }

    printf("Semaphore released and closed.\n");

    if (sem_unlink("/my_semaphore") == -1)
    {
        perror("sem_unink");
        return 1;
    }

    printf("Semaphore unlinked\n");

    return 0;
}
