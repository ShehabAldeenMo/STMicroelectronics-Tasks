#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

// Define the alternate stack size
// SIGSTKSZ: The min size to handle some signal
#define ALT_STACK_SIZE (SIGSTKSZ + 1024)

// Signal handler function
void signal_handler(int signo) {
    printf("Signal %d received and handled on the alternate stack\n", signo);
    exit(0);  // Exit the program
}

int main() {
    // Allocate memory for the alternate stack
    stack_t ss;
    ss.ss_sp = malloc(ALT_STACK_SIZE);
    if (ss.ss_sp == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    ss.ss_size = ALT_STACK_SIZE;
    ss.ss_flags = 0;

    // Set the alternate stack
    if (sigaltstack(&ss, NULL) == -1) {
        perror("sigaltstack");
        exit(EXIT_FAILURE);
    }

    printf("Alternate stack set up successfully\n");

    // Set up the signal action
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;    // Use the handler
    sa.sa_flags = SA_ONSTACK;          // Use the alternate stack
    sigemptyset(&sa.sa_mask);          // Block no additional signals
 
    // Register the signal handler for SIGSEGV (Segmentation fault)
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Signal handler with SA_ONSTACK set for SIGSEGV\n");

    // Trigger a segmentation fault to test the handler
    printf("Triggering a segmentation fault...\n");
    *(int *)0 = 0;  // Write to a null pointer

    // Clean up
    free(ss.ss_sp);

    return 0;
}

