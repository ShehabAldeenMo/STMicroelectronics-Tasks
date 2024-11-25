/*
 * File Name        : HttpServer.c
 * Description      : Implements server setup, request parsing, and response handling.
 * Functions        :
 *                    - initializeServer: Initializes the server socket, binds to a port, and begins listening for connections.
 *                    - acceptConnection: Accepts incoming client connections.
 *                    - handleRequest: Reads and parses HTTP requests, calls appropriate request handlers, and sends responses.
 *                    - handleClientFork: Forks a new process to process a client request independently.
 * Notes            : This file contains the implementation of the server's main functionality and integrates other modules.
 */

/*===================================  Includes ==============================*/
#include "HttpServer.h"

/*==================================  Core Main ==============================*/

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // open file descriptor for server and client
    int server_fd, client_fd;
    server_fd = SetServerSocket(argv[1]);

    for (;;)
    { // Handle client connections iteratively
        printf("Waiting for client connection ...\n");

        // Accept a connection. the listening socket ('server_fd') remains open
        //   and can be used to accept further connections.
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == FALSE)
        {
            printf("SERVER: accept failed (%s)\n", strerror(errno));
            continue;
        }

        printf("Client connected\n");

        // Fork the current process to create a child process to handle concurrent requests
        int ret_pid = fork();

        // differ the paths of executation for child or parent process or error in fork()
        if (ret_pid < 0)
        {
            // Fork failed
            perror("fork");
            close(client_fd);
            continue;
        }
        // child process
        else if (ret_pid == 0)
        {
            CloseFd(server_fd);         // there's no need to open fd of server in child process
            Handle_Requests(client_fd); // handle requests of client
            CloseFd(client_fd);         // close fd of client after served

            getchar();

            exit(SUCESS); // exit from child process
        }
        else
        {
            CloseFd(client_fd); // close fd of client after served

            // Wait for the child process to complete with no blocking state for parent
            // wait function syscall ensure to flush the childern process after termination to
            // avoid to be zombies
            int status = 0;
            waitpid(-1, &status, WNOHANG);
        }
    }

    return 0;
}