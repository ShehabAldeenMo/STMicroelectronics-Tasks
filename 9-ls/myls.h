/*============================================================================
 * @file name      : myls.h
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 *
 *
 ******************************************************************************
 ==============================================================================
*/

#ifndef MYLS_H_
#define MYLS_H_

/*===================================  Includes ==============================*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h> 
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

/*==================================  Definitions ===========================*/
#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')
#define COLOR_DIR       "\033[1;34m"  // Blue for directories
#define COLOR_REG       "\033[0m"     // Default for regular files
#define COLOR_LINK      "\033[1;36m"  // Cyan for symbolic links
#define COLOR_PIPE      "\033[1;33m"  // Yellow for named pipes
#define COLOR_SOCK      "\033[1;35m"  // Magenta for sockets
#define COLOR_BLK       "\033[1;33m"  // Yellow for block devices
#define COLOR_CHR       "\033[1;33m"  // Yellow for character devices
#define COLOR_RESET     "\033[0m"     // Reset to default color
#define COLOR_EXEC      "\033[1;32m"  // Bright green for regular executable files
#define FAIL              -1
#define SUCESS             1
#define MAX_PATH_LENGTH   255
#define MAX_ELEMENTS      255

// Custom data types
typedef unsigned char uint8;                 // 8-bit unsigned integer type
typedef unsigned int  uint16;                // 16-bit unsigned integer type



/*========================  myls File Functions Prototypes ===========================*/
/*
 * Name             : do_ls
 * Description      : 
 * Parameter In/Out : 
 * Input            : 
 * Return           : 
 */
void do_ls(const char* dir);

/*
 * Name             : PrintColoredType
 * Description      : 
 * Parameter In/Out : 
 * Input            : 
 * Return           : 
 */
void PrintColoredType(mode_t type, const char* text);



int read_directory(const char* dir, char* Elements[], size_t* elementCount);
void process_file(const char* dir, const char* filename);
void cleanup(char* Elements[], size_t elementCount, DIR* dp);

#endif