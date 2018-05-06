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
[export _csin]
[export _csinf]
[export _csinl]
%endif
[global _csin]
[global _csinf]
[global _csinl]
[extern exp]
SECTION data CLASS=DATA USE32
nm	db	"csin",0

SECTION code CLASS=CODE USE32

_csinf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    fld dword[ecx+4]
    jmp short csin
_csinl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    fld tword[ecx+10]
    jmp short csin
_csin:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    fld qword[ecx+8]
csin:
    lea eax,[nm]
    call clearmath
    push    ebp
    mov     ebp,esp
    sub     esp,20
    call    exp
    fld     st0
    fstp    tword [ebp-20]
    fld1
    fxch
    fdivp   st1
    fstp    tword [ebp-10]
    fld     tword [ebp-20]
    fld     tword [ebp-10]
    fsubp   st1
    fld st1
    fcos
    fmulp st1
    fld1
    fchs
    fxch
    fscale
    fxch
    fcomp   st0
    fxch
    fsin
    fld     tword [ebp-10]
    fld     tword [ebp-20]
    faddp   st1
    fmulp st1
    fld1
    fchs
    fxch
    fscale
    fxch
    fcomp   st0
    leave
    jmp wrapcomplex
