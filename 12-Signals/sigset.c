#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void printSigset();

static void hanlder(int sig)
{
    printf("caught signal %d (%s)\n", sig, strsignal(sig));
}

int main()
{
    const int numSecs = 5;
    signal(SIGINT, hanlder);
    pause();
    sigset_t blockedSignals;
    sigemptyset(&blockedSignals);
    sigaddset(&blockedSignals, SIGINT);
    sigaddset(&blockedSignals, SIGTSTP);

    if (sigprocmask(SIG_BLOCK, &blockedSignals, NULL) == -1)
    {
        perror("sigpending");
    }
    printf("sleep for 5 seconds after  SIG_BLOCK SIGINT && SIGTSTP\n");
    sleep(numSecs);
    printf("first sleep is occured\n");

    if (sigprocmask(SIG_UNBLOCK, &blockedSignals, NULL) == -1)
    {
        perror("sigpending");
    }
    printf("sleep for 5 seconds after  SIG_UNBLOCK SIGINT && SIGTSTP\n");
    sleep(numSecs);
    while (1)
        ;

    return 0;
}