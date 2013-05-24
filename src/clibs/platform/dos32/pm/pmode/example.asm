; This little program just prints a string to DOS using function 0300h.
; Then it allocates a real mode callback, calls the real mode callback from
; protected mode, and prints a string out of the protected mode callback
; routine. Not the standard usage of callbacks, but just to demonstrate.

.386p
locals

STACKLEN        = 40h                   ; size of stack in paragraphs

PMODE_TEXT      segment para public use16 'TRAN'
PMODE_TEXT      ends
_TEXT           segment para public use16 'TRAN'
_TEXT           ends
EXE_STACK       segment para stack use16 'STACK'
EXE_STACK       ends

extrn   _pm_info:far, _pm_init:far

_TEXT           segment para public use16 'TRAN'
assume  cs:_TEXT, ds:_TEXT
org     0

;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
errmsgtbl       dw      errmsg0,errmsg1,errmsg2,errmsg3
                dw      errmsg4,errmsg5,errmsg6

errmsg0         db      'Not enough low memory!',13,10,36
errmsg1         db      '80386 or better not detected!',13,10,36
errmsg2         db      'System already in protected mode and no VCPI or DPMI found!',13,10,36
errmsg3         db      'DPMI host is not 32bit!',13,10,36
errmsg4         db      'Could not enable A20 gate!',13,10,36
errmsg5         db      'Could not enter DPMI 32bit protected mode!',13,10,36
errmsg6         db      'Could not allocate needed DPMI selectors!',13,10,36

msg0            db      'Hello World from Protected Mode...',13,10
                db      'Press any key to continue...',13,10,36
msg1            db      'Hello World from a Real Mode Callback...',13,10
                db      'Press any key to exit...',13,10,36

align 4
regstruc_edi    label   dword
regstruc_di     dw      ?,?
regstruc_esi    label   dword
regstruc_si     dw      ?,?
regstruc_ebp    label   dword
regstruc_bp     dw      ?,?
                dd      ?
regstruc_ebx    label   dword
regstruc_bx     label   word
regstruc_bl     db      ?
regstruc_bh     db      ?,?,?
regstruc_edx    label   dword
regstruc_dx     label   word
regstruc_dl     db      ?
regstruc_dh     db      ?,?,?
regstruc_ecx    label   dword
regstruc_cx     label   word
regstruc_cl     db      ?
regstruc_ch     db      ?,?,?
regstruc_eax    label   dword
regstruc_ax     label   word
regstruc_al     db      ?
regstruc_ah     db      ?,?,?
regstruc_flags  dw      ?
regstruc_es     dw      ?
regstruc_ds     dw      ?
regstruc_fs     dw      ?
regstruc_gs     dw      ?
regstruc_ip     dw      ?
regstruc_cs     dw      ?
regstruc_sp     dw      ?
regstruc_ss     dw      ?

dataselector    dw      ?

;อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
start:                                  ; execution starts here
        push cs                         ; DS = CS
        pop ds

        call _pm_info                   ; get information
        jnc short @@startf0             ; if no error, go on

@@startf1:
    mov si,ax			; print error message for code AX
        add si,ax
        mov dx,errmsgtbl[si]
        mov ah,9
        int 21h
        mov ax,4cffh
        int 21h

@@startf0:
    xor ax,ax			; check low memory and allocate low
    mov cx,ss			;  buffer needed for protected mode
        add cx,STACKLEN
        mov dx,es:[2]
        sub dx,cx
        cmp dx,bx
        jb @@startf1
        mov es,cx

    call _pm_init			; enter protected mode
    jc @@startf1			; if error, go to error message

        mov dataselector,ds             ; now in protected mode, preserve data
                                        ;  selector

        push ds                         ; ES = DS for register structure
        pop es

        mov edi,offset regstruc_edi     ; offset of register structure
        xor cx,cx                       ; no parameters on stack
        mov bx,21h                      ; call interrupt 21h
        mov ax,300h                     ; INT 31h function 0300h

        mov regstruc_ah,9               ; function code 9, put string
        mov regstruc_ds,_TEXT           ; set DS:DX for DOS string put
        mov regstruc_dx,offset msg0
        mov regstruc_ss,0               ; SS:SP = 0, PMODE will provide stack
        mov regstruc_sp,0

        int 31h                         ; do the call to real mode


        xor ah,ah                       ; wait for keypress
        int 16h


        push ss                         ; ES = SS for register structure
        pop es

        sub esp,32                      ; stack space for register structure
        mov edi,esp                     ; EDI = offset of register structure

        push cs                         ; DS:ESI = callback routine
        pop ds
        mov esi,offset callback

        mov ax,303h                     ; INT 31h function 0303h
        int 31h                         ; allocate real mode callback


        sub esp,32                      ; stack space for register structure
        mov edi,esp                     ; EDI = offset of register structure

        mov dword ptr [esp+2eh],0       ; SS:SP = 0 in register structure
        mov [esp+2ah],dx                ; IP is offset of real mode callback
        mov [esp+2ch],cx                ; CS is segment of real mode callback
        mov word ptr [esp+20h],0        ; zero FLAGS in register structure

        xor cx,cx                       ; no parameters on stack
        xor bh,bh                       ; BH must be 0
        mov ax,301h                     ; INT 31h function 0301h

        int 31h                         ; call real mode callback


        add esp,64                      ; free stack space

        mov ah,4ch                      ; exit to DOS
        int 21h

;ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ
callback:
        cld
        lods dword ptr ds:[esi]         ; get return seg:off from real mode
        mov dword ptr es:[edi+2ah],eax  ;  stack, and update return address
        add word ptr es:[edi+2eh],4     ; adjust SP in register structure

        push es edi                     ; preserve register structure ES:EDI


        mov es,cs:dataselector          ; ES = main stream data selector

        mov edi,offset regstruc_edi     ; offset of register structure
        xor cx,cx                       ; no parameters on stack
        mov bx,21h                      ; call interrupt 21h
        mov ax,300h                     ; INT 31h function 0300h

        mov es:regstruc_dx,offset msg1  ; offset of new string, other fields
                                        ;  were set previously
        int 31h                         ; do the call to real mode


        xor ah,ah                       ; wait for keypress
        int 16h


        pop edi es                      ; restore register structure ES:EDI

        iretd                           ; done with callback

_TEXT           ends

EXE_STACK       segment para stack use16 'STACK'
                db      STACKLEN*10h dup(?)
EXE_STACK       ends

end     start
