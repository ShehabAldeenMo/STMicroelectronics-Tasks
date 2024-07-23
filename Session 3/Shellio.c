/*===================================  Includes ==========================================*/
#include "Shellio.h"

/*===================================  Main Code ==========================================*/
int main() {
    /* Initiliazations */
    char str[MAXSIZE] = {0} ;          // buffer of input command
    char *delimiters = " 0";           // Delimiters are space, comma, period, and exclamation mark
    char *token ;                      // to store each word into string
    char *exit = "exit" ;              // exit keyword to left shellio
    char ExitFlag = NOT_EXIT ;         // flag to ensure that the user wants to exit
    char *clear = "clear";             // clear keyword to clear shellio screen
    char ClearFlag = NOT_CLEARED ;     // flag to ensure that the user wants to exit
    char ArgCounter = 0 ;              // counter of input arguments to shellio

    do {
        /* Enter your commend */
        printf("Shellio > ");
        
        // Use fgets to read a whole line of input including spaces
        if (fgets(str, MAXSIZE, stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        // Remove trailing newline character, if present
        size_t len = strlen(str);
        if (len > 0 && str[len-1] == '\n') {
            str[len-1] = '\0';
        }

        // Initial call to strtok
        token = strtok(str, delimiters);

        // Subsequent calls to strtok
        while (token != NULL) {
            ExitFlag  = strcmp (token,exit);
            ClearFlag = strcmp (token,clear);
            token = strtok(NULL, delimiters);
            ArgCounter++;
        }

        // print goodby with exit command then exit
        if (ExitFlag == EXIT && ArgCounter == 1){
            printf("Good Bye\n");
            break ;
        }
        // clear screen with clear command
        else if (ClearFlag == CLEARED && ArgCounter == 1){
            system("clear");
        }
        else {
           // printf("%s\n",str);
        }
        
        // clear string buffer and counter of arguments
        memset (str,0, sizeof(str) );
        ArgCounter = 0 ;

    }while(1);

    return 0;
}
