#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <asm-generic/socket.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void send_error(int client_fd, int code, const char *message)
{
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: text/html\r\n\r\n"
             "<html><body><h1>%d %s</h1></body></html>",
             code, message, code, message);
    ssize_t bytes_sent = write(client_fd, response, strlen(response));
    if (bytes_sent < 0)
    {
        perror("Write system call failed");
    }
}

int ServerSetup()
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", PORT);
    return server_fd;
}

void list_directory(int client_fd, const char *path)
{
    DIR *dir = opendir(path);
    if (!dir)
    {
        send_error(client_fd, 500, "Internal Server Error");
        return;
    }

    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n\r\n"
             "<html><body><h1>Directory Listing</h1><ul>");
    ssize_t bytes_sent = write(client_fd, response, strlen(response));
    if (bytes_sent < 0)
    {
        perror("Write system call failed");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(response, sizeof(response), "<li><a href=\"%s\">%s</a></li>", entry->d_name, entry->d_name);
        bytes_sent = write(client_fd, response, strlen(response));
        if (bytes_sent < 0)
        {
            perror("Write system call failed");
        }
    }

    snprintf(response, sizeof(response), "</ul></body></html>");
    bytes_sent = write(client_fd, response, strlen(response));
    if (bytes_sent < 0)
    {
        perror("Write system call failed");
    }
    closedir(dir);
}

void send_file(int client_fd, const char *path)
{
    struct stat statbuf;

    // Check if the file exists and get its details
    if (stat(path, &statbuf) < 0)
    {
        send_error(client_fd, 404, "Requested file is Not Found");
        return;
    }

    // If it's a CGI file, execute it using execv
    if (strstr(path, ".cgi") != NULL)
    {
        // Set up a pipe to capture the CGI output
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            send_error(client_fd, 500, "Internal Server Error");
            return;
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            send_error(client_fd, 500, "Internal Server Error");
            return;
        }

        if (pid == 0)
        { // Child process: execute the CGI script
            // Close the write-end of the pipe in the child process
            close(pipefd[0]);

            // Redirect the output of the CGI script to the pipe
            dup2(pipefd[1], STDOUT_FILENO);

            // Execute the CGI script (provide absolute path if needed)
            char *argv[] = {(char *)path, NULL}; // The path to the CGI script
            execv(path, argv);

            // If execv fails
            perror("execv failed");
            exit(1);
        }
        else
        { // Parent process: read from pipe and send to client
            // Close the write-end of the pipe in the parent process
            close(pipefd[1]);

            // Send HTTP headers to the client
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
            ssize_t bytes_sent = write(client_fd, response, strlen(response));
            if (bytes_sent < 0)
            {
                perror("Write system call failed");
            }

            // Read from the pipe and send to the client
            char buffer[BUFFER_SIZE];
            ssize_t bytes_read;
            while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
            {
                bytes_sent = write(client_fd, buffer, bytes_read);
                if (bytes_sent < 0)
                {
                    perror("Write system call failed");
                }
            }

            close(pipefd[0]);
        }
    }
    // If it's a regular file, serve it
    else
    {
        int file_fd = open(path, O_RDONLY);
        if (file_fd < 0)
        {
            send_error(client_fd, 500, "Internal Server Error");
            return;
        }

        // Send HTTP headers
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
        ssize_t bytes_sent = write(client_fd, response, strlen(response));
        if (bytes_sent < 0)
        {
            perror("Write system call failed");
        }

        // Read and send the file content
        ssize_t bytes;
        while ((bytes = read(file_fd, response, sizeof(response))) > 0)
        {
            bytes_sent = write(client_fd, response, bytes);
            if (bytes_sent < 0)
            {
                perror("Write system call failed");
            }
        }

        close(file_fd);
    }
}

void handle_client(int client_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0)
    {
        perror("Failed to read from client");
        close(client_fd);
        return;
    }

    printf("Request:\n%s\n", buffer);
    printf("================================================================================\n");
    // Process the request
    // Parse the request to get the resource path
    char method[16], path[256], protocol[16];
    sscanf(buffer, "%s %s %s", method, path, protocol);

    // Handle based on the resource type

    // Strip leading "/"
    if (path[0] == '/')
        memmove(path, path + 1, strlen(path));
    if (strlen(path) == 0)
        strcpy(path, ".");

    struct stat file_stat;
    if (stat(path, &file_stat) == -1)
    {
        send_error(client_fd, 404, "Requested file is Not Found");
    }
    else if (S_ISDIR(file_stat.st_mode))
    {
        list_directory(client_fd, path);
    }
    else if (S_ISREG(file_stat.st_mode))
    {
        send_file(client_fd, path);
    }
    else
    {
        send_error(client_fd, 403, "Forbidden");
    }

    close(client_fd);

    // getchar();
}

int main()
{
    int server_fd = ServerSetup();
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    for (;;)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            close(client_fd);
            continue;
        }

        /* Child Process */
        if (pid == 0)
        {
            close(server_fd);
            handle_client(client_fd);
            close(client_fd);
            exit(0);
        }

        /* Parent Process */
        else
        {
            close(client_fd);

            // Wait for child to finish
            int status;
            // waitpid(pid, &status, WNOHANG); // Check for terminated child processes without blocking
            while (waitpid(-1, &status, WNOHANG) > 0)
            {
                // Handle child termination (you can log the exit status here if needed)
            }
        }
    }

    close(server_fd);
    return 0;
}
