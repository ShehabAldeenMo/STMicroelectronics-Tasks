#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>

#define MAX_DIGIT_COUNT (40)
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

	int fd = open(fileName, O_RDWR | O_CREAT | O_APPEND, 0666);
	if (fd == -1)
	{
		perror("open");

		exit(1);
	}

	while (1)
	{

		// exclusive lock the file, for writing
		if (flock(fd, LOCK_EX) == -1)
		{
			perror("flock");
			exit(1);
		}

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

		sleep(2);
		write(fd, lastSeq, count);

		if (flock(fd, LOCK_UN) == -1)
		{
			perror("flock");
			exit(1);
		}

		usleep(10000);
	}

	close(fd);
}
