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

%include "version.asi"
%include "prints.ase"
%include "regs.ase"
%include "input.ase"
%include "breaks.ase"
%include "exec.ase"
%include "swift.ase"
%include "fp.ase"
%include "asm.asi"
%include "asm.ase"

    global	reflags,dreax,drebx,drecx,dredx
    global	dresi,dredi,drebp
    global	dresp,dreip
    global	monitor_init, rtoss, dres,drds,sysds, entry1, entry3, entry13
    global  drcs, drss, drfs, drgs
    extern   hdwechk

    extern _winxcept
    extern ___clearxcept
    
    segment data USE32
;
; CPU instruction trap enable flag
;
TRAPFLAG EQU 100h
;
; Macro which sets things up to call the generic trap handler
;
%macro TRAP 2
%ifidn %2,NO
        
cwentry%1:
        push    ds
        mov     ds,[cs:sysds]
        mov     word [_winxcept], 0
        pop     ds
        jmp     %%join
%endif
; 
entry%1:
    call	___clearxcept
%%join:
%if %1 = 3
    call	swiftrace
%endif
%if %1 = 1
    call	hdwechk
    jc	%%noswift
    call	swiftrace
%%noswift:
%endif
    push	ds		; Switch to system data seg
    mov	ds,[cs:sysds]
    mov	word [trapnum],%1	; Save trapnu,
    %ifidn %2,YES
    inc	word [haserr]	; Set the error flag
    %endif
%if %1 = 1
    mov	byte [tracing],1
%endif
    jmp	traphandler	; Jump to trap handler
%endmacro

;
; List of all trap handlers
;
tsvects	dd	16		; Not allowing TRAP 16 because is video int
    dd	entry0,entry1,entry2,entry3,0,0
    dd	entry6,entry7,0,0,0,0
        dd      entry12,entry13,entry14,0,0 
;
; Register image
;
reflags	dd	0
dreax	dd	0
drebx	dd	0
drecx	dd	0
dredx	dd	0
dresi	dd	0
dredi	dd	0
drebp	dd	0
dresp	dd	0
dreip	dd	0
drds	dw	0
dres	dw	0
drfs	dw	0
drgs	dw	0
drss	dw	0
drcs	dw	0
rtoss	dd	0
sysds	dw	0
;
haserr	dw	0	; If there is an error# on stack
errnum	dw	0	; The error#
trapnum	dw	0	; The trap#
tracing	db	0	; True if tracing
;	db	10,13,0

    segment code USE32
;
; Save an image of the regs
; This MUST BE the first thing the trap handler calls; it assumes
; there is ONE PUSH (return address) followed by the DS at the time
; of interrupt followed by the interrupt data
;
saveregs	:	
    mov	[dreax],eax	; Save GP regs
    mov	[drebx],ebx	;
    mov	[drecx],ecx	;
    mov	[dredx],edx	;
    mov	[dresi],esi	;
    mov	[dredi],edi	;
    mov	[drebp],ebp	;
    lea	ebp,[esp+4]	; Point BP at DS on stack
    mov	ebx,4		; Offset past DS on stack
    mov	eax,[ebp]
    mov [drss],ss
    mov [drcs],cs
    mov	[drds],ax
    mov	[dres],es
    mov [drfs],fs
    mov [drgs],gs
    test	word [_winxcept],1
    jnz	windowsxcept
    bt	word [haserr],0	; See if an error
    jnc	short noerr	;
windowsxcept:
          add	ebp,4		; Yes, point errno
    add	ebx,4		; Offset past errno
    mov	ax,[ebp]	; Get the error #
    mov	[errnum],ax	;
noerr:
    mov	eax,[ebp + 4]	; Get CS:eip
    mov	[dreip],eax	;
    mov	eax,[ebp + 12]	; Get flags
    mov	[reflags],eax	;
    add	ebx,12		; Offset past CS:eip & flags
    mov	eax,ebp		; things in the trap routine
    add	eax,16		;
    mov	[dresp],eax	;
    ret
;
; Adjust EIP to the trap if it's not int 3
;
;extrn adjusteip : :

adjusteip       :    
        cmp     word [trapnum],3     ; See if int 3
        jnz     short noadj     ; No, get out
        mov     esi,[dreip]     ;
        cmp     byte [esi-1],0cch ; See if is an INT 3
        jz      short noadj ; Get out if so
        cmp     word [esi-2],03cdh ; long form
        jz      short noadj
        dec     dword [dreip]         ; Else point at trap
