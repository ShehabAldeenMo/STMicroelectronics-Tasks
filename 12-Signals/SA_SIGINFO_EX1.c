#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// Signal handler with SA_SIGINFO
void handler(int sig, siginfo_t *siginfo, void *ucontext) {
    printf("Received signal %d\n", sig);
    printf("Sent by process PID: %d\n", siginfo->si_pid);
    printf("User ID of sender: %d\n", siginfo->si_uid);

    if (sig == SIGSEGV) {
        printf("Segmentation fault at address: %p\n", siginfo->si_addr);
    }
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_flags = SA_SIGINFO; // Enable SA_SIGINFO to get extra info
    sa.sa_sigaction = handler; // Use sa_sigaction instead of sa_handler

    // Set the handler for SIGUSR1 -> a custom user signal.
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Send SIGUSR1 to this process using: kill -USR1 %d\n", getpid());

    // Wait for a signal
    while (1) {
        pause();
    }

    return 0;
}

