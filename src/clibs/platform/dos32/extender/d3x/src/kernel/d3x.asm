;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;		   D3X(tm) DOS-Extender v0.90 unleash `g' alpha
;				      core
;					
;			Copyright (c) 1998-2002 Borca Daniel
;					
;				 dborca@yahoo.com
;			 http://www.geocities.com/dborca
;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²

%include	"lang.mac"

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;	CONFIG
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;%define	EXC0f			;exception 15
;%define	PAGED			;enable paging
%define	SYMLINK				;support symbolic links (DJGPP)
EXTRALINEAR	equ	4*1024*1024	;used for physical to linear mapping

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;	DEFINES, EQUATES, MACROS & STRUCTURES
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
%ifdef	I21API
%define	extAPI
%endif
%ifdef	I33API
%define	extAPI
%endif

%ifdef	SYMLINK
%define	SYMLINK		(TARGET==1)
%else
%define	SYMLINK		0
%endif

%ifdef	PAGED
%define	VCPIreq		call myVCPIr
%else
%define	VCPIreq		int 67h
%endif

Locked_frame	equ	200h
Stack_16_sz	equ	200h
Stack_Lk_sz	equ	1000h
Stack_Sp_sz	equ	100h		;Must fit in Stack_16_sz and Locked_frame
DefMasterPIC	equ	08h
DefSlavePIC	equ	70h
%define	Num_IRQ		(8*2)
%define	Num_IRQ2	(Num_IRQ+2)	;Fake: 1ch, 23h
%define	Num_Exc		17
%define	Num_RMCB	16
MaxAvSel	equ	20h		;Max available LDT selectors
MaxXHndl	equ	80h		;Max extended memory blocks

DPMI_sels	equ	2
%define	DPMI_sel1	Flat_D_desc

struc		Addr16b
    Ofs16b	resw	1
    Seg16b	resw	1
endstruc

struc		Addr32b
    Ofs32b	resd	1
    Sel16b	resw	1
endstruc

mask_P		equ	80h
mask_DPL	equ	60h
mask_S		equ	10h
mask_G		equ	80h
mask_NUL	equ	20h
mask_L2		equ	0fh

PT_P		equ	01h		;present (else not)
PT_W		equ	02h		;writable (else read-only)
PT_U		equ	04h		;user mode (else kernel mode)
PT_CD		equ	10h		;page caching disabled (else enabled)
PT_A		equ	20h		;accessed (else not)
PT_D		equ	40h		;dirty (else clean)

PT_E		equ	4		;hi-byte: end block marker

struc		gate
    gateO1	resw	1
    gateSel	resw	1
        resb	1
    gateTyp	resb	1
    gateO2	resw	1
endstruc

struc		descr
    L1	resw	1
    B1	resw	1
    B2	resb	1
    access	resb	1
    granted	resb	1
    B3	resb	1
endstruc

struc		TSS
    tssBack	resw	2
    tssEsp0	resd	1
    tssSs0	resw	2
    tssEsp1	resd	1
    tssSs1	resw	2
    tssEsp2	resd	1
    tssSs2	resw	2
    tssCr3	resd	1
    tssEip	resd	1
    tssEfl	resd	1
    tssEax	resd	1
    tssEcx	resd	1
    tssEdx	resd	1
    tssEbx	resd	1
    tssEsp	resd	1
    tssEbp	resd	1
    tssEsi	resd	1
    tssEdi	resd	1
    tssEs	resw	2
    tssCs	resw	2
    tssSs	resw	2
    tssDs	resw	2
    tssFs	resw	2
    tssGs	resw	2
    tssLDT	resw	2
    tssT	resw	1
    tssIO	resw	1
        resd	64
    tssStk:
endstruc

struc		DPMI_R
    dpmiEdi:
    dpmiDi	resw	1
    dpmiDiH	resw	1
    dpmiEsi:
    dpmiSi	resw	1
    dpmiSiH	resw	1
    dpmiEbp:
    dpmiBp	resw	1
    dpmiBpH	resw	1
    dpmiRes	resd	1
    dpmiEbx:
    dpmiBx:
    dpmiBl	resb	1
    dpmiBh	resb	1
    dpmiBxH	resw	1
    dpmiEdx:
    dpmiDx:
    dpmiDl	resb	1
    dpmiDh	resb	1
    dpmiDxH	resw	1
    dpmiEcx:
    dpmiCx:
    dpmiCl	resb	1
    dpmiCh	resb	1
    dpmiCxH	resw	1
    dpmiEax:
    dpmiAx:
    dpmiAl	resb	1
    dpmiAh	resb	1
    dpmiAxH	resw	1
    dpmiFl	resw	1
    dpmiEs	resw	1
    dpmiDs	resw	1
    dpmiFs	resw	1
    dpmiGs	resw	1
    dpmiIp	resw	1
    dpmiCs	resw	1
    dpmiSp	resw	1
    dpmiSs	resw	1
endstruc

struc		VCPI_SW
    vcpiCr3	resd	1
    vcpiGdt	resd	1
    vcpiIdt	resd	1
    vcpiLdt	resw	1
    vcpiT	resw	1
    vcpiEip	resd	1
    vcpiCs	resw	1
endstruc

struc		excregs
    eCs	resw	1
    eEip	resd	1
    eErr	resw	1
    eEfl	resd	1
    eEax	resd	1
    eEbx	resd	1
    eEcx	resd	1
    eEdx	resd	1
    eEsi	resd	1
    eEdi	resd	1
    eEbp	resd	1
    eDs	resw	1
    eEs	resw	1
    eFs	resw	1
    eGs	resw	1
    eSs	resw	1
    eEsp	resd	1
    eCR2	resd	1
endstruc

struc		rmcb
    cbType	resb	1		;0=unused, 1=internal, 2=DPMI defined
    cbCode	resb	Addr32b_size
    cbRegs	resb	Addr32b_size
endstruc

struc		x_hndl
    x_flag	resb	1
    x_addr	resd	1
    x_size	resd	1
endstruc

%macro		clTLB	1
        mov	%1,cr3
        mov	cr3,%1
%endmacro

%macro		exitErr	2
        mov	si,%1
        mov	al,%2
        jmps	stub_error
%endmacro

%macro		MakeInt	1
Interrupt%1:
        call	Common_int_handler
%endmacro

%macro		MkXInt	1
xInterrupt%1:
        call	xInterrupt_ID
%endmacro

%macro		MkRMCB	1
RMCB%1:
        call	RMCB_ID
%endmacro

%macro		ExcOfs	1
        dw	E%1
%endmacro

%macro		MkRefl	1
        istruc	Addr32b
        at	Ofs32b,	dd Interrupt%1
        at	Sel16b,	dw Real_C_desc - GDT
        iend
%endmacro

%if	TARGET==0
%define	stubtype	'1'

struc		d3x1_hdr
    sign	resd	1		;signature 'D3X1'
    hdrsize	resd	1		;header size (bytes)
    binsize	resd	1		;binary size (bytes)
    admsize	resd	1		;additional memory (bytes)
    entry	resd	1		;entry point
    tos	resd	1		;top of stack
endstruc
%elif	TARGET==1
%define	stubtype	'd'

s_vaddr		equ	12
s_size		equ	16
s_scnptr	equ	20
%elif	TARGET==2
%define	stubtype	'w'

struc		le_hdr
LESignature	resd	1	; must be 0x454C since order is little endian
LEFormatLevel	resd	1	; not really needed
LECpuType	resw	1
LETargetSystem	resw	1	; should be 01 (OS/2)
LEModuleVersion	resd	1	; who cares...
LEModuleType	resd	1	; Must be flat
LENumberPages	resd	1
LEEntrySection	resd	1
LEEntryOffset	resd	1
LEStackSection	resd	1
LEInitialESP	resd	1
LEPageSize	resd	1
LEBytesLastPage	resd	1
LEFixupSize	resd	1
LEFixupChecks	resd	1
LELoaderSize	resd	1
LELoaderChecks	resd	1
LEObjectTable	resd	1
LEObjectEntries	resd	1
LEObjectPMTable	resd	1
LEIterateData	resd	1
LEResourceTable	resd	1
LEResources	resd	1
LEResNamesTable	resd	1
LEEntryTable	resd	1
LEModuleDirTab	resd	1
LEModuleDirs	resd	1
LEFixupPageTab	resd	1
LEFixupRecords	resd	1
LEWeDontNeed	resd	3		; getting bored
LEPPchecksumTab	resd	1
LEDataPages	resd	1
LEPreloadPages	resd	1
LENonResTable	resd	1
LEWeDontNeed2	resd	2
LEAutoData	resd	1
LEDebugStart	resd	1
LEDebugSize	resd	1
LEPrelInstPages	resd	1
LEDemdInstPages	resd	1
LEExtraHeap	resd	1
LEReserved	resd	1
endstruc

struc		obj_tbl
OTVirtualSize	resd	1
OTRelocBase	resd	1
OTObjectFlags	resd	1
OTPageMapIndex	resd	1
OTPageMapSize	resd	1
OTReserved	resd	1
endstruc

struc		opm_tbl
OPMTUnknown	resb	1
OPMTFixupTabIdx	resw	1	; big endian!
OPMTType	resb	1	; 0 = no reloc 3 = reloc
endstruc

struc		fix_rec
FRType		resw	1
FROffset	resw	1
FRObject	resb	1
FRItem		resw	1
endstruc
%endif

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;	CODE & DATA 16bit
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
segment		_REAL	private use16 class=CODE

        align	4
stubinfo:
stubinfo_magic:					; char [16]
%if	TARGET==0
        db	"d3x1stub, v 1.00"	; version may change, [0..7] won't
%elif	TARGET==1
        db	"go32stub, v 2.02"	; version may change, [0..7] won't
%elif	TARGET==2
        db	"o2LEstub, v 0.90"	; version may change, [0..7] won't
%endif
stubinfo_size:					; unsigned long
        dd	stubinfo_end-stubinfo	; bytes in structure
stubinfo_minstack:				; unsigned long
        dd	80000h			; minimum amount of DPMI stack space (512K)
stubinfo_memory_handle:				; unsigned long
        dd	0			; DPMI memory handle
stubinfo_initial_size:				; unsigned long
        dd	0			; size of initial segment
stubinfo_minkeep:				; unsigned short
        dw	16384			; amount of automatic real-mode buffer
stubinfo_ds_selector:				; unsigned short
        dw	0			; our DS selector (used for transfer buffer)
stubinfo_ds_segment:				; unsigned short
        dw	0			; our DS segment (used for simulated calls)
stubinfo_psp_selector:				; unsigned short
        dw	0			; PSP selector
stubinfo_cs_selector:				; unsigned short
        dw	0			; to be freed
stubinfo_env_size:				; unsigned short
        dw	0			; number of bytes of environment
%if	TARGET==0
stubinfo_platform:				; platform
        dd	0			; any system bits are passed here
%elif	TARGET==1
stubinfo_basename:				; char [8]
        resb	8			; base name of executable to load (asciiz if < 8)
stubinfo_argv0:					; char [16]
        resb	16			; used ONLY by the application (asciiz if < 16)
stubinfo_dpmi_server:				; char [16]
        resb	16			; used by stub to load DPMI server if no DPMI
                        ; already present. Not used by us
%endif
        align	4
stubinfo_end:

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	EntryPoint
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
..start:
        cld
        push	cs
        pop	ds
    ;Print via STDERR
        mov	ah,40h
        mov	bx,2
        mov	cx,CopyRightL
        mov	dx,CopyRight
        int	21h
    ;Clear BSS area
        push	es
        push	ds
        pop	es
        mov	di,lastbyte
        mov	cx,lastbss-lastbyte
        xor	al,al
        rep	stosb
        pop	es
        jmp	extender

exit:
        push	ax
        mov	ax,cs
        cmp	ax,0
REAL		equ	$-2
        jne	_exit
    ;!@# Hack alert {
    ;	If `exit' called from PM:
    ;		if we!=DPMI, we'll never get here and don't care
    ;		if we==DPMI, we got here by `main_switch' and DS==CS
    ;	If `exit' called from RM:
    ;		DS must equal CS
        call	cleanup
        cmp	byte [terminate],-1
        jne	_exit
        mov	ax,3
        int	10h
        mov	si,error_exit
        call	print_f
    ;!@# Hack alert }
_exit:
        pop	ax
        mov	ah,4ch
        int	21h

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Error exits
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
_need_386:
        exitErr	_386,-1
_need_dos3:
        exitErr	_dos,-2
_need_mem:
        exitErr	_mem,-3
_need_dpmi:
        exitErr	_dpmi,-4
_need_vcpi:
        exitErr	_vcpi,-5
_need_interf:
        exitErr	_interf,-6
_need_a20:
        exitErr	_a20,-7
_need_img:
        exitErr	_img,-8
_need_mem2:
        exitErr	_mem2,-9

stub_error:
        push	ax
        push	si
        mov	si,_error
        call	msg_stub_error
        pop	si
        call	msg_stub_error
        mov	dl,13
        int	21h
        mov	dl,10
        int	21h
        pop	ax
        jmps	exit

proc	near,	msg_stub_error
        mov	ah,02h
    beg_stub_error:
        cs	lodsb
        or	al,al
        jz	end_stub_error
        mov	dl,al
        int	21h
        jmps	beg_stub_error
    end_stub_error:
        ret
endp

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Initialization
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
extender:
    ;Check CPU and DOS ver
        call	detect_cpu
        cmp	dx,byte 3
        jb	_need_386
        mov	byte [cpu_id],dl
        mov	ah,30h
        int	21h
        cmp	al,3
        jb	_need_dos3
        mov	word [REAL],cs		;allow cleanup at exit
        mov	word [stubinfo_ds_segment],_STACK+(Stack_16_sz/16)
        mov	word [PSP_Segment],es

    ;Build GDT
        mov	ecx,cs
        shl	ecx,4
        lea	edx,[GDT]
        add	edx,ecx
        mov	dword [_GDTR+2],edx
        lea	edx,[LDT]
        add	edx,ecx
        callp	fixDescrBase, word LDT_desc, edx
        lea	edx,[tss1]
        add	edx,ecx
        callp	fixDescrBase, word TSS1_desc, edx
        lea	edx,[tss2]
        add	edx,ecx
        callp	fixDescrBase, word TSS2_desc, edx
        lea	edx,[tss8]
        add	edx,ecx
        callp	fixDescrBase, word TSS8_desc, edx

        callp	fixDescrBase2, word Locked_desc, word _LOCKED
        callp	fixDescrBase2, word PSP_desc, es
        callp	fixDescrBase2, word Env_desc, word [es:2ch]
        callp	fixDescrBase2, word Real_C_desc, cs
        callp	fixDescrBase2, word Real_D_desc, cs

    ;Build IDT
        lea	edx,[IDT]
        add	edx,ecx
        mov	dword [_IDTR+2],edx

        mov	cx,256
        mov	si,IDT
        mov	ax,Interrupt0
    MakeIDT:
        mov	word [byte si+gateSel],Real_C_desc - GDT
        mov	byte [si+gateTyp],8eh
        mov	word [si+gateO1],ax
        add	ax,Interrupt1 - Interrupt0
        add	si,byte gate_size
        loop	MakeIDT

        mov	cx,Num_Exc
        mov	si,IDT
        mov	ax,xInterrupt0
    MakeXIDT:
        mov	word [si+gateO1],ax
        add	ax,xInterrupt1 - xInterrupt0
        add	si,byte gate_size
        loop	MakeXIDT

        mov	word [IDT+8*gate_size+gateSel],TSS8_desc - GDT
        mov	word [IDT+8*gate_size+gateTyp],85h

    ;Build TSSs
        mov	cx,3
        mov	si,tss1
    common_tss:
        mov	word [si+tssEfl],3002h
        mov	word [byte si+tssEs],PSP_desc - GDT
        mov	word [byte si+tssCs],Real_C_desc - GDT
        mov	word [byte si+tssSs],Real_D_desc - GDT
        mov	word [byte si+tssDs],Real_D_desc - GDT
        mov	word [byte si+tssFs],Flat_D_desc - GDT
        mov	word [byte si+tssGs],Flat_C_desc - GDT
        mov	word [byte si+tssLDT],LDT_desc - GDT
        mov	word [si+tssIO],TSS_size
        add	si,word TSS_size
        loop	common_tss

        mov	dword [tss2+tssEip],exec_bin
        mov	dword [tss2+tssEsp],tss2+tssStk

        mov	dword [tss8+tssEip],Double_fault
        mov	dword [tss8+tssEsp],tss8+tssStk
        mov	dword [tss8+tssEbp],tss8+tssStk

    ;We don't change rings through gates, therefore this is superfluous
    ;	mov	word [tss1+tssSs0],Real_D_desc - GDT
    ;	mov	dword [tss1+tssEsp0],tss1+tssStk
    ;	mov	word [tss2+tssSs0],Real_D_desc - GDT
    ;	mov	dword [tss2+tssEsp0],tss2+tssStk
    ;	mov	word [tss8+tssSs0],Real_D_desc - GDT
    ;	mov	dword [tss8+tssEsp0],tss8+tssStk

    ;Test DPMI presence
        mov	ax,1687h
        int	2fh
        test	ax,ax
        jz	DPMInterface
    ;Test VCPI presence
        call	detect_VCPI
        or	ah,ah
        jz	near VCPInterface
    ;Test unconventional protected mode
        smsw	ax
        test	al,1
        jnz	near _need_interf
    ;Verify XMS presence
        mov	ax,4300h
        int	2fh
        cmp	al,80h
        jne	setup_pm
        mov	byte [platform],1
        mov	ax,4310h
        int	2fh
        mov	word [XMS_Driver+Ofs16b],bx
        mov	word [XMS_Driver+Seg16b],es

    ;Setup protected mode
    setup_pm:
        call	startup
