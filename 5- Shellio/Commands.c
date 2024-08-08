/*============================================================================
 * @file name      : Commands.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkdIn         : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * Shellio is a custom command-line shell designed to provide a simple interface 
 * for users to interact with the system. It allows the execution of both built-in 
 * commands (like path, clone, shift, leave, cls, display) and external programs. The shell 
 * supports local and environment variables, redirection, command history, and more.
 * The command execution is enhanced by custom handling of errors and outputs. The 
 * shell also includes features for displaying system information like memory usage 
 * and uptime.
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
 * This pointer points to the character in the string that is currently being processed.
 */
static uint8* Ptr_GlobalGetParsingPath = NULL;

/* Flag to indicate if the current process is a child process.
 * UNRAISED - Indicates that the process is not a child.
 * RAISED - Indicates that the process is a child.
 */
static uint8 IamChild = UNRAISED;

/* Flag to indicate if the current process is a parent process.
 * UNRAISED - Indicates that the process is not a parent.
 * RAISED - Indicates that the process is a parent.
 */
static uint8 IamParent = UNRAISED;

/* Counter for the number of local variables.
 * Keeps track of the total number of local variables currently in use.
 */
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

/*
 * Name             : LocalVariable Structure and Related Globals
 * Description      : Defines the structure for storing local variables and
 *                    declares global variables related to local variable management.
 *                    - LocalVariable: A structure that holds the name and value of a local variable.
 *                    - localVariables: An array of LocalVariable structures, 
 *                      representing the global list of local variables.
 *                    - localVarCount: A counter for tracking the number of local variables stored.
 * */

// Define the structure for local variables
typedef struct {
    char name[MAX_VAR_NAME];  /**< Name of the local variable */
    char value[MAX_VAR_VALUE]; /**< Value of the local variable */
} LocalVariable;


// Global variables
LocalVariable localVariables[MAX_VARS]; /**< Array to store local variables */

ProcessHistory *PtrProcessHistory[MAX_STACK_SIZE]; /**< Array to store process history entries */
uint8 processCounter = 0; /**< Counter for the number of process history entries */

uint8 *sharedString = NULL; /**< Pointer to a string used for various operations in the shell */

extern char **environ; /**< Pointer to the environment variables */

/*===================  Local File Functions Prototypes ========================*/
/*
 * Name             : GetParsedPath
 * Description      : Parses and returns the next path from the input string, moving the global parsing pointer forward.
 *                    This function extracts the path enclosed in double quotes (") and advances Ptr_GlobalGetParsingPath.
 * Parameter In/Out : None
 * Input            : command - The command string containing the variable name and value.
 * Return           : uint8* - Returns a pointer to the next parsed path or NULL if no path is found.
 */
static uint8* GetParsedPath(uint8* command);

/*
 * Name             : my_printf
 * Description      : A custom printf function for formatted output in Shellio.
 *                    This function allows formatted output similar to the standard printf function.
 * Parameter In/Out : None
 * Input            : format (const char*) - A format string containing the text to be written, optionally with format specifiers.
 *                    ... - Additional arguments to be formatted according to the format specifiers in the format string.
 * Return           : void
 */
static void my_printf(const char *format, ...);

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
static void cleanupProcessHistory();

/*
 * Name             : SearchOnCommand
 * Description      : Searches for an external command in the directories specified in the PATH environment variable.
 *                    Checks if the command exists and is executable.
 * Input            : token - The command to search for.
 * Output           : None
 * Return           : SUCCESS if the command is found and executable, otherwise FAILED.
 * Notes            : The function uses the `access` system call to check the executability of the command.
 */
static uint8 SearchOnCommand(uint8 *token);




/*
 * Name             : searchCharacter
 * Description      : Searches for the first occurrence of a specified character in a string.
 *                    If found, returns a pointer to the substring following the character.
 *                    Updates the global parsing pointer and extracts the relevant path information.
 * Input            : str - The string to search within.
 *                    character - The character to search for.
 *                    Remaining - Pointer to store the substring after the found character.
 * Output           : Remaining - Updated to point to the substring after the character, or NULL if not found.
 * Return           : VALID if the character is found and processed successfully, otherwise INVALID.
 */
static uint8 searchCharacter(const uint8 *str, char character, uint8** Remaining);

/*
 * Name             : redirect
 * Description      : Redirects the output to a specified file descriptor (FD) by opening the file at the given path.
 *                    The function handles file creation and setting the necessary permissions.
 * Input            : path - The path to the file where output should be redirected.
 *                    newFD - The file descriptor to be redirected (e.g., STDOUT or STDERR).
 * Output           : None
 * Return           : None
 * Notes            : The function uses `dup2` to redirect the output and `close` to close the original file descriptor.
 */
static void redirect(uint8* path, int newFD);

/*
 * Name             : fork_redirectionExec
 * Description      : Forks a new process to execute a command with output redirection.
 *                    The parent process waits for the child process to complete.
 * Input            : path - The path to the file for output redirection.
 *                    FD - The file descriptor to be redirected (e.g., STDOUT or STDERR).
 * Output           : None
 * Return           : None
 * Notes            : The function uses `fork` to create a new process and `redirect` to handle output redirection.
 */
static void fork_redirectionExec(uint8* path, int FD);

/*
 * Name             : ErrorFD_Path
 * Description      : Processes a string to extract a file path for error redirection.
 *                    The function adjusts the input path to point to the correct file for error output.
 * Input            : path - The input string containing the file path prefixed with "2>".
 * Output           : None
 * Return           : uint8* - Returns the adjusted file path for error redirection.
 * Notes            : The function updates the global parsing path and processes relative paths.
 */
static uint8* ErrorFD_Path(uint8* path);

/*
 * Name             : GetRelativePath
 * Description      : Computes the relative path from the provided absolute path.
 *                    If the path contains a specific keyword (e.g., "/home/shehabaldeen"),
 *                    it uses the path directly; otherwise, it constructs a relative path
 *                    based on the current working directory.
 * Input            : path - The absolute or relative path to be processed.
 * Output           : None
 * Return           : uint8* - Returns a pointer to the computed relative path.
 * Notes            : Allocates memory for the result path which should be freed by the caller.
 */
static uint8* GetRelativePath(uint8 path[]);

/*
 * Name             : DisplaySeq
 * Description      : Displays a sequence of information based on the input string.
 *                    This function processes the input string to generate formatted output,
 *                    possibly including command execution results or status messages.
 * Input            : str - The input string to be processed and displayed.
 * Output           : None
 * Return           : None
 * Notes            : The function may include formatted output and error handling as needed.
 */