noadj:
        ret
;
; Generic trap handler
;
traphandler	:	
    cld
    call	saveregs	; Save Regs
    push	ds
    pop	es
    add	esp,ebx		; Find usable top of stack
    mov	[rtoss],esp	;
    test	byte [tracing],1	; See if tracing
    jz		dbreak	;
    test	dword [reflags], TRAPFLAG; See if trap is set in flags
    jnz	short istracing	; No, not tracing
dbreak:	
    call	DisableBreaks	; Disable breakpoints if not
istracing:
    mov	byte [tracing],0	; Clear tracing flag
    and	dword [reflags],~ TRAPFLAG
    call	adjusteip	; Adjust the EIP to point to the breakpoint
    call	crlf
    cmp     word [trapnum],3	; No stats if it is int 3
    jz	short nostats	;
    cmp	word [trapnum],1	; Or int 1
    jz	short nostats	;
    Msg	"Trap: "
    mov	ax,[trapnum]	; Say which one
    call	PrintByte	;
    call	crlf
    btr	word [haserr],0	; If has error 
    jnc	nostats		;
    Msg	"Error: "
    mov	ax,[errnum]	; Say which one
    call	PrintWord
    call	crlf
nostats:
    test    byte [slashcmd],-1
    jz      notslashcmd
    call    slashcont
notslashcmd:
    call	DisplayRegisters; Display registers
    cmp	word [trapnum],1
    jnz	noretrap
    test	dword [TrapCount],-1	; in case a number specified on T command
    jz	noretrap
    call    ScanKey         ; if they pressed a key stop
    jnz     noreproceed
    dec	dword [TrapCount]
    jmp	ReTrap
noretrap:
    test    dword [proceedCount],-1
    jz      noreproceed
    call    ScanKey         ; if they pressed a key stop
    jnz     noreproceed
    dec     dword [proceedCount]
    jmp     proceedone
noreproceed:
    mov     dword [proceedCount],0
    mov	dword [TrapCount],0
    jmp	InputHandler	; Go do input

;
; Monitor init routine, point all traps to point to the monitor handler
;
monitor_init	:
    pushfd
    pop	dword [reflags]
    mov	[dreax],eax	; Save GP regs
    mov	[drebx],ebx	;
    mov	[drecx],ecx	;
    mov	[dredx],edx	;
    mov	[dresi],esi	;
    mov	[dredi],edi	;
    mov	[drebp],ebp	;
    pop	eax
    mov	[dreip],eax
    mov	[dresp],esp
    mov	[rtoss],esp
    mov	[sysds],ds
    mov	[drds],ds
    mov	[dres],es
    mov	ecx,[tsvects]		; Get the number of vectors
    mov	esi,tsvects + 4	; Get the offset to the vector handlers
    mov	edi,0			; Get the initial trap #
tilp:
    lodsd				; Read a trap handler
    or	eax,eax
    jz	noset
    push 	edi
    push 	ecx
    mov	edx,eax			;
    mov	ecx,cs			;
    mov	ebx,edi
    pushad
    mov	ax,203h			; try first as an except handler
    int 	31h
    popad
    jnc	mtok
overtake:
    mov	ax,205h			; if that fails (and it will in pmode307
    int 	31h			; try as an interrupt handler
mtok:
    pop	ecx
    pop	edi
noset:
    inc	edi			; Update trap#
    loop	tilp			; Loop till done
        ;
        ; support for causeway
        ;
        mov     ax,205h
        mov     ebx,3
        mov     edx,cwentry3
        mov     ecx,cs
        int     31h
        mov     ax,205h
        mov     ebx,0
        mov     edx,cwentry0
        mov     ecx,cs
        int     31h
    call	floatcheck
    Call	Message
    db	'Debug/386 Version '
    db	30h + VERID/10,'.',30h + VERID % 10
    db	' (DPMI) Copyright (c) 1997, 2006 LADsoft',13,10,0
;	mov	dword [TrapCount],4		; past the function call into main
;	call	ReTrap
    jmp	InputHandler
;
; Here are the individual trap handlers
;
TRAP	0,NO
TRAP	1,NO
TRAP	2,NO
TRAP	3,NO
TRAP	6,NO
TRAP	7,NO
TRAP	12,YES
TRAP	13,YES
TRAP	14,YES
 

END