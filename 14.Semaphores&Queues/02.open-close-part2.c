#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    // Open the existing semaphore (it should already be created beforehand)
    sem_t *sem = sem_open("/my_semaphore", 0); // 0 means no creation, just open the existing semaphore
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    printf("Named semaphore '/my_semaphore' opened successfully!\n");

    // Perform some operation (simulated by a sleep)
    sleep(2); // Simulating work done with the semaphore

    // Close the named semaphore
    if (sem_close(sem) == -1)
    {
        perror("sem_close");
        return 1;
    }

    printf("Semaphore closed successfully.\n");

    return 0;
}
