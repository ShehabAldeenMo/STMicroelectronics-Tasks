/*============================================================================
 * @file name      : Commands.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkdIn         : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * Shellio is a custom command-line shell that allows users to interact with a 
 * terminal interface. It provides a basic environment for executing and processing 
 * commands. Supported commands include pwd, cp, mv, exit, clear, and echo. 
 * This file implements the core functionality and API for the Shellio commands.
 ==============================================================================
*/

/*===================================  Includes ===============================*/
#include "Commands.h"


/*===========================  Local File Variables ===========================*/
/* A flag indicating whether the append mode is set for file operations.
 * CLEARED indicates the flag is not set, while a different value indicates it is set.*/
static char GlobalAppendFlag           = CLEARED     ;

/* A flag indicating whether the move operation should force overwrite.
 * CLEARED indicates the flag is not set, while a different value indicates it is set.*/ 
static char GlobalMoveForcedFlag       = CLEARED     ;

/* A flag indicating the current status of a move operation.
 * MOVE_FAILED indicates the operation has not started or failed, while a different value indicates a successful operation.*/
static char GlobalMoveOperation        = MOVE_FAILED ;

/* A global pointer used to track the current position in the input string during parsing.
 * Points to the character in the string currently being processed.*/
static uint8* Ptr_GlobalGetParsingPath = NULL        ;

static uint8 IamChild = UNRAISED ;
static uint8 IamParent = UNRAISED ;
int localVarCount = 0;

/*
 * Name             : ProcessHistory Structure and Related Globals
 * Description      : Defines the structure for storing process history entries and
 *                    declares global variables related to process history management.
 *                    - ProcessHistory: A structure that holds command and status information.
 *                    - PtrProcessHistory: An array of pointers to ProcessHistory structures,
 *                      representing the global stack for storing process history.
 *                    - processCounter: A counter for tracking the number of elements in the stack.
 *                    - sharedString: A pointer to a string used for various operations in the shell.
 *                    - environ: A pointer to the environment variables.
 * */

// Define the structure for process history
typedef struct {
    uint8 *command;
    uint8 status;
} ProcessHistory;

typedef struct {
    char name[MAX_VAR_NAME];
    char value[MAX_VAR_VALUE];
} LocalVariable;

LocalVariable localVariables[MAX_VARS];


// Global stack and counter
ProcessHistory *PtrProcessHistory[MAX_STACK_SIZE];
uint8 processCounter = 0;

// pointer to string
uint8 *sharedString = NULL;

extern char **environ;


/*===================  Local File Functions Prototypes ========================*/
/*
 * Name             : GetParsedPath
 * Description      : Parses and returns the next path from the input string, moving the global parsing pointer forward.
 *                    This function extracts the path enclosed in double quotes (") and advances Ptr_GlobalGetParsingPath.
 * Parameter In/Out : None
 * Input            : None
 * Return           : uint8* - Returns a pointer to the next parsed path or NULL if no path is found.
 */
static uint8* GetParsedPath(uint8* command );

/*
 * Name             : my_printf
 * Description      : A custom printf function for formatted output in Shellio.
 *                    This function allows formatted output similar to the standard printf function.
 * Parameter In/Out : None
 * Input            : format (const char*) - A format string containing the text to be written, optionally with format specifiers.
 *                    ... - Additional arguments to be formatted according to the format specifiers in the format string.
 * Return           : void
 */
static void my_printf(const char *format, ...) ;



/*
 * Name             : cleanupProcessHistory
 * Description      : Cleans up the process history by freeing all allocated memory.
 *                    Iterates through the process history stack and frees each command string
 *                    and its associated memory.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function should be called when the process history is no longer needed
 *                    to prevent memory leaks.
 */
static void cleanupProcessHistory() ;

/*
 * Name             : SearchOnCommand
 * Description      : Searches for an external command in the directories specified in the PATH environment variable.
 *                    Checks if the command exists and is executable.
 * Input            : token - The command to search for.
 * Output           : None
 * Return           : SUCCESS if the command is found and executable, otherwise FAILED.
 * Notes            : The function uses the `access` system call to check the executability of the command.
 */
static uint8 SearchOnCommand (uint8 * token);



static uint8 searchCharacter(const uint8 *str, char character, uint8** Remaining);
static void redirect (uint8* path, int newFD);
static void fork_redirectionExec(uint8* path,int FD);
static uint8* ErrorFD_Path(uint8* path);
static uint8* GetRelativePath(uint8 path[] );
static void DisplaySeq(uint8* str);
static void GetPathSeq();
static void Help_Seq ();
static void TypeSeq(uint8* str);
static void FreeSeq ();
static void uptimeSeq();
static uint8 printLocalVariables(char* var);

