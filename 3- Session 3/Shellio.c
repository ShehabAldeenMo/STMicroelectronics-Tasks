/*============================================================================
 * @file name      : Shellio.c
 * @Author         : Shehab aldeen mohammed
 * Github          : https://github.com/ShehabAldeenMo
 * LinkdIn         : https://www.linkedin.com/in/shehab-aldeen-mohammed/
 *
 =============================================================================
 * @Notes:
 * Shellio is a simple, custom command-line shell designed to interact with users through a terminal interface. 
 * It provides a basic environment for entering and processing commands. Few commands: pwd, cp, mv, exit, clear,
 * echo.
 ******************************************************************************
 ==============================================================================
*/

/*===================================  Includes ===============================*/
#include "Shellio.h"

/*===================================  Main Code ==============================*/
int main() {
    /* Initiliazations */
    char str[MAXSIZE] = {0} ;          // buffer of input command
    char *delimiters = " 0";           // Delimiters are space, comma, period, and exclamation mark
    char *token ;                      // to store each word into string
    char ArgCounter = 0 ;              // counter of input arguments to shellio to check on the number of arguments
    uint8 Arguments[MAX_ARGUMENTS][MAX_CHARACHTERS_OF_ONE_ARGUMENTS] 
                    = {0} ;            // store commands
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
        ArgCounter = 0;

        /* Subsequent calls to strtok */ 
        while (token != NULL && ArgCounter < MAX_ARGUMENTS && strcmp (token,"echo") != ECHO_PASS 
            && strcmp (token,"cp") != COPY_PASS && strcmp (token,"mv") != MV_PASS  ) {
            strcpy(Arguments[ArgCounter],token);    // to store token in this buffer
            token = strtok(NULL, delimiters);       // to continue tokenizing from the previous point
            ArgCounter++;                           // to count number of arguments
        }


        /* To check on commands with one argument */
        if (ArgCounter == 1){
            /* print goodby then exit */ 
            if (strcmp (Arguments[FIRST_ARGUMENT],"exit") == EXIT){
                printf("Good Bye\n");
                break ;
            }
            /* clear screen with clear command */ 
            else if (strcmp (Arguments[FIRST_ARGUMENT],"clear") == CLEARED){
                system("clear");
            }
            /* get absolute path of current working directory */
            else if (strcmp (Arguments[FIRST_ARGUMENT],"pwd") == PWD_PASS ){
                Shellio_GetPath();
            }
            else if (strcmp (Arguments[FIRST_ARGUMENT],"help") == HELP_PASS){
                Shellio_Help ();
            }
            else {
                HelpCommand = RAISED ;
            }
        }
        /* Any other case */
        else if ( NewLineOperator != NEWLINE_INPUT) {
            /* Check on echo command */
            if (strcmp(token,"echo") == ECHO_PASS ){
                token = strtok(NULL, "");       // to continue tokenizing from the previous point
                if ( token != NULL) {
                    Shellio_EchoInput(token);
                }
                else {
                    HelpCommand = RAISED ;
                }
            }
            else if (strcmp(token,"cp") == COPY_PASS || strcmp(token,"mv") == MV_PASS) {
                Shellio_MoveFile (strcmp(token,"mv"));

                /* mv or cp command*/
                token = strtok (NULL , "");
                uint8* Option = Shellio_ParsingPath(&ArgCounter,Arguments[SECOND_ARGUMENT], Arguments[THIRD_ARGUMENT] , token);
                
                /* copy function call */
                if (ArgCounter ==  ( MAX_ARGUMENTS -1 )  ){
                    Shellio_CopyFile (Arguments[SECOND_ARGUMENT],Arguments[THIRD_ARGUMENT]);  
                }
                else if (ArgCounter == MAX_ARGUMENTS ){
                    if (Option == NULL){
                        printf ("Error In Passing Option\n");
                    }
                    char Status = Shellio_FileOption (Option);  // to lift our static flag
                    
                    if (Status == VALID){
                        Shellio_CopyFile (Arguments[SECOND_ARGUMENT],Arguments[THIRD_ARGUMENT]); // to make our option 
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
        memset(Arguments, 0, sizeof(Arguments));
        ArgCounter = 0 ;
        HelpCommand = CLEARED ; 

    }while(1);

    return 0;
}
