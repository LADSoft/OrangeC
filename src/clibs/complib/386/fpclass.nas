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

%include "matherr.inc"
%ifdef __BUILDING_LSCRTL_DLL
[export ___fpclassify]
[export ___fpclassifyf]
[export ___fpclassifyl]
%endif
[global ___fpclassify]
[global ___fpclassifyf]
[global ___fpclassifyl]

%define FP_INFINITE 0
%define FP_NAN 1
%define FP_NORMAL 2
%define FP_SUBNORMAL 3
%define FP_ZERO 4

SECTION code CLASS=CODE USE32
___fpclassifyf:
    fld dword [esp + 4]
    jmp short fpclassify
___fpclassify:
___fpclassifyl:
    fld qword [esp + 4]
fpclassify:
    fxam
    fstsw ax
    popone
    fnclex
    and ah,45h
    cmp ah,1
    mov al,FP_NAN
    jz  retv
    cmp ah,4
    mov al,FP_NORMAL
    jz  retv
    cmp ah,5
    mov al, FP_INFINITE
    jz  retv
    cmp ah,040h
    mov al,FP_ZERO
    jz  retv
    cmp ah,044h
    mov al,FP_SUBNORMAL
    jz  retv
    mov al,-1
retv:
    movzx   eax,al
    ret