static void DisplaySeq(uint8* str);

/*
 * Name             : GetPathSeq
 * Description      : Retrieves and displays the current working directory or path sequence.
 *                    This function outputs the current path or path-related information
 *                    to provide context or debugging information.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : This function may include system calls like `getcwd` to fetch the current directory.
 */
static void GetPathSeq();

/*
 * Name             : Help_Seq
 * Description      : Provides help or guidance on using commands and functionalities.
 *                    This function displays information on available commands, usage,
 *                    and other relevant details to assist users.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : The function may include a list of commands, descriptions, and usage examples.
 */
static void Help_Seq();


/*
 * Name             : TypeSeq
 * Description      : Processes and displays the content of the provided string.
 *                    This function interprets the input string, performs required operations,
 *                    and outputs the result, which may include formatted text or command output.
 * Input            : str - The input string to be processed and displayed.
 * Output           : None
 * Return           : None
 * Notes            : The function may involve text formatting and output to the console.
 */
static void TypeSeq(uint8* str);

/*
 * Name             : FreeSeq
 * Description      : Frees resources allocated for handling sequences.
 *                    This function cleans up and deallocates any memory or resources used
 *                    for managing sequences or related operations.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : Call this function to avoid memory leaks and ensure proper resource management.
 */
static void FreeSeq();

/*
 * Name             : uptimeSeq
 * Description      : Retrieves and displays the system's uptime and idle time.
 *                    This function reads uptime information from the system's `/proc/uptime`
 *                    file and prints both the total uptime and idle time to the console.
 * Input            : None
 * Output           : None
 * Return           : None
 * Notes            : Uses system file operations to access uptime information and may handle errors.
 */
static void uptimeSeq();

/*
 * Name             : printLocalVariables
 * Description      : Prints the value of a specified local variable.
 *                    This function searches for a local variable with the given name and
 *                    prints its value if found. If the variable does not exist, an error message is shown.
 * Input            : var - The name of the local variable to be printed.
 * Output           : None
 * Return           : uint8 - Returns VALID if the variable is found and printed, otherwise INVALID.
 * Notes            : This function is used for debugging or displaying variable values in the shell.
 */
static uint8 printLocalVariables(char* var);


/*===========================  Functions Implementations ======================*/
void Shellio_GetPath(uint8* command) {
    const char *delimiters = "";  // Delimiters for tokenizing are empty (i.e., no specific delimiters)
    char *token = strtok(NULL, delimiters);  // Tokenize the input string after the command

    /* For gdb script debugging: 
       Initialize debugging variables (not used in this function). */
    uint8 NumOfoperand = 0;
    uint8* Operand[1] = {NULL};

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {

        uint8* path; 
        char *found = strstr(token, "2>");  // Check if the token contains redirection to stderr
        if (found != NULL) {
            path = ErrorFD_Path(found);  // Get the path for stderr redirection
            fork_redirectionExec(path, STDERR);  // Execute with stderr redirection
        }
        else if (searchCharacter(token, '>', &path) == VALID) {  // Check if the token contains redirection to stdout
            fork_redirectionExec(path, STDOUT);  // Execute with stdout redirection
        }
        else {
            // If no valid redirection is found, print an error message
            printf("command not found\nEnter 'assist' to know Shellio commands\n");
            pushProcessHistory(sharedString, FAILED);  // Record the failure in process history
            cleanSharedString();  // Clean up the shared string
            return;   
        }
    }

    /* If running in child process, get the current path and exit */
    if (IamChild == RAISED) {
        GetPathSeq();  // Call function to get and print the current working directory
        exit(100);  // Exit with a specific status code
    }
    else if (IamParent != RAISED) {
        /* If running in parent process and not raised, get the current path */
        GetPathSeq();
    }

    /* Clean up and reset parent status */
    cleanSharedString();
    IamParent = UNRAISED;  // Reset the parent status
}


static void GetPathSeq() {
    /* Buffer for storing the current working directory */
    char cwd[MAX_PATH];  // Array to store the current working directory path; MAX_PATH defines its size

    /* Get the current working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Attempt to get the current working directory and store it in cwd
        my_printf("Current working directory: %s\n", cwd);  // Print the current working directory if successful
        pushProcessHistory(sharedString, SUCCESS);  // Record the success of the operation in process history
    } else {
        perror("getcwd() error");  // If the operation fails, print an error message
    }
}


void Shellio_EchoInput(uint8* command) {
    uint8 *delimiters = "";  // Delimiters are space, comma, period, and exclamation mark (though currently empty)
    uint8 *token = strtok(NULL, delimiters);  // Tokenize the input string to extract the argument for echo

    /* For gdb script debugging */
    uint8 NumOfoperand = 1;  // Number of operands (arguments) for debugging
    uint8* Operand[1];  // Array to hold the operand
    Operand[0] = token;  // Store the token (argument) in the operand array

    /* Check if there is any additional input after the 'echo' command */
    if (token == NULL) {
        printf("command not found\nEnter (assist) to know Shellio Commands\n");  // Print error if no token is found
        pushProcessHistory(sharedString, FAILED);  // Log the failure in process history
        cleanSharedString();  // Clean up the shared string
        return;
    } else {
        uint8* path;
        char *found = strstr(token, "2>");  // Check for stderr redirection
        if (found != NULL) {
            path = ErrorFD_Path(found);  // Get the path for error redirection
            fork_redirectionExec(path, STDERR);  // Redirect stderr to the specified path
            token = strtok(token, "2>");  // Remove the redirection part from token
        } else if (searchCharacter(token, '>', &path) == VALID) {
            fork_redirectionExec(path, STDOUT);  // Redirect stdout to the specified path
            token = strtok(token, ">");  // Remove the redirection part from token
        }
    }

    if (IamChild == RAISED) {
        DisplaySeq(token);  // Display the token if in child process
        exit(100);  // Exit child process
    } else if (IamParent != RAISED) {
        DisplaySeq(token);  // Display the token if in parent process
    }

    cleanSharedString();  // Clean up the shared string
    IamParent = UNRAISED;  // Reset parent status
}

