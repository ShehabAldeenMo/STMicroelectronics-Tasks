#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    // Open or create a named semaphore with initial value 1
    sem_t *sem = sem_open("/my_semaphore", O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    // Simulate process trying to acquire the semaphore
    printf("Waiting to acquire the semaphore...\n");
    if (sem_wait(sem) == -1)
    {
        perror("sem_wait");
        return 1;
    }

    // Semaphore acquired, perform the operation
    printf("Semaphore acquired! Performing operation...\n");
    sleep(2); // Simulating work

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
