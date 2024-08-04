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

%ifdef __BUILDING_LSCRTL_DLL
[export __fpureset]
[export __fpuclear]
[export __fpustatus]
[export __fpucontrol]
%endif
[global __fpureset]
[global __fpuclear]
[global __fpustatus]
[global __fpucontrol]

SECTION code CLASS=CODE USE32

__fpureset:
    fninit
    ret

__fpuclear:
    push	dword 0
    fnstcw	[esp]
    pop	eax
    fnclex
    ret

__fpustatus:
    fnstsw	ax
    movzx	eax,ax
    ret

__fpucontrol:
    mov	ebx,[esp+8]
    mov	ecx,[esp+4]
    push	dword 0
    fnstcw	[esp]
    pop	eax
    and	cx,bx
    neg	bx
    and	ax,bx
    or	ax,cx
    push	eax
    fldcw	[esp]
    pop	eax
    leave
    ret
