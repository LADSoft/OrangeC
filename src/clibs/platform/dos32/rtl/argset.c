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

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dpmi.h>
#include "llp.h"

extern short __pspseg, __envseg;
int _RTL_DATA _argc, _RTL_DATA __argc;
char _RTL_DATA **_argv, _RTL_DATA **__argv;

#pragma startup argset 31

static void get_progname(char *buf)
{
    char * pos;
    SELECTOR sel;
    DPMI_REGS regs;
    int len;
    int temp;
    
    __doscall(0x19,&regs);
    buf[0] = regs.b.al + 'A';
    buf[1] = ':';
    /* get path */

    /* find file name, assumes DOS 3... */
    __asm push edi
    __asm push es
    __asm mov ecx,-1
    __asm sub edi,edi
   __asm mov es,[__envseg]
    __asm sub al,al
lp:
    __asm repnz scasb
    __asm cmp es:[edi],al
    __asm jnz lp
    __asm add edi,3
    __asm movzx edx,byte ptr es:[edi+1]
  __asm mov [pos],edi
  __asm mov [sel],es
    __asm mov ecx,-1
    __asm repnz scasb
    __asm not ecx
    __asm mov [len],ecx
    __asm pop es
    __asm pop edi
    __asm mov [temp],edx
    if (temp == ':')
        dpmi_copy_to_ds(buf,sel,(ULONG) pos,len);
    else
        dpmi_copy_to_ds(buf+2,sel,(ULONG) pos,len);
    

}
static void argset(void)
{
    char buf[260];
    char *bufp[300], *ocl, *_cmdline;
    int inquote = 0;
    ocl = _cmdline = malloc(128);
    dpmi_copy_to_ds(_cmdline,__pspseg,0x80,128);
  _cmdline[_cmdline[0]+1] = 0;
    _cmdline++;
    _argc = 1;
    while (*_cmdline) {
        while (isspace(*_cmdline)) _cmdline++;
        if (*_cmdline) {
            int i = 0;
            while ((inquote || !isspace(*_cmdline)) && *_cmdline)  {
            if (*_cmdline == '"' && (_cmdline == ocl+1 || *(_cmdline-1) != '\\')) {
                    inquote = !inquote ;
                    _cmdline++ ;
                    continue ;
                }
                buf[i++] = *_cmdline++;
            }
            buf[i++] = 0;
            bufp[_argc++] = strdup(buf) ;
        }
    }
   _argv = calloc((_argc+1),sizeof(char *));
   memcpy(_argv,bufp,_argc*sizeof(char *));
    _cmdline = ocl;
    get_progname(_cmdline);
   _argv[0] = _cmdline;
   __argv = _argv ;
   __argc = _argc ;
}