/*===========================  Functions Implementations ======================*/
void Shellio_GetPath(uint8* command) {
    const char *delimiters = "";  // Delimiters are space and tab characters
    char *token = strtok(NULL, delimiters);  // Tokenize the input string
    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {

        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
        }
        else {
            printf("command not found\nEnter 'assist' to know Shellio commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return;   
        }
    }

    if (IamChild == RAISED){
        GetPathSeq();
        exit(100);
    }
    else if (IamParent != RAISED){
        GetPathSeq();
    }
    cleanSharedString();
    IamParent = UNRAISED ;
}

static void GetPathSeq(){
    /* Buffer for storing the current working directory */
    char cwd[MAX_PATH];  // Array to store the current working directory path, with a maximum size defined by MAX_PATH

    /* Get the current working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Attempt to get the current working directory, storing it in cwd
        my_printf("Current working directory: %s\n", cwd);  // If successful, print the current working directory path
        pushProcessHistory(sharedString, SUCCESS);
    } else {
        perror("getcwd() error");  // If unsuccessful, print an error message indicating the failure
    }
}

void Shellio_EchoInput(uint8* command) {
    uint8 *delimiters = "";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string
    /* For gdb script debugging*/
    uint8 NumOfoperand = 1 ;
    uint8* Operand[1];
    Operand[0] = token ;


    /* Check if there is any additional input after the 'pwd' command */
    if (token == NULL) {
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return ;
    }
    else {
        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
            token = strtok (token,"2>");
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
            token = strtok (token,">");
        }
    }

    if (IamChild == RAISED){
        DisplaySeq(token);
        exit(100);
    }
    else if (IamParent != RAISED){
        DisplaySeq(token);
    }

    cleanSharedString();
    IamParent = UNRAISED ;
}

static void DisplaySeq(uint8* str){
    /* get length of coming string */
    size_t Loc_Count = strlen (str) ;

    /* write operation*/
    ssize_t ret = write(STDOUT, str,Loc_Count);
    
    /* Check on return value of sucessed written bytes on screen */
    if (ret < 0 ) {  // Check if the input string is not NULL
        perror("Error in writing on screen :: \n");  // Print the input string followed by a newline
        pushProcessHistory(sharedString, FAILED);
    }
    else if (ret < Loc_Count) {
        // Not all bytes were written; handle the partial write
        fprintf(stderr, "write_to_stdout: Partial write occurred. Expected %zd, wrote %zd\n", Loc_Count, ret);
        pushProcessHistory(sharedString, FAILED);
    }
    else {
        write(STDOUT, "\n",1);
        pushProcessHistory(sharedString, SUCCESS);
    }
}

void Shellio_CopyFile(const char *sourcePath, const char *destPath) {
    /* Check if the source and destination files are the same */
    if (strcmp(sourcePath, destPath) == SAME ) {
        my_printf("Error :: Source and Destination files are same \n");  
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return ; 
    }

    /* To check that our file is already move to the diserd directory */
    uint8 Suc_Move = CLEARED ;

    /* Try to open source */
    int FD_SrcFile = open(sourcePath, O_RDONLY);

    /* Terminate this operation if the source file does not exist */
    if (FD_SrcFile == FD_INVALID) {
        perror("Source file open() error");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    }

    int FD_DesFile = FD_INVALID ;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // Default permissions

    /* Determine how to open the destination file based on global flags */
    if (GlobalAppendFlag == SET){
        /* Open destination file in append mode, create it if it doesn't exist */
        FD_DesFile = open(destPath, O_RDWR | O_CREAT | O_APPEND , mode);

        /* Terminate this operation if the destination file does not exist */
        if (FD_DesFile == FD_INVALID ) {
            perror("Destination file open() error");
            close(FD_SrcFile);
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return;
        }        
    }
    else if ( GlobalMoveOperation == MOVE_PASS ){
        /* Attempt to open destination file in read/write mode */
        FD_DesFile = open(destPath, O_RDWR , mode);

        /* Open destination file in write mode, create it if it doesn't exist */
        if (FD_DesFile == FD_INVALID || GlobalMoveForcedFlag == SET){
            FD_DesFile = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, mode);

            /* Terminate this operation if the destination file does not exist */
            if (FD_DesFile == FD_INVALID ) {
                perror("Destination file open() error");
                close(FD_SrcFile);
                pushProcessHistory(sharedString, FAILED);
                cleanSharedString();
                return;
            } 
        }
        else {
            my_printf ("Error :: Destination File is already existed\n");
            close(FD_SrcFile);
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return ;
        }
    }
    else {
        /* I will check on it in next situation */
        FD_DesFile = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, mode);
    }

    /* Buffers used for handling file content */
    char SrcFileName[MAX_FILE_NAME];
    char ConcatenatedDesFile [MAX_PATH];

    /* If the destination file name was not provided or directory was incorrect */
    if (FD_DesFile == FD_INVALID ){
        /* Copy base name of source file to create destination file with the same name */
        strcpy (SrcFileName, basename(strdup(sourcePath) ) ) ;  // Use strdup to avoid modifying the original path
        snprintf(ConcatenatedDesFile, MAX_PATH, "%s/%s", destPath, SrcFileName);

        /* Check if the source file is different from the destination file */
        if (strcmp(sourcePath,ConcatenatedDesFile) == SAME ) {
            my_printf("Error :: Source and Destination files are same \n");  
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return ; 
        }

        /* Re-open the destination file with the proper mode */
        if (GlobalAppendFlag == SET){
            /* Open destination file in append mode, create it if it doesn't exist */
            FD_DesFile = open(destPath, O_RDWR | O_CREAT | O_APPEND , mode);

            /* Terminate this operation if the destination file does not exist */
            if (FD_DesFile == FD_INVALID ) {
                perror("Destination file open() error");
                close(FD_SrcFile);
                pushProcessHistory(sharedString, FAILED);
                cleanSharedString();
                return;
            }         
        }
        else {
            FD_DesFile = open(ConcatenatedDesFile, O_WRONLY | O_CREAT | O_TRUNC, mode);

            /* Terminate this operation if the destination file does not exist */
            if (FD_DesFile == FD_INVALID ) {
                perror("Destination file open() error");
                close(FD_SrcFile);
                pushProcessHistory(sharedString, FAILED);
                cleanSharedString();
                return;
            }        
        }

        /* Terminate operation if the destination file could not be created */
        if (FD_DesFile == FD_INVALID ){
            my_printf("Given path of directory isn't correct\n");
            perror("Destination fopen() error");
            close(FD_SrcFile);
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
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
            Suc_Move = CLEARED ;
            pushProcessHistory(sharedString, FAILED);
            break ;
        }
        else if (Write_Size < Read_Size) {
            // Not all bytes were written; handle the partial write
            fprintf(stderr, "write_to_stdout: Partial write occurred. Expected %zd, wrote %zd\n", Read_Size, Write_Size);
            Suc_Move = CLEARED ;
            pushProcessHistory(sharedString, FAILED);
            break ;
        }
        else {
            write(STDOUT, "\n",1);
            Suc_Move = SET ;
        }

        pushProcessHistory(sharedString, SUCCESS);

        /* Read file content */
        Read_Size = Read_Size = read(FD_SrcFile, Buffer, sizeof(Buffer));
    }

    /* Close the source and destination files */
    close(FD_SrcFile);
    close(FD_DesFile);

    /* Delete the source file if the move operation flag is set */
    if (GlobalMoveOperation == MOVE_PASS && Suc_Move == SET ){
        if (unlink(sourcePath) == -1) {
            perror("Error deleting file");
            pushProcessHistory(sharedString, FAILED);
        }
    }

    /* Clear whole flags */
    GlobalAppendFlag     = CLEARED ;
    GlobalMoveOperation  = MOVE_FAILED ;
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
    cleanSharedString();
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

