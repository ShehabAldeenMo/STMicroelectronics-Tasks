/*============================================================================
 * @file name      : Helper.h
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * This header file contains function declarations and macro definitions for
 * the Helper module in the Shellio project. It provides utility functions 
 * used across various components of the Shellio application.
 * 
 * Ensure that all functions declared in this header are defined in the 
 * corresponding Helper.c file.
 *
 * Include this file in source files where these utility functions are required.
 *
 ******************************************************************************
 ==============================================================================
*/


#ifndef HELPER_H_
#define HELPER_H_


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
#include <stdbool.h>        // Include this for the bool type
#include <ctype.h>
#include <signal.h>

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
#define OFF                                   0
#define ON                                    1
#define ENVIR_PASS                            0
#define ENV_VAR_PASS                          0
#define BUFFER_SIZE                          1024
#define MAX_PIPES                             5
#define PARENT                               -2
#define MAX_INPUT_LENGTH                     512
#define MAX_COMMANDS                          10
#define MAX_COMMAND_LENGTH                   100
#define FREE_PASS                             0    // Status code for a successful 'free' command
#define UPTIME_PASS                           0    // Status code for a successful 'uptime' command
#define ALLVAR_PASS                           0    // Status code for a successful 'allVar' command

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


/*
 * Name             : ProcessHistory Structure and Related Globals
 * Description      : Defines the structure for storing process history entries and
 *                    declares global variables related to process history management.
 *                    - ProcessHistory: A structure that holds command and status information.
 *                    - PtrProcessHistory: An array of pointers to ProcessHistory structures,
 *                      representing the global stack for storing process history.
 *                    - processCounter: A counter for tracking the number of elements in the stack.
 *                    - sharedString: A pointer to a string used for various operations in the shell.
 *                    - environ: A pointer to the environment variables.
 * */

// Define the structure for process history
typedef struct {
    uint8 *command;
    uint8 status;
} ProcessHistory;

/*
 * Name             : LocalVariable Structure and Related Globals
 * Description      : Defines the structure for storing local variables and
 *                    declares global variables related to local variable management.
 *                    - LocalVariable: A structure that holds the name and value of a local variable.
 *                    - localVariables: An array of LocalVariable structures, 
 *                      representing the global list of local variables.
 *                    - localVarCount: A counter for tracking the number of local variables stored.
 * */

// Define the structure for local variables
typedef struct {
    char name[MAX_VAR_NAME];  /**< Name of the local variable */
    char value[MAX_VAR_VALUE]; /**< Value of the local variable */
} LocalVariable;


/*===================  Helper File Functions Prototypes ========================*/
/*
 * Name             : Help_GetParsedPath
 * Description      : Parses and returns the next path from the input string, moving the global parsing pointer forward.
 *                    This function extracts the path enclosed in double quotes (") and advances Ptr_GlobalGetParsingPath.
 * Parameter In/Out : None
 * Input            : command - The command string containing the variable name and value.
 * Return           : uint8* - Returns a pointer to the next parsed path or NULL if no path is found.
 */
uint8* Help_GetParsedPath(uint8* command);

/*
 * Name             : Help_MyPrintf
 * Description      : A custom printf function for formatted output in Shellio.
 *                    This function allows formatted output similar to the standard printf function.
 * Parameter In/Out : None
 * Input            : format (const char*) - A format string containing the text to be written, optionally with format specifiers.
 *                    ... - Additional arguments to be formatted according to the format specifiers in the format string.
 * Return           : void
 */
void Help_MyPrintf(const char *format, ...);

/*
 * Name             : Help_CleanupProcessHistory
 * Description      : Cleans up the process history by freeing all allocated memory.
 *                    Iterates through the process history stack and frees each command string
 *                    and its associated memory.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function should be called when the process history is no longer needed
 *                    to prevent memory leaks.
 */
void Help_CleanupProcessHistory();

/*
 * Name             : Help_SearchOnCommand
 * Description      : Searches for an external command in the directories specified in the PATH environment variable.
 *                    Checks if the command exists and is executable.
 * Input            : token - The command to search for.
 * Output           : None
 * Return           : SUCCESS if the command is found and executable, otherwise FAILED.
 * Notes            : The function uses the `access` system call to check the executability of the command.
 */
uint8 Help_SearchOnCommand(uint8 *token);


/*
 * Name             : Help_Redirect
 * Description      : Redirects the output to a specified file descriptor (FD) by opening the file at the given path.
 *                    The function handles file creation and setting the necessary permissions.
 * Input            : path - The path to the file where output should be Help_Redirected.
 *                    newFD - The file descriptor to be Help_Redirected (e.g., STDOUT or STDERR).
 * Output           : None
 * Return           : None
 * Notes            : The function uses `dup2` to Help_Redirect the output and `close` to close the original file descriptor.
 */
void Help_Redirect(uint8* path, int newFD);

