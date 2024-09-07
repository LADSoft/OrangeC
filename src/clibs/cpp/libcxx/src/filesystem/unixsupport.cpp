#include <windows.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <sys/statvfs.h>
extern "C" {
#include <libp.h>
}

extern "C" {

extern char __uinames[HANDLE_MAX][256];

int link(const char *path1, const char *path2)
{
    errno = ENOENT;
    return -1;
}

int symlink(const char *path1, const char *path2)
{
    errno = ENOENT;
    return -1;
}
long pathconf(const char *path, int varcode)
{
    if (varcode == _PC_PATH_MAX)
       return _PC_PATH_MAX;
    return -1;
}
int _RTL_FUNC _IMPORT statvfs(const char* restrict __name, struct statvfs* restrict __statbuf)
{
    char *name, namebuf[10];
    if (__name[1] == ':')
    {
        namebuf[0] = __name[0];
        namebuf[1] = __name[1];
        namebuf[2] = '\\';
        namebuf[3] = 0;
        name = namebuf;
    }
    ULARGE_INTEGER caller_free, bytes, total_free;
    if (GetDiskFreeSpaceEx(name, &caller_free,  &bytes, &total_free))
    {
        memset(__statbuf, 0, sizeof(__statbuf));
        
        __statbuf->f_bsize = 512;
        __statbuf->f_frsize = 2048;
        __statbuf->f_blocks = bytes.QuadPart/2048;
        __statbuf->f_bfree = total_free.QuadPart/2048;
        __statbuf->f_bavail = caller_free.QuadPart/2048;

        return 0;
    }
    return -1;
}
int _RTL_FUNC _IMPORT fstatvfs(int __handle, struct statvfs* __statbuf)
{
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    else
    {
        int rv = statvfs(__uinames[__handle], __statbuf);
        __ll_exit_critical();
        return rv;
    }
}

}