void Shellio_Help (uint8* command){
    uint8 *delimiters = "";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string
    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {
        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
        }
        else {
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return ;  
        }
    }

    if (IamChild == RAISED){
        Help_Seq();
        exit(100);
    }
    else if (IamParent != RAISED){
        Help_Seq();
    }

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED ;
}

static void Help_Seq (){
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("path :: Display the current working directory\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("clone  :: Copy file1 in path1 to to file2 in path2\n");
    my_printf("clone PathOffile1,PathOffile2\n");
    my_printf("clone PathOffile1,-a,PathOffile2\n");
    my_printf("-> case file2 name isn't determinted, will create one with file1 name\n");
    my_printf("-> case file2 name is given but unallocated, will create one with disred name\n");
    my_printf("-> use -a to append to copied file\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("shift  :: move file1 in path1 to to file2 in path2\n");
    my_printf   ("mv PathOffile1,PathOffile2\n");
    my_printf("shift PathOffile1,-f,PathOffile2\n");
    my_printf("-> case file2 name isn't determinted, will create one with file1 name\n");
    my_printf("-> case file2 name is given but unallocated, will create one with disred name\n");
    my_printf("-> use -f to overwrite on existed file\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("display :: print on shellio termial\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("cls:: clears shellio termial\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("leave :: leave shellio terminal\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("-------------------------------------------------------------------------------\n");
}


static void my_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

uint8* Shellio_ParsingPath (uint8* ptr_ArgCounter,uint8* Ptr_1st_Path,
                                    uint8* Ptr_2nd_Path, uint8* Copy_token){
    /* buffer the return from GetParsedPath() */
    uint8 Buf[MAX_PATH/2] ;
    static uint8 Option[3]; // tp prevent danglying pointer
    uint8 NumOfoperand = 3 ;
    uint8* command = "clone";

    /* Set global pointer to the input of mv function or cp function */
    if (Copy_token == NULL){
        my_printf ("Error In Passing Pathes\n");
        return NULL ;
    }
    Ptr_GlobalGetParsingPath = Copy_token ;

    /* Get first path*/
    strcpy(Buf , GetParsedPath("clone") );
    if (Buf == NULL){
        printf("Error In Passing Paths\n");
        return NULL;
    }

    /* handle relative paths */
    char keyword[] = "/home/shehabaldeen";
    char *found = strstr(Buf, keyword);
    if (found != NULL) {
        strcpy ( Ptr_1st_Path ,Buf); 
    } else {
        snprintf(Ptr_1st_Path, MAX_PATH, "%s/%s", GetPathWithoutToken (), Buf);
    }
    (*ptr_ArgCounter)++;  // counting parameters

    /* To discard the second double quote of given path(")*/
    if (*Ptr_GlobalGetParsingPath != '"'){
        printf("Error In Passing Paths");
        return NULL;
    }
    Ptr_GlobalGetParsingPath++;
    

    /* To discard spaces between two paths till reached to the second path*/
    while (*Ptr_GlobalGetParsingPath != '"' && *Ptr_GlobalGetParsingPath != '\0'){
        if ( *Ptr_GlobalGetParsingPath++ == '-' ){
            snprintf(Option, 3, "-%c",*Ptr_GlobalGetParsingPath);
            (*ptr_ArgCounter)++;  // counting parameters
        }
    }

    /* if you are exit loop because of '\0' */
    if (*Ptr_GlobalGetParsingPath != '"'){
        printf("Error In Passing Paths");
        return NULL;
    }

    /* Get second path*/
    strcpy ( Buf , GetParsedPath("clone") );
    if (Buf == NULL){
        printf("Error In Passing Paths\n");
        return NULL;
    }

    /* handle relative paths */
    found = strstr(Buf, keyword);
    if (found != NULL) {
        strcpy ( Ptr_2nd_Path ,Buf); 
    } else {
        snprintf(Ptr_2nd_Path, MAX_PATH, "%s/%s", GetPathWithoutToken (), Buf);
    }
    (*ptr_ArgCounter)++;  // counting parameters

    uint8* Operand[3] = {Ptr_1st_Path,Option,Ptr_2nd_Path} ;

    return Option ;
}

static uint8* GetParsedPath(uint8* command ){
    /* buffer of stored path*/
    static uint8* Path = NULL ; // static to prevent dangling pointer
    uint8 NumOfoperand = 1 ;
    uint8* Operand[1] ;

    if (Ptr_GlobalGetParsingPath == NULL) {
        my_printf("Error: Ptr_GlobalGetParsingPath is NULL\n");
        return NULL;
    }

    while ( (*Ptr_GlobalGetParsingPath) != '"' && (*Ptr_GlobalGetParsingPath) != '\0') {
        Ptr_GlobalGetParsingPath++;
    }

    /* if the coming charachter if (")*/
    if ((*Ptr_GlobalGetParsingPath) == '"'){
        Ptr_GlobalGetParsingPath++; // to discard (")
    }
    else if (*Ptr_GlobalGetParsingPath == '\0'){
        my_printf("Error In Passing Paths");
        return NULL ;
    }

    /* Allocate memory for path */
    Path = malloc(strlen(Ptr_GlobalGetParsingPath) + 1); // Allocate memory for the path

    /* Check if memory allocation succeeded */
    if (Path == NULL) {
        my_printf("Memory allocation failed\n");
        return NULL;
    }

    /* Copy characters until the next double quote or end of the string */
    int i = 0 ;
    while (*Ptr_GlobalGetParsingPath != '"' && *Ptr_GlobalGetParsingPath != '\0') {
        Path[i++] = *Ptr_GlobalGetParsingPath++;
    }

    /* Null terminator */
    Path[i] = '\0';

    Operand[0] =  Path ;

    /* return actual path */
    return Path;
}


uint8 Shellio_Exit (uint8* command){
    uint8 *delimiters = " 0";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string
    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};

    /* if exit command isn't equal null, means that there is another input with. So, We should print error message*/
    if (token != NULL){
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        return (uint8)FAILED;
    } 
    /* print leaving message */
    printf("Good Bye\n");

    return (uint8)SUCCESS;
}

void Shellio_Clear (uint8* command){
    uint8 *delimiters = " 0";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string
    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};

    /* if command isn't equal null, means that there is another input with. So, We should print error message*/
    if (token != NULL){
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        pushProcessHistory(sharedString, FAILED );
        cleanSharedString();
        return ;
    } 

    pushProcessHistory(sharedString, SUCCESS );
    cleanSharedString();

    /* clear screen */
    system("clear");
}

void Shellio_Copy (uint8* command){
    uint8 Arguments[MAX_ARGUMENTS][MAX_CHARACHTERS_OF_ONE_ARGUMENTS] = {0} ;            // store commands
    uint8* token = strtok (NULL , "");
    uint8  ArgCounter = 0 ;
    uint8* Option = Shellio_ParsingPath(&ArgCounter,Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT] , token);
    /* For gdb script debugging*/
    uint8 NumOfoperand = 3 ;
    uint8* Operand[3] = {Arguments[SECOND_ARGUMENT],Option,Arguments[THIRD_ARGUMENT]};

    /* copy function call */
    if (ArgCounter ==  ( MAX_ARGUMENTS -1 )  ){
        Shellio_CopyFile (Arguments[SECOND_ARGUMENT],Arguments[THIRD_ARGUMENT]);  
    }
    else if (ArgCounter == MAX_ARGUMENTS ){
        if (Option == NULL){
            printf ("Error In Passing Option\n");
        }

        // to lift our static flag
        char Status = Shellio_FileOption (Option);  
                    
        // to make our option 
        if (Status == VALID){
            Shellio_CopyFile (Arguments[SECOND_ARGUMENT],Arguments[THIRD_ARGUMENT]); 
        }
    }
    else {
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
    }
    cleanSharedString();
}


void Shellio_PrintEnv(uint8* command,uint8* token){
    char **env = environ;

    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {
        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
        }
        else {
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return ;  
        }
    }

    if (IamChild == RAISED){
        printf("-------------------------------------------------------------------------\n");
        while (*env) {
            printf("%s\n", *env);
            env++;
        }
        printf("-------------------------------------------------------------------------\n");
        exit(100);
    }
    else if (IamParent != RAISED){
        printf("-------------------------------------------------------------------------\n");
        while (*env) {
            printf("%s\n", *env);
            env++;
        }
        printf("-------------------------------------------------------------------------\n");
    }

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED ;
}


