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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <process.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern char _RTL_DATA **_environ ;

static char *createenviron(char **env)
{
   int len = 0 ;
   char **dummy,*rv=0 ;
   if (!env)
      env = _environ ;
   dummy = env ;
   while (*dummy) {
      len += strlen(*dummy) + 1 ;
      dummy++ ;
   }
   if (len) {
      len += 1 ;
      rv = malloc (len) ;
      if (rv) {
         char *p = rv ;
         while(*env) {
            strcpy(p,*env) ;
            p+= strlen(p)+ 1 ;
            env++ ;
         }
         *p = 0 ;
      }
   }
   return rv ;
}
int __ll_spawn(char *file, char *parms, char **env , int mode)
{
    PROCESS_INFORMATION pi;
   STARTUPINFO si ;
   DWORD rv = -1;
   char buf[1000],*block = createenviron(env) ;
    memset(&si,0,sizeof(si)) ;
    si.cb = sizeof(STARTUPINFO) ;
    si.dwFlags = STARTF_USESTDHANDLES ;
    si.hStdInput = (HANDLE)__uiohandle(fileno(stdin)) ;
    si.hStdOutput = (HANDLE)__uiohandle(fileno(stdout)) ;
    si.hStdError = (HANDLE)__uiohandle(fileno(stderr)) ;
   sprintf(buf,"\"%s\" %s",file,parms) ;
   if (CreateProcess(file,buf,0,0,TRUE,
                        NORMAL_PRIORITY_CLASS,
                  (LPVOID)block,0,&si,&pi)) {
      if (mode == P_WAIT) {
         WaitForInputIdle(pi.hProcess,INFINITE) ;
         WaitForSingleObject(pi.hProcess,INFINITE);
         GetExitCodeProcess(pi.hProcess,&rv) ;
      }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread) ;
      free(block) ;
      return rv ;
   } else {
      free(block) ;
      return -1 ;
   }
}