%ifndef	PAGED
    ;Load the D3X application
        call	load_exe
%else
        jmp	build_pagetables
%endif
    enter_pm:
    ;Enter UNreal (protected)
        call	main_switch

    shut_down:
        pop	bp,bp
        mov	al,byte [tss2+tssEax]
        jmp	exit

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	DPMInterface
;		In:	ES:DI	= protected mode entry point
;			SI	= number of paragraphs to allocate for DPMI
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
DPMInterface:
        mov	byte [platform],3
    ;Save DPMI protected mode entry point
        mov	word [DPMI_Entry+Ofs16b],di
        mov	word [DPMI_Entry+Seg16b],es
    ;Resize memory to fit DPMI needs
        mov	es,word [stubinfo_ds_segment]
        add	word [stubinfo_ds_segment],si
    ;Load the D3X application
        call	load_exe
    ;Call real to protected mode entry point for 32bit
        mov	ax,1
        call	far [DPMI_Entry]
        jump	c,_need_dpmi
        push	es
    ;Set the protection ring on my descriptors
        push	ds
        pop	es
        mov	bx,cs
        lar	ax,bx
        and	ah,mask_DPL
        mov	si,DPMI_sel1
        mov	cx,DPMI_sels
    .setDPL:
        or	byte [si+access],ah
        add	si,byte descr_size
        loop	.setDPL
    ;Allocate selectors
        xor	ax,ax
        mov	cx,DPMI_sels
        int	31h
        go	c,_need_dpmi
        mov	bx,ax
    ;Build selectors
        lea	edi,[DPMI_sel1]
    .set_Descr:
        push	bx
        mov	ax,000ch
        int	31h
        go	c,_need_dpmi
        mov	ax,0003h
        int	31h
        add	bx,ax
        add	edi,byte descr_size
        loop	.set_Descr
    ;Save allocated selectors
        pop	gs		;Flat code
        pop	fs		;Flat data
        pop	es		;PSP selector
        jmp	exec_bin2

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	VCPInterface
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
VCPInterface:
        mov	byte [platform],2

    ;Read PIC mappings from VCPI server
        mov	ax,0de0ah
        int	67h
        mov	word [MasterPIC],bx
        mov	word [SlavePIC],cx

    build_pagetables:
    ; determine here # of max pagetables + pagedir
        mov	ax,0de03h			; # of free pages
                VCPIreq
    ; align to next pagetable + first MB + ExtraLinear
        add	edx,1023+1024+(((EXTRALINEAR+4096*1024-1) & ~(4096*1024-1))/(1024*4))
        and	dx,~1023
        shl	edx,2				; # of bytes for page tables
        mov	dword [pt_delta],edx		; store size of pagetables
        shr	edx,12-8			; calc # of paragraphs
        mov	bx,dx				; if highword is not zero, you would
                            ; still have enough mem!
        add	bx,(4096+4096) / 16		; page directory and page align

        movzx	eax,word [stubinfo_ds_segment]
        add	word [stubinfo_ds_segment],bx
    ;Load the D3X application
        call	load_exe

        shl	eax,4
        add	eax,4095
        and	ax,~4095			; align data area on page
        mov	dword [pagedir],eax

        add	eax,1000h			; skip pagedir
        mov	dword [pt_base],eax		; set base of page table area
        add	eax,dword [pt_delta]		; size of pagetables
        mov	dword [pt_top],eax		; set top of page table area

        mov	eax,dword [pagedir]
        shr	eax,4
        mov	gs,ax				; GS = segment of page directory
        mov	es,ax				; ES = segment of page directory
        add	ax,100h
        mov	fs,ax				; FS = segment of first page table

        xor	ax,ax
        mov	di,ax
        mov	cx,4096
        cld
        rep	stosw				; Clear pagedir & first pagetable

        push	fs
        pop	es				; ES = segment of first pagetable
        xor	di,di
        mov	si,VCPI1_desc			; descriptors for VCPI host
        mov	ax,0de01h
                VCPIreq
        test	ah,ah
        jnz	near _need_vcpi

        movzx	eax,di				; set base of usable page table area
        add	ax,4095
        and	ax,~4095			; align to next pagetable
        add	eax,dword [pt_base]
        mov	dword [pt_free],eax

        mov	dword [VCPI_Entry+Ofs32b],ebx

    ; For safety clear reserved bits 9-11
    ClearResBits:
        and	byte [es:di+1],0f1h
        sub	di,byte 4
        jnc	ClearResBits

    ; Link pagetables in pagedirectory
        mov	si,es				; DX = current page table segment
        xor	ebx,ebx				; index in page dir, also loop counter
        mov	ebp,dword [pt_delta]
        shr	ebp,12				; # of pagetables
        jmps	LinkPage

    ClearPage:
        xor	di,di				; clear page table
        mov	cx,800h
        xor	ax,ax
        rep	stosw
    LinkPage:
        mov	cx,si
        shr	cx,8				; get page number
        mov	ax,0de06h			; get physical address
                VCPIreq
        and	dh,0f1h				; For safety clear reserved bits 9-11
        or	dl,PT_U | PT_W | PT_P		; set present, User and R/W bit
        mov	dword [gs:ebx*4],edx		; store in page directory
        add	si,100h				; increment page table segment
        mov	es,si
        inc	bx				; increment index in page directory
        cmp	bx,bp				; at end of page tables?
        jb	ClearPage			; if no, loop

        mov	ecx,dword [pagedir]
        shr	ecx,12
        mov	ax,0de06h
        VCPIreq					; get physical address

        mov	dword [tss1+tssCr3],edx
        mov	dword [tss2+tssCr3],edx
        mov	dword [tss8+tssCr3],edx

    ;Build VCPI switch structure
        mov	dword [VCPI+vcpiCr3],edx
        mov	ecx,cs
        shl	ecx,4
        lea	edx,[_GDTR]
        add	edx,ecx
        mov	dword [VCPI+vcpiGdt],edx
        lea	edx,[_IDTR]
        add	edx,ecx
        mov	dword [VCPI+vcpiIdt],edx

        lea	edx,[VCPI]
        add	edx,ecx
        mov	dword [_VCPI_],edx

        jmp	enter_pm

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Detect VCPI presence
;		Out:	AH	= 0 if VCPI present, 84h if not
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	detect_VCPI
        mov	ax,3567h
        int	21h
        mov	ax,es
        or	ax,bx
        jz	VCPI_bad
        mov	bx,1
        mov	ah,43h
        int	67h
        or	ah,ah
        jne	.test_vcpi
        mov	word [VCPI_tHandle],dx
    .test_vcpi:
        mov	ax,0de00h
        int	67h
        or	ah,ah
        jz	VCPI_end
    VCPI_done:
        mov	dx,word [VCPI_tHandle]
        cmp	dx,byte -1
        je	VCPI_bad
        mov	ah,45h				; release EMS test handle
        int	67h
        mov	word [VCPI_tHandle],-1
    VCPI_bad:
        mov	ah,84h
    VCPI_end:
        ret
endp

%ifdef	PAGED
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Pseudo VCPI server (our paging)
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
proc	near,	myVCPIr
        cmp	byte [platform],2
        jne	pseudo_vcpi
        int	67h
        ret
    pseudo_vcpi:
        cmp	al,01
        je	vcpi_getinterface
        cmp	al,03
        je	vcpi_getfreepages
        cmp	al,06
        je	vcpi_getphysaddr
        mov	ah,8fh
        ret
    vcpi_getinterface:
        push	eax,ecx,si
        push	di,es
        push	ds
        pop	es
        mov	di,si
        mov	si,Real_C_desc
        movsd
        movsd
        mov	es,word [page_array]
        xor	di,di
        mov	ecx,dword [max_pages]
        add	ecx,byte 31
        shr	ecx,5
        xor	eax,eax
        rep	stosd
        shl	dword [page_array],4
        pop	es,di
        mov	ch,1
        mov	al,PT_D | PT_A | PT_U | PT_W | PT_P
    .map_1M:
        stosd
        add	eax,1000h
        loop	.map_1M
        lea	ebx,[myVCPIp]
        pop	si,ecx,eax
        xor	ah,ah
        ret
    vcpi_getfreepages:
        mov	edx,dword [raw_top]
        sub	edx,dword [raw_base]
        shr	edx,12
        xor	ah,ah
        ret
    vcpi_getphysaddr:
        movzx	edx,cx
        shl	edx,12
        xor	ah,ah
        ret
endp

proc	far,	myVCPIp
        cmp	al,04
        je	vcpi_alloc_physical
        cmp	al,05
        je	vcpi_free_physical
        mov	ah,8fh
        retfd
    vcpi_alloc_physical:
        push	ecx,esi,fs
        mov	ax,Flat_D_desc - GDT
        mov	fs,ax
        mov	esi,dword [page_array]
        mov	ecx,dword [plast]
        mov	ah,88h
    .loop:
        cmp	ecx,dword [max_pages]
        jnb	.ret_a
        call	bt_array
        jnz	.next
        mov	dword [plast],ecx
        call	bs_array
        mov	edx,ecx
        shl	edx,12
        add	edx,dword [raw_base]
        xor	ah,ah
    .ret_a:
        pop	fs,esi,ecx
        retfd
    .next:
        inc	ecx
        jmps	.loop
    vcpi_free_physical:
        push	ecx,fs
        mov	cx,Flat_D_desc - GDT
        mov	fs,cx
        mov	esi,dword [page_array]
        mov	ecx,edx
        sub	ecx,dword [raw_base]
        shr	ecx,12
        call	bt_array
        mov	ah,8ah
        jz	.ret_f
        call	br_array
        xor	ah,ah
        cmp	dword [plast],ecx
        jb	.ret_f
        mov	dword [plast],ecx
    .ret_f:
        pop	fs,ecx
        retfd

proc	near,	bt_array
        push	ecx
        push	cx
        mov	al,80h
        and	cl,7
        ror	al,cl
        pop	cx
        shr	ecx,3
        test	byte [fs:esi+ecx],al
        pop	ecx
        ret
endp

proc	near,	bs_array
        push	ecx
        push	cx
        mov	al,80h
        and	cl,7
        ror	al,cl
        pop	cx
        shr	ecx,3
        or	byte [fs:esi+ecx],al
        pop	ecx
        ret
endp

proc	near,	br_array
        push	ecx
        push	cx
        mov	al,7fh
        and	cl,7
        ror	al,cl
        pop	cx
        shr	ecx,3
        and	byte [fs:esi+ecx],al
        pop	ecx
        ret
endp

endp
%endif

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Non-DPMI Initialization
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Initialize descriptors
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	fixDescrBas
        mov	word [bx+B1],ax
        shr	eax,16
        mov	byte [bx+B2],al
        mov	byte [bx+B3],ah
        ret
endp

proc	Pascal,near, fixDescrBase
    arg	%$address,4
    arg	%$descriptor,2
        mov	bx,word [bp+%$descriptor]
        mov	eax,dword [bp+%$address]
        call	fixDescrBas
        ret
endp

proc	Pascal,near, fixDescrBase2
    arg	%$realseg,2
    arg	%$descriptor,2
        mov	bx,word [bp+%$descriptor]
        movzx	eax,word [bp+%$realseg]
        shl	eax,4
        call	fixDescrBas
        ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Startup & cleanup
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	startup
        cli

        call	Test_A20
        je	.s_0
        mov	bl,1
        call	Switch_A20
        test	al,al
        jz	near _need_a20
        mov	byte [A20],al
    .s_0:

%ifdef	PAGED
        cmp	byte [platform],1
        je	x_startup
%endif
        cmp	byte [platform],0
        jne	endstartup
    r_startup:
        call	get_extmem_raw
        jae	endstartup
        push	ax
        mov	ax,3515h
        int	21h
        mov	word [old15+Ofs16b],bx
        mov	word [old15+Seg16b],es
        mov	ax,2515h
        mov	dx,new15
        int	21h
        pop	ax
%ifndef	PAGED
        mov	bx,X_HDescr
        mov	cx,MaxXHndl
    .s_1:
        mov	byte [bx+x_flag],4
        add	bx,byte x_hndl_size
        loop	.s_1
        mov	edx,dword [raw_top]
        sub	edx,eax
        mov	byte [X_HDescr+x_flag],1
        mov	dword [X_HDescr+x_addr],eax
        mov	dword [X_HDescr+x_size],edx
    endstartup:
        ret
%else
    xr_startup:
        add	eax,4095
        and	ax,~4095
        mov	dword [raw_base],eax
        call	vcpi_getfreepages
        mov	dword [max_pages],edx
        add	edx,byte 127
        shr	edx,7		;bits to paragraphs
        mov	ax,word [stubinfo_ds_segment]
        add	word [stubinfo_ds_segment],dx
        mov	word [page_array],ax
    endstartup:
        ret
    x_startup:
        xor	eax,eax
        mov	ah,88h
        call	far [XMS_Driver]
        cmp	bl,80h
        jne	.xmsok1
        mov	ah,08h
        call	far [XMS_Driver]
    .xmsok1:
        mov	esi,eax
        mov	edx,eax
        shr	eax,16
        mov	ah,09h
        jz	.xmsok2
        mov	ah,89h
    .xmsok2:
        call	far [XMS_Driver]
        test	ax,ax
        jz	endstartup
        mov	word [xms_handle],dx
        mov	ah,0ch
        call	far [XMS_Driver]
        push	dx,bx
        pop	eax
        shl	esi,10
        add	esi,eax
        mov	dword [raw_base],eax
        mov	dword [raw_top],esi
        jmps	xr_startup
%endif
endp

proc	near,	cleanup
        mov	dx,020h
        call	clean_pic
        mov	dx,0a0h
        call	clean_pic

        call	reset_hwints

        mov	si,X_H
        mov	cx,MaxXHndl
    .c_0:
        lodsd
        test	eax,eax
        jz	.c_1
        push	cx,si
        xchg	esi,eax
        call	clean_free
        pop	si,cx
    .c_1:
        loop	.c_0

        call	VCPI_done

%ifdef	PAGED
        cmp	word [xms_handle],byte 0
        je	.c_2
        mov	dx,word [xms_handle]
        mov	ah,0dh
        call	far [XMS_Driver]
        mov	ah,0ah
        call	far [XMS_Driver]
    .c_2:
%endif

        push	ds
        lds	dx,[old15]
        mov	ax,ds
        or	ax,dx
        jz	.c_3
        mov	ax,2515h
        int	21h
    .c_3:
        pop	ds

        xor	bl,bl
        cmp	byte [A20],bl
        je	.c_4
        call	Switch_A20
    .c_4:
        ret
endp

proc	near,	get_extmem_raw
        xor	eax,eax
        mov	ah,88h
        int	15h
        test	ax,ax
        jz	.no_raw_mem
        shl	eax,10
        add	eax,100000h
        mov	dword [raw_top],eax
        mov	ax,0e801h
        int	15h
        jc	.vdisk_1
        cmp	ax,3c00h
        jne     .vdisk_1
        inc	bh
        shl	ebx,16
        mov	dword [raw_top],ebx
    .vdisk_1:
        xor	cx,cx
        mov	es,cx
        les	bx,[es:4*19h]
        mov	eax,100000h
        cmp	dword [es:bx+12h],'VDIS'
        jne	.vdisk_2
        mov	eax,dword [es:bx+2ch]
        add	eax,3ffh		;round to kb
        and	eax,0fffc00h		;address is only 24bit
    .vdisk_2:
        dec	cx
        mov	es,cx
        cmp	dword [es:13h],'VDIS'
        jne	.set_base
        movzx	ebx,word [es:2eh]
        shl	ebx,10
        cmp	eax,ebx
        ja	.set_base
        mov	eax,ebx
    .set_base:
        mov	dword [raw_base],eax
        cmp	eax,dword [raw_top]
    .no_raw_mem:
        ret
endp

proc	near,	clean_pic
        mov	al,0bh
        out	dx,al
        jmps	$+2
        in	al,dx
        test	al,al
        jz	pic_clean
        mov	al,20h
        out	dx,al
    pic_clean:
        ret
endp

