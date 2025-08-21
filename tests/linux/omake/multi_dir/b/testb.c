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
	printf("B trying to open\n");
	int fd = open("/tmp/test_omake", O_RDONLY);
	if (fd < 0)
	{
		printf("Errno is: %d\n", errno);
		return 1;
	}
	printf("B opened\n");
	char buf[1];
	ssize_t read_count = read(fd, buf, 1);
	printf("B trying to read\n");
	if (read_count == -1)
	{
		printf("Did not read!\n");
		return 1;
	}
	return 0;
}
