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
static char GlobalAppendFlag      = CLEARED     ;
static char GlobalMoveForcedFlag  = CLEARED     ;
static char GlobalMoveOperation   = MOVE_FAILED ;

/*===========================  Functions Implementations ======================*/
void Shellio_GetPath() {
    /* Buffer of path */
    uint8 cwd[MAX_PATH];  // Array to store the current working directory path, with a maximum size defined by PATH_MAX

    /* to get our working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Attempt to get the current working directory, storing it in cwd
        my_printf("Current working directory: %s\n", cwd);  // If successful, print the current working directory path
    } else {
        perror("getcwd() error");  // If unsuccessful, print an error message indicating the failure
    }
}

void Shellio_EchoInput(const char* Copy_Statment) {
    /* Check on passing parameter */
    if (Copy_Statment == NULL){
        my_printf ("Null passing parameter\n");
        return ;
    }

    /* get length of coming string */
    size_t Loc_Count = strlen (Copy_Statment) ;

    /* write operation*/
    ssize_t ret = write(STDOUT, Copy_Statment,Loc_Count);
    
    /* Check on return value of sucessed written bytes on screen */
    if (ret < 0 ) {  // Check if the input string is not NULL
        perror("Error in writing on screen :: \n");  // Print the input string followed by a newline
    }
    else if (ret < Loc_Count) {
        // Not all bytes were written; handle the partial write
        fprintf(stderr, "write_to_stdout: Partial write occurred. Expected %zd, wrote %zd\n", Loc_Count, ret);
    }
    else {
        write(STDOUT, "\n",1);
    }
}