void Shellio_TypeCommand(uint8* command){
    uint8* token = strtok (NULL,"");

    /* For gdb script debugging*/
    uint8 NumOfoperand = 1 ;
    uint8* Operand[1];
    Operand[0] = token ;

    /* Check if there is any additional input after the 'pwd' command */
    if (token == NULL) {
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return ;
    }
    else {
        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
            token = strtok (token," 2>");
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
            token = strtok (token," >");
        }
    }

    if (IamChild == RAISED){
        TypeSeq(token);
        exit(100);
    }
    else if (IamParent != RAISED){
        TypeSeq(token);
    }    

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED ;
}


static void TypeSeq(uint8* str){
    if (strcmp (str,"leave") == EQUALED)
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp (str,"cls") == CLEARED)
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp (str,"path") == PWD_PASS )
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp (str,"display") == ECHO_PASS )
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp (str,"assist") == HELP_PASS)
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp (str,"clone") == COPY_PASS )
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp (str,"shift") == MV_PASS )
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp(str, "cd") == CD_PASS) 
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp(str, "type") == TYPE_PASS) 
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp(str, "envir") == ENV_PASS)
        printf("It is an built-in command :: %s\n",str);
    else if (strcmp(str, "phist") == EXIT)
        printf("It is an built-in command :: %s\n",str);
    else {        
        uint8 status = SearchOnCommand (str);
        if (status == SUCCESS){
            printf("It is an external command :: %s\n",str);
        }
        else 
            printf("Undefined command :: %s\n",str);
    }
}


