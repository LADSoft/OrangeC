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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <dir.h>
#include <ctype.h>
#include <dpmi.h>
#include "llp.h"

#define TRUE 1
#define FALSE 0

#define MAXFILES 10000

extern short __pspseg, __envseg;

int _RTL_DATA _argc, _RTL_DATA __argc;
char _RTL_DATA **_argv, _RTL_DATA **__argv;
char *_passed_name ;
#pragma startup argset 225

static char oldcwd[256];
static int oldDrive = 0;
static struct ffblk fileBlock;
static char **filelist=0;
static int filenum=0,filecount=0;

static void get_progname(char *buf)
{
    char * pos;
    SELECTOR sel;
    DPMI_REGS regs;
    int len;

    __doscall(0x19,&regs);
    buf[0] = regs.b.al + 'A';
    buf[1] = ':';
    /* get path */

    /* find file name, assumes DOS 3... */
    asm push edi
    asm push es
    asm mov ecx,-1
    asm sub edi,edi
   asm mov es,[__envseg]
    asm sub al,al
lp:
    asm repnz scasb
    asm cmp es:[edi],al
    asm jnz lp
    asm add edi,3
    asm movzx edx,byte ptr es:[edi+1]
    asm mov [pos],edi
    asm mov [sel],es
    asm mov ecx,-1
    asm repnz scasb
    asm not ecx
    asm mov [len],ecx
    asm pop es
    asm pop edi
    if (_EDX == ':')
        dpmi_copy_to_ds(buf,sel,(ULONG) pos,len);
    else
        dpmi_copy_to_ds(buf+2,sel,(ULONG) pos,len);
    

}
static char *FirstFile(char *spec)
{
  if (findfirst(spec, &fileBlock, _A_NORMAL ))
    return(0);
  return(fileBlock.ff_name) ;
}
static char *NextFile(void)
{
  if (findnext( &fileBlock))
    return(0);
  return(fileBlock.ff_name);
}
static int rfsort(const void *elem1, const void *elem2)
{
    return(strcmp(*(char **)elem1, *(char **)elem2));
}
static char *litlate(char *dir, char *t)
{
   char *q,buf[256];
    if (!t)
        return 0;
   if (dir) {
      strcpy(buf,dir) ;
      q = strrchr(buf,'\\') ;
      if (q)
        strcpy(q+1,t) ;
      else
         strcpy(buf,t) ; 
   } else
      strcpy(buf,t) ;
   q = malloc(strlen(buf)+1);
    if (!q) {
        fprintf(stdout,"out of memory");
        exit(1) ;
    }
   strcpy(q,buf);
    return q;
}
static void ClearFiles(void)
{
    free(filelist);
    filelist = 0;
    filecount = 0 ;
}
static char *ReadFiles(char *spec)
{
   char buf[256] ;
   int fullpath = FALSE ;
   if (spec[0] == '.' && spec[1] == '.') {
      char *p ;
      getcwd(buf,256) ;
      p = strlen(buf) + buf ;

      while (spec[0] == '.' && spec[1] == '.') {
         spec += 2 ;
         while (*(p-1) != '\\' && *(p-1) != ':')
            p-- ;
         if (*(p-1) == '\\')
            p-- ;
         if (spec [0] != '\\')
            break ;
         spec++ ;
      }
      *p++ = '\\' ;
      strcpy(p,spec) ;
   } else
      strcpy(buf,spec) ;
   if (strchr(buf,'\\'))
      fullpath = TRUE ;
    if (!filelist) {
        filelist = malloc(sizeof(char *)*MAXFILES);
        if (!filelist) {
            fprintf(stderr,"out of memory");
            exit(1) ;
        }
    }
   if ((filelist[0] = litlate(fullpath ? buf : 0,FirstFile(buf))) == 0) {    
        return 0;
    }
   while((filelist[++filecount] = litlate(fullpath ? buf : 0,NextFile())) != 0)
        if (filecount >= MAXFILES-1) {
            fprintf(stderr,"Too many files... qualify file names to limit to %d at a time",MAXFILES);
            break ;
        }
    return (char *)1;
}
static int qualify(char *name)
{
  int i ;
    for(i=0; i < strlen(name); i++)
      if (!isalnum(*name) && *name != '\\' && *name != ':' && *name != '*' && *name != '?' && *name != '.')
            return FALSE ;
    return TRUE ;
}
static void argset(void)
{
    char buf[200];
    char *bufp[100], *ocl, *_cmdline;
    int inquote = 0;
    char *dir;
    char *file;
    char *fileName;
    int drive;
    ocl = _cmdline = malloc(128);
    dpmi_copy_to_ds(_cmdline,__pspseg,0x80,128);
  _cmdline[_cmdline[0]+1] = 0;
    _cmdline++;
    filenum = 0;
    filecount = 0;
   _argc = 1;
    while (*_cmdline) {
        while (isspace(*_cmdline)) _cmdline++;
        if (*_cmdline) {
         int wasquote = FALSE ;
            int i = 0;
            while ((inquote || !isspace(*_cmdline)) && *_cmdline)  {
                if (*_cmdline == '"') {
               wasquote = TRUE ;
                    inquote = !inquote ;
                    _cmdline++ ;
                    continue ;
                }
                buf[i++] = *_cmdline++;
            }
         buf[i++] = 0 ;
         if (!wasquote && qualify(buf)) {
            if ((fileName = ReadFiles(buf)) == 0) {
               goto join ;
            }     
            if (filecount) {
               _argv = realloc(_argv,(_argc + filecount) * sizeof(char *)) ;
               memcpy(_argv+_argc,filelist,filecount *sizeof(char *)) ;
               _argc += filecount ;
               ClearFiles() ;
            } else
               goto join ;
         } else
         {
join:
               _argv = realloc(_argv,(_argc + 1) * sizeof(char *)) ;
               _argv[_argc++] = strdup(buf) ;
         }
            
        }
    }
   _argv = realloc(_argv,(_argc + 1) * sizeof(char *)) ;
   _argv[_argc] = 0 ;
   _passed_name = _argv[0] ;
    _cmdline = ocl;
    get_progname(_cmdline);
   _argv[0] = _cmdline;
   __argc = _argc ;
   __argv = _argv ;
}