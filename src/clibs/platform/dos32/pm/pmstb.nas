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

SELSTOALLOC 	EQU 2
MAXMEM		EQU 32
BUFFERSIZE	EQU	512
NAMESIZE 	EQU	256
BUFFERSHIFT	EQU	9

%macro GDTENTRY 3
    dw	%2 & 0ffffh
    dw	%1 & 0ffffh
    db	(%1 >> 16) & 0ffh
    dw	%3 | ((%2 >> 8) & 0f00h)
    db	%1 >> 24
%endmacro

segment	EXE_STACK stack align=16 CLASS=STACK use16
segment	PMODE_TEXT align=16 CLASS=TRAN use16
extern   _pm_info, _pm_init, _pm_rmstacks, _pm_pmstacks
extern	_pm_pagetables, _pm_mode
segment END_TEXT align = 16 CLASS=TRAN use16
eop		dd	0
segment dummy align = 16 CLASS = TRAN use32
;group DGROUP dummy

;group STUB_GROUP PMODE_TEXT END_TEXT
segment PMODE_TEXT
;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;
; Header for external debuggers... must be FIRST THING after EXE header
;
_main equ 0
__stklen equ 0
%ifdef DEBUGXXX
        dd	'XDBG'			; signature
        dd	'i386'
        dw	1			; some Version info
        dw	DGROUP			; to locate entry + globals
        dd	BssEnd	; to determine memory size
        dd	__stklen		; to determine stack size
        dd	start32	; true entry point
        dd	_main	; where to set a breakpoint
%endif
;
; local vars
;
banner    db  'Stub-386, Copyright (c) 1997-2004, LADsoft',10,13
        db	'DPMI subsystem Copyright (c) 1994, TRAN (Thomas Pytel)',10,13,'$'
errmsgtbl       dw      errmsg0,errmsg1,errmsg2,errmsg3
                dw      errmsg4,errmsg5,errmsg6,errmsg7
                dw		errmsg8,errmsg9,errmsg10,errmsg11

errmsg0         db      'Not enough low memory!',13,10,36
errmsg1         db      '80386 or better not detected!',13,10,36
errmsg2         db      'System already in protected mode and no VCPI or DPMI found!',13,10,36
errmsg3         db      'DPMI host is not 32bit!',13,10,36
errmsg4         db      'Could not enable A20 gate!',13,10,36
errmsg5         db      'Could not enter DPMI 32bit protected mode!',13,10,36
errmsg6         db      'Could not allocate needed DPMI selectors!',13,10,36
errmsg7		db	'MS-DOS 3.0 required for this program',10,13,'$'

errmsg8		db	'Not enough extended memory',10,13,36
errmsg9		db	'Unexpected error while initializing dpmi',10,13,36
errmsg10	db  'Invalid exe file format',10,13,36
errmsg11	db  'File I/O error', 10,13,36

regstruc_edi:
regstruc_di     dw      0,0
regstruc_esi:
regstruc_si     dw      0,0
regstruc_ebp:
regstruc_bp     dw      0,0
                dd      0
regstruc_ebx:
regstruc_bx:
regstruc_bl     db      0
regstruc_bh     db      0,0,0
regstruc_edx:
regstruc_dx:
regstruc_dl     db      0
regstruc_dh     db      0,0,0
regstruc_ecx:
regstruc_cx:
regstruc_cl     db      0
regstruc_ch     db      0,0,0
regstruc_eax:     
regstruc_ax:
regstruc_al     db      0
regstruc_ah     db      0,0,0
regstruc_flags  dw      0
regstruc_es     dw      0
regstruc_ds     dw      0
regstruc_fs     dw      0
regstruc_gs     dw      0
regstruc_ip     dw      0
regstruc_cs     dw      0
regstruc_sp     dw      0
regstruc_ss     dw      0

sel32		dw	0
selx		dw	0
psp		dw	0
modeval		db	0
newcodebase	dw	0
    align 4
linearbase	dd	0
blockhandle	dd	0
blocksize	dd	0
initsize	dd	0
seekBase	dd	0
start32		dd	0
fileHandle	dw	0

cdesc:
    GDTENTRY	0,0fffffh,0c09Ah	; 386 code
ddesc:
    GDTENTRY	0,0fffffh,0c092H	; 386 data

;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
;
; program start
;
..start:                                  ; execution starts here

        push cs                         ; DS = CS
        pop ds
    mov [psp],es

    mov ah,30h   			; Check for dos 2.0
    int 21h
    cmp al,3
    jnc near .dosok
    mov ax,7			; err if not
    jmp error

.dosok:
    call get_filename			; get our exe file name
;
; Init to try for VCPI
;
    mov	byte [_pm_pagetables],MAXMEM/4	; Max out at 32 MB
    mov	byte [_pm_mode],1		; select VCPI if it exists
    mov	byte [_pm_rmstacks],6
    mov	byte [_pm_pmstacks],6

        call far _pm_info                   ; get information
        jnc startf0             ; if no error, go on

