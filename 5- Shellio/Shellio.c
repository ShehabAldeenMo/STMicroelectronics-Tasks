/*============================================================================
 * @file name      : Shellio.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkdIn         : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * Shellio is a custom command-line shell designed to provide a simple interface 
 * for users to interact with the system. It allows the execution of both built-in 
 * commands (like path, clone, shift, leave, cls, display) and external programs. The shell 
 * supports local and environment variables, redirection, command history, and more.
 * The command execution is enhanced by custom handling of errors and outputs. The 
 * shell also includes features for displaying system information like memory usage 
 * and uptime.
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ===============================*/
#include "Shellio.h"   // Include the header file that contains function declarations and macros for the Shellio program

/*===================================  Main Code ==============================*/
int main() {
    /* Initiliazations */
    char str[MAXSIZE] = {0};           // Buffer to store the input command from the user
    uint8 *token;                      // Pointer to store each word of the command as a token
    uint8* separators = "============================================================================================";  // Separator line for formatting output

    char* commands[MAX_PIPES] ;
    char TypeOfProcess = INVALID_ID ;

    do {
        /* Enter your command */
        printPrompt();                 // Function to print the shell prompt to the user
        char cwd[MAX_PATH];            // Buffer to store the current working directory path
        strcpy(cwd, GetPathWithoutToken());  // Get the current working directory path and copy it to 'cwd'
        printf("%s%s%s >> ", COLOR_BOLD_CYAN, cwd, COLOR_RESET);  // Print the current working directory in cyan color

        /* Use fgets to read a whole line of input including spaces */ 
        if (fgets(str, MAXSIZE, stdin) == NULL) {  // Read the input from the user
            perror("fgets failed");  // Print an error message if fgets fails
            continue;  // Skip to the next iteration of the loop
        }

        /* Remove trailing newline character, if present */ 
        size_t len = strlen(str);  // Get the length of the input string
        if (len > 0 && str[len-1] == '\n') {  // Check if the last character is a newline
            str[len-1] = '\0';  // Replace the newline character with a null terminator
        }
        
        if (strchr (str,'|') ){
            Execute_Piped_Commands(str);
            continue;
        }

        /* to remove spaces in first and last of each command */
        trim_spaces(str);        

        // To share this input into the history
        setSharedString(str);  // Store the input command in a global history buffer

        /* Initial call to strtok */ 
        token = strtok(str," ");  // Tokenize the input string using the specified delimiters

        if (token != NULL) {  // If a token was found
            /* print goodbye then exit */ 
            if (strcmp(token, "leave") == EQUALED) {  // If the command is 'leave'
                uint8 status = Shellio_Exit(token);  // Call the exit function

                /* exit if it succeeded */
                if (status == SUCCESS) {  // If the exit command succeeded
                    break;  // Exit the loop and terminate the shell
                }
            }
            /* clear screen with clear command */ 
            else if (strcmp(token, "cls") == CLEARED) {  // If the command is 'cls' (clear screen)
                Shellio_Clear(token);  // Clear the screen
            }
            /* get absolute path of current working directory */
            else if (strcmp(token, "path") == PWD_PASS) {  // If the command is 'path'
                Shellio_GetPath(token);  // Print the current working directory
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else if (strcmp(token, "display") == ECHO_PASS) {  // If the command is 'display'
                Shellio_EchoInput(token);  // Echo the input back to the user
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else if (strcmp(token, "assist") == HELP_PASS) {  // If the command is 'assist'
                Shellio_Help(token);  // Display help information
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else if (strcmp(token, "clone") == COPY_PASS) {  // If the command is 'clone'
                Shellio_Copy(token);  // Copy a file or directory
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else if (strcmp(token, "shift") == MV_PASS) {  // If the command is 'shift'
                Shellio_MoveFile(strcmp(token, "shift"));  // Move a file or directory
                Shellio_Copy(token);  // Copy a file or directory
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else if (strcmp(token, "cd") == CD_PASS) {  // If the command is 'cd'
                Shellio_ChangeDir(token);  // Change the current working directory
            }
            else if (strcmp(token, "type") == TYPE_PASS) {  // If the command is 'type'
                Shellio_TypeCommand(token);  // Display the contents of a file
            } 
            else if (strcmp(token, "envir") == ENV_PASS) {  // If the command is 'envir'
                uint8 * command = strdup(token);  // Duplicate the command string
                token = strtok(NULL, "");  // Get the next token, which is the argument to 'envir'

                if (token == NULL || *(token) == '2' || (*token ) == '>' && strstr(token, " < ") == NULL ) {  // If no argument or redirection is provided
                    Shellio_PrintEnv(command, token);  // Print the environment variables
                    printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
                } else {
                    Shellio_PrintEnvVar(command, token);  // Print the value of a specific environment variable
                    printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
                }
                free(command);  // Free the memory allocated for the duplicated command
            } 
            else if (strcmp(token, "phist") == EXIT) {  // If the command is 'phist'
                Shellio_Phist(token);  // Print the process history
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else if (strcmp(token, "free") == FREE_PASS) {  // If the command is 'free'
                Shellio_Meminfo(token);  // Display memory usage information
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            } 
            else if (strcmp(token, "uptime") == UPTIME_PASS) {  // If the command is 'uptime'
                Shellio_uptime(token);  // Display system uptime and idle time
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else if (strchr(token, '=') != NULL) {  // If the token contains an '=' character
                Shellio_setVariable(token);  // Set a local variable with the format name=value
            }
            else if (strcmp(token, "allVar") == ALLVAR_PASS) {  // If the command is 'allVar'
                Shellio_allVar();  // Print all local and environment variables
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            else {
                Shellio_ExecExternalCommands(token);  // Execute an external command
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
        }

        // terminate child process
        if (TypeOfProcess > INVALID_ID ){
            exit(VALID);
        }

        /* clear all flags and buffers */
        memset(str, 0, sizeof(str));  // Clear the input buffer for the next command

    } while (1);  // Loop indefinitely until the user decides to exit

    return 0;  // Return 0 to indicate successful execution
}





/*

1- Source file isn't correct
clone "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/Shehab.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt"

2- Same sourc and destination
clone "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt"

3- Normally copy

"/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/5- Shellio/file.txt"

clone "file.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/copiedfile.txt"
clone "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/copiedfile.txt"
clone "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3"

4- Wrong destination
clone "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/Wrong/hgv"

5- Append
clone "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt" -a "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/copiedfile.txt"

6- Wrong option
clone "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt" -f "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/copiedfile.txt"

7- mv with different directory
shift "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/file.txt"
 
8- mv with forced option on file
shift "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/copiedfile.txt" -f "//home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/file.txt"

9- mv with no forced option on file
shift "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/Session 3/fileMove.txt" "/home/shehabaldeen/Desktop/Linux/STMicroelectronics/STMicroelectronics-Tasks/file.txt"


path 
path > "file.txt"
path 2> "file.txt"
path > "file.txt" 2> "file2.txt"
path 2> "file.txt" > "file2.txt"


cd ..
cd 5- Shellio

display She hab 
display She hab > "file.txt"
display She hab 2> "file.txt"
display She hab > "file.txt" 2> "file2.txt"
display She hab 2> "file.txt" > "file2.txt"
display < "file3.txt"
display < "file3.txt" > "file.txt" 2> "file2.txt"
display < "file3.txt" 2> "file.txt" > "file2.txt"
display 2> "file.txt" > "file2.txt" < "file3.txt"

type cd
type < "file3.txt"
type cd 2> "file.txt" > "file2.txt"
type cd > "file.txt" 2> "file2.txt"
type < "file3.txt" > "file.txt" 2> "file2.txt"
type 2> "file.txt" > "file2.txt" < "file3.txt"


envir 2> "file.txt" > "file2.txt"
envir > "file.txt" 2> "file2.txt"

Name=Shehab

envir Name

envir PWD 2> "file.txt" > "file2.txt"
envir PWD > "file.txt" 2> "file2.txt"
envir < "file3.txt" 2> "file.txt" > "file2.txt"
envir > "file.txt" 2> "file2.txt" < "file3.txt"

phist > "file.txt"

uptime 2> "file.txt"

free > "file.txt"

allVar

ls -l
ls -l > "file.txt"                
ls -l 2> "file.txt"
ls -l 2> "file.txt" > "file2.txt"
ls -l > "file.txt" 2> "file2.txt" 

grep "shehab" non_existent_file.txt 2> "file.txt"


set follow-fork-mode child

*/