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
 * supports local and environment variables, Help_Redirection, command history, and more.
 * The command execution is enhanced by custom handling of errors and outputs. The 
 * shell also includes features for displaying system information like memory usage 
 * and uptime.
 ==============================================================================
*/

/*===================================  Includes ===============================*/
#include "Commands.h"
#include "Helper.h"

/* External Global variables */
extern char **environ; /**< Pointer to the environment variables */
extern char GlobalAppendFlag ;
extern char GlobalMoveForcedFlag ;
extern char GlobalMoveOperation  ;
extern uint8* Ptr_GlobalGetParsingPath ;
extern uint8 IamChild ;
extern uint8 IamParent ;
extern int localVarCount ;
extern LocalVariable localVariables[MAX_VARS];
extern ProcessHistory *PtrProcessHistory[MAX_STACK_SIZE];
extern uint8 processCounter ;
extern uint8 *sharedString ;


/*===========================  Functions Implementations ======================*/

void Commands_Loop(char* str){
    uint8 *token;                      // Pointer to store each word of the command as a token
    char* commands[MAX_PIPES] ;
    uint8* separators = "============================================================================================";  // Separator line for formatting output
    
    /* to remove spaces in first and last of each command */
    Help_TrimSpaces(str);        
    // To share this input into the history
    Help_SetSharedString(str);  // Store the input command in a global history buffer
    /* Initial call to strtok */ 
    token = strtok(str," ");  // Tokenize the input string using the specified delimiters

    if (token != NULL) {  // If a token was found
        /* print goodbye then exit */ 
        if (strcmp(token, "leave") == EQUALED) {  // If the command is 'leave'
            uint8 status = Commands_Exit(token);  // Call the exit function
            /* exit if it succeeded */
            if (status == SUCCESS) {  // If the exit command succeeded
                exit(0);  // Exit the loop and terminate the shell
            }
        }
        /* clear screen with clear command */ 
        else if (strcmp(token, "cls") == CLEARED) {  // If the command is 'cls' (clear screen)
            Commands_Clear(token);  // Clear the screen
        }
        /* get absolute path of current working directory */
        else if (strcmp(token, "path") == PWD_PASS) {  // If the command is 'path'
            Commands_GetPath(token);  // Print the current working directory
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else if (strcmp(token, "display") == ECHO_PASS) {  // If the command is 'display'
            Commands_EchoInput(token);  // Echo the input back to the user
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else if (strcmp(token, "assist") == HELP_PASS) {  // If the command is 'assist'
            Commands_Help(token);  // Display help information
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else if (strcmp(token, "clone") == COPY_PASS) {  // If the command is 'clone'
            Commands_Copy(token);  // Copy a file or directory
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else if (strcmp(token, "shift") == MV_PASS) {  // If the command is 'shift'
            Commands_MoveFile(strcmp(token, "shift"));  // Move a file or directory
            Commands_Copy(token);  // Copy a file or directory
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else if (strcmp(token, "cd") == CD_PASS) {  // If the command is 'cd'
            Commands_ChangeDir(token);  // Change the current working directory
        }
        else if (strcmp(token, "type") == TYPE_PASS) {  // If the command is 'type'
            Commands_TypeCommand(token);  // Display the contents of a file
        } 
        else if (strcmp(token, "envir") == ENV_PASS) {  // If the command is 'envir'
            uint8 * command = strdup(token);  // Duplicate the command string
            token = strtok(NULL, "");  // Get the next token, which is the argument to 'envir'
            if (token == NULL || *(token) == '2' || (*token ) == '>' && strstr(token, " < ") == NULL ) {  // If no argument or Help_Redirection is provided
                Commands_PrintEnv(command, token);  // Print the environment variables
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            } else {
                Commands_PrintEnvVar(command, token);  // Print the value of a specific environment variable
                printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
            }
            free(command);  // Free the memory allocated for the duplicated command
        } 
        else if (strcmp(token, "phist") == EXIT) {  // If the command is 'phist'
            Commands_Phist(token);  // Print the process history
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else if (strcmp(token, "free") == FREE_PASS) {  // If the command is 'free'
            Commands_Meminfo(token);  // Display memory usage information
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        } 
        else if (strcmp(token, "uptime") == UPTIME_PASS) {  // If the command is 'uptime'
            Commands_uptime(token);  // Display system uptime and idle time
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else if (strchr(token, '=') != NULL) {  // If the token contains an '=' character
            Commands_setVariable(token);  // Set a local variable with the format name=value
        }
        else if (strcmp(token, "allVar") == ALLVAR_PASS) {  // If the command is 'allVar'
            Commands_allVar();  // Print all local and environment variables
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
        else {
            Commands_ExecExternalCommands(token);  // Execute an external command
            printf("%s%s%s \n", COLOR_BOLD_BLUE, separators, COLOR_RESET);  // Print a separator line in blue
        }
    }
}


void Commands_GetPath(uint8* command) {
    uint8 Status = Help_RedirectionHandlerOfnoOption(command);

    if (Status == INVALID){
        return ;
    }

    /* If running in child process, get the current path and exit */
    if (IamChild == RAISED) {
        Help_GetPathSeq();  // Call function to get and print the current working directory
        exit(100);  // Exit with a specific status code
    }
    else if (IamParent != RAISED) {
        /* If running in parent process and not raised, get the current path */
        Help_GetPathSeq();
    }

    /* Clean up and reset parent status */
    Help_PushProcessHistory(sharedString, SUCCESS);
    Help_CleanSharedString();
    IamParent = UNRAISED;  // Reset the parent status
}


void Commands_EchoInput(uint8* command) {
    uint8* token = Help_RedirectionHandlerOfWithOption (command);

    if (token == NULL){
        return ;
    }

    if (IamChild == RAISED) {
        Help_DisplaySeq(token);  // Display the token if in child process
        exit(100);  // Exit child process
    } else if (IamParent != RAISED) {
        Help_DisplaySeq(token);  // Display the token if in parent process
    }

    Help_CleanSharedString();  // Clean up the shared string
    IamParent = UNRAISED;  // Reset parent status
    free(token);
}


void Commands_CopyFile(const char *sourcePath, const char *destPath) {
    /* Check if the source and destination files are the same */
    if (strcmp(sourcePath, destPath) == SAME ) {
        Help_MyPrintf("Error :: Source and Destination files are same \n");  
        Help_PushProcessHistory(sharedString, FAILED);
        Help_CleanSharedString();
        return ; 
    }

    /* To check that our file is already move to the diserd directory */
    uint8 Suc_Move = CLEARED ;

    /* Try to open source */
    int FD_SrcFile = open(sourcePath, O_RDONLY);

    /* Terminate this operation if the source file does not exist */
    if (FD_SrcFile == FD_INVALID) {
        perror("Source file open() error");
        Help_PushProcessHistory(sharedString, FAILED);
        Help_CleanSharedString();
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
            Help_PushProcessHistory(sharedString, FAILED);
            Help_CleanSharedString();
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
                Help_PushProcessHistory(sharedString, FAILED);
                Help_CleanSharedString();
                return;
            } 
        }
        else {
            Help_MyPrintf ("Error :: Destination File is already existed\n");
            close(FD_SrcFile);
            Help_PushProcessHistory(sharedString, FAILED);
            Help_CleanSharedString();
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
            Help_MyPrintf("Error :: Source and Destination files are same \n");  
            Help_PushProcessHistory(sharedString, FAILED);
            Help_CleanSharedString();
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
                Help_PushProcessHistory(sharedString, FAILED);
                Help_CleanSharedString();
                return;
            }         
        }
        else {
            FD_DesFile = open(ConcatenatedDesFile, O_WRONLY | O_CREAT | O_TRUNC, mode);

            /* Terminate this operation if the destination file does not exist */
            if (FD_DesFile == FD_INVALID ) {
                perror("Destination file open() error");
                close(FD_SrcFile);
                Help_PushProcessHistory(sharedString, FAILED);
                Help_CleanSharedString();
                return;
            }        
        }

        /* Terminate operation if the destination file could not be created */
        if (FD_DesFile == FD_INVALID ){
            Help_MyPrintf("Given path of directory isn't correct\n");
            perror("Destination fopen() error");
            close(FD_SrcFile);
            Help_PushProcessHistory(sharedString, FAILED);
            Help_CleanSharedString();
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
            Help_PushProcessHistory(sharedString, FAILED);
            break ;
        }
        else if (Write_Size < Read_Size) {
            // Not all bytes were written; handle the partial write
            fprintf(stderr, "write_to_stdout: Partial write occurred. Expected %zd, wrote %zd\n", Read_Size, Write_Size);
            Suc_Move = CLEARED ;
            Help_PushProcessHistory(sharedString, FAILED);
            break ;
        }
        else {
            write(STDOUT, "\n",1);
            Suc_Move = SET ;
        }

        Help_PushProcessHistory(sharedString, SUCCESS);

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
            Help_PushProcessHistory(sharedString, FAILED);
        }
    }

    /* Clear whole flags */
    GlobalAppendFlag     = CLEARED ;
    GlobalMoveOperation  = MOVE_FAILED ;
    GlobalMoveForcedFlag = CLEARED ;
}


char Commands_FileOption(const char* Copy_Option) {
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
        Help_MyPrintf("Incorrect option: %s\n", Copy_Option);  // Print error message for unrecognized option
        Local_Status = INVALID;  // Indicate that the option is invalid
    }

    Help_CleanSharedString();  // Clean shared string data
    return Local_Status;  // Return the status of the function
}

void Commands_MoveFile(const char Copy_MoveFlag) {
    if (Copy_MoveFlag == MOVE_PASS) {
        GlobalMoveOperation = MOVE_PASS;  // Allow move operations
    } else {
        GlobalMoveOperation = MOVE_FAILED;  // Disallow move operations
    }
}


void Commands_Help(uint8* command) {
    uint8 Status = Help_RedirectionHandlerOfnoOption(command);

    if (Status == INVALID){
        return ;
    }

    // Execute the Help_AssistSeq function based on the process state
    if (IamChild == RAISED) {
        Help_AssistSeq();  // Execute Help_AssistSeq in child process
        exit(100);  // Exit child process
    } else if (IamParent != RAISED) {
        Help_AssistSeq();  // Execute Help_AssistSeq in parent process
    }

    // Update process history and clean up
    Help_PushProcessHistory(sharedString, SUCCESS);
    Help_CleanSharedString();
    IamParent = UNRAISED;  // Reset parent process state
}


uint8* Commands_ParsingPath(uint8* ptr_ArgCounter, uint8* Ptr_1st_Path,
                            uint8* Ptr_2nd_Path, uint8* Copy_token) {
    // Buffer to temporarily store parsed paths
    uint8 Buf[MAX_PATH / 2];
    
    // Static buffer to store options (e.g., "-a" for append)
    static uint8 Option[3];
    
    // Command being processed (hardcoded as "clone" here)
    uint8* command = "clone";

    // Check if the input token is valid
    if (Copy_token == NULL) {
        Help_MyPrintf("Error: No input token provided.\n");
        return NULL;
    }
    
    // Set the global pointer to the input token
    Ptr_GlobalGetParsingPath = Copy_token;

    // Parse and store the first path from the input
    strcpy(Buf, Help_GetParsedPath(command));
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
        snprintf(Ptr_1st_Path, MAX_PATH, "%s/%s", Help_GetPathWithoutToken(), Buf);
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
    strcpy(Buf, Help_GetParsedPath(command));
    if (Buf == NULL) {
        printf("Error: Failed to get the second path.\n");
        return NULL;
    }

    // Handle relative paths similarly for the second path
    found = strstr(Buf, keyword);
    if (found != NULL) {
        strcpy(Ptr_2nd_Path, Buf);
    } else {
        snprintf(Ptr_2nd_Path, MAX_PATH, "%s/%s", Help_GetPathWithoutToken(), Buf);
    }

    // Increment argument counter for the second path
    (*ptr_ArgCounter)++;

    // Return the options buffer
    return Option;
}


uint8 Commands_Exit (uint8* command) {
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

void Commands_Clear (uint8* command) {
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
        Help_PushProcessHistory(sharedString, FAILED);
        Help_CleanSharedString();
        return;
    }

    // Push successful operation to process history and clean shared string
    Help_PushProcessHistory(sharedString, SUCCESS);
    Help_CleanSharedString();

    // Clear the terminal screen
    system("clear");
}


void Commands_Copy (uint8* command) {
    // Array to store arguments for the copy command
    uint8 Arguments[MAX_ARGUMENTS][MAX_CHARACHTERS_OF_ONE_ARGUMENTS] = {0};
    
    // Tokenize the input to get arguments
    uint8* token = strtok(NULL, "");
    uint8 ArgCounter = 0;
    
    // Parse paths and options from the tokenized input
    uint8* Option = Commands_ParsingPath(&ArgCounter, Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT], token);
    
    // For GDB script debugging
    uint8 NumOfoperand = 3;
    uint8* Operand[3] = {Arguments[SECOND_ARGUMENT], Option, Arguments[THIRD_ARGUMENT]};

    // Determine the number of arguments and proceed with copy operation
    if (ArgCounter == (MAX_ARGUMENTS - 1)) {
        // If exactly two arguments (source and destination paths) are provided
        Commands_CopyFile(Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT]);  
    } else if (ArgCounter == MAX_ARGUMENTS) {
        // If additional arguments are provided (e.g., options)
        if (Option == NULL) {
            // Print error message if option parsing failed
            printf("Error In Passing Option\n");
        } else {
            // Handle file options such as append or force move
            char Status = Commands_FileOption(Option);
            
            if (Status == VALID) {
                // Proceed with copy operation if the option is valid
                Commands_CopyFile(Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT]);
            }
        }
    } else {
        // Print error message for invalid number of arguments
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
    }
    
    // Clean up shared string data
    Help_CleanSharedString();
}



