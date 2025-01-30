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

	off_t fileSize = lseek(fd, 0, SEEK_END);
	if (fileSize == -1)
	{
		perror("lseek");
	}

	// Calculate the length to lock (whole file except the last 2 lines)
	off_t lockLength = fileSize > MAX_DIGIT_COUNT ? fileSize - (2 * MAX_DIGIT_COUNT) : 0;

	// read lock for the whole file except the last record
	fileLock.l_type = F_RDLCK;
	fileLock.l_whence = SEEK_SET;
	fileLock.l_start = 0;
	fileLock.l_len = lockLength;

	if (fcntl(fd, F_SETLKW, &fileLock) == -1)
	{
		perror("fcntl");
		exit(1);
	}

	printf("Read lock aquired\n");

	int readLength = lockLength == 0 ? MAX_DIGIT_COUNT : lockLength;

	lseek(fd, 0, SEEK_SET);
	count = read(fd, buff, readLength);

	write(1, buff, count);
	write(1, "\n", 1);

	// unlock the write lock
	fileLock.l_type = F_UNLCK;

	if (fcntl(fd, F_SETLKW, &fileLock) == -1)
	{
		perror("fcntl");
		exit(1);
	}

	printf("Read lock released\n");

	close(fd);
}