error:
        mov si,ax			; print error message for code AX
        add si,ax
        mov si,[si + errmsgtbl]
errlp:
        lodsb
        cmp	al,'$'
        jz	errxit
        mov	ah,2
        mov	dl,al
        int 21h
        jmp	errlp
errxit:
        mov ax,4cffh
        int 21h
fileIoErr:
    mov	ax,11
    jmp	error
invExe:
    mov ax,10
    jmp	error
initbad:
    mov	ax,9
    jmp	error
startf0:
    mov [newcodebase],ax
    call optimize_realmem		; optimize mem for spawns
    jnc near .startf2
    sub	ax,ax    		; number for no mem error
    jmp 	error
.startf2:
    call far _pm_init			; enter protected mode
    jc near error			; if error, go to error message


;
; At this point we are in pmode
;
    mov [modeval],ch		; VCPI/DPMI/RAW/XMS flag
                    ; = 3 for DPMI, 2 for VCPI

    mov [psp],es			; save our PSP for later

    push ds
    pop es

    mov ax,cs       		; fix the priv levels of our descriptors
    and al,3	
    shl al,5
    or  byte [cdesc+5],al
    or  byte [ddesc+5],al
;
; Get program size
;
    call OpenFile
    jc	fileIoErr
    mov	ecx,20
    mov	edx,0
    call ReadFile
    jc	fileIoErr
    cmp	word [si], 'LS'
    jnz	invExe
    cmp word [si+2], 'PM'
    jnz invExe
    cmp word [si+4], 20
    jnz	invExe
    cmp word [si + 6], 0
    jnz	invExe
    mov ecx,[si + 12]
    mov	[initsize],ecx
    mov ecx,[si + 16]
    mov [start32],ecx
    test ecx,0ffff0000h	; start address must be < 65536
    jnz	invExe
    mov	ecx,[esi + 8]
    mov [blocksize],ecx
    mov ebx,ecx
    shr ebx,16
;
; Allocate memory for the program
;
; the program WILL run in extended memory
;
    mov ax,501h			; allocate memory and save the
    int 31h				; base address of it
    jnc near .gotmem			; Should not get an error from this
                    ; but get out if we do
    mov ax,8		; no extended memory
    jmp	error
.gotmem:
    mov ax,bx
    shl eax,16
    mov ax,cx
    mov [linearbase],eax
    mov ax,si
    shl eax,16
    mov ax,di
    mov [blockhandle],eax
;
; get selector increase value
;
    mov ax,3			; Now get the selector increase value
    int 31h
    mov [selx],ax
;
; Allocate selectors and initialize the descriptors for
; 32-bit segments
;
    mov ax,0			; Allocate the 32-bit selectors
    mov cx,SELSTOALLOC		; CODE, DATA/stack, 
    int 31h
    jc near initbad
    mov [sel32],ax

    mov bx,[sel32]			; set CS descriptr
    mov di,cdesc		; 
    movzx edi,di
    mov ax,000ch			;
    int 31h
    jc near initbad			; Should not get an error from this

    mov bx,[sel32]			; set DS descriptor
    mov di,ddesc		; 
    movzx edi,di
    add bx,[selx]
    mov ax,000ch
    int 31h
    jc near initbad			; Should not get an error from this
    
; Set the base of the code and data segments to the linear address
; of the memory block
;
    mov eax,[linearbase]		; code segment
    mov dx,ax
    shr eax,16
    mov cx,ax
    mov bx,[sel32]
    mov ax,7
    int 31h
    jc near initbad
    
    mov eax,[linearbase]		; data segment
    mov dx,ax
    shr eax,16
    mov cx,ax
    mov bx,[sel32]
    add bx,[selx]
    mov ax,7
    int 31h
    jc near initbad
;
; blit the program to our memory block
;
    mov	ebx,[initsize]
    add	ebx,BUFFERSIZE-1
    shr	ebx,BUFFERSHIFT
    mov ax,[sel32]
    add ax,[selx]
    mov es,ax
    mov	edi,0
    mov	edx,20
rdlp:
    mov	ecx,BUFFERSIZE
    call	ReadFile
    jc	fileIoErr
    mov	ecx,BUFFERSIZE/4
    cld
    db  67h				; shift us to 32 bits...
    rep movsd			; do the blit
    add	edx,BUFFERSIZE
    dec	ebx
    ja	rdlp
    call	CloseFile
;	mov 	byte [es:10h],0cbh
;
; now call the 32-bit C0 routine
;
; must return with a RETF
;
    push ds

    push word 0  			; return address... returning from
    push cs				; a 32 bit segment so two dwords
    sub eax,eax
    mov ax,retpos
    push eax

    push word [sel32]			; address of 32-bit code
                    ; returning from this seg to 
    mov eax,[start32] 		; a 32-bit seg, two words
    push ax

    mov ax,[sel32]			; set up linear sel for DS load
    add ax,[selx]
    mov es,[psp]        		; ES: PSP
    mov ds,ax
    retf

