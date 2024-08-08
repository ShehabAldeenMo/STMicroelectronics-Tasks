/*============================================================================
 * @file name      : Commands.h
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * This file provides the definitions and declarations for the Commands module, 
 * which is used by Shellio to execute various built-in and external commands. 
 * The module includes definitions for constants, color codes, and data types 
 * that are used across the Shellio application. It facilitates handling of 
 * operations like file manipulation, process control, environment management, 
 * and more, ensuring smooth execution of commands within the Shellio environment.
 ******************************************************************************
 ==============================================================================
*/


#ifndef COMMANDS_H_
#define COMMANDS_H_

/*===================================  Includes ==============================*/
#include <unistd.h>         // Provides access to the POSIX operating system API
#include <stdio.h>          // Standard I/O functions
#include <stdlib.h>         // Standard library functions: memory management, program utilities, etc.
#include <string.h>         // String manipulation functions
#include <libgen.h>         // Functions for pathname manipulation
#include <sys/stat.h>       // File status information
#include <sys/syscall.h>    // System call declarations
#include <sys/types.h>      // Data types used in system calls
#include <fcntl.h>          // File control options
#include <stdarg.h>         // Variable argument functions
#include <sys/wait.h>       // Declarations for waiting for process termination
#include <pwd.h>            // Functions for password database operations

/*==================================  Definitions ===========================*/
#define MAX_PATH                             1024  // Maximum length for a file path
#define MAX_COPIED_CONTENT                   1024  // Maximum size for copied file content
#define EXIST                                 0    // Status indicating the existence of a file or directory
#define MAX_FILE_NAME                        256   // Maximum length for a file name
#define SAME                                  0    // Status indicating two strings or values are the same
#define CLEARED                               0    // Status indicating the screen or buffer has been cleared
#define SET                                   1    // Status indicating a setting has been applied
#define MOVE_PASS                             0    // Status indicating a successful move operation
#define MOVE_FAILED                           1    // Status indicating a failed move operation
#define VALID                                 1    // Status indicating validity or correctness
#define INVALID                               0    // Status indicating invalidity or error
#define STDIN                                 0    // File descriptor for standard input
#define STDOUT                                1    // File descriptor for standard output
#define STDERR                                2    // File descriptor for standard error
#define FD_INVALID                           -1    // Status indicating an invalid file descriptor
#define MAX_ARGUMENTS                         3    // Maximum number of arguments for a command
#define MAX_CHARACHTERS_OF_ONE_ARGUMENTS     255   // Maximum number of characters for a single argument
#define SUCCESS                               1    // Status indicating success
#define FAILED                                0    // Status indicating failure
#define FIRST_ARGUMENT                        0    // Index for the first argument in a command
#define SECOND_ARGUMENT                       1    // Index for the second argument in a command
#define THIRD_ARGUMENT                        2    // Index for the third argument in a command
#define MAX_STACK_SIZE                        10   // Maximum size for the process history stack
#define MAX_ARGS                              5    // Maximum number of arguments a command can accept
#define EXIT                                  0    // Status indicating an exit command
#define NOT_EXIT                              1    // Status indicating a command that doesn't cause exit
#define CLEARED                               0    // Status indicating a cleared state (e.g., screen cleared)
#define NOT_CLEARED                           1    // Status indicating a non-cleared state
#define RAISED                                1    // Status indicating a raised flag or condition
#define UNRAISED                              0    // Status indicating a non-raised flag or condition
#define NEWLINE_INPUT                         11   // ASCII code for a newline character in input
#define PWD_PASS                              0    // Status indicating a successful execution of 'pwd' command
#define ECHO_PASS                             0    // Status indicating a successful execution of 'echo' command
#define MV_PASS                               0    // Status indicating a successful execution of 'mv' command
#define COPY_PASS                             0    // Status indicating a successful execution of 'cp' command
#define CD_PASS                               0    // Status indicating a successful execution of 'cd' command
#define TYPE_PASS                             0    // Status indicating a successful execution of 'type' command
#define ENV_PASS                              0    // Status indicating a successful execution of 'envir' command
#define HELP_PASS                             0    // Status indicating a successful execution of 'assist' command
#define VALID                                 1    // Status indicating a valid operation or condition
#define INVALID                               0    // Status indicating an invalid operation or condition
#define EQUALED                               0    // Status indicating that two values or strings are equal
#define SUCCESS                               1    // Status indicating a successful operation
#define FAILED                                0    // Status indicating a failed operation
#define INVALID_ID                           -1    // Status indicating an invalid identifier

