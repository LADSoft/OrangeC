#include <io.h>
int _RTL_FUNC fsync(int fd)
{
  int h = dup(fd);
  if (h == -1)
    return -1;
  if (close(h) == -1)
    return -1;
  return 0;
}
