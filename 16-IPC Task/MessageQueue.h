#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <iostream>
#include <mqueue.h>
#include <fcntl.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fstream>
#include <signal.h>
#include <condition_variable>

class MessageQueue
{
private:
    int message_size = 256;
    mqd_t mq;

public:
    MessageQueue() : mq(-1) {}

    // Open the message queue with user-specified flags
    bool openQueue(const char *queue_name, int flags);

    // Send a message to the queue
    bool sendMessage(const std::string &message, unsigned int priority);

    // Receive a message from the queue
    std::string receiveMessage();

    // Close the queue
    void closeQueue();

    // Unlink (delete) the queue
    void deleteQueue(const char *queue_name);

    // Destructor (cleanup)
    ~MessageQueue();
};

#endif