/*
 * Name             : Help_ForkAssistRedirectionExec
 * Description      : Forks a new process to execute a command with output Help_Redirection.
 *                    The parent process waits for the child process to complete.
 * Input            : path - The path to the file for output Help_Redirection.
 *                    FD - The file descriptor to be Help_Redirected (e.g., STDOUT or STDERR).
 * Output           : None
 * Return           : None
 * Notes            : The function uses `fork` to create a new process and `Help_Redirect` to handle output Help_Redirection.
 */
void Help_ForkAssistRedirectionExec(uint8* path, int FD, int NullFD);

/*
 * Name             : Help_FindRedirectionPath
 * Description      : Processes a string to extract a file path for error Help_Redirection.
 *                    The function adjusts the input path to point to the correct file for error output.
 * Input            : path - The input string containing the file path prefixed with "2>".
 * Output           : None
 * Return           : uint8* - Returns the adjusted file path for error Help_Redirection.
 * Notes            : The function updates the global parsing path and processes relative paths.
 */
uint8* Help_FindRedirectionPath(uint8* path);

/*
 * Name             : Help_GetRelativePath
 * Description      : Computes the relative path from the provided absolute path.
 *                    If the path contains a specific keyword (e.g., "/home/shehabaldeen"),
 *                    it uses the path directly; otherwise, it constructs a relative path
 *                    based on the current working directory.
 * Input            : path - The absolute or relative path to be processed.
 * Output           : None
 * Return           : uint8* - Returns a pointer to the computed relative path.
 * Notes            : Allocates memory for the result path which should be freed by the caller.
 */
uint8* Help_GetRelativePath(uint8 path[]);

/*
 * Name             : Help_DisplaySeq
 * Description      : Displays a sequence of information based on the input string.
 *                    This function processes the input string to generate formatted output,
 *                    possibly including command execution results or status messages.
 * Input            : str - The input string to be processed and displayed.
 * Output           : None
 * Return           : None
 * Notes            : The function may include formatted output and error handling as needed.
 */
void Help_DisplaySeq(uint8* str);

/*
 * Name             : Help_GetPathSeq
 * Description      : Retrieves and displays the current working directory or path sequence.
 *                    This function outputs the current path or path-related information
 *                    to provide context or debugging information.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function may include system calls like `getcwd` to fetch the current directory.
 */
void Help_GetPathSeq();

/*
 * Name             : Help_AssistSeq
 * Description      : Provides help or guidance on using commands and functionalities.
 *                    This function displays information on available commands, usage,
 *                    and other relevant details to assist users.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The function may include a list of commands, descriptions, and usage examples.
 */
void Help_AssistSeq();


/*
 * Name             : Help_TypeSeq
 * Description      : Processes and displays the content of the provided string.
 *                    This function interprets the input string, performs required operations,
 *                    and outputs the result, which may include formatted text or command output.
 * Input            : str - The input string to be processed and displayed.
 * Output           : None
 * Return           : None
 * Notes            : The function may involve text formatting and output to the console.
 */
void Help_TypeSeq(uint8* str);

/*
 * Name             : Help_FreeSeq
 * Description      : Frees resources allocated for handling sequences.
 *                    This function cleans up and deallocates any memory or resources used
 *                    for managing sequences or related operations.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : Call this function to avoid memory leaks and ensure proper resource management.
 */
void Help_FreeSeq();

/*
 * Name             : Help_uptimeSeq
 * Description      : Retrieves and displays the system's uptime and idle time.
 *                    This function reads uptime information from the system's `/proc/uptime`
 *                    file and prints both the total uptime and idle time to the console.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : Uses system file operations to access uptime information and may handle errors.
 */
void Help_uptimeSeq();

/*
 * Name             : Help_PrintLocalVariables
 * Description      : Prints the value of a specified local variable.
 *                    This function searches for a local variable with the given name and
 *                    prints its value if found. If the variable does not exist, an error message is shown.
 * Input            : var - The name of the local variable to be printed.
 * Output           : None
 * Return           : uint8 - Returns VALID if the variable is found and printed, otherwise INVALID.
 * Notes            : This function is used for debugging or displaying variable values in the shell.
 */
uint8 Help_PrintLocalVariables(char* var);


/*
 * Name             : Help_PushProcessHistory
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
void Help_PushProcessHistory(const uint8 *command, uint8 status);


/*
 * Name             : Commands_SetLocalVariable
 * Description      : Sets a local variable with a specified name and value.
 *                    Updates or adds the variable to the local environment.
 * Input            : 
 *    - name: The name of the variable.
 *    - value: The value to be assigned to the variable.
 * Output           : None
 * Return           : None
 */
void Help_SetLocalVariable(const char* name, const char* value);

