/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <dpmi.h>

/*
 * This file is included in this release as an alpha version.
 *
 * The only things that have been tested at this time are
 * the series 00xx,01xx,03xx functions
 *
 * Use the rest at your own risk!
 */

/* group 0 */
int	_RTL_FUNC dpmi_alloc_descriptors(SELECTOR * basesel,UWORD num)
{
    __asm mov cx,[num]
    __asm mov eax,0
    __asm int 0x31
    __asm mov ecx,-1
    __asm jc xout
    __asm mov ecx,[basesel]
    __asm mov [ecx],ax
    __asm inc ecx
xout:
    __asm mov eax,ecx
}
int	_RTL_FUNC dpmi_free_selector(SELECTOR sel)
{
    __asm push ebx
    __asm mov	bx,[sel]
    __asm mov	ax,1
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_sel_increment(ULONG * rv)
{
    __asm mov eax,3
    __asm int 0x31
    __asm mov ecx,[rv]
    __asm movzx eax,ax
    __asm mov [ecx],eax
    return 0;
}
int _RTL_FUNC dpmi_get_sel_base(ULONG * base, SELECTOR sel)
{
    __asm push ebx
    __asm mov bx,[sel]
    __asm mov eax,6
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm	mov ax,cx
    __asm shl eax,16
    __asm mov ax,dx
    __asm mov ecx,[base]
    __asm mov [ecx],eax
    __asm sub eax,eax
xout:
}
int	_RTL_FUNC dpmi_set_sel_base(SELECTOR sel, ULONG base)
{
    __asm push ebx
    __asm mov bx,[sel]
    __asm mov eax,7
    __asm mov ecx,[base]
    __asm mov dx,cx
    __asm shr ecx,16
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_sel_limit(ULONG *limit, SELECTOR sel)
{
    ULONG base;
    if (dpmi_get_sel_base(&base,sel))
        return -1;
    __asm mov ax,[sel]
    __asm lsl eax,eax
    __asm mov ecx,[limit]
    __asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_set_sel_limit(SELECTOR sel , ULONG limit)
{
    __asm push ebx
    __asm mov bx,[sel]
    __asm mov eax,8
    __asm mov ecx,[limit]
    __asm mov dx,cx
    __asm shr ecx,16
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_sel_access_rights(UWORD * rights, SELECTOR sel)
{
    ULONG base;
    if (dpmi_get_sel_base(&base,sel))
        return -1;
    __asm mov ax,[sel]
    __asm lar eax,eax
    __asm mov ecx,[rights]
    __asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_set_sel_access_rights(SELECTOR sel, UWORD rights)
{
    __asm push ebx
    __asm mov bx,[sel]
    __asm mov eax,9
    __asm mov cx,[rights]
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_alias_descriptor(SELECTOR *alias, SELECTOR sel)
{
    __asm push ebx
    __asm mov bx,[sel]
    __asm mov eax,0xa
    __asm int 0x31
    __asm pop ebx
    __asm mov ecx,-1
    __asm jc xout
    __asm mov ecx,[alias]
    __asm mov [ecx],ax
    __asm sub ecx,ecx
xout:
    __asm mov eax,ecx
}
/* I haven't tested the next four */

int	_RTL_FUNC dpmi_set_descriptor(SELECTOR sel, DESCRIPTOR *desc)
{
    __asm push ebx
    __asm push edi
    __asm mov edi,[desc]
  __asm mov bx,[sel]
    __asm mov eax,0xc
    __asm int 0x31
    __asm pop edi
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_descriptor(DESCRIPTOR *desc, SELECTOR sel)
{
    __asm push ebx
    __asm push edi
    __asm mov edi,[desc]
  __asm mov bx,[sel]
    __asm mov eax,0xb
    __asm int 0x31
    __asm pop	edi
    __asm pop	ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_set_multiple_descriptors(MULTIPLE_DESCRIPTOR *desc , UWORD count)
{
    __asm push edi
    __asm mov edi,[desc]
  __asm mov cx,[count]
    __asm mov eax,0xf
    __asm int 0x31
    __asm pop edi
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

int	_RTL_FUNC dpmi_get_multiple_descriptors(MULTIPLE_DESCRIPTOR *desc , UWORD count)
{
    __asm push edi
    __asm mov edi,[desc]
  __asm mov cx,[count]
    __asm mov eax,0xE
    __asm int 0x31
    __asm pop edi
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

/* group 1 */
int	_RTL_FUNC dpmi_alloc_real_memory(SELECTOR *sel, UWORD *para, UWORD len)
{
    __asm push ebx
    __asm mov bx,[len]
    __asm mov eax,0x100
    __asm int 0x31
    __asm pop ebx
    __asm mov ecx,-1
    __asm jc xout
    __asm mov ecx,[sel]
    __asm mov [ecx],dx
    __asm mov ecx,[para]
    __asm mov [ecx],ax
    __asm sub ecx,ecx
xout:
    __asm mov eax,ecx

}

int	_RTL_FUNC dpmi_dealloc_real_memory(SELECTOR sel)
{
    __asm mov dx,[sel]
    __asm mov eax,0x101
    __asm int 0x31
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_realloc_real_memory(SELECTOR sel, UWORD size)
{
    __asm push ebx
    __asm mov bx,[size]
    __asm mov dx,[sel]
    __asm mov eax,0x102
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

int _RTL_FUNC dpmi_arbitrary_copy( SELECTOR dstsel, ULONG dest, SELECTOR srcsel , ULONG source, ULONG len)
{
    __asm push ds
    __asm push es
    __asm push esi
    __asm push edi
    __asm mov ecx,[len]
    __asm mov edi,[dest]
  __asm mov esi,[source]
  __asm mov es,[dstsel]
  __asm mov ds,[srcsel]
    __asm cld
    __asm rep movsb
    __asm pop edi
    __asm pop esi
    __asm pop es
    __asm pop ds
    return 0;
}
int	_RTL_FUNC dpmi_copy_from_ds(SELECTOR sel,ULONG dest,void *source, ULONG len)
{
    __asm push es
    __asm push esi
    __asm push edi
    __asm mov ecx,[len]
    __asm mov edi,[dest]
  __asm mov esi,[source]
  __asm mov es,[sel]
    __asm cld
    __asm rep movsb
    __asm pop edi
    __asm pop esi
    __asm pop es
    return 0;
}
int _RTL_FUNC dpmi_copy_to_ds(void *dest, SELECTOR sel, ULONG source, ULONG len)
{
    __asm push ds
    __asm push esi
    __asm push edi
    __asm mov ecx,[len]
    __asm mov edi,[dest]
  __asm mov esi,[source]
  __asm mov ds,[sel]
    __asm cld
    __asm rep movsb
    __asm pop edi
    __asm pop esi
    __asm pop ds
    return 0;
}

/* group 2 */
int	_RTL_FUNC dpmi_get_real_interrupt(UWORD *xseg, UWORD *ofs, UWORD num)
{
    __asm push ebx
    __asm mov bx,[num]
    __asm mov eax,0x200
    __asm int 0x31
    __asm pop ebx
    __asm mov ecx,-1
    __asm jc xout
    __asm mov eax,[xseg]
    __asm mov [eax],cx
    __asm mov eax,[ofs]
    __asm mov [eax],dx
    __asm sub ecx,ecx
xout:
    __asm mov eax, ecx
}
int	_RTL_FUNC dpmi_set_real_interrupt(UWORD num ,UWORD xseg,UWORD ofs)
{
    __asm push ebx
    __asm mov bx,[num]
    __asm mov cx,[xseg]
   __asm mov dx,[ofs]
    __asm mov eax,0x201
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

int	_RTL_FUNC dpmi_get_protected_except(SELECTOR *sel, ULONG *ofs, UWORD num)
{
    __asm push ebx
    __asm mov bx,[num]
    __asm mov eax,0x202
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm mov eax,[sel]
    __asm mov [eax],cx
    __asm mov eax,[ofs]
    __asm mov [eax],edx
    __asm sub eax,eax
xout:
}

int	_RTL_FUNC dpmi_set_protected_except(UWORD num,SELECTOR sel,ULONG ofs)
{
    __asm push ebx
    __asm mov bx,[num]
    __asm mov cx,[sel]
    __asm mov edx,[ofs]
    __asm mov eax,0x203
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

int	_RTL_FUNC dpmi_get_protected_interrupt(SELECTOR *sel, ULONG *ofs, UWORD num)
{
    __asm push ebx
    __asm mov bx,[num]
    __asm mov eax,0x204
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm mov eax,[sel]
    __asm mov [eax],cx
    __asm mov eax,[ofs]
    __asm mov [eax],edx
    __asm sub eax,eax
xout:
}

int	_RTL_FUNC dpmi_set_protected_interrupt(UWORD num,SELECTOR sel,ULONG ofs)
{
    __asm push ebx
    __asm mov bx,[num]
    __asm mov cx,[sel]
  __asm mov edx,[ofs]
    __asm mov eax,0x205
    __asm int 0x31
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

/* group 3 */
int	_RTL_FUNC dpmi_simulate_real_interrupt(UWORD num, DPMI_REGS *val)
{
    __asm push ebx
    __asm push edi
    __asm push esi
//    __asm push ebp
    __asm mov bx,[num]
    __asm sub bh,bh
    __asm mov edi,[val]
    __asm sub ecx,ecx
    __asm mov eax,0x300
    __asm int 0x31
//    __asm pop ebp
    __asm pop esi
    __asm pop edi
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

int	_RTL_FUNC dpmi_simulate_proc_far_ret_frame(DPMI_REGS *val)
{
    __asm push ebx
    __asm push edi
    __asm push esi
//    __asm push ebp
    __asm sub bx,bx
    __asm mov edi,[val]
    __asm sub ecx,ecx
    __asm mov eax,0x301
    __asm int 0x31
//    __asm pop ebp
    __asm pop esi
    __asm pop edi
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}

int	_RTL_FUNC dpmi_simulate_proc_int_frame(DPMI_REGS *val)
{
    __asm push ebx
    __asm push edi
    __asm push esi
//    __asm push ebp
    __asm sub bx,bx
    __asm mov edi,[val]
    __asm sub ecx,ecx
    __asm mov eax,0x302
    __asm int 0x31
//    __asm pop ebp
    __asm pop esi
    __asm pop edi
    __asm pop ebx
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_real_callback(void *proc, void *buf, UWORD *xseg, UWORD *ofs)
{
    __asm push esi
    __asm push edi
   __asm push ds
   __asm push cs
   __asm pop ds
    __asm mov esi,[proc]
    __asm mov edi,[buf]
    __asm mov eax,0x303
    __asm int 0x31
   __asm pop ds
    __asm pop edi
   __asm pop esi
    __asm mov eax,-1
    __asm jc xout
    __asm mov	eax,[xseg]
    __asm mov	[eax],cx
    __asm mov eax,[ofs]
    __asm mov [eax],dx
    __asm sub eax,eax
xout:
}
int	_RTL_FUNC dpmi_free_real_callback(UWORD xseg , UWORD ofs )
{
    __asm mov cx,[xseg]
    __asm mov dx,[ofs]
    __asm mov eax,0x304
    __asm int 0x31
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_state_save_restore(SELECTOR *csel, void *cofs, UWORD *xseg, UWORD *ofs, UWORD len)
{
    __asm push esi
    __asm push edi
    __asm mov eax,0x305
    __asm int 0x31
    __asm push ebx
    __asm mov bx,[len]
    __asm movzx eax,ax
    __asm mov [ebx],eax
    __asm pop ebx
    __asm mov eax,[csel]
    __asm mov [eax],si
    __asm mov eax,[cofs]
    __asm mov [eax],edi
    __asm mov eax,[xseg]
    __asm mov [eax],cx
    __asm mov eax,[ofs]
    __asm mov [eax],dx
    __asm pop edi
    __asm pop esi
    return 0;
}
int	_RTL_FUNC dpmi_call_state_save_restore(SELECTOR csel, void *proc, void *buf, UWORD flag)
{
    UBYTE fcall[6];
    __asm push esi
    __asm mov esi,[buf]
    __asm mov ax,[csel]
    __asm mov word ptr [fcall + 4],ax
    __asm mov eax,[proc]
    __asm mov dword ptr [fcall],eax
    __asm mov ax,[flag]
    __asm call fword ptr [fcall]
    __asm pop esi
    return 0;
    
}
int	_RTL_FUNC dpmi_get_version(DPMI_VER *ver)
{
    __asm mov eax,0x400
    __asm int 0x31
    __asm push esi
    __asm mov esi, [ver]
    __asm mov [esi],ax
    __asm mov [esi + 2],bx
    __asm mov [esi + 4],cl
    __asm mov [esi + 5],dx

    __asm pop esi
    return 0;
}

/* group 5 */
int	_RTL_FUNC dpmi_get_memory_info( DPMI_FREEMEM_INFO *info)
{
    __asm push edi
    __asm mov edi,[info]
    __asm mov eax,0x500
    __asm int 0x31
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:
    __asm pop edi
}
int	_RTL_FUNC dpmi_get_memory( ULONG *linear, DPMI_MEM_HANDLE *handle, ULONG size)		
{
    __asm push esi
    __asm push edi
    __asm push ebx
    __asm mov eax,0x501
    __asm mov bx,word ptr [size + 2]
    __asm mov cx,word ptr [size]
    __asm int 0x31
    __asm mov eax,-1
    __asm jc xout
    __asm mov eax,[linear]
    __asm mov [eax],cx
    __asm mov [eax+2],bx
    __asm mov eax,[handle]
    __asm mov [eax],di
    __asm mov [eax+2],si
    __asm sub eax,eax
xout:
    __asm pop ebx
    __asm pop edi
    __asm	pop esi
}

int	_RTL_FUNC dpmi_free_memory(DPMI_MEM_HANDLE handle)
{
    __asm push esi
    __asm push edi
    __asm mov si,word ptr [handle+2]
    __asm mov di,word ptr [handle]
    __asm mov eax,0x502
    __asm int 0x31
    __asm pop edi
    __asm pop esi
    __asm mov eax,-1
    __asm jc xout
    __asm inc eax
xout:

}
int	_RTL_FUNC dpmi_resize_memory( ULONG *linear, DPMI_MEM_HANDLE *handle, ULONG size)		
{
    __asm push esi
    __asm push edi
    __asm push ebx
    __asm mov eax,0x503
    __asm mov bx,word ptr [size + 2]
    __asm mov cx,word ptr [size]
    __asm mov si,word ptr [handle+2]
    __asm mov di,word ptr [handle]
    __asm int 0x31
    __asm mov eax,-1
    __asm jc xout
    __asm mov eax,[linear]
    __asm mov [eax],cx
    __asm mov [eax+2],bx
    __asm mov eax,[handle]
    __asm mov [eax],di
    __asm mov [eax+2],si
    __asm sub eax,eax
xout:
    __asm pop ebx
    __asm pop edi
    __asm pop esi
}

int	_RTL_FUNC dpmi_get_memory_params(ULONG *base, ULONG *size, DPMI_MEM_HANDLE handle)
{
    __asm push esi
    __asm push edi
    __asm push ebx
    __asm mov si,word ptr [handle+2]
    __asm mov di,word ptr [handle]
    __asm mov eax,0x504
    __asm int 0x31
    __asm mov eax,-1
    __asm jc xout
    __asm mov eax,[base]
    __asm mov [eax],si
    __asm mov [eax+2],di
    __asm mov eax,[size]
    __asm mov [eax],cx
    __asm mov [eax+2],bx
    __asm sub eax,eax
xout:
    __asm pop ebx
    __asm pop edi
    __asm pop esi
}
    

/* group 9 */
int	_RTL_FUNC dpmi_enable_virtual_flag(ULONG * rv)
{
  __asm mov eax,0x901
    __asm int 0x31
    __asm movzx eax,al
  __asm mov ecx,[rv]
    __asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_disable_virtual_flag(ULONG * rv)
{
  __asm mov eax,0x900
    __asm int 0x31
    __asm movzx eax,al
  __asm mov ecx,[rv]
    __asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_return_virtual_flag(ULONG * rv)
{
  __asm mov eax,0x902
    __asm int 0x31
    __asm movzx eax,al
  __asm mov ecx,[rv]
    __asm mov [ecx],eax
    return 0;
}