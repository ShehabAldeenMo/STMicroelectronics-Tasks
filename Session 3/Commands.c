/*============================================================================
 * @file name      : Shellio.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkdIn         : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * Commands is a simple, custom command-line shell designed to interact with users through a terminal interface. 
 * It provides a basic environment for entering and processing commands. Few commands: pwd, cp, mv, exit, clear,
 * echo. It used to provide API to shellio.
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ===============================*/
#include "Commands.h"


/*===========================  Local File Variables ===========================*/
static char GlobalAppendFlag = CLEARED ;
static char GlobalMoveForcedFlag = CLEARED ;
static char GlobalMoveOperation = MOVE_FAILED ;

/*===========================  Functions Implementations ======================*/
void Shellio_GetPath(){
    /* Buffer of path */
    uint8 cwd[PATH_MAX];

    /* to get our working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

void Shellio_EchoInput(const char* Copy_Statment){
    if (Copy_Statment != NULL){
        printf("%s\n",Copy_Statment);
    }
    else {
        printf("There is no input\n");
    }
    
}

void Shellio_CopyFile (const char* Copy_1st_Path,const char* Copy_2nd_Path ){
    /* Check if the source file is differnt from destination file */
    if (strcmp(Copy_1st_Path,Copy_2nd_Path) == SAME ) {
        printf("Error :: Source and Destination files are same \n");  
        return ; 
    }

    /* Try to open source and destination files */
    FILE* SrcFile = fopen(Copy_1st_Path,"r");
    FILE* DesFile = NULL ;

    /* Terminate this operation if the source file isn't existed */
    if (SrcFile == NULL ){
        perror("Src fopen() error");
        return;
    }

    /* To check that the destination file attribute in append or overwrite mode */
    if (GlobalAppendFlag == SET){
        DesFile = fopen(Copy_2nd_Path,"a+"); // if the user give file name that didn't exist. "a+" is attribute to create this name or if tis already exist will append to it
    }
    else if ( GlobalMoveOperation == MOVE_PASS ){
        DesFile = fopen(Copy_2nd_Path,"r+b"); // if the user give file name that didn't exist. "wb" is attribute to create this name.

        if (DesFile == NULL || GlobalMoveForcedFlag == SET){
            DesFile = fopen(Copy_2nd_Path,"wb"); // if the user give file name that didn't exist. "wb" is attribute to create this name.
        }
        else {
            printf ("Error :: Destination File is already existed\n");
            fclose(SrcFile);
            return ;
        }
    }
    else {
        DesFile = fopen(Copy_2nd_Path,"wb"); // if the user give file name that didn't exist. "wb" is attribute to create this name. 
    }

    /* To use these buffers in creation destintion file with the same name of source */
    char SrcFileName[MAX_FILE_NAME];
    char ConcatenatedDesFile [PATH_MAX];

    /* If the destination file name didn't pass or directory is unfound */
    if (DesFile == NULL ){
        /* copy base name of source file to create destination file with the same name */
        strcpy (SrcFileName, basename(strdup(Copy_1st_Path) ) ) ;  // Use strdup to avoid modifying the original path
        snprintf(ConcatenatedDesFile, PATH_MAX, "%s/%s", Copy_2nd_Path, SrcFileName);

        /* Check if the source file is differnt from destination file */
        if (strcmp(Copy_1st_Path,ConcatenatedDesFile) == SAME ) {
            printf("Error :: Source and Destination files are same \n");  
            return ; 
        }

        /* To check that the destination file attribute in append or overwrite mode */
        if (GlobalAppendFlag == SET){
            DesFile = fopen(ConcatenatedDesFile,"a+"); // if the user give file name that didn't exist. "a+" is attribute to create this name or if tis already exist will append to it
        }
        else {
            DesFile = fopen(ConcatenatedDesFile,"wb"); // if the user give file name that didn't exist. "wb" is attribute to create this name. 
        }

        /* Terminate this operation if the destination file name is created but the Concatenated path of directory isn't correct */
        if (DesFile == NULL ){
            printf("Given path of directory isn't correct\n");
            perror("Destination fopen() error");
            return;
        }

    }

    /* Buffering copied info from source file  */    
    uint8 Buffer[MAX_COPIED_CONTENT];

    /* read source file content */
    size_t ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;

    /* write into destination file */
    while ( ret_Size > 0 ){
        fwrite(Buffer, 1, ret_Size, DesFile);
        ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;
    }

    /* Close whole files */
    fclose(SrcFile);
    fclose(DesFile);

    // Delete the source file
    if (GlobalMoveOperation == MOVE_PASS){
        if (remove(Copy_1st_Path) != 0) {
            perror("Error deleting source file");
        }
    }


    /* Clear whole flags */
    GlobalAppendFlag = CLEARED ;
    GlobalMoveOperation = MOVE_FAILED ;
    GlobalMoveForcedFlag = CLEARED ;
}


void Shellio_FileOption (const char* Copy_Option ){
    /* Check on our option */
    if ( strcmp (Copy_Option,"-a") == SAME ){
        GlobalAppendFlag = SET ;
    }
    else if (strcmp (Copy_Option,"-f") == SAME ) {
        GlobalMoveForcedFlag = SET ;
    }
    else {
        printf("uncorrect option%s\n",Copy_Option);
    }
}

void Shellio_MoveFile     (const char Copy_MoveFlag ){
    if (Copy_MoveFlag == MOVE_PASS){
        GlobalMoveOperation = MOVE_PASS ;
    }
    else {
        GlobalMoveOperation = MOVE_FAILED ;
    }
}

void Shellio_Help (){
    printf("-------------------------------------------------------------------------------\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("pwd :: Display the current working directory\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("cp  :: Copy file1 in path1 to to file2 in path2\n");
    printf("cp PathOffile1,PathOffile2\n");
    printf("cp PathOffile1,-a,PathOffile2\n");
    printf("-> case file2 name isn't determinted, will create one with file1 name\n");
    printf("-> case file2 name is given but unallocated, will create one with disred name\n");
    printf("-> use -a to append to copied file\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("mv  :: move file1 in path1 to to file2 in path2\n");
    printf("mv PathOffile1,PathOffile2\n");
    printf("mv PathOffile1,-f,PathOffile2\n");
    printf("-> case file2 name isn't determinted, will create one with file1 name\n");
    printf("-> case file2 name is given but unallocated, will create one with disred name\n");
    printf("-> use -f to overwrite on existed file\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("echo :: print on shellio termial\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("clear:: clears shellio termial\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("exit :: leave shellio terminal\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("-------------------------------------------------------------------------------\n");
}