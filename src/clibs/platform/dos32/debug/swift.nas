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

%include "exec.ase"
%include "mtrap.ase"

    global	swiftrace
    global traceon
    global untrace
    segment data USE32
traceon	db	0	;is swift tracing enabled
int3trace db	0	;are we in the middle of a full-speed run to int3
image	db	0	;image at place int 3 is found
oldip	dd	0
oldsp	dd	0	; put there during swift trace...

;
;
    segment code USE32
        extern _winxcept
untrace	:
    test	byte [int3trace],1	;
    jz	nounload2	;
    push	eax		; registers picked because this used in
    mov	ebp,[oldip]
    mov	al,[image]	;
    mov	[ebp],al	;
    mov	byte [int3trace],0	;
    pop	eax
nounload2:
    mov	byte [traceon],0
    ret
swiftrace:
    test	byte [traceon],0ffh
    jz	near notrace
    push	ds
    push	es
    pushad			; get user CS:IP
    mov		ds,[cs:sysds]
    mov		es,[cs:sysds]
    lea		ebp, [esp + 32 + 8 + 4]
    test	word [_winxcept], 1
    jz		notwxc
    add		ebp, 4
notwxc:
    mov	ebx,[ebp]	; unload int 3 call again
    test	byte [int3trace],1	;
    jz	nounload	;
    mov	ebx,[oldip]
    cmp	ebp,[oldsp]	; this is a hack in case we run into an int 3
    jnz	noupdate	; during a subroutine run
;	mov	ax,ss
;	cmp	ax,[oldss]
;	jnz	noupdate
    mov	[ebp],ebx	; point back where int 3 was
noupdate:
    mov	al,[image]	;
    mov	[ebx],al	;
    mov	byte [int3trace],0	;
nounload:
    mov	al,[ebx]
    cmp	al,0cch
    jz	near found3
    
    call	WadePrefix	; wade through prefixes
    cmp	al,9dh		; ehcek for popf
    jnz	notpopf
    or	word [ebp + 8],100h	; make sure popf will trace
    jmp	stx
notpopf:
    cmp	al,0c2h		; now check for rets
    jz	retx
    cmp	al,0c3h
    jz	retx
    cmp	al,0cbh
    jz	retx
    cmp	al,0cah
    jz	retx
    cmp	al,0cfh		; we WILL check iret here
    jnz	tracex		; none of those, check for call/int/string stepping
    or	word [ebp + 8],100h ; be sure iret will trace
retx:
           mov	byte [traceon],0	; else kill trace flag and trace past ret/iret
tracex:	
    or	word [ebp + 8],100h	; set trace flag
    call	callcheck	; see if call.int
    jz	setcall		; yep, set a break there
    call    cmpstring	; see if is string instruction
    mov	eax,1
    jz	short setcall	; yes, set an int 3 instruction in place
stx:
    popad
    pop	es
    pop	ds
    add		esp,4		; past ret
    test	word [cs:_winxcept],1
    jz		noxcept
    add		esp,4		; past exception code
noxcept:
    iretd
setcall:        
    add	ebx,eax
    mov	al,[ebx]
    mov	[image],al
    mov	byte [ebx],0cch
    inc	byte [int3trace]
    mov	[oldip],ebx
    mov	[oldsp],ebp
    and	word [ebp + 8],0FEFFH	; no trace here
    jmp	stx
found3:
    mov	byte [traceon],0
    popad
    pop	es
    pop	ds
notrace:
    ret
