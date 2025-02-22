#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>

#define QUEUE_NAME "/my_message_queue"
#define MSG_SIZE 256

int main()
{
    mqd_t mq;
    char buffer[MSG_SIZE];
    unsigned int priority;
    struct mq_attr attr;

    // Open the queue in read mode
    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }

    // Get attributes
    if (mq_getattr(mq, &attr) == -1)
    {
        perror("mq_getattr");
        return EXIT_FAILURE;
    }

    printf("Queue Attributes:\n");
    printf("Maximum messages: %ld\n", attr.mq_maxmsg);
    printf("Maximum message size: %ld\n", attr.mq_msgsize);
    printf("Current number of messages: %ld\n", attr.mq_curmsgs);

    // Receive the message
    if (mq_receive(mq, buffer, MSG_SIZE, &priority) == -1)
    {
        perror("mq_receive failed");
        exit(1);
    }

    printf("Received message: %s\n", buffer);
    printf("Message priority: %u\n", priority);

    // Remove the message queue
    if (mq_unlink(QUEUE_NAME) == -1)
    {
        perror("mq_unlink failed");
        exit(1);
    }

    printf("Message queue deleted successfully.\n");

    // Close the queue
    if (mq_close(mq) == -1)
    {
        perror("mq_close failed");
        exit(1);
    }

    printf("Message queue closed successfully.\n");

    return 0;
}
