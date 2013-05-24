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
[export _casin]
[export _casinf]
[export _casinl]
%endif
[global _casin]
[global _casinf]
[global _casinl]
[extern _cabsl]
[extern _csqrtl]
[extern _clogl]
[extern _asinl]
SECTION data CLASS=DATA USE32
nm	db	"casin",0
oneeighth dq 0.125
two dd 2
epsilon dw 0,0,0,8000h,3fffh-64 ; 0x1P-64

SECTION code CLASS=CODE USE32

_casinf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short casin
_casinl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    mov dl,2
    jmp short casin
_casin:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
casin:
    push edx
    push ebp
    mov ebp,esp
    sub esp,76
    lea eax,[nm]
    call clearmath
    fstp tword [esp]
    ftst
    fstsw ax
    fstp tword [esp+10]
    sahf
    jz lasin ; imaginary is zero, do arc sine of real
    call _cabsl
    fcomp qword [oneeighth]
    fstsw ax
    sahf
    jb series
    call square
    fchs
    fld1
    faddp st1
    sub esp,20
    fstp tword [esp]
    fchs
    fstp tword [esp+10]
    call _csqrtl
    add esp,20
    fld tword [esp+10]
    fsubp st1
    fxch
    fld tword [esp]
    faddp st1
    fstp tword [esp+10]
    fstp tword [esp]
    call _clogl
    fchs
    fxch
    leave
    pop edx
    jmp wrapcomplex
lasin:
    call    _asinl
    fldz
    fxch
    leave
    pop edx
    jmp wrapcomplex
series:
%define n ebp - 4
%define cn ebp - 16
%define sum ebp-36
%define z2 ebp - 56
%define ca ebp - 76
%define ct ebp - 96
     mov dword [n],1
     fld1
     fstp tword [cn]
     call square
     fstp tword [z2]
     fstp tword [z2 + 10]
     fld tword [ca]
     fstp tword [sum]
     fld tword [ca + 10]
     fstp tword [sum + 10]
slp:
     call xmul
     sub esp,20
     fld st0
     fstp tword [ca]
     fstp tword [ct] 
     fld st0
     fstp tword [ca+10]
     fstp tword [ct + 10] 
     
     fld tword [cn]
     fimul dword [n]
     inc dword [n]
     fidiv dword [n]
     fld st0
     fstp tword [cn]
     inc dword [n]
     fidiv dword [n]

     fld tword [ct]
     fmul st1
     fld st0
     fstp tword [ct]
     fld tword [sum]
     faddp st1
     fstp tword [sum]

     fld tword [ct+10]
     fmulp st1
     fld st0
     fstp tword [ct+10]
     fld tword [sum+10]
     faddp st1
     fstp tword [sum+10]
     call _cabsl
     add    esp,20
     fld    tword [epsilon]
     fcompp
     fstsw ax
     sahf
     jc slp
     fld tword [sum+10]
     fld tword [sum]
     leave
     pop edx
     jmp wrapcomplex     
xmul:
    fld tword [esp + 14]
    fld tword [esp+24]
    fmulp st1    
    fld tword [esp+4]
    fld tword [esp+34]
    fmulp st1
    faddp st1
    
    fld tword [esp+4]
    fld tword [esp+24]
    fmulp st1
    fld tword [esp + 14]
    fld tword [esp+34]
    fmulp st1
    fsubp st1
    ret
square:
    fld tword [esp + 4]
    fld tword [esp+14]
    fmulp st1
    fimul dword [two]

    fld tword [esp+4]
    fld tword [esp + 14]
    fsubp st1
    fld tword [esp+4]
    fld tword [esp + 14]
    faddp st1
    fmulp st1
    ret