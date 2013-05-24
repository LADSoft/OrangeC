;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
    
    
    
    
    
    