proc	near,	reset_hwints
        xor	ax,ax
        mov	es,ax
    ;This is an `inverse function' for _irqno
        mov	cx,Num_IRQ+2
    hw_loop:
        dec	cx
        jl	out_hw_loop
        mov	bx,cx
        cmp	cx,byte Num_IRQ+1
        je	_23h
        cmp	cx,byte Num_IRQ
        je	_1ch
        cmp	cx,byte Num_IRQ/2
        jae	_2ndpic
    _1stpic:
        add	bx,word [MasterPIC]
        jmps	reset_hw
    _2ndpic:
        add	bx,word [SlavePIC]
        sub	bx,byte Num_IRQ/2
        jmps	reset_hw
    _1ch:
        mov	bl,1ch
        jmps	reset_hw
    _23h:
        mov	bl,23h
        jmps	reset_hw
    reset_hw:
    ;Restore hardware ints the user forgot hooked (timer, keyb ...)
        mov	si,cx
        shl	si,2
        mov	eax,dword [hwint_rmcb+si]
        test	eax,eax
        jz	hw_loop
        shl	bx,2
        mov	dword [es:bx],eax
        jmps	hw_loop
    out_hw_loop:
        ret
endp

proc	near,	clean_free
        cmp	byte [platform],2
        je	vc_free
%ifndef	PAGED
        cmp	byte [platform],1
        je	xc_free
    ;In raw mode, freeing is done in block, by revectoring int 15h
        ret
    xc_free:
        call	_free_xmcb
%endif
        ret
    vc_free:
        xor	ecx,ecx
        push	esi
        mov	eax,esi
        shr	eax,4
        mov	es,ax
        and	si,byte 0fh
        xchg	ecx,dword [es:si]
        pop	esi
        add	esi,byte 4
        mov	edx,ecx
        and	dx,0f000h
        mov	ax,0de05h
        int	67h
        test	ch,PT_E
        jz	vc_free
        ret
endp

new15:
        cmp	ah,88h
        jne	out15
        xor	ax,ax
        iret
out15:
        db	0eah
old15		resb	Addr16b_size

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	A20 gate
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	Switch_A20
        cmp	byte [platform],1
        je	.X_A20Sw
        cmp	byte [platform],0
        je	.R_A20Sw
    .V_A20Sw:
        mov	al,bl
        ret
    .X_A20Sw:
        mov	ah,bl
        xor	ah,1
        add	ah,5
        call	far [XMS_Driver]
        ret
    .R_A20Sw:
        mov	cl,bl
        shl	bl,1
    ;Try port 92h
        in	al,92h
        and	al,~2
        or	al,bl
        jmps	$+2
        out	92h,al
    ;Now check
        call	Test_A20
        cmp	al,cl
        je	a20_ok
    ;Try 8042
        call	wait_1to0
        mov	al,0d1h
        out	64h,al
        call	wait_1to0
        mov	al,0dfh & ~2
        or	al,bl
        out	60h,al
        call	wait_1to0
        mov	al,0ffh
        out	64h,al
        call	wait_1to0
        call	Test_A20
    a20_ok:
        ret
endp

proc	near,	wait_1to0
        xor	cx,cx
    loop_1to0:
        jmps	$+2
        in	al,64h			; port 64h, keyboard status
        and	al,2
        loopnz	loop_1to0
        ret
endp

proc	near,	Test_A20
        push	ds
        xor	ax,ax
        mov	ds,ax
        dec	ax
        mov	es,ax
        mov	al,byte [00h]
        mov	ah,al
        not	al
        xchg	al,byte [es:10h]
        xchg	bl,byte [00h]
        cmp	ah,bl
        xchg	bl,byte [00h]
        mov	byte [es:10h],al
        sete	al
        pop	ds
        ret
endp

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Main switch routine
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
proc	near,	main_switch
        push	bp
    init_application:
        cli
        mov	bx,-1
        and	byte [TSS1_desc+access],0fdh

        mov	word [real_stack+Ofs16b],sp
        mov	word [real_stack+Seg16b],ss
        movzx	esp,sp

        cmp	byte [platform],2
        jb	RMtoPM_raw
        mov	esi,dword [_VCPI_]
        mov	ax,0de0ch
        int	67h

    RMtoPM_raw:
        lgdt	[_GDTR]
        lidt	[_IDTR]

%ifndef	PAGED
        mov	eax,cr0
        and	eax,7ffffff6h
        or	al,1
%else
        mov	eax,dword [tss1+tssCr3]
        mov	cr3,eax
        mov	eax,cr0
        and	al,0f6h
        or	eax,80000001h
%endif
        mov	cr0,eax
        db	0eah
        dw	$+4
        dw	Real_C_desc - GDT

        mov	ax,TSS1_desc - GDT
        ltr	ax
        mov	ax,LDT_desc - GDT
        lldt	ax

    vcpi_protected:
        mov	ax,Real_D_desc - GDT
        mov	ds,ax
        mov	es,ax
        mov	fs,ax
        mov	gs,ax
        mov	ss,ax
        mov	sp,tss1+tssStk

        jmpt	TSS2_desc - GDT

        cmp	byte [platform],2
        jb	PMtoRM_raw
        clts
        mov	eax,_REAL	;high word of EAX is zeroed
        push	eax
        push	eax
        push	eax
        push	eax
        mov	ax,word [real_stack+Seg16b]
        push	eax
        mov	ax,word [real_stack+Ofs16b]
        push	eax
        pushfd
        push	dword _REAL
        push	dword vcpi_real
        mov	ax,Flat_D_desc - GDT
        mov	ds,ax
        mov	ax,0de0ch
        call	dword far [cs:VCPI_Entry]

    PMtoRM_raw:
        mov	ax,Real_D_desc - GDT
        mov	es,ax
        mov	fs,ax
        mov	gs,ax

        mov	eax,cr0
        and	eax,7ffffff6h
        mov	cr0,eax
        db	0eah
        dw	$+4
        dw	_REAL

        mov	ax,cs
        mov	ds,ax
        lss	sp,[real_stack]

        lidt	[_IDTR_]

    vcpi_real:
        cmp	byte [terminate],0
        jne	near shut_down
        test	bx,bx
        js	exit_application
        call	simulate_int
        jmp	init_application

    exit_application:
        pop	bp
        ret
endp

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Simulate real interrupt
;		In:	BL	= interrupt number
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
proc	near,	simulate_int
        cmp	bl,31h
        je	near rDPMI_Functions

        mov	ax,word [tss2+tssEfl]
        and	ax,0011111011010101b		;User flags
        push	ax
        push	cs

        call	do_int

        pushf
        call	to_user

        pop	ax
        mov	dx,0000111011010101b		;User flags
        mov	cx,word [tss2+tssEfl]
        and	ax,dx
        not	dx
        and	cx,dx
        or	ax,cx
        mov	word [tss2+tssEfl],ax
        ret

    do_int:
        and	ah,~2
        push	ax

        call	_irqno
        jc	IVT
        shl	bp,2
        mov	eax,dword [ds:hwint_rmcb+bp]
        test	eax,eax
        jnz	do_it
    IVT:
        xor	ax,ax
        mov	es,ax
        shl	bx,2
        mov	eax,dword [es:bx]
    do_it:
        push	eax

        call	from_user

        iret

proc	near,	to_user
        cli
        mov	dword [tss2+tssEax],eax
        mov	dword [tss2+tssEbx],ebx
        mov	dword [tss2+tssEcx],ecx
        mov	dword [tss2+tssEdx],edx
        mov	dword [tss2+tssEsi],esi
        mov	dword [tss2+tssEdi],edi
        mov	dword [tss2+tssEbp],ebp
        ret
endp

proc	near,	from_user
        mov	eax,dword [tss2+tssEax]
        mov	ebx,dword [tss2+tssEbx]
        mov	ecx,dword [tss2+tssEcx]
        mov	edx,dword [tss2+tssEdx]
        mov	esi,dword [tss2+tssEsi]
        mov	edi,dword [tss2+tssEdi]
        mov	ebp,dword [tss2+tssEbp]
        ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Real mode DPMI services
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
rDPMI_Functions:
        mov	ax,word [tss2+tssEax]

        cmp	ax,0001h
        je	DPMI_FreeLDTSel
        cmp	ax,0100h
        je	DPMI_AllocDOS
        cmp	ax,0101h
        je	near DPMI_FreeDOS
        cmp	ax,0102h
        je	near DPMI_ResizeDOS
        cmp	ax,0300h
        je	near DPMI_Simulate_RFrame
        cmp	ax,0301h
        je	near DPMI_Simulate_RFrame
        cmp	ax,0302h
        je	near DPMI_Simulate_RFrame
%ifndef	PAGED
        cmp	ax,0500h
        je	near DPMI_GetMemory
        cmp	ax,0501h
        je	near DPMI_AllocMemory
        cmp	ax,0502h
        je	near DPMI_FreeMemory
        cmp	ax,0503h
        je	near DPMI_ResizeMemory
%endif
    rDPMI_Fail:
        or	byte [tss2+tssEfl],1
    rDPMI_Quit:
        ret

DPMI_FreeLDTSel:
        mov	bx,word [tss2+tssEbx]
    FreeSelector:
        call	_free_LDT_sel
        jc	rDPMI_Fail
        mov	si,tss2+tssEs
        mov	cx,4
    FreeSelLoop:
        cmp	word [si],bx
        jne	FreeSelSkip
        mov	word [si],0
    FreeSelSkip:
        cmp	cx,byte 4
        jne	FreeSelNext
        add	si,byte 8
    FreeSelNext:
        add	si,byte 4
        loop	FreeSelLoop
        jmps	rDPMI_Quit
DPMI_AllocDOS:
        mov	cx,1
        call	_alloc_LDT_sel
        jc	rDPMI_Fail
        mov	bx,word [tss2+tssEbx]
        mov	ah,48h
        int	21h
        mov	word [tss2+tssEax],ax
        jc	AllocDOSFail
        mov	word [tss2+tssEdx],di
        movzx	edx,bx
        and	di,byte ~7
        lea	bx,[LDT+di]
        callp	fixDescrBase2, bx, ax
        jmps	DOSBlockLim
    AllocDOSFail:
        mov	word [tss2+tssEbx],bx
        mov	bx,di
        call	_free_LDT_sel
        jmps	rDPMI_Fail
DPMI_FreeDOS:
        mov	bx,word [tss2+tssEdx]
        call	_verify_sel
        jc	rDPMI_Fail
        mov	ah,byte [LDT+bx+B3]
        mov	al,byte [LDT+bx+B2]
        shl	eax,16
        mov	ax,word [LDT+bx+B1]
        shr	eax,4
        mov	es,ax
        mov	ah,49h
        int	21h
        jc	FreeDOSFail
        mov	bx,word [tss2+tssEdx]
        jmp	FreeSelector
    FreeDOSFail:
        mov	word [tss2+tssEax],ax
        jmp	rDPMI_Fail
DPMI_ResizeDOS:
        mov	bx,word [tss2+tssEdx]
        call	_verify_sel
        jc	near rDPMI_Fail
        mov	di,bx
        mov	dh,byte [LDT+bx+B3]
        mov	dl,byte [LDT+bx+B2]
        shl	edx,16
        mov	dx,word [LDT+bx+B1]
        shr	edx,4
        mov	es,dx
        mov	bx,word [tss2+tssEbx]
        mov	ah,4ah
        int	21h
        jc	ResizeDOSFail
        movzx	edx,bx
    DOSBlockLim:
        shl	edx,4
        dec	edx
        mov	word [LDT+di+L1],dx
        shr	edx,16
        and	dl,mask_L2
        and	byte [LDT+di+granted],~mask_L2
        or	byte [LDT+di+granted],dl
        jmp	rDPMI_Quit
    ResizeDOSFail:
        mov	word [tss2+tssEax],ax
        mov	word [tss2+tssEbx],bx
        jmp	rDPMI_Fail
DPMI_Simulate_RFrame:
        mov	dx,sp
        sub	sp,Stack_Sp_sz
    ;Here DR is set by `pSimulate_RFrame'
        test	al,al
        jnz	User_CS_IP
        xor	bx,bx
        mov	es,bx
        mov	bl,byte [tss2+tssEbx]
        shl	bx,2
        mov	ebx,dword [es:bx]
        mov	dword [DR+dpmiIp],ebx
    User_CS_IP:
        cmp	dword [DR+dpmiSp],byte 0
        jnz	User_Stack
        mov	word [DR+dpmiSs],ss
        mov	word [DR+dpmiSp],dx
    User_Stack:
        and	word [DR+dpmiFl],3ed7h
        or	word [DR+dpmiFl],3002h
        mov	cx,word [tss2+tssEcx]
        jcxz	No_User_Parms
        shl	cx,1
        sub	word [DR+dpmiSp],cx
        mov	esi,dword [tss2+tssEsp]
        mov	bx,word [tss2+tssSs]
        movzx	edi,word [DR+dpmiSp]
        movzx	edx,word [DR+dpmiSs]
        shl	edx,4
        add	edi,edx
        mov	dx,Flat_D_desc - GDT
        call	memcopy
    No_User_Parms:
        push	dword [tmp_stack]	;Save for reentrancy
        mov	word [tmp_stack+Ofs16b],sp
        mov	word [tmp_stack+Seg16b],ss
        lss	sp,[DR+dpmiSp]
        cmp	al,1
        je	RetF_Frame
        push	word [DR+dpmiFl]
        and	word [DR+dpmiFl],0fcffh
    RetF_Frame:
        push	cs
        push	word ReturnFromFrame
        push	word [DR+dpmiFl]
        push	word [DR+dpmiCs]
        push	word [DR+dpmiIp]
        call	rest_regs
        iret
    ReturnFromFrame:
        call	save_regs
        lss	sp,[tmp_stack]			;Restore stack
        pop	dword [tmp_stack]
        add	sp,Stack_Sp_sz
    ;Restore RM registers
        mov	bx,Real_D_desc - GDT
        lea	esi,[DR]
        mov	dx,word [tss2+tssEs]
        mov	edi,dword [tss2+tssEdi]
        mov	cx,DPMI_R_size-4*2	;Don't update CS:IP or SS:SP
        call	memcopy
        jmp	rDPMI_Quit
%ifndef	PAGED
DPMI_GetMemory:
        xor	eax,eax
        mov	ah,88h
        call	far [XMS_Driver]
        cmp	bl,80h
        jne	.xmsok
        mov	ah,08h
        call	far [XMS_Driver]
    .xmsok:
        shl	eax,10
        mov	edx,eax
        shr	edx,12
        push	edx
        push	edx
        push	eax
        mov	edi,ss
        shl	edi,4
        movzx	esi,sp
        add	esi,edi
        mov	bx,Flat_D_desc - GDT
        mov	dx,word [tss2+tssEs]
        mov	edi,dword [tss2+tssEdi]
        mov	cx,3*4
        call	memcopy
        add	sp,byte 3*4
        jmp	rDPMI_Quit
DPMI_AllocMemory:
        mov	dx,word [tss2+tssEbx]
        shl	edx,16
        mov	dx,word [tss2+tssEcx]
        call	_malloc
    retblock:
        jc	near rDPMI_Fail
        mov	word [tss2+tssEdi],ax
        shr	eax,16
        mov	word [tss2+tssEsi],ax
        mov	word [tss2+tssEcx],dx
        shr	edx,16
        mov	word [tss2+tssEbx],dx
        jmp	rDPMI_Quit
DPMI_FreeMemory:
        mov	si,word [tss2+tssEsi]
        shl	esi,16
        mov	si,word [tss2+tssEdi]
        call	_mfree
        jc	near rDPMI_Fail
        jmp	rDPMI_Quit
DPMI_ResizeMemory:
        mov	dx,word [tss2+tssEbx]
        shl	edx,16
        mov	dx,word [tss2+tssEcx]
        mov	si,word [tss2+tssEsi]
        shl	esi,16
        mov	si,word [tss2+tssEdi]
        call	_realloc
        jmps	retblock
%endif

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	RM DPMI aid functions
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	_malloc
        test	edx,edx				;!@# Don't allow >7FFFFFFFh
        jle	malloc_fail
        xor	eax,eax
        call	_test_x
        jne	malloc_fail
        push	di,es
        call	_alloc_xmcb
        pop	es,di
        jc	malloc_fail
        call	_inject_x
        clc
        ret
    malloc_fail:
        stc
        ret
endp

proc	near,	_mfree
        mov	eax,esi
        test	eax,eax
        jz	mfree_fail
        call	_test_x
        jne	mfree_fail
        push	di,es
        call	_free_xmcb
        pop	es,di
        jc	mfree_fail
        call	_inject_x
        clc
        ret
    mfree_fail:
        stc
        ret
endp

proc	near,	_realloc
        test	edx,edx				;!@# Don't allow >7FFFFFFFh
        jle	realloc_fail
        mov	eax,esi
        test	eax,eax
        jz	realloc_fail
        call	_test_x
        jne	realloc_fail
        push	di,es
        call	_realloc_xmcb
        pop	es,di
        jc	realloc_fail
        call	_inject_x
        clc
        ret
    realloc_fail:
        stc
        ret
endp

proc	near,	_alloc_xmcb
%ifndef	PAGED
        cmp	byte [platform],2
        je	alloc_xmcb_v
        cmp	byte [platform],1
        je	alloc_xmcb_x
        cmp	byte [platform],0
        je	alloc_xmcb_r
    alloc_xmcb_fail:
        stc
        ret
    alloc_xmcb_v:
%endif
        mov	ax,Flat_D_desc - GDT
        mov	es,ax
        add	edx,4095
        shr	edx,12
        mov	ebx,edx
        call	vcpi_malloc
        jc	alloc_xmcb_fail
        clTLB	eax
        mov	edx,esi
        sub	edx,dword [pt_base]
        shl	edx,10
        mov	eax,esi