void Commands_PrintEnv(uint8* command, uint8* token) {
    char **env = environ;  // Pointer to the environment variables

    // For GDB script debugging
    uint8 NumOfoperand = 0;
    uint8* Operand[1] = {NULL};

    // Check if there is additional input after the 'printenv' command
    if (token != NULL) {
        char ErrorFlag = ON ;
        uint8* path; 
        int SecondFD = -1 , SecondFDWithout2 = -1 ;

        char *found = strstr(token, "2>");  // Check if the token contains Help_Redirection to stderr
        int pos = Help_SearchOnSpaceBeforeArrow (token) ; 

        /* if contain > then cancel print on screen because of there are target file after > */
        if ( pos != INVALID_ID && found != NULL){
            SecondFD = STDOUT ;
            SecondFDWithout2 = STDERR ;
            ErrorFlag = OFF ;
        }

        if (found != NULL) {
            path = Help_FindRedirectionPath(found);  // Get the path for stderr Help_Redirection
            Help_ForkAssistRedirectionExec(path, STDERR, SecondFD);  // Execute with stderr Help_Redirection
            ErrorFlag = OFF ;
        }

        if ( pos == INVALID_ID )
            found = NULL ;
        else{
            token+=pos;
            found = strstr(token, ">"); 
        }
        
        if (found != NULL) {  // Check if the token contains Help_Redirection to stdout
            path = Help_FindRedirectionPath(found);
            Help_ForkAssistRedirectionExec(path, STDOUT, SecondFDWithout2);  // Execute with stdout Help_Redirection
            ErrorFlag = OFF ;
        }
        
        if (ErrorFlag == ON ) {
            // If no valid Help_Redirection is found, print an error message
            printf("command not found\nEnter 'assist' to know Shellio commands\n");
            Help_PushProcessHistory(sharedString, FAILED);  // Record the failure in process history
            Help_CleanSharedString();  // Clean up the shared string
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
    Help_PushProcessHistory(sharedString, SUCCESS);
    Help_CleanSharedString();
    IamParent = UNRAISED;  // Reset parent status
}


void Commands_TypeCommand(uint8* command) {
    uint8* token = Help_RedirectionHandlerOfWithOption (command);

    if (token == NULL){
        return ;
    }

    // Handle the 'type' command
    if (IamChild == RAISED) {
        // If in a child process, execute the type command and exit with status 100
        Help_TypeSeq(token);
        exit(100);
    } else if (IamParent != RAISED) {
        // If in the parent process, execute the type command
        Help_TypeSeq(token);
    }

    // Update process history and clean up
    Help_PushProcessHistory(sharedString, SUCCESS);
    Help_CleanSharedString();
    IamParent = UNRAISED;  // Reset parent status
    free(token);
}

// Function to execute external commands with Help_Redirections
void Commands_ExecExternalCommands(uint8 *command) {
    int status;
    pid_t pid, wpid;
    char* args[MAX_ARGS];
    char* cmd;

    // Buffer for copying the shared string into
    uint8 Cpstr[MAX_CHARACHTERS_OF_ONE_ARGUMENTS];
    strncpy(Cpstr, sharedString, MAX_CHARACHTERS_OF_ONE_ARGUMENTS);

    // Handle Help_Redirections before tokenizing
    uint8* InputFile = Help_HandleOptionRedirection(Cpstr,"<");
    uint8* OutFile = Help_HandleOptionRedirection(Cpstr," >");
    uint8* ErrFile = Help_HandleOptionRedirection(Cpstr,"2>");

    // Tokenize the input from the copied string
    cmd =  strtok( Cpstr ,"2><");
    Help_TrimSpaces(cmd); // Ensure leading and trailing spaces are removed

    args[0] = "sh" ;
    args[1] = "-c" ;
    args[2] = cmd;
    args[3] = NULL ;

    // Create a child process to execute the external command
    pid = fork();

    if (pid == -1) {
        // Fork failed
        perror("fork");
        Help_PushProcessHistory(sharedString, FAILED);
        Help_CleanSharedString();
        return;
    } else if (pid == 0) {
        // Child process
        Help_Redirect (InputFile,STDIN);
        Help_Redirect (OutFile,STDOUT);
        Help_Redirect (ErrFile,STDERR);
        if (InputFile != NULL ) free (InputFile);
        if (OutFile != NULL ) free (OutFile);           
        if (ErrFile != NULL ) free (ErrFile);
        execvp("sh", args);
        perror("execvp");
        printf("Command not found\nEnter 'assist' to know Shellio commands\n");
        Help_PushProcessHistory(sharedString, FAILED);
        Help_CleanSharedString();
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        wpid = wait(&status); // Wait for the child process to complete

        if (wpid == -1) {
            // Wait failed
            perror("wait");
            printf("Command failed\nEnter 'assist' to know Shellio commands\n");
            Help_PushProcessHistory(sharedString, FAILED);
            Help_CleanSharedString();
            return;
        }
    }

    // Clean up and reset shared string
    Help_PushProcessHistory(sharedString, SUCCESS);
    Help_CleanSharedString();
}



void Commands_ChangeDir(uint8* command) {
    // Tokenize the input to extract the directory path after the command
    char *token = strtok(NULL, "");  // Extract the directory path from the input

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
        Help_PushProcessHistory(sharedString, FAILED);
        // Clean up shared string buffer
        Help_CleanSharedString();
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
            uint8* path = Help_GetPathWithoutToken();  // Get the current directory path
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
            snprintf(AbsolutePath, MAX_PATH, "%s/%s", Help_GetPathWithoutToken(), token);
        }
    }

    Operand[0] = AbsolutePath;  // Store the resolved path for debugging

    // Attempt to change the current working directory to the specified path
    if (chdir(AbsolutePath) == 0) {  // chdir returns 0 on success
        // Directory change successful, record success in the process history
        Help_PushProcessHistory(sharedString, SUCCESS);
    } else {
        // If directory change fails, print an error message and record failure
        Help_PushProcessHistory(sharedString, FAILED);
        perror("cd error::");  // Print error reason
    }

    // Clean up shared string buffer to prevent memory leaks
    Help_CleanSharedString();
}


