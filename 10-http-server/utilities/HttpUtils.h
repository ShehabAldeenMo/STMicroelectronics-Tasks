/*
 * File Name        : HttpUtils.h
 * Description      : Helper functions for common tasks such as string manipulation and response formatting.
 * Functions        :
 *                    - formatHttpResponse: Generates an HTTP response string based on the provided status, headers, and body.
 *                    - parseRequestLine: Extracts and processes the HTTP request method, path, and version from the request line.
 *                    - isCgiFile: Determines if the requested file is a CGI script based on its extension.
 * Notes            : These utility functions simplify repetitive tasks and improve code readability.
 */

/*===================================  Includes ==============================*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <asm-generic/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <sys/sysmacros.h>

#include "ServerConfig.h"

/*================================  Definations ==============================*/
#define FALSE -1
#define SUCESS 0
#define MAX_ELEMENTS 255
#define MAX_PATH_LENGTH 1024
#define MAX_ARGUMENT 10

/*=================================  Prototypes ==============================*/
int SetServerSocket(char *port);
void Handle_Requests(int cfd);
void CloseFd(int fd);
void ParsePath(const char *buffer, char *path, size_t pathSize);
void ListContent(int cfd, char *path);
int read_directory(const char *dir, char *Elements[], size_t *elementCount);
void cleanup(char *Elements[], size_t elementCount);
void ErrorResponse(int cfd, int code, char *message);
void FileOperation(int cfd, char *path);
void CatFile(int cfd, char *path);
void ExecuteFile(int cfd, char *path);