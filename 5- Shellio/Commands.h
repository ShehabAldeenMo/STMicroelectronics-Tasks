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
#include "Helper.h"


/*==========================  Function Prototypes ===========================*/

/*
 * Name             : main
 * Description      : Entry point for the Shellio application. Continuously reads user 
 *                    commands, processes them, and executes the appropriate built-in or 
 *                    external commands. It supports command piping and handles redirections.
 * Input            : None
 * Output           : None
 * Return           : Returns 0 upon successful execution of the shell.
 * Notes            : The main loop reads commands from the user, processes them to handle 
 *                    piping and redirection, and executes them until the 'leave' command is issued.
 */
void Commands_Loop(char* str);

/*
 * Name             : Commands_GetPath
 * Description      : Prints the current working directory to standard output.
 *                    This function uses the `getcwd` system call to retrieve the 
 *                    absolute path of the current working directory and outputs it.
 * Input            : command - The command string containing any arguments (if applicable).
 * Output           : None
 * Return           : None
 * Notes            : If retrieving the current directory fails (e.g., due to permissions 
 *                    issues or the directory being removed), an error message is printed using `perror`.
 */
void Commands_GetPath(uint8* command);

/*
 * Name             : Commands_EchoInput
 * Description      : Prints a given string to standard output.
 *                    This function mimics the behavior of the `echo` command, printing 
 *                    the provided string followed by a newline character.
 * Input            : command - The command string containing the text to be echoed.
 * Output           : None
 * Return           : None
 * Notes            : Assumes the input string is null-terminated and does not alter it.
 */
void Commands_EchoInput(uint8* command);

/*
 * Name             : Commands_CopyFile
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
void Commands_CopyFile(const char* Copy_1st_Path, const char* Copy_2nd_Path);

/*
 * Name             : Commands_FileOption
 * Description      : Sets flags based on the specified file operation option.
 *                    Handles options such as appending or forced overwriting.
 * Input            : 
 *    - Copy_Option: The option string (e.g., "-a" for append, "-f" for forced overwrite).
 * Output           : None
 * Return           : Status of the option check (VALID or INVALID).
 */
char Commands_FileOption(const char* Copy_Option);

/*
 * Name             : Commands_MoveFile
 * Description      : Sets a flag indicating a file move operation.
 *                    Used to mark the current operation as a move.
 * Input            : 
 *    - Copy_MoveFlag: The flag indicating the move operation (SET or CLEARED).
 * Output           : None
 * Return           : None
 */
void Commands_MoveFile(const char  Copy_MoveFlag);

/*
 * Name             : Commands_Help
 * Description      : Displays a help menu with information on Shellio commands.
 *                    Provides usage instructions for the available commands.
 * Input            : command - The command string that may contain additional arguments.
 * Output           : None
 * Return           : None
 */
void Commands_Help(uint8* command);

/*
 * Name             : Commands_ParsingPath
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
uint8* Commands_ParsingPath(uint8* ptr_ArgCounter, uint8* Ptr_1st_Path, uint8* Ptr_2nd_Path, uint8* Copy_token);

/*
 * Name             : Commands_Exit
 * Description      : Exits the shell program.
 *                    This function handles the cleanup and exit process for the shell, 
 *                    ensuring any necessary resources are released and the program 
 *                    terminates gracefully.
 * Input            : command - The command string (usually contains the "exit" keyword).
 * Output           : None
 * Return           : uint8 - Status code indicating the success or failure of the exit operation.
 * Notes            : This function is typically called when the "exit" command is entered by the user.
 */
uint8 Commands_Exit(uint8* command);

/*
 * Name             : Commands_Clear
 * Description      : Clears the terminal screen.
 *                    This function executes the command to clear the terminal screen, 
 *                    similar to the "clear" command in Unix-like systems, providing a 
 *                    clean slate for further command input.
 * Input            : command - The command string (typically "cls" or "clear").
 * Output           : None
 * Return           : None
 * Notes            : The implementation may vary depending on the operating system and terminal environment.
 */
void Commands_Clear(uint8* command);

/*
 * Name             : Commands_Copy
 * Description      : Handles the copying of files within the shell.
 *                    This function manages the logic for copying files, 
 *                    including error checking and managing file permissions.
 * Input            : command - The command string containing paths and options.
 * Output           : None
 * Return           : None
 * Notes            : It typically involves calling `Commands_CopyFile` with appropriate arguments.
 */
void Commands_Copy(uint8* command);

/*
 * Name             : Commands_PrintEnvVar
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
void Commands_PrintEnvVar(uint8* command,uint8* copy_token);

/*
 * Name             : Commands_TypeCommand
 * Description      : Determines whether the given token is a built-in command or an external command.
 *                    Compares the token against predefined built-in command names and prints the result.
 *                    Checks if the token is an external command using the `SearchOnCommand` function.
 * Input            : command - The command string containing the token to be checked.
 * Output           : None
 * Return           : None
 * Notes            : The function does not execute the command, only identifies its type.
 */
void Commands_TypeCommand(uint8* command);

/*
 * Name             : Commands_PrintEnv
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
void Commands_PrintEnv(uint8* command,uint8* token);
/*
 * Name             : Commands_ChangeDir
 * Description      : Changes the current working directory to the directory specified by the token.
 *                    Handles errors if the directory change fails.
 * Input            : command - The command string containing the path to change to.
 * Output           : None
 * Return           : None
 * Notes            : The function uses the `chdir` system call to change the directory.
 */
void Commands_ChangeDir(uint8* command);

/*
 * Name             : Commands_Phist
 * Description      : Prints the process history, showing the commands executed and their status.
 *                    Iterates through the process history stack and prints each entry.
 * Input            : command - The command string to indicate the history command.
 * Output           : None
 * Return           : None
 * Notes            : The process history stack is managed using an array of pointers.
 */
void Commands_Phist(uint8* command);


/*
 * Name             : Commands_ExecExternalCommands
 * Description      : Executes external commands by forking a new process and using execvp.
 *                    Constructs the arguments array and handles quotation marks in paths.
 *                    Manages the child process and waits for its completion.
 * Input            : token - The command and its arguments to be executed.
 * Output           : None
 * Return           : None
 * Notes            : The function updates the process history based on the success or failure of the command execution.
 */
void Commands_ExecExternalCommands(uint8 *command);

/*
 * Name             : Commands_Meminfo
 * Description      : Displays information about the system's memory usage.
 *                    Provides details such as total, used, and free memory.
 * Input            : command - The command string (typically "meminfo").
 * Output           : None
 * Return           : None
 */
void Commands_Meminfo(uint8* command);

/*
 * Name             : Commands_uptime
 * Description      : Displays the system's uptime.
 *                    Shows how long the system has been running since the last boot.
 * Input            : command - The command string (typically "uptime").
 * Output           : None
 * Return           : None
 */
void Commands_uptime(uint8* command);

/*
 * Name             : Commands_setVariable
 * Description      : Sets a local variable in the shell environment.
 *                    The function parses the command and sets the variable with the given value.
 * Input            : command - The command string containing the variable name and value.
 * Output           : None
 * Return           : None
 */
void Commands_setVariable(uint8* command);

/*
 * Name             : Commands_allVar
 * Description      : Prints all local variables currently set in the shell.
 *                    Lists the variables and their values.
 * Input            : None
 * Output           : None
 * Return           : None
 */
void Commands_allVar();

#endif /* Header Guard */
