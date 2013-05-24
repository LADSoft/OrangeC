; PMODE v3.07 DPMI/VCPI/XMS/raw protected mode interface kernel.
; Copyright (c) 1994, Tran (a.k.a. Thomas Pytel).

global  _pm_selectors, _pm_pagetables, _pm_rmstacklen, _pm_rmstacks
global  _pm_callbacks, _pm_pmstacklen, _pm_pmstacks, _pm_mode
global  pmstackbase, pmstacktop, rmstackbase, rmstacktop

global _pm_info, _pm_init

segment	PMODE_TEXT align=16 CLASS=TRAN use16

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; DATA
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
SELCODE         EQU 08h                   ; PMODE_TEXT code selector
SELZERO         EQU 10h                   ; selector of entire memory space
SELCALLBACKDS   EQU 18h                   ; callback DS selector
SELREAL         EQU 20h                   ; real mode attributes selector
SELVCPITSS      EQU 28h                   ; TSS selector for VCPI
SELVCPICODE     EQU 30h                   ; VCPI call code selector
SYSSELECTORS    EQU 9                     ; number of system selectors in GDT

align 4
dpmiepmode      dd      0               ; DPMI enter pmode addx
codebase        dd      0               ; PMODE_TEXT linear address
vcpistrucaddx   dd      vcpi_cr3    ; VCPI switch structure linear address
vcpiswitchstack dd      0               ; VCPI temporary mode switch stack
oldint15vector  dd      0               ; preserved INT 15h vector
int3vector      dd      intrmatrix+3    ; protected mode INT 3 vector
                dw      SELCODE
                dw      0               ; just here for alignment

pmstacklen      dd      0               ; protected mode stack length in bytes
pmstackbase		dd		0			; bottom of protected mode stack area
pmstacktop      dd      0               ; top of protected mode stack area
callbackbase    dd      0               ; base of real mode callbacks
callbackseg     dw      0               ; segment of callbacks

selzero         dw      SELZERO         ; for immediate segreg loading
selcallbackds   dw      SELCALLBACKDS   ; for immediate segreg loading

rawextmemused   dw      1               ; raw extended memory used in K
rawextmembase   dd      0ffffffffh      ; raw extended memory base
rawextmemtop    dd      0               ; raw extended memory top

rmstackbase     dw      0               ; bottom of real mode stack area
rmstacktop      dw      0               ; top of real mode stack area
rmstackparmtop  dw      0               ; for functions 0300h, 0301h, 0302h

gdtseg          dw      0               ; segment of GDT

gdtlimit        dw      0               ; GDT limit                          |
gdtbase         dd      0               ; GDT base                           |
idtlimit        dw      7ffh            ; IDT limit                         |
idtbase         dd      0               ; IDT base                          |
rmidtlimit      dw      3ffh            ; real mode IDT limit                |
rmidtbase       dd      0               ; real mode IDT base                 |

rmtopmswrout    dw      v_rmtopmsw  ; addx of real to protected routine
pmtormswrout    dd      v_pmtormsw  ; addx of protected to real routine

pagetablebase   dd      0               ; base of page table area
pagetabletop    dd      0               ; top of page table area
pagetablefree   dd      0               ; base of available page table area
                db      0               ; just here for alignment

_pm_pagetables  db      1               ; number of page tables under VCPI
_pm_selectors   dw      64              ; max selectors under VCPI/XMS/raw
_pm_rmstacklen  dw      40h             ; real mode stack length, in para
_pm_pmstacklen  dw      80h             ; protected mode stack length, in para
_pm_rmstacks    db      4               ; real mode stack nesting
_pm_pmstacks    db      2               ; protected mode stack nesting
_pm_callbacks   db      16              ; number of real mode callbacks
_pm_mode        db      1               ; mode bits

processortype   db      0               ; processor type                     |
pmodetype       db      2               ; protected mode type                |
picslave        db      70h             ; PIC slave base interrupt          |
picmaster       db      8               ; PIC master base interrupt         |

tempd0          EQU $                   ; temporary variables                |
tempw0          EQU $                   ;                                    |
tempb0          db      0               ;                                    |
tempb1          db      0               ;                                    |
tempw1          EQU $                   ;                                    |
tempb2          db      0               ;                                    |
tempb3          db      0               ;                                    |
tempd1          EQU $                   ;                                    |
tempw2          EQU $                   ;                                    |
tempb4          db      0               ;                                    |
tempb5          db      0               ;                                    |
tempw3          EQU $                   ;                                    |
tempb6          db      0               ;                                    |
tempb7          db      0               ;                                    |

xms_callip      dw      0               ; XMS driver                 |
xms_callcs      dw      0               ; XMS driver segment                |

vcpi_cr3        dd      0               ; VCPI CR3 value for protected mode  |
vcpi_gdtaddx    dd      gdtlimit    ; linear addx of GDT limit and base  |
vcpi_idtaddx    dd      idtlimit    ; linear addx of IDT limit and base  |
vcpi_selldt     dw      0               ; LDT selector for protected mode    |
vcpi_seltss     dw      SELVCPITSS      ; TSS selector for protected mode    |
vcpi_eip        dd      v_rmtopmswpm; destination EIP in protected mode  |
vcpi_cs         dw      SELCODE         ; destination CS in protected mode   |

vcpi_calleip    dd      0               ; VCPI protected mode call   |
vcpi_callcs     dw      SELVCPICODE     ; VCPI protected mode call selector |
raw300buf		resw	19h	; buffer for 100,101, and 102 calls

initrouttbl     dw      r_init, x_init, v_init, d_init

int31functbl    dw      0900h, 0901h, 0902h, 0000h, 0001h, 0003h, 0006h, 0007h
                dw      0008h, 0009h, 0200h, 0201h, 0204h, 0205h, 0305h, 0306h
                dw      0400h
                dw      000ah, 000bh, 000ch, 000eh, 000fh
                dw      0300h, 0301h, 0302h
                dw      0303h, 0304h
        dw	0100h, 0101h, 0102h
                dw      0500h, 0501h, 0502h, 0503h, 050ah
INT31FUNCNUM    EQU ($ - int31functbl) / 2

int31routtbl    dw      int310900, int310901, int310902, int310000
                dw      int310001, int310003, int310006, int310007
                dw      int310008, int310009, int310200, int310201
                dw      int310204, int310205, int310305, int310306
                dw      int310400
                dw      int31000a, int31000b, int31000c, int31000e, int31000f
                dw      int310300, int310301, int310302
                dw      int310303, int310304
        dw	int310100, int310101, int310102
int31mrouttbl   dw      int310500v, int310501v, int310502v, int310503v
                dw      int31050av
int31mxrouttbl  dw      int310500x, int310501x, int310502x, int310503x
                dw      int31050ax
int31mrrouttbl  dw      int310500r, int310501r, int310502r, int310503r
                dw      int31050ar
int31mnrouttbl  dw      int310500rnomem, int31fail8013, int31fail8023
                dw      int31fail8023, int31fail8023

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; DETECT/INIT CODE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; Get protected mode info
; Out:
;   AX = return code:
;     0000h = successful
;     0001h = no 80386+ detected
;     0002h = system already in protected mode and no VCPI or DPMI found
;     0003h = DPMI - host is not 32bit
;   CF = set on error, if no error:
;     BX = number of paragraphs needed for protected mode data (may be 0)
;     CL = processor type:
;       02h = 80286
;       03h = 80386
;       04h = 80486
;       05h = 80586
;       06h-FFh = reserved for future use
;     CH = protected mode type:
;       00h = raw
;       01h = XMS
;       02h = VCPI
;       03h = DPMI
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
_pm_info:
        push dx                         ; preserve registers
        push si
        push di
        push bp
        push ds
        push es
        push bx
        push cx

        push cs                         ; DS = CS (PMODE_TEXT)
        pop ds

        test byte [_pm_mode],1          ; check order of DPMI/VCPI detection
        jz short .infof0

        call .detect_VCPI              ; check for VCPI first
        call .detect_DPMI              ; check for DPMI second
        jmp short .infof2              ; neither found, go on to XMS check

.infof0:
        call .detect_DPMI              ; check for DPMI first
        call .detect_VCPI              ; check for VCPI second

;-----------------------------------------------------------------------------
.infof2:
        smsw ax                         ; AX = machine status word
        test al,1                       ; is system in protected mode?
        mov ax,2                        ; error code in case protected mode
        jnz short .infofail            ; if in protected mode, fail

        mov ax,4300h                    ; chek for XMS
        int 2fh
        cmp al,80h                      ; XMS present?
        sete ch                         ; if yes, pmode type is XMS

        mov bx,80h                      ; BX = memory requirement (IDT)

;-----------------------------------------------------------------------------
.infof1:
        movzx ax,byte[_pm_rmstacks]     ; size of real mode stack area
        imul ax,[_pm_rmstacklen]
        add bx,ax

        movzx ax,byte [_pm_pmstacks]    ; size of protected mode stack area
        imul ax,[_pm_pmstacklen]
        add bx,ax

        movzx ax,byte [_pm_callbacks]   ; size of callbacks
        imul ax,25
        add ax,0fh
        shr ax,4
        add bx,ax

        mov ax,[_pm_selectors]          ; size of GDT
        add ax,1+SYSSELECTORS+5
        shr ax,1
        add bx,ax

        jmp short .infook              ; go to done ok

;-----------------------------------------------------------------------------
.infofail:
        pop cx 
        pop bx                       ; restore BX and CX
        stc                             ; carry set, failed
        jmp short .infodone

;-----------------------------------------------------------------------------
.infook:
        mov [pmodetype],ch                ; store pmode type

        add sp,4                        ; skip BX and CX on stack
        xor ax,ax                       ; success code, also clear carry flag

;-----------------------------------------------------------------------------
.infodone:
        pop es 
        pop ds 
        pop bp 
        pop di 
        pop si 
        pop dx           ; restore other registers
        retf                            ; return

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.detect_DPMI:                          ; detect a DPMI host
        pop bp                          ; pop return address from stack

        mov ax,1687h                    ; check for DPMI
        int 2fh

        or ax,ax                        ; DPMI present?
        jnz short .detect_DPMIdone     ; if no, exit routine

        mov ax,3                        ; error code in case DPMI not 32bit
        test bl,1                       ; is DPMI 32bit?
        jz .infofail                   ; if no, fail

        mov ax,1                        ; error code in case no processor 386+
        cmp cl,3                        ; is processor 386+? (redundant)
        jb .infofail                   ; if no, fail

        mov word [dpmiepmode],di   		; store DPMI initial mode switch addx
        mov word [dpmiepmode + 2],es

        mov bx,si                       ; BX = number of paragraphs needed
        mov ch,3                        ; pmode type is 3 (DPMI)

        jmp .infook                    ; go to done ok

.detect_DPMIdone:
        jmp bp                          ; return to calling routine

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.detect_VCPI:                          ; detect a VCPI server
        pop bp                          ; pop return address from stack

        call .detect_processor         ; get processor type

        mov ax,1                        ; error code in case no processor 386+
        cmp cl,3                        ; is processor 386+?
        jb .infofail                   ; if no, no VCPI

        mov [processortype],cl            ; store processor type

        xor ax,ax                       ; get INT 67h vector
        mov es,ax
        mov ax,[es:67h*4]
        or ax,[es:67h*4+2]              ; is vector NULL
        jz short .detect_VCPIdone      ; if yes, no VCPI

        mov ax,0de00h                   ; call VCPI installation check
        int 67h
        or ah,ah                        ; AH returned as 0?
        jnz short .detect_VCPIdone     ; if no, no VCPI

        movzx bx,byte[_pm_pagetables]   ; BX = VCPI page table memory needed
        shl bx,8                        ; 100h paragraphs per page table
        add bx,100h+0ffh+7+80h          ; + page dir + align buf + TSS + IDT

        mov ch,2                        ; pmode type is 2 (VCPI)

        jmp .infof1                    ; go to figure other memory needed

.detect_VCPIdone:
        jmp bp                          ; return to calling routine

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.detect_processor:                     ; get processor: 286, 386, 486, or 586
        xor cl,cl                       ; processor type 0 in case of exit

        pushf                           ; transfer FLAGS to BX
        pop bx

        mov ax,bx                       ; try to clear high 4 bits of FLAGS
        and ah,0fh

        push ax                         ; transfer AX to FLAGS
        popf
        pushf                           ; transfer FLAGS back to AX
        pop ax

        and ah,0f0h                     ; isolate high 4 bits
        cmp ah,0f0h
        je short .detect_processordone ; if bits are set, CPU is 8086/8

        mov cl,2                        ; processor type 2 in case of exit

        or bh,0f0h                      ; try to set high 4 bits of FLAGS

        push bx                         ; transfer BX to FLAGS
        popf
        pushf                           ; transfer FLAGS to AX
        pop ax

        and ah,0f0h                     ; isolate high 4 bits
        jz short .detect_processordone ; if bits are not set, CPU is 80286

        inc cx                          ; processor type 3 in case of exit

        push eax                        ; preserve 32bit registers
        push ebx

        pushfd                          ; transfer EFLAGS to EBX
        pop ebx

        mov eax,ebx                     ; try to flip AC bit in EFLAGS
        xor eax,40000h

        push eax                        ; transfer EAX to EFLAGS
        popfd
        pushfd                          ; transfer EFLAGS back to EAX
        pop eax

        xor eax,ebx                     ; AC bit fliped?
        jz short .detect_processordone2; if no, CPU is 386

        inc cx                          ; processor type 4 in case of exit

        mov eax,ebx                     ; try to flip ID bit in EFLAGS
        xor eax,200000h

        push eax                        ; transfer EAX to EFLAGS
        popfd
        pushfd                          ; transfer EFLAGS back to EAX
        pop eax

        xor eax,ebx                     ; ID bit fliped?
        jz short .detect_processordone2; if no, CPU is 486

        inc cx                          ; processor type 5, CPU is 586

.detect_processordone2:
        pop ebx 
        pop eax                     ; restore 32bit registers

.detect_processordone:
        ret                             ; return

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
; Initialize protected mode
; In:
;   ES = real mode segment for protected mode data (ignored if not needed)
; Out:
;   AX = return code:
;     0000h = successful
;     0001h = no 80386+ detected
;     0002h = system already in protected mode and no VCPI or DPMI found
;     0003h = DPMI - host is not 32bit
;     0004h = could not enable A20 gate
;     0005h = DPMI - could not enter 32bit protected mode
;     0006h = DPMI - could not allocate needed selectors
;   CF = set on error, if no error:
;     ESP = high word clear
;     CS = 16bit selector for real mode CS with limit of 64k
;     SS = selector for real mode SS with limit of 64k
;     DS = selector for real mode DS with limit of 64k
;     ES = selector for PSP with limit of 100h
;     FS = 0 (NULL selector)
;     GS = 0 (NULL selector)
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
_pm_init:
        push bx 
        push cx                      ; get initial info on protected mode
        call far _pm_info
        pop cx 
        pop bx
        jnc short .initf0              ; error?

        retf                            ; yup, abort

