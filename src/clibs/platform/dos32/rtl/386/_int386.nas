;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
[global __int386]
[global _int386]
[extern ___GetErrno]
SECTION code CLASS=CODE USE32

callint:  
   db 0cdh
ivec:
   db 0
   ret

__int386:
_int386:
   push  ebp
   mov   ebp,esp
   push  esi
   push  edi
   push  ebx
   push  dword [ebp + 010h] ; outregs
   mov   al,[ebp + 8] ; int no
   mov   [ds:ivec],al
   mov   ebp,[ebp + 00ch] ; inregs
   mov   ah,[ebp + 6 * 4] ; flags
   and   ah,1
   or    ah,42h
   sahf
   mov   eax,[ebp]
   mov   ebx,[ebp+4]
   mov   ecx,[ebp+8]
   mov   edx,[ebp+12]
   mov   esi,[ebp+16]
   mov   edi,[ebp+20]
   call  callint
   pop   ebp       ; outregs
   pushfd
   pop   dword [ebp + 24]
   and   dword [ebp+24],1
   mov   [ebp],eax
   mov   [ebp+4],ebx
   mov   [ebp+8],ecx
   mov   [ebp+12],edx
   mov   [ebp+16],esi
   mov   [ebp+20],edi
   
   je   noerr
   movzx ebx,al
   call  ___GetErrno
   mov   [eax],ebx
noerr:
   pop   ebx
   pop   edi
   pop   esi
   pop   ebp
   ret