// Function to execute external commands
void Shellio_ExecExternalCommands(uint8 *token) {
    int status;
    pid_t pid, wpid;
    char *args[MAX_ARGS];
    uint8 argcounter = 0;
    char *command = token;

    // New buffer for getting path into
    uint8 Cpstr[MAX_CHARACHTERS_OF_ONE_ARGUMENTS];
    strncpy(Cpstr, sharedString, MAX_CHARACHTERS_OF_ONE_ARGUMENTS);

    // Tokenize the input
    char *arg = Cpstr;
    char *end;
    while (*arg != '\0' && argcounter < MAX_ARGS - 1) {
        // Skip leading spaces
        while (*arg == ' ') arg++;

        // Handle paths enclosed in quotes
        if (*arg == '"') {
            arg++; // Skip the opening quote
            char path[MAX_PATH];
            size_t i = 0;

            while (*arg != '"' && *arg != '\0') {
                if (i < MAX_PATH - 1) {
                    path[i++] = *arg;
                }
                arg++;
            }
            path[i] = '\0'; // Null-terminate the path
            if (*arg == '"') arg++; // Skip the closing quote

            args[argcounter++] = strdup(path);
            printf("path %d= %s\n", argcounter - 1, path);
        } else {
            // Read until the next space or end of string
            end = strchr(arg, ' ');
            if (end == NULL) end = arg + strlen(arg);
            char temp[MAX_PATH];
            size_t len = end - arg;
            if (len < MAX_PATH - 1) {
                strncpy(temp, arg, len);
                temp[len] = '\0';
            } else {
                temp[MAX_PATH - 1] = '\0';
            }
            arg = end;
            args[argcounter++] = strdup(temp);
        }
    }
    args[argcounter] = NULL; // Null-terminate the argument array

    // Print arguments for debugging
    /*printf("Executing command:\n");
    for (int i = 0; i < argcounter; i++) {
        printf("Arg %d: %s\n", i, args[i]);
    }*/

    pid = fork();

    if (pid == -1) {
        perror("fork");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    } else if (pid == 0) {
        // Child process
        execvp(command, args);
        // If execvp returns, it must have failed
        perror("execvp");
        printf("Command not found\nEnter 'assist' to know Shellio commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        wpid = wait(&status);

        if (wpid == -1) {
            perror("wait");
            printf("Command failed\nEnter 'assist' to know Shellio commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            pushProcessHistory(sharedString, SUCCESS);
        } else {
            pushProcessHistory(sharedString, FAILED);
        }
    }

    cleanSharedString();
}

void Shellio_ChangeDir(uint8* command){
    const char *delimiters = "0";  // Delimiters are space and tab characters
    char *token = strtok(NULL, delimiters);  // Tokenize the input string

    /* For gdb script debugging*/
    uint8 NumOfoperand = 1 ;
    uint8* Operand[1];
    Operand[0] = token ;
    char keyword[] = "/home/shehabaldeen";

    uint8 AbsolutePath[MAX_PATH];
    strcpy(AbsolutePath ,token);

    /* Check if there is any additional input after the 'pwd' command */
    if (token == NULL) {
        printf("command not found\nEnter 'assist' to know Shellio commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    }

    /* handle relative paths */
    char *found = strstr(token, keyword);
    if (found == NULL) {
        found = strstr(token,"../");
        if (found != NULL){
            uint8* path = GetPathWithoutToken();
            uint8* BaseName = basename(path);
            uint8 len_path = strlen(path);
            uint8 len_BaseName = strlen(BaseName);
            uint8 len = (len_path-len_BaseName) ;

            // Copy the substring to the destination
            strncpy(AbsolutePath, path , len );
            AbsolutePath[len] = '\0';
            len++;
        }
        else {
            snprintf(AbsolutePath, MAX_PATH, "%s/%s", GetPathWithoutToken (), token);
        }
    } 

    Operand[0] = AbsolutePath ; // for debugging by gdb

    /* Get the current working directory */
    if (chdir(AbsolutePath) == EXIST) {  // Attempt to get the current working directory, storing it in cwd
        pushProcessHistory(sharedString, SUCCESS);
    } else {
        pushProcessHistory(sharedString, FAILED);
        perror("cd error::");  // If unsuccessful, print an error message indicating the failure
    }

    cleanSharedString();
}


void Shellio_Phist(uint8* command){
    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};
    uint8* token = strtok (NULL,"");

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {
        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
        }
        else {
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return ;  
        }
    }

    if (IamChild == RAISED){
        printf("Process History:\n");
        for (int i = processCounter-1 ; i >= 0 ; i--) {
            printf("%d\tCommand: %s\tStatus: %d\n",i, PtrProcessHistory[i]->command, PtrProcessHistory[i]->status);
        }
        exit(100);
    }
    else if (IamParent != RAISED){
        printf("Process History:\n");
        for (int i = processCounter-1 ; i >= 0 ; i--) {
            printf("%d\tCommand: %s\tStatus: %d\n",i, PtrProcessHistory[i]->command, PtrProcessHistory[i]->status);
        }
    }

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED ;
}


// Function to push into the stack
void pushProcessHistory(const uint8 *command, uint8 status) {
    // Allocate memory for the new element
    ProcessHistory *newEntry = (ProcessHistory *)malloc(sizeof(ProcessHistory));
    if (newEntry == NULL) {
        perror("Failed to allocate memory for new process history entry");
        exit(EXIT_FAILURE);
    }
    newEntry->command = strdup(command);  // Duplicate the command string
    if (newEntry->command == NULL) {
        perror("Failed to duplicate command string");
        free(newEntry);
        exit(EXIT_FAILURE);
    }
    newEntry->status = status;

    // If the stack is full, remove the last element
    if (processCounter >= MAX_STACK_SIZE) {
        free(PtrProcessHistory[MAX_STACK_SIZE - 1]->command);
        free(PtrProcessHistory[MAX_STACK_SIZE - 1]);
        processCounter--;
    }

    // Shift elements to the right
    for (int i = processCounter; i > 0; i--) {
        PtrProcessHistory[i] = PtrProcessHistory[i - 1];
    }

    // Insert the new entry at the front
    PtrProcessHistory[0] = newEntry;
    processCounter++;
}

// Cleanup function to free all allocated memory
static void cleanupProcessHistory() {
    for (int i = 0; i < processCounter; i++) {
        free(PtrProcessHistory[i]->command);
        free(PtrProcessHistory[i]);
    }
}

void setSharedString (const uint8 * str){
    sharedString = strdup(str);
}

void cleanSharedString(){
    free(sharedString);
}

static uint8 SearchOnCommand (uint8 * token){
    char *path_env = getenv("PATH");

    if (path_env == NULL) {
        return FAILED ; // PATH environment variable not set
    }

    char path_copy[MAX_PATH];
    strncpy(path_copy, path_env, sizeof(path_copy));

    char *dir = strtok(path_copy, ":");
    while (dir != NULL) {
        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, token);

        if (access(full_path, X_OK) == 0) {
            return SUCCESS; // Command found and is executable
        }

        dir = strtok(NULL, ":");
    }

    return FAILED; // Command not found or not executable
}

const char* getUserName() {
    struct passwd *pw;
    uid_t uid;

    uid = geteuid();
    pw = getpwuid(uid);
    if (pw) {
        return pw->pw_name;
    }
    return "";
}

void getHostName(char *hostname, size_t size) {
    if (gethostname(hostname, size) == -1) {
        perror("gethostname");
        strcpy(hostname, "unknown");
    }
}

void printPrompt() {
    const char *user = getUserName();
    char host[256];

    getHostName(host, sizeof(host));
    printf("%s%s%s", COLOR_BOLD_GREEN,user, COLOR_RESET);
    printf("@%s%s%s:", COLOR_BOLD_GREEN,host, COLOR_RESET);
}

uint8* GetPathWithoutToken(){
    // to avoid dangling pointer
    static char cwd[MAX_PATH]; 
    getcwd(cwd, sizeof(cwd));
    return cwd ;
}


void Shellio_Meminfo(uint8* command){
    uint8 *token  = strtok(NULL, "") ;  // to store each word into string
    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};


    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {
        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
        }
        else {
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return ;  
        }
    }

    if (IamChild == RAISED){
        FreeSeq();
        exit(100);
    }
    else if (IamParent != RAISED){
        FreeSeq();
    }

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED ;
}



