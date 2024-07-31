/*============================================================================
 * @file name      : Shellio.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkdIn         : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * Commands is a simple, custom command-line shell designed to interact with users through a terminal interface. 
 * It provides a basic environment for entering and processing commands. Few commands: pwd, cp, mv, exit, clear,
 * echo. It used to provide API to shellio.
 ******************************************************************************
 ==============================================================================
*/

#ifndef COMMANDS_H_
#define COMMANDS_H_

/*===================================  Includes ==============================*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/syscall.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/wait.h>

/*==================================  Definitions ===========================*/
#define MAX_PATH                             1024
#define MAX_COPIED_CONTENT                   1024
#define EXIST                                 0
#define MAX_FILE_NAME                        256
#define SAME                                  0
#define CLEARED                               0
#define SET                                   1
#define MOVE_PASS                             0
#define MOVE_FAILED                           1
#define VALID                                 1
#define INVALID                               0
#define STDIN                                 0
#define STDOUT                                1
#define STDERR                                2
#define FD_INVALID                           -1
#define MAX_ARGUMENTS                         3
#define MAX_CHARACHTERS_OF_ONE_ARGUMENTS     128
#define SUCCESS                               1
#define FAILED                                0
#define FIRST_ARGUMENT                        0
#define SECOND_ARGUMENT                       1
#define THIRD_ARGUMENT                        2
#define MAX_STACK_SIZE                        10
#define MAX_ARGS                              5
#define MAXSIZE                              512
#define EXIT                                  0
#define NOT_EXIT                              1
#define CLEARED                               0
#define NOT_CLEARED                           1
#define RAISED                                1
#define NEWLINE_INPUT                         11
#define PWD_PASS                              0
#define ECHO_PASS                             0
#define MV_PASS                               0
#define COPY_PASS                             0
#define CD_PASS                               0
#define TYPE_PASS                             0
#define ENV_PASS                              0
#define HELP_PASS                             0
#define VALID                                 1
#define INVALID                               0
#define EQUALED                               0
#define SUCCESS                               1
#define FAILED                                0
#define INVALID_ID                           -1



typedef unsigned char uint8;
typedef unsigned int  uint16;

/*==========================  Function Prototypes ===========================*/

/*
 * Name             : Shellio_GetPath
 * Description      : Prints the current working directory to standard output.
 *                    This function uses the `getcwd` system call to retrieve the 
 *                    absolute path of the current working directory and outputs it.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : If retrieving the current directory fails (e.g., due to permissions 
 *                    issues or the directory being removed), an error message is printed using `perror`.
 */
void Shellio_GetPath();

/*
 * Name             : Shellio_EchoInput
 * Description      : Prints a given string to standard output.
 *                    This function mimics the behavior of the `echo` command, printing 
 *                    the provided string followed by a newline character.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : Assumes the input string is null-terminated and does not alter it.
 */
void Shellio_EchoInput();

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
 * Return           : Status of the option check.
 */
char Shellio_FileOption(const char* Copy_Option);

/*
 * Name             : Shellio_MoveFile
 * Description      : Sets a flag indicating a file move operation.
 *                    Used to mark the current operation as a move.
 * Input            : 
 *    - Copy_MoveFlag: The flag indicating the move operation.
 * Output           : None
 * Return           : None
 */
void Shellio_MoveFile(const char  Copy_MoveFlag);

/*
 * Name             : Shellio_Help
 * Description      : Displays a help menu with information on Shellio commands.
 *                    Provides usage instructions for the available commands.
 * Input            : None
 * Output           : None
 * Return           : None
 */
void Shellio_Help();

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
 * Input            : None
 * Output           : None
 * Return           : uint8 - Status code indicating the success or failure of the exit operation.
 * Notes            : This function is typically called when the "exit" command is entered by the user.
 */
uint8 Shellio_Exit();

/*
 * Name             : Shellio_Clear
 * Description      : Clears the terminal screen.
 *                    This function executes the command to clear the terminal screen, 
 *                    similar to the "clear" command in Unix-like systems, providing a 
 *                    clean slate for further command input.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The implementation may vary depending on the operating system and terminal environment.
 */
void Shellio_Clear();

/*
 * Name             : Shellio_Copy
 * Description      : Handles the copying of files within the shell.
 *                    This function manages the logic for copying files, 
 *                    including error checking and managing file permissions.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : It typically involves calling `Shellio_CopyFile` with appropriate arguments.
 */
void Shellio_Copy();

/*
 * Name             : Shellio_PrintEnvVar
 * Description      : Retrieves and prints the value of an environment variable specified by `copy_token`.
 *                    If the environment variable does not exist, it prints an error message.
 *                    Updates the process history with the status of the operation.
 * Input            : copy_token - A string representing the environment variable to be retrieved.
 * Output           : None
 * Return           : None
 * Notes            : The process history is updated with either SUCCESS or FAILED based on the outcome.
 */
void Shellio_PrintEnvVar(uint8* copy_token);

/*
 * Name             : Shellio_TypeCommand
 * Description      : Determines whether the given token is a built-in command or an external command.
 *                    Compares the token against predefined built-in command names and prints the result.
 *                    Checks if the token is an external command using the `SearchOnCommand` function.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The function does not execute the command, only identifies its type.
 */
void Shellio_TypeCommand();

/*
 * Name             : Shellio_PrintEnv
 * Description      : Prints all the environment variables available in the shell.
 *                    Iterates through the environment variables and prints each one.
 *                    Updates the process history with the status of the operation.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The function retrieves the environment variables from the global 'environ' array.
 */
void Shellio_PrintEnv();

/*
 * Name             : Shellio_ChangeDir
 * Description      : Changes the current working directory to the directory specified by the token.
 *                    Handles errors if the directory change fails.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The function uses the `chdir` system call to change the directory.
 */
void Shellio_ChangeDir();

/*
 * Name             : Shellio_Phist
 * Description      : Prints the process history, showing the commands executed and their status.
 *                    Iterates through the process history stack and prints each entry.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The process history stack is managed using an array of pointers.
 */
void Shellio_Phist();

/*
 * Name             : setSharedString
 * Description      : Sets the shared string used for various operations in the shell.
 *                    Duplicates the input string `str` and stores it in the global `sharedString` variable.
 * Input            : str - The string to be set as the shared string.
 * Output           : None
 * Return           : None
 * Notes            : The function uses `strdup` to duplicate the input string.
 */
void setSharedString (const uint8 * str);

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
 * Input            : command - The command executed.
 *                    status  - The status of the command execution (SUCCESS or FAILED).
 * Output           : None
 * Return           : None
 * Notes            : The function dynamically allocates memory for the new history entry.
 */
void pushProcessHistory(const uint8 *command, uint8 status) ;

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

#endif /* Header Guard */
