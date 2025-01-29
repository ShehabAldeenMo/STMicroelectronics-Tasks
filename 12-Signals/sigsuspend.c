#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Signal handler for SIGINT
void sigint_handler(int signum) {
    printf("\nSIGINT received! Exiting gracefully.\n");
}

int main() {
    sigset_t block_mask, old_mask, wait_mask;

    // Set up the signal handler for SIGINT
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Block SIGINT before calling sigsuspend
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &block_mask, &old_mask);  // Block SIGINT

    // Set up the mask to wait only for SIGINT
    sigemptyset(&wait_mask);

    printf("Waiting for SIGINT (Press Ctrl+C)...\n");

    // Suspend the process and wait for SIGINT
    sigsuspend(&wait_mask);

    // Restore the original signal mask
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    printf("Program exiting.\n");
    return 0;
}