static void DisplaySeq(uint8* str) {
    /* Get the length of the input string */
    size_t Loc_Count = strlen(str);  // Length of the string to be written

    /* Write operation */
    ssize_t ret = write(STDOUT, str, Loc_Count);  // Write the string to standard output
    
    /* Check on return value of written bytes */
    if (ret < 0) {  // Error in writing
        perror("Error in writing on screen :: \n");  // Print error message
        pushProcessHistory(sharedString, FAILED);  // Log the failure in process history
    } else if (ret < Loc_Count) {
        // Not all bytes were written; handle partial write
        fprintf(stderr, "write_to_stdout: Partial write occurred. Expected %zd, wrote %zd\n", Loc_Count, ret);
        pushProcessHistory(sharedString, FAILED);  // Log the failure in process history
    } else {
        write(STDOUT, "\n", 1);  // Write a newline character to standard output
        pushProcessHistory(sharedString, SUCCESS);  // Log the success in process history
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
    char Local_Status = INVALID;  // Status to return: INVALID by default

    /* Check if the provided option is "-a" for appending */
    if (strcmp(Copy_Option, "-a") == SAME) {
        GlobalAppendFlag = SET;  // Set global flag to append to the destination file
        Local_Status = VALID;    // Indicate that the option is valid
    }
    /* Check if the provided option is "-f" for forcing the move operation */
    else if (strcmp(Copy_Option, "-f") == SAME && GlobalMoveOperation == MOVE_PASS) {
        GlobalMoveForcedFlag = SET;  // Set global flag to force move operation
        Local_Status = VALID;        // Indicate that the option is valid
    }
    /* Handle unrecognized options */
    else {
        my_printf("Incorrect option: %s\n", Copy_Option);  // Print error message for unrecognized option
        Local_Status = INVALID;  // Indicate that the option is invalid
    }

    cleanSharedString();  // Clean shared string data
    return Local_Status;  // Return the status of the function
}

void Shellio_MoveFile(const char Copy_MoveFlag) {
    if (Copy_MoveFlag == MOVE_PASS) {
        GlobalMoveOperation = MOVE_PASS;  // Allow move operations
    } else {
        GlobalMoveOperation = MOVE_FAILED;  // Disallow move operations
    }
}


void Shellio_Help(uint8* command) {
    uint8 *delimiters = "";  // No delimiters in this case, as we are checking for additional input
    uint8 *token = strtok(NULL, delimiters);  // Get the next token from the command string

    /* For gdb script debugging */
    uint8 NumOfoperand = 0;  // Number of operands for debugging purposes
    uint8* Operand[1] = {NULL};  // Array to store operands for debugging

    // Check if there is additional input after the 'help' command
    if (token != NULL) {
        uint8* path;
        char *found = strstr(token, "2>");
        if (found != NULL) {
            // Redirect standard error if "2>" is found in the input
            path = ErrorFD_Path(found);
            fork_redirectionExec(path, STDERR);
        } else if (searchCharacter(token, '>', &path) == VALID) {
            // Redirect standard output if ">" is found in the input
            fork_redirectionExec(path, STDOUT);
        } else {
            // Print error message if the command is not recognized
            printf("Command not found\nEnter 'assist' to know Shellio commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return;
        }
    }

    // Execute the Help_Seq function based on the process state
    if (IamChild == RAISED) {
        Help_Seq();  // Execute Help_Seq in child process
        exit(100);  // Exit child process
    } else if (IamParent != RAISED) {
        Help_Seq();  // Execute Help_Seq in parent process
    }

    // Update process history and clean up
    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED;  // Reset parent process state
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
    va_list args;           // Declare a variable to hold the variable arguments
    va_start(args, format); // Initialize the va_list variable with the format argument
    vprintf(format, args); // Call vprintf to handle the formatted output
    va_end(args);           // Clean up the va_list variable
}

uint8* Shellio_ParsingPath(uint8* ptr_ArgCounter, uint8* Ptr_1st_Path,
                            uint8* Ptr_2nd_Path, uint8* Copy_token) {
    // Buffer to temporarily store parsed paths
    uint8 Buf[MAX_PATH / 2];
    
    // Static buffer to store options (e.g., "-a" for append)
    static uint8 Option[3];
    
    // Command being processed (hardcoded as "clone" here)
    uint8* command = "clone";

    // Check if the input token is valid
    if (Copy_token == NULL) {
        my_printf("Error: No input token provided.\n");
        return NULL;
    }
    
    // Set the global pointer to the input token
    Ptr_GlobalGetParsingPath = Copy_token;

    // Parse and store the first path from the input
    strcpy(Buf, GetParsedPath(command));
    if (Buf == NULL) {
        printf("Error: Failed to get the first path.\n");
        return NULL;
    }

    // Define a base path keyword for relative path handling
    char keyword[] = "/home/shehabaldeen";
    char* found = strstr(Buf, keyword);

    // Handle relative paths by constructing absolute path
    if (found != NULL) {
        // If keyword is found, copy the path directly
        strcpy(Ptr_1st_Path, Buf);
    } else {
        // Otherwise, construct absolute path from base path
        snprintf(Ptr_1st_Path, MAX_PATH, "%s/%s", GetPathWithoutToken(), Buf);
    }
    
    // Increment argument counter for the first path
    (*ptr_ArgCounter)++;

    // Skip the closing double quote for the first path if present
    if (*Ptr_GlobalGetParsingPath != '"') {
        printf("Error: Expected double quote after the first path.\n");
        return NULL;
    }
    Ptr_GlobalGetParsingPath++;

    // Parse options and handle the second path
    while (*Ptr_GlobalGetParsingPath != '"' && *Ptr_GlobalGetParsingPath != '\0') {
        if (*Ptr_GlobalGetParsingPath == '-') {
            // If an option is found, store it in Option buffer
            snprintf(Option, sizeof(Option), "-%c", *(++Ptr_GlobalGetParsingPath));
            // Increment argument counter for the option
            (*ptr_ArgCounter)++;
        }
        Ptr_GlobalGetParsingPath++;
    }

    // Ensure that the second path is properly closed with a double quote
    if (*Ptr_GlobalGetParsingPath != '"') {
        printf("Error: Expected double quote after options.\n");
        return NULL;
    }

    // Parse and store the second path from the input
    strcpy(Buf, GetParsedPath(command));
    if (Buf == NULL) {
        printf("Error: Failed to get the second path.\n");
        return NULL;
    }

    // Handle relative paths similarly for the second path
    found = strstr(Buf, keyword);
    if (found != NULL) {
        strcpy(Ptr_2nd_Path, Buf);
    } else {
        snprintf(Ptr_2nd_Path, MAX_PATH, "%s/%s", GetPathWithoutToken(), Buf);
    }

    // Increment argument counter for the second path
    (*ptr_ArgCounter)++;

    // Return the options buffer
    return Option;
}

static uint8* GetParsedPath(uint8* command) {
    // Static pointer to hold the parsed path
    static uint8* Path = NULL;  // Static to persist across function calls and avoid dangling pointer issues
    
    // Array to hold operands (not used in this function but included for completeness)
    uint8 NumOfoperand = 1;
    uint8* Operand[1];

    // Check if the global pointer for parsing paths is valid
    if (Ptr_GlobalGetParsingPath == NULL) {
        my_printf("Error: Ptr_GlobalGetParsingPath is NULL\n");
        return NULL;
    }

    // Advance the pointer to skip characters until a double quote (") or end of string is found
    while (*Ptr_GlobalGetParsingPath != '"' && *Ptr_GlobalGetParsingPath != '\0') {
        Ptr_GlobalGetParsingPath++;
    }

    // Check if the current character is a double quote
    if (*Ptr_GlobalGetParsingPath == '"') {
        Ptr_GlobalGetParsingPath++;  // Skip the double quote
    } else if (*Ptr_GlobalGetParsingPath == '\0') {
        my_printf("Error: Expected closing double quote\n");
        return NULL;
    }

    // Allocate memory for the path, considering the length of remaining string plus null terminator
    Path = malloc(strlen(Ptr_GlobalGetParsingPath) + 1);
    
    // Check if memory allocation succeeded
    if (Path == NULL) {
        my_printf("Memory allocation failed\n");
        return NULL;
    }

    // Copy characters from the global pointer to the allocated path buffer
    int i = 0;
    while (*Ptr_GlobalGetParsingPath != '"' && *Ptr_GlobalGetParsingPath != '\0') {
        Path[i++] = *Ptr_GlobalGetParsingPath++;
    }

    // Null-terminate the path string
    Path[i] = '\0';

    // Assign the path to Operand array (though Operand is not used further in this function)
    Operand[0] = Path;

    // Return the parsed path
    return Path;
}

uint8 Shellio_Exit (uint8* command) {
    // Delimiters used for tokenizing input (space and zero)
    uint8 *delimiters = " 0";                   
    // Tokenize the input string to check for additional arguments
    uint8 *token = strtok(NULL, delimiters);  
    
    // For GDB script debugging
    uint8 NumOfoperand = 0;
    uint8* Operand[1] = {NULL};

    // Check if there is additional input after the 'exit' command
    if (token != NULL) {
        // Print error message if additional arguments are present
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        return (uint8)FAILED;  // Return failure status
    }

    // Print a goodbye message
    printf("Good Bye\n");

    return (uint8)SUCCESS;  // Return success status
}

void Shellio_Clear (uint8* command) {
    // Delimiters used for tokenizing input (space and zero)
    uint8 *delimiters = " 0";                   
    // Tokenize the input string to check for additional arguments
    uint8 *token = strtok(NULL, delimiters);  
    
    // For GDB script debugging
    uint8 NumOfoperand = 0;
    uint8* Operand[1] = {NULL};

    // Check if there is additional input after the 'clear' command
    if (token != NULL) {
        // Print error message if additional arguments are present
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    }

    // Push successful operation to process history and clean shared string
    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();

    // Clear the terminal screen
    system("clear");
}


void Shellio_Copy (uint8* command) {
    // Array to store arguments for the copy command
    uint8 Arguments[MAX_ARGUMENTS][MAX_CHARACHTERS_OF_ONE_ARGUMENTS] = {0};
    
    // Tokenize the input to get arguments
    uint8* token = strtok(NULL, "");
    uint8 ArgCounter = 0;
    
    // Parse paths and options from the tokenized input
    uint8* Option = Shellio_ParsingPath(&ArgCounter, Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT], token);
    
    // For GDB script debugging
    uint8 NumOfoperand = 3;
    uint8* Operand[3] = {Arguments[SECOND_ARGUMENT], Option, Arguments[THIRD_ARGUMENT]};

    // Determine the number of arguments and proceed with copy operation
    if (ArgCounter == (MAX_ARGUMENTS - 1)) {
        // If exactly two arguments (source and destination paths) are provided
        Shellio_CopyFile(Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT]);  
    } else if (ArgCounter == MAX_ARGUMENTS) {
        // If additional arguments are provided (e.g., options)
        if (Option == NULL) {
            // Print error message if option parsing failed
            printf("Error In Passing Option\n");
        } else {
            // Handle file options such as append or force move
            char Status = Shellio_FileOption(Option);
            
            if (Status == VALID) {
                // Proceed with copy operation if the option is valid
                Shellio_CopyFile(Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT]);
            }
        }
    } else {
        // Print error message for invalid number of arguments
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
    }
    
    // Clean up shared string data
    cleanSharedString();
}