void Shellio_CopyFile (const char* Copy_1st_Path,const char* Copy_2nd_Path ){
    /* Check if the source file is differnt from destination file */
    if (strcmp(Copy_1st_Path,Copy_2nd_Path) == SAME ) {
        my_printf("Error :: Source and Destination files are same \n");  
        return ; 
    }

    /* Try to open source */
    int FD_SrcFile = open(Copy_1st_Path, O_RDONLY);

    /* Terminate this operation if the source file does not exist */
    if (FD_SrcFile == FD_INVALID) {
        perror("Source file open() error");
        return;
    }

    int FD_DesFile = FD_INVALID ;

    // Determine how to open the destination file
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // Default permissions

    /* Determine how to open the destination file based on global flags */
    if (GlobalAppendFlag == SET){
        /* Open destination file in append mode, create it if it doesn't exist */
        FD_DesFile = open(Copy_2nd_Path, O_RDWR | O_CREAT | O_APPEND , mode);

        /* Terminate this operation if the destination file does not exist */
        if (FD_DesFile == FD_INVALID ) {
            perror("Destination file open() error");
            close(FD_SrcFile);
            return;
        }        
    }
    else if ( GlobalMoveOperation == MOVE_PASS ){
        /* Attempt to open destination file in read/write mode */
        FD_DesFile = open(Copy_2nd_Path, O_RDWR , mode);

        /* Open destination file in write mode, create it if it doesn't exist */
        if (FD_DesFile == FD_INVALID || GlobalMoveForcedFlag == SET){
            FD_DesFile = open(Copy_2nd_Path, O_WRONLY | O_CREAT | O_TRUNC, mode);

            /* Terminate this operation if the destination file does not exist */
            if (FD_DesFile == FD_INVALID ) {
                perror("Destination file open() error");
                close(FD_SrcFile);
                return;
            } 
        }
        else {
            my_printf ("Error :: Destination File is already existed\n");
            close(FD_SrcFile);
            return ;
        }
    }
    else {
        /* I will check on it in next situation */
        FD_DesFile = open(Copy_2nd_Path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    }

    /* Buffers used for handling file content */
    char SrcFileName[MAX_FILE_NAME];
    char ConcatenatedDesFile [MAX_PATH];

    /* If the destination file name was not provided or directory was incorrect */
    if (FD_DesFile == FD_INVALID ){
        /* Copy base name of source file to create destination file with the same name */
        strcpy (SrcFileName, basename(strdup(Copy_1st_Path) ) ) ;  // Use strdup to avoid modifying the original path
        snprintf(ConcatenatedDesFile, MAX_PATH, "%s/%s", Copy_2nd_Path, SrcFileName);

        /* Check if the source file is different from the destination file */
        if (strcmp(Copy_1st_Path,ConcatenatedDesFile) == SAME ) {
            my_printf("Error :: Source and Destination files are same \n");  
            return ; 
        }

        /* Re-open the destination file with the proper mode */
        if (GlobalAppendFlag == SET){
            /* Open destination file in append mode, create it if it doesn't exist */
            FD_DesFile = open(Copy_2nd_Path, O_RDWR | O_CREAT | O_APPEND , mode);

            /* Terminate this operation if the destination file does not exist */
            if (FD_DesFile == FD_INVALID ) {
                perror("Destination file open() error");
                close(FD_SrcFile);
                return;
            }         
        }
        else {
            FD_DesFile = open(ConcatenatedDesFile, O_WRONLY | O_CREAT | O_TRUNC, mode);

            /* Terminate this operation if the destination file does not exist */
            if (FD_DesFile == FD_INVALID ) {
                perror("Destination file open() error");
                close(FD_SrcFile);
                return;
            }        
        }

        /* Terminate operation if the destination file could not be created */
        if (FD_DesFile == FD_INVALID ){
            my_printf("Given path of directory isn't correct\n");
            perror("Destination fopen() error");
            close(FD_SrcFile);
            return;
        }

    }

    /* Buffer for reading file content */   
    uint8 Buffer[MAX_COPIED_CONTENT];

    /* Read content from the source file */
    ssize_t Read_Size = read(FD_SrcFile, Buffer, sizeof(Buffer));

    /* Write content to the destination file */
    while ( Read_Size > 0 ){
        /* write operation*/
        ssize_t Write_Size = write(FD_DesFile, Buffer,Read_Size);
        
        /* Check on return value of sucessed written bytes in file */
        if (Write_Size < 0 ) {  // Check if the input string is not NULL
            perror("Error in writing in file ");  // Print the input string followed by a newline
            close(FD_SrcFile);
            close(FD_DesFile);
            break ;
        }
        else if (Write_Size < Read_Size) {
            // Not all bytes were written; handle the partial write
            fprintf(stderr, "write_to_stdout: Partial write occurred. Expected %zd, wrote %zd\n", Read_Size, Write_Size);
        }
        else {
            write(STDOUT, "\n",1);
        }

        Read_Size = Read_Size = read(FD_SrcFile, Buffer, sizeof(Buffer));
    }

    /* Close the source and destination files */
    close(FD_SrcFile);
    close(FD_DesFile);

    /* Delete the source file if the move operation flag is set */
    if (GlobalMoveOperation == MOVE_PASS){
        if (remove(Copy_1st_Path) != 0) {
            perror("Error deleting source file\n");
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
        my_printf("uncorrect option%s\n", Copy_Option);
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
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("pwd :: Display the current working directory\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("cp  :: Copy file1 in path1 to to file2 in path2\n");
    Shellio_EchoInput("cp PathOffile1,PathOffile2\n");
    Shellio_EchoInput("cp PathOffile1,-a,PathOffile2\n");
    Shellio_EchoInput("-> case file2 name isn't determinted, will create one with file1 name\n");
    Shellio_EchoInput("-> case file2 name is given but unallocated, will create one with disred name\n");
    Shellio_EchoInput("-> use -a to append to copied file\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("mv  :: move file1 in path1 to to file2 in path2\n");
    Shellio_EchoInput("mv PathOffile1,PathOffile2\n");
    Shellio_EchoInput("mv PathOffile1,-f,PathOffile2\n");
    Shellio_EchoInput("-> case file2 name isn't determinted, will create one with file1 name\n");
    Shellio_EchoInput("-> case file2 name is given but unallocated, will create one with disred name\n");
    Shellio_EchoInput("-> use -f to overwrite on existed file\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("echo :: print on shellio termial\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("clear:: clears shellio termial\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("exit :: leave shellio terminal\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
    Shellio_EchoInput("-------------------------------------------------------------------------------\n");
}

void my_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}