static void FreeSeq (){
    uint8 buffer[1024];
    ssize_t bytes_read;
    uint8 *line_start = buffer;
    uint8 *line_end;
    int FD ;

    FD = open("/proc/meminfo", O_RDONLY);
    if (FD < 0) {
        perror("open");
        return ;
    }

    unsigned long total_ram ,free_ram, used_ram, total_swap, free_swap, used_swap ;

    while ( (bytes_read = read(FD, buffer, sizeof(buffer) - 1)) > 0 ) {
        buffer[bytes_read] = '\0';
        uint8 *ptr = line_start;

        while (( line_end = strchr(ptr, '\n')) != NULL) {
            *line_end = '\0'; // Null-terminate the line  

            // Parse each line
            sscanf(ptr, "MemTotal: %lu kB", &total_ram);
            sscanf(ptr, "MemFree: %lu kB", &free_ram);
            sscanf(ptr, "SwapTotal: %lu kB", &total_swap);
            sscanf(ptr, "SwapFree: %lu kB", &free_swap);

            ptr = line_end + 1;
        }  
    }

    close(FD);
    
    used_ram = total_ram - free_ram ;
    used_swap = total_swap - free_swap ;
        
    printf("             total                used                  free \n");
    printf("Mem        %lu             %lu                %lu\n",total_ram,used_ram,free_ram);
    printf("Swap           %lu                    %lu                     %lu\n",total_swap,used_swap,free_swap);

}


