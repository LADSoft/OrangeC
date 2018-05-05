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

%ifdef __BUILDING_LSCRTL_DLL
[export _strrchr]
%endif
[global _strrchr]
section code CLASS=CODE USE32

_strrchr:
	mov		eax,[esp+4]
	dec	eax
lp1:
	inc	eax
	test al,3
	jnz x1
lp2:
	mov	ecx,[eax]
	add	eax,BYTE 4
	mov edx,ecx
	sub	edx,001010101h
	not ecx
	and edx,080808080h
	and edx,ecx
	jz lp2
	sub eax,BYTE 4
x1:
	cmp	byte [eax],BYTE 0
	jne	lp1

	inc	eax
	mov	edx,[esp+4]
	mov	ecx,[esp+8]
lp:
	dec	eax
	cmp		edx,eax
	ja		none
	cmp		cl,[eax]
	jne		lp
	ret
none
	sub		eax,eax
	ret	
