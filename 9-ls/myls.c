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
static char* Global_dir = ""; // to fix lstat issue of reading files in directory

/*==============================  local functions ============================*/
static int cmpstringp(const void *p1, const void *p2);
static int cmpAcecssp(const void *p1, const void *p2);
static int cmpChangep(const void *p1, const void *p2);
static int cmpModifip(const void *p1, const void *p2);

/*=================================  main ====================================*/
int main (int argc, char* argv[]){
    int opt;

    if (argc == 1){
        // in case of enetring ./myls only will operate on the current working space
       do_ls(".");
       printf("\n");
    }else{
        // Parse command-line options
        CheckOnOptions(argc,argv);

        if (optind >= argc) {
            // there is no arguments
            do_ls(".");
            printf("\n");
        }else if (optind == argc-1){
            // there is one argument will pass it
            do_ls(argv[optind]);
            printf("\n");
        }else{
            // will work on directories names only
            if (Options[OPTION_d] == SUCESS){
                for (int i = optind; i < argc; ++i) {
                    do_ls(argv[i]);
                }
                printf("\n");
            }else {
                PathInfo paths[MAX_ELEMENTS];
                int path_count = 0;

                for (int i = optind; i < argc; i++) {
                    paths[path_count].original_path = strdup(argv[i]);
                    if (realpath(argv[i], paths[path_count].resolved_path) == NULL) {
                        perror("realpath error");
                    }
                    argv[i]=strdup(paths[path_count].resolved_path);
                    path_count++;
                }

                // Check the conditions and apply the appropriate sorting function
                sortBuffer(&argv[optind],argc-optind,Options);


                // will operate on our each input
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


void do_ls(const char* dir) {
    DIR *dp;                         // Pointer to a directory stream
    char* Elements[MAX_ELEMENTS] 
                       = {NULL};     // Array to store filenames
    size_t ElementNumber = 0;        // Counter for the number of elements
    errno = 0;                       // Set errno to 0 before starting
    FirstEntry = FAIL ;
    Global_dir = (char* )dir ;

    if (Options[OPTION_d] == SUCESS) {
        struct stat buf;

        // If `-d` is set, we just need to print the directory itself
        if (lstat(dir, &buf) == -1) {
            perror("lstat error");
            return;
        }

        process_fileHelper(buf,dir);
        
        // to make each element in one line only
        if ( (SUCESS == Options[OPTION_1] || SUCESS == Options[OPTION_l]))
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
        if (Options[OPTION_f] != SUCESS)
            // Check the conditions and apply the appropriate sorting function
            sortBuffer(Elements,ElementNumber,Options);

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


static int cmpstringp(const void *p1, const void *p2)
{
    return strcmp(*(const char **) p1, *(const char **) p2);
}

char* getAbsolutePath(const char* inputPath) {
    // Allocate memory for the absolute path
    char *absolutePath = malloc(MAX_PATH_LENGTH);
    char path[MAX_PATH_LENGTH];

    // Construct the full path of the file
    if (snprintf(path, sizeof(path), "%s/%s", Global_dir,  inputPath) >= sizeof(path)) {
        fprintf(stderr, "Path truncated: %s/%s\n", Global_dir, inputPath);
        return NULL;
    }

    if (absolutePath == NULL) {
        perror("malloc error");
        return NULL;
    }

    // Use realpath to resolve the inputPath and return the absolute path
    if (realpath(path, absolutePath) == NULL) {
        printf("realpath error : %s\n",absolutePath);
        free(absolutePath);
        return NULL;
    }

    return absolutePath;
}

char cmpHelper(const void *p1, const void *p2,struct stat* buf1,struct stat * buf2){
    const char *file1 = *(const char **) p1;
    const char *file2 = *(const char **) p2;
    char* path1 = getAbsolutePath(file1);
    char* path2 = getAbsolutePath(file2);

    if (path1 == NULL || path2 == NULL){
        printf("Error in path1 or path2");
        exit(FAIL);
    }

    /** Get file stats for each file */
    if (lstat(path1, buf1) < 0) {
        printf("stat error for file1: %s\n", file1);
        free(path1);
        free(path2);
        return FAIL;
    }

    // Construct the full path of the file
    if (lstat(path2, buf2) < 0) {
        printf("stat error for file2: %s\n", file2);
        free(path1);
        free(path2);
        return FAIL; 
    }

    free(path1);
    free(path2);

    return SUCESS ;
}

static int cmpAcecssp(const void *p1, const void *p2){
    struct stat buf1, buf2;
    char status = cmpHelper(p1,p2,&buf1,&buf2);

    if (status == FAIL){
        return 0 ;
    }

    /** Compare change times (st_atime) */
    if (buf1.st_atime > buf2.st_atime)
        return -1;  
    else if (buf1.st_atime < buf2.st_atime)
        return 1; 
    else
        return 0;   /** change times are the same */
}

static int cmpChangep(const void *p1, const void *p2){
    struct stat buf1, buf2;
    char status = cmpHelper(p1,p2,&buf1,&buf2);

    if (status == FAIL){
        return 0 ;
    }

    /** Compare change times (st_atime) */
    if (buf1.st_ctime > buf2.st_ctime)
        return -1;  
    else if (buf1.st_ctime < buf2.st_ctime)
        return 1; 
    else
        return 0;   /** change times are the same */
}

static int cmpModifip(const void *p1, const void *p2){
    struct stat buf1, buf2;
    char status = cmpHelper(p1,p2,&buf1,&buf2);

    if (status == FAIL){
        return 0 ;
    }

    /** Compare change times (st_atime) */
    if (buf1.st_mtime > buf2.st_mtime)
        return -1;  
    else if (buf1.st_mtime < buf2.st_mtime)
        return 1; 
    else
        return 0;   /** change times are the same */
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

    process_fileHelper(buf,filename);
}

void process_fileHelper(struct stat buf, const char* filename){
    // Print inode number if `-i` is set
    if (SUCESS == Options[OPTION_i]){
        printf("%ld ", buf.st_ino);
    } 
    
    // Print long listing format if `-l` is set
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


void sortBuffer(char **argv, int counter, char Options[]){
    // Check the conditions and apply the appropriate sorting function
    if (Options[OPTION_f] != SUCESS && Options[OPTION_u] != SUCESS && 
        Options[OPTION_c] != SUCESS && Options[OPTION_t] != SUCESS) {
        // Sort by name
        qsort(argv, counter, sizeof(char *), cmpstringp);
    } 
    else if (Options[OPTION_f] != SUCESS && Options[OPTION_u] == SUCESS && 
             Options[OPTION_c] != SUCESS && Options[OPTION_t] != SUCESS) {
        // Sort by access time
        qsort(argv, counter, sizeof(char *), cmpAcecssp);
    } 
    else if (Options[OPTION_f] != SUCESS && Options[OPTION_u] != SUCESS && 
             Options[OPTION_c] == SUCESS && Options[OPTION_t] != SUCESS) {
        // Sort by change time
        qsort(argv, counter, sizeof(char *), cmpChangep);
    } 
    else if (Options[OPTION_f] != SUCESS && Options[OPTION_u] != SUCESS && 
             Options[OPTION_c] != SUCESS && Options[OPTION_t] == SUCESS) {
        // Sort by modification time
        qsort(argv, counter, sizeof(char *), cmpModifip);
    }
}
