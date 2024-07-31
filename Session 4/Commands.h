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


#endif /* Header Guard */