%ifndef	PAGED
        jmps	alloc_xmcb_done
    alloc_xmcb_x:
        add	edx,1023
        shr	edx,10
        cmp	edx,0ffffh
        mov	ah,09h
        jbe	.xmsok
        mov	ah,89h
    .xmsok:
        call	far [XMS_Driver]
        test	ax,ax
        jz	alloc_xmcb_fail
        push	byte 0,dx
        mov	ah,0ch
        call	far [XMS_Driver]
        shl	edx,16
        mov	dx,bx		;Base address in EDX
        pop	eax		;Handle in EAX
        jmps	alloc_xmcb_done
    alloc_xmcb_r:
        add	edx,1023
        and	dx,~1023	;Make it kb granular
        mov	ebx,edx
        call	raw_malloc
        jc	alloc_xmcb_fail
        mov	edx,dword [si+x_addr]
        mov	eax,esi
    alloc_xmcb_done:
%endif
        clc
        ret
%ifdef	PAGED
    alloc_xmcb_fail:
        stc
        ret
%endif
endp

proc	near,	_free_xmcb
%ifndef	PAGED
        cmp	byte [platform],2
        je	free_xmcb_v
        cmp	byte [platform],1
        je	free_xmcb_x
        cmp	byte [platform],0
        je	free_xmcb_r
    free_xmcb_fail:
        stc
        ret
    free_xmcb_v:
%endif
        mov	ax,Flat_D_desc - GDT
        mov	es,ax
        call	vcpi_mfree
        clTLB	eax
%ifndef	PAGED
        jmps	free_xmcb_done
    free_xmcb_x:
        mov	dx,si
        mov	ah,0dh
        call	far [XMS_Driver]
        mov	ah,0ah
        call	far [XMS_Driver]
        jmps	free_xmcb_done
    free_xmcb_r:
        call	raw_mfree
    free_xmcb_done:
%endif
        xor	eax,eax
        ret
endp

proc	near,	_realloc_xmcb
%ifndef	PAGED
        cmp	byte [platform],2
        je	realloc_xmcb_v
        cmp	byte [platform],1
        je	realloc_xmcb_x
        cmp	byte [platform],0
        je	realloc_xmcb_r
    realloc_xmcb_fail:
        stc
        ret
    realloc_xmcb_v:
%endif
        mov	ax,Flat_D_desc - GDT
        mov	es,ax
        add	edx,4095
        shr	edx,12
        mov	ebx,edx
        call	vcpi_realloc
        jc	realloc_xmcb_fail
        clTLB	eax
        mov	edx,esi
        sub	edx,dword [pt_base]
        shl	edx,10
        mov	eax,esi
%ifndef	PAGED
        jmps	realloc_xmcb_done
    realloc_xmcb_x:
        add	edx,1023
        shr	edx,10
        xchg	esi,edx
        mov	ah,0dh
        call	far [XMS_Driver]
        mov	ebx,esi
        shr	esi,16
        mov	ah,0fh
        jz	.xmsok
        mov	ah,8fh
    .xmsok:
        call	far [XMS_Driver]
        test	ax,ax
        jz	realloc_xmcb_fail
        push	edx
        mov	ah,0ch
        call	far [XMS_Driver]
        shl	edx,16
        mov	dx,bx		;Base address in EDX
        pop	eax		;Handle in EAX
        jmps	realloc_xmcb_done
    realloc_xmcb_r:
        add	edx,1023
        and	dx,~1023	;Make it kb granular
        mov	ebx,edx
        call	raw_realloc
        jc	realloc_xmcb_fail
        mov	edx,dword [si+x_addr]
        mov	eax,esi
    realloc_xmcb_done:
%endif
        clc
        ret
%ifdef	PAGED
    realloc_xmcb_fail:
        stc
        ret
%endif
endp

proc	near,	_test_x
        push	ds
        pop	es
        mov	di,X_H
        mov	cx,MaxXHndl
        cld
        repne	scasd
        ret
endp

proc	near,	_inject_x
        sub	di,byte 4
        stosd
        ret
endp

proc	near,	vcpi_malloc
        call	vcpi_lmalloc
        cmp	edx,byte 1
        jc	.vm_fail
        cmp	edx,ebx
        jb	.vm_1
        call	vcpi_pmalloc
        jmps	.vm_done
    .vm_1:
        mov	ebx,edx
        call	vcpi_pmalloc
        jc	.vm_fail
        call	vcpi_mfree
        mov	eax,ebx
        stc
    .vm_fail:
    .vm_done:
        ret
endp

proc	near,	vcpi_lmalloc
        mov	edi,dword [pt_free]
        mov	ecx,dword [pt_top]
        sub	ecx,edi
        shr	ecx,2
        xor	eax,eax
        cdq
        push	ebx
    .vlm_1:
        jecxz	.vlm_done
        repne	a32 scasd
        jne	.vlm_done
        mov	ebp,ecx
        lea	ebx,[edi-4]
        repe	a32 scasd
        jne	.vlm_2
        inc	ebp
    .vlm_2:
        sub	ebp,ecx
        cmp	ebp,edx
        jb	.vlm_1
        mov	esi,ebx
        mov	edx,ebp
        cmp	ebp,dword [esp]
        jb	.vlm_1
    .vlm_done:
        pop	ebx
        ret
endp

proc	near,	vcpi_pmalloc
        mov	edi,esi
        xor	ebp,ebp
    .vpm_1:
        mov	ax,0de04h
        call	dword far [VCPI_Entry]
        or	ah,ah
        jz	.vpm_2
        cmp	ebp,byte 1
        jc	.vpm_done
        or	byte [es:edi-3],PT_E
        call	vcpi_mfree
        stc
        jmps	.vpm_done
    .vpm_2:
        and	dh,0f0h
        mov	dl,PT_U | PT_W | PT_P
        mov	dword [es:edi],edx
        add	edi,byte 4
        inc	ebp
        cmp	ebp,ebx
        jb	.vpm_1
        or	byte [es:edi-3],PT_E
        clc
    .vpm_done:
        mov	eax,ebp
        ret
endp

proc	near,	vcpi_mfree
        mov	edi,esi
    .vmf_1:
        xor	ecx,ecx
        xchg	ecx,dword [es:edi]
        add	edi,byte 4
        mov	edx,ecx
        and	dx,0f000h
        mov	ax,0de05h
        call	dword far [VCPI_Entry]
        test	ch,PT_E
        jz	.vmf_1
        ret
endp

proc	near,	vcpi_realloc
    ;In:	ES	= flat data
    ;	EBX	= new size in pages
    ;	ESI	= old handle
        mov	edi,esi				; EDI = ESI, ptr to linear block start
        xor	ebp,ebp				; EBP = running block size in pages
    .vra_getsize:
        add	edi,byte 4			; increment page table ptr
        inc	ebp				; increment block size
        test	byte [es:edi-3],PT_E		; last page of block?
        jz	.vra_getsize			; if no, loop
        sub	ebx,ebp				; EBX = change in block size
        jz	near .vra_done			; if no change, done
        jc	near .vra_shrink		; if block made smaller, just free top
        mov	ecx,dword [pt_top]		; ECX = count of pages to search
        sub	ecx,edi
        shr	ecx,2
        mov	edx,ecx				; EDX = current count
        xor	eax,eax				; EAX = search unit, free entry (0)
        jecxz	.vra_trybelow			; if no entries above, try below
        repe	a32 scasd			; check for free entries above block
        je	.vra_1				; if previous entry free, go on
        dec	edx				; previous entry not free, minus one
    .vra_1:
        sub	edx,ecx				; EDX = number of free pages in block
        cmp	edx,ebx				; enough linear memory?
        jb	.vra_trybelow			; if no, try below in linear memory
        push	esi				; preserve start of block
        lea	esi,[esi+ebp*4]			; ESI -> start of new block for alloc
        call	vcpi_pmalloc			; try to allocate physical memory
        mov	edi,esi				; EDI -> start of new block
        pop	esi				; restore start of old block
        jc	near .vra_fail			; if alloc failed, error 8013h
        and	byte [es:edi-3],~PT_E		; clear last bit in old block end
        jmp	.vra_done			; go to done
    .vra_trybelow:
        mov	ecx,esi				; ECX = count of pages to search up
        sub	ecx,dword [pt_free]
        shr	ecx,2
        or	ecx,ecx				; any linear memory below?
        jz	near .vra_alloc			; if no, try to allocate
        push	ebp				; preserve size of original block
        lea	edi,[esi-4]			; EDI = ESI, ptr to linear block start
        mov	ebp,ecx				; EBP = current count
        std					; search is up
        repe	a32 scasd			; check for free entries after block
        cld
        je	.vra_2				; if previous entry free, go on
        dec	ebp				; previous entry not free, minus one
    .vra_2:
        sub	ebp,ecx				; EBP = number of free pages in block
        lea	eax,[ebp+edx]			; free size below + free size above
        pop	ebp				; restore original block size
        cmp	eax,ebx				; enough linear memory?
        jb	near .vra_alloc			; if no, try to allocate
        push	esi				; preserve original block address
        sub	ebx,edx				; EBX = number of pages needed below
        lea	eax,[ebx*4]			; get base of block below
        sub	esi,eax
        push	edx,ebp				; preserve some vars
        call	vcpi_pmalloc			; try to allocate physical memory
        pop	ebp,edx				; restore some vars
        mov	edi,esi				; EDI -> base of block below
        pop	esi				; restore base of original block
        jc	near .vra_fail			; if alloc failed, error 8013h
        or	edx,edx				; any pages needed above?
        jz	.vra_3				; if no, go on
        push	esi,edi,ebp			; preserve some vars
        mov	ebx,edx				; EBX = size of block below
        lea	esi,[esi+ebp*4]			; ESI -> start of block above
        call	vcpi_pmalloc			; try to allocate physical memory
        pop	ebp,edi,esi			; restore some vars
        jnc	.vra_4				; if allocated ok, go on
        mov	esi,edi				; ESI -> allocated block below
        call	vcpi_mfree			; free allocated block below
        jmp	.vra_fail			; fail, error 8013h
    .vra_4:
        and	byte [es:esi-3],~PT_E		; clear last bit in below block end
    .vra_3:
        and	byte [es:esi+ebp*4-3],~PT_E	; clear last bit in old block end
        push	edi				; preserve new block start
        mov	edx,edi				; EDX = base of move area
        lea	ebx,[esi-4]			; EBX = current location in move area
    .vra_shiftpage:
        mov	edi,ebx				; set up to shift up a page
        mov	esi,ebx
        mov	ecx,ebp
        push	ds				; save data selector
        push	es
        pop	ds				; ds = core selector
        a32	lodsd				; shift old pages a page down in table
        rep	a32 movsd
        a32	stosd
        pop	ds				; ds now data selector again
        sub	ebx,byte 4			; decrement to next page to shift
        cmp	ebx,edx				; more pages to shift?
        jae	.vra_shiftpage			; if yes, loop
        pop	esi				; restore new block start address
        jmps	.vra_done			; go to done
    .vra_alloc:
        add	ebx,ebp				; restore EBX as requested size
        push	esi,ebp				; preserve some vars
        call	vcpi_lmalloc			; check for linear memory block
        pop	ebp,edi				; restore some vars
        cmp	edx,ebx				; enough linear memory?
        jb	.vra_fail			; if no, error 8012h
        sub	ebx,ebp				; EBX = extra pages needed
        push	esi				; preserve for later copy
        lea	esi,[esi+ebp*4]			; ESI -> start of extra space needed
        push	edi,ebp				; preserve some vars
        call	vcpi_pmalloc			; try to allocate physical memory
        pop	ecx,esi,edi			; restore some vars
        jc	.vra_fail			; if not enough mem, error 8013h
        push	edi,esi,ecx			; preserve, new and old block, size
        push	ds
        push	es
        pop	ds
        rep	a32 movsd			; copy old block pages
        pop	ds
        and	byte [es:edi-3],~PT_E		; clear last bit in old block end
        pop	ecx,edi				; restore to clear old block
        xor	eax,eax				; new page table entry is free (0)
        rep	a32 stosd			; clear old page table block
        pop	esi				; restore new block address
        jmps	.vra_done			; go to done
    .vra_shrink:
        sub	edi,byte 4			; decrement page table ptr
        xor	edx,edx				; new page table entry is free (0)
        xchg	edx,dword [es:edi]		; swap EDX with page table entry
        and	dx,0f000h			; mask off low 12 bits
        mov	ax,0de05h			; VCPI free a page
        call	dword far [VCPI_Entry]
        inc	ebx				; increment negative change counter
        jnz	.vra_shrink			; if more pages to free, loop
        or	byte [es:edi-3],PT_E		; set next page up as last of block
    .vra_done:
        clc
        ret
    .vra_fail:
        stc
        ret
endp

%ifndef	PAGED
proc	near,	raw_malloc
        xor	edx,edx
        xor	eax,eax
        xor	esi,esi
        mov	cx,MaxXHndl
        mov	di,X_HDescr
        push	ebx
        mov	ebx,-1
    .rma_run:
        cmp	byte [di+x_flag],1		;Test free
        jne	.rma_next
        mov	ebp,dword [di+x_size]
        add	edx,ebp			;EDX = running total of free
        cmp	ebp,eax
        jb	.rma_1
        mov	eax,ebp			;EAX = largest free block
    .rma_1:
        cmp	ebp,dword [esp]		;Current block fits?
        jb	.rma_next		;If no then pass
        cmp	ebx,ebp			;Is it smaller than previous?
        jb	.rma_next		;If no, pass
        mov	si,di			;Save pointer
        mov	ebx,ebp
    .rma_next:
        add	di,byte x_hndl_size
        loop	.rma_run
        pop	ebx
        or	si,si
        jz	.rma_fail
; EBX = requested size
; SI = matching block address
; EAX = largest available block
; EDX = running total of free bytes
        cmp	dword [si+x_size],ebx
        je	.rma_finish			;Finish if exact size
        mov	cx,MaxXHndl
        mov	di,X_HDescr
    .rma_unused:
        cmp	byte [di+x_flag],4		;Test unused
        je	.rma_alloc_block
        add	di,byte x_hndl_size
        loop	.rma_unused
    .rma_fail:
        stc
    .rma_done:
        ret
    .rma_alloc_block:
        mov	ecx,dword [si+x_addr]
        add	ecx,ebx
        mov	dword [di+x_addr],ecx
        mov	ecx,ebx
        xchg	dword [si+x_size],ebx
        sub	ebx,ecx
        mov	dword [di+x_size],ebx
        mov	byte [di+x_flag],1
    .rma_finish:
        mov	byte [si+x_flag],2
        clc
        jmps	.rma_done
endp

proc	near,	raw_mfree
    ;High word of handle not used (=0000)
        mov	byte [si+x_flag],1		;Mark free
    ;Merge free blocks
    .rmf_merge_begin:
        mov	ebx,dword [si+x_addr]
        mov	eax,ebx
        add	eax,dword [si+x_size]
        mov	cx,MaxXHndl
        mov	di,X_HDescr
    .rmf_merge_loop:
        cmp	byte [di+x_flag],1
        jne	.rmf_merge_next
        mov	edx,dword [di+x_addr]
        cmp	edx,eax
        je	.rmf_merge_1
        add	edx,dword [di+x_size]
        cmp	edx,ebx
        je	.rmf_merge_0
    .rmf_merge_next:
        add	di,byte x_hndl_size
        loop	.rmf_merge_loop
        ret
    .rmf_merge_0:
        xchg	si,di
    .rmf_merge_1:
        mov	edx,dword [si+x_size]
        add	edx,dword [di+x_size]
        mov	dword [si+x_size],edx
        mov	byte [di+x_flag],4
        jmps	.rmf_merge_begin
endp

proc	near,	raw_realloc
        push	si
    ;Backup block descriptors
        mov	si,X_HDescr
        mov	di,X_HDescr_bak
        mov	cx,MaxXHndl*x_hndl_size
        rep	movsb
        pop	si
    ;Save old address and size
        push	dword [si+x_addr]
        push	dword [si+x_size]
    ;Free old block
        push	edx
        call	raw_mfree
        pop	ebx
    ;Try allocate new block
        call	raw_malloc
    ;Fetch old size and address
        pop	ecx,eax
    ;If can't allocate, restore the handles and fail
        jc	.rra_fail
    ;Copy the smallest of the two (in protected mode)
        cmp	ecx,dword [si+x_size]
        jbe	.rra_1
        mov	ecx,dword [si+x_size]
    .rra_1:
        mov	edi,dword [si+x_addr]
        xchg	esi,eax
        cmp	edi,esi
        je	.rra_done
        push	ds
        mov	dx,Flat_D_desc - GDT
        mov	ds,dx
        mov	es,dx
        jb	.rra_moveb
        std
        add	esi,ecx
        add	edi,ecx
        dec	esi
        dec	edi
    .rra_moveb:
        mov	dl,cl
        and	dl,3
        shr	ecx,2
        rep	a32 movsd
        mov	cl,dl
        rep	a32 movsb
        cld
        pop	ds
    .rra_done:
        mov	esi,eax
        clc
        ret
    .rra_fail:
    ;Restore block descriptors
        mov	di,X_HDescr
        mov	si,X_HDescr_bak
        mov	cx,MaxXHndl*x_hndl_size
        rep	movsb
        stc
        ret
endp
%endif

