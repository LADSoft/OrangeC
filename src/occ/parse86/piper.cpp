#ifndef HAVE_UNISTD_H
#    include <windows.h>
#else
#    include <stdlib.h>
#    include <string.h>
#endif
#include <stdio.h>

void fatal(char*, ...);
char pipeName[260];

struct data
{
    char* buf;
    int level;
    int length;
};
#ifndef HAVE_UNISTD_H

static void WaitForPipeData(HANDLE hPipe, int size)
{
    int xx = GetTickCount();
    while (xx + 10000 > GetTickCount())
    {
        DWORD avail;
        if (!PeekNamedPipe(hPipe, NULL, 0, NULL, &avail, NULL))
        {
            break;
        }
        if (avail >= size)
        {
            return;
        }
        Sleep(0);
    }
    fatal("Broken pipe");
}
#endif
static struct data* readFileFromPipe(char* filname)
{
    char pipe[260];
    struct data* rv = NULL;
#ifndef HAVE_UNISTD_H
    HANDLE handle;
    sprintf(pipe, "\\\\.\\pipe\\%s", pipeName);
    handle = CreateFile(pipe, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle != INVALID_HANDLE_VALUE)
    {
        DWORD n = strlen(filname);
        DWORD read;
        if (WriteFile(handle, &n, sizeof(DWORD), &read, NULL) && WriteFile(handle, filname, n, &read, NULL))
        {
            WaitForPipeData(handle, sizeof(DWORD));
            if (ReadFile(handle, &n, sizeof(DWORD), &read, NULL))
            {
                if (n != 0)
                {
                    char* buf = (char *) malloc(n + 1);
                    buf[n] = 0;
                    WaitForPipeData(handle, n);
                    if (ReadFile(handle, buf, n, &read, NULL) && read == n)
                    {
                        rv = (data *)calloc(1, sizeof(struct data));
                        rv->buf = buf;
                        rv->length = n;
                        rv->level = 0;
                    }
                    else
                    {
                        free(buf);
                    }
                }
            }
        }
        CloseHandle(handle);
    }
#endif
    return rv;
}
void ccCloseFile(FILE* handle)
{
    extern void ccEndFile(void);
    ccEndFile();
    if (pipeName[0])
    {
        struct data* v = (struct data*)handle;
        free(v->buf);
        free(v);
    }
    else
    {
        fclose(handle);
    }
}
size_t ccReadFile(void* __ptr, size_t __size, size_t __n, FILE* __stream)
{
    if (pipeName[0])
    {
        struct data* v = (struct data*)__stream;
        int needed = __size * __n;
        if (needed > v->length - v->level)
            needed = v->length - v->level;
        memcpy(__ptr, v->buf + v->level, needed);
        v->level += needed;
        return needed;
    }
    else
    {
        return fread(__ptr, __size, __n, __stream);
    }
}

FILE* ccOpenFile(char* filname, FILE* fil, char* mode)
{
    if (pipeName[0])
    {
        if (fil)
            fclose(fil);
        fil = (FILE*)readFileFromPipe(filname);
    }
    return fil;
}
