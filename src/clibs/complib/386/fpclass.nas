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
___fpclassifyl:
    fld tword [esp + 4]
    jmp short fpclassify
___fpclassifyf:
    fld dword [esp + 4]
    jmp short fpclassify
___fpclassify:
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