proc	near,	save_regs
        push	ds
        pushf
        cli
        push	cs
        pop	ds
        pop	word [DR+dpmiFl]
        and	byte [DR+dpmiFl+1],3fh
        mov	dword [DR+dpmiEax],eax
        mov	dword [DR+dpmiEbx],ebx
        mov	dword [DR+dpmiEcx],ecx
        mov	dword [DR+dpmiEdx],edx
        mov	dword [DR+dpmiEsi],esi
        mov	dword [DR+dpmiEdi],edi
        mov	dword [DR+dpmiEbp],ebp
        mov	word [DR+dpmiEs],es
        mov	word [DR+dpmiFs],fs
        mov	word [DR+dpmiGs],gs
        pop	word [DR+dpmiDs]
        ret
endp

proc	near,	rest_regs
        mov	eax,dword [DR+dpmiEax]
        mov	ebx,dword [DR+dpmiEbx]
        mov	ecx,dword [DR+dpmiEcx]
        mov	edx,dword [DR+dpmiEdx]
        mov	esi,dword [DR+dpmiEsi]
        mov	edi,dword [DR+dpmiEdi]
        mov	ebp,dword [DR+dpmiEbp]
        mov	es,word [DR+dpmiEs]
        mov	fs,word [DR+dpmiFs]
        mov	gs,word [DR+dpmiGs]
        mov	ds,word [DR+dpmiDs]
        ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Copy CX bytes from BX:ESI to DX:EDI
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	C,near,	memcopy
    loc	%$t_info,64
        pusha
    ;Save TSS2 info
        mov	si,tss2+tssEip
        push	ss
        pop	es
        lea	di,[bp+%$t_info]
        mov	cx,32
        cld
        rep	movsw
        popa
        pusha
    ;Setup TSS2 for copy
        mov	word [tss2+tssCs],Real_C_desc - GDT
        mov	dword [tss2+tssEip],_memcopy
        mov	word [tss2+tssEcx],cx
        mov	word [tss2+tssEcx+2],0
        mov	word [tss2+tssDs],bx
        mov	dword [tss2+tssEsi],esi
        mov	word [tss2+tssEs],dx
        mov	dword [tss2+tssEdi],edi
        mov	dword [tss2+tssEfl],3002h
    ;Execute copy in protected mode
        call	main_switch
    ;Restore TSS2
        push	ds
        push	ss
        pop	ds
        lea	si,[bp+%$t_info]
        push	cs
        pop	es
        mov	di,tss2+tssEip
        mov	cx,32
        cld
        rep	movsw
        pop	ds
        popa
        ret
_memcopy:
        rep	a32 movsb
        jmpt	TSS1_desc - GDT
endp

endp

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Real-mode Call-backs
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
        %assign	x 0
        %rep	Num_IRQ2+Num_RMCB
            MkRMCB	x
            %assign	x x+1
        %endrep
RMCB_ID:
        call	save_regs
        pop	ax

        mov	word [DR+dpmiSp],sp
        mov	word [DR+dpmiSs],ss

        mov	dx,_LOCKED
        mov	ss,dx			;Don't rely on user stack
        mov	esp,dword [locked_top]

        sub	dword [locked_top],Locked_frame
        inc	byte [in_rmcb]
        call	common_rmcb
        dec	byte [in_rmcb]
        add	dword [locked_top],Locked_frame

        lss	sp,[DR+dpmiSp]
        push	word [DR+dpmiFl]
        push	word [DR+dpmiCs]
        push	word [DR+dpmiIp]
        call	rest_regs
        iret

proc	C,near,	common_rmcb
    loc	%$t_info,64

    ;Save TSS2 info
        mov	si,tss2+tssEip
        push	ss
        pop	es
        lea	di,[bp+%$t_info]
        mov	cx,32
        cld
        rep	movsw

    ;Setup RMCB call frame
        sub	ax,RMCB1
        mov	bl,RMCB1 - RMCB0
        div	bl
        imul	ax,byte rmcb_size
        xchg	ax,bx
        mov	word [tss2+tssEbp],bx

    ;Setup TSS2 for the great Call-back
        mov	al,byte [rmcbs+bx+cbType]
        mov	byte [tss2+tssEax],al
        mov	dword [tss2+tssEfl],3002h
        mov	word [tss2+tssCs],Real_C_desc - GDT
        mov	dword [tss2+tssEip],rmcb_task
        mov	word [tss2+tssSs],Locked_desc - GDT
        mov	ecx,dword [locked_top]
        mov	dword [tss2+tssEsp],ecx
        mov	word [tss2+tssFs],Real_D_desc - GDT
        mov	dword [tss2+tssEbx],DR
        mov	word [tss2+tssDs],Flat_D_desc - GDT
        movzx	ecx,word [DR+dpmiSs]
        shl	ecx,4
        movzx	edx,word [DR+dpmiSp]
        add	ecx,edx
        mov	dword [tss2+tssEsi],ecx

        cmp	al,2
        je	not_internal_rmcb
        mov	ax,word [tss2+tssSs]
        mov	word [tss2+tssEs],ax		; We want regs stored on stack
        sub	dword [tss2+tssEsp],byte ((DPMI_R_size+3) & ~3)
        mov	eax,dword [tss2+tssEsp]
        mov	dword [tss2+tssEdi],eax		; ES:EDI == SS:ESP
        jmps	enter_rmcb
    not_internal_rmcb:
        mov	eax,dword [rmcbs+bx+cbRegs+Ofs32b]
        mov	dword [tss2+tssEdi],eax
        mov	ax,word [rmcbs+bx+cbRegs+Sel16b]
        mov	word [tss2+tssEs],ax
enter_rmcb:
        sub	dword [locked_top],Locked_frame
        call	main_switch
        add	dword [locked_top],Locked_frame
    ;Restore TSS2
        push	ds
        push	ss
        pop	ds
        lea	si,[bp+%$t_info]
        push	cs
        pop	es
        mov	di,tss2+tssEip
        mov	cx,32
        cld
        rep	movsw
        pop	ds
        ret
rmcb_task:
        push	esi,edi
        mov	esi,ebx			;Copy RM regs to PM
        mov	ecx,DPMI_R_size/2
        rep	fs a32 movsw
        pop	edi,esi
        push	ebx,fs			;Save for return restore
        push	esi,ds			;Real stack for us if internal int
        push	ax

        pushfd
        call	dword far [fs:rmcbs+bp+cbCode]

        pop	ax
        pop	ds,esi
        cmp	al,2
        je	not_internal
        a32	lodsd			;Get CS:IP from real stack in eax
        mov	dword [es:edi+dpmiIp],eax
        a32	lodsw				; Get flags
        mov	word [es:edi+dpmiFl],ax
        add	word [es:edi+dpmiSp],byte 6	; Adjust RM SP
    not_internal:
        push	es
        pop	ds
        mov	esi,edi			; ds:esi now = es:edi
        pop	es,edi			; es:edi is now reg struct
        mov	ecx,DPMI_R_size/2	; Size of reg struct in words
        rep	a32 movsw
    ;Invalidate int no for `main_switch' return
        push	es
        pop	ds
        mov	bx,-1
jmp_TSS1_raw:
        and	byte [TSS2_desc+access],0fdh
        mov	ax,TSS1_desc - GDT
        ltr	ax
        mov	ss,word [tss1+tssSs]
        mov	esp,dword [tss1+tssEsp]
        push	dword [tss1+tssCs]
        push	dword [tss1+tssEip]
        retfd
endp

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Interrupt handling
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	All interrupts
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
        %assign	x 0
        %rep	256
            MakeInt	x
            %assign	x x+1
        %endrep

Common_int_handler:
        cli
        push	ds,cx,ax

        mov	cx,Real_D_desc - GDT
        mov	ds,cx
        mov	ch,ah
        mov	ax,word [esp+3*2]
        sub	ax,Interrupt1
        mov	cl,Interrupt1 - Interrupt0
        div	cl
        mov	word [esp+3*2],ax

        mov	ah,ch
        cmp	ax,4c21h
        sete	byte [terminate]
        cmp	al,31h
        pop	ax,cx
        je	Int31_handler

    PM2RM_Bypass:
        call	to_user
        pop	word [tss2+tssDs]
        mov	word [tss2+tssEs],es
        mov	word [tss2+tssFs],fs
        mov	word [tss2+tssGs],gs
        pop	bx
        pop	dword [tss2+tssEip]
        pop	word [tss2+tssCs]
        pop	ax
        pop	dword [tss2+tssEfl]
        mov	word [tss2+tssSs],ss
        mov	dword [tss2+tssEsp],esp
        jmp	jmp_TSS1_raw

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	INT 31h
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
Int31_handler:
        push	fs,gs,es,ss
        pushad
        cld
        push	byte (Flat_D_desc - GDT)
        pop	fs
        mov	ebp,esp
        and	byte [ebp+8*4+5*2+2+2*4],~1
        push	bx
        mov	bx,Int31_fns
    pDPMI_Search:
        cmp	ax,word [bx]
        je	pDPMI_Found
        inc	bx
        inc	bx
        cmp	bx,Int31_ofs
        jne	pDPMI_Search
        pop	bx
    pDPMI_Bypass:
        popad
        pop	ss,es,gs,fs
        push	byte (Real_D_desc - GDT)
        pop	ds
        jmps	PM2RM_Bypass
    pDPMI_Found:
        mov	bx,word [byte Int31_ofs+bx-Int31_fns]
        xchg	word [esp],bx
        RETn
    pDPMI_Fail:
        or	byte [esp+8*4+5*2+2+2*4],1
    pDPMI_Quit:
        popad
        pop	ss,es,gs,fs,ds
        add	esp,byte 2
        iretd

DPMI_AllocLDTSel:
        jcxz	pDPMI_Fail
        call	_alloc_LDT_sel
        jc	pDPMI_Fail
        dec	cx
        mov	word [pointer_inc],cx
        mov	word [ebp+dpmiAx],di
        jmps	pDPMI_Quit
DPMI_GetPointerInc:
        xor	ax,ax
        cmp	word [pointer_inc],ax
        je	setpointerinc
        dec	word [pointer_inc]
        mov	al,8
    setpointerinc:
        mov	word [ebp+dpmiAx],ax
        jmps	pDPMI_Quit
DPMI_GetSegBase:
        call	_get_descriptor
        jc	pDPMI_Fail
        mov	ax,word [si+B1]
        mov	word [ebp+dpmiDx],ax
        mov	al,byte [si+B2]
        mov	ah,byte [si+B3]
        mov	word [ebp+dpmiCx],ax
        jmps	pDPMI_Quit
DPMI_SetSegBase:
        call	_verify_sel
        jc	pDPMI_Fail
        mov	word [LDT+bx+B1],dx
        mov	byte [LDT+bx+B2],cl
        mov	byte [LDT+bx+B3],ch
        jmps	pDPMI_Quit
DPMI_SetSegLim:
        call	_verify_sel
        jc	pDPMI_Fail
        cmp	cx,byte 0fh
        jbe	SetLim
        mov	ax,dx
        and	ax,0fffh
        cmp	ax,0fffh
        jne	pDPMI_Fail
        shrd	dx,cx,12
        shr	cx,12
        or	cl,mask_G
    SetLim:
        mov	word [LDT+bx+L1],dx
        and	byte [LDT+bx+granted],~(mask_L2|mask_G)
        or	byte [LDT+bx+granted],cl
        jmp	pDPMI_Quit
DPMI_SetSegAcc:
        call	_verify_sel
        jc	near pDPMI_Fail
        call	_test_access
        jc	near pDPMI_Fail
        mov	byte [LDT+bx+access],cl
        and	byte [LDT+bx+granted],mask_L2
        or	byte [LDT+bx+granted],ch
        jmp	pDPMI_Quit
DPMI_CreateAlias:
        call	_get_descriptor
        jc	near pDPMI_Fail
        mov	cx,1
        call	_alloc_LDT_sel
        jc	near pDPMI_Fail
        mov	word [ebp+dpmiAx],di
        push	ds
        pop	es
        and	di,byte ~7
        add	di,LDT
        movsd
        movsd
        mov	byte [di-8+access],92h
        jmp	pDPMI_Quit
DPMI_GetDescriptor:
        call	_get_descriptor
        jc	near pDPMI_Fail
        movzx	esi,si
        a32	movsd
        a32	movsd
        jmp	pDPMI_Quit
DPMI_SetDescriptor:
        call	_verify_sel
        jc	near pDPMI_Fail
        push	ds,es
        pop	ds,es
        mov	esi,edi
        lea	edi,[LDT+bx]
        mov	cl,byte [esi+access]
        mov	ch,byte [esi+granted]
        call	_test_access
        jc	near pDPMI_Fail
        a32	movsd
        a32	movsd
        jmp	pDPMI_Quit
DPMI_GetRInterrupt:
        xor	bh,bh
        shl	bx,2
        mov	dx,word [fs:bx+Ofs16b]
        mov	cx,word [fs:bx+Seg16b]
        mov	word [ebp+dpmiDx],dx
        mov	word [ebp+dpmiCx],cx
        jmp	pDPMI_Quit
DPMI_SetRInterrupt:
        xor	bh,bh
        shl	bx,2
        mov	word [fs:bx+Ofs16b],dx
        mov	word [fs:bx+Seg16b],cx
        jmp	pDPMI_Quit
DPMI_GetXVector:
        cmp	bl,Num_Exc
        jnb	near pDPMI_Fail
        xor	bh,bh
        imul	bx,byte Addr32b_size
        mov	edx,dword [Exc_Excs+bx+Ofs32b]
        mov	cx,word [Exc_Excs+bx+Sel16b]
        mov	dword [ebp+dpmiEdx],edx
        mov	word [ebp+dpmiCx],cx
        jmp	pDPMI_Quit
DPMI_SetXVector:
        cmp	bl,Num_Exc
        jnb	near pDPMI_Fail
        xor	bh,bh
        imul	bx,byte Addr32b_size
        mov	dword [Exc_Excs+bx+Ofs32b],edx
        mov	word [Exc_Excs+bx+Sel16b],cx
        jmp	pDPMI_Quit
DPMI_GetPInterrupt:
        xor	bh,bh
        cmp	bl,Num_Exc
        jb	Get_Exc_Int
    Get_Reg_Int:
        shl	bx,3
        mov	dx,word [IDT+bx+gateO2]
        shl	edx,16
        mov	dx,word [IDT+bx+gateO1]
        mov	cx,word [IDT+bx+gateSel]
        jmps	RetPInt
    Get_Exc_Int:
        imul	bx,byte Addr32b_size
        mov	edx,dword [Exc_Ints+bx+Ofs32b]
        mov	cx,word [Exc_Ints+bx+Sel16b]
    RetPInt:
        mov	dword [ebp+dpmiEdx],edx
        mov	word [ebp+dpmiCx],cx
        jmp	pDPMI_Quit
DPMI_SetPInterrupt:
        xor	bh,bh
        call	_hook_hw
        cmp	bl,Num_Exc
        jb	Set_Exc_Int
    Set_Reg_Int:
        shl	bx,3
        mov	word [IDT+bx+gateO1],dx
        shr	edx,16
        mov	word [IDT+bx+gateO2],dx
        mov	word [IDT+bx+gateSel],cx
        jmp	pDPMI_Quit
    Set_Exc_Int:
        imul	bx,byte Addr32b_size
        mov	dword [Exc_Ints+bx+Ofs32b],edx
        mov	word [Exc_Ints+bx+Sel16b],cx
        jmp	pDPMI_Quit
DPMI_pSimulate_RFrame:
    ;Save an extra mode swap (memcopy) for `Simulate_RFrame'
        push	ds,es
        pop	ds,es
        mov	esi,edi
        lea	edi,[DR]
        mov	ecx,DPMI_R_size/2
        rep	a32 movsw
        jmp	pDPMI_Bypass
DPMI_AllocRMCB:
        mov	si,Num_IRQ2*rmcb_size
        mov	cx,Num_RMCB
    rmcbs_loop:
        cmp	byte [rmcbs+si+cbType],0
        jne	rmcb_next
        mov	byte [rmcbs+si+cbType],2
        mov	eax,dword [ebp+dpmiEsi]
        mov	dword [rmcbs+si+cbCode+Ofs32b],eax
        mov	ax,word [ebp+8*4+4*2]
        mov	word [rmcbs+si+cbCode+Sel16b],ax
        mov	eax,dword [ebp+dpmiEdi]
        mov	dword [rmcbs+si+cbRegs+Ofs32b],eax
        mov	ax,word [ebp+dpmiEs]
        mov	word [rmcbs+si+cbRegs+Sel16b],ax
        mov	word [ebp+dpmiCx],_REAL
        mov	bl,rmcb_size
        xchg	ax,si
        div	bl
        imul	ax,byte (RMCB1 - RMCB0)
        add	ax,RMCB0
        mov	word [ebp+dpmiDx],ax
        jmp	pDPMI_Quit
    rmcb_next:
        add	si,byte rmcb_size
        loop	rmcbs_loop
        jmp	pDPMI_Fail
DPMI_FreeRMCB:
        cmp	cx,_REAL
        jne	near pDPMI_Fail
        mov	ax,dx
        sub	ax,RMCB0
        mov	bl,RMCB1 - RMCB0
        div	bl
        imul	ax,byte rmcb_size
        xchg	ax,si
        cmp	byte [rmcbs+si+cbType],2
        jne	near pDPMI_Fail
        mov	byte [rmcbs+si+cbType],0
        jmp	pDPMI_Quit