void Shellio_uptime(uint8* command) {
    /* For gdb script debugging*/
    uint8 NumOfoperand = 0 ;
    uint8* Operand[1] = {NULL};

    uint8 *token  = strtok(NULL, "") ;  // to store each word 

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {
        uint8* path ; 
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
        }
        else if ( searchCharacter(token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
        }
        else {
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return ;  
        }
    }

    if (IamChild == RAISED){
        uptimeSeq();
        exit(100);
    }
    else if (IamParent != RAISED){
        uptimeSeq();
    }

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED ;
}

static void uptimeSeq(){
    char buffer[100];

    ssize_t bytes_read;
    int FD;
    char *uptime, *idletime;

    FD = open("/proc/uptime", O_RDONLY);
    if (FD < 0) {
        perror("open");
        return;
    }

    bytes_read = read(FD, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(FD);
        return;
    }
    buffer[bytes_read] = '\0';  // Null-terminate the string

    // Close the file descriptor
    close(FD);

    // Tokenize the buffer
    uptime = strtok(buffer, " ");
    idletime = strtok(NULL, " ");

    uint8 len = strlen(idletime);
    idletime[len-1]='\0';

    if (uptime != NULL && idletime != NULL) {
        printf("The uptime : %s seconds\n", uptime);
        printf("The idletime : %s seconds\n", idletime);
    } else {
        printf("Error parsing uptime data.\n");
    }

}

static uint8 searchCharacter(const uint8 *str, char character, uint8** Remaining) {
    // Use strchr to find the first occurrence of the character
    *Remaining = (uint8*) strchr((const char*)str, character);

    if (*Remaining != NULL) {
        // If the character is found, skip the character itself
        (*Remaining)++;
        
        // Extract the part after the character
        *Remaining = (uint8*) strtok((char*) *Remaining, "");
        if (*Remaining) {
            Ptr_GlobalGetParsingPath = *Remaining ;
            *Remaining = GetParsedPath("path");
            *Remaining  = GetRelativePath(*Remaining );
            return VALID;
        }
        else {
            return INVALID ;
        }
    }

    return INVALID;
}

static uint8* ErrorFD_Path(uint8* path) {
    // Move the pointer 2 characters ahead
    path += 2;

    // Tokenize the string starting from path, ensure correct type casting
    path = (uint8*) strtok((char*) path, "");

    // Set the global parsing path
    Ptr_GlobalGetParsingPath = path;

    // Get the parsed path
    path = GetParsedPath("path");

    path = GetRelativePath(path);

    return path;
}

