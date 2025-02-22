#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PATH "test.txt"

int main()
{
    std::cout << "Begin Our Parent Process \n";
    char *add = (char *)mmap(NULL, 1024, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pid_t PID = fork();

    if (PID == 0)
    { // chlid
        std::cout << "Child PID: " << getpid() << "\n";
        int i = 0;
        while (*(add + (i)) != '\0')
        {
            std::cout << *(add + (i));
            i++;
        }
    }
    else if (PID > 0)
    { // parent
        *(add + 0) = '0';
        *(add + 1) = '\n';
        *(add + 2) = '1';
        *(add + 3) = '\n';
        *(add + 4) = '2';
        *(add + 5) = '\n';
        *(add + 6) = '3';
        *(add + 7) = '\n';
        *(add + 8) = '\0';
    }
    else
    {
        std::cerr << "fork error \n";
    }

    return 0;
}