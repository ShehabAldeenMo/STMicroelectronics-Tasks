# HttpServer

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Compilation](#compilation)
4. [Usage](#usage)

## Introduction
HttpServer is a simple HTTP server that handles client requests using socket programming and process forking for concurrency.

## Features
- Handles multiple client connections concurrently.
- Supports HTTP request and response handling.
- Implements process management to avoid zombie processes.

## Compilation
To compile the server, run:
```bash
make
```

## Usage
```bash
./HttpServer <port>
```
The server will listen on the specified port and handle incoming HTTP requests.
