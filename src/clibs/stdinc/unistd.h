#include <io.h>
#include <sys/types.h>
#define ftruncate chsize
int fsync(int fd);
ssize_t pread(int fd, void *buf, size_t nbyte, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t nbyte, off_t offset);
