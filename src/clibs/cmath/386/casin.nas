; Software License Agreement
; 
;     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version, with the addition of the 
;     Orange C "Target Code" exception.
; 
;     The Orange C Compiler package is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>

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