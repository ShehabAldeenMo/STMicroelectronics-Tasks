#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./a.out <Option> <PID>\n";
        exit(false);
    }

    union sigval value;
    value.sival_int = (int)atoi(argv[1]); // Some data to send along with the signal
    std::cout << "level: " << value.sival_int << "\n";
    sigqueue((pid_t)atoi(argv[2]), SIGRTMIN, value);

    return 0;
}