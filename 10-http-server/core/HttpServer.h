/*
 * File Name        : HttpServer.h
 * Description      : Contains declarations for server setup, connection handling, and utility functions.
 * Functions        :
 *                    - initializeServer: Sets up the server socket and binds it to a specified port.
 *                    - acceptConnection: Waits for and accepts incoming client connections.
 *                    - handleRequest: Parses HTTP requests and determines appropriate responses.
 *                    - handleClientFork: Spawns a new process to handle a client request.
 * Notes            : This header file declares the core functionality needed to manage the server lifecycle.
 */

/*===================================  Includes ==============================*/
#include "../utilities/ServerConfig.h"
#include "../utilities/HttpUtils.h"
