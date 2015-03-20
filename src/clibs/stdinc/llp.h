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
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __DPMI_H
#include <dpmi.h>
#endif

#ifndef __STDDEF_H
#include <stddef.h>
#endif


void       _RTL_FUNC _IMPORT __clearxcept(void);
int        _RTL_FUNC _IMPORT __setxcept(int num, void *routine);
void       _RTL_FUNC _IMPORT __lockregion(void *start, void *end); 
int        _RTL_FUNC _IMPORT __nametodta(char *name, int pos);
int        _RTL_FUNC _IMPORT __buftodta(unsigned char *buf, int len);
int        _RTL_FUNC _IMPORT __dtatobuf(unsigned char *buf, int len);
int        _RTL_FUNC _IMPORT __doscall(int func, DPMI_REGS *regs);
void __swPutChar(void **buf, wchar_t ch, int wide);
wchar_t __swGetChar(void **buf, int wide);
void __swMovePointer(void **buf, int offs, int wide);

#ifdef __cplusplus
};
#endif
