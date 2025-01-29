#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    sigset_t mask;
    siginfo_t info;

    // Block SIGUSR1 so that it can be received using sigwaitinfo
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    printf("Waiting for SIGUSR1 %d ... \n", getpid());

    // Wait for SIGUSR1 synchronously
    int sig = sigwaitinfo(&mask, &info);

    if (sig == SIGUSR1)
    {
        printf("Received SIGUSR1! Sender PID: %d\n", info.si_pid);
    }
    else
    {
        perror("sigwaitinfo");
        exit(EXIT_FAILURE);
    }

    return 0;
}