DPMI_GetVersion:
        mov	word [ebp+dpmiAx],005ah
        movzx	ax,byte [platform]
        or	al,1
        mov	word [ebp+dpmiBx],ax
        mov	al,byte [cpu_id]
        mov	byte [ebp+dpmiCl],al
        mov	ah,byte [MasterPIC]
        mov	al,byte [SlavePIC]
        mov	word [ebp+dpmiDx],ax
        jmp	pDPMI_Quit
DPMI_pGetMemory:
        push	edi,es
        mov	eax,-1
        mov	ecx,12
        rep	a32 stosd
        mov	ebx,eax
%ifndef	PAGED
        cmp	byte [platform],2
        je	V_GetMemory
        cmp	byte [platform],0
        je	R_GetMemory
        pop	es,edi
        jmp	pDPMI_Bypass
    R_GetMemory:
        call	raw_malloc
        jmps	pGM_@@0
    V_GetMemory:
%endif
        push	fs
        pop	es
        call	vcpi_malloc
        shl	eax,12
    pGM_@@0:
        pop	es,edi
        a32	stosd
        shr	eax,12
        a32	stosd
        a32	stosd
        jmp	pDPMI_Quit
DPMI_pAllocMemory:
%ifndef	PAGED
        cmp	byte [platform],1
        je	near pDPMI_Bypass
%endif
        push	bx,cx
        pop	edx
        call	_malloc
    pretblock:
        jc	near pDPMI_Fail
        mov	word [esp+dpmiDi],ax
        shr	eax,16
        mov	word [esp+dpmiSi],ax
        mov	word [esp+dpmiCx],dx
        shr	edx,16
        mov	word [esp+dpmiBx],dx
        jmp	pDPMI_Quit
DPMI_pFreeMemory:
%ifndef	PAGED
        cmp	byte [platform],1
        je	near pDPMI_Bypass
%endif
        push	si,di
        pop	esi
        call	_mfree
        jc	near pDPMI_Fail
        jmp	pDPMI_Quit
DPMI_pResizeMemory:
%ifndef	PAGED
        cmp	byte [platform],1
        je	near pDPMI_Bypass
%endif
        push	bx,cx
        pop	edx
        push	si,di
        pop	esi
        call	_realloc
        jmps	pretblock
DPMI_GetPageSize:
        mov	word [ebp+dpmiBx],0
        mov	word [ebp+dpmiCx],4096
        jmp	pDPMI_Quit
DPMI_PhysicalMap:
        push	fs
        pop	es
        push	bx,cx
        pop	eax
        cmp	eax,100000h
        jb	near pDPMI_Fail
        push	si,di
        pop	ebx
        test	ebx,ebx
        jz	near pDPMI_Fail
        and	ecx,0fffh
        add	ebx,ecx
        jc	near pDPMI_Fail
        add	ebx,4095
        jc	near pDPMI_Fail
%ifndef	PAGED
        cmp	byte [platform],2
        jne	near pDPMI_Quit
%endif
        shr	ebx,12
        push	ecx,eax
        call	vcpi_lmalloc
        pop	eax,ebp
        cmp	edx,ebx
        jb	near pDPMI_Fail
        mov	edi,esi
        and	ah,0f0h
        mov	al,PT_CD | PT_U | PT_W | PT_P
    .pmap:
        mov	dword [es:edi],eax
        add	eax,4096
        add	edi,byte 4
        dec	ebx
        jne	.pmap
        or	byte [es:edi-3],PT_E
        mov	ecx,esi
        sub	ecx,dword [pt_base]
        shl	ecx,10
        add	ecx,ebp
        mov	word [esp+dpmiCx],cx
        shr	ecx,16
        mov	word [esp+dpmiBx],cx
        clTLB	eax
        jmp	pDPMI_Quit
DPMI_DisableInts:
        mov	al,byte [ebp+8*4+5*2+2+2*4+1]
        and	byte [ebp+8*4+5*2+2+2*4+1],~2
        jmps	ReturnInts
DPMI_EnableInts:
        mov	al,byte [ebp+8*4+5*2+2+2*4+1]
        or	byte [ebp+8*4+5*2+2+2*4+1],2
        jmps	ReturnInts
DPMI_GetInts:
        mov	al,byte [ebp+8*4+5*2+2+2*4+1]
    ReturnInts:
        test	al,2
        setnz	byte [ebp+dpmiAl]
        jmp	pDPMI_Quit

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	INT 31h dispatcher data
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
Int31_fns	dw	0000h,0003h,0006h,0007h,0008h,0009h,000ah,000bh,000ch
        dw	0200h,0201h,0202h,0203h,0204h,0205h
        dw	0300h,0301h,0302h,0303h,0304h
        dw	0400h
        dw	0500h,0501h,0502h,0503h
        dw	0600h,0601h,0604h
        dw	0800h
        dw	0900h,0901h,0902h

Int31_ofs	dw	DPMI_AllocLDTSel
        dw	DPMI_GetPointerInc
        dw	DPMI_GetSegBase
        dw	DPMI_SetSegBase
        dw	DPMI_SetSegLim
        dw	DPMI_SetSegAcc
        dw	DPMI_CreateAlias
        dw	DPMI_GetDescriptor
        dw	DPMI_SetDescriptor
        dw	DPMI_GetRInterrupt
        dw	DPMI_SetRInterrupt
        dw	DPMI_GetXVector
        dw	DPMI_SetXVector
        dw	DPMI_GetPInterrupt
        dw	DPMI_SetPInterrupt
        dw	DPMI_pSimulate_RFrame
        dw	DPMI_pSimulate_RFrame
        dw	DPMI_pSimulate_RFrame
        dw	DPMI_AllocRMCB
        dw	DPMI_FreeRMCB
        dw	DPMI_GetVersion
        dw	DPMI_pGetMemory
        dw	DPMI_pAllocMemory
        dw	DPMI_pFreeMemory
        dw	DPMI_pResizeMemory
        dw	pDPMI_Quit
        dw	pDPMI_Quit
        dw	DPMI_GetPageSize
        dw	DPMI_PhysicalMap
        dw	DPMI_DisableInts
        dw	DPMI_EnableInts
        dw	DPMI_GetInts

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	PM DPMI aid functions
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	_alloc_LDT_sel
        mov	ax,MaxAvSel
        sub	ax,cx
        jc	alloc_fail
        xor	di,di
    test_space:
        mov	bx,di
        mov	dx,cx
    test_frees:
        test	byte [LDT+bx+access],mask_P
        jnz	advance
        add	bx,byte descr_size
        dec	dx
        jnz	test_frees
        mov	bx,di
        mov	dx,cx
    alloc_sel:
        mov	dword [LDT+bx],0
        mov	dword [LDT+bx+4],9200h
        add	bx,byte descr_size
        dec	dx
        jnz	alloc_sel
        or	di,byte 4
        clc
        ret
    advance:
        add	di,byte descr_size
        dec	ax
        jnl	test_space
    alloc_fail:
        stc
        ret
endp

proc	near,	_free_LDT_sel
        push	bx
        call	_verify_sel
        jc	free_fail
        mov	dword [LDT+bx],0
        and	dword [LDT+bx+4],7f00h
    free_fail:
        pop	bx
        ret
endp

proc	near,	_verify_sel
        mov	al,bl
        and	al,7
        cmp	al,4
        jne	verify_fail
        and	bl,~7
        mov	ax,bx
        shr	ax,3
        cmp	ax,MaxAvSel
        jae	verify_fail
        test	byte [LDT+bx+access],mask_S
        jz	verify_fail
        ret
    verify_fail:
        stc
        ret
endp

proc	near,	_get_descriptor
        test	bl,100b
        jz	sel_in_GDT
    sel_in_LDT:
        call	_verify_sel
        jc	rights_fail
        lea	si,[LDT+bx]
        ret
    sel_in_GDT:
        test	bl,11b
        jnz	rights_fail
        cmp	bx,byte (Real_C_desc - GDT)
        jb	rights_fail
        cmp	bx,byte (Flat_C_desc - GDT)
        ja	rights_fail
        lea	si,[GDT+bx]
        clc
        ret
    rights_fail:
        stc
        ret
endp

proc	near,	_test_access
        test	cx,(mask_NUL << 8) | mask_DPL
        jnz	access_bad
        test	cl,mask_P | mask_S
        jz	access_bad
        jpo	access_bad
        test	cl,8
        jz	access_ok
        test	cl,4
        jnz	access_bad
        test	cl,2
        jz	access_bad
    access_ok:
        and	ch,~mask_L2
        ret
    access_bad:
        stc
        ret
endp

proc	near,	_hook_hw
        push	bx,cx,edx
    ;Test if IRQ
        call	_irqno
        jc	hook_hw_done
        mov	di,bx
        shl	di,2
    ;Get IRQ RMCB structure address
        imul	ax,bp,byte rmcb_size
        mov	si,cs
        xchg	ax,si
    ;!@# Hack alert {
    ;	Call-back for int23 is never released
    ;	The restoring is done by reset_hwints
        cmp	bl,23h
        je	hook_hw
    ;!@# Hack alert }
        cmp	cx,ax
        je	unhook_hw
    hook_hw:
    ;If already hooked, setup Call-back without hooking Call-back
        mov	dword [rmcbs+si+cbCode+Ofs32b],edx
        mov	word [rmcbs+si+cbCode+Sel16b],cx
    ;Test if Call-back available, hook Call-back
        cmp	byte [rmcbs+si+cbType],0
        jne	hook_hw_done
    ;Setup Call-back structure
        mov	byte [rmcbs+si+cbType],1
    ;Save old real-int vector
        mov	eax,dword [fs:di]
        mov	si,bp
        shl	si,2
        mov	dword [hwint_rmcb+si],eax
    ;Hook real-int
        imul	ax,bp,byte (RMCB1 - RMCB0)
        add	ax,RMCB0
        mov	word [fs:di+Ofs16b],ax
        mov	word [fs:di+Seg16b],_REAL
    hook_hw_done:
        pop	edx,cx,bx
        ret
    unhook_hw:
    ;If not call-backed, don't reset real-int
        cmp	byte [rmcbs+si+cbType],0
        je	hook_hw_done
    ;Reset real IVT
        mov	byte [rmcbs+si+cbType],0
        mov	si,bp
        shl	si,2
        xor	eax,eax
        xchg	eax,dword [hwint_rmcb+si]
        mov	dword [fs:di],eax
        jmps	hook_hw_done
endp

proc	near,	_irqno
        mov	ax,bx
        mov	bp,ax		;IRQ number
        and	ax,0f8h

        cmp	ax,word [MasterPIC]
        jne	test_slavepic
        sub	bp,ax
        clc
        ret
    test_slavepic:
        cmp	ax,word [SlavePIC]
        jne	test_fake_irq
        sub	bp,ax
        add	bp,byte Num_IRQ/2
        clc
        ret
    test_fake_irq:
        cmp	bx,byte 1ch
        jne	test_fake_irq2
        mov	bp,Num_IRQ
        ret
    test_fake_irq2:
        cmp	bx,byte 23h
        jne	no_hardirq
        mov	bp,Num_IRQ+1
        ret
    no_hardirq:
        stc
        ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Exception ints
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
        %assign	x 0
        %rep	Num_Exc
            MkXInt	x
            %assign	x x+1
        %endrep
xInterrupt_ID:
        cli
        pushad
        pushf
        push	ds
        mov	ax,Real_D_desc - GDT
        mov	ds,ax

        mov	ax,word [esp+2*2+8*4]
        sub	ax,xInterrupt1
        mov	cl,xInterrupt1 - xInterrupt0
        div	cl
        mov	bx,ax

        cmp	bl,02h				;NMI passed down
        je	near Go_interrupt
%ifndef	EXC0f
        cmp	bl,0fh
        je	near Go_interrupt
%endif

        mov	ch,al
        mov	cl,al
        and	cx,0f807h
        mov	dx,020h
        cmp	ch,byte [MasterPIC]
        je	Check_hw
        mov	dx,0a0h
        cmp	ch,byte [SlavePIC]
        jne	No_hw_int
    Check_hw:
        mov	al,0bh
        out	dx,al
        jmps	$+2
        in	al,dx
        inc	cl
        shr	al,cl
        jc	Go_interrupt

    No_hw_int:
        mov	edi,dword [esp+2*2+8*4+2]	;Load EIP/error code
        xor	bp,bp

        cmp	ch,8			;0..7,16 have no error
        jne	Check_sw_int
        cmp	bx,byte 9		;9 has no error
        je	Check_sw_int
        cmp	bx,byte 0fh		;15 has no error
        je	Check_sw_int

    ;3rd dword on user stack resembles a valid code selector?
        mov	dx,bx
        mov	bx,word [esp+2*2+8*4+2+8]
        call	_get_descriptor
        mov	bx,dx
        jc	Go_interrupt
        test	byte [si+access],8
        jz	Go_interrupt

        dec	bp
    Go_exception:
        cmp	bl,07h
        jne	Do_exception
        smsw	ax
        test	al,1000b
        jz	Do_exception
        clts
        o32	POP ds		;also discard FLAGS
        popad
        add	esp,byte 2
Do_userback:
        iretd

    Check_sw_int:
        cmp	edi,byte 2
        jb	Go_exception
    ;!@# Hack alert {
    ;	RF set only for faults, while INTnn is a trap
        test	byte [esp+2*2+8*4+2+10],1
        jnz	Go_exception
    ;!@# Hack alert }
        mov	ah,bl
        mov	al,0cdh
        mov	dx,ds
        mov	ds,word [esp+2*2+8*4+2+4]
        cmp	ax,word [edi-2]
        mov	ds,dx
        jne	Go_exception
    Go_interrupt:
        imul	bx,byte Addr32b_size
        mov	eax,dword [Exc_Ints+bx+Ofs32b]
        mov	dword [tmp_ivec+Ofs32b],eax
        mov	ax,word [Exc_Ints+bx+Sel16b]
        mov	word [tmp_ivec+Sel16b],ax

        pop	ds
        popf
        popad
        lea	esp,[esp+2]
        jmp	dword far [cs:tmp_ivec]

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Exceptions
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
Do_exception:
        pop	word [tss2+tssDs]
        pop	ax
        pop	dword [tss2+tssEdi]
        pop	dword [tss2+tssEsi]
        pop	dword [tss2+tssEbp]
        pop	eax
        pop	dword [tss2+tssEbx]
        pop	dword [tss2+tssEdx]
        pop	dword [tss2+tssEcx]
        pop	dword [tss2+tssEax]
        pop	ax
        xor	edi,edi
        test	bp,bp
        jz	exc_noerr
        pop	edi
    exc_noerr:
        pop	dword [tss2+tssEip]
        pop	word [tss2+tssCs]
        pop	ax
        pop	dword [tss2+tssEfl]
        mov	dword [tss2+tssEsp],esp
        mov	word [tss2+tssSs],ss
        str	ax

    exception_processor:
    ;Switch to a safe area in locked stack (and correct it for reentrancy)
        mov	cx,Locked_desc - GDT
        mov	ss,cx
        mov	esp,dword [locked_top]
        sub	dword [locked_top],Locked_frame

    ;Build stack like DPMI says
    ;!@# Hack alert {
    ;	According to DPMI, this is a reserved
    ;	field and should not be modified by user
    ;	We store here the exception number, just in
    ;	case we are terminating after nested exceptions
        push	bx
    ;!@# Hack alert }
        push	word [tss2+tssSs]
        push	dword [tss2+tssEsp]
        push	dword [tss2+tssEfl]
        push	ax
        push	word [tss2+tssCs]
        push	dword [tss2+tssEip]
        push	edi
        o32	PUSH cs
        push	dword Exception_Back2User

    ;Build exception jump frame (RETF-based)
        jz	Internal_exception
        cmp	ax,TSS2_desc - GDT
        je	Exception_Chain
    Internal_exception:
    ;The locked stack is exhausted or internal exception
        o32	PUSH cs
        push	dword Exception_Terminate
        jmp	Exception_Go
    Exception_Chain:
        imul	bx,byte Addr32b_size
        push	dword [Exc_Excs+bx+Sel16b]
        push	dword [Exc_Excs+bx+Ofs32b]
        jmp	Exception_Go

Exception_Back2User:
        push	ds
        push	byte (Real_D_desc - GDT)
        pop	ds
        call	to_user
        mov	word [tss2+tssEs],es
        mov	word [tss2+tssFs],fs
        mov	word [tss2+tssGs],gs
        pop	word [tss2+tssDs]
        pop	eax
        pop	dword [tss2+tssEip]
        pop	word [tss2+tssCs]
        pop	ax
        pop	dword [tss2+tssEfl]
        pop	dword [tss2+tssEsp]
        pop	word [tss2+tssSs]
        pop	ax
        add	dword [locked_top],Locked_frame
    ;Task switch: we can't touch the user stack
        and	byte [TSS2_desc+access],0fdh
        mov	ax,TSS8_desc - GDT
        ltr	ax
        mov	ss,word [tss8+tssSs]
        mov	esp,dword [tss8+tssEsp]
        mov	ebp,esp
        push	ds,ds,ds
        pop	es,fs,gs
        jmpt	TSS2_desc - GDT
