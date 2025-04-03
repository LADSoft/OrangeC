/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <stdio.h>
#include <errno.h>

void __uio_clearerr(int __handle);
void _RTL_FUNC clearerr(FILE* stream)
{
    flockfile(stream);
    (clearerr_unlocked)(stream);
    funlockfile(stream);
}
void _RTL_FUNC clearerr_unlocked(FILE* stream)
{
    if (stream->token == FILTOK)
    {
        stream->flags &= ~(_F_ERR | _F_EOF | _F_XEOF | _F_VBUF);
        __uio_clearerr(fileno(stream));
    }
    //  errno = _dos_errno = 0;
}
int _RTL_FUNC(feof)(FILE* stream)
{
    flockfile(stream);
    int rv = (feof_unlocked)(stream);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC(feof_unlocked)(FILE* stream)
{
    if (stream->token == FILTOK)
    {
        return (stream->flags & _F_EOF);
    }
    return _F_EOF;
}
int _RTL_FUNC(ferror)(FILE* stream)
{
    flockfile(stream);
    int rv = (ferror_unlocked)(stream);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC(ferror_unlocked)(FILE* stream)
{
    if (stream->token == FILTOK)
    {
        return (stream->flags & _F_ERR);
    }
    return _F_ERR;
}
