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
    uint8 *token ;                     // to store each word into string

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
            str[len-1] = '\0';
        }

        /* Initial call to strtok */ 
        token = strtok(str, delimiters);

        if (token != NULL){
            /* print goodby then exit */ 
            if (strcmp (token,"exit") == EQUALED){
                uint8 status = Shellio_Exit();

                /* exit if it successed state */
                if (status == SUCCESS ){
                    break ;
                }
            }
            /* clear screen with clear command */ 
            else if (strcmp (token,"clear") == CLEARED){
                Shellio_Clear();
            }
            /* get absolute path of current working directory */
            else if (strcmp (token,"pwd") == PWD_PASS ){
                Shellio_GetPath();
            }
            else if (strcmp (token,"echo") == ECHO_PASS ){
                Shellio_EchoInput();
            }
            else if (strcmp (token,"help") == HELP_PASS){
                Shellio_Help ();
            }
            else if (strcmp (token,"cp") == COPY_PASS ){
                Shellio_Copy ();
            }
            else if (strcmp (token,"mv") == MV_PASS ){
                Shellio_MoveFile (strcmp(token,"mv"));
                Shellio_Copy ();
            }
            else {
                printf("command not found\nEnter (help) to know Shellio Commands\n");
            }
        }

        /* clear all flags and buffers */
        memset (str,0, sizeof(str) );

    }while(1);

    return 0;
}

