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
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "io.h"
#include "libp.h"

extern int _abterm;

FILE* _pstreams[_NFILE_];
int __maxfiles;

FILE _RTL_DATA* __stdin;
FILE _RTL_DATA* __stdout;
FILE _RTL_DATA* __stderr;
FILE _RTL_DATA* __stdaux;
FILE _RTL_DATA* __stdprn;

#pragma startup __fileinit 30
#pragma rundown __closeall 10

void __fileinit(void)
{
    __ll_init();
    __stdin = __getStream(0);
    __stdout = __getStream(1);
    __stderr = __getStream(2);
}

void __closeall(void)
{
    if (!_abterm)
    {
        fcloseall();
    }
}
int _RTL_FUNC fcloseall(void)
{
    int i;
    int rv = __maxfiles - 3;
    for (i = __maxfiles - 1; i >= 3; i--)
        fclose(_pstreams[i]);
    fflush(stdout);
    fflush(stderr);
    __maxfiles = 3;
    return rv;
}
int _RTL_FUNC _fcloseall(void) { return fcloseall(); }
int __basefclose(FILE* stream, int release)
{
    int rv, i;

    if (stream->token == FILTOK && __maxfiles)
    {
        wchar_t* fname = stream->extended->name;
        stream->flags &= ~_F_VBUF;
        fflush(stream);
        stream->token = (short)-1;
        if (__maxfiles > 1)
        {
            __ll_enter_critical();
            for (i = 0; i < __maxfiles; i++)
                if (_pstreams[i] == stream)
                {
                    _pstreams[i] = _pstreams[__maxfiles - 1];
                    __maxfiles--;
                    break;
                }
            __ll_exit_critical();
        }
        rv = close(fileno(stream));
        if (fname)
            free(fname);
        if (release)
        {
            free(stream->extended);
            if (stream->flags & _F_BUF)
                free(stream->buffer);
            free(stream);
        }
        if (!rv)
            return 0;
        else
        {
            return EOF;
        }
    }
    else
    {
        errno = _dos_errno = ENOENT;
        return EOF;
    }
}
int _RTL_FUNC fclose(FILE* stream) { return __basefclose(stream, 1); }