#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

// Signal handler for SIGALRM
void timer_handler(int signum) {
    printf("Timer expired! Signal caught: %d\n", signum);
}

int main() {
    struct itimerval timer;

    // Set up the signal handler
    signal(SIGALRM, timer_handler);

    // Configure the timer to fire in 5 seconds
    timer.it_value.tv_sec = 5;  // Initial timeout
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;  // No periodic firing
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }
    printf("Timer set for 5 seconds.\n");

    // Simulate some work
    sleep(3);
    printf("Turning off the timer before it expires.\n");

    // Turn off the timer by setting it to zero
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    // Simulate more work
    sleep(3);
    printf("Program finished.\n");

    return 0;
}

