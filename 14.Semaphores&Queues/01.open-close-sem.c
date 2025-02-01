#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>

int main()
{
    // Open or create a named semaphore with initial value 1
    sem_t *sem = sem_open("/my_semaphore", O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    printf("Named semaphore '/my_semaphore' created successfully!\n");

    // Perform some operation (simulated by a sleep)
    sleep(2); // Simulating some work done with the semaphore

    // Close the named semaphore
    if (sem_close(sem) == -1)
    {
        perror("sem_close");
        return 1;
    }

    printf("Semaphore closed successfully.\n");

    return 0;
}