// Color definitions for shell output
// Regular Colors
#define COLOR_BLACK "\033[0;30m"             // Regular black color
#define COLOR_RED "\033[0;31m"               // Regular red color
#define COLOR_GREEN "\033[0;32m"             // Regular green color
#define COLOR_YELLOW "\033[0;33m"            // Regular yellow color
#define COLOR_BLUE "\033[0;34m"              // Regular blue color
#define COLOR_MAGENTA "\033[0;35m"           // Regular magenta color
#define COLOR_CYAN "\033[0;36m"              // Regular cyan color
#define COLOR_WHITE "\033[0;37m"             // Regular white color

// Bold Colors
#define COLOR_BOLD_BLACK "\033[1;30m"        // Bold black color
#define COLOR_BOLD_RED "\033[1;31m"          // Bold red color
#define COLOR_BOLD_GREEN "\033[1;32m"        // Bold green color
#define COLOR_BOLD_YELLOW "\033[1;33m"       // Bold yellow color
#define COLOR_BOLD_BLUE "\033[1;34m"         // Bold blue color
#define COLOR_BOLD_MAGENTA "\033[1;35m"      // Bold magenta color
#define COLOR_BOLD_CYAN "\033[1;36m"         // Bold cyan color
#define COLOR_BOLD_WHITE "\033[1;37m"        // Bold white color

// Background Colors
#define BACKGROUND_BLACK "\033[40m"          // Background black color
#define BACKGROUND_RED "\033[41m"            // Background red color
#define BACKGROUND_GREEN "\033[42m"          // Background green color
#define BACKGROUND_YELLOW "\033[43m"         // Background yellow color
#define BACKGROUND_BLUE "\033[44m"           // Background blue color
#define BACKGROUND_MAGENTA "\033[45m"        // Background magenta color
#define BACKGROUND_CYAN "\033[46m"           // Background cyan color
#define BACKGROUND_WHITE "\033[47m"          // Background white color

#define MAX_VARS                             100  // Maximum number of variables (local/environment)
#define MAX_VAR_NAME                         50   // Maximum length of a variable name
#define MAX_VAR_VALUE                        100  // Maximum length of a variable value

// Reset color to default
#define COLOR_RESET "\033[0m"                // Reset color to terminal default

// Custom data types
typedef unsigned char uint8;                 // 8-bit unsigned integer type
typedef unsigned int  uint16;                // 16-bit unsigned integer type


/*==========================  Function Prototypes ===========================*/

/*
 * Name             : Shellio_GetPath
 * Description      : Prints the current working directory to standard output.
 *                    This function uses the `getcwd` system call to retrieve the 
 *                    absolute path of the current working directory and outputs it.
 * Input            : command - The command string containing any arguments (if applicable).
 * Output           : None
 * Return           : None
 * Notes            : If retrieving the current directory fails (e.g., due to permissions 
 *                    issues or the directory being removed), an error message is printed using `perror`.
 */
void Shellio_GetPath(uint8* command);

/*
 * Name             : Shellio_EchoInput
 * Description      : Prints a given string to standard output.
 *                    This function mimics the behavior of the `echo` command, printing 
 *                    the provided string followed by a newline character.
 * Input            : command - The command string containing the text to be echoed.
 * Output           : None
 * Return           : None
 * Notes            : Assumes the input string is null-terminated and does not alter it.
 */
void Shellio_EchoInput(uint8* command);

/*
 * Name             : Shellio_CopyFile
 * Description      : Copies content from a source file to a destination file. 
 *                    It can either append to or overwrite the destination file, 
 *                    and optionally delete the source file if moving.
 * Input            : 
 *    - Copy_1st_Path: The path of the source file.
 *    - Copy_2nd_Path: The path of the destination file or directory.
 * Output           : None
 * Return           : None
 * Notes            : 
 *    - Avoids overwriting if source and destination paths are the same.
 *    - Attempts to open the source file for reading; if it fails, an error is returned.
 *    - Destination file handling depends on global flags: append or overwrite.
 *    - The source file is deleted if the move flag is set.
 */
void Shellio_CopyFile(const char* Copy_1st_Path, const char* Copy_2nd_Path);

/*
 * Name             : Shellio_FileOption
 * Description      : Sets flags based on the specified file operation option.
 *                    Handles options such as appending or forced overwriting.
 * Input            : 
 *    - Copy_Option: The option string (e.g., "-a" for append, "-f" for forced overwrite).
 * Output           : None
 * Return           : Status of the option check (VALID or INVALID).
 */
char Shellio_FileOption(const char* Copy_Option);

/*
 * Name             : Shellio_MoveFile
 * Description      : Sets a flag indicating a file move operation.
 *                    Used to mark the current operation as a move.
 * Input            : 
 *    - Copy_MoveFlag: The flag indicating the move operation (SET or CLEARED).
 * Output           : None
 * Return           : None
 */
