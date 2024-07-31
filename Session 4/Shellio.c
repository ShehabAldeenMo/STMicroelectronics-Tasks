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

        // to share this input unto history
        setSharedString(str);

        /* Initial call to strtok */ 
        token = strtok(str, delimiters);

        if (token != NULL){
            /* print goodby then exit */ 
            if (strcmp (token,"leave") == EQUALED){
                uint8 status = Shellio_Exit();

                /* exit if it successed state */
                if (status == SUCCESS ){
                    break ;
                }
            }
            /* clear screen with clear command */ 
            else if (strcmp (token,"cls") == CLEARED){
                Shellio_Clear();
            }
            /* get absolute path of current working directory */
            else if (strcmp (token,"path") == PWD_PASS ){
                Shellio_GetPath();
            }
            else if (strcmp (token,"display") == ECHO_PASS ){
                Shellio_EchoInput();
            }
            else if (strcmp (token,"assist") == HELP_PASS){
                Shellio_Help ();
            }
            else if (strcmp (token,"clone") == COPY_PASS ){
                Shellio_Copy ();
            }
            else if (strcmp (token,"shift") == MV_PASS ){
                Shellio_MoveFile (strcmp(token,"shift"));
                Shellio_Copy ();
            }
            else if (strcmp(token, "cd") == CD_PASS) {
                Shellio_ChangeDir();
            } 
            else if (strcmp(token, "type") == TYPE_PASS) {
                Shellio_TypeCommand();
            } 
            else if (strcmp(token, "envir") == ENV_PASS) {
                /* check on arguments */
                token = strtok(NULL, delimiters);

                if (token == NULL) {
                    Shellio_PrintEnv();
                } else {
                    Shellio_PrintEnvVar(token);
                }
            } 
            else if (strcmp(token, "phist") == EXIT) {
                Shellio_Phist();
            }
            else {
                Shellio_ExecExternalCommands(token);
            }
        }

        /* clear all flags and buffers */
        memset (str,0, sizeof(str) );

    }while(1);

    return 0;
}




/*

1- Source file isn't correct
cp "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/Shehab.txt" "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt"

2- Same sourc and destination
cp "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt"

3- Normally copy
cp "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/copiedfile.txt"
cp "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3"

4- Wrong destination
cp "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Wrong/hgv"

5- Append
cp "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt" -a "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/copiedfile.txt"

6- Wrong option
cp "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt" -f "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/copiedfile.txt"

7- mv with different directory
mv "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/file.txt" "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/file.txt"
 
8- mv with forced option on file
    mv "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/copiedfile.txt" -f "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/file.txt"

9- mv with no forced option on file
mv "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/Session 3/fileMove.txt" "/home/shehab/Desktop/Embedded Linux Track/STMicroelectronics Internship/STMicroelectronics-Tasks/file.txt"


*/