void Shellio_PrintEnv(uint8* command, uint8* token) {
    char **env = environ;  // Pointer to the environment variables

    // For GDB script debugging
    uint8 NumOfoperand = 0;
    uint8* Operand[1] = {NULL};

    // Check if there is additional input after the 'printenv' command
    if (token != NULL) {
        uint8* path; 
        char *found = strstr(token, "2>");
        
        if (found != NULL) {
            // Handle redirection to standard error
            path = ErrorFD_Path(found);
            fork_redirectionExec(path, STDERR);
        } else if (searchCharacter(token, '>', &path) == VALID) {
            // Handle redirection to standard output
            fork_redirectionExec(path, STDOUT);
        } else {
            // Print error message for invalid command or option
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return;  
        }
    }

    // Print environment variables
    if (IamChild == RAISED) {
        // If in a child process, print environment variables and exit
        printf("-------------------------------------------------------------------------\n");
        while (*env) {
            printf("%s\n", *env);
            env++;
        }
        printf("-------------------------------------------------------------------------\n");
        exit(100);  // Exit child process with status 100
    } else if (IamParent != RAISED) {
        // If in the parent process, print environment variables
        printf("-------------------------------------------------------------------------\n");
        while (*env) {
            printf("%s\n", *env);
            env++;
        }
        printf("-------------------------------------------------------------------------\n");
    }

    // Update process history and clean up
    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED;  // Reset parent status
}


