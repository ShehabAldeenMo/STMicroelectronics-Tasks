#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

int main()
{
    int fd1 = open("example.txt", O_RDWR | O_CREAT, 0644);
    int fd2 = open("example.txt", O_RDWR); // New open() creates a new file description

    flock(fd1, LOCK_EX); // Lock file with fd1

    // This will block because fd2 is independent and tries to acquire the same lock
    flock(fd2, LOCK_EX);
    printf("This will never print if flock blocks.\n");

    close(fd1);
    close(fd2);
    return 0;
}
