;  Software License Agreement
;  
;      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
;  
;      This file is part of the Orange C Compiler package.
;  
;      The Orange C Compiler package is free software: you can redistribute it and/or modify
;      it under the terms of the GNU General Public License as published by
;      the Free Software Foundation, either version 3 of the License, or
;      (at your option) any later version.
;  
;      The Orange C Compiler package is distributed in the hope that it will be useful,
;      but WITHOUT ANY WARRANTY; without even the implied warranty of
;      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;      GNU General Public License for more details.
;  
;      You should have received a copy of the GNU General Public License
;      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
;  
;      contact information:
;          email: TouchStone222@runbox.com <David Lindauer>
;  

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