void Shellio_TypeCommand(uint8* command) {
    uint8* token = strtok(NULL, "");  // Extract the command after 'type'

    // For GDB script debugging
    uint8 NumOfoperand = 1;
    uint8* Operand[1];
    Operand[0] = token;

    // Check if there is any additional input after the 'type' command
    if (token == NULL) {
        // If no additional input is provided, print an error message
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    } else {
        uint8* path;
        char *found = strstr(token, "2>");  // Check for redirection to stderr
        
        if (found != NULL) {
            // Handle redirection to standard error
            path = ErrorFD_Path(found);
            fork_redirectionExec(path, STDERR);
            token = strtok(token, " 2>");  // Update token to remove redirection part
        } else if (searchCharacter(token, '>', &path) == VALID) {
            // Handle redirection to standard output
            fork_redirectionExec(path, STDOUT);
            token = strtok(token, " >");  // Update token to remove redirection part
        }
    }

    // Handle the 'type' command
    if (IamChild == RAISED) {
        // If in a child process, execute the type command and exit with status 100
        TypeSeq(token);
        exit(100);
    } else if (IamParent != RAISED) {
        // If in the parent process, execute the type command
        TypeSeq(token);
    }

    // Update process history and clean up
    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED;  // Reset parent status
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

    // New buffer for copying the shared string into
    uint8 Cpstr[MAX_CHARACHTERS_OF_ONE_ARGUMENTS];
    strncpy(Cpstr, sharedString, MAX_CHARACHTERS_OF_ONE_ARGUMENTS);

    // Tokenize the input from the copied string
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

            // Copy characters until the closing quote or end of string
            while (*arg != '"' && *arg != '\0') {
                if (i < MAX_PATH - 1) {
                    path[i++] = *arg;
                }
                arg++;
            }
            path[i] = '\0'; // Null-terminate the path
            if (*arg == '"') arg++; // Skip the closing quote

            // Store the path in the arguments array
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

    // Print arguments for debugging (commented out in production code)
    /*
    printf("Executing command:\n");
    for (int i = 0; i < argcounter; i++) {
        printf("Arg %d: %s\n", i, args[i]);
    }
    */

    // Create a child process to execute the external command
    pid = fork();

    if (pid == -1) {
        // Fork failed
        perror("fork");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        return;
    } else if (pid == 0) {
        // Child process
        execvp(command, args); // Execute the command
        // If execvp returns, it must have failed
        perror("execvp");
        printf("Command not found\nEnter 'assist' to know Shellio commands\n");
        pushProcessHistory(sharedString, FAILED);
        cleanSharedString();
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        wpid = wait(&status); // Wait for the child process to complete

        if (wpid == -1) {
            // Wait failed
            perror("wait");
            printf("Command failed\nEnter 'assist' to know Shellio commands\n");
            pushProcessHistory(sharedString, FAILED);
            cleanSharedString();
            return;
        }

        // Check the exit status of the child process
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            pushProcessHistory(sharedString, SUCCESS);
        } else {
            pushProcessHistory(sharedString, FAILED);
        }
    }

    // Clean up and reset shared string
    cleanSharedString();
}


void Shellio_ChangeDir(uint8* command) {
    // Define delimiters for tokenization. Here, "0" is used as a placeholder.
    const char *delimiters = "0";  // Delimiters are space and tab characters
    // Tokenize the input to extract the directory path after the command
    char *token = strtok(NULL, delimiters);  // Extract the directory path from the input

    /* For gdb script debugging */
    uint8 NumOfoperand = 1;  // Number of operands (arguments) for debugging purposes
    uint8* Operand[1];  // Array to hold pointers to operands
    Operand[0] = token;  // Store the extracted directory path for debugging

    // Define a base path keyword to identify absolute paths
    char keyword[] = "/home/shehabaldeen";

    // Buffer to store the absolute path derived from the input
    uint8 AbsolutePath[MAX_PATH];
    strcpy(AbsolutePath, token);  // Copy the extracted path into the buffer

    // Check if the user provided a directory path after the 'cd' command
    if (token == NULL) {
        // If no directory is provided, print an error message
        printf("command not found\nEnter 'assist' to know Shellio commands\n");
        // Record the failed operation in the process history
        pushProcessHistory(sharedString, FAILED);
        // Clean up shared string buffer
        cleanSharedString();
        return;  // Exit the function early
    }

    // Handle relative paths and resolve them to absolute paths
    // Check if the provided path contains the base keyword for absolute paths
    char *found = strstr(token, keyword);
    if (found == NULL) {
        // If the path does not contain the base keyword, handle relative paths
        found = strstr(token, "../");  // Look for parent directory reference
        if (found != NULL) {
            // Resolve the parent directory reference by removing the last directory
            uint8* path = GetPathWithoutToken();  // Get the current directory path
            uint8* BaseName = basename(path);  // Get the base name of the current path
            uint8 len_path = strlen(path);  // Length of the current path
            uint8 len_BaseName = strlen(BaseName);  // Length of the base name
            uint8 len = (len_path - len_BaseName);  // Calculate the length to keep

            // Copy the directory path excluding the base name into AbsolutePath
            strncpy(AbsolutePath, path, len);
            AbsolutePath[len] = '\0';  // Null-terminate the string
            len++;
        } else {
            // If the path is relative and not a parent directory reference, concatenate paths
            snprintf(AbsolutePath, MAX_PATH, "%s/%s", GetPathWithoutToken(), token);
        }
    }

    Operand[0] = AbsolutePath;  // Store the resolved path for debugging

    // Attempt to change the current working directory to the specified path
    if (chdir(AbsolutePath) == 0) {  // chdir returns 0 on success
        // Directory change successful, record success in the process history
        pushProcessHistory(sharedString, SUCCESS);
    } else {
        // If directory change fails, print an error message and record failure
        pushProcessHistory(sharedString, FAILED);
        perror("cd error::");  // Print error reason
    }

    // Clean up shared string buffer to prevent memory leaks
    cleanSharedString();
}


