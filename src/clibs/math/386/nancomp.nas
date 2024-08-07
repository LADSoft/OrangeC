;  Software License Agreement
;  
;      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
[export ___nancompare]
%endif
[global ___nancompare]

SECTION data CLASS=DATA USE32

section code CLASS=CODE USE32

___nancompare:
    mov ecx,[esp + 28]
    mov eax,0
    call clearmath
    fld tword [esp+4]  ; x
    fnclex
    fxam
    fnclex
    fstsw ax
    and ah,45h
    cmp ah,1
    jz retunordered1
    fld tword [esp+16] ; y
    fnclex
    fxam
    fnclex
    fstsw ax
    and ah,45h
    cmp ah,1
    jz retunordered2
    or ecx,ecx      ; unorder check?
    jz retfalse2
    fcompp
    fstsw ax
    test cl,1
    jnz notequal
    sahf
    jz rettrue
notequal:
    test cl,cl
    jns less
    sahf
    ja rettrue
    cmp cl,-3 ; less or greater
    je less
    jmp retfalse
less:
    sahf
    jb rettrue
retfalse:
    sub eax,eax
    ret
retfalse2:
    poptwo
    jmp retfalse
retunordered2:
    ffree st1
retunordered1:
    ffree st0
    or ecx,ecx
    jnz retfalse
rettrue:
    mov eax,1
    ret