void Shellio_MoveFile(const char  Copy_MoveFlag);

/*
 * Name             : Shellio_Help
 * Description      : Displays a help menu with information on Shellio commands.
 *                    Provides usage instructions for the available commands.
 * Input            : command - The command string that may contain additional arguments.
 * Output           : None
 * Return           : None
 */
void Shellio_Help(uint8* command);

/*
 * Name             : Shellio_ParsingPath
 * Description      : Parses and extracts paths and options from command arguments.
 *                    It identifies source and destination paths, and any options.
 * Input            : 
 *    - ptr_ArgCounter: Pointer to a counter for the number of arguments parsed.
 *    - Ptr_1st_Path: Buffer to store the first path.
 *    - Ptr_2nd_Path: Buffer to store the second path.
 *    - Copy_token: Input string containing paths and optional parameters.
 * Output           : None
 * Return           : Pointer to any identified option in the input, or NULL if none.
 */
uint8* Shellio_ParsingPath(uint8* ptr_ArgCounter, uint8* Ptr_1st_Path, uint8* Ptr_2nd_Path, uint8* Copy_token);

/*
 * Name             : Shellio_Exit
 * Description      : Exits the shell program.
 *                    This function handles the cleanup and exit process for the shell, 
 *                    ensuring any necessary resources are released and the program 
 *                    terminates gracefully.
 * Input            : command - The command string (usually contains the "exit" keyword).
 * Output           : None
 * Return           : uint8 - Status code indicating the success or failure of the exit operation.
 * Notes            : This function is typically called when the "exit" command is entered by the user.
 */
uint8 Shellio_Exit(uint8* command);

/*
 * Name             : Shellio_Clear
 * Description      : Clears the terminal screen.
 *                    This function executes the command to clear the terminal screen, 
 *                    similar to the "clear" command in Unix-like systems, providing a 
 *                    clean slate for further command input.
 * Input            : command - The command string (typically "cls" or "clear").
 * Output           : None
 * Return           : None
 * Notes            : The implementation may vary depending on the operating system and terminal environment.
 */
void Shellio_Clear(uint8* command);

/*
 * Name             : Shellio_Copy
 * Description      : Handles the copying of files within the shell.
 *                    This function manages the logic for copying files, 
 *                    including error checking and managing file permissions.
 * Input            : command - The command string containing paths and options.
 * Output           : None
 * Return           : None
 * Notes            : It typically involves calling `Shellio_CopyFile` with appropriate arguments.
 */
void Shellio_Copy(uint8* command);

/*
 * Name             : Shellio_PrintEnvVar
 * Description      : Retrieves and prints the value of an environment variable specified by `copy_token`.
 *                    If the environment variable does not exist, it prints an error message.
 *                    Updates the process history with the status of the operation.
 * Input            : 
 *    - command: The command string containing the environment variable name.
 *    - copy_token: A string representing the environment variable to be retrieved.
 * Output           : None
 * Return           : None
 * Notes            : The process history is updated with either SUCCESS or FAILED based on the outcome.
 */
void Shellio_PrintEnvVar(uint8* command,uint8* copy_token);

/*
 * Name             : Shellio_TypeCommand
 * Description      : Determines whether the given token is a built-in command or an external command.
 *                    Compares the token against predefined built-in command names and prints the result.
 *                    Checks if the token is an external command using the `SearchOnCommand` function.
 * Input            : command - The command string containing the token to be checked.
 * Output           : None
 * Return           : None
 * Notes            : The function does not execute the command, only identifies its type.
 */
void Shellio_TypeCommand(uint8* command);

/*
 * Name             : Shellio_PrintEnv
 * Description      : Prints all the environment variables available in the shell.
 *                    Iterates through the environment variables and prints each one.
 *                    Updates the process history with the status of the operation.
 * Input            : 
 *    - command: The command string (typically "envir").
 *    - token: Additional arguments (if any) related to the environment variable.
 * Output           : None
 * Return           : None
 * Notes            : The function retrieves the environment variables from the global 'environ' array.
 */
void Shellio_PrintEnv(uint8* command,uint8* token);
/*
 * Name             : Shellio_ChangeDir
 * Description      : Changes the current working directory to the directory specified by the token.
 *                    Handles errors if the directory change fails.
 * Input            : command - The command string containing the path to change to.
 * Output           : None
 * Return           : None
 * Notes            : The function uses the `chdir` system call to change the directory.
 */
void Shellio_ChangeDir(uint8* command);

