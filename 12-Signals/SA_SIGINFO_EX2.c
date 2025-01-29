/*
Advanced Example: Handling SIGSEGV
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// SIGSEGV handler
void segfault_handler(int sig, siginfo_t *siginfo, void *ucontext) {
    printf("Segmentation Fault! Signal: %d\n", sig);
    printf("Faulty address: %p\n", siginfo->si_addr);
    exit(EXIT_FAILURE);
}

int main() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = segfault_handler;
    sigaction(SIGSEGV, &sa, NULL);

    printf("Triggering segmentation fault...\n");
    int *ptr = NULL;
    *ptr = 42; // This will cause a SIGSEGV

    return 0;
}

