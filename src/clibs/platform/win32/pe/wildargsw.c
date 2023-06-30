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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <dir.h>

#define TRUE 1
#define FALSE 0

#define MAXFILES 10000

extern char* _oscmd;
extern HINSTANCE* __hInstance;

int _RTL_DATA _argc, _RTL_DATA __argc;
char _RTL_DATA **_argv, _RTL_DATA **__argv;
char* _passed_name;
#pragma startup argset 31

static char oldcwd[256];
static int oldDrive = 0;
static struct ffblk fileBlock;
static char** filelist = 0;
static int filenum = 0, filecount = 0;

static char* FirstFile(char* spec)
{
    if (findfirst(spec, &fileBlock, _A_NORMAL))
        return (0);
    return (fileBlock.ff_name);
}
static char* NextFile(void)
{
    if (findnext(&fileBlock))
        return (0);
    return (fileBlock.ff_name);
}
static char* litlate(char* dir, char* t)
{
    char *q, buf[256];
    if (!t)
        return 0;
    if (dir)
    {
        char* p;
        strcpy(buf, dir);
        q = strrchr(buf, '\\');
        p = strrchr(buf, '/');
        if (p > q)
            q = p;
        if (q)
            strcpy(q + 1, t);
        else
            strcpy(buf, t);
    }
    else
        strcpy(buf, t);
    q = malloc(strlen(buf) + 1);
    if (!q)
    {
        fprintf(stdout, "out of memory");
        exit(1);
    }
    strcpy(q, buf);
    return q;
}
static void ClearFiles(void)
{
    free(filelist);
    filelist = 0;
    filecount = 0;
}
static char* ReadFiles(char* spec)
{
    char buf[256];
    int fullpath = FALSE;
    if (spec[0] == '.' && spec[1] == '.')
    {
        char* p;
        getcwd(buf, 256);
        p = strlen(buf) + buf;

        while (spec[0] == '.' && spec[1] == '.')
        {
            spec += 2;
            while (*(p - 1) != '\\' && *(p - 1) != '/' && *(p - 1) != ':')
                p--;
            if (*(p - 1) == '\\' || *(p - 1) == '/')
                p--;
            if (spec[0] != '\\' && spec[0] != '/')
                break;
            spec++;
        }
        *p++ = '\\';
        strcpy(p, spec);
    }
    else
        strcpy(buf, spec);
    if (strchr(buf, '\\'))
        fullpath = TRUE;
    else if (strchr(buf, '/'))
        fullpath = TRUE;
    if (!filelist)
    {
        filelist = malloc(sizeof(char*) * MAXFILES);
        if (!filelist)
        {
            fprintf(stderr, "out of memory");
            exit(1);
        }
    }
    if ((filelist[0] = litlate(fullpath ? buf : 0, FirstFile(buf))) == 0)
    {
        return 0;
    }
    while ((filelist[++filecount] = litlate(fullpath ? buf : 0, NextFile())) != 0)
        if (filecount >= MAXFILES - 1)
        {
            fprintf(stderr, "Too many files... qualify file names to limit to %d at a time", MAXFILES);
            break;
        }
    return (char*)1;
}
static int qualify(char* name)
{
    int i;
    for (i = 0; i < strlen(name); i++)
        if (!isalnum(*name) && *name != '\\' && *name != '/' && *name != ':' && *name != '*' && *name != '?' && *name != '.')
            return FALSE;
    return TRUE;
}
static void argset(void)
{
    char buf[256];
    char *bufp[10000], *ocl;
    char* _cmdline = _oscmd;
    int inquote = 0;
    char* dir;
    char* file;
    char* fileName;
    int drive;
    filenum = 0;
    filecount = 0;
    _argc = 0;
    while (*_cmdline)
    {
        while (isspace(*_cmdline))
            _cmdline++;
        if (*_cmdline)
        {
            int wasquote = FALSE;
            int i = 0;
            while ((inquote || !isspace(*_cmdline)) && *_cmdline)
            {
                if (*_cmdline == '"')
                {
                    wasquote = TRUE;
                    inquote = !inquote;
                    _cmdline++;
                    continue;
                }
                if (*_cmdline == '\\' && *(_cmdline + 1) == '"')
                    _cmdline++;
                buf[i++] = *_cmdline++;
            }
            buf[i++] = 0;
            if (!wasquote && qualify(buf))
            {
                if ((fileName = ReadFiles(buf)) == 0)
                {
                    goto join;
                }
                if (filecount)
                {
                    _argv = realloc(_argv, (_argc + filecount) * sizeof(char*));
                    memcpy(_argv + _argc, filelist, filecount * sizeof(char*));
                    _argc += filecount;
                    ClearFiles();
                }
                else
                    goto join;
            }
            else
            {
            join:
                _argv = realloc(_argv, (_argc + 1) * sizeof(char*));
                _argv[_argc++] = strdup(buf);
            }
        }
    }
    _argv = realloc(_argv, (_argc + 1) * sizeof(char*));
    _argv[_argc] = 0;
    _passed_name = _argv[0];
    GetModuleFileName(__hInstance, buf, 256);
    _argv[0] = strdup(buf);
    __argv = _argv;
    __argc = _argc;
}
