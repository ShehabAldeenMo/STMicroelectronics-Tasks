#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>

// Non-blocking wait
class Solution
{
private:
    static void wait_handler(int sig, siginfo_t *siginfo, void *ucontext);

public:
    static int wait_child();
};

// Signal handler with SA_SIGINFO
void Solution::wait_handler(int sig, siginfo_t *siginfo, void *ucontext)
{
    std::cout << ("CHILD PID: ") << siginfo->si_pid << "\n";
    std::cout << ("CHILD UID: ") << siginfo->si_uid << "\n";
    std::cout << ("CHILD Exit Status: ") << siginfo->si_status << "\n";

    // Check termination reason
    if (siginfo->si_code == CLD_EXITED)
    {
        std::cout << "\nTermination: Normal exit\n";
    }
    else if (siginfo->si_code == CLD_KILLED || siginfo->si_code == CLD_DUMPED)
    {
        std::cout << "\nTermination: Killed by signal " << siginfo->si_status << "\n";
    }
}

int Solution::wait_child()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = Solution::wait_handler; // Use sa_sigaction instead of sa_handler
    sigemptyset(&sa.sa_mask);                 // non blocking for othewr signals
    sa.sa_flags = SA_SIGINFO | SA_NOCLDWAIT;  // Enable SA_SIGINFO to get extra info

    // Set the handler for SIGUSR1 -> a custom user signal.
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        return false;
    }
    return true;
}
