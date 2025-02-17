#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_DIGIT_COUNT (10)
#define BUF_SIZE (1000000)

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("the file name is missing\n");
		exit(1);
	}

	const char *fileName = argv[1];

	char buff[BUF_SIZE];
	char *lastLine;
	char *lastNewLine;
	char lastSeq[MAX_DIGIT_COUNT];

	ssize_t count = 0;
	int seq = 0;

	struct flock fileLock;

	int fd = open(fileName, O_RDWR | O_CREAT, 0666);
	if (fd == -1)
	{
		perror("open");

		exit(1);
	}

	while (1)
	{

		off_t fileSize = lseek(fd, 0, SEEK_END);
		if (fileSize == -1)
		{
			perror("lseek");
		}

		// Calculate the length to lock (whole file except the last 20 bytes)
		int lockLength = fileSize > MAX_DIGIT_COUNT ? -MAX_DIGIT_COUNT : 0;

		// write lock for the last record
		fileLock.l_type = F_WRLCK;
		fileLock.l_whence = SEEK_END;
		fileLock.l_start = lockLength;
		fileLock.l_len = 0;

		/*
			 100 -> 90 - 110 /// reader size - 2 * 10
		*/

		if (fcntl(fd, F_SETLKW, &fileLock) == -1)
		{
			perror("fcntl");
			exit(1);
		}

		printf("Write lock aquired\n");

		lseek(fd, 0, SEEK_SET);

		count = read(fd, buff, BUF_SIZE);
		if (count != 0)
		{
			// remove the last new line
			buff[count - 1] = '\0';

			// get the last new line
			lastNewLine = strrchr(buff, '\n');
			if (lastNewLine == NULL)
			{
				seq = 1;
				printf("Last line: 0\n");
			}
			else
			{
				lastLine = lastNewLine + 1;

				printf("Last line: %s\n", lastLine);

				seq = atoi(lastLine);
				seq++;
			}
		}
		else
		{
			seq = 0;
			printf("File is empty\n");
		}

		count = snprintf(lastSeq, MAX_DIGIT_COUNT, "%d\n", seq);

		printf("write seq no %d\n", seq);

		// size = 100 -> 90 -> 100

		write(fd, lastSeq, count);

		// size = 110 -> 90 -> 110

		sleep(1);

		// 90

		// 100

		// unlock the write lock
		fileLock.l_type = F_UNLCK;
		fileLock.l_whence = SEEK_SET;
		fileLock.l_start = 0;
		fileLock.l_len = 0;

#if 0
          fileLock.l_whence = SEEK_END;
          fileLock.l_start = 2 * - MAX_DIGIT_COUNT;
          fileLock.l_len = 0;
#endif

		if (fcntl(fd, F_SETLKW, &fileLock) == -1)
		{
			perror("fcntl");
			exit(1);
		}

		usleep(100000); // Sleep for 100 milliseconds

		printf("Write lock released\n");
	}

	close(fd);
}
