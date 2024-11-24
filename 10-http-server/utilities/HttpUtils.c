/*
 * File Name        : HttpUtils.c
 * Description      : Implements utility functions for string manipulation, request parsing, and response formatting.
 * Functions        :
 *                    - formatHttpResponse: Creates a formatted HTTP response string.
 *                    - parseRequestLine: Splits the HTTP request line and validates its components.
 *                    - isCgiFile: Checks if a file is a CGI script based on its extension (e.g., ".cgi").
 * Notes            : This file provides reusable helper functions to support core server logic.
 */

/*===================================  Includes ==============================*/
#include "HttpUtils.h"

/*============================  Function Implementation =======================*/
int SetServerSocket(char *port)
{
    int opt = 1;
    struct sockaddr_in svaddr;
    int server_fd;
    ssize_t numBytes;
    char buf[SERVER_BUF];

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("SERVER: socket create failed (%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    /* Construct server socket address, bind socket to it,
       and make this a listening socket */
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = INADDR_ANY; // bind to any NIC on this machine
    svaddr.sin_port = htons(atoi(port));

    if (bind(server_fd, (struct sockaddr *)&svaddr, sizeof(svaddr)) == FALSE)
    {
        printf("SERVER: bind failed (%s)\n", strerror(errno));
        exit(1);
    }

    if (listen(server_fd, BACKLOG) == FALSE)
    {
        printf("SERVER: listen failed (%s)\n", strerror(errno));
        exit(1);
    }

    // connect to port
    printf("Server listening on port: %d\n", atoi(port));

    return server_fd;
}

void Handle_Requests(int cfd)
{
    char RecBuf[SERVER_BUF] = {0};
    if ((read(cfd, RecBuf, sizeof(RecBuf) - 1)) < 0)
    {
        perror("Failed to read from client");
        close(cfd);
        exit(FALSE);
    }
    printf("Requested:\n%s\n\n", RecBuf);
}

void CloseFd(int fd)
{
    if (close(fd) == FALSE)
    {
        printf("SERVER: close failed (%s)\n", strerror(errno));
        exit(FALSE);
    }
}