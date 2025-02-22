#include "MessageQueue.h"

// Open the message queue with user-specified flags
bool MessageQueue::openQueue(const char *queue_name, int flags)
{
    struct mq_attr attr;
    attr.mq_flags = 0;              // Blocking mode
    attr.mq_maxmsg = 10;            // Max messages allowed
    attr.mq_msgsize = message_size; // Max message size
    attr.mq_curmsgs = 0;            // Current messages in queue

    // Try opening the queue (without creating)
    mq = mq_open(queue_name, O_RDWR);

    if (mq == (mqd_t)-1)
    {
        // If queue does not exist, create it
        mq = mq_open(queue_name, flags, 0666, &attr);
        if (mq == (mqd_t)-1)
        {
            std::cerr << "Error: mq_open failed, errno = " << errno << " (" << strerror(errno) << ")\n";
            return false;
        }
        std::cout << "Queue created successfully.\n";
    }
    else
    {
        std::cout << "Queue opened successfully.\n";
    }

    return true;
}

// Send a message to the queue
bool MessageQueue::sendMessage(const std::string &message, unsigned int priority)
{
    if (mq_send(mq, message.c_str(), message.size() + 1, 1) == -1)
    {
        std::cerr << "Error: mq_send failed, errno = " << errno << " (" << strerror(errno) << ")\n";
        return false;
    }

    return true;
}

// Receive a message from the queue
std::string MessageQueue::receiveMessage()
{
    char buffer[message_size];
    memset(buffer, 0, sizeof(buffer));
    unsigned int priority;

    if (mq_receive(mq, buffer, message_size, &priority) == -1)
    {
        std::cerr << "Error: mq_receive failed\n";
        return "ERROR";
    }

    return std::string(buffer);
}

// Close the queue
void MessageQueue::closeQueue()
{
    if (mq_close(mq) == -1)
    {
        std::cerr << "Error: mq_close failed\n";
    }
    else
    {
        std::cout << "Message queue closed successfully.\n";
    }
}

// Unlink (delete) the queue
void MessageQueue::deleteQueue(const char *queue_name)
{
    if (mq_unlink(queue_name) == -1)
    {
        std::cerr << "Error: mq_unlink failed\n";
    }
    else
    {
        std::cout << "Message queue deleted successfully.\n";
    }
}

// Destructor (cleanup)
MessageQueue::~MessageQueue()
{
    closeQueue();
}
