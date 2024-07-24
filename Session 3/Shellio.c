/*===================================  Includes ==========================================*/
#include "Shellio.h"

/*===================================  Main Code ==========================================*/
int main() {
    /* Initiliazations */
    char str[MAXSIZE] = {0} ;          // buffer of input command
    char *delimiters = " 0";           // Delimiters are space, comma, period, and exclamation mark
    char *token ;                      // to store each word into string
    char *exit = "exit" ;              // exit keyword to left shellio
    char *clear = "clear";             // clear keyword to clear shellio screen
    char ArgCounter = 0 ;              // counter of input arguments to shellio to check on the number of arguments
    uint8 Arguments[MAX_ARGUMENTS][MAX_CHARACHTERS_OF_ONE_ARGUMENTS] 
                    = {0} ;            // store commands
    char *pwd = "pwd" ;                // get absolute current working directory
    char *echo = "echo" ;              // to store echo keyword that used in check on conditions
    char *cp = "cp" ;                  // to store copy command
    char HelpCommand = CLEARED ;       // to ensure that help statment display or not
    char NewLineOperator = 0 ;         // Use it to check about newline command


    do {
        /* Enter your commend */
        printf("Shellio > ");
        
        /* Use fgets to read a whole line of input including spaces */ 
        if (fgets(str, MAXSIZE, stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        /* Remove trailing newline character, if present */ 
        size_t len = strlen(str);
        if (len > 0 && str[len-1] == '\n') {
            NewLineOperator = len + str[0] ;
            str[len-1] = '\0';
        }

        /* Initial call to strtok */ 
        token = strtok(str, delimiters);

        /* Subsequent calls to strtok */ 
        while (token != NULL && ArgCounter < 3 && strcmp (token,echo) != ECHO_PASS 
            && strcmp (token,cp) != COPY_VALID ) {
            strcpy(Arguments[ArgCounter],token);    // to store token in this buffer
            token = strtok(NULL, delimiters);       // to continue tokenizing from the previous point
            ArgCounter++;                           // to count number of arguments
        }

        /* To check on commands with one argument */
        if (ArgCounter == 1){
            /* print goodby then exit */ 
            if (strcmp (Arguments[FIRST_ARGUMENT],exit) == EXIT){
                printf("Good Bye\n");
                break ;
            }
            /* clear screen with clear command */ 
            else if (strcmp (Arguments[FIRST_ARGUMENT],clear) == CLEARED){
                system("clear");
            }
            /* get absolute path of current working directory */
            else if (strcmp (Arguments[FIRST_ARGUMENT],pwd) == PWD_CORRECT ){
                Shellio_GetPath();
            }
            else {
                HelpCommand = RAISED ;
            }
        }
        /* Any other case */
        else if ( NewLineOperator != NEWLINE_INPUT) {
            /* Check on echo command */
            if (strcmp(token,echo) == ECHO_PASS ){
                token = strtok(NULL, "");       // to continue tokenizing from the previous point
                if ( token != NULL) {
                    Shellio_EchoInput(token);
                }
                else {
                    HelpCommand = RAISED ;
                }
            }
            else if (strcmp(token,cp) == COPY_VALID){
                /* Handle first path */
                while (token != NULL){
                    ArgCounter++;
                    token = strtok(NULL, ",");       // to continue tokenizing from the previous point

                    if (token != NULL){
                        strcpy(Arguments[ArgCounter],token);
                        strncpy(Arguments[ArgCounter],token, MAX_CHARACHTERS_OF_ONE_ARGUMENTS - 1) ;
                        Arguments[ArgCounter][MAX_CHARACHTERS_OF_ONE_ARGUMENTS - 1] = '\0'; // ensure null termination
                    }
                }

                /* copy function call */
                if (ArgCounter == 3 ){
                    if (strcmp(Arguments[SECOND_ARGUMENT],Arguments[THIRD_ARGUMENT])) {
                        Shellio_CopyFile (Arguments[SECOND_ARGUMENT],Arguments[THIRD_ARGUMENT]);    
                    }
                    else {
                        printf("Impossible to creat new file in the same directory with the same src file name\n");
                    }
                }
                /* To print help if number of arguments aren't equall 3 */
                else {
                    HelpCommand = RAISED ;
                }                
            }
            /* if it's not echo command or newline. means want help */
            else { 
                    HelpCommand = RAISED ;
            }
        }
                        
        /* If the input isn't equal '\n' and not a standard shellio command will print help statment */
        if (HelpCommand) {
           printf("command not found\nEnter (help) to know Shellio Commands\n");
        }
        
        /* clear all flags and buffers */
        memset (str,0, sizeof(str) );
        ArgCounter = 0 ;
        HelpCommand = CLEARED ; 

    }while(1);

    return 0;
}


/*

    cp /home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt,/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt
 */