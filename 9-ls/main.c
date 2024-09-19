/*============================================================================
 * @file name      : main.c
 * @Author         : Shehab Aldeen Mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * 
 * This file implements the main logic for the custom 'ls'-like utility 'myls'.
 * It handles command-line arguments, processes directories and files, and
 * applies various options to format and display the output.
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ==============================*/
#include "myls.h"


/*==============================   Variables    =============================*/
extern int errno;
char Options[MAX_NUMBER_OPTIONS] = {FAIL};
char FirstEntry = FAIL;
char* Global_dir = ""; // to fix lstat issue of reading files in directory



/*=================================  main ====================================*/
int main(int argc, char* argv[]) {
    int opt;

    if (argc == 1) {
        // No arguments provided, default to current directory
        do_ls(".");
        printf("\n");
    } else {
        // Parse command-line options
        CheckOnOptions(argc, argv);

        if (optind >= argc) {
            // No arguments after options, default to current directory
            do_ls(".");
            printf("\n");
        } else if (optind == argc - 1) {
            // One argument provided, process it as directory or file
            do_ls(argv[optind]);
            printf("\n");
        } else {
            // Multiple arguments provided
            if (Options[OPTION_d] == SUCCESS) {
                // If -d option is set, treat all arguments as directories
                for (int i = optind; i < argc; ++i) {
                    do_ls(argv[i]);
                }
                printf("\n");
            } else {
                // Handle other options and sort arguments
                PathInfo paths[MAX_ELEMENTS];
                int path_count = 0;

                if ( !(Options[OPTION_f] != SUCCESS && Options[OPTION_u] != SUCCESS && 
                    Options[OPTION_c] != SUCCESS && Options[OPTION_t] != SUCCESS) ){
                    // Convert each path to absolute path and store
                    for (int i = optind; i < argc; i++) {
                        paths[path_count].original_path = strdup(argv[i]);

                        // Resolve the absolute path
                        if (realpath(argv[i], paths[path_count].resolved_path) == NULL) {
                            perror("realpath error");
                            continue; // Skip this path if there's an error
                        }

                        argv[i] = strdup(paths[path_count].resolved_path);
                        path_count++;
                    }
                }

                // Sort paths based on options
                sortBuffer(&argv[optind], argc-optind, Options);

                if ( !(Options[OPTION_f] != SUCCESS && Options[OPTION_u] != SUCCESS && 
                    Options[OPTION_c] != SUCCESS && Options[OPTION_t] != SUCCESS) ){
                    // Restore original path names after sorting
                    for (int i = 0; i < path_count; i++) {
                        int index = linearSearch(argv, argc, paths[i].resolved_path);

                        if (index != -1) {
                            argv[index] = paths[i].original_path;
                        } else {
                            printf("Path not found:%s\n", paths[i].resolved_path);
                        }
                    }
                }

                // Process and list each directory or file
                int i = 0;
                for (i = optind; i < argc - 1; ++i) {
                    printf("%s:\n", argv[i]);
                    do_ls(argv[i]);
                    printf("\n\n");
                }

                // Prevent extra new-line for the last entry
                printf("'%s':\n", argv[i]);
                do_ls(argv[i]);
                printf("\n");

                if ( !(Options[OPTION_f] != SUCCESS && Options[OPTION_u] != SUCCESS && 
                    Options[OPTION_c] != SUCCESS && Options[OPTION_t] != SUCCESS) ){
                    // Clean up dynamically allocated memory
                    for (int i = 0; i < (argc - optind); i++) {
                        free(paths[i].original_path);
                    }
                }
            }
        }
    }
    return 0;
}


/*===================================  Functions Implementations ==============================*/
void CheckOnOptions(int num, char** command){
    int opt = 0 ;

    // Parse command-line options
    while ((opt = getopt(num, command, ":latucifd1")) != FAIL) {
        switch (opt) {
            case 'l':
                Options[OPTION_l] = SUCCESS;  // Set flag if -l option is provided
                break;
            case 'a':
                Options[OPTION_a] = SUCCESS;  // Set flag if -l option is provided
                break;
            case 't':
                Options[OPTION_t] = SUCCESS;  // Set flag if -l option is provided
                break;
            case 'u':
                Options[OPTION_u] = SUCCESS;  // Set flag if -l option is provided
                break;
            case 'c':
                Options[OPTION_c] = SUCCESS;  // Set flag if -l option is provided
                break;
            case 'i':
                Options[OPTION_i] = SUCCESS;  // Set flag if -l option is provided
                break;
            case 'f':
                Options[OPTION_f] = SUCCESS;  // Set flag if -l option is provided
                Options[OPTION_a] = SUCCESS;  // Set flag if -l option is provided
                break;
            case 'd':
                Options[OPTION_d] = SUCCESS;  // Set flag if -l option is provided
                break;
            case '1':
                Options[OPTION_1] = SUCCESS;  // Set flag if -l option is provided
                break;
            default:    
                printf("Unexpected case in switch()");  
                exit(FAIL);
        }
    }   
}


void do_ls(const char* dir) {
    DIR *dp;                         // Pointer to a directory stream
    char* Elements[MAX_ELEMENTS] 
                       = {NULL};     // Array to store filenames
    size_t ElementNumber = 0;        // Counter for the number of elements
    errno = 0;                       // Set errno to 0 before starting
    FirstEntry = FAIL ;
    Global_dir = (char* )dir ;

    if (Options[OPTION_d] == SUCCESS) {
        struct stat buf;

        // If `-d` is set, we just need to print the directory itself
        if (lstat(dir, &buf) == -1) {
            perror("lstat error");
            return;
        }

        process_fileHelper(buf,dir);
        
        // to make each element in one line only
        if ( (SUCCESS == Options[OPTION_1] || SUCCESS == Options[OPTION_l]))
            printf("\n");
    } else {
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
        if (Options[OPTION_f] != SUCCESS)
            // Check the conditions and apply the appropriate sorting function
            sortBuffer(Elements,ElementNumber,Options);

        // Process and print information for each file
        for (size_t i = 0; i < ElementNumber; ++i) {
            process_file(dir, Elements[i]);

            // to prevent printing double new-line in last case
            if ( (SUCCESS == Options[OPTION_1] || SUCCESS == Options[OPTION_l]) && (i != ElementNumber-1) )
                printf("\n");
        }

        // Cleanup
        cleanup(Elements, ElementNumber, dp);
    }
}



// Function to perform a linear search on paths array
int linearSearch(char* paths[], int size, const char *target) {
    for (int i = 0; i < size; ++i) {
        if (strcmp(paths[i], target) == 0) {
            return i; // Found the target at index i
        }
    }
    return -1; // Target not found
}