.initf0:                               ; no error, init protected mode
        pushad
        push ds
        mov bp,sp
        push cs                         ; DS = PMODE_TEXT
        pop ds
        cld

        mov eax,PMODE_TEXT              ; set base addx of PMODE_TEXT
        shl eax,4
        mov [codebase],eax

        movzx bx,byte[pmodetype]              ; jump to appropriate init code
        shl bx,1
        jmp [bx + initrouttbl]

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
d_init:                                 ; DPMI protected mode init
        pop ds                          ; get original caller DS from stack
        mov ax,1                        ; enter DPMI protected mode
        call far [cs:dpmiepmode]
        push ds                         ; put DS back onto stack
        jnc short dvxr_init             ; error? if not, go on with init

        mov bx,6                        ; error entering protected mode, set
        cmp ax,8011h                    ;  error code and abort
        stc
        je short init_done
        mov bl,5                        ; error code 5, not 6

;-----------------------------------------------------------------------------
init_done:                              ; return with return code
        mov [bp+30],bx
        pop ds
        popad
        retf

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
dvxr_init:                              ; DPMI/VCPI/XMS/raw common init tail
        xor ax,ax                       ; allocate selector for return code
        mov cx,1
        int 31h
        jnc short .dvxr_initf0

        mov ah,4ch                      ; could not allocate selector
        int 21h                         ; terminate immediately

.dvxr_initf0:
        mov bx,ax                       ; new code descriptor for return

        mov ax,0007h                    ; set base address of calling segment
        xor cx,cx
        mov dx,[bp+36]
        shld cx,dx,4
        shl dx,4
        int 31h

        mov ax,0008h                    ; set selector limit of 64k
        xor cx,cx
        mov dx,0ffffh
        int 31h

        mov ax,0009h                    ; set selector type and access rights
        mov dx,cs                       ; get DPL from current CPL, and access
        lar cx,dx                       ;  rights and type from current CS
        shr cx,8                        ; type is already 16bit code segment
        int 31h

.dvxr_initdone:
        mov [bp+36],bx                  ; store selector in return address
        xor bx,bx                       ; init successful, carry clear
        jmp init_done

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
v_init:                                 ; VCPI protected mode init
        mov ax,0de0ah                   ; get PIC mappings
        int 67h
        mov [picmaster],bl
        mov [picslave],cl

        mov eax,[codebase]              ; adjust addresses for VCPI structure
        add [vcpi_gdtaddx],eax
        add [vcpi_idtaddx],eax
        add [vcpistrucaddx],eax

        mov dx,es                       ; align data area on page
        add dx,0ffh
        xor dl,dl
        mov es,dx

        movzx eax,dx                    ; set base and top of page table area
        shl eax,4
        add eax,1000h
        mov [pagetablebase],eax
        movzx ecx,byte[_pm_pagetables]
        shl ecx,12
        add eax,ecx
        mov [pagetabletop],eax

        xor di,di                       ; clear page dir and first page table
        mov cx,1000h
        xor ax,ax
        rep stosw

        mov gs,dx                       ; GS = segment of page directory
        lea eax,[edx+100h]
        mov es,ax                       ; ES = segment of first page table
        mov fs,ax                       ; FS = segment of first page table

        push ss                         ; stack space for VCPI descriptors
        pop ds
        sub sp,8*3
        mov si,sp

        xor di,di                       ; get VCPI protected mode interface
        mov ax,0de01h
        int 67h

        push cs                         ; DS = PMODE_TEXT
        pop ds

        movzx eax,di                    ; set base of usable page table area
        add eax,[pagetablebase]
        add [pagetablefree],eax

        mov [vcpi_calleip],ebx            ; store protected mode VCPI call EIP
        movzx si,dh                     ; get physical address of page dir
        shl si,2                        ;  from first page table for CR3
        db 64h ; FS:
           lodsd
        mov [vcpi_cr3],eax

.v_initl0:
        and byte [es:di+1],0f1h     ; clear bits 9-11 in copied page table
        sub di,4
        jnc .v_initl0

        mov dx,es                       ; DX = current page table segment
        xor ebx,ebx                     ; index in page dir, also loop counter
        jmp short .v_initl1f0

.v_initl1:
        xor di,di                       ; clear page table
        mov cx,800h
        xor ax,ax
        rep stosw

.v_initl1f0:
        add dx,100h                     ; increment page table segment
        mov es,dx

        db 64h ; FS:
        lodsd                        ; set physical address of page table
        mov [gs:ebx*4],eax              ;  in page directory

        inc bx                          ; increment index in page directory
        cmp bl,[_pm_pagetables]           ; at end of page tables?
        jb .v_initl1                   ; if no, loop

;-----------------------------------------------------------------------------
        push dx                         ; preserve seg of TSS for later use

        xor di,di                       ; clear TSS with all 0, not really
        mov cx,34h                      ;  needed, but just to be safe
        xor ax,ax
        rep stosw

        mov eax,[vcpi_cr3]                ; set CR3 in TSS
        mov [es:1ch],eax
        mov dword [es:64h],680000h  ; set of I/O permission bitmap
                                        ;  and clear T bit
        add dx,7                        ; increment next data area ptr
        mov es,dx

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
vxr_init:                               ; VCPI/XMS/raw common init tail
        mov ax,es                       ; set IDT base address
        movzx ebx,ax
        shl ebx,4
        mov [idtbase],ebx

        movzx bx,byte [_pm_rmstacks]    ; set top and base of real mode stack
        imul bx,[_pm_rmstacklen]        ;  area for interrupt redirection
        add ax,80h                      ;  from protected mode
        mov [rmstackbase],ax
        add ax,bx
        mov [rmstacktop],ax

        xor di,di                       ; set up IDT
        mov dx,word [picslave]
        xor ecx,ecx

.vxr_initl0:
        lea eax,[ecx + SELCODE*10000h+intrmatrix]
        stosd

        mov eax,8e00h                   ; interrupt gate type
        mov bl,cl                       ; isolate high 5 bits of int num
        and bl,0f8h

        test cl,0f8h                    ; one of the low 8 interrupts?
        jz short .vxr_initl0f0         ; if yes, store as interrupt gate

        cmp bl,dl                       ; one of the high IRQs?
        je short .vxr_initl0f0         ; if yes, store as interrupt gate
        cmp bl,dh                       ; one of the low IRQs?
        je short .vxr_initl0f0         ; if yes, store as interrupt gate

        or ah,1                         ; set to trap gate type

.vxr_initl0f0:
        stosd

        inc cl                          ; increment interrupt number
        jnz .vxr_initl0                ; loop if more interrupts to go

        mov word [es:8*31h],int31    ; protected mode INT 31h
        mov word [es:8*21h],int21    ; protected mode INT 21h
        mov word [es:8*3],intr  ; protected mode INT 3

        mov es,[rmstacktop]               ; set next data area ptr to end of
                                        ;  real mode stack area
;-----------------------------------------------------------------------------
    mov ax,es			; set protected mode stack area base
        movzx eax,ax                    ;  for callbacks
        shl eax,4
    mov [pmstackbase],eax

        movzx ecx,word [_pm_pmstacklen] ; set protected mode stack area top
        movzx ebx,byte [_pm_pmstacks]   ;  for callbacks
        shl ecx,4
        mov [pmstacklen],ecx            ; protected mode stack size in bytes
        imul ebx,ecx
        add ebx,eax
        mov [pmstacktop],ebx            ; protected mode stack area top

        mov cl,[_pm_callbacks]          ; CL = number of callbacks
        or cl,cl                        ; any callbacks?
        jz short .vxr_initf3           ; if no, done with this part

        mov [callbackbase],ebx          ; top of stacks is base of callbacks
        shr ebx,4                       ; BX = seg of callback area
        mov [callbackseg],bx

        mov es,bx                       ; ES = seg of callback area
        xor di,di                       ; location within callback seg

.vxr_initl1:
        mov word [es:di],6066h      ; PUSHAD instruction
        mov byte [es:di+2],068h     ; PUSH WORD instruction
        mov word [es:di+3],0        ; immediate 0 used as free flag
        mov word [es:di+5],06866h   ; PUSH DWORD instruction
        mov byte [es:di+11],0b9h    ; MOV CX,? instruction
        mov word [es:di+14],06866h  ; PUSH DWORD instruction
        mov byte [es:di+20],0eah    ; JMP FAR PTR ?:? intruction
        mov word [es:di+21],callback
        mov word [es:di+23],PMODE_TEXT

        add di,25                       ; increment ptr to callback
        dec cl                          ; decrement loop counter
        jnz .vxr_initl1                ; if more callbacks to do, loop

    add di,0fh			; align next data area on paragraph
    shr di,4
    add bx,di
        mov es,bx                       ; set ES to base of next data area

.vxr_initf3:

;-----------------------------------------------------------------------------
        mov [gdtseg],es			; store segment of GDT

        mov ax,es                       ; set GDT base address
        movzx eax,ax
        shl eax,4
        mov [gdtbase],eax

        mov cx,[_pm_selectors]          ; set GDT limit
        lea ecx,[ecx*8+8*5+8*SYSSELECTORS-1]
        mov [gdtlimit],cx

        xor di,di                       ; clear GDT with all 0
        inc cx
        shr cx,1
        xor ax,ax
        rep stosw

        cmp byte [pmodetype],2               ; if under VCPI, do VCPI GDT set up
        jne short .vxr_initf1

        pop ax                          ; restore TSS seg from stack
        movzx eax,ax                    ; set up TSS selector in GDT
        shl eax,4
        mov dword [es:SELVCPITSS+2],eax
        mov byte [es:SELVCPITSS],67h
        mov byte [es:SELVCPITSS+5],89h

        add eax,64h-4*9                 ; unused part of TSS is also
        mov [vcpiswitchstack],eax       ;  temporary switch stack

        mov di,SELVCPICODE              ; copy 3 VCPI descriptors from stack
        mov si,sp                       ;  to GDT
        mov cx,4*3
        db 36h; ss:
        rep movsw

        add sp,8*3                      ; adjust stack

.vxr_initf1:
        mov word [es:SELZERO],0ffffh; set SELZERO descriptor
        mov word [es:SELZERO+5],0df92h

        mov word [es:SELCALLBACKDS],0ffffh  ; set callback DS descriptor
    mov word [es:SELCALLBACKDS+5],0df92h

        mov word [es:SELREAL],0ffffh; set real mode attributes descriptor
        mov word [es:SELREAL+5],01092h

        mov ax,cs                       ; set SELCODE descriptor (PMODE_TEXT)
        mov bx,SELCODE                  ; BX = index to SELCODE descriptor
        mov cx,0ffffh                   ; CX = limit (64k)
        mov dx,109ah                    ; DX = access rights
        call vxr_initsetdsc

        mov bx,8*SYSSELECTORS           ; BX = base of free descriptors
        push bx                         ; store selector

        mov ax,ss                       ; set caller SS descriptor
        mov dx,5092h
        call vxr_initsetdsc

        mov ax,[bp]                     ; set caller DS descriptor
        mov [bp],bx                     ; put DS selector on stack for exit
        call vxr_initsetdsc

        push bx                         ; get PSP segment
        mov ah,51h
        int 21h
        mov si,bx
        pop bx

        mov fs,si                       ; set caller environment descriptor
        mov ax,[fs:2ch]
        or ax,ax                        ; is environment seg 0?
        jz short .vxr_initf0           ; if yes, dont convert to descriptor
        mov [fs:2ch],bx                 ; store selector value in PSP
        call vxr_initsetdsc

.vxr_initf0:
        mov ax,si                       ; set caller PSP descriptor
        mov cx,0ffh                     ; limit is 100h bytes
        call vxr_initsetdsc

;-----------------------------------------------------------------------------
        lea ecx,[ebx-8]                 ; CX = ES descriptor, just set
        pop dx                          ; DX = SS descriptor, from stack
        mov ax,SELZERO                  ; AX = DS descriptor, SELZERO
        movzx ebx,sp                    ; EBX = SP, current SP - same stack
        mov si,SELCODE                  ; target CS is SELCODE, same segment
        mov edi,.vxr_initf2        ; target EIP

        jmp word [rmtopmswrout]              ; jump to mode switch routine

.vxr_initf2:   
        mov edi,[cs:codebase]           ; EDI = of PMODE_TEXT from 0
        mov eax,[ds:4*15h]              ; get INT 15h vector
        mov [edi + oldint15vector],eax  ; store INT 15h vector

        mov esi,[cs:rawextmembase]      ; ESI = raw base of extended memory
        cmp esi,[cs:rawextmemtop]       ; is there any raw extended memory?
        jae dvxr_init                   ; if no, go DPMI/VCPI/XMS/raw init

        dw 0b866h,int15,PMODE_TEXT  ; MOV EAX,PMODE_TEXT:int15
        mov [ds:4*15h],eax              ; set new INT 15h handler

        mov edi,[cs:rawextmemtop]       ; EDI = raw top of extended memory
        mov eax,edi                     ; EAX = size of extended memory
        sub eax,esi
        sub eax,10h                     ; subtract memory control block size
        mov [edi-16],eax                ; store size in memory control block
        xor eax,eax
        mov [edi-12],eax                ; no next memory control block
        mov [edi-8],eax                 ; no previous memory control block
        mov [edi-4],al                  ; memory block is free

        jmp dvxr_init                   ; go to DPMI/VCPI/XMS/raw init tail

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
vxr_initsetdsc:                         ; set descriptor for VCPI/XMS/raw init
        movzx eax,ax                    ; EAX = base of segment
        shl eax,4
        mov word [es:bx],cx         ; limit = CX
        mov dword [es:bx+2],eax     ; base address = EAX
        mov word [es:bx+5],dx       ; access rights = DX
        add bx,8                        ; increment descriptor index
        ret

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
x_init:                                 ; XMS protected mode init
        push es                         ; preserve ES, INT 2Fh destroys it

        mov ax,4310h                    ; get XMS driver address
        int 2fh

        mov [xms_callip],bx             ; store XMS driver address
        mov [xms_callcs],es

        pop es                          ; restore ES (buffer segment)

        mov ah,3                        ; enable A20
        call far [xms_callip]

        mov bx,4                        ; error code 0004h in case of error
        cmp ax,1                        ; error enabling A20?
        jc init_done                    ; if yes, exit with error 0004h

        mov si,int31mxrouttbl       ; set XMS memory allocation functions

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
xr_init:                                ; XMS/raw common init tail
        push es                         ; preserve ES

        push ds                         ; ES = DS for table copy
        pop es

        mov di,int31mrouttbl        ; copy memory allocation function
        mov cx,5                        ;  addresses to table
        rep movsw

        pop es                          ; restore ES (buffer segment)

        mov word [rmtopmswrout],xr_rmtopmsw; set XMS/raw mode switch addresses
        mov dword [pmtormswrout],xr_pmtormsw

        jmp vxr_init                    ; go to VCPI/XMS/raw continue init

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
r_init:                                 ; raw protected mode init
        mov ah,88h                      ; how much extended memory free
        int 15h

        mov si,int31mnrouttbl       ; SI -> no memory allocation functions
        or ax,ax                        ; if none, done with raw init
        jz xr_init

        movzx eax,ax                    ; convert AX K to ptr to top of mem
        shl eax,10
        add eax,100000h
        mov [cs:rawextmemtop],eax

        call enablea20                  ; enable A20

        push es                         ; preserve ES (buffer segment)

        xor cx,cx                       ; ES -> 0 (interrupt vector table)
        mov es,cx
        les bx,[es:4*19h]     ; ES:BX -> int vector table

        mov eax,100000h                 ; initial free extended memory base
        cmp dword [es:bx+12h],'SIDV'; VDISK memory allocation?
        jne short .r_initf0            ; if present, get base of free mem

        mov eax,dword [es:bx+2ch]   ; get first free byte of extended mem
        add eax,0fh                     ; align on paragraph
        and eax,0fffff0h                ; address is only 24bit

