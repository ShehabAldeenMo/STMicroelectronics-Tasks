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
#include <stdio.h>         // Standard I/O functions
#include <string.h>        // String manipulation functions
#include <stdlib.h>        // Standard library for memory allocation and process control
#include "Commands.h"      // Custom commands specific to Shellio
#include <pwd.h>           // For password-related operations (e.g., getpwuid)

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
#define FREE_PASS                             0    // Status code for a successful 'free' command
#define UPTIME_PASS                           0    // Status code for a successful 'uptime' command
#define ALLVAR_PASS                           0    // Status code for a successful 'allVar' command

// Regular Colors
#define COLOR_BLACK "\033[0;30m"              // Black color code
#define COLOR_RED "\033[0;31m"                // Red color code
#define COLOR_GREEN "\033[0;32m"              // Green color code
#define COLOR_YELLOW "\033[0;33m"             // Yellow color code
#define COLOR_BLUE "\033[0;34m"               // Blue color code
#define COLOR_MAGENTA "\033[0;35m"            // Magenta color code
#define COLOR_CYAN "\033[0;36m"               // Cyan color code
#define COLOR_WHITE "\033[0;37m"              // White color code

// Bold Colors
#define COLOR_BOLD_BLACK "\033[1;30m"         // Bold black color code
#define COLOR_BOLD_RED "\033[1;31m"           // Bold red color code
#define COLOR_BOLD_GREEN "\033[1;32m"         // Bold green color code
#define COLOR_BOLD_YELLOW "\033[1;33m"        // Bold yellow color code
#define COLOR_BOLD_BLUE "\033[1;34m"          // Bold blue color code
#define COLOR_BOLD_MAGENTA "\033[1;35m"       // Bold magenta color code
#define COLOR_BOLD_CYAN "\033[1;36m"          // Bold cyan color code
#define COLOR_BOLD_WHITE "\033[1;37m"         // Bold white color code

// Background Colors
#define BACKGROUND_BLACK "\033[40m"           // Background black color code
#define BACKGROUND_RED "\033[41m"             // Background red color code
#define BACKGROUND_GREEN "\033[42m"           // Background green color code
#define BACKGROUND_YELLOW "\033[43m"          // Background yellow color code
#define BACKGROUND_BLUE "\033[44m"            // Background blue color code
#define BACKGROUND_MAGENTA "\033[45m"         // Background magenta color code
#define BACKGROUND_CYAN "\033[46m"            // Background cyan color code
#define BACKGROUND_WHITE "\033[47m"           // Background white color code

// Reset
#define COLOR_RESET "\033[0m"                 // Reset color code to default

#endif /* Header Guard */
