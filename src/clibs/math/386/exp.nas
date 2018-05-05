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
[export _exp]
[export _expf]
[export _expl]
%endif
[global _exp]
[global _expf]
[global _expl]
[global exp]
[global exp2]
[global expm1]

SECTION data CLASS=DATA USE32
nm	db	"exp",0

SECTION code CLASS=CODE USE32

expm1:
    fldl2e
    mov cl,1
    jmp exp2x
exp:
    fldl2e
exp2:
    sub cl,cl
exp2x:
    fmulp	st1
    enter	4,0
    fstcw	[ebp-4]
    mov	ax,[ebp-4]
    and	ah,0f3h
    or	ah,0ch
    mov	[ebp-2],ax
    fldcw	[ebp-2]
    fld	st0
    frndint
    fxch	st1
    fsub	st0,st1
    f2xm1
    fxch
    fxam
    fnstsw ax
    fxch
    test ah,40h
    jz doadd1
    test cl,1
    jnz noadd1
doadd1:
    or cl,2
    fld1
    faddp	st1,st0
noadd1:
    call checkedscale
    jc  xit
    cmp cl,3
    jnz nosub1
    fld1
    fsubp st1
nosub1:
    fxch	st1
    popone
xit:
    fldcw	[ebp-4]
    leave
    ret

_expf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short xexp
_expl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short xexp
_exp:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
xexp:    
    lea eax,[nm]
    call    clearmath
    call	exp
    jmp wrapmath