.r_initf0:
        dec cx                          ; ES -> 0ffffh for ext mem addressing
        mov es,cx

        cmp dword [es:13h],'SIDV'   ; VDISK memory allocation?
        jne short .r_initf1            ; if present, get base of free mem

        movzx ebx,word [es:2eh]     ; get first free K of extended memory
        shl ebx,10                      ; adjust K to bytes

        cmp eax,ebx                     ; pick larger of 2 addresses
        ja short .r_initf1

        mov eax,ebx

.r_initf1:
        pop es                          ; restore ES (buffer segment)

        mov si,int31mnrouttbl       ; SI -> no memory allocation functions
        cmp eax,[cs:rawextmemtop]         ; any valid free extended memory
        jae xr_init                     ; if none, done with raw init

        mov [cs:rawextmembase],eax
        mov si,int31mrrouttbl       ; set raw memory allocation functions

        jmp xr_init                     ; go to XMS/raw continue init

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
enablea20:                              ; hardware enable gate A20
        pushf
        push fs
        push gs
        cli

        xor ax,ax                       ; set A20 test segments 0 and 0ffffh
        mov fs,ax
        dec ax
        mov gs,ax

        call enablea20test              ; is A20 already enabled?
        jz short .enablea20done        ; if yes, done

        in al,92h                       ; PS/2 A20 enable
        or al,2
        jmp short $+2
        jmp short $+2
        jmp short $+2
        out 92h,al

        call enablea20test              ; is A20 enabled?
        jz short .enablea20done        ; if yes, done

        call enablea20kbwait            ; AT A20 enable
        jnz short .enablea20f0

        mov al,0d1h
        out 64h,al

        call enablea20kbwait
        jnz short .enablea20f0

        mov al,0dfh
        out 60h,al

        call enablea20kbwait

.enablea20f0:                          ; wait for A20 to enable
        mov cx,800h                     ; do 800h tries

.enablea20l0:
        call enablea20test              ; is A20 enabled?
        jz .enablea20done              ; if yes, done

        in al,40h                       ; get current tick counter
        jmp short $+2
        jmp short $+2
        jmp short $+2
        in al,40h
        mov ah,al

.enablea20l1:                          ; wait a single tick
        in al,40h
        jmp short $+2
        jmp short $+2
        jmp short $+2
        in al,40h
        cmp al,ah
        je .enablea20l1

        loop .enablea20l0              ; loop for another try

        mov bp,sp                       ; error, A20 did not enable
        mov ax,4                        ; error code 4
        mov word [bp+6],init_done       ; set init_done return address

.enablea20done:
        pop gs 
        pop fs
        popf
        ret

;-----------------------------------------------------------------------------
enablea20kbwait:                        ; wait for safe to write to 8042
        xor cx,cx
.enablea20kbwaitl0:
        jmp short $+2
        jmp short $+2
        jmp short $+2
        in al,64h                       ; read 8042 status
        test al,2                       ; buffer full?
        loopnz .enablea20kbwaitl0      ; if yes, loop
        ret

;-----------------------------------------------------------------------------
enablea20test:                          ; test for enabled A20
        mov al,[fs:0]                   ; get byte from 0:0
        mov ah,al                       ; preserve old byte
        not al                          ; modify byte
        xchg al,[gs:10h]                ; put modified byte to 0ffffh:10h
        cmp ah,[fs:0]                   ; set zero if byte at 0:0 not modified
        mov [gs:10h],al                 ; put back old byte at 0ffffh:10h
        ret                             ; return, zero if A20 enabled

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
int15:                                  ; real mode INT 15h handler
        cmp ah,88h                      ; function 88h?
        je short .int15f0              ; if yes, need to process

        jmp far [cs:oldint15vector]   ; no, go on to old INT 15h handler

.int15f0:
        pushf                           ; call old int 15h handler
        call far [cs:oldint15vector]

        sub ax,[cs:rawextmemused]       ; adjust AX by extended memory used

        push bp                         ; clear carry flag on stack for IRET
        mov bp,sp
        and byte [bp+6],0feh
        pop bp

        iret                            ; return with new AX extended memory

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; PROTECTED MODE KERNEL CODE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
v_rmtopmsw:                             ; VCPI real to protected switch
        pushf                           ; store FLAGS
        cli
        push cs                         ; DS = PMODE_TEXT
        pop ds
        pop word [tempw0]               ; move FLAGS from stack to temp
        mov [tempw1],ax                 ; store AX (protected mode DS)
        mov [tempw2],si                 ; store SI (protected mode CS)
        mov esi,[vcpistrucaddx]         ; ESI = linear addx of VCPI structure
        mov ax,0de0ch                   ; VCPI switch to protected mode
        int 67h
v_rmtopmswpm:
        mov ss,dx                       ; load protected mode SS:ESP
        mov esp,ebx
        mov ds,[cs:tempw1]              ; load protected mode DS
        mov es,cx                       ; load protected mode ES
        xor ax,ax
        mov fs,ax                       ; load protected mode FS with NULL
        mov gs,ax                       ; load protected mode GS with NULL
        pushfd                          ; store EFLAGS
        mov ax,[cs:tempw0]              ; move bits 0-11 of old FLAGS onto
        and ah,0fh                      ;  stack for IRETD
        mov [esp],ax
        push dword [cs:tempd1]          ; store protected mode target CS
        push edi                        ; store protected mode target EIP
        iretd                           ; go to targed addx in protected mode

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
v_pmtormsw:                             ; VCPI protected to real switch
        pushf                           ; store FLAGS
        cli
        push ax                         ; store AX (real mode DS)
        mov ds,[cs:selzero]             ; DS -> 0 (beginning of memory)
        movzx ebx,bx                    ; clear high word of EBX, real mode SP
        movzx edx,dx                    ; clear high word of EDX, real mode SS
        mov eax,[cs:vcpiswitchstack]    ; EAX -> top of temporary switch stack
        mov dword [ds:eax+32],0     ; store real mode GS
        mov dword [ds:eax+28],0     ; store real mode FS
        movzx ecx,cx                    ; clear high word of ECX, real mode ES
        mov [ds:eax+20],ecx             ; store real mode ES
        pop cx                          ; move real mode DS from protected
        mov [ds:eax+24],ecx             ;  mode stack to VCPI call stack
        mov [ds:eax+16],edx             ; store real mode SS
        mov [ds:eax+12],ebx             ; store real mode SP
        mov dword [ds:eax+4],PMODE_TEXT             ; store real mode CS
        mov dword [ds:eax+0],.v_pmtormswf0     ; store real mode IP
        pop bx                          ; restore FLAGS from stack
        mov ss,[cs:selzero]             ; SS -> 0 (beginning of memory)
        mov esp,eax                     ; ESP = stack ptr for VCPI call
        mov ax,0de0ch                   ; VCPI switch to real mode (V86)
        db 066h
        call far [cs:vcpi_calleip]
.v_pmtormswf0:
        push bx                         ; store old FLAGS
    push si 			; store target CS in real mode
    push di 			; store target IP in real mode
    iret				; go to target addx in real mode

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
xr_rmtopmsw:                            ; XMS/raw real to protected switch
        pushfd                          ; store EFLAGS
        cli
        push ax                         ; store AX (protected mode DS)
        lidt [cs:idtlimit]      ; load protected mode IDT
        lgdt [cs:gdtlimit]      ; load protected mode GDT
        mov eax,cr0                     ; switch to protected mode
        or al,1
        mov cr0,eax
        db 0eah                         ; JMP FAR PTR SELCODE:$+4
        dw $+4,SELCODE                  ;  (clear prefetch que)
        pop ds                          ; load protected mode DS
        mov es,cx                       ; load protected mode ES
        xor ax,ax
        mov fs,ax                       ; load protected mode FS with NULL
        mov gs,ax                       ; load protected mode GS with NULL
        pop eax
        mov ss,dx                       ; load protected mode SS:ESP
        mov esp,ebx
        and ah,0bfh                     ; set NT=0 in old EFLAGS
        push ax                         ; set current FLAGS
        popf
        push eax                        ; store old EFLAGS
        push esi                        ; store protected mode target CS
        push edi                        ; store protected mode target EIP
        iretd                           ; go to targed addx in protected mode

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
xr_pmtormsw:                            ; XMS/raw protected to real switch
        push cx                         ; store CX (real mode ES)
        pushf                           ; store FLAGS
        cli
        push ax                         ; store AX (real mode DS)
        mov ds,[cs:selzero]             ; DS -> 0 (beginning of memory)
        mov ecx,[cs:codebase]           ; get of PMODE_TEXT from 0
        pop word [ecx + tempw0]              ; move real mode DS from stack to temp
        pop word [ecx + tempw1]              ; move FLAGS from stack to temp
        pop cx                          ; restore CX (real mode ES)
        mov ax,SELREAL                  ; load descriptors with real mode seg
        mov ds,ax                       ;  attributes
        mov es,ax
        mov fs,ax
        mov gs,ax
        mov ss,ax                       ; load descriptor with real mode attr
        movzx esp,bx                    ; load real mode SP, high word 0
        lidt [cs:rmidtlimit]    ; load real mode IDT
        mov eax,cr0                     ; switch to real mode
        and al,0feh
        mov cr0,eax
        db 0eah                         ; JMP FAR PTR PMODE_TEXT:$+4
        dw $+4,PMODE_TEXT               ;  (clear prefetch que)
        mov ss,dx                       ; load real mode SS
        mov ds,[cs:tempw0]              ; load real mode DS
        mov es,cx                       ; load real mode ES
        xor ax,ax
        mov fs,ax                       ; load real mode FS with NULL
        mov gs,ax                       ; load real mode GS with NULL
        push word [cs:tempw1]                ; store old FLAGS
        push si                         ; store real mode target CS
        push di                         ; store real mode target IP
        iret                            ; go to target addx in real mode

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
vxr_saverestorerm:                      ; VCPI/XMS/raw save/restore status
        retf                            ; no save/restore needed, 16bit RETF

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
vxr_saverestorepm:                      ; VCPI/XMS/raw save/restore status
        db 66h,0cbh                     ; no save/restore needed, 32bit RETF

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
critical_error:                         ; some unrecoverable error
        cli                             ; make sure we are not interrupted
        in al,61h                       ; beep
        or al,3
        out 61h,al
        jmp $                           ; now hang

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
callback:                               ; real mode callback handler
        mov ax,sp                       ; preserve SS:SP for callback
        push ss
        push ax

        push gs 
        push fs 
        push ds 
        push es                         ; preserve real mode regs for callback
        pushf                           ; preserve FLAGS for callback

        cli
        cld

        mov ebp,[cs:pmstacktop]         ; EBP = ESP for protected mode

        mov ebx,ebp                     ; set EBX to next stack location
        sub ebx,[cs:pmstacklen]
        mov [cs:pmstacktop],ebx         ; update ptr for possible reenterancy

        cmp ebx,[cs:pmstackbase]        ; exceeded protected mode stack space?
        jb critical_error               ; if yes, critical error (hang)

        xor eax,eax                     ; EAX = base address of SS
        mov ax,ss
        shl eax,4

    movzx ebx,sp			; EBX = current linear SS:SP
        add ebx,eax

    mov es,[cs:gdtseg]		; set for protected mode callback DS
    or eax,92000000h		;  base address in GDT
        mov [es:SELCALLBACKDS+2],eax

        mov ax,SELZERO                  ; DS selector for protected mode
        mov dx,ax                       ; SS selector = DS selector
        mov si,SELCODE                  ; target protected mode CS:EIP
        mov edi,.callbackf0

        jmp word [cs:rmtopmswrout]             ; go to protected mode

.callbackf0:
        mov edi,[esp+14]                ; EDI -> register structure from stack

        lea esi,[esp+24]                ; copy general registers from stack
        mov ecx,8                       ;  to register structure
        db 67h
        rep movsd

        mov esi,esp                     ; copy FLAGS, ES, DS, FG, and GS
        db	67h
        movsw
        db	67h
        movsd
        db	67h
        movsd

        db	67h
        lodsd                           ; EAX = real mode SS:SP from stack
        add ax,42                       ; adjust SP for stuff on stack
        mov [es:edi+4],eax              ; put in register structure

        mov ds,[cs:selcallbackds]       ; DS = callback DS selector
        sub edi,42                      ; EDI -> register structure
        movzx esi,ax                    ; ESI = old real mode SP
        xchg esp,ebp                    ; ESP = protected mode stack

        pushfd                          ; push flags for IRETD from callback
        db 66h                          ; push 32bit CS for IRETD
        push cs
        dw 6866h,.callbackf1,0         ; push 32bit EIP for IRETD

        movzx eax,word [ebp+22]     ; EAX = target CS of callback
        push eax                        ; push 32bit CS for RETF to callback
        push dword [ebp+18]         ; push 32bit EIP for retf

        db 66h                          ; 32bit RETF to callback
        retf

