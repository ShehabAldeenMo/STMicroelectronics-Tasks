#ifndef COMMANDS_H_
#define COMMANDS_H_

/*===================================  Includes ==========================================*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*==================================  Definations =========================================*/
#define PATH_MAX              1024
#define MAX_COPIED_CONTENT    1024

typedef unsigned char uint8;

/*===============================  Functions Prototypes ==================================*/
void Shellio_GetPath();
void Shellio_EchoInput(const char* Copy_Statment);
void Shellio_CopyFile (const char* Copy_1st_Path,const char* Copy_2nd_Path );

#endif /* Header Gaurd */