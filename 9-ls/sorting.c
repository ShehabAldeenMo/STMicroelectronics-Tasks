/*============================================================================
 * @file name      : 
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



/*==============================  local functions ============================*/
static int cmpstringp(const void *p1, const void *p2);
static int cmpAcecssp(const void *p1, const void *p2);
static int cmpChangep(const void *p1, const void *p2);
static int cmpModifip(const void *p1, const void *p2);


void sortBuffer(char **argv, int counter, char Options[]){
    // Check the conditions and apply the appropriate sorting function
    if (Options[OPTION_f] != SUCCESS && Options[OPTION_u] != SUCCESS && 
        Options[OPTION_c] != SUCCESS && Options[OPTION_t] != SUCCESS) {
        // Sort by name
        qsort(argv, counter, sizeof(char *), cmpstringp);
    } 
    else if (Options[OPTION_f] != SUCCESS && Options[OPTION_u] == SUCCESS && 
             Options[OPTION_c] != SUCCESS && Options[OPTION_t] != SUCCESS) {
        // Sort by access time
        qsort(argv, counter, sizeof(char *), cmpAcecssp);
    } 
    else if (Options[OPTION_f] != SUCCESS && Options[OPTION_u] != SUCCESS && 
             Options[OPTION_c] == SUCCESS && Options[OPTION_t] != SUCCESS) {
        // Sort by change time
        qsort(argv, counter, sizeof(char *), cmpChangep);
    } 
    else if (Options[OPTION_f] != SUCCESS && Options[OPTION_u] != SUCCESS && 
             Options[OPTION_c] != SUCCESS && Options[OPTION_t] == SUCCESS) {
        // Sort by modification time
        qsort(argv, counter, sizeof(char *), cmpModifip);
    }
}

static int cmpstringp(const void *p1, const void *p2)
{
    const char *file1 = *(const char **) p1;
    const char *file2 = *(const char **) p2;
    return strcmp(file1, file2);
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

    return SUCCESS ;
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
