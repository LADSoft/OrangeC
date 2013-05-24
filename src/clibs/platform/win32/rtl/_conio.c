/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdarg.h>
#include <dpmi.h>

#include "conio.h"

char * 		 _RTL_FUNC _cgets(char *p)
{
    return cgets(p);
}
int			 _RTL_FUNC _cputs(const char *p)
{
    return cputs(p);
}
int			 _RTL_FUNC _getch(void)
{
    return getch();
}
int			 _RTL_FUNC _getche(void)
{
    return getche();
}
int 		 _RTL_FUNC _inp(unsigned short port)
{
    return inp(port);
}
unsigned short _RTL_FUNC _inpw(unsigned short port)
{
    return inpw(port);
}
unsigned long _RTL_FUNC _inpd(unsigned short port)
{
    asm mov dx,[port];
    asm in	eax,dx;
    return _EAX;
}
int 		 _RTL_FUNC _kbhit(void)
{
    return kbhit();
}
int			 _RTL_FUNC _outp(unsigned short port, int val)
{
    return outp(port, val);
}
unsigned short _RTL_FUNC _outpw(unsigned short port , unsigned short val)
{
    return outpw(port, val);
}
unsigned long _RTL_FUNC _outpd(unsigned short port , unsigned long val)
{
    asm mov dx,[port];
    asm mov eax,dword ptr [val];
    asm out dx,eax;
    return val;
}
int			 _RTL_FUNC _putch(int ch)
{
    return putch(ch);
}
int			 _RTL_FUNC _ungetch(int ch)
{
    return ungetch(ch);
}
int			 _RTL_FUNC _cprintf(const char *__format, ...)
{
    char buf[4096];
    va_list argptr;

    va_start( argptr, __format);
    vsprintf( buf,__format, argptr);
    va_end(argptr);
    cputs(buf);
    return 0;
}
#ifndef VSDLL
int			 _RTL_FUNC _cscanf(const char *__format, ...)
{
    va_list argptr;
    char buf[1024];
    cgets(buf);
    va_start( argptr, __format);
    vsscanf( buf,__format, argptr);
    va_end(argptr);
    return 0;
}
#endif