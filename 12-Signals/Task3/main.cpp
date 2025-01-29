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
        if (info->si_value.sival_int >= 0 && info->si_value.sival_int <= 3)
        {
            Solution::level = info->si_value.sival_int;
            std::cout << "level: " << Solution::level << "\n";
        }
    }

public:
    Solution(int init = LOG_INFO)
    {
        Solution::level = init;
    }

    void SetLevel()
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = handler_rtmin; // Use sa_sigaction instead of sa_handler
        sigemptyset(&sa.sa_mask);        // non blocking for othewr signals
        sigaction(SIGRTMIN, &sa, NULL);  // Set the handler for SIGUSR1 -> a custom user signal.
    }
};

// Initialize static variable
int Solution::level = Solution::LOG_INFO;

int main()
{
    // print PID of parent process
    std::cout << "My PID: " << getpid() << "\n";
    Solution Deb;
    Deb.SetLevel();

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