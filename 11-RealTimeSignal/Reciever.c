#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_signal(int sig, siginfo_t *info, void *context) {
    int data = info->si_value.sival_int; // Get the data sent with the signal

    printf("Received signal with data: %d\n", data);

    if (data == 0) {
        printf("Terminating gracefully.\n");
        exit(EXIT_SUCCESS);
    } else if (data == 1) {
        printf("Aborting (core dump generated).\n");
        abort(); // Terminate with a core dump
    } else {
        printf("Unknown data received: %d\n", data);
    }
}

int main() {
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;        // Use siginfo_t to get signal data
    sa.sa_sigaction = handle_signal; // Handler function
    sigemptyset(&sa.sa_mask);

    // Register the signal handler for SIGRTMIN
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("Receiver PID: %d\n", getpid());
    printf("Waiting for signals...\n");

    // Keep the receiver running to handle signals
    while (1) {
        pause(); // Wait for a signal
    }

    return EXIT_SUCCESS;
}