void Shellio_Phist(uint8* command) {
    /* For gdb script debugging */
    uint8 NumOfoperand = 0;  // Number of operands (arguments) for debugging purposes
    uint8* Operand[1] = {NULL};  // Array to hold pointers to operands (not used in this function)
    uint8* token = strtok(NULL, "");  // Tokenize input to check for additional parameters

    // Check if there are additional inputs after the 'phist' command
    if (token != NULL) {
        uint8* path; 
        char *found = strstr(token, "2>");  // Look for stderr redirection
        if (found != NULL) {
            // Handle stderr redirection
            path = ErrorFD_Path(found);  // Extract the path for redirection
            fork_redirectionExec(path, STDERR);  // Execute redirection
        } else if (searchCharacter(token, '>', &path) == VALID) {
            // Handle stdout redirection
            fork_redirectionExec(path, STDOUT);  // Execute redirection
        } else {
            // Print error if the redirection syntax is invalid
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED);  // Record failure in process history
            cleanSharedString();  // Clean up shared string buffer
            return;  // Exit function
        }
    }

    // Check if the current process is a child or parent
    if (IamChild == RAISED) {
        // Print process history in child process
        printf("Process History:\n");
        for (int i = processCounter - 1; i >= 0; i--) {
            printf("%d\tCommand: %s\tStatus: %d\n", i, PtrProcessHistory[i]->command, PtrProcessHistory[i]->status);
        }
        exit(100);  // Exit child process
    } else if (IamParent != RAISED) {
        // Print process history in parent process
        printf("Process History:\n");
        for (int i = processCounter - 1; i >= 0; i--) {
            printf("%d\tCommand: %s\tStatus: %d\n", i, PtrProcessHistory[i]->command, PtrProcessHistory[i]->status);
        }
    }

    // Record success in process history and clean up
    pushProcessHistory(sharedString, SUCCESS);
    cleanSharedString();
    IamParent = UNRAISED;  // Reset parent status
}


// Function to push a new entry into the process history stack
void pushProcessHistory(const uint8 *command, uint8 status) {
    // Allocate memory for a new ProcessHistory entry
    ProcessHistory *newEntry = (ProcessHistory *)malloc(sizeof(ProcessHistory));
    if (newEntry == NULL) {
        perror("Failed to allocate memory for new process history entry");
        exit(EXIT_FAILURE);  // Exit if memory allocation fails
    }

    // Duplicate the command string and check for allocation failure
    newEntry->command = strdup(command);
    if (newEntry->command == NULL) {
        perror("Failed to duplicate command string");
        free(newEntry);  // Free the allocated memory for the new entry
        exit(EXIT_FAILURE);  // Exit if string duplication fails
    }

    // Set the status for the new entry
    newEntry->status = status;

    // If the stack is full (i.e., reached MAX_STACK_SIZE), remove the oldest element
    if (processCounter >= MAX_STACK_SIZE) {
        // Free the memory allocated for the oldest element
        free(PtrProcessHistory[MAX_STACK_SIZE - 1]->command);
        free(PtrProcessHistory[MAX_STACK_SIZE - 1]);
        processCounter--;  // Decrease the counter as we are removing an element
    }

    // Shift existing elements to the right to make space for the new entry
    for (int i = processCounter; i > 0; i--) {
        PtrProcessHistory[i] = PtrProcessHistory[i - 1];
    }

    // Insert the new entry at the beginning of the stack
    PtrProcessHistory[0] = newEntry;
    processCounter++;  // Increase the counter to reflect the added entry
}

// Cleanup function to free all allocated memory for process history
static void cleanupProcessHistory() {
    // Iterate through all entries in the stack
    for (int i = 0; i < processCounter; i++) {
        // Free the command string and the ProcessHistory entry itself
        free(PtrProcessHistory[i]->command);
        free(PtrProcessHistory[i]);
    }
}

// Function to set the global sharedString to a duplicate of the input string
void setSharedString(const uint8 *str) {
    // Duplicate the input string and assign it to sharedString
    sharedString = strdup(str);

    // Note: Ensure to handle NULL inputs or check for memory allocation failures
}

// Function to free the memory allocated for the global sharedString
void cleanSharedString() {
    // Free the allocated memory for sharedString
    free(sharedString);

    // After freeing, ensure sharedString is set to NULL to avoid dangling pointers
    sharedString = NULL;
}

// Function to search for a command in the directories listed in the PATH environment variable
static uint8 SearchOnCommand(uint8 *token) {
    // Retrieve the PATH environment variable
    char *path_env = getenv("PATH");

    // Check if PATH is not set; return FAILED if NULL
    if (path_env == NULL) {
        return FAILED; // PATH environment variable not set
    }

    // Create a copy of PATH to tokenize and search
    char path_copy[MAX_PATH];
    strncpy(path_copy, path_env, sizeof(path_copy));

    // Tokenize the PATH variable to extract directories
    char *dir = strtok(path_copy, ":");
    while (dir != NULL) {
        // Construct the full path by appending the command token to the directory
        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, token);

        // Check if the file exists and is executable
        if (access(full_path, X_OK) == 0) {
            return SUCCESS; // Command found and is executable
        }

        // Move to the next directory in PATH
        dir = strtok(NULL, ":");
    }

    // Command not found in any directory listed in PATH
    return FAILED; // Command not found or not executable
}

// Function to get the username of the current user
const char* getUserName() {
    struct passwd *pw;   // Pointer to a structure containing user information
    uid_t uid;           // User ID of the current process

    uid = geteuid();     // Get the effective user ID of the calling process
    pw = getpwuid(uid);  // Retrieve user information based on UID

    // Check if the user information retrieval was successful
    if (pw) {
        return pw->pw_name; // Return the username
    }
    return ""; // Return an empty string if user information could not be retrieved
}

// Function to get the hostname of the system
void getHostName(char *hostname, size_t size) {
    // Get the hostname and check for errors
    if (gethostname(hostname, size) == -1) {
        perror("gethostname"); // Print an error message if gethostname fails
        strcpy(hostname, "unknown"); // Set hostname to "unknown" if an error occurs
    }
}

// Function to print the command prompt with user and hostname information
void printPrompt() {
    const char *user = getUserName(); // Retrieve the username
    char host[256]; // Buffer to store the hostname

    getHostName(host, sizeof(host)); // Retrieve the hostname
    // Print the username in bold green, followed by the hostname in bold green
    printf("%s%s%s", COLOR_BOLD_GREEN, user, COLOR_RESET);
    printf("@%s%s%s:", COLOR_BOLD_GREEN, host, COLOR_RESET);
}

// Function to get the current working directory without a token
uint8* GetPathWithoutToken() {
    // Static buffer to store the current working directory
    static char cwd[MAX_PATH];

    // Retrieve the current working directory
    getcwd(cwd, sizeof(cwd));
    return cwd; // Return the buffer containing the current working directory
}