void Commands_Phist(uint8* command) {
    uint8 Status = Help_RedirectionHandlerOfnoOption(command);

    if (Status == INVALID){
        return ;
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
    Help_PushProcessHistory(sharedString, SUCCESS);
    Help_CleanSharedString();
    IamParent = UNRAISED;  // Reset parent status
}






void Commands_Meminfo(uint8* command){
    uint8 Status = Help_RedirectionHandlerOfnoOption(command);

    if (Status == INVALID){
        return ;
    }

    // Check if the current process is a child process
    if (IamChild == RAISED){
        Help_FreeSeq(); // Free memory or resources associated with the sequence
        exit(100); // Exit the child process with status 100
    }
    // If the current process is the parent process
    else if (IamParent != RAISED){
        Help_FreeSeq(); // Free memory or resources associated with the sequence
    }

    // Log the success status and clean up
    Help_PushProcessHistory(sharedString, SUCCESS); // Log success in process history
    Help_CleanSharedString(); // Clean up the shared string memory
    IamParent = UNRAISED ; // Reset parent process status
}


void Commands_uptime(uint8* command) {
    uint8 Status = Help_RedirectionHandlerOfnoOption(command);

    if (Status == INVALID){
        return ;
    }

    // Check if the current process is a child process
    if (IamChild == RAISED){
        Help_uptimeSeq(); // Execute the uptime sequence for the child process
        exit(100); // Exit the child process with status 100
    }
    // If the current process is the parent process
    else if (IamParent != RAISED){
        Help_uptimeSeq(); // Execute the uptime sequence for the parent process
    }

    // Log the success status and clean up
    Help_PushProcessHistory(sharedString, SUCCESS); // Log success in process history
    Help_CleanSharedString(); // Clean up the shared string memory
    IamParent = UNRAISED ; // Reset parent process status
}


void Commands_PrintEnvVar(uint8* command, uint8* token) {
    // Debugging information for GDB
    uint8 NumOfoperand = 1;
    uint8* Operand[1];
    Operand[0] = token;

    char* input_Help_Redirection = (char*)malloc(BUFFER_SIZE);
    bool fileContentRead = false;  // Flag to check if content has been read
    int SecondFD = -1, SecondFDWithout2 = -1;


    // Check if there is additional input after the command
    if (token != NULL) {
        uint8* path; 

        char *found = strstr(token, "2>");  // Check if the token contains Help_Redirection to stderr
        int pos = Help_SearchOnSpaceBeforeArrow (token) ; 
        
        /* if contain > then cancel print on screen because of there are target file after > */
        if ( pos != INVALID_ID && found != NULL){
            SecondFD = STDOUT ;
            SecondFDWithout2 = STDERR ;
        }

        if (found != NULL) {
            path = Help_FindRedirectionPath(found);  // Get the path for stderr Help_Redirection
            Help_ForkAssistRedirectionExec(path, STDERR, SecondFD);  // Execute with stderr Help_Redirection
            int i = 0 ;
            while ( *(token+i) != '\0'){
                if ( *(token+i) == '>' ){
                    if (*(token+i-1) == '2'){
                        *(token+i-2) = '\0'; 
                        break;
                    }
                    else {
                        *(token+i-1) = '\0'; 
                        break;
                    }
                }
                i++;
            }
        }

        if ( pos == INVALID_ID )
            found = NULL ;
        else{
            found = strstr(token+pos, ">"); 
        }
        
        if (found != NULL) {  // Check if the token contains Help_Redirection to stdout
            path = Help_FindRedirectionPath(found);
            Help_ForkAssistRedirectionExec(path, STDOUT, SecondFDWithout2);  // Execute with stdout Help_Redirection

            int i = 0 ;
            while ( *(token+i) != '\0'){
                if ( *(token+i) == '>' ){
                    if (*(token+i-1) == '2'){
                        *(token+i-2) = '\0'; 
                        break;
                    }
                    else {
                        *(token+i-1) = '\0'; 
                        break;
                    }
                }
                i++;
            }
        }

        // Handle `<` input Help_Redirection
        found = strstr(token, "<");
        if (found != NULL) {
            path = Help_FindRedirectionPath(found);
            
            int fd = open(path, O_RDONLY);
            if (fd == INVALID_ID) {
                perror("Error opening file for input Help_Redirection");
                return ;
            }

            ssize_t length = read(fd, input_Help_Redirection, BUFFER_SIZE - 1);
            if (length == INVALID_ID) {
                perror("Error reading file for input Help_Redirection");
                close(fd);
                return ;
            }

            input_Help_Redirection[length-1] = '\0';
            close(fd);

            fileContentRead = true;
            token = input_Help_Redirection;
        }
    }

    // Child process block
    if (IamChild == RAISED) {
        // Retrieve the environment variable value
        char *path_env = getenv(token);
    
        if (path_env == NULL) {
            // If the environment variable is not found, check local variables
            uint8 status = Help_PrintLocalVariables(token);

            if (status == INVALID) {
                // Print message if variable is not found
                printf("This variable isn't existed :: %s\n", token);
                Help_PushProcessHistory(sharedString, FAILED);
            } else {
                Help_PushProcessHistory(sharedString, SUCCESS);
            }
        } else {
            // Print the environment variable value
            printf("%s = %s\n", token, path_env);
            Help_PushProcessHistory(sharedString, SUCCESS);
        }
        // Exit the child process
        exit(100);
    }
    // Parent process block
    else if (IamParent != RAISED) {
        // Retrieve the environment variable value
        char *path_env = getenv(token);
    
        if (path_env == NULL) {
            // If the environment variable is not found, check local variables
            uint8 status = Help_PrintLocalVariables(token);

            if (status == INVALID) {
                // Print message if variable is not found
                printf("This variable isn't existed :: %s\n", token);
                Help_PushProcessHistory(sharedString, FAILED);
            } else {
                Help_PushProcessHistory(sharedString, SUCCESS);
            }
        } else {
            // Print the environment variable value
            printf("%s = %s\n", token, path_env);
            Help_PushProcessHistory(sharedString, SUCCESS);
        }
    }

    // Clean up and reset parent status
    Help_CleanSharedString();
    IamParent = UNRAISED;
}


void Commands_setVariable(uint8* command) {
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
        Help_SetLocalVariable(name, value);
        // Confirm that the variable has been set
        printf("Variable set: %s=%s\n", name, value);
    } else {
        // Print an error message if the format is incorrect
        printf("Invalid variable format. Use: name=value\n");
    }
}

void Commands_allVar() {
    // Print local variables
    printf("Local Variables:\n");
    for (int i = 0; i < localVarCount; i++) {
        printf("%s=%s\n", localVariables[i].name, localVariables[i].value);
    }

    // Print environment variables
    printf("Environment Variables:\n");
    Commands_PrintEnv("allVar", NULL);
}




