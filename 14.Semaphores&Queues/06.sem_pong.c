#include <stdio.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
int main()
{

    sem_t *sem;

    sem = sem_open("/test\0", O_CREAT, 0644, 1);
    int fd = open("sem_file", O_CREAT | O_RDWR | O_APPEND, 0644);
    while (1)
    {

        sem_wait(sem);
        lseek(fd, 0, SEEK_END);
        write(fd, "pong\n", strlen("pong\n"));
        sem_post(sem);
        sleep(1);
    }

    return 0;
}
