#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>

#define FILE_PATH "testfile.txt"

void check_if_locked(int fd)
{
    // Try acquiring a non-blocking exclusive lock to check if it's free
    if (flock(fd, LOCK_EX | LOCK_NB) == 0)
    {
        printf("Parent: The file is NOT locked anymore. (Lock was released)\n");
        flock(fd, LOCK_UN); // Unlock immediately after checking
    }
    else
    {
        printf("Parent: The file is still locked.\n");
    }
}

int main()
{
    // Open the file
    int fd = open(FILE_PATH, O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(1);
    }

    // Parent acquires an exclusive lock
    printf("Parent: Acquiring lock...\n");
    if (flock(fd, LOCK_EX) == -1)
    {
        perror("Parent: Error locking file");
        exit(1);
    }
    printf("Parent: Lock acquired.\n");

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0)
    {
        // Child process
        printf("Child: Unlocking file...\n");
        if (flock(fd, LOCK_UN) == -1)
        {
            perror("Child: Error unlocking file");
        }
        else
        {
            printf("Child: File unlocked successfully!\n");
        }
        exit(0);
    }
    else
    {
        // Parent process
        wait(NULL); // Wait for child to finish
        printf("Parent: Checking if lock is still held...\n");
        check_if_locked(fd);
    }

    // Close file
    close(fd);
    return 0;
}