.callbackf1:
        cli
        cld

        push es                         ; DS:ESI = register structure
        pop ds
        mov esi,edi

        mov es,[cs:selzero]             ; ES -> 0 (beginning of memory)

        movzx ebx,word [esi+2eh]    ; EBX = real mode SP from structure
        movzx edx,word [esi+30h]    ; EDX = real mode SS from structure
        sub bx,42                       ; subtract size of vars to be put

        mov ebp,[esi+0ch]               ; EBP = pushed ESP from real mode
        mov bp,bx                       ; EBP = old high & new low word of ESP

        lea edi,[edx*4]                 ; EDI -> real mode base of stack
        lea edi,[ebx+edi*4]             ;  of vars to be stored

        mov ecx,8                       ; copy general registers to stack
        db 67h
        rep movsd

        mov eax,[esi+6]                 ; EAX = return FS and GS for real mode
        mov [es:edi],eax                ; store on real mode stack for return

        mov ax,[esi]                    ; AX = return FLAGS for real mode
        mov [es:edi+8],ax               ; store on real mode stack for return
        mov eax,[esi+10]                ; EAX = return CS:IP for real mode
        mov [es:edi+4],eax              ; store on real mode stack for return

        mov ax,[esi+4]                  ; AX = return DS for real mode
        mov cx,[esi+2]                  ; CX = return ES for real mode

        mov si,PMODE_TEXT               ; real mode target CS:IP
        mov di,.callbackf2

        db 66h                          ; jmp far, as in 32bit offset,
        jmp word [cs:pmtormswrout]      ;  not seg:16bit offset

.callbackf2:
        mov esp,ebp                     ; restore total ESP, old high word

        mov eax,[cs:pmstacklen]         ; restore top of protected mode stack
        add [cs:pmstacktop],eax

        popad                           ; get callback return general regs
        pop fs 
        pop gs                       ; get callback return FS and GS values
        iret                            ; go to callback return CS:IP

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
intrmatrix:				; INT redirectors for all INTs
;		times	100h db 0cch
        resb	100h,0cch

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
intr:                                   ; general interrupt redirector
        cmp word [esp+4],SELCODE    ; INT redirection or internal INT 3?
        je short .intrf0               ; if not INT 3, jump to redirection

        db 066h
        jmp far [cs:int3vector]       ; INT 3, jump to INT 3 vector

.intrf0:
        mov [esp+8],eax                 ; store EAX for later POPAD
        mov eax,[esp]                   ; get address in redirection matrix
        add esp,8                       ; discard EIP and CS from INT 3
        push ecx 
        push edx 
        push ebx 
        push esp 
        push ebp 
        push esi 
        push edi; store rest of registers for POPAD
        push ds 
        push es 
        push fs 
        push gs

        mov ds,[cs:selzero]             ; DS -> 0 (beginning of memory)
        mov edi,[cs:codebase]           ; EDI = of PMODE_TEXT from 0

        mov dx,[cs:rmstacktop]          ; DX = SS for real mode redirection
        movzx ebp,dx                    ; EBP -> top of real mode stack
        shl ebp,4

        mov bx,[cs:_pm_rmstacklen]      ; get size of real mode stack
        sub dx,bx                       ; adjust DX to next stack location
        mov [edi + rmstacktop],dx       ; update ptr for possible reenterancy
        shl bx,4                        ; set real mode SP to top of stack

        cmp dx,[cs:rmstackbase]         ; exceeded real mode stack space?
        jb critical_error               ; if yes, critical error (hang)

        mov word [ds:ebp-2],ss      ; store SS:ESP on real mode stack
        mov [ds:ebp-6],esp

        sub ax,intrmatrix+1         ; AX = int number
        mov ah,al                       ; AH = high 5 bits of int number
        and ah,0f8h

        cmp ah,[cs:picslave]            ; high IRQ?
        je short intrirq                ; if yes, do IRQ
        cmp ah,[cs:picmaster]           ; low IRQ?
        jne short intrint               ; if no, do INT (with general regs)

;-----------------------------------------------------------------------------
intrirq:                                ; an IRQ redirection
        mov [ds:edi + .intrirqintnum],al  ; modify code with interrupt number

        mov si,PMODE_TEXT               ; real mode target CS:IP
        mov di,.intrirqf0
        sub bx,6                        ; adjust real mode SP for stored vars

        db 66h                          ; jmp far, as in 32bit offset,
        jmp word [cs:pmtormswrout]      ;  not seg:16bit offset

.intrirqf0:
        db 0cdh                         ; INT .intrirqintnum
.intrirqintnum db      0

        mov ax,SELZERO                  ; DS selector value for protected mode
        mov cx,ax                       ; ES selector value for protected mode
        pop ebx                         ; get protected mode SS:ESP from stack
        pop dx
        mov si,SELCODE                  ; target CS:EIP in protected mode
        mov edi,.intrirqf1

        jmp word [cs:rmtopmswrout]           ; go back to protected mode

.intrirqf1:
        mov edi,[cs:codebase]           ; restore top of real mode stack
        mov ax,[cs:_pm_rmstacklen]
        add [ds:edi + rmstacktop],ax

        pop gs 
        pop fs 
        pop es 
        pop ds                              ; restore all registers
        popad
        iretd

;-----------------------------------------------------------------------------
intrint:                                ; an INT redirection
        mov [ds:edi + .intrintintnum],al  ; modify code with interrupt number

        mov es,[cs:selzero]             ; copy registers from protected mode
        lea edi,[ebp-26h]               ;  stack to real mode stack
        lea esi,[esp+8]
        mov ecx,8
        cld
        db 67h
        db 36h; ss:
        rep movsd

        mov si,PMODE_TEXT               ; real mode target CS:IP
        mov di,.intrintf0
        sub bx,26h                      ; adjust real mode SP for stored vars

        db 66h                          ; jmp far, as in 32bit offset,
        jmp word [cs:pmtormswrout]      ;  not seg:16bit offset

.intrintf0:
        popad                           ; load regs with int call values

        db 0cdh                         ; INT .intrirqintnum
.intrintintnum db      0

        pushad                          ; store registers on stack
        pushf                           ; store flags on stack
        cli

        xor eax,eax                     ; EAX = linear ptr to SS
        mov ax,ss
        shl eax,4
        movzx ebp,sp                    ; EBP = SP

        mov ebx,[bp+22h]                ; get protected mode SS:ESP from stack
        mov dx,[bp+26h]

        add ebp,eax                     ; EBP -> stored regs on stack

        mov ax,SELZERO                  ; DS selector value for protected mode
        mov cx,ax                       ; ES selector value for protected mode
        mov si,SELCODE                  ; target CS:EIP in protected mode
        mov edi,.intrintf1

        jmp word [cs:rmtopmswrout]      ; go back to protected mode

.intrintf1:
        mov edi,[cs:codebase]             ; restore top of real mode stack
        mov ax,[cs:_pm_rmstacklen]
        add [edi + rmstacktop],ax

        mov ax,[ds:ebp]                 ; move return FLAGS from real mode
        and ax,8d5h                     ;  stack to protected mode stack
        mov bx,[esp+30h]
        and bx,~ 8d5h
        or ax,bx
        mov [esp+30h],ax

        mov edi,[ds:ebp+2]              ; restore return registers from real
        mov esi,[ds:ebp+6]              ;  mode stack
        mov ebx,[ds:ebp+18]
        mov edx,[ds:ebp+22]
        mov ecx,[ds:ebp+26]
        mov eax,[ds:ebp+30]
        mov ebp,[ds:ebp+10]

        pop gs 
        pop fs 
        pop es 
        pop ds                          ; restore segment regs
        add esp,20h                     ; skip old general registers on stack
        iretd

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
int21:                                  ; watch for INT 21h AH=4Ch
        cmp ah,4ch                      ; AH = 4Ch?
        jne intrmatrix+21h              ; if no, go to INT 21h redirection

        mov ds,[cs:selzero]               ; DS -> 0 (beginning of memory)
        mov edi,[cs:codebase]             ; EDI = of PMODE_TEXT from 0

        mov ebx,[cs:oldint15vector]       ; put back old INT 15h handler
        mov [ds:4*15h],ebx

        jmp intrmatrix+21h              ; go to INT 21h redirection

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; INT 31h INTERFACE
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
int31:                                  ; protected mode INT 31h handler
        cli
        cld
        push ds 
        push es 
        push fs 
        push gs                ; push regs needed
        pushad
        mov ds,[cs:selzero]             ; DS -> 0 (beginning of memory)

        push bx
        mov bx,(INT31FUNCNUM - 1) * 2   ; number of functions to check
.int31l0:
        cmp ax,[cs:bx + int31functbl]   ; found function value?
        jne short .int31l0c

        mov bx,[cs:bx + int31routtbl]    ; yes, go to appropriate handler
        xchg bx,[esp]
        ret

.int31l0c:
        sub bx,2                        ; no, continue loop
        jnc .int31l0

        pop bx                          ; no function found
        jmp int31fail8001               ; error 8001h

;-----------------------------------------------------------------------------
int31fail8024:                          ; INT 31h return fail with error 8024h
        mov word [esp+28],8024h     ; set AX on stack to 8024h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8023:                          ; INT 31h return fail with error 8023h
        mov word [esp+28],8023h     ; set AX on stack to 8023h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8022:                          ; INT 31h return fail with error 8022h
        mov word [esp+28],8022h     ; set AX on stack to 8022h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8021:                          ; INT 31h return fail with error 8021h
        mov word [esp+28],8021h     ; set AX on stack to 8021h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8016:                          ; INT 31h return fail with error 8016h
        mov word [esp+28],8016h     ; set AX on stack to 8016h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8015:                          ; INT 31h return fail with error 8015h
        mov word [esp+28],8015h     ; set AX on stack to 8015h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8013:                          ; INT 31h return fail with error 8013h
        mov word [esp+28],8013h     ; set AX on stack to 8013h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8012:                          ; INT 31h return fail with error 8012h
        mov word [esp+28],8012h     ; set AX on stack to 8012h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8011:                          ; INT 31h return fail with error 8011h
        mov word [esp+28],8011h     ; set AX on stack to 8011h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8010:                          ; INT 31h return fail with error 8010h
        mov word [esp+28],8010h     ; set AX on stack to 8010h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31fail8001:                          ; INT 31h return fail with error 8001h
        mov word [esp+28],8001h     ; set AX on stack to 8001h for POPAD
        jmp short int31fail

;-----------------------------------------------------------------------------
int31failcx:                            ; INT 31h return fail with CX,AX
        mov word [esp+24],cx        ; put CX onto stack for POPAD

;-----------------------------------------------------------------------------
int31failax:                            ; INT 31h return fail with AX
        mov word [esp+28],ax        ; put AX onto stack for POPAD

;-----------------------------------------------------------------------------
int31fail:                              ; INT 31h return fail, pop all regs
        popad
        pop gs
        pop fs
        pop es
        pop ds

;-----------------------------------------------------------------------------
int31failnopop:                         ; INT 31h return fail with carry set
        or byte [esp+8],1           ; set carry in EFLAGS on stack
        iretd

;-----------------------------------------------------------------------------
int31okedx:                             ; INT 31h return ok with EDX,CX,AX
        mov [esp+20],edx                ; put EDX onto stack for POPAD
        jmp short int31okcx

;-----------------------------------------------------------------------------
int31okdx:                              ; INT 31h return ok with DX,CX,AX
        mov [esp+20],dx                 ; put DX onto stack for POPAD
        jmp short int31okcx

;-----------------------------------------------------------------------------
int31oksinoax:                          ; INT 31h return ok SI,DI,BX,CX
        mov ax,[esp+28]                 ; get old value of AX for restore

;-----------------------------------------------------------------------------
int31oksi:                              ; INT 31h return ok SI,DI,BX,CX,AX
        mov [esp+4],si                  ; put SI onto stack for POPAD
        mov [esp],di                    ; put DI onto stack for POPAD
        mov [esp+16],bx                 ; put BX onto stack for POPAD

;-----------------------------------------------------------------------------
int31okcx:                              ; INT 31h return ok with CX,AX
        mov [esp+24],cx                 ; put CX onto stack for POPAD

;-----------------------------------------------------------------------------
int31okax:                              ; INT 31h return ok with AX
        mov [esp+28],ax                 ; put AX onto stack for POPAD

;-----------------------------------------------------------------------------
int31ok:                                ; INT 31h return ok, pop all regs
        popad
        pop gs
        pop fs
        pop es
        pop ds

;-----------------------------------------------------------------------------
int31oknopop:                           ; INT 31h return ok with carry clear
        and byte [esp+8],0feh       ; clear carry in EFLAGS on stack
        iretd

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; DESCRIPTOR FUNCTIONS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;-----------------------------------------------------------------------------
int31testsel:                           ; test for valid selector BX
        pop bp                          ; pop return address

        cmp bx,[cs:gdtlimit]            ; selector BX out of range?
        ja int31fail8022                ; if yes, fail with error 8022h

        mov edi,[cs:gdtbase]            ; get base of GDT
        and bl,0f8h                     ; mask table index and RPL
        movzx ebx,bx                    ; EBX = selector index
        test byte [ds:edi+ebx+6],10h; is descriptor used?
        jz int31fail8022                ; if descriptor not used, fail 8022h

        jmp bp                          ; return ok

;-----------------------------------------------------------------------------
int31testaccess:                        ; test access bits in CX
        pop bp                          ; pop return address

        test ch,20h                     ; test MUST BE 0 bit in CH
        jnz int31fail8021               ; if not 0, error 8021h

        test cl,90h                     ; test present and MUST BE 1 bits
        jz int31fail8021                ; if both 0, error 8021h
        jpo int31fail8021               ; if unequal, error 8021h

        test cl,60h                     ; test DPL
        jnz int31fail8021               ; if not 0, error 8021h

        test cl,8                       ; if code, more tests needed
        jz short .int31testselok       ; if data, skip code tests

        test cl,6                       ; test conforming and readable bits
        jz int31fail8021                ; if both 0, error 8021h
        jpe int31fail8021               ; if equal, error 8021h

.int31testselok:
        jmp bp                          ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310000:                              ; allocate descriptors
        or cx,cx                        ; if CX = 0, error 8021h
        jz int31fail8021

        mov edx,[cs:gdtbase]            ; get base of GDT
        movzx eax,word [cs:gdtlimit]           ; EAX = last selector index
        and al,0f8h

        mov bx,cx                       ; BX = number of selectors to find
.int310000l0:
        test byte [ds:edx+eax+6],10h; is descriptor used?
        jnz short .int310000l0f0

        dec bx                          ; found free descriptor, dec counter
        jnz short .int310000l0f1       ; continue if need to find more

        mov ebx,eax                     ; found all descriptors requested
