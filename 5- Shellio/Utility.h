/*============================================================================
 * @file name      : utilities.h
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

#ifndef UTILITIES_H_
#define UTILITIES_H_

/*===================================  Includes ===============================*/
#include "Commands.h"      // Custom commands specific to Shellio

/*===================================  Definitions ============================*/
#define EXIT                                  0    // Exit status code
#define NOT_EXIT                              1    // Status code indicating no exit
#define CLEARED                               0    // Status code indicating the screen is cleared
#define NOT_CLEARED                           1    // Status code indicating the screen is not cleared
#define RAISED                                1    // Status code indicating a flag is raised
#define NEWLINE_INPUT                         11   // Status code indicating newline input
#define PWD_PASS                              0    // Status code for a successful 'pwd' command
#define ECHO_PASS                             0    // Status code for a successful 'echo' command
#define MV_PASS                               0    // Status code for a successful 'mv' command
#define COPY_PASS                             0    // Status code for a successful 'cp' command
#define CD_PASS                               0    // Status code for a successful 'cd' command
#define TYPE_PASS                             0    // Status code for a successful 'type' command
#define ENV_PASS                              0    // Status code for a successful 'envir' command
#define HELP_PASS                             0    // Status code for a successful 'assist' command
#define VALID                                 1    // General status code for valid operations
#define INVALID                               0    // General status code for invalid operations
#define EQUALED                               0    // Status code indicating equality
#define SUCCESS                               1    // Status code indicating success
#define FAILED                                0    // Status code indicating failure
#define PARENT                               -2



/*
 * Name             : Utility_ExecuteCommands
 * Description      : Executes a series of piped commands by forking child processes 
 *                    and connecting their input/output via pipes.
 * Input            : input - The input string containing commands separated by pipes.
 * Output           : None
 * Return           : A pointer to the last command if executed by the child process, 
 *                    or NULL if no command was executed.
 * Notes            : The function parses the input into individual commands, creates 
 *                    pipes for communication between them, and handles the necessary 
 *                    file descriptor redirections. or handle command with 
 */
void Utility_ExecuteCommands(char* input);


/*
 * Name             : Utility_PrintPrompt
 * Description      : Prints the shell prompt for user input.
 *                    This function typically includes the username and hostname in the prompt.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The format of the prompt can be customized.
 */
void Utility_PrintPrompt();

#endif