/*
 * Name             : Help_CleanSharedString
 * Description      : Frees the memory allocated for the shared string.
 *                    Ensures that the shared string is properly cleaned up after use to prevent memory leaks.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function should be called whenever the shared string is no longer needed.
 */
void Help_CleanSharedString();

/*
 * Name             : GetPathWithoutToken
 * Description      : Retrieves the current working directory path without any additional tokens.
 * Input            : None
 * Output           : None
 * Return           : uint8* - The path of the current working directory.
 * Notes            : The function may handle path tokens or other modifications internally.
 */
uint8* Help_GetPathWithoutToken();

/*
 * Name             : Help_SearchOnSpaceBeforeArrow
 * Description      : Searches for a space character immediately preceding an arrow ('>') in the provided path string.
 * Input            : path - A pointer to the string where the search is to be performed.
 * Output           : None
 * Return           : Returns the index of the space character found before the arrow, 
 *                    or -1 if no such space exists.
 * Notes            : This function is typically used to validate syntax before handling 
 *                    output Help_Redirection in commands.
 */
int     Help_SearchOnSpaceBeforeArrow (char* path);

/*
 * Name             : Help_RedirectionHandlerOfnoOption
 * Description      : Handles Help_Redirection for commands without specific options, 
 *                    typically involving '>' or '2>' for standard output or error Help_Redirection.
 * Input            : command - A pointer to the command string that includes the Help_Redirection symbol.
 * Output           : None
 * Return           : Returns `SUCCESS` if the Help_Redirection was handled successfully, 
 *                    or an error code otherwise.
 * Notes            : The function modifies the command string to remove Help_Redirection symbols 
 *                    and set up the appropriate file descriptors for Help_Redirection.
 */
uint8   Help_RedirectionHandlerOfnoOption(uint8* command);

/*
 * Name             : Help_RedirectionHandlerOfWithOption
 * Description      : Handles Help_Redirection for commands with specific options, including input and output Help_Redirection.
 * Input            : command - A pointer to the command string that includes Help_Redirection and options.
 * Output           : None
 * Return           : A pointer to the modified command string with Help_Redirection handled, 
 *                    or an error code otherwise.
 * Notes            : This function is used to handle more complex Help_Redirection scenarios 
 *                    where both input and output need to be managed and there is an option from user also
 */
uint8*  Help_RedirectionHandlerOfWithOption(uint8* command);

/*
 * Name             : Help_HandleOptionRedirection
 * Description      : Processes a command string to handle Help_Redirection based on specified delimiters.
 * Input            : input      - The original command string.
 *                    delimiters - A string containing the delimiters used to identify Help_Redirection points.
 * Output           : None
 * Return           : A pointer to a new string with Help_Redirection handled, 
 *                    or NULL if no Help_Redirection is detected.
 * Notes            : This function is designed to work with commands where Help_Redirection 
 *                    needs to be parsed and processed separately.
 */
uint8*  Help_HandleOptionRedirection(const char *input, const char* delimiters);

/*
 * Name             : Help_TrimSpaces
 * Description      : Removes leading and trailing spaces from the provided string.
 * Input            : str - The string from which spaces will be trimmed.
 * Output           : None
 * Return           : None
 * Notes            : This function directly modifies the input string to eliminate any 
 *                    unnecessary spaces before or after the command.
 */
void    Help_TrimSpaces(char *str);

/*
 * Name             : Help_ParseCommand
 * Description      : Parses the input string into separate commands based on the pipe '|' delimiter.
 * Input            : input    - The input command string to be parsed.
 *                    commands - An array of strings where parsed commands will be stored.
 * Output           : None
 * Return           : The number of commands parsed from the input string.
 * Notes            : This function splits the input string at each pipe '|' and stores 
 *                    each command in the provided array.
 */
char    Help_ParseCommand(char *input, char **commands) ;


/*
 * Name             : Help_GetUserName
 * Description      : Retrieves the username of the current user.
 * Input            : None
 * Output           : None
 * Return           : const char* - The username of the current user.
 * Notes            : This function typically uses system-specific methods to get the current username.
 */
const char* Help_GetUserName();

/*
 * Name             : Help_GetHostName
 * Description      : Retrieves the hostname of the system.
 *                    Copies the hostname into the provided buffer.
 * Input            : 
 *    - hostname: Buffer to store the hostname.
 *    - size: The size of the buffer.
 * Output           : None
 * Return           : None
 * Notes            : The buffer should be large enough to hold the hostname string.
 */
void Help_GetHostName(char *hostname, size_t size);


/*
 * Name             : Help_SetSharedString
 * Description      : Sets the shared string used for various operations in the shell.
 *                    Duplicates the input string `str` and stores it in the global `sharedString` variable.
 * Input            : str - The string to be set as the shared string.
 * Output           : None
 * Return           : None
 * Notes            : The function uses `strdup` to duplicate the input string.
 */
void Help_SetSharedString(const uint8 * str);

#endif