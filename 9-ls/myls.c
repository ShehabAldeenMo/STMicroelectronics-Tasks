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


/*==============================   Variables    =============================*/
extern int errno;
static char Options[MAX_NUMBER_OPTIONS] = {FAIL};
static char FirstEntry = FAIL;

/*==============================  local functions ============================*/
static int cmpstringp(const void *p1, const void *p2);


/*=================================  main ====================================*/
int main (int argc, char* argv[]){
    int opt;

    if (argc == 1){
       do_ls(".");
       printf("\n");
    }else{
        // Parse command-line options
        CheckOnOptions(argc,argv);

        if (optind >= argc) {
            do_ls(".");
            printf("\n");
        }else if (optind == argc-1){
            do_ls(argv[optind]);
            printf("\n");
        }else{
            // Sort and process directory arguments
            if (Options[OPTION_f] != SUCESS)
                qsort(&argv[optind], argc - optind, sizeof(char *), cmpstringp);

            int i = 0 ;
            for (i = optind; i < argc-1; ++i) {
                printf("%s:\n", argv[i]);
                do_ls(argv[i]);
                printf("\n\n");
            }
            // to prevent printing double new-line in last case
            printf("'%s':\n",argv[i]);
            do_ls(argv[i]);
            printf("\n");
        }
    }
    return 0 ; 
}





/*===================================  Functions Implementations ==============================*/
void CheckOnOptions(int num, char** command){
    int opt = 0 ;

    // Parse command-line options
    while ((opt = getopt(num, command, ":latucifd1")) != FAIL) {
        switch (opt) {
            case 'l':
                Options[OPTION_l] = SUCESS;  // Set flag if -l option is provided
                break;
            case 'a':
                Options[OPTION_a] = SUCESS;  // Set flag if -l option is provided
                break;
            case 't':
                Options[OPTION_t] = SUCESS;  // Set flag if -l option is provided
                break;
            case 'u':
                Options[OPTION_u] = SUCESS;  // Set flag if -l option is provided
                break;
            case 'c':
                Options[OPTION_c] = SUCESS;  // Set flag if -l option is provided
                break;
            case 'i':
                Options[OPTION_i] = SUCESS;  // Set flag if -l option is provided
                break;
            case 'f':
                Options[OPTION_f] = SUCESS;  // Set flag if -l option is provided
                Options[OPTION_a] = SUCESS;  // Set flag if -l option is provided
                break;
            case 'd':
                Options[OPTION_d] = SUCESS;  // Set flag if -l option is provided
                break;
            case '1':
                Options[OPTION_1] = SUCESS;  // Set flag if -l option is provided
                break;
            default:    
                printf("Unexpected case in switch()");  
                exit(FAIL);
        }
    }   
}


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
    FirstEntry = FAIL ;

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
    if (Options[OPTION_f] != SUCESS)
        qsort(Elements, ElementNumber, sizeof(char *), cmpstringp);

    // Process and print information for each file
    for (size_t i = 0; i < ElementNumber; ++i) {
        process_file(dir, Elements[i]);

        // to prevent printing double new-line in last case
        if ( (SUCESS == Options[OPTION_1] || SUCESS == Options[OPTION_l]) && (i != ElementNumber-1) )
            printf("\n");
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
    if ((type &  00170000) == 0010000)  // Named pipe (FIFO)
        printf("%s%s%s\t", COLOR_PIPE, text, COLOR_RESET);
    else if ((type &  00170000) == 0020000)  // Character Special file
        printf("%s%s%s\t", COLOR_CHR, text, COLOR_RESET);
    else if ((type &  00170000) == 0040000){  // Directory
        printf("%s%s%s\t", COLOR_DIR, text, COLOR_RESET); 
    }
    else if ((type &  00170000) == 0060000)  // Block Special file
        printf("%s%s%s\t", COLOR_BLK, text, COLOR_RESET);
    else if ((type &  00170000) == 0100000) {  // Regular file
        // Check if the file is executable
        if ((type & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0)
            printf("%s%s%s\t", COLOR_EXEC, text, COLOR_RESET);
        else
            printf("%s%s%s\t", COLOR_REG, text, COLOR_RESET);
    }
    else if ((type &  00170000) == 0120000)  // Symbolic link
        printf("%s%s%s\t", COLOR_LINK, text, COLOR_RESET);
    else if ((type &  00170000) == 0140000)  // Socket
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
        if (entry->d_name[0] == '.' && Options [OPTION_a] != SUCESS) {
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

    if ( FirstEntry == FAIL && Options[OPTION_l] == SUCESS){
        printf("total %ld\n", buf.st_blocks/2*3);
        FirstEntry = SUCESS;
    }

    if (SUCESS == Options[OPTION_i]){
        printf("%ld ", buf.st_ino);
    } 
    
    if (SUCESS == Options[OPTION_l]){
        Print_L_OptionInfo(&buf);
    }

    // Print the file type with color coding
    if (Options[OPTION_f] == SUCESS)
        printf("%s\t", filename);
    else
        PrintColoredType(buf.st_mode, filename);
}

void cleanup(char* Elements[], size_t elementCount, DIR* dp) {
    for (size_t i = 0; i < elementCount; ++i) {
        free(Elements[i]); // Free allocated memory for each filename
    }
    closedir(dp); // Close the directory stream
}


void print_permissions(mode_t mode) {
    char permissions[11];

    // File type
    if (S_ISREG(mode)) permissions[0] = '-';  // Regular file
    else if (S_ISDIR(mode)) permissions[0] = 'd';  // Directory
    else if (S_ISLNK(mode)) permissions[0] = 'l';  // Symbolic link
    else if (S_ISCHR(mode)) permissions[0] = 'c';  // Character device
    else if (S_ISBLK(mode)) permissions[0] = 'b';  // Block device
    else if (S_ISFIFO(mode)) permissions[0] = 'p';  // FIFO/pipe
    else if (S_ISSOCK(mode)) permissions[0] = 's';  // Socket
    else permissions[0] = '?';

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

    printf("%s ", permissions);
}

void Print_L_OptionInfo(struct stat* buf){
    // Permissions
    print_permissions(buf->st_mode);

    // Number of hard links (2 characters wide, right-aligned)
    printf("%2ld ", (long) buf->st_nlink);

    // Owner name (allocate 8 characters, left-aligned)
    struct passwd* pwd = getpwuid(buf->st_uid);
    printf("%-8s ", pwd->pw_name);

    // Group name (allocate 8 characters, left-aligned)
    struct group* grp = getgrgid(buf->st_gid);
    printf("%-8s ", grp->gr_name);

    // File size (allocate 8 characters, right-aligned)
    printf("%8lld ", (long long) buf->st_size);

    // Last modified time (formatted as "Month Day HH:MM")
    char time_str[64];
    struct tm* timeinfo = localtime(&buf->st_mtime);
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
    printf("%s ", time_str);
}