/*============================================================================
 * @file name      : myls.h
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * This header file defines the interface for a custom implementation of the
 * `ls` command. It includes function prototypes, macro definitions, custom 
 * data types, and includes necessary system and standard library headers.
 * The functions and types provided support listing directory contents, 
 * printing file types with color, handling various file operations, and 
 * sorting/searching directory entries.
 *
 ******************************************************************************
 ==============================================================================
*/

#ifndef MYLS_H_
#define MYLS_H_

/*===================================  Includes ==============================*/
#include <ctype.h>    // Character handling functions
#include <stdio.h>    // Standard I/O functions
#include <stdlib.h>   // Standard library functions
#include <unistd.h>   // POSIX operating system API
#include <sys/types.h> // Data types used in system calls
#include <sys/stat.h>  // File status functions
#include <time.h>     // Time/date functions
#include <fcntl.h>    // File control options
#include <grp.h>      // Group database functions
#include <pwd.h>      // User database functions
#include <dirent.h>   // Directory entries functions
#include <string.h>   // String handling functions
#include <errno.h>    // Error number definitions
#include <getopt.h>   // Command-line option parsing

/*==================================  Definitions ===========================*/
#define COLOR_DIR       "\033[1;34m"  // Blue for directories
#define COLOR_REG       "\033[0m"     // Default for regular files
#define COLOR_LINK      "\033[1;36m"  // Cyan for symbolic links
#define COLOR_PIPE      "\033[1;33m"  // Yellow for named pipes
#define COLOR_SOCK      "\033[1;35m"  // Magenta for sockets
#define COLOR_BLK       "\033[1;33m"  // Yellow for block devices
#define COLOR_CHR       "\033[1;33m"  // Yellow for character devices
#define COLOR_RESET     "\033[0m"     // Reset to default color
#define COLOR_EXEC      "\033[1;32m"  // Bright green for regular executable files
#define FAIL              -1          // Failure return code
#define SUCCESS           1           // Success return code
#define MAX_PATH_LENGTH   255         // Maximum path length
#define MAX_ELEMENTS      30          // Maximum number of elements
#define MAX_NUMBER_OPTIONS 9          // Maximum number of command-line options
#define OPTION_l           0          // Option for long listing format
#define OPTION_a           1          // Option for including hidden files
#define OPTION_t           2          // Option for sorting by modification time
#define OPTION_u           3          // Option for sorting by access time
#define OPTION_c           4          // Option for sorting by change time
#define OPTION_i           5          // Option for including inode numbers
#define OPTION_f           6          // Option for using default sorting
#define OPTION_d           7          // Option for listing directories themselves
#define OPTION_1           8          // Option for one file per line

// Custom data types
typedef unsigned char uint8;    // 8-bit unsigned integer type
typedef unsigned int  uint16;   // 16-bit unsigned integer type

typedef struct {
    char *original_path;         // Original path of the file
    char resolved_path[MAX_PATH_LENGTH]; // Resolved absolute path of the file
} PathInfo;

/*========================  myls File Functions Prototypes ===========================*/
/*
 * Name             : do_ls
 * Description      : List directory contents based on provided options.
 * Parameter In/Out : dir - Directory path to list contents of.
 * Input            : dir - Directory path.
 * Return           : void
 */
void do_ls(const char* dir);

/*
 * Name             : PrintColoredType
 * Description      : Print file type with appropriate color coding.
 * Parameter In/Out : type - File type.
 *                    text - Text to print.
 * Input            : type - File type (e.g., directory, regular file).
 *                    text - Text associated with the file.
 * Return           : void
 */
void PrintColoredType(mode_t type, const char* text);

/*
 * Name             : read_directory
 * Description      : Read directory entries into an array of strings.
 * Parameter In/Out : dir - Directory path.
 *                    Elements - Array to store directory entries.
 *                    elementCount - Number of entries read.
 * Input            : dir - Directory path.
 * Output           : Elements - Array of directory entry strings.
 * Return           : int - Number of entries read, or error code.
 */