retpos:					; when 32-bit code exits with a retf
    pop ds				; we come here
getout:
    push ax				; save return code
    mov bx,[sel32]
    mov cx,SELSTOALLOC
    mov ax,1
selfrlp:
    pusha
    int 31h
    popa
    add bx,[selx]
    loop selfrlp

releasemem:
    mov eax,[blockhandle]		; Free the program block
    or eax,eax
    jz near endprog
;
; release the memory
;
    mov eax,[blockhandle]		; Free the program block
    mov di,ax
    shr eax,16
    mov si,ax
    mov ax,502h
    int 31h

endprog:
        pop ax
        mov ah,4ch                      ; exit to DOS
        int 21h

message:
        mov di,regstruc_edi     ; offset of register structure
        movzx edi,di
        xor cx,cx                       ; no parameters on stack
        mov bx,21h                      ; call interrupt 21h
        mov ax,300h                     ; INT 31h function 0300h

        mov byte [regstruc_ah],9               ; function code 9, put string
        mov word [regstruc_ds],PMODE_TEXT           ; set DS:DX for DOS string put
        mov word [regstruc_ss],0               ; SS:SP = 0, PMODE will provide stack
        mov word [regstruc_sp],0

    push ds  
    pop es
        int 31h                         ; do the call to real mode
    ret
OpenFile:
    push es
    push ds
    pop es
    mov di,regstruc_edi     ; offset of register structure
    movzx	edi,di
    xor cx,cx                       ; no parameters on stack
    mov bx,21h                      ; call interrupt 21h
    mov ax,300h                     ; INT 31h function 0300h
    mov byte [regstruc_ah],3dh               ; function code 3dh, open file
    mov word [regstruc_ds],PMODE_TEXT           ; set DS:DX for name
    mov word [regstruc_dx],eop + BUFFERSIZE
    mov word [regstruc_ss],0               ; SS:SP = 0, PMODE will provide stack
    mov word [regstruc_sp],0
    int 31h
    test	byte [regstruc_flags],1
    stc
    jnz	fox
    mov ax,[regstruc_ax]
    mov	[fileHandle],ax
    mov dword [seekBase],0
    mov	ecx,4
    mov edx,03ch	; pointer to program header
    call	ReadFile
    jc	fox
    mov	eax,[si]
    mov	[seekBase],eax
    clc
fox:
    pop es
    ret
    
ReadFile:
    push es
    push edx
    push ebx
    push edi
    push ds
    pop es
    mov	di,regstruc_edi     ; offset of register structure
    movzx	edi,di
    mov byte [regstruc_ah],3fh               ; function code 3dh, read file
    mov word [regstruc_ds],PMODE_TEXT           ; set DS:DX for name
    mov word [regstruc_dx],eop
    mov word [regstruc_cx],cx
    mov ax,[fileHandle]
    mov word [regstruc_bx],ax
    mov word [regstruc_ss],0               ; SS:SP = 0, PMODE will provide stack
    mov word [regstruc_sp],0
    
    mov ax,4200h		; seek
    mov bx,[fileHandle]
    add	edx,[seekBase]
    mov cx,dx
    shr edx,16
    xchg cx,dx
    int	21h
    jc .xit	
    xor cx,cx                       ; no parameters on stack
    mov bx,21h                      ; call interrupt 21h
    mov ax,300h                     ; INT 31h function 0300h
    int 31h
    mov ah,[regstruc_flags]
    sahf
.xit:
    mov	esi,eop
    pop edi
    pop	ebx
    pop	edx
    pop	es
    ret
CloseFile:
    mov	bx,[fileHandle]
    mov ax,3e00h
    int 21h
    ret
WriteHex:
    push	eax
    shr		eax,16
    call	whw
    pop		eax
whw:
    push	eax
    shr		eax,8
    call	whb
    pop		eax
whb:
    push	eax
    shr		eax,4
    call	whn
    pop		eax
whn:
    and		ax,15
    cmp		ax,9
    jbe		qq
    add		ax,7
qq:
    add		ax,30h
    mov		dl,al
    mov		ah,2
    int		21h
    ret
;ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
;
; real mode subroutines
;
optimize_realmem:
    push    bx
    mov 	bx,ss
    mov 	ax,sp
    add		ax,15
    shr		ax,4
    add		bx,ax
    pop     ax
    push    bx
    add     bx,ax
    sub bx,[psp]
    mov es,[psp]
    mov ah,4ah
    int 21h
    pop es
    ret
get_filename:
    mov	es,[psp]
    mov	es,[es:2ch]
    mov	di,0
    cld
.lp1:
    mov	cx,-1
    mov	al,0
    repnz scasb
    test	byte [es:di],0ffh
    jnz	.lp1
    add	di,3
    mov	si,di
    mov	di,eop + BUFFERSIZE
    push ds
    push ds
    push es
    pop ds
    pop es
.lp2:
    lodsb
    stosb
    or al,al
    jnz .lp2
    pop ds
    ret
