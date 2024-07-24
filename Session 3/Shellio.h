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

/*===================================  Definations ============================*/
#define MAXSIZE                              512
#define EXIT                                  0
#define NOT_EXIT                              1
#define CLEARED                               0
#define NOT_CLEARED                           1
#define MAX_ARGUMENTS                         4
#define MAX_CHARACHTERS_OF_ONE_ARGUMENTS     128
#define FIRST_ARGUMENT                        0
#define SECOND_ARGUMENT                       1
#define THIRD_ARGUMENT                        2
#define FOURTH_ARGUMENT                       3
#define RAISED                                1
#define NEWLINE_INPUT                         11
#define PWD_PASS                              0
#define ECHO_PASS                             0
#define MV_PASS                               0
#define COPY_PASS                             0
#define HELP_PASS                             0

#endif /* Header Guard */