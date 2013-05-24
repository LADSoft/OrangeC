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
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <stdarg.h>
#include <time.h>
#include <dir.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

static int spawnbase(const char *path, const char *args[], 
                                         const char *env[], int toexit, int tosearch)
{
    FILE *fil;
  int rv;
    char name[260],*vv;
    char parms[1024];
    parms[0] = ' ';
    parms[1] = 0;
    if (*args)
    {
       while (*++args) {
          strcat(parms," ") ;
            strcat(parms,*args);
       }
    }
    strcpy(name,path);
    if (tosearch)
    {
        if (!(vv = searchpath(name))) {
            strcat(name,".EXE");
            if (!(vv = searchpath(name))) {
                strcpy(name,path);
                strcat(name,".COM");
                if (!(vv = searchpath(name))) {
                    return -1 ;
                }
            }
        }
    }
   fflush(0) ;
   rv = __ll_spawn(vv,parms,env,toexit);
    if (toexit)
        exit(rv);
    return rv;
}
static char **findenv(const char **argv)
{
   char **vv;
    vv = argv;
    while (*vv)
        vv++;
   return (*++vv);
}
int _RTL_FUNC execl(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,1,0);
}
int _RTL_FUNC execle(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),1,0);
}
int _RTL_FUNC execlp(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,1,1);
}
int _RTL_FUNC execlpe(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),1,1);
}
int _RTL_FUNC execv(const char *path, const char **argv)
{
    return spawnbase(path,argv,0,1,0);
}
int _RTL_FUNC execve(const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,1,0);
}
int _RTL_FUNC execvp(const char *path, const char **argv)
{
    return spawnbase(path,argv,0,1,1);
}
int _RTL_FUNC execvpe(const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,1,1);
}
int _RTL_FUNC spawnl(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,mode,0);
}
int _RTL_FUNC spawnle(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),mode,0);
}
int _RTL_FUNC spawnlp(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,mode,1);
}
int _RTL_FUNC spawnlpe(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),mode,1);
}
int _RTL_FUNC spawnv(int mode,const char *path, const char **argv)
{
    return spawnbase(path,argv,0,mode,0);
}
int _RTL_FUNC spawnve(int mode,const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,mode,0);
}
int _RTL_FUNC spawnvp(int mode,const char *path, const char **argv)
{
    return spawnbase(path,argv,0,mode,1);
}
int _RTL_FUNC spawnvpe(int mode,const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,mode,1);
}
int _RTL_FUNC _execl(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,1,0);
}
int _RTL_FUNC _execle(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),1,0);
}
int _RTL_FUNC _execlp(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,1,1);
}
int _RTL_FUNC _execlpe(const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),1,1);
}
int _RTL_FUNC _execv(const char *path, const char **argv)
{
    return spawnbase(path,argv,0,1,0);
}
int _RTL_FUNC _execve(const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,1,0);
}
int _RTL_FUNC _execvp(const char *path, const char **argv)
{
    return spawnbase(path,argv,0,1,1);
}
int _RTL_FUNC _execvpe(const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,1,1);
}
int _RTL_FUNC _spawnl(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,mode,0);
}
int _RTL_FUNC _spawnle(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),mode,0);
}
int _RTL_FUNC _spawnlp(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,0,mode,1);
}
int _RTL_FUNC _spawnlpe(int mode,const char *path, const char *argv0,...)
{
    return spawnbase(path,&argv0,findenv(&argv0),mode,1);
}
int _RTL_FUNC _spawnv(int mode,const char *path, const char **argv)
{
    return spawnbase(path,argv,0,mode,0);
}
int _RTL_FUNC _spawnve(int mode,const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,mode,0);
}
int _RTL_FUNC _spawnvp(int mode,const char *path, const char **argv)
{
    return spawnbase(path,argv,0,mode,1);
}
int _RTL_FUNC _spawnvpe(int mode,const char *path, const char **argv, const char **env)
{
    return spawnbase(path,argv,env,mode,1);
}