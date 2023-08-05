/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <stdarg.h>
#include <time.h>
#include <dir.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <io.h>
#include <fcntl.h>

static int spawnbase(const char* path, const char* args[], const char* env[], int toexit, int tosearch)
{
    FILE* fil;
    int rv;
    char name[260], *vv;
    char *parms= NULL;
    if (*args)
    {
        int len = 2;
        const char **p = args;
        while (*p) 
        {
            len += strlen(p) + 1;
            p++;
        }
        parms = calloc(sizeof(char), len);
        while (*++args)
        {
            strcat(parms, " ");
            strcat(parms, *args);
        }
    }
    strcpy(name, path);
    if (tosearch)
    {
        if (!(vv = searchpath(name)))
        {
            strcat(name, ".EXE");
            if (!(vv = searchpath(name)))
            {
                strcpy(name, path);
                strcat(name, ".COM");
                if (!(vv = searchpath(name)))
                {
                    errno = ENOENT;
                    return -1;
                }
            }
        }
    }
    else
    {
        int h = open(name, O_RDONLY);
        if (h == -1)
        {
            errno = ENOENT;
            return -1;
        }
        close(h);
        vv = name;
    }
    fflush(0);
    rv = __ll_spawn(vv, parms ? parms : " ", env, toexit);
    if (toexit == P_OVERLAY)
        exit(rv);
    return rv;
}
static char** findenv(const char** argv)
{
    char** vv;
    vv = argv;
    while (*vv)
        vv++;
    return (*++vv);
}
int _RTL_FUNC execl(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, P_OVERLAY, 0); }
int _RTL_FUNC execle(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, findenv(&argv0), P_OVERLAY, 0); }
int _RTL_FUNC execlp(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, P_OVERLAY, 1); }
int _RTL_FUNC execlpe(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, findenv(&argv0), P_OVERLAY, 1); }
int _RTL_FUNC execv(const char* path, const char** argv) { return spawnbase(path, argv, 0, P_OVERLAY, 0); }
int _RTL_FUNC execve(const char* path, const char** argv, const char** env) { return spawnbase(path, argv, env, P_OVERLAY, 0); }
int _RTL_FUNC execvp(const char* path, const char** argv) { return spawnbase(path, argv, 0, P_OVERLAY, 1); }
int _RTL_FUNC execvpe(const char* path, const char** argv, const char** env) { return spawnbase(path, argv, env, P_OVERLAY, 1); }
int _RTL_FUNC spawnl(int mode, const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, mode, 0); }
int _RTL_FUNC spawnle(int mode, const char* path, const char* argv0, ...)
{
    return spawnbase(path, &argv0, findenv(&argv0), mode, 0);
}
int _RTL_FUNC spawnlp(int mode, const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, mode, 1); }
int _RTL_FUNC spawnlpe(int mode, const char* path, const char* argv0, ...)
{
    return spawnbase(path, &argv0, findenv(&argv0), mode, 1);
}
int _RTL_FUNC spawnv(int mode, const char* path, const char** argv) { return spawnbase(path, argv, 0, mode, 0); }
int _RTL_FUNC spawnve(int mode, const char* path, const char** argv, const char** env)
{
    return spawnbase(path, argv, env, mode, 0);
}
int _RTL_FUNC spawnvp(int mode, const char* path, const char** argv) { return spawnbase(path, argv, 0, mode, 1); }
int _RTL_FUNC spawnvpe(int mode, const char* path, const char** argv, const char** env)
{
    return spawnbase(path, argv, env, mode, 1);
}
int _RTL_FUNC _execl(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, P_OVERLAY, 0); }
int _RTL_FUNC _execle(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, findenv(&argv0), P_OVERLAY, 0); }
int _RTL_FUNC _execlp(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, P_OVERLAY, 1); }
int _RTL_FUNC _execlpe(const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, findenv(&argv0), P_OVERLAY, 1); }
int _RTL_FUNC _execv(const char* path, const char** argv) { return spawnbase(path, argv, 0, P_OVERLAY, 0); }
int _RTL_FUNC _execve(const char* path, const char** argv, const char** env) { return spawnbase(path, argv, env, P_OVERLAY, 0); }
int _RTL_FUNC _execvp(const char* path, const char** argv) { return spawnbase(path, argv, 0, P_OVERLAY, 1); }
int _RTL_FUNC _execvpe(const char* path, const char** argv, const char** env) { return spawnbase(path, argv, env, P_OVERLAY, 1); }
int _RTL_FUNC _spawnl(int mode, const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, mode, 0); }
int _RTL_FUNC _spawnle(int mode, const char* path, const char* argv0, ...)
{
    return spawnbase(path, &argv0, findenv(&argv0), mode, 0);
}
int _RTL_FUNC _spawnlp(int mode, const char* path, const char* argv0, ...) { return spawnbase(path, &argv0, 0, mode, 1); }
int _RTL_FUNC _spawnlpe(int mode, const char* path, const char* argv0, ...)
{
    return spawnbase(path, &argv0, findenv(&argv0), mode, 1);
}
int _RTL_FUNC _spawnv(int mode, const char* path, const char** argv) { return spawnbase(path, argv, 0, mode, 0); }
int _RTL_FUNC _spawnve(int mode, const char* path, const char** argv, const char** env)
{
    return spawnbase(path, argv, env, mode, 0);
}
int _RTL_FUNC _spawnvp(int mode, const char* path, const char** argv) { return spawnbase(path, argv, 0, mode, 1); }
int _RTL_FUNC _spawnvpe(int mode, const char* path, const char** argv, const char** env)
{
    return spawnbase(path, argv, env, mode, 1);
}