.int310000l1:
        mov dword [ds:edx+ebx],0    ; set entire new descriptor
        mov dword [ds:edx+ebx+4],109200h
        add bx,8                        ; increment selector index
        dec cx                          ; dec counter of descriptors to mark
        jnz .int310000l1               ; loop if more to mark

        jmp int31okax                   ; return ok, with AX

.int310000l0f0:
        mov bx,cx                       ; reset number of selectors to find

.int310000l0f1:
        sub ax,8                        ; dec current selector counter
        cmp ax,8*SYSSELECTORS           ; more descriptors to go?
        jae .int310000l0               ; if yes, loop

        jmp int31fail8011               ; did not find descriptors

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310001:                              ; free descriptor
        call int31testsel               ; test for valid selector BX

        and byte [ds:edi+ebx+6],0efh; mark descriptor as free

        mov cx,4                        ; zero any segregs loaded with BX
        lea ebp,[esp+32]                ; EBP -> selectors on stack
.int310001l0:
        cmp word [ebp],bx           ; selector = BX?
        jne short .int310001l0f0       ; if no, continue loop

        mov word [ebp],0            ; zero selector on stack

.int310001l0f0:
        add ebp,2                       ; increment selector ptr
        loop .int310001l0              ; loop

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310003:                              ; get selector increment value
        mov ax,8                        ; selector increment value is 8
        jmp int31okax                   ; return ok, with AX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310006:                              ; get segment base address
        call int31testsel               ; test for valid selector BX

        mov dx,word [ds:edi+ebx+2]  ; low word of 32bit linear address
        mov cl,byte [ds:edi+ebx+4]  ; high word of 32bit linear address
        mov ch,byte [ds:edi+ebx+7]

        jmp int31okdx                   ; return ok, with DX, CX, AX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310007:                              ; set segment base address
        call int31testsel               ; test for valid selector BX

        mov word [ds:edi+ebx+2],dx  ; low word of 32bit linear address
        mov byte [ds:edi+ebx+4],cl  ; high word of 32bit linear address
        mov byte [ds:edi+ebx+7],ch

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310008:                              ; set segment limit
        call int31testsel               ; test for valid selector BX

        cmp cx,0fh                      ; limit greater than 1M?
        jbe short .int310008f0

        mov ax,dx                       ; yup, limit greater than 1M
        and ax,0fffh
        cmp ax,0fffh                    ; low 12 bits set?
        jne int31fail8021               ; if no, error 8021h

        shrd dx,cx,12                   ; DX = low 16 bits of page limit
        shr cx,12                       ; CL = high 4 bits of page limit
        or cl,80h                       ; set granularity bit in CL

.int310008f0:
        mov word [ds:edi+ebx],dx    ; put low word of limit
        and byte [ds:edi+ebx+6],70h ; mask G and high nybble of limit
        or byte [ds:edi+ebx+6],cl   ; put high nybble of limit

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310009:                              ; set descriptor access rights
        call int31testsel               ; test for valid selector BX

        call int31testaccess            ; test access bits in CX

        or ch,10h                       ; set AVL bit, descriptor used
        and ch,0f0h                     ; mask low nybble of CH
        and byte [ds:edi+ebx+6],0fh ; mask high nybble access rights
        or byte [ds:edi+ebx+6],ch   ; or in high access rights byte
        mov byte [ds:edi+ebx+5],cl  ; put low access rights byte

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31000a:                              ; create alias descriptor
        call int31testsel               ; test for valid selector BX

        mov ax,0000h                    ; allocate descriptor
        mov cx,1
        int 31h
        jc int31fail8011                ; if failed, descriptor unavailable

        push ax                         ; preserve allocated selector

        push ds                         ; copy descriptor and set type data
        pop es
        movzx edi,ax                    ; EDI = target selector
        mov esi,[cs:gdtbase]            ; ESI -> GDT
        add edi,esi                     ; adjust to target descriptor in GDT
        add esi,ebx                     ; adjust to source descriptor in GDT

        db 67h
        movsd
        db 67h
        lodsd
        mov ah,92h                      ; set descriptor type - R/W up data
        db 67h
        stosd

        pop ax                          ; restore allocated selector

        jmp int31okax                   ; return ok, with AX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31000b:                              ; get descriptor
        call int31testsel               ; test for valid selector BX

        lea esi,[edi+ebx]               ; ESI -> descriptor in GDT
        mov edi,[esp]                   ; get EDI buffer ptr from stack
        db 67h
        movsd
        db 67h
        movsd

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31000c:                              ; set descriptor
        call int31testsel               ; test for valid selector BX

        mov esi,[esp]                   ; ESI = EDI buffer ptr from stack
        mov cx,[es:esi+5]               ; get access rights from descriptor
        call int31testaccess            ; test access bits in CX

        push ds                         ; swap DS and ES, target and source
        push es
        pop ds
        pop es

        add edi,ebx                     ; adjust EDI to descriptor in GDT
        db 67h
        movsd
        db 67h
        lodsd
        or eax,100000h                  ; set descriptor AVL bit
        db 67h
        stosd

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31000e:                              ; get multiple descriptors
        mov ax,000bh                    ; function 000bh, get descriptor

;-----------------------------------------------------------------------------
int31000ef:                             ; common to funcions 000eh and 000fh
        or cx,cx                        ; if CX = 0, return ok immediately
        jz int31ok

        mov dx,cx                       ; DX = number of descriptors
        xor cx,cx                       ; CX = successful counter
.int31000efl0:
        mov bx,[es:edi]                 ; BX = selector to get
        add edi,2

        int 31h                         ; get/set descriptor
        jc int31failcx                  ; if error, fail with AX and CX

        add edi,8                       ; increment descriptor ptr
        inc cx                          ; increment successful copy counter
        dec dx                          ; decrement loop counter
        jnz .int31000efl0              ; if more descriptors to go, loop

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31000f:                              ; set multiple descriptors
        mov ax,000ch                    ; function 000ch, set descriptor

        jmp int31000ef                  ; go to common function

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; DOS allocation functions
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310100:                              ; allocate real mode memory
    push	bx         		; save size
    mov	es,[cs:selzero]		; use function 300 to get mem
    mov	edi,[cs:codebase]
    mov	word [es:edi + raw300buf + 1ch],04800h ; ax
    mov	[es:edi + raw300buf + 10h],bx	; bx
    mov	ax,300h
    mov	bx,21h
    mov	cx,0
    lea	edi,[edi + raw300buf]
    int	31h
    pop	bx
    jc	int31failax

    mov	dx,[cs:raw300buf + 10h]	; return size in paragraphs
    mov	ax,[cs:raw300buf + 1ch] ; and error code
    mov	[esp+18],dx		; if DOS barfed
    test	byte [cs:raw300buf + 20h],1
    jnz	int31failax

    push	bx			; get a selector
    mov	ax,0
    mov	cx,1
    int	31h
    pop	cx
    jc	int31failax

    mov	bx,ax
    push	bx 			; set base
    push	cx
    mov	cx,word [cs:raw300buf + 1ch]
    mov	dx,cx
    shr	cx,12
    shl	dx,4
    mov	ax,7
    int	31h
    pop	cx
    pop	bx
    jc	int31failax

    push	bx			; set limit
    mov	dx,cx
    shl	dx,4
    shr	cx,12
    mov	ax,8
    int	31h
    pop	dx
    jc	int31failax

    mov	[esp+20],dx		; return selector in dx
    mov	ax,[cs:raw300buf + 1ch]	; and real mode par in ax
    jmp	int31okax


;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310101:                              ; free real mode memory
    mov	bx,dx			; check sel
    call	int31testsel

    push	bx  			; get selector base
    mov	ax,6
    int	31h
    pop	bx
    shl	cx,12
    shr	dx,4
    or	cx,dx

    push	cx  			; free selector
    mov	ax,1
    int	31h
    pop	cx

    mov	es,[cs:selzero]		; free real memory
    mov	edi,[cs:codebase]
    mov	word [es:edi + raw300buf + 1ch],04900h ; ax
    mov	[es:edi + raw300buf + 22h],cx	; es
    mov	ax,300h
    mov	bx,21h
    mov	cx,0
    lea	edi,[edi + raw300buf]
    int	31h
    jc	int31failax

    mov	ax,[cs:raw300buf + 10h]	; return pars left in ax
    test byte [cs:raw300buf + 20h],1	; if dos didn't like it
    jnz	int31failax
    jmp	int31ok
    
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310102:                              ; resize real mode memory
    xchg	bx,dx                   ; bx = descriptor, dx = size
    call	int31testsel

    push	dx			; set new limit
    push	bx
    mov	cx,dx
    shr	cx,12
    shl	dx,4
    mov	ax,8
    int	31h
    pop	bx
    pop	dx
    jc	int31failax

    push	dx			; get base
    mov	ax,6
    int	31h
    pop	bx
    shl	cx,12
    shr	dx,4
    or	cx,dx

    mov	es,[cs:selzero]		; resize
    mov	edi,[cs:codebase]
    mov	word [es:edi + raw300buf + 1ch],04A00h ; ax
    mov	[es:edi + raw300buf + 10h],bx ; bx
    mov	[es:edi + raw300buf + 22h],cx ; es
    mov	ax,300h
    mov	bx,21h
    mov	cx,0
    lea	edi,[edi + raw300buf]
    int	31h
    jc	int31failax

    mov	dx,[cs:raw300buf + 10h]	; return size in paragraphs
    mov	[esp+18],dx		; if DOS barfed
    mov	ax,[cs:raw300buf + 10h]
    test byte [cs:raw300buf + 20h],1
    jnz	int31failax
    jmp	int31ok
    

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; INTERRUPT FUNCTIONS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310200:                              ; get real mode interrupt vector
        movzx ebx,bl                    ; EBX = BL (interrupt number)
        mov dx,[ds:ebx*4]               ; load real mode vector offset
        mov cx,[ds:ebx*4+2]             ; load real mode vector segment

        jmp int31okdx                   ; return ok, with AX, CX, DX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310201:                              ; set real mode interrupt vector
        movzx ebx,bl                    ; EBX = BL (interrupt number)
        mov [ds:ebx*4],dx               ; set real mode vector offset
        mov [ds:ebx*4+2],cx             ; set real mode vector segment

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310204:                              ; get protected mode interrupt vector
        cmp bl,3                        ; INT 3 vector?
        je short .int310204f00         ; if yes, go to special INT 3 handling

        movzx ebx,bl                    ; EBX = BL (interrupt number)
        shl ebx,3                       ; adjust for location in IDT
        add ebx,[cs:idtbase]            ; add base of IDT

        mov edx,dword [ds:ebx+4]    ; get high word of offset
        mov dx,word [ds:ebx]        ; get low word of offset
        mov cx,word [ds:ebx+2]      ; get selector

        jmp int31okedx                  ; return ok, with AX, CX, EDX

.int310204f00:
        mov edx,dword [cs:int3vector]      ; get of INT 3
        mov cx,word [cs:int3vector + 4]        ; get selector of INT 3

        jmp int31okedx                  ; return ok, with AX, CX, EDX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310205:                              ; set protected mode interrupt vector
        xchg bx,cx                      ; swap int number with int selector
        call int31testsel               ; test for valid selector BX

        cmp cl,3                        ; INT 3 vector?
        je short .int310205f00         ; if yes, go to special INT 3 handling

        movzx ecx,cl                    ; ECX = CL (interrupt number)
        shl ecx,3                       ; adjust for location in IDT
        add ecx,[cs:idtbase]            ; add base of IDT

        mov word [ds:ecx],dx        ; set low word of offset
        shr edx,16
        mov word [ds:ecx+6],dx      ; set high word of offset
        mov word [ds:ecx+2],bx      ; set selector

        jmp int31ok                     ; return ok

.int310205f00:
        mov edi,[cs:codebase]             ; EDI = of PMODE_TEXT from 0

        mov dword [edi + int3vector],edx  ; set of INT 3
        mov word [edi + int3vector+4],bx    ; set selector of INT 3

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310900:                              ; get and disable interrupt state
        add esp,26h                     ; adjust stack
        pop ds                          ; restore DS

        btc word [esp+8],9          ; test and clear IF bit in EFLAGS
        setc al                         ; set AL = carry (IF flag from EFLAGS)

        jmp int31oknopop                ; return ok, dont pop registers

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310901:                              ; get and enable interrupt state
        add esp,26h                     ; adjust stack
        pop ds                          ; restore DS

        bts word [esp+8],9          ; test and set IF bit in EFLAGS
        setc al                         ; set AL = carry (IF flag from EFLAGS)

        jmp int31oknopop                ; return ok, dont pop registers

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310902:                              ; get interrupt state
        add esp,26h                     ; adjust stack
        pop ds                          ; restore DS

        bt word [esp+8],9           ; just test IF bit in EFLAGS
        setc al                         ; set AL = carry (IF flag from EFLAGS)

        jmp int31oknopop                ; return ok, dont pop registers

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; REAL/PROTECTED MODE TRANSLATION FUNCTIONS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310300:                              ; simulate real mode interrupt
        movzx ebx,bl                    ; get real mode INT CS:IP
        mov ebp,dword [ds:ebx*4]

        jmp short int3103               ; go to common code

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310301:                              ; call real mode FAR procedure
                                        ; same start as function 0302h
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310302:                              ; call real mode IRET procedure
        mov ebp,dword [es:edi+2ah]  ; get target CS:IP from structure

;-----------------------------------------------------------------------------
int3103:                                ; common to 0300h, 0301h, and 0302h
        mov esi,[cs:codebase]           ; ESI = of PMODE_TEXT from 0

        movzx ebx,word [es:edi+2eh] ; EBX = SP from register structure
        movzx edx,word [es:edi+30h] ; EDX = SS from register structure

        mov ax,bx                       ; check if caller provided stack
        or ax,dx
        jnz short .int3103f3           ; if yes, go on to set stack

        mov dx,[cs:rmstacktop]          ; DX = SS for real mode redirection
        mov bx,[cs:_pm_rmstacklen]      ; get size of real mode stack
        sub dx,bx                       ; adjust DX to next stack location

        cmp dx,[cs:rmstackbase]         ; exceeded real mode stack space?
        jb int31fail8012                ; if yes, error 8012h

        mov [esi + rmstacktop],dx       ; update ptr for possible reenterancy
        shl bx,4                        ; adjust BX from paragraphs to bytes

