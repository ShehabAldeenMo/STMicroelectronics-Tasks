#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include "Wait.hpp"

int main()
{
    std::cout << "Hi, I am parent process \n\n";

    pid_t PID;
    PID = fork();

    if (PID == 0)
    { // child process
        char *args[] = {(char *)"ls", (char *)"-l", (char *)NULL};
        execvp("ls", args);
    }
    else if (PID > 0)
    { // parent
        int status = Solution::wait_child();
        std::cout << "\nI will continue my work with status: " << (status == true ? "Succeed" : "Failed") << "\n\n";
    }
    else
    {
        std::cout << "error";
        exit(false);
    }

    while (1)
        ;

    return 0;
}
