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

[global exp]

SECTION data CLASS=DATA USE32
nm	db	"exp",0

SECTION code CLASS=CODE USE32

exp:
    fldl2e
    sub cl,cl
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