.int3103f3:
        lea edi,[edx*4]                 ; EDI -> top of real mode stack
        lea edi,[ebx+edi*4]

        lea ax,[bx-8]                   ; AX = top of stack parms
        xchg ax,[esi + rmstackparmtop]  ; preserve and set new top of stack
        push ax                         ;  parms for possible reenterancy

        movzx ax,byte [esp+30]      ; AX = AL of original INT 31h call
        and al,1                        ; if function 0301h, AL=0, else, AL=2
        xor al,1
        shl al,1
        sub bx,ax                       ; adjust BX for possible FLAGS

        movzx eax,cx                    ; EAX = length of stack parms
        shl eax,1
        sub bx,36h                      ; adjust real mode SP for needed vars
        sub bx,ax                       ; adjust real mode SP for stack parms

        mov word [ds:edi-2],ss      ; store SS:ESP on real mode stack
        mov [ds:edi-6],esp
        mov word [ds:edi-8],es      ; store ES on real mode stack

        push ds                         ; swap DS and ES
        push es
        pop ds
        pop es

        std                             ; string copy backwards

        sub edi,10                      ; copy stack parms from protected mode
        movzx ecx,cx                    ;  stack to real mode stack
        lea esi,[esp+ecx*2+36h-2]
        db 67h
        db 36h; ss:
        rep movsw

        mov esi,[esp+2]                 ; ESI = of structure from stack
        mov ax,[esi+20h]                ; AX = FLAGS from register structure

        mov [es:edi],ax                 ; store data for real mode return IRET

        cmp byte [esp+30],1         ; check AL on stack for function code
        je short .int3103f4            ; if function 0301h, go on

        and ah,0fch                     ; 0300h or 0302h, clear IF and TF flag

.int3103f4:
        cld                             ; string copy forward
        lea edi,[edx*4]                 ; EDI -> bottom of stack
        lea edi,[ebx+edi*4]

        mov ecx,8                       ; copy general regs to real mode stack
        db 67h
        rep movsd

        add esi,6                       ; copy FS and GS to real mode stack
        db 67h
        movsd

        mov word [es:edi+8],PMODE_TEXT      ; return address from call
        mov word [es:edi+6],.int3103f1

        mov [es:edi+4],ax               ; store FLAGS for real mode IRET maybe
        mov dword [es:edi],ebp      ; put call address to real mode stack

        mov ax,[esi-6]                  ; real mode DS from register structure
        mov cx,[esi-8]                  ; real mode ES from register structure

        mov si,PMODE_TEXT               ; real mode target CS:IP
        mov di,.int3103f0

        db 66h                          ; jmp far, as in 32bit offset,
        jmp word [cs:pmtormswrout]    ;  not seg:16bit offset

.int3103f0:                            ; real mode INT, FAR, or IRET call
        popad                           ; load regs with call values
        pop fs
        pop gs

        iret                            ; go to call address

.int3103f1:
        mov sp,[cs:rmstackparmtop]        ; remove stack parameters

        push gs 
        push fs 
        push ds 
        push es                ; store registers on stack
        pushf                           ; store flags on stack
        pushad
        cli

        mov ax,ss                       ; EAX = linear ptr to SS
        movzx eax,ax
        shl eax,4
        movzx ebp,sp                    ; EBP = SP

        mov cx,[bp+2ah]                 ; get protected mode ES from stack
        mov ebx,[bp+2ch]                ; get protected mode SS:ESP from stack
        mov dx,[bp+30h]

        add ebp,eax                     ; EBP -> stored regs on stack

        mov ax,SELZERO                  ; DS selector value for protected mode
        mov si,SELCODE                  ; target CS:EIP in protected mode
        mov edi,.int3103f2

        jmp word [cs:rmtopmswrout]             ; go back to protected mode

.int3103f2:
        mov edi,[cs:codebase]           ; restore old stack parameter length
        pop word [edi + rmstackparmtop]

        mov edi,[esp]                   ; get structure from stack
        mov esi,ebp                     ; copy return regs from real mode
        mov ecx,15h                     ;  stack to register structure
        cld
        db 67h
        rep movsw

        cmp dword [es:edi+4],0      ; stack provided by caller?
        jne int31ok                     ; if yes, done now

        mov edi,[cs:codebase]           ; restore top of real mode stack
        mov ax,[cs:_pm_rmstacklen]
        add [ds:edi + rmstacktop],ax

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310303:                              ; allocate real mode callback address
        mov bl,[cs:_pm_callbacks]       ; CL = total number of callbacks
        or bl,bl                        ; are there any?
        jz int31fail8015                ; if no, error 8015h

        mov edx,[cs:callbackbase]       ; EDX -> base of callbacks
        mov ecx,edx                     ; for later use

.int310303l0:
        cmp word [edx+3],0          ; is this callback free?
        jz short .int310303f0          ; if yes, allocate

        add edx,25                      ; increment ptr to callback
        dec bl                          ; decrement loop counter
        jnz .int310303l0               ; if more callbacks to check, loop

        jmp int31fail8015               ; no free callback, error 8015h

.int310303f0:
        mov bx,[esp+38]                 ; BX = caller DS from stack
        mov [edx+3],bx                  ; store callback parms in callback
        mov [edx+7],esi
        mov word [edx+12],es
        mov [edx+16],edi

        sub edx,ecx                     ; DX = of callback
        shr ecx,4                       ; CX = segment of callback

        jmp int31okdx                   ; return ok, with DX, CX, AX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310304:                              ; free real mode callback address
        cmp cx,[cs:callbackseg]         ; valid callback segment?
        jne int31fail8024               ; if no, error 8024h

        movzx ebx,dx                    ; EBX = of callback

        xor ax,ax                       ; check if valid offset
        xchg dx,ax
        mov cx,25
        div cx

        or dx,dx                        ; is there a remainder
        jnz int31fail8024               ; if yes, not valid, error 8024h

        or ah,ah                        ; callback index too big?
        jnz int31fail8024               ; if yes, not valid, error 8024h

        cmp al,[cs:_pm_callbacks]       ; callback index out of range?
        jae int31fail8024               ; if yes, not valid, error 8024h

        add ebx,[cs:callbackbase]       ; EBX -> callback
        mov word [ebx+3],0          ; set callback as free

        jmp int31ok                     ; return ok

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; MISC FUNCTIONS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310305:                              ; get state save/restore addresses
        add esp,26h                     ; adjust stack
        pop ds                          ; restore DS

        xor ax,ax                       ; size needed is none
        mov bx,PMODE_TEXT               ; real mode seg of same RETF
        mov cx,vxr_saverestorerm    ; same of 16bit RETF
        mov si,cs                       ; selector of routine is this one
        mov edi,vxr_saverestorepm   ; of simple 32bit RETF

        jmp int31oknopop                ; return ok, dont pop registers

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310306:                              ; get raw mode switch addresses
        add esp,26h                     ; adjust stack
        pop ds                          ; restore DS

        mov si,cs                       ; selector of pmtorm rout is this one
        mov edi,[cs:pmtormswrout]       ; in this seg of rout
        mov bx,PMODE_TEXT               ; real mode seg of rmtopm rout
        mov cx,[cs:rmtopmswrout]        ; of rout in real mode

        jmp int31oknopop                ; return ok, dont pop registers

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310400:                              ; get version
        add esp,26h                     ; adjust stack
        pop ds                          ; restore DS

        mov ax,100h                     ; return version 1.0
        mov bx,3                        ; capabilities
        mov cl,[cs:processortype]       ; processor type
        mov dx,word [cs:picslave]       ; master and slave PIC values

        jmp int31oknopop                ; return ok, dont pop registers

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; VCPI EXTENDED MEMORY FUNCTIONS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;-----------------------------------------------------------------------------
int310500vsiditoesi:                    ; convert handle SI:DI to ptr ESI
        pop bp                          ; pop return address

        test di,3                       ; is handle (ptr) aligned on dword?
        jnz int31fail8023               ; if no, error 8023h

        shl esi,16                      ; ESI = SI:DI
        mov si,di

        cmp esi,[cs:pagetablefree]      ; handle too low?
        jb int31fail8023                ; if yes, error 8023h

        cmp esi,[cs:pagetabletop]       ; handle too high?
        jae int31fail8023               ; if yes, error 8023h

        test byte [esi+1],2         ; is page first in allocated block?
        jz int31fail8023                ; if no, error 8023h

        jmp bp                          ; return ok

;-----------------------------------------------------------------------------
int310500vbxcxtoebx:                    ; convert BX:CX bytes to EBX pages
        pop bp                          ; pop return address

        shl ebx,16                      ; EBX = BX:CX
        mov bx,cx

        or ebx,ebx                      ; check for invalid value
        jz int31fail8021                ; if invalid value, error 8021h

        add ebx,0fffh                   ; convert EBX to page count
        shr ebx,12

        jmp bp                          ; return ok

;-----------------------------------------------------------------------------
int310500vpmalloc:                      ; allocate physical memory block
        mov edi,esi                     ; EDI = ESI, ptr to linear block start
        xor ebp,ebp                     ; EBP = running allocated page count

.int310500vpmallocl0:
        mov ax,0de04h                   ; VCPI allocate a page
        db 066h
        call far [cs:vcpi_calleip]

        or ah,ah                        ; got a page?
    jz short .int310500vpmallocl0f0; if yes, go on

    cmp ebp,1			; allocated any pages?
    jc short .int310500vpmallocdone; if no, fail immediately

    or byte [edi-3],4		; set last allocated page as last

    call int310500vfree		; free what was allocated

    stc				; carry set, failed
    jmp short .int310500vpmallocdone	; go to done

.int310500vpmallocl0f0:
        and dh,0f0h                     ; clear 4 bits of page table entry
        mov dl,7                        ; set page as user/writeable/present
        mov [edi],edx                   ; store page in page table
    add edi,4			; increment page table ptr

    inc ebp 			; increment allocated page count
    cmp ebp,ebx			; allocated all needed pages?
    jb .int310500vpmallocl0	; if no, loop

        or byte [esi+1],2           ; set first allocated page as first
        or byte [edi-3],4           ; set last allocated page as last
        clc                             ; carry clear, success

.int310500vpmallocdone:
        mov eax,ebp                     ; EAX = number of pages allocated
    ret				; return

;-----------------------------------------------------------------------------
int310500vlmalloc:                      ; check for linear memory block
        mov edi,[cs:pagetablefree]      ; EDI = search ptr in page table

        mov ecx,[cs:pagetabletop]       ; ECX = count of pages to search
        sub ecx,edi
        shr ecx,2

        xor edx,edx                     ; EDX = largest linear block found
        xor eax,eax                     ; EAX = search unit, free entry (0)

        push ebx                        ; preserve EBX, memory requested

.int310500vlmallocl0:
        jecxz .int310500vlmallocdone     ; if no more entries, done

        db 67h
        repne scasd                     ; search for first next free entry
        jne short .int310500vlmallocdone       ; if no more free, go on

        mov ebp,ecx                     ; EBP = current count
        lea ebx,[edi-4]                 ; EBX = start of free block

        db 67h
        repe scasd                      ; search for end of free linear block
        jne short .int310500vlmallocl0f0; if previous entry not free, go on

        inc ebp                         ; previous entry free, extra one

.int310500vlmallocl0f0:
        sub ebp,ecx                     ; EBP = number of free pages in block

        cmp ebp,edx                     ; new block larger than last largest?
        jb .int310500vlmallocl0        ; if no, loop

        mov esi,ebx                     ; ESI = ptr to largest block found
        mov edx,ebp                     ; size of new largest block found

        cmp ebp,[esp]                   ; block sufficient for memory request?
        jb .int310500vlmallocl0        ; if no, loop

.int310500vlmallocdone:
        pop ebx                         ; restore EBX, memory requested
        ret                             ; return

;-----------------------------------------------------------------------------
int310500vmalloc:                       ; allocate linear+physical mem block
        call int310500vlmalloc          ; try to allocate linear memory block

        cmp edx,1                       ; found ANY free linear area?
        jc short .int310500vmallocdone ; if no, done

        cmp edx,ebx                     ; linear block enough for request?
        jb short .int310500vmallocf0   ; if no, go to physical memory check

        call int310500vpmalloc          ; try to allocate physical mem

        mov cl,1                        ; error is not enough physical memory
        jmp short .int310500vmallocdone; go to done

.int310500vmallocf0:
        mov ebx,edx                     ; only linear block size physical mem

        call int310500vpmalloc          ; try to allocate physical memory
        jc short .int310500vmallocfaillinear   ; if failed, done

        call int310500vfree             ; success, so must free block

        mov eax,ebx                     ; can allocate this much total memory
        stc                             ; carry set, failed

.int310500vmallocfaillinear:
        mov cl,0                        ; error is not enough linear memory

.int310500vmallocdone:
        ret                             ; return

;-----------------------------------------------------------------------------
int310500vfree:                         ; free linear+physical memory block
        mov edi,esi                     ; EDI = ESI, ptr to linear block start

.int310500vfreel0:
        xor ecx,ecx                     ; new page table entry is free (0)
    xchg ecx,[edi]			; swap ECX with page table entry
    add edi,4			; increment page table ptr

        mov edx,ecx                     ; EDX = page table entry
        and dx,0f000h                   ; mask low 12 bits

        mov ax,0de05h                   ; VCPI free a page
        db 066h
        call far [cs:vcpi_calleip]

        test ch,4                       ; last page of block?
        jz .int310500vfreel0           ; if no, loop

        ret                             ; return

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310500v:                             ; VCPI get free memory information
        push ds                         ; ES = DS for VCPI malloc functions
        pop es

        mov ebx,0ffffffffh              ; try to allocate an impossible amount
        call int310500vmalloc

        shl eax,12                      ; returned EAX is highest possible

        jmp int310500xsetbuf            ; put memory information in buffer

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310501v:                             ; VCPI allocate memory block
        push ds                         ; ES = DS for VCPI malloc functions
        pop es

        call int310500vbxcxtoebx        ; convert BX:CX bytes to EBX pages

        call int310500vmalloc           ; try to allocate requested amount
        jnc short int310501vaddxnhandle ; if successful, go to done

        or cl,cl                        ; error is not enough linear memory?
        jz int31fail8012                ; if yes, error 8013h
        jmp int31fail8013               ; error is physical, error 8013h

int310501vaddxnhandle:
        mov ecx,esi                     ; figure address of block from handle
        sub ecx,[cs:pagetablebase]
        shl ecx,10
        shld ebx,ecx,16

        mov di,si                       ; SI:DI = ESI, handle
        shr esi,16

        mov eax,[cs:vcpi_cr3]           ; reload CR3 to clear TLB
        mov cr3,eax

        jmp int31oksinoax               ; return ok, with SI, DI, BX, CX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310502v:                             ; VCPI free memory block
        call int310500vsiditoesi        ; convert handle SI:DI to ptr ESI

        call int310500vfree             ; free memory block

        mov eax,[cs:vcpi_cr3]           ; reload CR3 to clear TLB
        mov cr3,eax

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310503v:                             ; VCPI resize memory block
        push ds                         ; ES = DS for VCPI malloc functions
        pop es

        call int310500vbxcxtoebx        ; convert BX:CX bytes to EBX pages

        call int310500vsiditoesi        ; convert handle SI:DI to ptr ESI

        mov edi,esi                     ; EDI = ESI, ptr to linear block start
        xor ebp,ebp                     ; EBP = running block size in pages

