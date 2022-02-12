; Software License Agreement
; 
;     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version.
; 
;     The Orange C Compiler package is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     As a special exception, if other files instantiate templates or
;     use macros or inline functions from this file, or you compile
;     this file and link it with other works to produce a work based
;     on this file, this file does not by itself cause the resulting
;     work to be covered by the GNU General Public License. However
;     the source code for this file must still be made available in
;     accordance with section (3) of the GNU General Public License.
;     
;     This exception does not invalidate any other reasons why a work
;     based on this file might be covered by the GNU General Public
;     License.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _fmod]
[export _fmodf]
[export _fmodl]
%endif
[global _fmod]
[global _fmodf]
[global _fmodl]
SECTION data CLASS=DATA USE32
nm	db	"fmod",0

SECTION code CLASS=CODE USE32
_fmodf:
    lea	ecx,[esp+4]
    lea	edx,[esp+8]
    fld	dword[ecx]
    fld	dword[edx]
    sub dl,dl
    jmp short fmod
_fmodl:
_fmod:
    lea	ecx,[esp+4]
    lea	edx,[esp+12]
    fld	qword[ecx]
    fld	qword[edx]
    mov dl,1
fmod:
    lea eax,[nm]
    call clearmath
    ftst
    fnstsw ax
    sahf
    mov cl,ah
    jz moderr
    fxch
    fld st1
    fld st1
reprem:
    fprem1
    fstsw ax
    sahf
    jp reprem
    fxch
    ftst    ;y
    fnstsw ax
    mov ch,ah
    popone
    ftst    ;result
    fnstsw ax
    mov cl,ah
    fxch
    ftst    ;x
    fnstsw ax
    popone
    test cl,40h ; result zero?
    jnz zeradj
    xor ah,cl
    test ah,1 ; carry bit = sign
    jz noadj
    xor cl,ch
    test cl,1
    jnz addadj
    fxch
    fsubp st1
    jmp wrapmath
zeradj:
    test ah,1
    jz noadj
    fchs
    jmp noadj
addadj:
    fadd st1
noadj:
    fxch
    popone
    jmp wrapmath
moderr:
    popone
    jmp domainerr