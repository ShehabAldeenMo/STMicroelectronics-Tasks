/*
 * File Name        : ServerConfig.h
 * Description      : Contains constants and configurations used throughout the HTTP server.
 * Constants        :
 *                    - SERVER_PORT: Default port number for the server.
 *                    - ROOT_DIRECTORY: Base directory for serving files.
 *                    - MAX_CONNECTIONS: Maximum number of simultaneous connections.
 * Notes            : Modify these constants as needed to configure the server's behavior.
 */

/*==================================  Definations =============================*/
#define _1K 1024
#define SERVER_BUF 4 * _1K
#define BACKLOG 2