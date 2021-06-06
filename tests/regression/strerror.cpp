#include <string.h>
#include <errno.h>
#include <stdio.h>
errno_t printerr(int err)
{
      char *s = strerror (err);
      printf("%s\n", s);
      return 0;
}
errno_t printerr(char *buf, unsigned size, errno_t num)
{
      auto rv = strerror_s(buf, size, num);
      printf("%s\n", buf);
      return rv;
}
errno_t printerr(char *buf, unsigned size, const char* msg)
{
      auto rv = _strerror_s(buf, size, msg);
      printf("%s\n", buf);
      return rv;
}
#define MSG "hi dave"
#define MSGLEN sizeof(MSG)

int main()
{
	printf("%d\n", MSGLEN);
        char *p = strerror(ENOSPC);
	char buf[1024];
	printerr(ENOSPC);
        printf("%d\n", printerr(buf, 1024, ENOSPC));
	errno = ENOSPC;
        printf("%d\n", printerr(buf, 1024, MSG));
        printf("%d\n", printerr(buf, strlen(p)+1, ENOSPC));
	errno = ENOSPC;
        printf("%d\n", printerr(buf, strlen(p) + 2 + MSGLEN, MSG));
        printf("%d\n", printerr(buf, strlen(p), ENOSPC));
	errno = ENOSPC;
        printf("%d\n", printerr(buf, strlen(p) + 3, MSG));
	errno = ENOSPC;
        printf("%d\n", printerr(buf, MSGLEN, MSG));
        printf("%d\n", printerr(buf, MSGLEN+1, MSG));
        printf("%d\n", printerr(buf, MSGLEN+2, MSG));
        printf("%d\n", printerr(buf, MSGLEN+3, MSG));
        printf("%d\n", printerr(buf, MSGLEN+8, MSG));

}