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
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _atan2]
[export _atan2f]
[export _atan2l]
%endif
[global _atan2]
[global _atan2f]
[global _atan2l]

SECTION data CLASS=DATA USE32
nm	db	"atan2",0

SECTION code CLASS=CODE USE32
_atan2f:
    lea	ecx,[esp+4]
    lea	edx,[esp+8]
    fld	dword[edx]
    fld	dword[ecx]
    sub dl,dl
    jmp short atan2
_atan2l:
    lea	ecx,[esp+4]
    lea	edx,[esp+16]
    fld	tword[edx]
    fld	tword[ecx]
    mov dl,2
    jmp short atan2
_atan2:
    lea	ecx,[esp+4]
    lea	edx,[esp+12]
    fld	qword[edx]
    fld	qword[ecx]
    mov dl,1
atan2:
    lea	eax,[nm]
    call    clearmath
    ftst
    fstsw ax
    sahf
    fxch
    jnz ok
    ftst
    fstsw ax
    sahf
    jz baddomain
ok:
    fpatan
    jmp wrapmath
baddomain:
    popone
    jmp domainerr