.int310503vl0:
        add edi,4                       ; increment page table ptr
        inc ebp                         ; increment block size

        test byte [edi-3],4         ; last page of block?
        jz .int310503vl0               ; if no, loop

        sub ebx,ebp                     ; EBX = change in block size
        jz int310501vaddxnhandle        ; if no change, done

        jc near .int310503vf0               ; if block made smaller, just free top

        mov ecx,[cs:pagetabletop]       ; ECX = count of pages to search
        sub ecx,edi
        shr ecx,2

        mov edx,ecx                     ; EDX = current count
        xor eax,eax                     ; EAX = search unit, free entry (0)

        jecxz .int310503vf3      ; if no entries above, try below

        db 67h
        repe scasd                      ; check for free entries above block
        je short .int310503vf2         ; if previous entry free, go on

        dec edx                         ; previous entry not free, minus one

.int310503vf2:
        sub edx,ecx                     ; EDX = number of free pages in block

        cmp edx,ebx                     ; enough linear memory?
        jb short .int310503vf3         ; if no, try below in linear memory

        push esi                        ; preserve start of block
        lea esi,[esi+ebp*4]             ; ESI -> start of new block for alloc

        call int310500vpmalloc          ; try to allocate physical memory
        mov edi,esi                     ; EDI -> start of new block
        pop esi                         ; restore start of old block
        jc int31fail8013                ; if alloc failed, error 8013h

        and byte [edi-3],0fbh       ; clear last bit in old block end
        and byte [edi+1],0fdh       ; clear first bit in new block start

        jmp int310501vaddxnhandle       ; go to done

.int310503vf3:
        mov ecx,esi                     ; ECX = count of pages to search up
        sub ecx,[cs:pagetablefree]
        shr ecx,2

        or ecx,ecx                      ; any linear memory below?
        jz near .int310503vf1               ; if no, try to allocate

        push ebp                        ; preserve size of original block

        lea edi,[esi-4]                 ; EDI = ESI, ptr to linear block start
        mov ebp,ecx                     ; EBP = current count

        std                             ; search is up
        db 67h
        repe scasd                      ; check for free entries after block
        cld
        je short .int310503vf4         ; if previous entry free, go on

        dec ebp                         ; previous entry not free, minus one

.int310503vf4:
        sub ebp,ecx                     ; EBP = number of free pages in block
        lea eax,[ebp+edx]               ; free size below + free size above

        pop ebp                         ; restore original block size

        cmp eax,ebx                     ; enough linear memory?
        jb near .int310503vf1               ; if no, try to allocate

        push esi                        ; preserve original block address

        sub ebx,edx                     ; EBX = number of pages needed below
        lea eax,[ebx*4]                 ; get base of block below
        sub esi,eax

        push edx
        push ebp                        ; preserve some vars
        call int310500vpmalloc          ; try to allocate physical memory
        pop ebp 
        pop edx                     ; restore some vars

        mov edi,esi                     ; EDI -> base of block below
        pop esi                         ; restore base of original block
        jc int31fail8013                ; if alloc failed, error 8013h

        or edx,edx                      ; any pages needed above?
        jz short .int310503vf6         ; if no, go on

        push esi 
        push edi 
        push ebp                        ; preserve some vars

        mov ebx,edx                     ; EBX = size of block below
        lea esi,[esi+ebp*4]             ; ESI -> start of block above

        call int310500vpmalloc          ; try to allocate physical memory
        pop ebp 
        pop edi
        pop esi                         ; restore some vars
        jnc short .int310503vf5        ; if allocated ok, go on

        mov esi,edi                     ; ESI -> allocated block below
        call int310500vfree             ; free allocated block below

        jmp int31fail8013               ; fail, error 8013h

.int310503vf5:
        and byte [esi-3],0fbh       ; clear last bit in below block end
        and byte [esi+ebp*4+1],0fdh ; clear first bit in above block start

.int310503vf6:
        and byte [edi+1],0fdh       ; clear first bit in below block start
        and byte [esi+ebp*4-3],0fbh ; clear last bit in old block end

        push edi                        ; preserve new block start

        mov edx,edi                     ; EDX = base of move area
        lea ebx,[esi-4]                 ; EBX = current location in move area

.int310503vl1:
        mov edi,ebx                     ; set up to shift up a page
        mov esi,ebx
        mov ecx,ebp

        db 67h
        lodsd
        db 67h
        rep movsd
        db 67h
        stosd

        sub ebx,4                       ; decrement to next page to shift
        cmp ebx,edx                     ; more pages to shift?
        jae .int310503vl1              ; if yes, loop

        pop esi                         ; restore new block start address

        jmp int310501vaddxnhandle       ; go to done

.int310503vf1:
        add ebx,ebp                     ; restore EBX as requested size

        push esi 
        push ebp                        ; preserve some vars
        call int310500vlmalloc          ; check for linear memory block
        pop ebp 
        pop edi                         ; restore some vars

        cmp edx,ebx                     ; enough linear memory?
        jb int31fail8012                ; if no, error 8012h

        sub ebx,ebp                     ; EBX = extra pages needed
        push esi                        ; preserve for later copy
        lea esi,[esi+ebp*4]             ; ESI -> start of extra space needed

        push edi 
        push ebp                        ; preserve some vars
        call int310500vpmalloc          ; try to allocate physical memory
        pop ecx 
        pop esi 
        pop edi                         ; restore some vars
        jc int31fail8013                ; if not enough mem, error 8013h

        push edi 
        push esi 
        push ecx                ; preserve, new and old block, size

        db 67h
        rep movsd

        and byte [edi-3],0fbh       ; clear last bit in old block end
        and byte [edi+1],0fdh       ; clear first bit in new block start

        pop ecx 
        pop edi                     ; restore to clear old block

        xor eax,eax                     ; new page table entry is free (0)
        db 67h
        rep stosd

        pop esi                         ; restore new block address

        jmp int310501vaddxnhandle       ; go to done

.int310503vf0:
        sub edi,4                       ; decrement page table ptr

        xor edx,edx                     ; new page table entry is free (0)
        xchg edx,[edi]                  ; swap EDX with page table entry
        and dx,0f000h                   ; mask low 12 bits

        mov ax,0de05h                   ; VCPI free a page
        db 066h
        call far [cs:vcpi_calleip]

        inc ebx                         ; increment negative change counter
        jnz .int310503vf0              ; if more pages to free, loop

        or byte [edi-3],4           ; set next page up as last of block

        jmp int310501vaddxnhandle       ; go to done

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31050av:                             ; VCPI get memory block size and base
        shrd ecx,esi,16                 ; figure address of block from handle
        mov cx,di
        sub ecx,[cs:pagetablebase]
        shl ecx,10
        shld ebx,ecx,16

        call int310500vsiditoesi        ; convert handle SI:DI to ptr ESI

        xor edi,edi                     ; EDI = running page count

.int31050avl0:
        inc edi                         ; increment page count

        db 67h
        lodsd                           ; EAX = page table entry
        test ah,4                       ; is this the last page of the block?
        jz .int31050avl0               ; if no, loop

        shl edi,12                      ; convert EDI pages to bytes
        shld esi,edi,16                 ; SI:DI = EDI, size in bytes

        jmp int31oksinoax               ; return ok, with SI, DI, BX, CX

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; XMS EXTENDED MEMORY FUNCTIONS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;-----------------------------------------------------------------------------
int310500xsetbuf:
        mov es,[esp+24h]       ; get ES:EDI buffer ptr from stack
        mov edi,[esp]

        db 67h
        stosd                           ; store block size in output buffer
        mov ecx,0bh                     ; fill rest of buffer with 0ffffffffh
        mov eax,0ffffffffh
        db 67h
        rep stosd

        jmp int31ok                     ; return ok

;-----------------------------------------------------------------------------
int310500xcallxms:                      ; make a all to real mode XMS driver
        pop bp                          ; pop return address
        sub esp,32h                     ; stack space for register structure

        mov [esp+1ch],ax                ; put AX in register structure
        mov [esp+10h],bx                ; put BX in register structure
        mov [esp+18h],cx                ; put CX in register structure
        mov [esp+14h],dx                ; put DX in register structure

        mov word [esp+20h],0            ; zero FLAGS in register structure
        mov dword [esp+2eh],0           ; zero SS:SP in register structure
        mov eax,dword [cs:xms_callip]   ; put XMS driver address in CS:IP in
        mov [esp+2ah],eax               ;  regis]er structure

        push ss                         ; ES:EDI -> register structure
        pop es
        mov edi,esp

        xor cx,cx                       ; copy 0 words as stack parameters
        xor bh,bh                       ; doesnt really need to be here
        mov ax,301h                     ; call real mode FAR procedure
        int 31h

        mov ax,[esp+1ch]                ; get AX from register structure
        mov bx,[esp+10h]                ; get BX from register structure
        mov cx,[esp+18h]                ; get CX from register structure
        mov dx,[esp+14h]                ; get DX from register structure

        lea esp,[esp+32h]               ; adjust ESP without changing FLAGS
        jc int31fail8010                ; if INT 31h failed, error 8010h

        jmp bp                          ; return ok

;-----------------------------------------------------------------------------
int310500xbxcxtodx:                     ; convert BX:CX bytes to DX K
        pop bp                          ; pop return address

        mov dx,cx                       ; check for invalid value, BX=CX=0
        or dx,bx
        jz int31fail8021                ; if invalid value, error 8021h

        add cx,1023+15                  ; adjust for size in K and align
        adc bx,0

        test bh,0fch                    ; memory request too high
        jnz int31fail8013               ; if yes, error 8013h

        shrd cx,bx,10                   ; CX = memory in K
        mov dx,cx

        jmp bp                          ; return ok

;-----------------------------------------------------------------------------
int310500xerror:                        ; XMS error, return with DPMI error
        cmp bl,0a0h                     ; out of memory?
        je int31fail8013                ; if yes, error 8013h

        cmp bl,0a1h                     ; handles exhausted?
        je int31fail8016                ; if yes, error 8016h

        cmp bl,0a2h                     ; invalid handle?
        je int31fail8023                ; if yes, error 8023h

        jmp int31fail8010               ; else, error 8010h

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310500x:                             ; XMS get free memory information
        mov ah,8                        ; get largest free memory block in K
        call int310500xcallxms

        movzx eax,ax                    ; EAX = free memory in bytes
        shl eax,10

        sub eax,15                      ; adjust by extra alignment size
        jnc int310500xsetbuf            ; if no overflow, put info to buffer

        xor eax,eax                     ; overflow, so no memory available
        jmp int310500xsetbuf            ; put memory information in buffer

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310501x:                             ; XMS allocate memory block
        call int310500xbxcxtodx         ; convert BX:CX bytes to DX K
        mov cx,dx                       ; preserve size of block in K

        mov ah,9                        ; allocate DX K of XMS memory
        call int310500xcallxms
        or ax,ax                        ; error?
        jz int310500xerror              ; if yes, convert XMS error to DPMI

        mov si,dx                       ; get DPMI handle from XMS handle

int310501xgotmem:
        mov ah,0ch                      ; lock memory block
        call int310500xcallxms
        or ax,ax                        ; error?
        jnz short .int310501xf0        ; if no, go on

        push bx                         ; yup, preserve error number

        mov ah,0ah                      ; free block causing lock error
        call int310500xcallxms

        pop bx                          ; restore error number
        jmp int310500xerror             ; XMS error, return with DPMI error

.int310501xf0:
        mov di,cx                       ; low word of DPMI handle is size in K

        mov cx,bx                       ; XMS linear address to DPMI regs
        mov bx,dx

        add cx,0fh                      ; align linear address on paragraph
        adc bx,0
        and cl,0f0h

        jmp int31oksinoax               ; return ok, with SI, DI, BX, CX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310502x:                             ; XMS free memory block
        mov dx,si                       ; get XMS handle from DPMI handle

        mov ah,0dh                      ; unlock memory block
        call int310500xcallxms
        or ax,ax                        ; error?
        jz int310500xerror              ; if XMS error, return with DPMI error

        mov ah,0ah                      ; free memory block
        call int310500xcallxms
        or ax,ax                        ; error?
        jz int310500xerror              ; if yes, convert XMS error to DPMI

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310503x:                             ; XMS resize memory block
        call int310500xbxcxtodx         ; convert BX:CX bytes to DX K
        mov cx,dx                       ; preserve size of block in K
        mov dx,si                       ; get XMS handle from DPMI handle

        mov ah,0dh                      ; unlock memory block
        call int310500xcallxms
        or ax,ax                        ; error?
        jz int310500xerror              ; if XMS error, return with DPMI error

        mov bx,cx                       ; BX = new size in K
        mov ah,0fh                      ; resize memory block
        call int310500xcallxms
        mov dx,si                       ; get XMS handle again
        or ax,ax                        ; error in resize?
        jnz int310501xgotmem            ; if no, go to memory block code

        push bx                         ; yup, preserve error number

        mov ah,0ch                      ; lock memory block
        call int310500xcallxms

        pop bx                          ; restore error number
        jmp int310500xerror             ; XMS error, return with DPMI error

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31050ax:                             ; XMS get memory block size and base
        mov dx,si                       ; get XMS handle from DPMI handle
        mov si,di                       ; SI = size of block in K

        mov ah,0dh                      ; unlock memory block
        call int310500xcallxms
        or ax,ax                        ; error?
        jz int310500xerror              ; if XMS error, return with DPMI error

        mov ah,0ch                      ; lock memory block
        call int310500xcallxms
        or ax,ax                        ; error?
        jz int310500xerror              ; if XMS error, return with DPMI error

        xor di,di                       ; convert size in K to size in bytes
        shrd di,si,6
        shr si,6

        mov cx,bx                       ; XMS linear address to DPMI regs
        mov bx,dx

        mov ax,cx                       ; figure out alignment stub
        dec ax
        and ax,0fh
        xor al,0fh

        sub di,ax                       ; subtract alignment stub from size
        sbb si,0

        add cx,0fh                      ; align linear address on paragraph
        adc bx,0
        and cl,0f0h

        jmp int31oksinoax               ; return ok, with SI, DI, BX, CX

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; RAW EXTENDED MEMORY FUNCTIONS
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;-----------------------------------------------------------------------------
int310500rnomem:                        ; no free extended memory present
        xor eax,eax                     ; 0 available extended memory
        jmp int310500xsetbuf            ; put memory information in buffer

