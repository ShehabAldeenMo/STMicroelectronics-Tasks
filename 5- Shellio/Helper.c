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



uint8* GetParsedPath(uint8* command) {
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

void my_printf(const char *format, ...) {
    va_list args;           // Declare a variable to hold the variable arguments
    va_start(args, format); // Initialize the va_list variable with the format argument
    vprintf(format, args); // Call vprintf to handle the formatted output
    va_end(args);           // Clean up the va_list variable
}



void cleanupProcessHistory() {
    // Iterate through all entries in the stack
    for (int i = 0; i < processCounter; i++) {
        // Free the command string and the ProcessHistory entry itself
        free(PtrProcessHistory[i]->command);
        free(PtrProcessHistory[i]);
    }
}

uint8 SearchOnCommand(uint8 *token) {
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

uint8 searchCharacter(const uint8 *str, char character, uint8** Remaining) {
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

void redirect(uint8* path, int newFD) {
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


void fork_redirectionExec(uint8* path, int FD) {
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



uint8* GetRelativePath(uint8 path[]) {
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


uint8* ErrorFD_Path(uint8* path) {
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

void DisplaySeq(uint8* str) {
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

void GetPathSeq() {
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


void Help_Seq (){
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


void TypeSeq(uint8* str){
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


void FreeSeq (){
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


void uptimeSeq() {
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

uint8 printLocalVariables(char* var) {
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

// Function to get the current working directory without a token
uint8* GetPathWithoutToken() {
    // Static buffer to store the current working directory
    static char cwd[MAX_PATH];

    // Retrieve the current working directory
    getcwd(cwd, sizeof(cwd));
    return cwd; // Return the buffer containing the current working directory
}