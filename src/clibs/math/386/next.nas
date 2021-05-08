; Software License Agreement
; 
;     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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
[export _nextafter]
[export _nextafterf]
[export _nextafterl]
[export _nexttoward]
[export _nexttowardf]
[export _nexttowardl]
%endif
[global _nextafter]
[global _nextafterf]
[global _nextafterl]
[global _nexttoward]
[global _nexttowardf]
[global _nexttowardl]
SECTION data CLASS=DATA USE32
nm	db	"nexttoward",0
;1111111
;0010111
;1101000
; 340

; 3ff
;  34
; 3dc

; 3fff
;   40
; 3fbf
small_f dd 34000000h
really_small_f dd 1
small_d dd 0,3cb00000h
really_small_d dd 1,0
small_l dw 0,0,0,8000h,3fc0h
really_small_l dw 1,0,0,0,0

SECTION code CLASS=CODE USE32
_nextafterf:
    fld	dword [esp+4]
    fld	dword [esp+8]
    sub esp,16
    fstp tword[esp+4]
    fstp dword[esp]
    call _nexttowardf
    add esp,16
    ret
_nextafterl:
_nextafter:
    fld	qword [esp+4]
    fld	qword [esp+12]
    sub esp,20
    fstp tword[esp+8]
    fstp qword[esp]
    call _nexttoward
    add esp,20
    ret
_nexttowardf:
    lea eax,[nm]
    call clearmath
    sub dl,dl
    fld dword [esp+4]
    fld tword [esp+8]
    fcomp st1
    fstsw ax
    sahf
    je wrapmath
    jc fdown
    ftst
    fstsw ax
    sahf
    je frsmall
    fld st0
    fxtract
    popone
    fld dword [small_f]
    fscale
    fxch
    popone
    faddp st1
    jmp wrapmath
frsmall:
    popone
    fld dword [really_small_f]
    jmp wrapmath
fdown:
    ftst
    fstsw ax
    sahf
    je mfrsmall
    fld st0
    fxtract
    popone
    fld dword [small_f]
    fscale
    fxch
    popone
    fsubp st1
    jmp wrapmath
mfrsmall:
    popone
    fld dword [really_small_f]
    fchs
    jmp wrapmath
_nexttowardl:
_nexttoward:
    lea eax,[nm]
    call clearmath
    mov dl,1
    fld qword [esp+4]
    fld tword [esp+12]
    fcomp st1
    fstsw ax
    sahf
    je wrapmath
    jc down
    ftst
    fstsw ax
    sahf
    je rsmall
    fld st0
    fxtract
    popone
    fld qword [small_d]
    fscale
    fxch
    popone
    faddp st1
    jmp wrapmath
rsmall:
    popone
    fld qword [really_small_d]
    jmp wrapmath
down:
    ftst
    fstsw ax
    sahf
    je mrsmall
    fld st0
    fxtract
    popone
    fld qword [small_d]
    fscale
    fxch
    popone
    fsubp st1
    jmp wrapmath
mrsmall:
    popone
    fld qword [really_small_d]
    fchs
    jmp wrapmath
