#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

int main() {
    sigset_t mask;
    siginfo_t info;
    struct timespec timeout;

    // Block SIGUSR1 so that it can be received using sigtimedwait
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Set a timeout of 5 seconds
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;

    printf("Waiting for SIGUSR1 (timeout: 5 seconds)...\n");

    // Wait for SIGUSR1 with a timeout
    int sig = sigtimedwait(&mask, &info, &timeout);

    if (sig == SIGUSR1) {
        printf("Received SIGUSR1! Sender PID: %d\n", info.si_pid);
    } else if (sig == -1 && errno == EAGAIN) {
        printf("Timeout reached! No signal received.\n");
    } else {
        perror("sigtimedwait");
        exit(EXIT_FAILURE);
    }

    return 0;
}

