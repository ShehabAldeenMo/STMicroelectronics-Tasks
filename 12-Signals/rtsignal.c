#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler_rtmin(int sig, siginfo_t *info, void *context) {
    // Extract details from the siginfo_t structure
    printf("Received signal %d\n", sig);
    printf("Signal sent by process ID: %d\n", info->si_pid);
    printf("Real user ID of sender: %d\n", info->si_uid);
    printf("Signal source: %s\n", (info->si_code == SI_QUEUE) ? "SI_QUEUE" : "Unknown");

    if (sig == SIGRTMIN) {
        printf("Real-time signal data: %d\n", info->si_value.sival_int);
    }
}

int main() {
    struct sigaction sa_rtmin;

    // Setup signal handler for SIGRTMIN with SA_SIGINFO flag
    sa_rtmin.sa_sigaction = handler_rtmin;
    sa_rtmin.sa_flags = SA_SIGINFO;  // Use SA_SIGINFO to get additional info in handler
    sigemptyset(&sa_rtmin.sa_mask);

    if (sigaction(SIGRTMIN, &sa_rtmin, NULL) == -1) {
        perror("sigaction(SIGRTMIN)");
        return 1;
    }

    // Print the process ID for sending signals
    printf("PID: %d\n", getpid());

    // Sending SIGRTMIN with data using sigqueue
    union sigval value;
    value.sival_int = 42;  // Some data to send along with the signal
    if (sigqueue(getpid(), SIGRTMIN, value) == -1) {
        perror("sigqueue");
        return 1;
    }

    // Wait for the signal
    pause();  // Wait for the signal to be delivered

    return 0;
}
