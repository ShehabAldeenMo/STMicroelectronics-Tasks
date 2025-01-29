#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Signal handler for SIGALRM
void alarm_handler(int signum) {
    printf("Timer expired! Signal caught: %d\n", signum);
}

int main() {
    // Set up the signal handler
    signal(SIGALRM, alarm_handler);

    // Set an alarm for 5 seconds
    alarm(5);
    printf("Timer set for 5 seconds.\n");

    // Simulate some work
    sleep(3);
    printf("Turning off the timer before it expires.\n");

    // Turn off the alarm
    alarm(0);

    // Simulate more work
    sleep(3);
    printf("Program finished.\n");

    return 0;
}

