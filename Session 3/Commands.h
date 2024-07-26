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


/*==================================  Definations ===========================*/
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
#define MAX_ARGUMENTS                         4
#define MAX_CHARACHTERS_OF_ONE_ARGUMENTS     128


typedef unsigned char uint8;
typedef unsigned int  uint16;

/*==========================  Functions Prototypes ===========================*/

/*
 * Name             : Shellio_GetPath
 * Description      : Retrieves and prints the current working directory.
 *                    The function uses the `getcwd` system call to obtain the 
 *                    absolute path of the current working directory and prints 
 *                    it to the standard output.
 * Parameter In/Out : None
 * Input            : None
 * Return           : None
 * Notes            : If the current working directory cannot be retrieved (for example,
 *                    due to insufficient permissions or the directory being deleted), 
 *                    the function prints an error message using `perror`.
 */
void Shellio_GetPath      ();   

/*
 * Name             : Shellio_EchoInput
 * Description      : Outputs a given string to the standard output.
 *                    This function is designed to echo the input string provided
 *                    by the user, simulating the behavior of the `echo` command
 *                    in a shell. It prints the input string followed by a newline.
 * Parameter In/Out : None
 * Input            : Copy_Statment - A pointer to a null-terminated string that 
 *                                     represents the text to be echoed.
 * Return           : None
 * Notes            : The function does not modify the input string and assumes
 *                    that the string is properly null-terminated.
 */
void Shellio_EchoInput    (const char* Copy_Statment);

/*
 * Name             : Shellio_CopyFile
 * Description      : Copies the content of a source file to a destination file. 
 *                    This function can either append to the destination file or overwrite it, 
 *                    depending on the mode specified by global flags. It also supports moving 
 *                    the file by deleting the source file after copying.
 * Parameter In/Out : None
 * Input            : 
 *    - Copy_1st_Path: The path to the source file (input).
 *    - Copy_2nd_Path: The path to the destination file or directory (input).
 * Return           : None
 * Notes            : 
 *    - The function first checks if the source and destination paths are the same, 
 *      to avoid overwriting the source file.
 *    - It then attempts to open the source file for reading. If the source file cannot be opened, 
 *      the function returns an error.
 *    - The destination file is opened in either append mode ("a+") or overwrite mode ("wb"), 
 *      based on the global flags. If the destination file does not exist and the destination 
 *      path is a directory, a new file is created with the same name as the source file.
 *    - The function reads the content of the source file in chunks and writes it to the 
 *      destination file.
 *    - If the move operation flag is set, the source file is deleted after copying.
 *    - Global flags are cleared after the operation.
 */
void Shellio_CopyFile     (const char* Copy_1st_Path,const char* Copy_2nd_Path );

/*
 * Name             : Shellio_FileOption
 * Description      : Sets global flags based on the file operation option provided by the user.
 *                    This function handles options like append and forced overwrite for file operations.
 * Parameter In/Out : None
 * Input            : 
 *    - Copy_Option: The option string (e.g., "-a" for append, "-f" for forced overwrite) (input).
 * Return           : Status of checking
 */
char Shellio_FileOption   (const char* Copy_Option );

/*
 * Name             : Shellio_MoveFile
 * Description      : Sets the global flag for moving a file. This function is used to indicate whether 
 *                    the current operation is a move operation.
 * Parameter In/Out : None
 * Input            : 
 *    - Copy_MoveFlag: The flag indicating the move operation (input).
 * Return           : None
 */
void Shellio_MoveFile     (const char  Copy_MoveFlag );

/*
 * Name             : Shellio_Help
 * Description      : Displays the help menu with information on available commands in Shellio.
 *                    This function prints out the usage instructions for various Shellio commands.
 * Parameter In/Out : None
 * Input            : None
 * Return           : None
 */
void Shellio_Help         ();

void Shellio_ParsingPath (uint8* ptr_ArgCounter,uint8* Ptr_1st_Path,uint8* Ptr_Option, 
                                    uint8* Ptr_2nd_Path, uint8* Copy_token);

#endif /* Header Gaurd */