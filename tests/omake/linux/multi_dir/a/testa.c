#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
int main()
{
	// Yes, I know this can fail if the other end opens this fifo first, I don't particularly care.
	// I just need this to exist
	mkfifo("/tmp/test_omake", 0666);
	printf("A Trying to open\n");
	int fd = open("/tmp/test_omake", O_WRONLY);
	if (fd < 0)
	{
		printf("Errno is: %d\n", errno);
		return 1;
	}
	printf("A Trying to write\n");
	ssize_t written = write(fd, "1", 1);
	if (written == -1)
	{
		printf("Did not write!\n");
		return 1;
	}
	printf("A Written\n");
	fflush(stdout);
	return 0;
}