/*
 * Name             : Shellio_Phist
 * Description      : Prints the process history, showing the commands executed and their status.
 *                    Iterates through the process history stack and prints each entry.
 * Input            : command - The command string to indicate the history command.
 * Output           : None
 * Return           : None
 * Notes            : The process history stack is managed using an array of pointers.
 */
void Shellio_Phist(uint8* command);

/*
 * Name             : setSharedString
 * Description      : Sets the shared string used for various operations in the shell.
 *                    Duplicates the input string `str` and stores it in the global `sharedString` variable.
 * Input            : str - The string to be set as the shared string.
 * Output           : None
 * Return           : None
 * Notes            : The function uses `strdup` to duplicate the input string.
 */
void setSharedString(const uint8 * str);

/*
 * Name             : cleanSharedString
 * Description      : Frees the memory allocated for the shared string.
 *                    Ensures that the shared string is properly cleaned up after use to prevent memory leaks.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function should be called whenever the shared string is no longer needed.
 */
void cleanSharedString();

/*
 * Name             : pushProcessHistory
 * Description      : Pushes a new entry into the process history stack.
 *                    Creates a new process history entry with the command and status.
 *                    Manages the stack size, removing the oldest entry if the stack is full.
 * Input            : 
 *    - command: The command executed.
 *    - status: The status of the command execution (SUCCESS or FAILED).
 * Output           : None
 * Return           : None
 * Notes            : The function dynamically allocates memory for the new history entry.
 */
void pushProcessHistory(const uint8 *command, uint8 status);

/*
 * Name             : Shellio_ExecExternalCommands
 * Description      : Executes external commands by forking a new process and using execvp.
 *                    Constructs the arguments array and handles quotation marks in paths.
 *                    Manages the child process and waits for its completion.
 * Input            : token - The command and its arguments to be executed.
 * Output           : None
 * Return           : None
 * Notes            : The function updates the process history based on the success or failure of the command execution.
 */
void Shellio_ExecExternalCommands(uint8 *token);

/*
 * Name             : getUserName
 * Description      : Retrieves the username of the current user.
 * Input            : None
 * Output           : None
 * Return           : const char* - The username of the current user.
 * Notes            : This function typically uses system-specific methods to get the current username.
 */
const char* getUserName();

/*
 * Name             : getHostName
 * Description      : Retrieves the hostname of the system.
 *                    Copies the hostname into the provided buffer.
 * Input            : 
 *    - hostname: Buffer to store the hostname.
 *    - size: The size of the buffer.
 * Output           : None
 * Return           : None
 * Notes            : The buffer should be large enough to hold the hostname string.
 */
void getHostName(char *hostname, size_t size);

/*
 * Name             : printPrompt
 * Description      : Prints the shell prompt for user input.
 *                    This function typically includes the username and hostname in the prompt.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The format of the prompt can be customized.
 */
void printPrompt();

/*
 * Name             : GetPathWithoutToken
 * Description      : Retrieves the current working directory path without any additional tokens.
 * Input            : None
 * Output           : None
 * Return           : uint8* - The path of the current working directory.
 * Notes            : The function may handle path tokens or other modifications internally.
 */
uint8* GetPathWithoutToken();

/*
 * Name             : Shellio_Meminfo
 * Description      : Displays information about the system's memory usage.
 *                    Provides details such as total, used, and free memory.
 * Input            : command - The command string (typically "meminfo").
 * Output           : None
 * Return           : None
 */
void Shellio_Meminfo(uint8* command);

/*
 * Name             : Shellio_uptime
 * Description      : Displays the system's uptime.
 *                    Shows how long the system has been running since the last boot.
 * Input            : command - The command string (typically "uptime").
 * Output           : None
 * Return           : None
 */
void Shellio_uptime(uint8* command);

/*
 * Name             : Shellio_setVariable
 * Description      : Sets a local variable in the shell environment.
 *                    The function parses the command and sets the variable with the given value.
 * Input            : command - The command string containing the variable name and value.
 * Output           : None
 * Return           : None
 */
void Shellio_setVariable(uint8* command);

/*
 * Name             : Shellio_allVar
 * Description      : Prints all local variables currently set in the shell.
 *                    Lists the variables and their values.
 * Input            : None
 * Output           : None
 * Return           : None
 */
void Shellio_allVar();

/*
 * Name             : setLocalVariable
 * Description      : Sets a local variable with a specified name and value.
 *                    Updates or adds the variable to the local environment.
 * Input            : 
 *    - name: The name of the variable.
 *    - value: The value to be assigned to the variable.
 * Output           : None
 * Return           : None
 */
void setLocalVariable(const char* name, const char* value);

#endif /* Header Guard */
