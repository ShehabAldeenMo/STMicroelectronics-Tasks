#ifndef COMMANDS_H_
#define COMMANDS_H_

/*===================================  Includes ==========================================*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

/*==================================  Definations =========================================*/
#define PATH_MAX              1024
#define MAX_COPIED_CONTENT    1024
#define EXIST                  0
#define MAX_FILE_NAME         256

typedef unsigned char uint8;

/*===============================  Functions Prototypes ==================================*/
void Shellio_GetPath();
void Shellio_EchoInput(const char* Copy_Statment);
void Shellio_CopyFile (const char* Copy_1st_Path,const char* Copy_2nd_Path );

#endif /* Header Gaurd */