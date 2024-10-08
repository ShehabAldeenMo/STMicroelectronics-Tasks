/*============================================================================
 * @file name      : Helper.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkedIn        : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * 
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ===============================*/
#include "Helper.h"

/*===========================  Local File Variables ===========================*/
/* A flag indicating whether the append mode is set for file operations.
 * CLEARED indicates the flag is not set, while a different value indicates it is set.*/
char GlobalAppendFlag           = CLEARED     ;

/* A flag indicating whether the move operation should force overwrite.
 * CLEARED indicates the flag is not set, while a different value indicates it is set.*/ 
char GlobalMoveForcedFlag       = CLEARED     ;

/* A flag indicating the current status of a move operation.
 * MOVE_FAILED indicates the operation has not started or failed, while a different value indicates a successful operation.*/
char GlobalMoveOperation        = MOVE_FAILED ;

/* A global pointer used to track the current position in the input string during parsing.
 * This pointer points to the character in the string that is currently being processed.
 */
uint8* Ptr_GlobalGetParsingPath = NULL;

/* Flag to indicate if the current process is a child process.
 * UNRAISED - Indicates that the process is not a child.
 * RAISED - Indicates that the process is a child.
 */
uint8 IamChild = UNRAISED;

/* Flag to indicate if the current process is a parent process.
 * UNRAISED - Indicates that the process is not a parent.
 * RAISED - Indicates that the process is a parent.
 */
uint8 IamParent = UNRAISED;

/* Counter for the number of local variables.
 * Keeps track of the total number of local variables currently in use.
 */
int localVarCount = 0;

// Global variables
LocalVariable localVariables[MAX_VARS]; /**< Array to store local variables */

ProcessHistory *PtrProcessHistory[MAX_STACK_SIZE]; /**< Array to store process history entries */
uint8 processCounter = 0; /**< Counter for the number of process history entries */

uint8 *sharedString = NULL; /**< Pointer to a string used for various operations in the shell */

extern char **environ; /**< Pointer to the environment variables */



