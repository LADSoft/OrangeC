;  Software License Agreement
;  
;      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

global	TableDispatch, nofunction

    segment code USE32
;
; Core dispatch routine.  Calls the subfunction indicated in AL
; and then set the return address to after the dispatch table
; This expects a subfunction code to be on the stack
;
TableDispatch:
    ENTER	0,0
    xchg	ebx,[ebp+4]		; xchg ret address & ebx
    cmp	al,[ebx]		; Limit check
    ja	short noaction		; Error if too big
    ; Here we call the routine
    push	finishup		; Return address
    sub	ah,ah			; Make key a dword
    cwde				;
    push	dword [ebx + eax * 4 + 4]	; Get code address to stack
    xchg	ebx,[ebp+4]		; put things as they were
    mov	eax,[ebp + 8]		; Get the subkey
    cld				; Assume move dir up
    ret				; Go to subroutine
    
noaction:
    xchg	ebx,[ebp+4]		; Put things as they were
    call	nofunction		; Register bad function error
finishup:
    ; Now we have to find the return address
    xchg	ebx,[ebp+4]		; Get return address
    push	eax
    mov	eax,[ebx]
    lea	ebx,[ebx + eax * 4 + 8]	; Get offset to return address
    pop	eax
    xchg	ebx,[ebp+4]		; Xchg with orig value of ebx
    LEAVE
    ret	4
nofunction:
    mov	eax,-1			; Ill function error
    stc				; Set carry flag
    ret
    
    
    
    
    
    