void Shellio_Meminfo(uint8* command){
    // Tokenize the input to handle additional arguments
    uint8 *token  = strtok(NULL, "");  // Extract the next token from the command input
    /* For gdb script debugging */
    uint8 NumOfoperand = 0 ;   // Number of operands (set to 0 as default)
    uint8* Operand[1] = {NULL}; // Array to store operands (only one in this case)

    // Check if there is any additional input after the 'meminfo' command
    if (token != NULL) {
        uint8* path; 
        // Check if the token contains redirection for stderr
        char *found = strstr(token, "2>");
        if (found != NULL) {
            path = ErrorFD_Path(found); // Get the file path for stderr redirection
            fork_redirectionExec(path, STDERR); // Execute redirection for stderr
        }
        // Check if the token contains redirection for stdout
        else if (searchCharacter(token, '>', &path) == VALID) {
            fork_redirectionExec(path, STDOUT); // Execute redirection for stdout
        }
        else {
            // If no valid redirection is found, print an error message
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED); // Log failure in process history
            cleanSharedString(); // Clean up the shared string memory
            return;  // Exit the function as the command is not valid
        }
    }

    // Check if the current process is a child process
    if (IamChild == RAISED){
        FreeSeq(); // Free memory or resources associated with the sequence
        exit(100); // Exit the child process with status 100
    }
    // If the current process is the parent process
    else if (IamParent != RAISED){
        FreeSeq(); // Free memory or resources associated with the sequence
    }

    // Log the success status and clean up
    pushProcessHistory(sharedString, SUCCESS); // Log success in process history
    cleanSharedString(); // Clean up the shared string memory
    IamParent = UNRAISED ; // Reset parent process status
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
    /* For gdb script debugging */
    uint8 NumOfoperand = 0 ;   // Number of operands (set to 0 as default)
    uint8* Operand[1] = {NULL}; // Array to store operands (only one in this case)

    // Tokenize the input to handle additional arguments
    uint8 *token  = strtok(NULL, "") ;  // Extract the next token from the command input

    // Check if there is any additional input after the 'uptime' command
    if (token != NULL) {
        uint8* path ; 
        // Check if the token contains redirection for stderr
        char *found = strstr(token, "2>");
        if (found != NULL){
            path = ErrorFD_Path(found); // Get the file path for stderr redirection
            fork_redirectionExec(path, STDERR); // Execute redirection for stderr
        }
        // Check if the token contains redirection for stdout
        else if (searchCharacter(token, '>', &path) == VALID ) {
            fork_redirectionExec(path, STDOUT); // Execute redirection for stdout
        }
        else {
            // If no valid redirection is found, print an error message
            printf("command not found\nEnter (assist) to know Shellio Commands\n");
            pushProcessHistory(sharedString, FAILED); // Log failure in process history
            cleanSharedString(); // Clean up the shared string memory
            return;  // Exit the function as the command is not valid
        }
    }

    // Check if the current process is a child process
    if (IamChild == RAISED){
        uptimeSeq(); // Execute the uptime sequence for the child process
        exit(100); // Exit the child process with status 100
    }
    // If the current process is the parent process
    else if (IamParent != RAISED){
        uptimeSeq(); // Execute the uptime sequence for the parent process
    }

    // Log the success status and clean up
    pushProcessHistory(sharedString, SUCCESS); // Log success in process history
    cleanSharedString(); // Clean up the shared string memory
    IamParent = UNRAISED ; // Reset parent process status
}

static void uptimeSeq() {
    char buffer[100];  // Buffer to store the contents of /proc/uptime

    ssize_t bytes_read;
    int FD;
    char *uptime, *idletime;

    // Open the /proc/uptime file for reading
    FD = open("/proc/uptime", O_RDONLY);
    if (FD < 0) {
        perror("open");  // Print error if file cannot be opened
        return;
    }

    // Read the contents of the file into the buffer
    bytes_read = read(FD, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");  // Print error if reading fails
        close(FD);       // Close the file descriptor before returning
        return;
    }
    buffer[bytes_read] = '\0';  // Null-terminate the string to make it a valid C string

    // Close the file descriptor after reading
    close(FD);

    // Tokenize the buffer to separate uptime and idletime
    uptime = strtok(buffer, " ");       // Extract uptime (time since last boot)
    idletime = strtok(NULL, " ");        // Extract idletime (time the system has been idle)

    uint8 len = strlen(idletime);        // Get the length of the idletime string
    idletime[len-1] = '\0';              // Remove the trailing newline character from idletime

    if (uptime != NULL && idletime != NULL) {
        // Print the uptime and idletime values
        printf("The uptime : %s seconds\n", uptime);
        printf("The idletime : %s seconds\n", idletime);
    } else {
        // Print an error message if parsing fails
        printf("Error parsing uptime data.\n");
    }
}

static uint8 searchCharacter(const uint8 *str, char character, uint8** Remaining) {
    // Use strchr to find the first occurrence of the specified character in the string
    *Remaining = (uint8*) strchr((const char*)str, character);

    if (*Remaining != NULL) {
        // If the character is found, move the pointer past the character itself
        (*Remaining)++;
        
        // Extract the part of the string after the character
        *Remaining = (uint8*) strtok((char*) *Remaining, "");
        if (*Remaining) {
            // Set the global variable with the parsed path (if necessary)
            Ptr_GlobalGetParsingPath = *Remaining;

            // Get the absolute path
            *Remaining = GetParsedPath("path");

            // Get the relative path
            *Remaining = GetRelativePath(*Remaining);

            return VALID;  // Indicate that the character was found and processing succeeded
        } else {
            return INVALID;  // Return INVALID if strtok fails to extract the remaining part
        }
    }

    return INVALID;  // Return INVALID if the character was not found in the string
}

static uint8* ErrorFD_Path(uint8* path) {
    // Skip the initial "2>" characters, which are used for error redirection
    path += 2;

    // Tokenize the string to isolate the path after the redirection operator
    path = (uint8*) strtok((char*) path, "");

    // Set the global parsing path to the extracted path
    Ptr_GlobalGetParsingPath = path;

    // Parse the path to ensure it's in the correct format
    path = GetParsedPath("path");

    // Convert the path to a relative path if necessary
    path = GetRelativePath(path);

    // Return the processed path
    return path;
}

static void redirect(uint8* path, int newFD) {
    // Define file permissions: read and write for the owner, read for group and others
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    // Open the file specified by the path. Create the file if it doesn't exist, and open it for writing.
    int outfd = open(path, O_CREAT | O_WRONLY, mode);

    if (outfd < 0) {
        // Print an error message if the file cannot be opened or created
        perror("Error In Opening File :: ");
        return;
    }

    // Redirect the new file descriptor (newFD) to the opened file descriptor (outfd)
    int FD = dup2(outfd, newFD);

    if (FD != newFD) {
        // Print an error message if the file descriptor could not be reserved correctly
        perror("Error In reserving FD of stdout");
    }

    // Close the file descriptor as it's no longer needed after redirection
    close(outfd);
}


