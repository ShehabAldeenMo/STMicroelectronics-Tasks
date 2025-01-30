/*============================================================================
 * @file name      : utilities.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * 
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ===============================*/
#include "Utility.h"


void Utility_ExecuteCommands(char *input) {

    /***********************  call commands loop for single pipe *********************************/
    if (strchr (input,'|') == NULL) {
        Commands_Loop(input);
        return ;
    }

    /**************  call commands loop after parse pipes commands with pipes ********************/
    // Array to hold file descriptors for the pipes
    // (2 file descriptors per pipe, for reading and writing)
    int pipefds[2 * (MAX_COMMANDS - 1)];

    // Array to hold the parsed commands
    char *commands[MAX_COMMANDS];

    // Number of commands parsed from the input
    char num_commands = Help_ParseCommand(input, commands);

    // Create pipes for inter-process communication
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Array to hold process IDs of the child processes
    pid_t pids[MAX_COMMANDS];

    // Fork and execute each command
    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            // Determine the input and output file descriptors for the current command
            if (i > 0) dup2( pipefds[(i - 1) * 2], STDIN_FILENO);
            if (i < (num_commands-1) )  dup2(pipefds[i * 2 + 1], STDOUT_FILENO);

            /* Close all pipe file descriptors in the child process. */
            for (int k = 0; k < 2 * (num_commands-1); k++) {
                close(pipefds[k]);
            }

            Commands_Loop(commands[i]);

            exit(VALID); /* Exit the child process with the command's exit status. */
        }

        if (pid>0){
            Help_PushProcessHistory(commands[i], SUCCESS);  // Log the success in process history
        }
    }

    /* Close all pipe file descriptors in the parent process. */
    for (int i = 0; i < 2 * (num_commands-1); i++) {
        close(pipefds[i]);
    }

    // Wait for all child processes to complete
    // The number of commands is used to wait for all processes
    for (int i = 0; i <= (num_commands-1); i++) {
        wait(NULL);
    }
}

// Function to print the command prompt with user and hostname information
void Utility_PrintPrompt() {
    const char *user = Help_GetUserName(); // Retrieve the username
    char host[256]; // Buffer to store the hostname

    Help_GetHostName(host, sizeof(host)); // Retrieve the hostname
    // Print the username in bold green, followed by the hostname in bold green
    printf("%s%s%s", COLOR_BOLD_GREEN, user, COLOR_RESET);
    printf("@%s%s%s:", COLOR_BOLD_GREEN, host, COLOR_RESET);

    char cwd[MAX_PATH];            // Buffer to store the current working directory path
    strcpy(cwd, Help_GetPathWithoutToken());  // Get the current working directory path and copy it to 'cwd'
    printf("%s%s%s >> ", COLOR_BOLD_CYAN, cwd, COLOR_RESET);  // Print the current working directory in cyan color

}