#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <PID> <data (0 or 1)>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t receiver_pid = atoi(argv[1]);
    int data = atoi(argv[2]);

    if (data < 0 || data > 3)
    {
        std::cout << "data: " << data << "\n";
        fprintf(stderr, "Error: data must between 0:3.\n");
        return EXIT_FAILURE;
    }

    // Prepare the signal value
    union sigval sig_data;
    sig_data.sival_int = data;

    // Send the real-time signal
    if (sigqueue(receiver_pid, SIGRTMIN, sig_data) == -1)
    {
        perror("sigqueue");
        return EXIT_FAILURE;
    }

    printf("Sent signal to PID %d with data %d\n", receiver_pid, data);
    return EXIT_SUCCESS;
}