static void redirect (uint8* path, int newFD){

    // Determine how to open the destination file
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // Default permissions
    int outfd = open (path,O_CREAT| O_WRONLY , mode);

    if (outfd < 0){
        perror("Error In Opening File :: ");
        return ;
    }

    // close (newFD);
    int FD = dup2 (outfd,newFD);

    if (FD != newFD){
        perror("Error In reserving FD of stdout");
    } 

    close (outfd);
} 

static void fork_redirectionExec(uint8* path,int FD){
    int retPid = fork();

    if (retPid > 0){  
        pushProcessHistory(sharedString, SUCCESS);  
        IamParent = RAISED ;            
	    int status =0;	
	    wait(&status);
	}
	else if (retPid == 0){
        redirect (path,FD);
        IamChild = RAISED ;
	}
	else{
	    perror("fork");
	}
}


static uint8* GetRelativePath(uint8 path[]){
    uint8* ResultPath = (uint8*) malloc(MAX_PATH);
    char keyword[] = "/home/shehabaldeen";
    char *found = strstr((char*)path, keyword);

    if (found == NULL) {
        snprintf((char*)ResultPath, MAX_PATH, "%s/%s", GetPathWithoutToken(), path);
    } else {
        strncpy((char*)ResultPath, (char*)path, MAX_PATH);
    }

    return ResultPath;
}






void Shellio_PrintEnvVar(uint8* command,uint8* copy_token){
    /* For gdb script debugging*/
    uint8 NumOfoperand = 1 ;
    uint8* Operand[1];
    Operand[0] = copy_token ;
    uint8 token[20] ;


    /* Check if there is any additional input after the 'pwd' command */
    if (copy_token != NULL) {
        uint8* path ; 
        char *found = strstr(copy_token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found);
            fork_redirectionExec(path,STDERR);
            strcpy (token ,strtok (copy_token," 2>") );
        }
        else if ( searchCharacter(copy_token,'>',&path) == VALID ) {
            fork_redirectionExec(path,STDOUT);
            strcpy (token ,strtok (copy_token," >") );
        }
    }

    if (IamChild == RAISED){
        char *path_env = getenv(token);
    
        if (path_env == NULL) {
            uint8 status = printLocalVariables(token);

            if (status == INVALID){
                printf("This variable isn't existed :: %s\n",token);
                pushProcessHistory(sharedString, FAILED);
            }
            else{
                pushProcessHistory(sharedString, SUCCESS);
            }
        }
        else {
            printf("%s = %s\n",token,path_env);
            pushProcessHistory(sharedString, SUCCESS);
        }
        exit(100);
    }
    else if (IamParent != RAISED){
        strcpy (token ,strtok (copy_token," ") );
        char *path_env = getenv(token);
    
        if (path_env == NULL) {
            uint8 status = printLocalVariables(token);

            if (status == INVALID){
                printf("This variable isn't existed :: %s\n",token);
                pushProcessHistory(sharedString, FAILED);
            }
            else{
                pushProcessHistory(sharedString, SUCCESS);
            }
        }
        else {
            printf("%s = %s\n",token,path_env);
            pushProcessHistory(sharedString, SUCCESS);
        }
    }

    cleanSharedString();
    IamParent = UNRAISED ;
}











void Shellio_setVariable(uint8* command) {
    if (command == NULL) {
        printf("Invalid command format.\n");
        return;
    }

    char* name = strtok(command, "=");
    char* value = strtok(NULL, "");

    if (name != NULL && value != NULL) {
        setLocalVariable(name, value);
        printf("Variable set: %s=%s\n", name, value);
    } else {
        printf("Invalid variable format. Use: name=value\n");
    }
}

void Shellio_allVar() {
    // Print local variables
    printf("Local Variables:\n");
    for (int i = 0; i < localVarCount; i++) {
        printf("%s=%s\n", localVariables[i].name, localVariables[i].value);
    }

    // Print environment variables
    printf("Environment Variables:\n");
    Shellio_PrintEnv("allVar",NULL);
}

void setLocalVariable(const char* name, const char* value) {
    for (int i = 0; i < localVarCount; i++) {
        if (strcmp(localVariables[i].name, name) == 0) {
            strncpy(localVariables[i].value, value, MAX_VAR_VALUE);
            return;
        }
    }
    
    if (localVarCount < MAX_VARS) {
        strncpy(localVariables[localVarCount].name, name, MAX_VAR_NAME);
        strncpy(localVariables[localVarCount].value, value, MAX_VAR_VALUE);
        localVarCount++;
    } else {
        printf("Maximum number of local variables reached.\n");
    }
}


static uint8 printLocalVariables(char* var) {
    for (int i = 0; i < localVarCount; ++i) {
        if (strcmp(localVariables[i].name, var) == 0) {
            printf("%s\n", localVariables[i].value);
            return VALID;
        }
    }
    return INVALID;
}

