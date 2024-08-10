/*============================================================================
 * @file name      : Shellio.h
 * @Author         : Shehab Aldeen Mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
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
 *******************************************************************************
 ==============================================================================
*/


#ifndef SHELLIO_H_
#define SHELLIO_H_

/*===================================  Includes ===============================*/
#include "Commands.h"      // Custom commands specific to Shellio

/*===================================  Definitions ============================*/
#define MAXSIZE                              1024  // Maximum size for input strings
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
#define FREE_PASS                             0    // Status code for a successful 'free' command
#define UPTIME_PASS                           0    // Status code for a successful 'uptime' command
#define ALLVAR_PASS                           0    // Status code for a successful 'allVar' command

#endif /* Header Guard */
