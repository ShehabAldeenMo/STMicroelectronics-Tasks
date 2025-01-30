/*============================================================================
 * @file name      : fileutils.c
 * @Author         : Shehab Aldeen Mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 *
 * This file provides utility functions for handling file operations, including
 * printing file permissions, obtaining absolute paths, and displaying file
 * information with color coding.
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ==============================*/
#include "myls.h"

/*=============================   External Variables   ======================*/
extern char* Global_dir;  // Global variable for the base directory path

/*=================================  Functions Implementations ==============================*/
void print_permissions(mode_t mode) {
    char permissions[11];

    // Determine file type
    if (S_ISREG(mode)) permissions[0] = '-';  // Regular file
    else if (S_ISDIR(mode)) permissions[0] = 'd';  // Directory
    else if (S_ISLNK(mode)) permissions[0] = 'l';  // Symbolic link
    else if (S_ISCHR(mode)) permissions[0] = 'c';  // Character device
    else if (S_ISBLK(mode)) permissions[0] = 'b';  // Block device
    else if (S_ISFIFO(mode)) permissions[0] = 'p';  // FIFO/pipe
    else if (S_ISSOCK(mode)) permissions[0] = 's';  // Socket
    else permissions[0] = '?';  // Unknown file type

    // Owner permissions
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';

    // Group permissions
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';

    // Others permissions
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';

    permissions[10] = '\0';  // Null-terminate the string

    printf("%s ", permissions);  // Print permissions
}

char* getAbsolutePath(const char* inputPath) {
    // Allocate memory for the absolute path
    char *absolutePath = malloc(MAX_PATH_LENGTH);
    char path[MAX_PATH_LENGTH];

    // Construct the full path using the base directory and input path
    if (snprintf(path, sizeof(path), "%s/%s", Global_dir, inputPath) >= sizeof(path)) {
        fprintf(stderr, "Path truncated: %s/%s\n", Global_dir, inputPath);
        return NULL;
    }

    // Check for memory allocation error
    if (absolutePath == NULL) {
        perror("malloc error");
        return NULL;
    }

    // Resolve the full path to an absolute path
    if (realpath(path, absolutePath) == NULL) {
        perror("realpath error");
        free(absolutePath);
        return NULL;
    }

    return absolutePath;  // Return the resolved absolute path
}

void PrintColoredType(mode_t type, const char* text) {
    // Print file type with color coding
    if ((type & 00170000) == 0010000)  // Named pipe (FIFO)
        printf("%s%s%s\t", COLOR_PIPE, text, COLOR_RESET);
    else if ((type & 00170000) == 0020000)  // Character Special file
        printf("%s%s%s\t", COLOR_CHR, text, COLOR_RESET);
    else if ((type & 00170000) == 0040000)  // Directory
        printf("%s%s%s\t", COLOR_DIR, text, COLOR_RESET); 
    else if ((type & 00170000) == 0060000)  // Block Special file
        printf("%s%s%s\t", COLOR_BLK, text, COLOR_RESET);
    else if ((type & 00170000) == 0100000) {  // Regular file
        // Check if the file is executable
        if ((type & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0)
            printf("%s%s%s\t", COLOR_EXEC, text, COLOR_RESET);
        else
            printf("%s%s%s\t", COLOR_REG, text, COLOR_RESET);
    }
    else if ((type & 00170000) == 0120000)  // Symbolic link
        printf("%s%s%s\t", COLOR_LINK, text, COLOR_RESET);
    else if ((type & 00170000) == 0140000)  // Socket
        printf("%s%s%s\t", COLOR_SOCK, text, COLOR_RESET);
    else
        printf("Unknown file type\n");  // Handle unknown file types
}

void Print_L_OptionInfo(struct stat* buf) {
    // Print file permissions
    print_permissions(buf->st_mode);

    // Print number of hard links (2 characters wide, right-aligned)
    printf("%2ld ", (long) buf->st_nlink);

    // Print owner name (8 characters wide, left-aligned)
    struct passwd* pwd = getpwuid(buf->st_uid);
    printf("%-8s ", pwd->pw_name);

    // Print group name (8 characters wide, left-aligned)
    struct group* grp = getgrgid(buf->st_gid);
    printf("%-8s ", grp->gr_name);

    // Print file size (8 characters wide, right-aligned)
    printf("%8lld ", (long long) buf->st_size);

    // Print last modified time (formatted as "Month Day HH:MM")
    char time_str[64];
    struct tm* timeinfo = localtime(&buf->st_mtime);
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
    printf("%s ", time_str);
}
