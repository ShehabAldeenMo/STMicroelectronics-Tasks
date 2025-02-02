#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>

class Solution
{
public:
    static const int LOG_ERROR = 0;
    static const int LOG_WARNING = 1;
    static const int LOG_INFO = 2;
    static const int LOG_DEBUG = 3;
    static int level;

private:
    static void handler_rtmin(int sig, siginfo_t *info, void *context)
    {
        level = info->si_value.sival_int; // Get the data sent with the signal
        std::cout << "level: " << Solution::level << "\n";
    }

public:
    Solution(int init = LOG_INFO)
    {
        level = init;
    }

    void SetLevel()
    {
        struct sigaction sa;

        sa.sa_flags = SA_SIGINFO;        // Use siginfo_t to get signal data
        sa.sa_sigaction = handler_rtmin; // Handler function
        sigemptyset(&sa.sa_mask);

        // Register the signal handler for SIGRTMIN
        if (sigaction(SIGRTMIN, &sa, NULL) == -1)
        {
            perror("sigaction");
        }
    }
};

int main()
{
    // print PID of parent process
    std::cout << "My PID: " << getpid() << "\n";
    Solution Debug;
    Debug.SetLevel();

    // stop till set my signal from another process
    pause();

    /*======================*/
    if (Solution::level == Solution::LOG_ERROR)
    {
        std::cout << "I am in Error Level Checking\n";
    }

    if (Solution::level == Solution::LOG_WARNING)
    {
        std::cout << "I am in Warning Level Checking\n";
    }

    if (Solution::level == Solution::LOG_INFO)
    {
        std::cout << "I am in Info Level Checking\n";
    }

    if (Solution::level == Solution::LOG_DEBUG)
    {
        std::cout << "I am in Debuging Level Checking\n";
    }
    /*=============*/

    return 0;
}