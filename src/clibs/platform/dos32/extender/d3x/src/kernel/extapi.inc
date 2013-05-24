;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;		   D3X(tm) DOS-Extender v0.90 unleash `g' alpha
;				 INT 21h/33h API
;					
;			Copyright (c) 1998-2002 Borca Daniel
;					
;				 dborca@yahoo.com
;			 http://www.geocities.com/dborca
;²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²

proc	near,	_dpmi_int
		push	es
		pushad
		mov	ax,0300h
		xor	ecx,ecx
		mov	es,word [cs:host_ds]
		lea	edi,[tmp_dr]
		mov	word [es:edi+dpmiFl],3202h
		mov	dword [es:edi+dpmiSp],ecx
		int	31h
		jc	.0
		test	byte [es:edi+dpmiFl],1
		jz	.0
		stc
	.0:
		popad
		pop	es
		ret
endp


%ifdef	I21API
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;	DOS/4GW INT 21h API
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Dispatcher data
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
i21api_fns	db	0ffh
		db	09h,1ah,1bh,1ch,1fh,25h,2fh,32h,34h,35h,39h,3ah,3bh,3ch,3dh,3fh,40h,41h,43h,47h,48h,49h,4ah,4bh,4eh,4fh,56h,5bh,62h
		db	0

i21api_ofs:
		dw	i21api_dos4gw
		dw	i21api_displaystring
		dw	i21api_setdta
		dw	i21api_getfatinfo
		dw	i21api_getfatinfodsk
		dw	i21api_getdpb
		dw	i21api_setvector
		dw	i21api_getdta
		dw	i21api_getdpbdsk
		dw	i21api_getindos
		dw	i21api_getvector
		dw	i21api_makedir
		dw	i21api_removedir
		dw	i21api_changedir
		dw	i21api_createfile
		dw	i21api_openfile
		dw	i21api_readfile
		dw	i21api_writefile
		dw	i21api_deletefile
		dw	i21api_chmod
		dw	i21api_getpath
		dw	i21api_allocmem
		dw	i21api_freemem
		dw	i21api_resizemem
		dw	i21api_execute
		dw	i21api_findfirst
		dw	i21api_findnext
		dw	i21api_rename
		dw	i21api_newfile
		dw	i21api_getpsp

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Dispatcher
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
new21:
		cld
		and	byte [esp+8],~1
		push	word ret21
		push	ax,si,di
		mov	si,i21api_fns
		mov	di,i21api_ofs
	get_i21api:
		cs	lodsb
		test	al,al
		jz	get_i21api_out
		cmp	al,ah
		je	get_i21api_found
		add	di,byte 2
		jmps	get_i21api
	get_i21api_found:
		mov	ax,word [cs:di]
		pop	di,si
		xchg	ax,word [esp]
		RETn
	get_i21api_out:
		pop	di,si,ax
out21:
		add	esp,byte 2
		dw	0ea66h
old21		resb	Addr32b_size

ret21:
		jnc	ret21ok
		or	byte [esp+8],1
ret21ok:
		iretd

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	DOS/4GW installation check
;
;	In:	AX	ff00h
;		DX	0078h
;	Out:	AL	nonzero
;		GS	kernel selector
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_dos4gw:
		test	al,al
		jnz	out21
		cmp	dx,byte 78h
		jne	out21
		mov	ah,al
		mov	gs,ax
		dec	ax
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Display string
;
;	In:	AH	09h
;		DS:EDX	'$'-terminated string
;	Out:	none
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_displaystring:
		push	ds,es
		pushad
		push	ds
		mov	esi,edx
		mov	ds,word [cs:host_ds]
		mov	byte [tmp_dr+dpmiAh],ah
		mov	word [tmp_dr+dpmiDx],0
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],ax
		mov	es,word [stubinfo_ds_selector]
		pop	ds
	reload_chunk:
		xor	edi,edi
		mov	cx,word [cs:stubinfo_minkeep]
	copy_chunk:
		dec	cx
		jz	end_chunk
		a32	lodsb
		a32	stosb
		cmp	al,'$'
		jne	copy_chunk
	end_chunk:
		mov	al,'$'
		a32	stosb
		call	_dos_int
		jcxz	reload_chunk
		popad
		pop	es,ds
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Set DTA
;
;	In:	AH	1ah
;		DS:EDX	address of DTA
;	Out:	none
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_setdta:
		push	ds
		push	ds
		mov	ds,word [cs:host_ds]
		mov	dword [user_dta+Ofs32b],edx
		pop	word [user_dta+Sel16b]
		pop	ds
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Get FAT info
;
;	In:	AH	1bh/1ch
;		DL	drive number (only for AH==1ch)
;	Out:	DS:EBX	address of FAT ID byte
;		DX	total clusters
;		AL	sectors per cluster
;		CX	bytes per sector
;	Error:	AL	-1
;
;	Get drive parameter block
;
;	In:	AH	1fh/32h
;		DL	drive number (only for AH==32h)
;	Out:	DS:EBX	address of drive parameter block
;		AL	0
;	Error:	AL	-1
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_getfatinfo:
i21api_getfatinfodsk:
i21api_getdpb:
i21api_getdpbdsk:
		push	ds
		mov	ds,word [cs:host_ds]
		mov	byte [tmp_dr+dpmiAh],ah
		mov	byte [tmp_dr+dpmiDl],dl
		call	_dos_int
		mov	al,byte [tmp_dr+dpmiAl]
		cmp	al,-1
		je	getfat_fail
		pop	bx
		cmp	ah,1fh
		jae	get_dpb
		mov	cx,word [tmp_dr+dpmiCx]
		mov	dx,word [tmp_dr+dpmiDx]
	get_dpb:
		push	eax
		movzx	eax,word [tmp_dr+dpmiDs]
		shl	eax,4
		movzx	ebx,word [tmp_dr+dpmiBx]
		add	ebx,eax
		pop	eax
		mov	ds,word [flat_ds]
		clc
		RETn
	getfat_fail:
		pop	ds
		stc
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Set PM interrupt handler
;
;	In:	AH	25h
;		AL	interrupt number
;		DS:EDX	interrupt handler
;	Out:	none
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_setvector:
		pusha
		mov	bl,al
		mov	cx,ds
		mov	ax,0205h
		int	31h
		popa
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Get DTA
;
;	In:	AH	2fh
;	Out:	ES:EBX	address of DTA
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_getdta:
		les	ebx,[cs:user_dta]
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Get InDOS flag
;
;	In:	AH	34h
;	Out:	ES:EBX	address of DOS active flag
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_getindos:
		push	ds
		mov	ds,word [cs:host_ds]
		mov	byte [tmp_dr+dpmiAh],ah
		call	_dos_int
		push	eax
		movzx	eax,word [tmp_dr+dpmiEs]
		shl	eax,4
		movzx	ebx,word [tmp_dr+dpmiBx]
		add	ebx,eax
		pop	eax
		mov	es,word [flat_ds]
		pop	ds
		clc
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Get PM interrupt handler
;
;	In:	AH	35h
;		AL	interrupt number
;	Out:	ES:EBX	interrupt handler
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_getvector:
		push	ax,cx,edx
		mov	bl,al
		mov	ax,0204h
		int	31h
		mov	es,cx
		mov	ebx,edx
		pop	edx,cx,ax
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Make directory
;
;	In:	AH	39h
;		DS:EDX	directory name
;	Out:	none
;	Error:	EAX	error code
;
;	Remove directory
;
;	In:	AH	3ah
;		DS:EDX	directory name
;	Out:	none
;	Error:	EAX	error code
;
;	Change directory
;
;	In:	AH	3bh
;		DS:EDX	directory name
;	Out:	none
;	Error:	EAX	error code
;
;	Delete file
;
;	In:	AH	41h
;		DS:EDX	file name
;	Out:	none
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_makedir:
i21api_removedir:
i21api_changedir:
i21api_deletefile:
		push	ds
		call	_copy_down
		mov	ds,word [cs:host_ds]
		push	ax
		mov	byte [tmp_dr+dpmiAh],ah
		mov	word [tmp_dr+dpmiDx],0
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],ax
		pop	ax
		call	_dos_int
		jnc	dir_return
		movzx	eax,word [tmp_dr+dpmiAx]
	dir_return:
		pop	ds
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Create/New file
;
;	In:	AH	3ch/5bh
;		CX	file attribute
;		DS:EDX	file name
;	Out:	EAX	file handle
;	Error:	EAX	error code
;
;	Open file
;
;	In:	AH	3dh
;		AL	open mode
;		DS:EDX	file name
;	Out:	EAX	file handle
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_createfile:
i21api_openfile:
i21api_newfile:
		push	ds
		call	_copy_down
		mov	ds,word [cs:host_ds]
		mov	word [tmp_dr+dpmiAx],ax
		mov	word [tmp_dr+dpmiCx],cx
		mov	word [tmp_dr+dpmiDx],0
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],ax
		call	_dos_int
		movzx	eax,word [tmp_dr+dpmiAx]
		pop	ds
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Read from file
;
;	In:	AH	3fh
;		BX	file handle
;		ECX	number of bytes to read
;		DS:EDX	buffer to read to
;	Out:	EAX	number of bytes actually read
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_readfile:
		push	ds,es,fs
		pushad
		push	ds
		mov	ds,word [cs:stubinfo_ds_selector]
		pop	es
		mov	fs,word [cs:host_ds]
		mov	edi,edx
		xor	ebp,ebp
		mov	word [fs:tmp_dr+dpmiBx],bx
		mov	word [fs:tmp_dr+dpmiDx],bp
		mov	ax,word [fs:stubinfo_ds_segment]
		mov	word [fs:tmp_dr+dpmiDs],ax
	fread_chunk:
		jecxz	fread_ok
		movzx	eax,word [fs:stubinfo_minkeep]
		cmp	ecx,eax
		jae	fread_buffer
		mov	eax,ecx
	fread_buffer:
		mov	byte [fs:tmp_dr+dpmiAh],3fh
		mov	word [fs:tmp_dr+dpmiCx],ax
		call	_dos_int
		jc	fread_fail
		mov	ax,word [fs:tmp_dr+dpmiAx]
		push	ecx
		mov	ecx,eax
		xor	esi,esi
		rep	a32 movsb
		pop	ecx
		sub	ecx,eax
		add	ebp,eax
		cmp	ax,word [fs:tmp_dr+dpmiCx]
		je	fread_chunk
	fread_ok:
		mov	eax,ebp
		clc
	fread_return:
		mov	dword [esp+1ch],eax
		popad
		pop	fs,es,ds
		RETn
	fread_fail:
		mov	ax,word [fs:tmp_dr+dpmiAx]
		jmps	fread_return

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Write to file
;
;	In:	AH	40h
;		BX	file handle
;		ECX	number of bytes to write (0 truncates the file)
;		DS:EDX	buffer to write from
;	Out:	EAX	number of bytes actually written
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_writefile:
		push	es,fs
		pushad
		mov	es,word [cs:stubinfo_ds_selector]
		mov	fs,word [cs:host_ds]
		mov	esi,edx
		xor	ebp,ebp
		mov	word [fs:tmp_dr+dpmiBx],bx
		mov	word [fs:tmp_dr+dpmiDx],bp
		mov	ax,word [fs:stubinfo_ds_segment]
		mov	word [fs:tmp_dr+dpmiDs],ax
	fwrite_chunk:
		movzx	eax,word [fs:stubinfo_minkeep]
		cmp	ecx,eax
		jae	fwrite_buffer
		mov	eax,ecx
	fwrite_buffer:
		push	ecx
		mov	ecx,eax
		xor	edi,edi
		rep	a32 movsb
		pop	ecx
		mov	byte [fs:tmp_dr+dpmiAh],40h
		mov	word [fs:tmp_dr+dpmiCx],ax
		call	_dos_int
		jc	fwrite_fail
		mov	ax,word [fs:tmp_dr+dpmiAx]
		sub	ecx,eax
		add	ebp,eax
		jecxz	fwrite_ok
		cmp	ax,word [fs:tmp_dr+dpmiCx]
		je	fwrite_chunk
	fwrite_ok:
		mov	eax,ebp
		clc
	fwrite_return:
		mov	dword [esp+1ch],eax
		popad
		pop	fs,es
		RETn
	fwrite_fail:
		mov	ax,word [fs:tmp_dr+dpmiAx]
		jmps	fwrite_return

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Set/Query file attribute
;
;	In:	AH	43h
;		AL	0 get file attribute
;			1 set file attribute
;		CX	attribute to use (only for AL==1)
;		DS:EDX	file name
;	Out:	CX	file attribute
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_chmod:
		push	ds
		call	_copy_down
		mov	ds,word [cs:host_ds]
		push	ax
		mov	word [tmp_dr+dpmiAx],ax
		mov	word [tmp_dr+dpmiCx],cx
		mov	word [tmp_dr+dpmiDx],0
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],ax
		pop	ax
		call	_dos_int
		jc	chmod_fail
		mov	cx,word [tmp_dr+dpmiCx]
	chmod_return:
		pop	ds
		RETn
	chmod_fail:
		movzx	eax,word [tmp_dr+dpmiAx]
		jmps	chmod_return

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Get current directory
;
;	In:	AH	47h
;		DL	drive number
;		DS:ESI	buffer to write to
;	Out:	none
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_getpath:
		push	esi,edi,ds,es
		push	ds
		pop	es
		mov	edi,esi
		mov	ds,word [cs:host_ds]
		mov	byte [tmp_dr+dpmiAh],ah
		mov	byte [tmp_dr+dpmiDl],dl
		mov	word [tmp_dr+dpmiSi],0
		mov	si,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],si
		call	_dos_int
		jc	getpath_fail
		mov	ds,word [stubinfo_ds_selector]
		xor	esi,esi
		call	_copy_tozero
		clc
	getpath_return:
		pop	es,ds,edi,esi
		RETn
	getpath_fail:
		movzx	eax,word [tmp_dr+dpmiAx]
		jmps	getpath_return

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Allocate DOS block
;
;	In:	AH	48h
;		BX	number of paragraphs
;	Out:	EAX	selector
;	Error:	EAX	error code
;		EBX	size of available memory in paragraphs
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_allocmem:
		push	dx
		mov	ax,0100h
		int	31h
		jnc	allocmem_return
		movzx	ebx,bx
		mov	dx,ax
	allocmem_return:
		movzx	eax,dx
		pop	dx
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Free DOS block
;
;	In:	AH	49h
;		ES	selector
;	Out:	none
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_freemem:
		push	dx,ax
		mov	ax,0101h
		mov	dx,es
		int	31h
		jnc	freemem_return
	freemem_fail:
		pop	dx
		movzx	eax,ax
		push	ax
	freemem_return:
		pop	ax,dx
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Resize DOS block
;
;	In:	AH	4ah
;		BX	number of paragraphs
;		ES	selector
;	Out:	none
;	Error:	EAX	error code
;		EBX	size of available memory in paragraphs
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_resizemem:
		push	dx,ax
		mov	ax,0102h
		mov	dx,es
		int	31h
		jnc	freemem_return
		movzx	ebx,bx
		jmps	freemem_fail

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Execute child
;
;	In:	AX	4b00h
;		DS:EDX	program name
;		ES:EBX	exec parameter block
;	Out:	none
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_execute:
		push	ds,es,fs,gs
		pushad
	;Transferbuffer
	;	Addr	Length	Description
	;	0000h	0eh	Exec parameter block
	;	0010h		Program to execute
	;	0180h		Command line

		test	al,al
		jnz	near exec_fail1

		push	es
		pop	gs
		mov	ebp,ebx
		mov	fs,word [cs:stubinfo_ds_selector]

	;Copy the target program name
		push	fs
		pop	es
		mov	edi,10h
		mov	esi,edx
		call	_copy_tozero

	;Copy the command line
		lds	esi,[gs:ebp+6]
		mov	edi,180h
		movzx	ecx,byte [esi]
		add	cl,2
		rep	a32 movsb

	;Fill in the exec parameter block
		mov	ds,word [cs:host_ds]
		mov	word [tmp_dr+dpmiAx],ax
		mov	word [tmp_dr+dpmiDx],10h
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],ax
		mov	word [tmp_dr+dpmiEs],ax

		mov	word [fs:2],180h
		mov	word [fs:4],ax
		xor	eax,eax
		mov	word [tmp_dr+dpmiBx],ax
		mov	dword [fs:6],eax
		mov	dword [fs:0ah],eax

		les	edi,[gs:ebp]
		or	ecx,byte -1
	exec_scan_newenv:
		repne	a32 scasb
		dec	ecx
		a32	scasb
		jne	exec_scan_newenv
		not	ecx
		lea	ebx,[ecx+15]
		shr	ebx,4
		mov	ax,0100h
		int	31h
		jc	exec_fail
		push	ds
		lds	esi,[gs:ebp]
		mov	es,dx
		xor	edi,edi
		rep	a32 movsb
		pop	ds
		mov	word [fs:0],ax

		call	_dos_int
		pushf
		mov	ax,0101h
		int	31h
		popf
		mov	ax,word [tmp_dr+dpmiAx]
		jc	exec_fail

	;Reset DTA
		mov	byte [tmp_dr+dpmiAh],1ah
		mov	word [tmp_dr+dpmiDx],tmp_dta
		mov	word [tmp_dr+dpmiDs],seg tmp_dta
		call	_dos_int
	;DOS(1a) clears CF

	exec_return:
		popad
		pop	gs,fs,es,ds
		RETn
	exec_fail1:
		mov	ax,1
	exec_fail:
		movzx	eax,ax
		mov	dword [esp+1ch],eax
		stc
		jmps	exec_return

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Find first file
;
;	In:	AH	4eh
;		CX	attribute to match
;		DS:EDX	file spec
;	Out:	EAX	0
;		DTA filled
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_findfirst:
		push	ds,es
		pushad
		call	_copy_down
		mov	ds,word [cs:host_ds]
		mov	word [tmp_dr+dpmiCx],cx
		mov	word [tmp_dr+dpmiDx],0
		mov	cx,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],cx
		jmps	fnext

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Find next file
;
;	In:	AH	4fh
;		DTA	filled from a previous FindFirst
;	Out:	EAX	0
;		DTA filled
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_findnext:
		push	ds,es
		pushad
		mov	es,word [cs:host_ds]
		lea	edi,[tmp_dta]
		lds	esi,[cs:user_dta]
		mov	ecx,43
		rep	a32 movsb
		push	es
		pop	ds
	fnext:
		mov	byte [tmp_dr+dpmiAh],ah
		xor	eax,eax
		call	_dos_int
		jc	fnext_fail
		lea	esi,[tmp_dta]
		les	edi,[user_dta]
		mov	ecx,43
		rep	a32 movsb
	fnext_return:
		mov	dword [esp+1ch],eax
		popad
		pop	es,ds
		RETn
	fnext_fail:
		mov	ax,word [tmp_dr+dpmiAx]
		jmps	fnext_return

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Rename file
;
;	In:	AH	56h
;		DS:EDX	old name
;		ES:EDI	new name
;	Out:	none
;	Error:	EAX	error code
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_rename:
		push	esi,edi,ds,es
		call	_copy_down
		push	ds
		push	es
		pop	ds
		mov	esi,edi
		mov	es,word [cs:stubinfo_ds_selector]
		mov	edi,100h
		call	_copy_tozero
		pop	ds
		mov	ds,word [cs:host_ds]
		push	ax
		mov	byte [tmp_dr+dpmiAh],ah
		mov	word [tmp_dr+dpmiDx],0
		mov	word [tmp_dr+dpmiDi],100h
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiDs],ax
		mov	word [tmp_dr+dpmiEs],ax
		pop	ax
		call	_dos_int
		jnc	rename_return
		movzx	eax,word [tmp_dr+dpmiAx]
	rename_return:
		pop	es,ds,edi,esi
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Get PSP
;
;	In:	AH	62h
;	Out:	EBX	PSP selector
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i21api_getpsp:
		movzx	ebx,word [cs:stubinfo_psp_selector]
		RETn

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Aid functions
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
proc	near,	_dos_int
		push	bx
		mov	bx,21h
		call	_dpmi_int
		pop	bx
		ret
endp

proc	near,	_copy_down
		push	esi,edi,es
		mov	esi,edx
		mov	es,word [cs:stubinfo_ds_selector]
		xor	edi,edi
		call	_copy_tozero
		pop	es,edi,esi
		ret
endp

proc	near,	_copy_tozero
		push	ax,esi,edi
	_do_copy_tozero:
		a32	lodsb
		a32	stosb
		test	al,al
		jnz	_do_copy_tozero
		pop	edi,esi,ax
		ret
endp
%endif


%ifdef	I33API
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
;	DOS/4GW INT 33h API
;±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Dispatcher
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
new33:
		cld
		and	byte [esp+8],~1
		push	word ret33
		cmp	ax,09h
		je	i33api_setptr
		cmp	ax,0ch
		je	i33api_sethdlr
		cmp	ax,16h
		je	near i33api_save
		cmp	ax,17h
		je	near i33api_load
out33:
		add	esp,byte 2
		dw	0ea66h
old33		resb	Addr32b_size

ret33:
		jnc	ret33ok
		or	byte [esp+8],1
ret33ok:
		iretd

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Set graphics pointer shape
;
;	In:	AX	0009h
;		BX	pointer hot spot (horizontal)
;		CX	pointer hot spot (vertical)
;		DS:EDX	address of pointer shape
;	Out:	none
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i33api_setptr:
		push	ds,es
		pushad
		mov	ds,word [cs:host_ds]

		mov	word [tmp_dr+dpmiAx],ax
		mov	word [tmp_dr+dpmiBx],bx
		mov	word [tmp_dr+dpmiCx],cx
		mov	word [tmp_dr+dpmiDx],0
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiEs],ax

		mov	esi,edx
		xor	edi,edi
		push	es
		mov	es,word [stubinfo_ds_selector]
		pop	ds
		mov	ecx,32
		rep	a32 movsw

		call	_mouse_int
		popad
		pop	es,ds
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Set mouse event handler
;
;	In:	AX	000ch
;		CX	event mask
;		DS:EDX	event handler
;	Out:	none
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i33api_sethdlr:
		push	eax
		push	ds
		mov	ds,word [cs:host_ds]

		mov	word [tmp_dr+dpmiAx],ax
		mov	word [tmp_dr+dpmiCx],cx
		mov	eax,dword [mouse_cb]
		mov	word [tmp_dr+dpmiDx],ax
		shr	eax,16
		mov	word [tmp_dr+dpmiEs],ax

		mov	dword [mouse_],edx
		mov	word [mouse_+4],es

		call	_mouse_int
		pop	ds
		pop	eax
		RETn

_mouse:
		cld
		a32	lodsd
		mov	dword [es:edi+dpmiIp],eax
		add	word [es:edi+dpmiSp],byte 4

		push	edi,es
		movzx	eax,word [es:edi+dpmiAx]
		movzx	ebx,word [es:edi+dpmiBx]
		movzx	ecx,word [es:edi+dpmiCx]
		movzx	edx,word [es:edi+dpmiDx]
		movsx	esi,word [es:edi+dpmiSi]
		movsx	edi,word [es:edi+dpmiDi]
		call	dword far [cs:mouse_]
		pop	es,edi

		iretd

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Save driver state
;
;	In:	AX	0016h
;		DS:EDX	buffer for driver state
;	Out:	none
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i33api_save:
		push	ds
		pushad
		mov	ds,word [cs:host_ds]

		mov	word [tmp_dr+dpmiAx],ax
		mov	word [tmp_dr+dpmiDx],0
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiEs],ax
		call	_mouse_int
		jc	save_return

		mov	ax,0015h
		int	33h
		movzx	ecx,bx
		mov	ds,word [stubinfo_ds_selector]
		xor	esi,esi
		mov	edi,edx
		rep	a32 movsb

	save_return:
		popad
		pop	ds
		RETn

;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	Load driver state
;
;	In:	AX	0017h
;		DS:EDX	buffer containing state
;	Out:	none
;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
i33api_load:
		push	ds,es
		pushad
		mov	ds,word [cs:host_ds]

		mov	word [tmp_dr+dpmiAx],ax
		mov	word [tmp_dr+dpmiDx],0
		mov	ax,word [stubinfo_ds_segment]
		mov	word [tmp_dr+dpmiEs],ax

		mov	ax,0015h
		int	33h
		movzx	ecx,bx
		push	es
		mov	es,word [stubinfo_ds_selector]
		xor	edi,edi
		pop	ds
		mov	esi,edx
		rep	a32 movsb

		call	_mouse_int

		popad
		pop	es,ds
		RETn

;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
;	Aid functions
;°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±°±
proc	near,	_mouse_int
		push	bx
		mov	bx,33h
		call	_dpmi_int
		pop	bx
		ret
endp
%endif
