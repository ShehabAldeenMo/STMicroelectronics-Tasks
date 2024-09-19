/*============================================================================
 * @file name      : directories.c
 * @Author         : Shehab Aldeen Mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 *
 * This file contains functions for reading directory contents, processing
 * files within directories, and handling file information display based on
 * user-specified options.
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ==============================*/
#include "myls.h"

/*=============================   External Variables   ======================*/
extern char Options[MAX_NUMBER_OPTIONS];   // Array storing command-line options
extern char FirstEntry;                    // Flag indicating the first directory entry

/*=================================  Functions Implementations ==============================*/
int read_directory(const char* dir, char* Elements[], size_t* elementCount) {
    DIR *dp = opendir(dir);  // Open directory
    if (dp == NULL) {
        return -1;  // Failed to open directory
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        // Skip hidden files (files starting with '.')
        if (entry->d_name[0] == '.' && Options[OPTION_a] != SUCCESS) {
            continue;
        }

        // Check if the number of elements has reached the maximum limit
        if (*elementCount >= MAX_ELEMENTS) {
            fprintf(stderr, "Too many elements\n");
            closedir(dp);  // Close directory before returning
            return -1;
        }

        // Allocate memory for storing the file name
        Elements[*elementCount] = malloc(strlen(entry->d_name) + 1);
        if (Elements[*elementCount] == NULL) {
            perror("malloc error");
            closedir(dp);  // Close directory before returning
            return -1;
        }
        strcpy(Elements[*elementCount], entry->d_name); // Copy filename
        (*elementCount)++;
    }

    closedir(dp);  // Close directory
    return 0;      // Success
}


void process_file(const char* dir, const char* filename) {
    char path[MAX_PATH_LENGTH];
    struct stat buf;

    // Construct the full path of the file
    if (snprintf(path, sizeof(path), "%s/%s", dir, filename) >= sizeof(path)) {
        fprintf(stderr, "Path truncated: %s/%s\n", dir, filename);
        return;
    }

    // Use lstat to get file information
    if (lstat(path, &buf) == -1) {
        perror("lstat error");
        return;
    }

    if (FirstEntry == FAIL && Options[OPTION_l] == SUCCESS) {
        // Print total block size if -l option is set and it's the first entry
        printf("total %ld\n", buf.st_blocks);
        FirstEntry = SUCCESS;  // Set flag indicating first entry has been processed
    }

    process_fileHelper(buf, filename);  // Process file details
}


void process_fileHelper(struct stat buf, const char* filename) {
    // Print inode number if -i option is set
    if (SUCCESS == Options[OPTION_i]) {
        printf("%ld ", buf.st_ino);
    }

    // Print long listing format if -l option is set
    if (SUCCESS == Options[OPTION_l]) {
        Print_L_OptionInfo(&buf);
    }

    // Print the file type with color coding if -f option is set
    if (Options[OPTION_f] == SUCCESS) {
        printf("%s\t", filename);
    } else {
        PrintColoredType(buf.st_mode, filename);
    }
}

void cleanup(char* Elements[], size_t elementCount, DIR* dp) {
    for (size_t i = 0; i < elementCount; ++i) {
        free(Elements[i]);  // Free allocated memory for each filename
    }
    closedir(dp);  // Close directory stream
}