Double_fault:
        and	byte [TSS8_desc+access],0fdh
        and	byte [TSS2_desc+access],0fdh
        mov	ax,TSS2_desc - GDT
        ltr	ax
        mov	es,word [tss2+tssEs]
        mov	fs,word [tss2+tssFs]
        mov	gs,word [tss2+tssGs]
        cmp	ebp,esp
        jne	tss8_exc08
        mov	ss,word [tss2+tssSs]
        mov	esp,dword [tss2+tssEsp]
        push	dword [tss2+tssEfl]
        push	dword [tss2+tssCs]
        push	dword [tss2+tssEip]
        push	dword [Exc_Ints+8*Addr32b_size+Sel16b]
        push	dword [Exc_Ints+8*Addr32b_size+Ofs16b]
    Exception_Go:
        mov	eax,dword [tss2+tssEfl]
        and	ah,3ch
        push	eax
        popfd
        call	from_user
        mov	ds,word [tss2+tssDs]
        retfd
    tss8_exc08:
        mov	bx,8
        pop	edi
        mov	ax,word [tss8+tssBack]
        jmp	exception_processor

Exception_Terminate:
        add	esp,byte 8		;Discard return address
        push	ds
        push	byte (Real_D_desc - GDT)
        pop	ds
        mov	dword [ER+eEax],eax
        mov	dword [ER+eEbx],ebx
        mov	dword [ER+eEcx],ecx
        mov	dword [ER+eEdx],edx
        mov	dword [ER+eEsi],esi
        mov	dword [ER+eEdi],edi
        mov	dword [ER+eEbp],ebp
        mov	word [ER+eEs],es
        mov	word [ER+eFs],fs
        mov	word [ER+eGs],gs
        pop	word [ER+eDs]

        pop	word [ER+eErr]
        pop	ax
        pop	dword [ER+eEip]
        pop	word [ER+eCs]
        pop	ax
        pop	dword [ER+eEfl]
        pop	dword [ER+eEsp]
        pop	word [ER+eSs]
        pop	ax

        cmp	byte [in_rmcb],0
        je	exc_not_in_rmcb
        mov	word [error_message2],_IN_RMCB_
    exc_not_in_rmcb:

        mov	bx,ax
        shl	bx,1
        mov	dx,word [En+bx]
        mov	word [error_message],dx
        mov	edx,cr2
        mov	dword [ER+eCR2],edx

        add	al,0e0h
        mov	byte [terminate],-1
        jmpt	TSS1_desc - GDT

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Miscellaneous procedures
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Get CPU type
;		Out:	DX	= (2=808x/8018x/286, 3=386, 4=486 etc.)
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	detect_cpu
        mov	dx,2
        pushf
        pop	ax
        mov	cx,ax
        xor	ah,40h
        push	ax
        popf
        pushf
        pop	ax
        push	cx
        popf
        xor	ah,ch
        jz	.ret_cpu
        inc	dx
        mov	bx,sp
        and	sp,byte ~3
        pushfd
        pop	eax
        mov	ecx,eax
        btc	eax,18
        push	eax
        popfd
        pushfd
        pop	eax
        push	ecx
        popfd
        mov	sp,bx
        xor	eax,ecx
        jz	.ret_cpu
        inc	dx
        mov	eax,ecx
        btc	eax,21
        push	eax
        popfd
        pushfd
        pop	eax
        push	ecx
        popfd
        xor	eax,ecx
        jz	.ret_cpu
        inc	dx
        xor	eax,eax
        cpuid
        test	eax,eax
        jz	.ret_cpu
        xor	eax,eax
        inc	ax
        cpuid
        mov	dl,ah
        and	dx,byte 0fh
    .ret_cpu:
        ret
endp

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Print formatted text
;		In:	DS:SI	= format
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
proc	near,	print_f
        pushad
        cld

        mov	di,si
    loop0:
        inc	di
        cmp	byte [di-1],0
        jne	loop0

    read_ch:
        lodsb
        or	al,al
        jz	end_string
        cmp	al,'%'
        je	chk_format
    write_ch:
        mov	ah,0eh
        int	10h
        jmps	read_ch
    end_string:
        popad
        ret

    chk_format:
        lodsb
        cmp	al,'%'
        je	write_ch
        cmp	al,'s'
        je	write_string
        cmp	al,'w'
        je	write_word
        cmp	al,'d'
        je	write_double
        dec	si
        dec	si
        lodsb
        jmps	write_ch

    write_string:
        push	si
        mov	si,word [di]
        call	print_f
        pop	si
        inc	di
        inc	di
        jmps	read_ch
    write_word:
        mov	bl,16
        mov	cx,2
        mov	bp,4
        movzx	eax,word [di]
        jmps	write_number
    write_double:
        mov	bl,16
        mov	cx,4
        mov	bp,8
        mov	eax,dword [di]
        jmps	write_number

write_number:
        movzx	ebx,bl
        add	di,cx
        xor	cx,cx

    divide_number:
        xor	edx,edx
        div	ebx
        cmp	dl,09h
        jbe	digit_ok
        add	dl,'A'-'9'-1
    digit_ok:
        add	dx,0e30h
        push	dx
        inc	cx
        test	eax,eax
        jnz	divide_number

        sub	bp,cx
        jle	display_number
        mov	ax,0e30h
    make_pad:
        int	10h
        dec	bp
        jnz	make_pad
    display_number:
        pop	ax
        int	10h
        loop	display_number

        jmp	read_ch
endp

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	The loader
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
proc	near,	load_exe
        pushad
        push	es
        mov	es,word [PSP_Segment]
    ;Allocate a transferbuffer
        mov	bx,word [stubinfo_ds_segment]
        mov	ax,es
        sub	bx,ax

        mov	ax,word [stubinfo_minkeep]
        shr	ax,4
        add	bx,ax
        mov	ah,4ah
        int	21h
        jc	near _need_mem
    ;Scan environment for the stub's full name after environment
        push	ds
        mov	es,word [es:2ch]
        xor	di,di
        mov	cx,-1
        xor	al,al
    scan_environment:
        repne	scasb
        scasb
        jne	scan_environment
        scasw
        push	di
        repne	scasb
        mov	word [stubinfo_env_size],di
        mov	word [Env_desc+L1],di
%if	SYMLINK
        cmp	byte [stubinfo_basename+0],0
        je	no_symlink
        pop	si
        push	es
        pop	ds
        push	ss
        pop	es
    back_scan:
        dec	di
        mov	al,byte [di]
        cmp	al,'/'
        je	copy_path
        cmp	al,'\'
        je	copy_path
        cmp	al,':'
        jne	back_scan
    copy_path:
        mov	cx,di
        sub	cx,si
        inc	cx
        xor	di,di
        push	di
        rep	movsb
        mov	cx,8
        mov	si,stubinfo_basename
    copy_basename:
        cs	lodsb
        test	al,al
        jz	end_basename
        stosb
        loop	copy_basename
    end_basename:
        mov	eax,'.EXE'
        stosd
        xor	al,al
        stosb
    no_symlink:
%endif
    ;Open the file
        push	es
        pop	ds
        pop	dx
        mov	ax,3d00h
        int	21h
        pop	ds
%if	SYMLINK
        go	c,_need_img
%endif
        mov	word [exeFile],ax
        mov	bx,ax
    ;Compute our length
        mov	ah,3fh
        mov	cx,6
        mov	dx,exeHeader
        int	21h
        jump	c,_need_img
        cmp	ax,cx
        jump	ne,_need_img
%if	SYMLINK
        xor	edx,edx
        cmp	word [exeHeader+0],'MZ'
        jne	.seekimage
        mov	dx,word [exeHeader+4]
%else
        movzx	edx,word [exeHeader+4]
%endif
        movzx	eax,word [exeHeader+2]
        neg	ax
        and	ax,511
        shl	edx,9
        sub	edx,eax
    .seekimage:
        mov	dword [inExe],edx
    ;Seek for integrated image
        mov	ax,4200h
        push	edx
        pop	dx,cx
        int	21h
%if	TARGET==0
    ;Now read d3x header
        mov	ah,3fh
        mov	dx,d3x1Header
        mov	cx,d3x1_hdr_size
        int	21h
        go	c,_need_img
        cmp	cx,ax
        go	ne,_need_img
    ;Here test if D3X
        cmp	dword [d3x1Header+sign],'D3X1'
        go	ne,_need_img
    ;Save what we need
        mov	eax,dword [d3x1Header+binsize]
        add	eax,dword [d3x1Header+admsize]
        add	eax,4095
        and	ax,~4095
%elif	TARGET==1
    ;Now read coff header
        mov	ah,3fh
        mov	dx,coffHeader
        mov	cx,coffHeaderL
        int	21h
        go	c,_need_img
        cmp	cx,ax
        go	ne,_need_img
    ;Here test if COFF
        cmp	word [coffHeader+00h],014ch
        go	ne,_need_img
    ;Set some variables from coff header
        mov	eax,dword [bss_section+s_vaddr]
        add	eax,dword [bss_section+s_size]
        mov	ebx,00010001h
        cmp	eax,ebx
        jae	.f1
        mov	eax,ebx
    .f1:
        add	eax,0000ffffh
        xor	ax,ax
%elif	TARGET==2
    ;Now read le header
        mov	ah,3fh
        mov	dx,leHeader
        mov	cx,le_hdr_size
        int	21h
        go	c,_need_img
        cmp	cx,ax
        go	ne,_need_img
    ;Here test if LE
        cmp	dword [leHeader+LESignature],'LE'
        go	ne,_need_img
    ;Compute needed memory (from object table)
        mov	ax,4200h
        mov	ecx,dword [leHeader+LEObjectTable]
        add	ecx,dword [inExe]
        mov	dx,cx
        shr	ecx,16
        int	21h
        mov	esi,dword [leHeader+LEObjectEntries]
        mov	cx,obj_tbl_size
        xor	edi,edi
        mov	dx,objEntry
    .object_loop:
        mov	ah,3fh
        int	21h
        go	c,_need_img
        cmp	cx,ax
        go	ne,_need_img
        mov	ebp,dword [objEntry+OTVirtualSize]
        add	ebp,4095
        and	bp,~4095
        add	edi,ebp
        dec	esi
        jnz	.object_loop
        xchg	eax,edi
%endif
        mov	dword [stubinfo_initial_size],eax
%ifdef	I21API
    ;Set DTA
        mov	ah,1ah
        mov	dx,tmp_dta
        int	21h
%endif
        pop	es
        popad
        ret
endp

exec_bin:
        mov	word [es:2ch],Env_desc - GDT

        mov	ax,0205h
        mov	bl,23h
        mov	cx,cs
        lea	edx,[Do_userback]
        int	31h
exec_bin2:
        sti

    ;Save PSP selector in stubinfo
        mov	word [stubinfo_psp_selector],es

%ifdef	extAPI
        mov	word [host_ds],ds
        mov	word [flat_ds],fs
%endif
%ifdef	I21API
        mov	ax,0204h
        mov	bl,21h
        int	31h
        mov	dword [old21+Ofs32b],edx
        mov	word [old21+Sel16b],cx
        mov	ax,0205h
        mov	cx,cs
        lea	edx,[new21]
        int	31h
    ;DTA -> PSP:80
        mov	byte [user_dta+Ofs32b],80h
        mov	word [user_dta+Sel16b],es
%endif
%ifdef	I33API
    ;not my problem if mouse not installed
        mov	ax,0204h
        mov	bl,33h
        int	31h
        mov	dword [old33+Ofs32b],edx
        mov	word [old33+Sel16b],cx
        mov	ax,0205h
        mov	cx,cs
        lea	edx,[new33]
        int	31h
    ;allocate call-back
        push	ds
        mov	ax,0303h
        push	ds
        pop	es
        lea	edi,[mouse_dr]
        push	cs
        pop	ds
        lea	esi,[_mouse]
        int	31h
        pop	ds
        go	c,_need_dpmi
        mov	word [mouse_cb+Ofs16b],dx
        mov	word [mouse_cb+Seg16b],cx
%endif

    ;Allocate DPMI memory for program
        mov	ax,0501h
        mov	bx,word [stubinfo_initial_size+2]
        mov	cx,word [stubinfo_initial_size]
        int	31h
        jump	c,_need_mem2

        mov	word [client_memory+2],bx
        mov	word [client_memory],cx
        mov	word [stubinfo_memory_handle+2],si
        mov	word [stubinfo_memory_handle],di

    ;Allocate 4 selectors (2 for stubinfo + 2 for program)
        xor	ax,ax
%if	TARGET<2
        mov	cx,4
%else
        mov	cx,1
%endif
        int	31h
        jump	c,_need_dpmi
        mov	bx,ax
%if	TARGET<2
        mov	ebp,dword [client_memory]
        mov	esi,dword [stubinfo_initial_size]
        mov	di,0c09ah
        call	setup_selector
        mov	word [client_cs],bx
        mov	ax,0003h
        int	31h
        add	bx,ax
        mov	ebp,dword [client_memory]
        mov	esi,dword [stubinfo_initial_size]
        mov	di,0c092h
        call	setup_selector
        mov	word [client_ds],bx
        mov	ax,0003h
        int	31h
        add	bx,ax
        movzx	ebp,word [stubinfo_ds_segment]
        shl	ebp,4
        movzx	esi,word [stubinfo_minkeep]
        mov	di,009ah
        call	setup_selector
        mov	word [stubinfo_cs_selector],bx
        mov	ax,0003h
        int	31h
        add	bx,ax
%else
        mov	word [client_cs],gs
        mov	word [client_ds],fs
%endif
        movzx	ebp,word [stubinfo_ds_segment]
        shl	ebp,4
        movzx	esi,word [stubinfo_minkeep]
        mov	di,4092h
        call	setup_selector
        mov	word [stubinfo_ds_selector],bx

    ;Load the program data
%if	TARGET==0
        mov	esi,dword [d3x1Header+hdrsize]
        add	esi,dword [inExe]
        xor	edi,edi
        mov	ecx,dword [d3x1Header+binsize]
        call	read_section

        mov	es,word [client_ds]
        mov	edi,dword [d3x1Header+binsize]
        mov	ecx,dword [d3x1Header+admsize]
        xor	al,al
        rep	a32 stosb
%elif	TARGET==1
        mov	esi,dword [text_section+s_scnptr]
        add	esi,dword [inExe]
        mov	edi,dword [text_section+s_vaddr]
        mov	ecx,dword [text_section+s_size]
        call	read_section

        mov	esi,dword [data_section+s_scnptr]
        add	esi,dword [inExe]
        mov	edi,dword [data_section+s_vaddr]
        mov	ecx,dword [data_section+s_size]
        call	read_section

        mov	es,word [client_ds]
        mov	edi,dword [bss_section+s_vaddr]
        mov	ecx,dword [bss_section+s_size]
        xor	eax,eax
        shr	ecx,2
        rep	a32 stosd
%elif	TARGET==2
    ;read loader
        mov	ebp,dword [leHeader+LELoaderSize]
        mov	ax,0501h
        push	ebp
        pop	cx,bx
        int	31h
        go	c,_need_mem2
        push	si,di
        push	bx,cx
        pop	eax
        mov	ecx,ebp
        mov	edi,eax
        mov	esi,dword [leHeader+LEObjectTable]
        sub	eax,esi
        add	dword [leHeader+LEObjectTable],eax
        add	dword [leHeader+LEObjectPMTable],eax
        add	dword [leHeader+LEFixupPageTab],eax
        add	dword [leHeader+LEFixupRecords],eax
        add	esi,dword [inExe]
        call	read_section

    ;zero out the memory
        push	fs
        pop	es
        mov	edi,dword [client_memory]
        mov	ecx,dword [stubinfo_initial_size]
        shr	ecx,2
        xor	eax,eax
        rep	a32 stosd

    ;start processing objects: hope the header is OK for ESI !@#
        mov	esi,dword [leHeader+LEDataPages]
        mov	edi,dword [client_memory]
        mov	ebp,dword [leHeader+LEObjectEntries]
        mov	ebx,dword [leHeader+LENumberPages]
        mov	edx,dword [leHeader+LEObjectTable]
    ;esi = file pointer
    ;edi = relocation base (where to load)
    ;ebp = # of objects in file
    ;ebx = # of pages in file
    ;edx -> ObjectTable
    load_object:
        mov	ecx,dword [fs:edx+OTPageMapSize]
        xor	eax,eax
    ;zero, because we store alias selector
        xchg	eax,dword [fs:edx+OTPageMapIndex]
        shl	eax,2
        add	eax,dword [leHeader+LEObjectPMTable]
    ;use reserved field to store actual object load address
        mov	dword [fs:edx+OTReserved],edi
        push	edi			; store load address
        jecxz	next_object
    readpage:
        push	ecx
        mov	ecx,dword [leHeader+LEPageSize]
        cmp	ebx,byte 1
        ja	notlastpage
        mov	ecx,dword [leHeader+LEBytesLastPage]
    notlastpage:
    ;check against Object Page Table (load or relocation only)
        cmp	byte [fs:eax+OPMTType-4],0
        jnz	afterread
        pushad
        mov	bx,word [exeFile]
        call	read_section
        popad
    afterread:
        add	esi,ecx
        add	edi,ecx
        add	eax,byte opm_tbl_size
        pop	ecx
        dec	ebx
        loop	readpage
    next_object:
        pop	edi
        mov	eax,dword [fs:edx+OTVirtualSize]	; get vsize of current section
        add	eax,4095				; align to pages
        and	ax,~4095
        add	edi,eax					; Set to start of next object
        add	edx,byte obj_tbl_size
        dec	ebp
        jnz	load_object
