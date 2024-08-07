/*============================================================================
 * @file name      : Shellio.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkdIn         : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * Shellio is a simple, custom command-line shell designed to interact with users through a terminal interface. 
 * It provides a basic environment for entering and processing commands. Few commands: pwd, cp, mv, exit, clear,
 * echo.
 ******************************************************************************
 ==============================================================================
*/

#ifndef SHELLIO_H_
#define SHELLIO_H_

/*===================================  Includes ===============================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Commands.h"
#include <pwd.h>
    

/*===================================  Definations ============================*/
#define MAXSIZE                              1024
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



// Regular Colors
#define COLOR_BLACK "\033[0;30m"
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_CYAN "\033[0;36m"
#define COLOR_WHITE "\033[0;37m"

// Bold Colors
#define COLOR_BOLD_BLACK "\033[1;30m"
#define COLOR_BOLD_RED "\033[1;31m"
#define COLOR_BOLD_GREEN "\033[1;32m"
#define COLOR_BOLD_YELLOW "\033[1;33m"
#define COLOR_BOLD_BLUE "\033[1;34m"
#define COLOR_BOLD_MAGENTA "\033[1;35m"
#define COLOR_BOLD_CYAN "\033[1;36m"
#define COLOR_BOLD_WHITE "\033[1;37m"

// Background Colors
#define BACKGROUND_BLACK "\033[40m"
#define BACKGROUND_RED "\033[41m"
#define BACKGROUND_GREEN "\033[42m"
#define BACKGROUND_YELLOW "\033[43m"
#define BACKGROUND_BLUE "\033[44m"
#define BACKGROUND_MAGENTA "\033[45m"
#define BACKGROUND_CYAN "\033[46m"
#define BACKGROUND_WHITE "\033[47m"

// Reset
#define COLOR_RESET "\033[0m"


#endif /* Header Guard */