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
[export _tanh]
[export _tanhf]
[export _tanhl]
%endif
[global _tanh]
[global _tanhf]
[global _tanhl]
[extern exp]
SECTION data CLASS=DATA USE32
nm	db	"tanh",0

SECTION code CLASS=CODE USE32

_tanhf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short tanh
_tanhl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short tanh
_tanh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
tanh:
    lea eax,[nm]
    call    clearmath
    call	exp
    fld1
    fld st1
    fdivp st1
    fxch
    fld	st0
    fsub	st0,st2
    fxch	st1
    fadd	st0,st2
    fdivp	st1
    fxch	st1
    popone
    jmp     wrapmath