uint8* Help_GetParsedPath(uint8* command) {
    // Static pointer to hold the parsed path
    static uint8* Path = NULL;  // Static to persist across function calls and avoid dangling pointer issues
    
    // Array to hold operands (not used in this function but included for completeness)
    uint8 NumOfoperand = 1;
    uint8* Operand[1];

    // Check if the global pointer for parsing paths is valid
    if (Ptr_GlobalGetParsingPath == NULL) {
        Help_MyPrintf("Error: Ptr_GlobalGetParsingPath is NULL\n");
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
        Help_MyPrintf("Error: Expected closing double quote\n");
        return NULL;
    }

    // Allocate memory for the path, considering the length of remaining string plus null terminator
    Path = malloc(strlen(Ptr_GlobalGetParsingPath) + 1);
    
    // Check if memory allocation succeeded
    if (Path == NULL) {
        Help_MyPrintf("Memory allocation failed\n");
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

void Help_MyPrintf(const char *format, ...) {
    va_list args;           // Declare a variable to hold the variable arguments
    va_start(args, format); // Initialize the va_list variable with the format argument
    vprintf(format, args); // Call vprintf to handle the formatted output
    va_end(args);           // Clean up the va_list variable
}



void Help_CleanupProcessHistory() {
    // Iterate through all entries in the stack
    for (int i = 0; i < processCounter; i++) {
        // Free the command string and the ProcessHistory entry itself
        free(PtrProcessHistory[i]->command);
        free(PtrProcessHistory[i]);
    }
}

uint8 Help_SearchOnCommand(uint8 *token) {
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

void Help_Redirect(uint8* path, int newFD) {

    if (path == NULL){
        return ;
    }

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

    // Close the file descriptor as it's no longer needed after Help_Redirection
    close(outfd);
}


void Help_ForkAssistRedirectionExec(uint8* path, int FD, int NullFD){
    // Fork the current process to create a child process
    int retPid = fork();

    if (retPid > 0) {  
        // This block executes in the parent process

        // Record the success of the command execution in process history
        Help_PushProcessHistory(sharedString, SUCCESS);  
        
        // Indicate that the parent process is currently active
        IamParent = RAISED;            
        
        // Wait for the child process to complete
        int status = 0;	
        wait(&status);
    } 
    else if (retPid == 0) {
        // This block executes in the child process

        // Redirect the file descriptor (FD) to the specified path
        Help_Redirect(path, FD);

        if ( NullFD != -1 ){
            Help_Redirect("/dev/null",NullFD);
        }
        
        // Indicate that the child process is currently active
        IamChild = RAISED;
    } 
    else {
        // Fork failed
        perror("fork");
    }
}



uint8* Help_GetRelativePath(uint8 path[]) {
    // Allocate memory for the resulting path
    uint8* ResultPath = (uint8*) malloc(MAX_PATH);
    
    // Define the base path to check against
    char keyword[] = "/home/shehabaldeen";
    
    // Check if the path contains the base path keyword
    char *found = strstr((char*)path, keyword);

    if (found == NULL) {
        // If the base path is not found, construct a relative path
        snprintf((char*)ResultPath, MAX_PATH, "%s/%s", Help_GetPathWithoutToken(), path);
    } else {
        // If the base path is found, copy the original path to the result
        strncpy((char*)ResultPath, (char*)path, MAX_PATH);
    }

    // Return the resulting path
    return ResultPath;
}


uint8* Help_FindRedirectionPath(uint8* path) {
    // Skip the initial "2>" characters, which are used for error Help_Redirection
    path += 2;

    // Tokenize the string to isolate the path after the Help_Redirection operator
    path = (uint8*) strtok((char*) path, "");

    // Set the global parsing path to the extracted path
    Ptr_GlobalGetParsingPath = path;

    // Parse the path to ensure it's in the correct format
    path = Help_GetParsedPath("path");

    // Convert the path to a relative path if necessary
    path = Help_GetRelativePath(path);

    // Return the processed path
    return path;
}

void Help_DisplaySeq(uint8* str) {
    /* Get the length of the input string */
    size_t Loc_Count = strlen(str);  // Length of the string to be written

    /* Write operation */
    ssize_t ret = write(STDOUT, str, Loc_Count);  // Write the string to standard output
    
    /* Check on return value of written bytes */
    if (ret < 0) {  // Error in writing
        perror("Error in writing on screen :: \n");  // Print error message
        Help_PushProcessHistory(sharedString, FAILED);  // Log the failure in process history
    } else if (ret < Loc_Count) {
        // Not all bytes were written; handle partial write
        fprintf(stderr, "write_to_stdout: Partial write occurred. Expected %zd, wrote %zd\n", Loc_Count, ret);
        Help_PushProcessHistory(sharedString, FAILED);  // Log the failure in process history
    } else {
        write(STDOUT, "\n", 1);  // Write a newline character to standard output
        Help_PushProcessHistory(sharedString, SUCCESS);  // Log the success in process history
    }
}

void Help_GetPathSeq() {
    /* Buffer for storing the current working directory */
    char cwd[MAX_PATH];  // Array to store the current working directory path; MAX_PATH defines its size

    /* Get the current working directory */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {  // Attempt to get the current working directory and store it in cwd
        Help_MyPrintf("Current working directory: %s\n", cwd);  // Print the current working directory if successful
        Help_PushProcessHistory(sharedString, SUCCESS);  // Record the success of the operation in process history
    } else {
        perror("getcwd() error");  // If the operation fails, print an error message
    }
}


void Help_AssistSeq (){
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("path :: Display the current working directory\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("clone  :: Copy file1 in path1 to to file2 in path2\n");
    Help_MyPrintf("clone PathOffile1,PathOffile2\n");
    Help_MyPrintf("clone PathOffile1,-a,PathOffile2\n");
    Help_MyPrintf("-> case file2 name isn't determinted, will create one with file1 name\n");
    Help_MyPrintf("-> case file2 name is given but unallocated, will create one with disred name\n");
    Help_MyPrintf("-> use -a to append to copied file\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("shift  :: move file1 in path1 to to file2 in path2\n");
    Help_MyPrintf   ("mv PathOffile1,PathOffile2\n");
    Help_MyPrintf("shift PathOffile1,-f,PathOffile2\n");
    Help_MyPrintf("-> case file2 name isn't determinted, will create one with file1 name\n");
    Help_MyPrintf("-> case file2 name is given but unallocated, will create one with disred name\n");
    Help_MyPrintf("-> use -f to overwrite on existed file\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("display :: print on shellio termial\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("cls:: clears shellio termial\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("leave :: leave shellio terminal\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
    Help_MyPrintf("-------------------------------------------------------------------------------\n");
}


void Help_TypeSeq(uint8* str){
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
        uint8 status = Help_SearchOnCommand (str);
        if (status == SUCCESS){
            printf("It is an external command :: %s\n",str);
        }
        else 
            printf("Undefined command :: %s\n",str);
    }
}


void Help_FreeSeq (){
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


void Help_uptimeSeq() {
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

uint8 Help_PrintLocalVariables(char* var) {
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


// Function to push a new entry into the process history stack
void Help_PushProcessHistory(const uint8 *command, uint8 status) {
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

int Help_SearchOnSpaceBeforeArrow(char* path) {
    int position = INVALID_ID;

    // Check if path is NULL
    if (path == NULL) {
        return position;
    }

    // Traverse the string until the null terminator
    int i = 0 ;
    while (*path != '\0') {
        // Check if current character is '>'
        if (*path == '>') {
            // Check if the previous character is '2'
            if (*(path - 1) != '2') {
                position = i;
            }
        }
        path++;  // Move to the next character
        i++;
    }

    return position;
}

uint8 Help_RedirectionHandlerOfnoOption(uint8* command){
    char *token = strtok(NULL, "");  // Tokenize the input string after the command

    /* For gdb script debugging: 
       Initialize debugging variables (not used in this function). */
    uint8 NumOfoperand = 0;
    uint8* Operand[1] = {NULL};

    /* Check if there is any additional input after the 'pwd' command */
    if (token != NULL) {
        char ErrorFlag = ON ; // to check on input options to path command is ok or not 
        uint8* path;
        int SecondFD = -1 , SecondFDWithout2 = -1 ;

        char *found = strstr(token, "2>");  // Check if the token contains Help_Redirection to stderr
        int pos = Help_SearchOnSpaceBeforeArrow (token) ; // get position of > without 2 anymore and return position 

        /* if contain > then cancel print on screen because of there are target file after > */
        if ( pos != INVALID_ID && found != NULL){
            SecondFD = STDOUT ;
            SecondFDWithout2 = STDERR ;
            ErrorFlag = OFF ;
        }

        /* Implement 2> seq */
        if (found != NULL) {
            path = Help_FindRedirectionPath(found);  // Get the path for stderr Help_Redirection
            Help_ForkAssistRedirectionExec(path, STDERR, SecondFD);  // Execute with stderr Help_Redirection
            ErrorFlag = OFF ;
        }
        
        /* if there are no position ret found to null to ensure that it willn't implement > seq */
        if ( pos == INVALID_ID )
            found = NULL ;
        else{
            /* implement > seq */
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
            return INVALID;   
        }
    }

    return VALID ;
}


uint8* Help_RedirectionHandlerOfWithOption(uint8* command) {
    char* input_Help_Redirection = (char*)malloc(BUFFER_SIZE);
    uint8* token = strdup(strtok(NULL, ""));  // Tokenize the input string to extract the argument
    int SecondFD = -1, SecondFDWithout2 = -1;
    bool fileContentRead = false;  // Flag to check if content has been read

    // For gdb script debugging
    uint8 NumOfoperand = 1;  // Number of operands (arguments) for debugging
    uint8* Operand[1];  // Array to hold the operand
    Operand[0] = strdup(token);  // Store the token (argument) in the operand array in dynamic memory

    if (token == NULL) {
        printf("command not found\nEnter (assist) to know Shellio Commands\n");
        Help_PushProcessHistory(sharedString, FAILED);  // Log the failure in process history
        Help_CleanSharedString();  // Clean up the shared string
        return NULL;
    } else {
        uint8* path;

        // Handle `2>` stderr Help_Redirection
        char *found = strstr(token, "2>");
        int pos = Help_SearchOnSpaceBeforeArrow(token);

        if (pos != INVALID_ID && found != NULL) {
            SecondFD = STDOUT;
            SecondFDWithout2 = STDERR;
        }

        if (found != NULL) {
            path = Help_FindRedirectionPath(found);
            Help_ForkAssistRedirectionExec(path, STDERR, SecondFD);
            int i = 0;
            while (*(token + i) != '\0') {
                if (*(token + i) == '>') {
                    if (*(token + i - 1) == '2') {
                        *(token + i - 2) = '\0';
                        break;
                    } else {
                        *(token + i - 1) = '\0';
                        break;
                    }
                }
                i++;
            }
        }

        if (pos == INVALID_ID)
            found = NULL;
        else
            found = strstr((Operand[0] + pos), ">");

        if (found != NULL) {  // Handle `>` stdout Help_Redirection
            path = Help_FindRedirectionPath(found);
            Help_ForkAssistRedirectionExec(path, STDOUT, SecondFDWithout2);
            int i = 0;
            while (*(token + i) != '\0') {
                if (*(token + i) == '>') {
                    if (*(token + i - 1) == '2') {
                        *(token + i - 2) = '\0';
                        break;
                    } else {
                        *(token + i - 1) = '\0';
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
                return NULL;
            }

            ssize_t length = read(fd, input_Help_Redirection, BUFFER_SIZE - 1);
            if (length == INVALID_ID) {
                perror("Error reading file for input Help_Redirection");
                close(fd);
                return NULL;
            }

            input_Help_Redirection[length-1] = '\0';
            close(fd);

            fileContentRead = true;
            token = input_Help_Redirection;
        }
    }

    free(Operand[0]);
    return token;
}


// Function to free the memory allocated for the global sharedString
void Help_CleanSharedString() {
    // Free the allocated memory for sharedString
    free(sharedString);

    // After freeing, ensure sharedString is set to NULL to avoid dangling pointers
    sharedString = NULL;
}

// Handle input Help_Redirection
uint8* Help_HandleOptionRedirection(const char *input, const char* delimiters) {
    char *File = strstr(input, delimiters);
    if (File) {
        /* ============================ */
        File = Help_FindRedirectionPath(File);
        return File;
    }
    return NULL;
}

// Function to wait for child processes
void wait_for_children(int num_children, pid_t pids[]) {
    for (int i = 0; i < num_children; i++) {
        waitpid(pids[i], NULL, 0);
    }
}


// Function to parse the input command into multiple commands separated by '|'
char Help_ParseCommand(char *input, char **commands) {
    char num_commands = 0;
    
    // Ensure the input and commands array are not NULL
    if (input == NULL || commands == NULL) {
        fprintf(stderr, "Error: NULL pointer passed to Help_TrimSpaces.\n");
        return 0;
    }

    char *command = strtok(input, "|");
    while (command != NULL) {
        // Trim leading and trailing spaces from the command
        while (*command == ' ') command++;
        char *end = command + strlen(command) - 1;
        while (end > command && *end == ' ') end--;
        *(end + 1) = '\0';

        // Check if we have enough space in the commands array
        if (num_commands >= MAX_COMMANDS) {
            fprintf(stderr, "Error: Too many commands (max %d).\n", MAX_COMMANDS);
            return num_commands;
        }

        // Allocate memory for the command and store it
        commands[num_commands] = strdup(command);
        if (commands[num_commands] == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        num_commands++;

        // Get the next command
        command = strtok(NULL, "|");
    }

    return num_commands;
}

void Help_TrimSpaces(char *str) {
    char *start = str;
    char *end = str + strlen(str) - 1;

    // Trim leading spaces
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // Trim trailing spaces
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    // Write the trimmed string back to the original buffer
    if (start != str) {
        memmove(str, start, end - start + 1);
    }
    str[end - start + 1] = '\0';
}


// Function to get the username of the current user
const char* Help_GetUserName() {
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
void Help_GetHostName(char *hostname, size_t size) {
    // Get the hostname and check for errors
    if (gethostname(hostname, size) == -1) {
        perror("gethostname"); // Print an error message if gethostname fails
        strcpy(hostname, "unknown"); // Set hostname to "unknown" if an error occurs
    }
}

// Function to get the current working directory without a token
uint8* Help_GetPathWithoutToken() {
    // Static buffer to store the current working directory
    static char cwd[MAX_PATH];

    // Retrieve the current working directory
    getcwd(cwd, sizeof(cwd));
    return cwd; // Return the buffer containing the current working directory
}

// Function to set the global sharedString to a duplicate of the input string
void Help_SetSharedString(const uint8 *str) {
    // Duplicate the input string and assign it to sharedString
    sharedString = strdup(str);

    // Note: Ensure to handle NULL inputs or check for memory allocation failures
}

void Help_SetLocalVariable(const char* name, const char* value) {
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