static void fork_redirectionExec(uint8* path, int FD) {
    // Fork the current process to create a child process
    int retPid = fork();

    if (retPid > 0) {  
        // This block executes in the parent process

        // Record the success of the command execution in process history
        pushProcessHistory(sharedString, SUCCESS);  
        
        // Indicate that the parent process is currently active
        IamParent = RAISED;            
        
        // Wait for the child process to complete
        int status = 0;	
        wait(&status);
    } 
    else if (retPid == 0) {
        // This block executes in the child process

        // Redirect the file descriptor (FD) to the specified path
        redirect(path, FD);
        
        // Indicate that the child process is currently active
        IamChild = RAISED;
    } 
    else {
        // Fork failed
        perror("fork");
    }
}



static uint8* GetRelativePath(uint8 path[]) {
    // Allocate memory for the resulting path
    uint8* ResultPath = (uint8*) malloc(MAX_PATH);
    
    // Define the base path to check against
    char keyword[] = "/home/shehabaldeen";
    
    // Check if the path contains the base path keyword
    char *found = strstr((char*)path, keyword);

    if (found == NULL) {
        // If the base path is not found, construct a relative path
        snprintf((char*)ResultPath, MAX_PATH, "%s/%s", GetPathWithoutToken(), path);
    } else {
        // If the base path is found, copy the original path to the result
        strncpy((char*)ResultPath, (char*)path, MAX_PATH);
    }

    // Return the resulting path
    return ResultPath;
}



void Shellio_PrintEnvVar(uint8* command, uint8* copy_token) {
    // Debugging information for GDB
    uint8 NumOfoperand = 1;
    uint8* Operand[1];
    Operand[0] = copy_token;
    
    // Buffer for storing environment variable names
    uint8 token[20];

    // Check if there is additional input after the command
    if (copy_token != NULL) {
        uint8* path;
        
        // Check if the command contains redirection to stderr
        char *found = strstr(copy_token, "2>");
        if (found != NULL) {
            // Extract the path for redirection and execute with stderr redirection
            path = ErrorFD_Path(found);
            fork_redirectionExec(path, STDERR);
            // Remove redirection part from the token
            strcpy(token, strtok(copy_token, " 2>"));
        }
        // Check if the command contains redirection to stdout
        else if (searchCharacter(copy_token, '>', &path) == VALID) {
            // Extract the path for redirection and execute with stdout redirection
            fork_redirectionExec(path, STDOUT);
            // Remove redirection part from the token
            strcpy(token, strtok(copy_token, " >"));
        }
    }

    // Child process block
    if (IamChild == RAISED) {
        // Retrieve the environment variable value
        char *path_env = getenv(token);
    
        if (path_env == NULL) {
            // If the environment variable is not found, check local variables
            uint8 status = printLocalVariables(token);

            if (status == INVALID) {
                // Print message if variable is not found
                printf("This variable isn't existed :: %s\n", token);
                pushProcessHistory(sharedString, FAILED);
            } else {
                pushProcessHistory(sharedString, SUCCESS);
            }
        } else {
            // Print the environment variable value
            printf("%s = %s\n", token, path_env);
            pushProcessHistory(sharedString, SUCCESS);
        }
        // Exit the child process
        exit(100);
    }
    // Parent process block
    else if (IamParent != RAISED) {
        // Remove redirection part from the token
        strcpy(token, strtok(copy_token, " "));
        // Retrieve the environment variable value
        char *path_env = getenv(token);
    
        if (path_env == NULL) {
            // If the environment variable is not found, check local variables
            uint8 status = printLocalVariables(token);

            if (status == INVALID) {
                // Print message if variable is not found
                printf("This variable isn't existed :: %s\n", token);
                pushProcessHistory(sharedString, FAILED);
            } else {
                pushProcessHistory(sharedString, SUCCESS);
            }
        } else {
            // Print the environment variable value
            printf("%s = %s\n", token, path_env);
            pushProcessHistory(sharedString, SUCCESS);
        }
    }

    // Clean up and reset parent status
    cleanSharedString();
    IamParent = UNRAISED;
}


void Shellio_setVariable(uint8* command) {
    // Check if the command input is NULL
    if (command == NULL) {
        printf("Invalid command format.\n");
        return;
    }

    // Tokenize the input command to extract the variable name and value
    char* name = strtok((char*)command, "=");
    char* value = strtok(NULL, "");

    // Check if both name and value are non-NULL
    if (name != NULL && value != NULL) {
        // Set the local variable using the extracted name and value
        setLocalVariable(name, value);
        // Confirm that the variable has been set
        printf("Variable set: %s=%s\n", name, value);
    } else {
        // Print an error message if the format is incorrect
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
    Shellio_PrintEnv("allVar", NULL);
}



void setLocalVariable(const char* name, const char* value) {
    // Iterate through the list of local variables to check if the variable already exists
    for (int i = 0; i < localVarCount; i++) {
        // If the variable with the same name is found, update its value
        if (strcmp(localVariables[i].name, name) == 0) {
            // Copy the new value into the existing variable's value field
            strncpy(localVariables[i].value, value, MAX_VAR_VALUE);
            // Ensure the value is null-terminated
            localVariables[i].value[MAX_VAR_VALUE - 1] = '\0';
            return;
        }
    }
    
    // If the variable does not exist and there is space to add new variables
    if (localVarCount < MAX_VARS) {
        // Copy the new variable's name and value into the next available slot
        strncpy(localVariables[localVarCount].name, name, MAX_VAR_NAME);
        // Ensure the name is null-terminated
        localVariables[localVarCount].name[MAX_VAR_NAME - 1] = '\0';
        
        strncpy(localVariables[localVarCount].value, value, MAX_VAR_VALUE);
        // Ensure the value is null-terminated
        localVariables[localVarCount].value[MAX_VAR_VALUE - 1] = '\0';
        
        // Increment the count of local variables
        localVarCount++;
    } else {
        // Print an error message if the maximum number of local variables has been reached
        printf("Maximum number of local variables reached.\n");
    }
}


static uint8 printLocalVariables(char* var) {
    // Iterate through the list of local variables
    for (int i = 0; i < localVarCount; ++i) {
        // Check if the variable name matches the input variable name
        if (strcmp(localVariables[i].name, var) == 0) {
            // Print the value of the matching variable
            printf("%s\n", localVariables[i].value);
            return VALID;
        }
    }
    // Return INVALID if the variable is not found
    return INVALID;
}
