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
void Shellio_GetPath() {
    /* Buffer of path */
    uint8 cwd[MAX_PATH];  // Array to store the current working directory path, with a maximum size defined by PATH_MAX

    /* to get our working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Attempt to get the current working directory, storing it in cwd
        printf("Current working directory: %s\n", cwd);  // If successful, print the current working directory path
    } else {
        perror("getcwd() error");  // If unsuccessful, print an error message indicating the failure
    }
}

void Shellio_EchoInput(const char* Copy_Statment) {
    if (Copy_Statment != NULL) {  // Check if the input string is not NULL
        printf("%s\n", Copy_Statment);  // Print the input string followed by a newline
    } else {
        printf("There is no input\n");  // Print an error message if the input string is NULL
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

    /* Terminate this operation if the source file does not exist */
    if (SrcFile == NULL ){
        perror("Src fopen() error");
        return;
    }

    /* Determine how to open the destination file based on global flags */
    if (GlobalAppendFlag == SET){
        /* Open destination file in append mode, create it if it doesn't exist */
        DesFile = fopen(Copy_2nd_Path,"a+"); 
    }
    else if ( GlobalMoveOperation == MOVE_PASS ){
        /* Attempt to open destination file in read/write mode */
        DesFile = fopen(Copy_2nd_Path,"r+b"); 

        if (DesFile == NULL || GlobalMoveForcedFlag == SET){
            /* Open destination file in write mode, create it if it doesn't exist */
            DesFile = fopen(Copy_2nd_Path,"wb"); 
        }
        else {
            printf ("Error :: Destination File is already existed\n");
            fclose(SrcFile);
            return ;
        }
    }
    else {
        /* Open destination file in write mode, create it if it doesn't exist */
        DesFile = fopen(Copy_2nd_Path,"wb"); 
    }

    /* Buffers used for handling file content */
    char SrcFileName[MAX_FILE_NAME];
    char ConcatenatedDesFile [MAX_PATH];

    /* If the destination file name was not provided or directory was incorrect */
    if (DesFile == NULL ){
        /* Copy base name of source file to create destination file with the same name */
        strcpy (SrcFileName, basename(strdup(Copy_1st_Path) ) ) ;  // Use strdup to avoid modifying the original path
        snprintf(ConcatenatedDesFile, MAX_PATH, "%s/%s", Copy_2nd_Path, SrcFileName);

        /* Check if the source file is different from the destination file */
        if (strcmp(Copy_1st_Path,ConcatenatedDesFile) == SAME ) {
            printf("Error :: Source and Destination files are same \n");  
            return ; 
        }

        /* Re-open the destination file with the proper mode */
        if (GlobalAppendFlag == SET){
            DesFile = fopen(ConcatenatedDesFile,"a+"); // Append mode
        }
        else {
            DesFile = fopen(ConcatenatedDesFile,"wb"); // Write mode     
        }

        /* Terminate operation if the destination file could not be created */
        if (DesFile == NULL ){
            printf("Given path of directory isn't correct\n");
            perror("Destination fopen() error");
            return;
        }

    }

    /* Buffer for reading file content */   
    uint8 Buffer[MAX_COPIED_CONTENT];

    /* Read content from the source file */
    size_t ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;

    /* Write content to the destination file */
    while ( ret_Size > 0 ){
        fwrite(Buffer, 1, ret_Size, DesFile);
        ret_Size = fread(Buffer, 1 , sizeof(Buffer), SrcFile) ;
    }

    /* Close the source and destination files */
    fclose(SrcFile);
    fclose(DesFile);

    /* Delete the source file if the move operation flag is set */
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


char Shellio_FileOption(const char* Copy_Option) {
    /* to return status of function */
    char Local_Status = INVALID ;

    /* Check on our option */
    if (strcmp(Copy_Option, "-a") == SAME  ) {
        /* Set flag to append to the destination file */
        GlobalAppendFlag = SET;
        Local_Status = VALID ;
    }
    else if (strcmp(Copy_Option, "-f") == SAME && GlobalMoveOperation == MOVE_PASS )  {
        /* Set flag to force move operation */
        GlobalMoveForcedFlag = SET;
        Local_Status = VALID ;
    }
    else {
        /* Print error if the option is not recognized */
        printf("uncorrect option%s\n", Copy_Option);
        Local_Status = INVALID ;
    }

    return Local_Status;
}

void Shellio_MoveFile(const char Copy_MoveFlag) {
    if (Copy_MoveFlag == MOVE_PASS) {
        /* Set flag to indicate move operation is allowed */
        GlobalMoveOperation = MOVE_PASS;
    }
    else {
        /* Set flag to indicate move operation is not allowed */
        GlobalMoveOperation = MOVE_FAILED;
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