%endif

    ;Close the file
        mov	ah,3eh
        mov	bx,word [exeFile]
        int	21h

%if	TARGET==0
    ;Set the user stack
        mov	ss,word [client_ds]
        mov	esp,dword [d3x1Header+tos]
    ;Push entry (IRETD based) address
        push	dword 3202h
        push	dword [client_cs]
        push	dword [d3x1Header+entry]

        mov	al,byte [platform]
        mov	byte [stubinfo_platform],al
%elif	TARGET==1
    ;Use the transferbuffer as initial COFF stack: not 100% ok !@#
        mov	ss,word [stubinfo_ds_selector]
        movzx	esp,word [stubinfo_minkeep]
    ;Push entry (IRETD based) address
        push	dword 3202h
        push	dword [client_cs]
        push	dword [aoutHeader+10h]

        mov	fs,word [stubinfo_ds_selector]
%elif	TARGET==2
    ;raw data loaded, now do fixups
        mov	eax,dword [leHeader+LEObjectTable]
        mov	edi,dword [leHeader+LEFixupPageTab]
        mov	ecx,dword [leHeader+LEFixupRecords]

        push	ds,fs
        pop	ds,es
    ;Quick and dirty fix for UPX
        mov	esi,eax
    FixupObject:
        cmp	dword [eax+OTPageMapSize],byte 0
        je	near ZeroPagesObject
        mov	edx,dword [eax+OTReserved]
    FixupPage:
        push	eax
        mov	ebx,dword [edi]
        cmp	ebx,dword [edi+4]
        jnc	near FixupDone
    FixupRecord:
        mov	bp,word [ebx+ecx+FRType]
        and	bp,00efh
    ; sort of an inner loop [ebx+ecx] = start of fixup record
        push	edi
        movzx	edi,byte [ebx+ecx+FRObject]
        cmp	byte [ebx+ecx+FRType+1],10h
        jnz	check16bit
        mov	eax,dword [ebx+ecx+FRItem]
        jmps	targetselected
    check16bit:
        cmp	byte [ebx+ecx+FRType+1],0
        jnz	near _need_img
        movzx	eax,word [ebx+ecx+FRItem]
    targetselected:
        lea	edi,[edi*2+edi-3]
        lea	edi,[edi*8+esi]
        cmp	bp,byte 3
        jnz	FuNoFarCall16
        call	get_obj_sel
        shl	eax,16
        mov	ax,word [ebx+ecx+5]
        movsx	edi,word [ebx+ecx+FROffset]		;can be negative!
        mov	dword [edi+edx],eax
        jmp	OnlyOfs
    FuNoFarCall16:
        cmp	bp,byte 2
        jnz	FuNoSel16
        call	get_obj_sel
        movsx	edi,word [ebx+ecx+FROffset]		;can be negative!
        mov	word [edi+edx],ax
        movzx	edi,byte [ebx+ecx+FRType+1]
        shr	edi,3
        add	ebx,edi
        pop	edi
        add	ebx,byte 5
        cmp	ebx,dword [edi+4]
        jc	near FixupRecord
        jmps	FixupDone
    FuNoSel16:
        test	byte [ebx+ecx+FRType],10h
        jnz	DontAddOff32
        add	eax,dword [edi+OTReserved]
    DontAddOff32:
        push	edi
        movsx	edi,word [ebx+ecx+FROffset]		;can be negative!
        cmp	bp,byte 8
        jnz	NoRel16
        sub	eax,edx
        sub	eax,edi
        sub	eax,byte 4
    NoRel16:
        mov	word [edi+edx],ax
        cmp	bp,byte 5
        jz	Fu16_16
        mov	dword [edi+edx],eax
    Fu16_16:
        pop	edi
        cmp	bp,byte 7
        jz	OnlyOfs
        cmp	bp,byte 5
        jz	OnlyOfs
        cmp	bp,byte 8
        jz	OnlyOfs
        cmp	bp,byte 6
        jnz	near _need_img
        push	edi
        call	get_obj_sel
        movsx	edi,word [ebx+ecx+FROffset]		;can be negative!
        mov	word [edi+edx+4],ax
        pop	edi
    OnlyOfs:
        movzx	edi,byte [ebx+ecx+FRType+1]
        shr	edi,3
        add	ebx,edi
        pop	edi
        add	ebx,byte 7
        cmp	ebx,dword [edi+4]
        jc	near FixupRecord
    FixupDone:
        pop	eax
        add	edi,byte 4
        add	edx,dword [es:leHeader+LEPageSize]
        dec	dword [eax+OTPageMapSize]
        jnz	near FixupPage
    ZeroPagesObject:
        add	eax,byte obj_tbl_size
        dec	dword [es:leHeader+LEObjectEntries]
        jnz	near FixupObject

        mov	edi,dword [es:leHeader+LEEntrySection]
        lea	edi,[edi*2+edi-3]
        lea	edi,[edi*8+esi]
        call	get_obj_sel
        push	es
        pop	ds
        mov	cx,ax
        mov	ebx,dword [leHeader+LEEntryOffset]
        cmp	ax,word [client_cs]
        jne	entry16
        add	ebx,dword [fs:edi+OTReserved]
    entry16:

        mov	edi,dword [leHeader+LEStackSection]
        lea	edi,[edi*2+edi-3]
        lea	edi,[edi*8+esi]
        mov	eax,dword [leHeader+LEInitialESP]
        add	eax,dword [fs:edi+OTReserved]

        pop	di,si

    ;Set the user stack
        mov	ss,word [client_ds]
        mov	esp,eax
    ;Push entry (IRETD based) address
        push	dword 3202h
        push	ecx
        push	ebx

        mov	es,word [stubinfo_psp_selector]
%endif

%if	TARGET<2
    ;Copy stubinfo to transferbuffer
        mov	es,word [stubinfo_ds_selector]
        xor	di,di
        mov	si,stubinfo
        mov	cx,word [stubinfo_size]
        rep	movsb
%else
    ;free resident loader
        mov	ax,0502h
        int	31h
%endif

        mov	ds,word [client_ds]
        iretd

%if	TARGET==2
proc	near,	get_obj_sel
        test	byte [edi+OTObjectFlags+1],10h
        jnz	.alias
        mov	ax,fs
        test	byte [edi+OTObjectFlags],4		;executable?
        jz	.done
        mov	ax,gs
    .done:
        ret
    .alias:
        mov	ax,word [edi+OTPageMapIndex]
        test	ax,ax
        jnz	.done
        pushad
        xor	ax,ax
        mov	cx,1
        int	31h
        jc	near _need_dpmi
        mov	bx,ax
        mov	word [edi+OTPageMapIndex],ax
        mov	ebp,dword [edi+OTReserved]
        mov	esi,dword [edi+OTVirtualSize]
        test	byte [edi+OTObjectFlags],4		;executable?
        mov	di,8092h
        jz	.makesel
        mov	di,809ah
    .makesel:
        call	setup_selector
        popad
        jmps	.alias
endp
%endif

proc	near,	read_section
%if	TARGET==1
        mov	eax,esi
        and	eax,511
        add	ecx,eax
        and	si,~511
        and	di,~511
%endif
        mov	ebp,ecx

        mov	word [tmp_dr+dpmiFl],3202h
        mov	bx,word [exeFile]
        mov	word [tmp_dr+dpmiBx],bx
        mov	word [tmp_dr+dpmiDx],0
        mov	ax,word [stubinfo_ds_segment]
        mov	word [tmp_dr+dpmiDs],ax

        mov	ax,4200h
        push	esi
        pop	dx,cx
        int	21h

    read_loop:
        mov	byte [tmp_dr+dpmiAh],3fh
        movzx	eax,word [stubinfo_minkeep]
        cmp	eax,ebp
        jbe	read_bytes
        mov	eax,ebp
    read_bytes:
        mov	word [tmp_dr+dpmiCx],ax
        push	edi
        mov	ax,0300h
        mov	bx,21h
        xor	cx,cx
        push	ds
        pop	es
        lea	edi,[tmp_dr]
        int	31h
        pop	edi
        jc	near _need_dpmi
        test	byte [tmp_dr+dpmiFl],1
        jump	nz,_need_img
        movzx	ecx,word [tmp_dr+dpmiAx]
        cmp	cx,word [tmp_dr+dpmiCx]
        go	nz,_need_img

        sub	ebp,ecx

        movzx	esi,word [stubinfo_ds_segment]
        shl	esi,4
        mov	es,word [client_ds]
%if	TARGET==0
        rep	fs a32 movsb
%elif	TARGET==1
        shr	cx,2
        rep	fs a32 movsd
%elif	TARGET==2
        mov	al,cl
        shr	cx,2
        and	al,3
        rep	fs a32 movsd
        mov	cl,al
        rep	fs a32 movsb
%endif

        test	ebp,ebp
        jnz	read_loop

        ret
endp

proc	near,	setup_selector
        mov	ax,0007h
        push	ebp
        pop	dx,cx
        int	31h
        mov	ax,0009h
        mov	cx,cs
        shl	cx,5
        and	cx,byte mask_DPL
        or	cx,di
        int	31h
        mov	ax,0008h
        dec	esi
        push	esi
        pop	dx,cx
        int	31h
        ret
endp

%ifdef	extAPI
%include	"extapi.inc"
%endif

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Extender data area
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Messages
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
CopyRight	db	"D3X/",stubtype," DOS extender v0.90.g (alpha)",13,10
        db	"Copyright (c) 1998-2001 Borca Daniel",13,10
CopyRightL	equ	$-CopyRight

_error		db	'D3X: error 0xf',0
_386		db	'f: need 386+ CPU',0
_dos		db	'e: need DOS 3.0+',0
_mem		db	'd: no DOS memory',0
_dpmi		db	'c: DPMI host error',0
_vcpi		db	'b: VCPI server error',0
_interf		db	'a: V86 but no DPMI/VCPI',0
_a20		db	'9: A20 gate failure',0
_img		db	'8: unable to spawn image',0
_mem2		db	'7: no DPMI memory',0

E0		db	'Divide error',0
E1		db	'Debug',0
E2		db	'NMI',0
E3		db	'Breakpoint',0
E4		db	'Overflow',0
E5		db	'Bounds check',0
E6		db	'Invalid opcode',0
E7		db	'FPU unavailable',0
E8		db	'Double fault',0
E9		db	'FPU overrun',0
E10		db	'Invalid TSS',0
E11		db	'Segment not present',0
E12		db	'Stack fault',0
E13		db	'General protection fault',0
E14		db	'Page fault',0
E15		db	'#15',0
E16		db	'FPU error',0

En:
        %assign	x 0
        %rep	Num_Exc
            ExcOfs	x
            %assign	x x+1
        %endrep

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Descriptor tables
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
        align	4
GDT:
NULL_desc	istruc	descr
        iend
VCPI1_desc	istruc	descr
        iend
VCPI2_desc	istruc	descr
        iend
VCPI3_desc	istruc	descr
        iend
TSS1_desc	istruc	descr
        at	L1,	dw TSS_size-1
        at	access,	db 89h
        iend
TSS2_desc	istruc	descr
        at	L1,	dw TSS_size-1
        at	access,	db 89h
        iend
TSS8_desc	istruc	descr
        at	L1,	dw TSS_size-1
        at	access,	db 89h
        iend
LDT_desc	istruc	descr
        at	L1,	dw LDT_Length-1
        at	access,	db 82h
        iend
BIOS_desc	istruc	descr
        at	L1,	dw 2ffh
        at	B1,	dw 400h
        at	access,	db 92h
        iend
Locked_desc	istruc	descr
        at	L1,	dw Stack_Lk_sz-1
        at	access,	db 92h
        iend
Real_D_desc	istruc	descr
        at	L1,	dw -1
        at	access,	db 92h
        iend
Real_C_desc	istruc	descr
        at	L1,	dw -1
        at	access,	db 9ah
        iend
PSP_desc	istruc	descr
        at	L1,	dw 0ffh
        at	access,	db 92h
        iend
Env_desc	istruc	descr
        at	access,	db 92h
        iend
Flat_D_desc	istruc	descr
        at	L1,	dw -1
        at	access,	db 92h
        at	granted,db 0cfh
        iend
Flat_C_desc	istruc	descr
        at	L1,	dw -1
        at	access,	db 9ah
        at	granted,db 0cfh
        iend
GDT_Length	equ	$-GDT

_GDTR		dw	GDT_Length-1
        dd	0
_IDTR		dw	07ffh
        dd	0
_IDTR_		dw	03ffh
        dd	0

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	VCPI
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
VCPI		istruc	VCPI_SW
        at	vcpiLdt,dw LDT_desc - GDT
        at	vcpiT,	dw TSS1_desc - GDT
        at	vcpiEip,dd vcpi_protected
        at	vcpiCs,	dw Real_C_desc - GDT
        iend
VCPI_Entry	istruc	Addr32b
        at	Sel16b,	dw VCPI1_desc - GDT
        iend
VCPI_tHandle	dw	-1

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Interrupt engine
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
MasterPIC	dw	DefMasterPIC
SlavePIC	dw	DefSlavePIC

Exc_Ints:
        %assign	x 0
        %rep	Num_Exc
            MkRefl	x
            %assign	x x+1
        %endrep

Exc_Excs:
        %rep	Num_Exc
            istruc	Addr32b
            at	Ofs32b,	dd Exception_Terminate
            at	Sel16b,	dw Real_C_desc - GDT
            iend
        %endrep

locked_top	dd	Stack_Lk_sz

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Error exit
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
error_exit	db	'%s%s at %w:%d error=%w eflags=%d',13,10
        db	'eax=%d ebx=%d ecx=%d edx=%d esi=%d edi=%d',13,10
        db	'ebp=%d ds=%w es=%w fs=%w gs=%w stack=%w:%d cr2=%d',13,10
error_exit_end	db	0
error_message	dw	0
error_message2	dw	error_exit_end
ER		resb	excregs_size

_IN_RMCB_	db	' in RMCB',0

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	End of our data
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
        align	16			;512 for DJGPP
lastbyte:
    ;Last generated byte. All data after this point is uninitialized

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Tables (initialized by hand)
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
IDT		resb	256*gate_size		;IDT is here aligned

LDT:
        resb	MaxAvSel*descr_size
LDT_Length	equ	$-LDT

tss1		resb	TSS_size		;mode transition (main_switch)
tss2		resb	TSS_size		;application
tss8		resb	TSS_size		;double_fault and int08h

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Misc
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
A20		resb	1			;set if A20 need to be restored

terminate	resb	1
real_stack	resb	Addr16b_size
tmp_stack	resb	Addr16b_size

tmp_ivec	resb	Addr32b_size

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Platforms
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
PSP_Segment	resw	1
cpu_id		resb	1
platform	resb	1

DPMI_Entry	resb	Addr16b_size

_VCPI_		resd	1

pagedir		resd	1			;linear address of pagedirectory
pt_base		resd	1			;base of page table area
pt_top		resd	1			;top of page table area
pt_free		resd	1			;base of available page table area
pt_delta	resd	1			;# of KB for allocated pagetables

XMS_Driver	resb	Addr16b_size

raw_base	resd	1
raw_top		resd	1

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	RMCB
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
rmcbs		resb	(Num_IRQ2+Num_RMCB)*rmcb_size
hwint_rmcb	resb	Num_IRQ2*Addr16b_size

in_rmcb		resb	1

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Internal DPMI host data
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
DR		resb	DPMI_R_size

pointer_inc	resw	1

X_H		resd	MaxXHndl
%ifndef	PAGED
X_HDescr	resb	MaxXHndl*x_hndl_size
X_HDescr_bak	resb	MaxXHndl*x_hndl_size
%else
xms_handle	resd	1
page_array	resd	1
max_pages	resd	1
plast		resd	1
%endif

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Loader data area
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
inExe		resd	1
exeFile		resw	1

exeHeader	resw	3

tmp_dr		resb	DPMI_R_size
client_memory	resd	1
client_cs	resw	1
client_ds	resw	1

%if	TARGET==0
d3x1Header	resb	d3x1_hdr_size
%elif	TARGET==1
coffHeader	resb	20
aoutHeader	resb	28
text_section	resb	40
data_section	resb	40
bss_section	resb	40
coffHeaderL	equ	$-coffHeader
%elif	TARGET==2
leHeader	resb	le_hdr_size
objEntry	resb	obj_tbl_size
%endif

%ifdef	extAPI
host_ds		resw	1
flat_ds		resw	1
%endif
%ifdef	I21API
user_dta	resb	Addr32b_size
tmp_dta		resb	43
%endif
%ifdef	I33API
mouse_		resb	Addr32b_size
mouse_cb	resb	Addr16b_size
mouse_dr	resb	DPMI_R_size
%endif

lastbss:

;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;	STACK
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
segment		_LOCKED	align=16 private class=STACK
        resb	Stack_Lk_sz

segment		_STACK	align=16 stack class=STACK
        resb	Stack_16_sz
