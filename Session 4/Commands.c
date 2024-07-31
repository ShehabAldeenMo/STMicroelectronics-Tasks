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
static uint8* GetParsedPath(void);

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



/*===========================  Functions Implementations ======================*/
void Shellio_GetPath() {
    const char *delimiters = " 0";  // Delimiters are space and tab characters
    char *token = strtok(NULL, delimiters);  // Tokenize the input string

    /* Buffer for storing the current working directory */
    char cwd[MAX_PATH];  // Array to store the current working directory path, with a maximum size defined by MAX_PATH

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {
        printf("command not found\nEnter 'assist' to know Shellio commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    }

    /* Get the current working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Attempt to get the current working directory, storing it in cwd
        my_printf("Current working directory: %s\n", cwd);  // If successful, print the current working directory path
        pushProcessHistory(sharedString, SUCCESS);
    } else {
        perror("getcwd() error");  // If unsuccessful, print an error message indicating the failure
    }

    cleanSharedString();
}


void Shellio_EchoInput() {
    uint8 *delimiters = "0";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string

    /* if command equals null, means that there is no input with. So, We should print error message*/
    if (token == NULL){
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return ;
    }

    /* get length of coming string */
    size_t Loc_Count = strlen (token) ;

    /* write operation*/
    ssize_t ret = write(STDOUT, token,Loc_Count);
    
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

    cleanSharedString();
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

void Shellio_Help (){
    uint8 *delimiters = " 0";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string

    /* if command isn't equal null, means that there is another input with. So, We should print error message*/
    if (token != NULL){
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return ;
    } 

    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("pwd :: Display the current working directory\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("cp  :: Copy file1 in path1 to to file2 in path2\n");
    my_printf("cp PathOffile1,PathOffile2\n");
    my_printf("cp PathOffile1,-a,PathOffile2\n");
    my_printf("-> case file2 name isn't determinted, will create one with file1 name\n");
    my_printf("-> case file2 name is given but unallocated, will create one with disred name\n");
    my_printf("-> use -a to append to copied file\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("mv  :: move file1 in path1 to to file2 in path2\n");
    my_printf   ("mv PathOffile1,PathOffile2\n");
    my_printf("mv PathOffile1,-f,PathOffile2\n");
    my_printf("-> case file2 name isn't determinted, will create one with file1 name\n");
    my_printf("-> case file2 name is given but unallocated, will create one with disred name\n");
    my_printf("-> use -f to overwrite on existed file\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("echo :: print on shellio termial\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("clear:: clears shellio termial\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("exit :: leave shellio terminal\n");
    my_printf("-------------------------------------------------------------------------------\n");
    my_printf("-------------------------------------------------------------------------------\n");
    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
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
    uint8 *Buf ;
    static uint8 Option[3]; // tp prevent danglying pointer

    /* Set global pointer to the input of mv function or cp function */
    if (Copy_token == NULL){
        my_printf ("Error In Passing Pathes\n");
        return NULL ;
    }
    Ptr_GlobalGetParsingPath = Copy_token ;

    /* Get first path*/
    strcpy(Buf , GetParsedPath() );
    if (Buf == NULL){
        printf("Error In Passing Paths\n");
        return NULL;
    }
    strcpy ( Ptr_1st_Path ,Buf);   
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
    strcpy ( Buf , GetParsedPath() );
    if (Buf == NULL){
        printf("Error In Passing Paths\n");
        return NULL;
    }
    strcpy ( Ptr_2nd_Path , Buf );
    (*ptr_ArgCounter)++;  // counting parameters

    return Option ;
}

static uint8* GetParsedPath(void){
    /* buffer of stored path*/
    static uint8* Path = NULL ; // static to prevent dangling pointer

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

    /* return actual path */
    return Path;
}


uint8 Shellio_Exit (){
    uint8 *delimiters = " 0";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string

    /* if exit command isn't equal null, means that there is another input with. So, We should print error message*/
    if (token != NULL){
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        return (uint8)FAILED;
    } 
    /* print leaving message */
    printf("Good Bye\n");

    return (uint8)SUCCESS;
}

void Shellio_Clear (){
    uint8 *delimiters = " 0";                   // Delimiters are space, comma, period, and exclamation mark
    uint8 *token  = strtok(NULL, delimiters) ;  // to store each word into string

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

void Shellio_Copy (){
    uint8 Arguments[MAX_ARGUMENTS][MAX_CHARACHTERS_OF_ONE_ARGUMENTS] = {0} ;            // store commands
    uint8* token = strtok (NULL , "");
    uint8  ArgCounter = 0 ;
    uint8* Option = Shellio_ParsingPath(&ArgCounter,Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT] , token);

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



void Shellio_PrintEnvVar(uint8* copy_token){
    char *path_env = getenv(copy_token);

    if (path_env == NULL) {
        printf("This variable isn't existed :: %s\n",copy_token);
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
    }
    else {
        printf("%s = %s\n",copy_token,path_env);
        pushProcessHistory(sharedString, SUCCESS);
        cleanSharedString();
    }
}
void Shellio_PrintEnv(){
    char **env = environ;

    printf("-------------------------------------------------------------------------\n");
    while (*env) {
        printf("%s\n", *env);
        env++;
    }
    printf("-------------------------------------------------------------------------\n");

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
}


void Shellio_TypeCommand(){
    uint8* token = strtok (NULL," ");

    if (strcmp (token,"leave") == EQUALED)
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp (token,"cls") == CLEARED)
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp (token,"path") == PWD_PASS )
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp (token,"display") == ECHO_PASS )
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp (token,"assist") == HELP_PASS)
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp (token,"clone") == COPY_PASS )
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp (token,"shift") == MV_PASS )
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp(token, "cd") == CD_PASS) 
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp(token, "type") == TYPE_PASS) 
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp(token, "envir") == ENV_PASS)
        printf("It is an built-in command :: %s\n",token);
    else if (strcmp(token, "phist") == EXIT)
        printf("It is an built-in command :: %s\n",token);
    else {        
        uint8 status = SearchOnCommand (token);
        if (status == SUCCESS){
            printf("It is an external command :: %s\n",token);
        }
        else 
            printf("Undefined command :: %s\n",token);
    }

    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
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
    printf("Executing command:\n");
    for (int i = 0; i < argcounter; i++) {
        printf("Arg %d: %s\n", i, args[i]);
    }

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

void Shellio_ChangeDir(){
    const char *delimiters = "0";  // Delimiters are space and tab characters
    char *token = strtok(NULL, delimiters);  // Tokenize the input string

    /* Check if there is any additional input after the 'pwd' command */
    if (token == NULL) {
        printf("command not found\nEnter 'assist' to know Shellio commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    }

    /* Get the current working directory */
    if (chdir(token) == EXIST) {  // Attempt to get the current working directory, storing it in cwd
        pushProcessHistory(sharedString, SUCCESS);
    } else {
        pushProcessHistory(sharedString, FAILED);
        perror("cd error::");  // If unsuccessful, print an error message indicating the failure
    }

    cleanSharedString();
}


void Shellio_Phist(){
    printf("Process History:\n");
    for (int i = processCounter-1 ; i >= 0 ; i--) {
        printf("%d\tCommand: %s\tStatus: %d\n",i, PtrProcessHistory[i]->command, PtrProcessHistory[i]->status);
    }
    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
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