;-----------------------------------------------------------------------------
int310500rbxcxtoebx:                    ; convert BX:CX bytes to EBX bytes
        pop bp                          ; pop return address

        shl ebx,16                      ; EBX = BX:CX
        mov bx,cx

        or ebx,ebx                      ; check for invalid value
        jz int31fail8021                ; if invalid value, error 8021h

        add ebx,0fh                     ; align EBX on paragraph
        and bl,0f0h

        jmp bp                          ; return ok

;-----------------------------------------------------------------------------
int310500rfindmcb:                      ; find MCB for handle SI:DI
        pop bp                          ; pop return address

        shl esi,16                      ; ESI = handle SI:DI = address of MCB
        mov si,di

        mov edi,[cs:rawextmemtop]       ; EDI -> first memory control block

.int310500rfindmcbl0:
        cmp edi,esi                     ; found MCB?
        jne short .int310500rfindmcbl0f0       ; if no, keep looking

        cmp byte [edi-4],0          ; memory block free?
        je int31fail8023                ; if yes, error 8023h

        jmp bp                          ; return ok, found MCB

.int310500rfindmcbl0f0:
        mov edi,[edi-12]                ; EDI -> next memory control block
        or edi,edi                      ; is there another MCB?
        jnz .int310500rfindmcbl0       ; if yes, loop

        jmp int31fail8023               ; fail, error 8023h

;-----------------------------------------------------------------------------
int310500radjustused:                   ; adjust INT 15h extended memory used
        mov eax,[cs:rawextmemtop]       ; EAX -> first memory control block

.int310500radjustusedl0:
        cmp dword [eax-12],0        ; last memory control block?
        jz short .int310500radjustusedf0       ; if yes, go to set new used K

        mov eax,[eax-12]                ; EAX -> next memory control block
        jmp .int310500radjustusedl0    ; loop

.int310500radjustusedf0:
        cmp byte [eax-4],0          ; memory block free?
        je short .int310500radjustusedf1       ; if no, go on

        sub eax,[eax-16]                ; used, adjust by size of block

.int310500radjustusedf1:
        sub eax,10h                     ; adjust by size of MCB
        and eax,0fffffc00h              ; align on K
        sub eax,[cs:rawextmemtop]       ; size of extended memory used
        neg eax
        shr eax,10                      ; convert from bytes to K

        mov ebp,[cs:codebase]           ; EBP = of PMODE_TEXT from 0
        mov [ds:ebp + rawextmemused],ax ; adjust INT 15h extended memory used

.int310500radjustuseddone:
        ret                             ; return

;-----------------------------------------------------------------------------
int310500rlinkmcb:                      ; link memory blocks at ESI and EDI
        mov eax,[esi-16]                ; combine two block sizes
        add eax,10h
        add [edi-16],eax                ; add size of next block to this one
        mov ecx,[esi-12]                ; copy next MCB field
        mov [edi-12],ecx
        jecxz .int310500rlinkmcbf0       ; if no next MCB, done

        mov [ecx-8],edi                 ; set prev MCB in next MCB to this MCB

.int310500rlinkmcbf0:
        ret                             ; return

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310500r:                             ; raw get free memory information
        mov edi,[cs:rawextmemtop]       ; EDI -> first memory control block

        xor eax,eax                     ; running highest free memory block
.int310500rl0:
        cmp byte [edi-4],0          ; is block free?
        jne short .int310500rl0f0      ; if no, loop

        mov ebx,[edi-16]                ; EBX = size of block
        cmp eax,ebx                     ; last free block larger?
        ja short .int310500rl0f0       ; if yes, loop

        mov eax,ebx                     ; found larger block, new largest

.int310500rl0f0:
        mov edi,[edi-12]                ; EDI -> next memory control block
        or edi,edi                      ; is there another MCB?
        jnz .int310500rl0              ; if yes, loop

        jmp int310500xsetbuf            ; put memory information in buffer

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310501r:                             ; raw allocate memory block
        call int310500rbxcxtoebx        ; convert BX:CX bytes to EBX bytes

        mov edi,[cs:rawextmemtop]       ; EDI -> first memory control block

.int310501rl0:
        cmp byte [ds:edi-4],0       ; is block free?
        je short .int310501rl0f2       ; if yes, check block

.int310501rl0f0:
        mov edi,[edi-12]                ; EDI -> next memory control block
        or edi,edi                      ; is there another MCB?
        jnz .int310501rl0              ; if yes, loop

        jmp int31fail8013               ; fail, error 8013h

.int310501rl0f2:
        lea ecx,[edi-10h]               ; ECX -> possible new MCB
        sub ecx,ebx

        mov eax,[edi-16]                ; EAX = size of block
        sub eax,ebx                     ; enough free memory in block?
        jc short .int310501rl0f0       ; if no, loop

        jz short .int310501rl0f1       ; if exactly same size, continue

        sub eax,10h                     ; adjust for size of new created MCB
        mov [ecx-16],eax                ; put size of new block in new MCB
        mov eax,ecx                     ; set next MCB in old MCB as new one
        xchg [edi-12],eax               ; copy next MCB from old to new MCB
        mov [ecx-12],eax
        or eax,eax                      ; is there a next MCB?
        jz short .int310501rl0f3       ; if no, go on

        mov [eax-8],ecx                 ; set prev MCB in next MCB to new MCB

.int310501rl0f3:
        mov [ecx-8],edi                 ; set prev MCB in new MCB as old one
        mov byte [ecx-4],0          ; set new MCB as free
        mov [edi-16],ebx                ; set size of allocated block

.int310501rl0f1:
        mov byte [edi-4],1          ; set block as allocated

int310501raddxnhandle:
        shld ebx,ecx,16                 ; BX:CX = ECX, address of block
        shld esi,edi,16                 ; SI:DI = EDI, handle (address of MCB)

        call int310500radjustused       ; adjust INT 15h extended memory used

        jmp int31oksinoax               ; return ok, with SI, DI, BX, CX

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310502r:                             ; raw free memory block
        call int310500rfindmcb          ; find MCB for handle SI:DI

        mov byte [edi-4],0          ; set this memory block as free

        mov esi,[edi-12]                ; ESI -> next memory control block
        or esi,esi                      ; is there next MCB?
        jz short .int310502rf0         ; if no, go on

        cmp byte [esi-4],0          ; is next memory block free?
        jne short .int310502rf0        ; if no, go on

        call int310500rlinkmcb          ; link two memory blocks

.int310502rf0:
        mov esi,[edi-8]                 ; ESI -> previous memory control block
        or esi,esi                      ; is there previous MCB?
        jz short .int310502rf1         ; if no, go on

        cmp byte [esi-4],0          ; is previous memory block free?
        jne short .int310502rf1        ; if no, go on

        xchg esi,edi                    ; change mcb order for function
        call int310500rlinkmcb          ; link two memory blocks

.int310502rf1:
        call int310500radjustused       ; adjust INT 15h extended memory used

        jmp int31ok                     ; return ok

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int310503r:                             ; raw resize memory block
        call int310500rbxcxtoebx        ; convert BX:CX bytes to EBX bytes
        mov edx,ebx                     ; EDX = size of new block

        call int310500rfindmcb          ; find MCB for handle SI:DI

        push ds                         ; ES = DS for possible block copy
        pop es

        sub ebx,[esi-16]                ; EBX = change in block size
        jz near .int310503rf0               ; if no change, done

        jc near .int310503rf1               ; if block made smaller, just free top

        xor eax,eax                     ; running memory counter

        mov ecx,[esi-12]                ; ECX -> next MCB
        jecxz .int310503rf4      ; if no next MCB, check previous MCB

        cmp byte [ecx-4],0          ; next MCB free?
        jne short .int310503rf4        ; if not, check previous MCB

        mov eax,[ecx-16]                ; EAX = amount of free memory in block
        add eax,10h                     ;  including memory control block

.int310503rf4:
        mov ecx,[esi-8]                 ; ECX -> previous MCB
        jecxz .int310503rf5      ; if no previous MCB, check memory

        cmp byte [ecx-4],0          ; previous MCB free?
        jne short .int310503rf5        ; if not, check memory

        add eax,[ecx-16]                ; add amount of free memory in block
        add eax,10h                     ;  including memory control block

.int310503rf5:
        cmp eax,ebx                     ; enough to resize within area
        jb near .int310503rf3               ; if no, try to allocate

        or ecx,ecx                      ; is there a previous MCB?
        jz near .int310503rf6               ; if no, resize to next

        cmp byte [ecx-4],0          ; previous MCB free?
        jne short .int310503rf6        ; if not, resize to next

        mov ebp,[ecx-16]                ; EBP = size of previous block, try to
        add ebp,10h                     ;  resize within previous block

        sub ebp,ebx                     ; EBP = prev block size - needed size
        jbe short .int310503rf7        ; if prev block too small, go on

        mov edi,ecx                     ; EDI -> new MCB for new memory block
        sub edi,ebp                     ;  resize will fit entirely

        lea eax,[ebp-10h]               ; EAX = new size of free block
        mov [ecx-16],eax                ; store mew free size in prev MCB
        mov [edi-16],edx                ; store new size in new MCB
        mov [ecx-12],edi                ; set next MCB in prev MCB to new MCB
        mov [edi-8],ecx                 ; set prev MCB in new MCB to prev MCB
        mov byte [edi-4],1          ; set new MCB as used

        mov ecx,[esi-12]                ; copy next MCB field
        mov [edi-12],ecx
        or ecx,ecx                      ; is there a next MCB
        jz near .int310503rf0               ; if no, go to block readjust

        mov [ecx-8],edi                 ; set prev MCB in next MCB to new MCB

        jmp .int310503rf0              ; go to block readjust

.int310503rf7:
        mov edi,ecx                     ; EDI -> new MCB for new memory block

        lea eax,[edx+ebp]               ; EAX = size of new block
        mov [edi-16],eax                ; store mew free size in new MCB
        mov byte [edi-4],1          ; set new MCB as used

        mov ecx,[esi-12]                ; copy next MCB field
        mov [edi-12],ecx
        jecxz .int310503rf8      ; if no next MCB, go on

        mov [ecx-8],edi                 ; set prev MCB in next MCB to new MCB

.int310503rf8:
        add ebp,ebx                     ; EBP = size of block just acquired
        sub ebx,ebp                     ; EBX = new size still needed
        jz near .int310503rf0               ; if no more space needed, done

        mov esi,edi                     ; ESI -> new MCB

.int310503rf6:
        mov edi,[esi-12]                ; EDI -> next MCB

        mov ecx,[edi-12]                ; copy next MCB field
        mov [esi-12],ecx
        jecxz .int310503rf9      ; if no next MCB, go on

        mov [ecx-8],esi                 ; set prev MCB in next MCB to this MCB

.int310503rf9:
        mov eax,[edi-16]                ; EAX = size of next memory block
        add eax,10h

        sub edi,eax                     ; EDI -> start of next memory block
        sub eax,ebx                     ; EAX = amount of free space left

        mov ecx,[esi-16]                ; ECX = old size of this memory block
        mov [esi-16],edx                ; store new size in this MCB
        mov byte [esi-4],1          ; set this MCB as used

        sub esi,ecx                     ; ESI -> start of this memory block
        sub esi,10h

        shr ecx,2                       ; copy this memory block down in mem
        db 67h
        rep movsd

        add esi,10h                     ; adjust ESI to top MCB
        mov edi,esi                     ; EDI -> top MCB

        mov ebx,eax                     ; EBX = negative of space free at top
        neg ebx
        jz near .int310503rf0               ; in no space free, go to done

        jmp short .int310503rf1        ; set new MCBs for moved block

.int310503rf3:
        mov ebp,edi                     ; preserve old block MCB address
        sub edx,ebx                     ; EDX = size of old block

        mov bx,[esp+16]                 ; BX:CX = new block size from stack
        mov cx,[esp+24]
        mov ax,501h                     ; try to allocate new block
        int 31h
        jc int31failax                  ; if could not, fail with error AX

        shrd eax,esi,16                 ; EAX -> new block MCB
        mov ax,di

        shrd edi,ebx,16                 ; EDI -> start of new block
        mov di,cx

        lea esi,[ebp-10h]               ; ESI -> start of old block
        sub esi,edx

        mov ecx,edx                     ; copy memory from old block to new
        shr ecx,2
        db 67h
        rep movsd

        mov edx,eax                     ; EDX -> new block MCB

        shld esi,ebp,16                 ; SI:DI = handle of old block
        mov di,bp
        mov ax,0502h                    ; free old block
        int 31h

        mov edi,edx                     ; EDI -> new block MCB for done

        jmp short .int310503rf0        ; go to done

.int310503rf1:
        lea edi,[esi+ebx]               ; EDI -> new MCB for new memory block

        lea eax,[ebx+10h]               ; EAX = size of freed block
        neg eax
        mov [esi-16],eax                ; store freed size in old MCB
        mov [edi-16],edx                ; store new size in new MCB

        mov ecx,[esi-12]                ; copy next MCB field
        mov [edi-12],ecx
        jecxz .int310503rf2      ; if no next MCB, go on

        mov [ecx-8],edi                 ; set prev MCB in next MCB to new MCB

.int310503rf2:
        mov [esi-12],edi                ; set next MCB in old MCB as new MCB
        mov [edi-8],esi                 ; set prev MCB in new MCB as old MCB
        mov byte [edi-4],1          ; set new MCB as used
        mov byte [esi-4],0          ; set old MCB as free

        mov ecx,[esi-8]                 ; ECX -> prev MCB of old MCB
        jecxz .int310503rf0      ; if no prev MCB, done

        cmp byte [ecx-4],0          ; is previous MCB free?
        jne short .int310503rf0        ; if no, dont link

        push edi                        ; preserve new MCB
        mov edi,ecx                     ; top MCB = prev MCB for link function
        call int310500rlinkmcb          ; link two memory blocks
        pop edi                         ; restore new MCB

.int310503rf0:
        mov ecx,[edi-16]                ; ECX = base address of new block
        neg ecx
        lea ecx,[ecx+edi-10h]
        
        jmp int310501raddxnhandle       ; return address and handle

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
int31050ar:                             ; raw get memory block size and base
        call int310500rfindmcb          ; find MCB for handle SI:DI

        mov edi,[esi-16]                ; EDI = size of memory block
        lea ecx,[esi-10h]               ; get base address of memory block
        sub ecx,edi

        shld ebx,ecx,16                 ; BX:CX = ECX, address of block
        shld esi,edi,16                 ; SI:DI = EDI, size of memory block

        jmp int31oksinoax               ; return ok, with SI, DI, BX, CX
