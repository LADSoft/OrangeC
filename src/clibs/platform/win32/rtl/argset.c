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
#include <windows.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern char *_oscmd ;
extern HINSTANCE *__hInstance ;

int _RTL_DATA _argc, _RTL_DATA __argc;
char _RTL_DATA **_argv, _RTL_DATA **__argv;
char *_passed_name ;
#pragma startup argset 225

static void argset(void)
{
        char buf[260];
        char *bufp[10000], *ocl ;
        char *_cmdline = _oscmd ;
  int inquote = 0 ;
        _argc = 0;
        while (*_cmdline) {
                while (isspace(*_cmdline)) _cmdline++;
                if (*_cmdline) {
                        int i = 0;
                        while ((inquote || !isspace(*_cmdline)) && *_cmdline)  {
                                if (*_cmdline == '"') {
                                        inquote = !inquote ;
                                        _cmdline++ ;
                                        continue ;
                                }
                                if (*_cmdline == '\\' && *(_cmdline+1) == '"')
                                        _cmdline++ ;
                                buf[i++] = *_cmdline++;
                        }
                        buf[i++] = 0;
                        bufp[_argc++] = strdup(buf) ;
                }
        }
   _argv = calloc((_argc+1),sizeof(char *));
   memcpy(_argv,bufp,_argc*sizeof(char *));
   _passed_name = _argv[0] ;
        GetModuleFileName(__hInstance,buf,200) ;
   _argv[0] = strdup(buf) ;
   __argv = _argv ;
   __argc = _argc ;
}
