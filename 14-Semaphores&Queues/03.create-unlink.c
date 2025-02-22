#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    // Open or create a named semaphore with initial value 1
    sem_t *sem = sem_open("/my_semaphore", O_CREAT);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    printf("Named semaphore '/my_semaphore' created successfully!\n");

    // Perform some operation (simulated by a sleep)
    sleep(2); // Simulating some work done with the semaphore

    // Unlink the named semaphore (removes it from the system)
    if (sem_unlink("/my_semaphore") == -1)
    {
        perror("sem_unlink");
        return 1;
    }

    printf("Semaphore '/my_semaphore' unlinked successfully.\n");

    // Close the semaphore (this is not necessary after unlinking, but it's good practice)
    if (sem_close(sem) == -1)
    {
        perror("sem_close");
        return 1;
    }

    printf("Semaphore closed successfully.\n");

    return 0;
}