int read_directory(const char* dir, char* Elements[], size_t* elementCount);

/*
 * Name             : process_file
 * Description      : Process a single file for directory listing.
 * Parameter In/Out : dir - Directory path.
 *                    filename - Name of the file to process.
 * Input            : dir - Directory path.
 *                    filename - File name.
 * Return           : void
 */
void process_file(const char* dir, const char* filename);

/*
 * Name             : cleanup
 * Description      : Free allocated memory and close directory stream.
 * Parameter In/Out : Elements - Array of directory entry strings.
 *                    elementCount - Number of entries.
 *                    dp - Directory stream pointer.
 * Input            : Elements - Array of directory entry strings.
 *                    elementCount - Number of entries.
 *                    dp - Directory stream pointer.
 * Return           : void
 */
void cleanup(char* Elements[], size_t elementCount, DIR* dp);

/*
 * Name             : print_permissions
 * Description      : Print file permissions in a human-readable format.
 * Parameter In/Out : mode - File mode (permissions).
 * Input            : mode - File mode (permissions).
 * Return           : void
 */
void print_permissions(mode_t mode);

/*
 * Name             : Print_L_OptionInfo
 * Description      : Print additional information for long listing format.
 * Parameter In/Out : buf - File status buffer.
 * Input            : buf - File status buffer.
 * Return           : void
 */
void Print_L_OptionInfo(struct stat* buf);

/*
 * Name             : CheckOnOptions
 * Description      : Check and process command-line options.
 * Parameter In/Out : num - Number of options.
 *                    command - Array of options.
 * Input            : num - Number of options.
 *                    command - Array of options.
 * Return           : void
 */
void CheckOnOptions(int num, char** command);

/*
 * Name             : process_fileHelper
 * Description      : Helper function for processing a file based on file status.
 * Parameter In/Out : buf - File status buffer.
 *                    filename - Name of the file.
 * Input            : buf - File status buffer.
 *                    filename - File name.
 * Return           : void
 */
void process_fileHelper(struct stat buf, const char* filename);

/*
 * Name             : cmpHelper
 * Description      : Compare function for sorting files.
 * Parameter In/Out : p1 - Pointer to first element.
 *                    p2 - Pointer to second element.
 *                    buf1 - File status buffer for first element.
 *                    buf2 - File status buffer for second element.
 * Input            : p1 - Pointer to first element.
 *                    p2 - Pointer to second element.
 *                    buf1 - File status buffer for first element.
 *                    buf2 - File status buffer for second element.
 * Return           : char - Comparison result.
 */
char cmpHelper(const void *p1, const void *p2, struct stat* buf1, struct stat *buf2);

/*
 * Name             : getAbsolutePath
 * Description      : Convert a relative path to an absolute path.
 * Parameter In/Out : inputPath - Path to convert.
 * Input            : inputPath - Relative path.
 * Output           : Absolute path.
 * Return           : char* - Absolute path.
 */
char* getAbsolutePath(const char* inputPath);

/*
 * Name             : sortBuffer
 * Description      : Sort an array of strings based on specified options.
 * Parameter In/Out : argv - Array of strings to sort.
 *                    counter - Number of strings in array.
 *                    Options - Sorting options.
 * Input            : argv - Array of strings.
 *                    counter - Number of strings.
 *                    Options - Sorting options.
 * Return           : void
 */
void sortBuffer(char **argv, int counter, char Options[]);

/*
 * Name             : linearSearch
 * Description      : Perform binary search on an array of paths.
 * Parameter In/Out : paths - Array of path strings.
 *                    size - Size of the array.
 *                    target - Path to search for.
 * Input            : paths - Array of path strings.
 *                    size - Size of array.
 *                    target - Path to search.
 * Return           : int - Index of the target if found, otherwise -1.
 */
int linearSearch(char* paths[], int size, const char *target);

#endif // MYLS_H_
