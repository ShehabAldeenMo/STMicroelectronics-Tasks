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
    char* RetCommandFromPipe ;

    do {
        /* Enter your command */
        Utility_PrintPrompt();                 // Function to print the shell prompt to the user

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
        
        Utility_ExecuteCommands(str);

        /* clear all flags and buffers */
        memset(str, 0, sizeof(str));  // Clear the input buffer for the next command

    } while (1);  // Loop indefinitely until the user decides to exit

    return 0;  // Return 0 to indicate successful execution
}


/*
test cases:
-----------
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


ls -l | grep ".txt"
ls -l | grep ".txt" > "file.txt"
display Shehab | grep "She"
ls -l | grep ".txt" | sort

*/