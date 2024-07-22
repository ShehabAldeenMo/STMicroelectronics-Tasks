#include <stdio.h>
#include <string.h>

#define MAXSIZE     200
#define EXIT         0
#define NOT_EXIT     1


int main() {
    char str[MAXSIZE] = {0} ;   // buffer of input command
    char *delimiters = " ,.!0"; // Delimiters are space, comma, period, and exclamation mark
    char *token ;               // to store each word into string
    char *exit = "exit" ;       // exit keyword to left shellio
    char ExitFlag = NOT_EXIT ;  // flag to ensure that the user wants to exit

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
            ExitFlag = strcmp (token,exit);
            printf("%s\n", token);
            token = strtok(NULL, delimiters);
        }

        // clear string buffer
        memset (str,0,sizeof(str)-1);

    }while(ExitFlag != EXIT);

    return 0;
}
