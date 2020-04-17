; Software License Agreement
; 
;     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

%ifdef __BUILDING_LSCRTL_DLL
[export _wcsstr]
%endif
[global _wcsstr]
section code CLASS=CODE USE32

_wcsstr:
	push	ebp
	mov	ebp,esp
	push	esi
	push	edi
	mov	esi,[ebp+8]
	mov	edi,[ebp+12]
	test	edi,edi
	jz	exit	
	mov	ax,[edi]
	test	ax,ax
	jz	exit	
	push	edi
	sub	ax,ax
	mov	ecx,-1
	cld
	repne	scasw
	not	ecx
	dec	ecx
	pop	edi
lp:
	mov	ax,[esi]
	or	ax,ax
	je	badexit
	cmp	ax,[edi]
	jne	nocomp
	push	ecx
	push	esi
	push	edi
	rep	cmpsb
	pop	edi
	pop	esi
	pop	ecx
	je	exit
nocomp:
	inc esi
    inc esi
	jmp	lp
badexit:
	sub	esi,esi
exit:
	mov	eax,esi
	pop	edi
	pop	esi
	pop	ebp
	ret
