/*============================================================================
 * @file name      : myls.c
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

/*===================================  Includes ==============================*/
#include "myls.h"


/*==============================  External Variables =========================*/
extern int errno;

/*==============================  local functions ============================*/
static int cmpstringp(const void *p1, const void *p2);


/*=================================  main ====================================*/
int main (int argc, char* argv[]){

    qsort(&argv[1], argc - 1, sizeof(char *), cmpstringp);

    if (argc == 1){
       do_ls(".");
       printf("\n");
    }
    else if (argc == 2){
        do_ls(argv[1]);
        printf("\n");
    }
    else {
       int i = 0;
       while(++i < argc){
        printf("%s%s%s:\n","'",argv[i],"'");
        do_ls(argv[i]);
        printf("\n\n");
       }
    }

    return 0 ; 
}





/*===================================  Functions Implementations ==============================*/
/*
 * Name             : do_ls
 * Description      : 
 * Parameter In/Out : 
 * Input            : 
 * Return           : 
 */
void do_ls(const char* dir) {
    DIR *dp;                         // Pointer to a directory stream
    char* Elements[MAX_ELEMENTS];    // Array to store filenames
    size_t ElementNumber = 0;        // Counter for the number of elements
    errno = 0;                       // Set errno to 0 before starting

    // Initialize Elements array
    for (size_t i = 0; i < MAX_ELEMENTS; ++i) {
        Elements[i] = NULL;
    }

    // Open the directory specified by the dir argument
    dp = opendir(dir);
    if (dp == NULL) {
        printf("Cannot open directory: %s\n", dir);
        return;
    }

    // Read directory entries and store filenames in Elements
    if (read_directory(dir, Elements, &ElementNumber) != 0) {
        closedir(dp); // Close the directory stream on error
        return;
    }

    // Sort the Elements array based on filenames
    qsort(Elements, ElementNumber, sizeof(char *), cmpstringp);

    // Process and print information for each file
    for (size_t i = 0; i < ElementNumber; ++i) {
        process_file(dir, Elements[i]);
    }

    // Cleanup
    cleanup(Elements, ElementNumber, dp);
}

/*
 * Name             : PrintColoredType
 * Description      : 
 * Parameter In/Out : 
 * Input            : 
 * Return           : 
 */
void PrintColoredType(mode_t type, const char* text) {
    if ((type & S_IFMT) == S_IFIFO)  // Named pipe (FIFO)
        printf("%s%s%s\t", COLOR_PIPE, text, COLOR_RESET);
    else if ((type & S_IFMT) == S_IFCHR)  // Character Special file
        printf("%s%s%s\t", COLOR_CHR, text, COLOR_RESET);
    else if ((type & S_IFMT) == S_IFDIR)  // Directory
        printf("%s'%s'%s\t", COLOR_DIR, text, COLOR_RESET);
    else if ((type & S_IFMT) == S_IFBLK)  // Block Special file
        printf("%s%s%s\t", COLOR_BLK, text, COLOR_RESET);
    else if ((type & S_IFMT) == S_IFREG) {  // Regular file
        // Check if the file is executable
        if ((type & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0)
            printf("%s%s%s\t", COLOR_EXEC, text, COLOR_RESET);
        else
            printf("%s%s%s\t", COLOR_REG, text, COLOR_RESET);
    }
    else if ((type & S_IFMT) == S_IFLNK)  // Symbolic link
        printf("%s%s%s\t", COLOR_LINK, text, COLOR_RESET);
    else if ((type & S_IFMT) == S_IFSOCK)  // Socket
        printf("%s%s%s\t", COLOR_SOCK, text, COLOR_RESET);
    else
        printf("Unknown file type\n");
}


/*
 * Name             : cmpstringp
 * Description      : 
 * Parameter In/Out : 
 * Input            : 
 * Return           : 
 */
static int cmpstringp(const void *p1, const void *p2)
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */
    static int count = 0;
    return strcmp(*(const char **) p1, *(const char **) p2);
}


int read_directory(const char* dir, char* Elements[], size_t* elementCount) {
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        // Skip hidden files (starting with '.')
        if (entry->d_name[0] == '.') {
            continue;
        }

        // Check if the number of elements has reached the maximum limit
        if (*elementCount >= MAX_ELEMENTS) {
            fprintf(stderr, "Too many elements\n");
            closedir(dp);
            return -1;
        }

        // Allocate memory for storing the file name and check for allocation failure
        Elements[*elementCount] = malloc(strlen(entry->d_name) + 1);
        if (Elements[*elementCount] == NULL) {
            perror("malloc error");
            closedir(dp);
            return -1;
        }
        strcpy(Elements[*elementCount], entry->d_name); // Copy the file name into allocated memory
        (*elementCount)++;
    }

    closedir(dp);
    return 0;
}

void process_file(const char* dir, const char* filename) {
    char path[MAX_PATH_LENGTH];
    struct stat buf;

    // Construct the full path of the file
    if (snprintf(path, sizeof(path), "%s/%s", dir, filename) >= sizeof(path)) {
        fprintf(stderr, "Path truncated: %s/%s\n", dir, filename);
        return;
    }

    // Use lstat to get file information for the full path
    if (lstat(path, &buf) == -1) {
        perror("lstat error");
        return;
    }

    // Print the file type with color coding
    PrintColoredType(buf.st_mode, filename);
}

void cleanup(char* Elements[], size_t elementCount, DIR* dp) {
    for (size_t i = 0; i < elementCount; ++i) {
        free(Elements[i]); // Free allocated memory for each filename
    }
    closedir(dp); // Close the directory stream
}