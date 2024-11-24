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
    char path[_1K / 4];
    struct stat sb;

    if ((read(cfd, RecBuf, sizeof(RecBuf) - 1)) < 0)
    {
        perror("Failed to read from client");
        close(cfd);
        exit(FALSE);
    }

    ParsePath(RecBuf, path, sizeof(path));
    printf("path: %s\n\n", path);

    if (lstat(path, &sb) == FALSE)
    {
        ErrorResponse(cfd, 404, "Requested file is Not Found");
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    if (strlen(path) == 0)
        strcpy(path, ".");

    if (S_ISDIR(sb.st_mode))
    {
        ListContent(cfd, path);
    }
    else if (S_ISREG(sb.st_mode))
    {
        FileOperation(cfd, path);
    }
    else
    {
        ErrorResponse(cfd, 403, "Forbidden");
    }
}

void CloseFd(int fd)
{
    if (close(fd) == FALSE)
    {
        printf("SERVER: close failed (%s)\n", strerror(errno));
        exit(FALSE);
    }
}

void ParsePath(const char *buffer, char *path, size_t pathSize)
{
    // Ensure the inputs are valid
    if (buffer == NULL || path == NULL)
    {
        fprintf(stderr, "Invalid input to ParsePath function\n");
        return;
    }

    // Find the first space (after the HTTP method)
    const char *methodEnd = strchr(buffer, ' ');
    if (methodEnd == NULL)
    {
        fprintf(stderr, "Invalid HTTP request format\n");
        return;
    }

    // Find the second space (end of the path)
    const char *pathEnd = strchr(methodEnd + 1, ' ');
    if (pathEnd == NULL)
    {
        fprintf(stderr, "Invalid HTTP request format\n");
        return;
    }

    // Calculate the length of the path and copy it to the output
    size_t pathLength = pathEnd - (methodEnd + 1);
    if (pathLength >= pathSize)
    {
        fprintf(stderr, "Path buffer too small\n");
        return;
    }

    strncpy(path, methodEnd + 1, pathLength);
    path[pathLength] = '\0'; // Null-terminate the string
}

void ListContent(int cfd, char *path)
{
    char *Elements[MAX_ELEMENTS] = {NULL}; // Array to store filenames
    size_t ElementNumber = 0;              // Counter for the number of elements
    char response[_1K];

    // Read directory entries and store filenames in Elements
    if (read_directory(path, Elements, &ElementNumber) != SUCESS)
    {
        ErrorResponse(cfd, 500, "Error in Reading Content");
        printf("Error in Reading Content\n"); // Close the directory stream on error
        exit(EXIT_FAILURE);
    }

    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n\r\n"
             "<html><body><h1>Directory Listing</h1><ul>");
    write(cfd, response, strlen(response));

    // Process and print information for each file
    for (size_t i = 0; i < ElementNumber; ++i)
    {
        snprintf(response, sizeof(response), "<li><a href=\"%s\">%s</a></li>", Elements[i], Elements[i]);
        write(cfd, response, strlen(response));
    }

    // Cleanup
    cleanup(Elements, ElementNumber);
}

int read_directory(const char *path, char *Elements[], size_t *elementCount)
{
    DIR *dp = opendir(path); // Open directory
    if (dp == NULL)
    {
        return FALSE; // Failed to open directory
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL)
    {
        // Check if the number of elements has reached the maximum limit
        if (*elementCount >= MAX_ELEMENTS)
        {
            fprintf(stderr, "Too many elements\n");
            closedir(dp); // Close directory before returning
            return FALSE;
        }

        // Allocate memory for storing the file name
        Elements[*elementCount] = malloc(strlen(entry->d_name) + 1);
        if (Elements[*elementCount] == NULL)
        {
            perror("malloc error");
            closedir(dp); // Close directory before returning
            return -1;
        }
        strcpy(Elements[*elementCount], entry->d_name); // Copy filename
        (*elementCount)++;
    }

    closedir(dp);  // Close directory
    return SUCESS; // Success
}

void cleanup(char *Elements[], size_t elementCount)
{
    for (size_t i = 0; i < elementCount; ++i)
    {
        free(Elements[i]); // Free allocated memory for each filename
    }
}

void ErrorResponse(int cfd, int code, char *message)
{
    char response[_1K];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: text/html\r\n\r\n"
             "<html><body><h1>%d %s</h1></body></html>",
             code, message, code, message);
    write(cfd, response, strlen(response));
}

void FileOperation(int cfd, char *path)
{
    char *ret = strstr(path, ".cgi");
    if (ret != NULL)
    {
        ExecuteFile(cfd, path);
    }
    else
    {
        CatFile(cfd, path);
    }
}

void ExecuteFile(int cfd, char *path)
{
    int pid;

    int pipefd[2];
    if (pipe(pipefd) == FALSE)
    {
        ErrorResponse(cfd, 500, "Error in pipe");
        return;
    }

    pid = fork();
    if (pid < 0)
    {
        ErrorResponse(cfd, 404, "Error in fork");
        perror("Error in fork");
        exit(EXIT_FAILURE);
    }
    // child
    else if (pid == 0)
    {
        close(pipefd[0]);               // close output fd
        dup2(pipefd[1], STDOUT_FILENO); // redirect stdout of child to the output of pipe into child
        close(pipefd[1]);

        // Execute the CGI script (provide absolute path if needed)
        char *argv[] = {path, NULL}; // The path to the CGI script
        execv(path, argv);

        ErrorResponse(cfd, 404, "Error in execv child process");
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    // parent
    else
    {
        close(pipefd[1]); // Close unused write end
        char buffer[_1K];
        ssize_t bytes_read;

        // Write HTTP response headers
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        write(cfd, response, strlen(response));

        // Read from pipe and write to client
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        {
            write(cfd, buffer, bytes_read);
        }

        close(pipefd[0]); // Close read end of pipe
        wait(NULL);       // Wait for child process to complete
    }
}

void CatFile(int cfd, char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        ErrorResponse(cfd, 500, "Error in Reading File");
        return;
    }

    // Send HTTP headers
    char response[_1K];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    ssize_t bytes_sent = write(cfd, response, strlen(response));
    if (bytes_sent < 0)
    {
        perror("Write system call failed");
    }

    // Read and send the file content
    ssize_t bytes;
    while ((bytes = read(fd, response, sizeof(response))) > 0)
    {
        bytes_sent = write(cfd, response, bytes);
        if (bytes_sent < 0)
        {
            perror("Write system call failed");
        }
    }
}