#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define QUEUE_NAME "/my_message_queue"

int main()
{
    mqd_t mq;
    char message[] = "Hello, Message Queue!";
    struct mq_attr attr;

    // Set message queue attributes
    attr.mq_flags = 0;     // Blocking mode
    attr.mq_maxmsg = 10;   // Max messages in the queue
    attr.mq_msgsize = 256; // Max size of each message
    attr.mq_curmsgs = 0;   // Number of messages currently in the queue

    // Open or create the message queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1)
    {
        perror("mq_open failed");
        exit(1);
    }

    printf("Message queue created successfully.\n");

    // Send message with priority 1
    if (mq_send(mq, message, strlen(message) + 1, 1) == -1)
    {
        perror("mq_send failed");
        exit(1);
    }

    printf("Message sent successfully.\n");

    // Close the message queue
    mq_close(mq);

    return 0;
}
