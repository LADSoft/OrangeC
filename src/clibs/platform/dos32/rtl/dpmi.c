/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    asm mov cx,[num]
    asm mov ax,0
    asm int 0x31
    asm mov ecx,-1
    asm jc xout
    asm mov ecx,[basesel]
    asm mov [ecx],ax
    asm inc ecx
xout:
    asm mov eax,ecx
}
int	_RTL_FUNC dpmi_free_selector(SELECTOR sel)
{
    asm push ebx
    asm mov	bx,[sel]
    asm mov	ax,1
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_sel_increment(ULONG * rv)
{
    asm mov ax,3
    asm int 0x31
    asm mov ecx,[rv]
    asm movzx eax,ax
    asm mov [ecx],eax
    return 0;
}
int _RTL_FUNC dpmi_get_sel_base(ULONG * base, SELECTOR sel)
{
    asm push ebx
    asm mov bx,[sel]
    asm mov ax,6
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm	mov ax,cx
    asm shl eax,16
    asm mov ax,dx
    asm mov ecx,[base]
    asm mov [ecx],eax
    asm sub eax,eax
xout:
}
int	_RTL_FUNC dpmi_set_sel_base(SELECTOR sel, ULONG base)
{
    asm push ebx
    asm mov bx,[sel]
    asm mov ax,7
    asm mov ecx,[base]
    asm mov dx,cx
    asm shr ecx,16
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_sel_limit(ULONG *limit, SELECTOR sel)
{
    ULONG base;
    if (dpmi_get_sel_base(&base,sel))
        return -1;
    asm mov ax,[sel]
    asm lsl eax,eax
    asm mov ecx,[limit]
    asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_set_sel_limit(SELECTOR sel , ULONG limit)
{
    asm push ebx
    asm mov bx,[sel]
    asm mov ax,8
    asm mov ecx,[limit]
    asm mov dx,cx
    asm shr ecx,16
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_sel_access_rights(UWORD * rights, SELECTOR sel)
{
    ULONG base;
    if (dpmi_get_sel_base(&base,sel))
        return -1;
    asm mov ax,[sel]
    asm lar eax,eax
    asm mov ecx,[rights]
    asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_set_sel_access_rights(SELECTOR sel, UWORD rights)
{
    asm push ebx
    asm mov bx,[sel]
    asm mov ax,9
    asm mov cx,[rights]
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_alias_descriptor(SELECTOR *alias, SELECTOR sel)
{
    asm push ebx
    asm mov bx,[sel]
    asm mov ax,0xa
    asm int 0x31
    asm pop ebx
    asm mov ecx,-1
    asm jc xout
    asm mov ecx,[alias]
    asm mov [ecx],ax
    asm sub ecx,ecx
xout:
    asm mov eax,ecx
}
/* I haven't tested the next four */

int	_RTL_FUNC dpmi_set_descriptor(SELECTOR sel, DESCRIPTOR *desc)
{
    asm push ebx
    asm push edi
    asm mov edi,[desc]
  asm mov bx,[sel]
    asm mov ax,0xc
    asm int 0x31
    asm pop edi
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_descriptor(DESCRIPTOR *desc, SELECTOR sel)
{
    asm push ebx
    asm push edi
    asm mov edi,[desc]
  asm mov bx,[sel]
    asm mov ax,0xb
    asm int 0x31
    asm pop	edi
    asm pop	ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_set_multiple_descriptors(MULTIPLE_DESCRIPTOR *desc , UWORD count)
{
    asm push edi
    asm mov edi,[desc]
  asm mov cx,[count]
    asm mov ax,0xf
    asm int 0x31
    asm pop edi
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

int	_RTL_FUNC dpmi_get_multiple_descriptors(MULTIPLE_DESCRIPTOR *desc , UWORD count)
{
    asm push edi
    asm mov edi,[desc]
  asm mov cx,[count]
    asm mov ax,0xE
    asm int 0x31
    asm pop edi
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

/* group 1 */
int	_RTL_FUNC dpmi_alloc_real_memory(SELECTOR *sel, UWORD *para, UWORD len)
{
    asm push ebx
    asm mov bx,[len]
    asm mov ax,0x100
    asm int 0x31
    asm pop ebx
    asm mov ecx,-1
    asm jc xout
    asm mov ecx,[sel]
    asm mov [ecx],dx
    asm mov ecx,[para]
    asm mov [ecx],ax
    asm sub ecx,ecx
xout:
    asm mov eax,ecx

}

int	_RTL_FUNC dpmi_dealloc_real_memory(SELECTOR sel)
{
    asm mov dx,[sel]
    asm mov ax,0x101
    asm int 0x31
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_realloc_real_memory(SELECTOR sel, UWORD size)
{
    asm push ebx
    asm mov bx,[size]
    asm mov dx,[sel]
    asm mov ax,0x102
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

int _RTL_FUNC dpmi_arbitrary_copy( SELECTOR dstsel, ULONG dest, SELECTOR srcsel , ULONG source, ULONG len)
{
    asm push ds
    asm push es
    asm push esi
    asm push edi
    asm mov ecx,[len]
    asm mov edi,[dest]
  asm mov esi,[source]
  asm mov es,[dstsel]
  asm mov ds,[srcsel]
    asm cld
    asm rep movsb
    asm pop edi
    asm pop esi
    asm pop es
    asm pop ds
    return 0;
}
int	_RTL_FUNC dpmi_copy_from_ds(SELECTOR sel,ULONG dest,void *source, ULONG len)
{
    asm push es
    asm push esi
    asm push edi
    asm mov ecx,[len]
    asm mov edi,[dest]
  asm mov esi,[source]
  asm mov es,[sel]
    asm cld
    asm rep movsb
    asm pop edi
    asm pop esi
    asm pop es
    return 0;
}
int _RTL_FUNC dpmi_copy_to_ds(void *dest, SELECTOR sel, ULONG source, ULONG len)
{
    asm push ds
    asm push esi
    asm push edi
    asm mov ecx,[len]
    asm mov edi,[dest]
  asm mov esi,[source]
  asm mov ds,[sel]
    asm cld
    asm rep movsb
    asm pop edi
    asm pop esi
    asm pop ds
    return 0;
}

/* group 2 */
int	_RTL_FUNC dpmi_get_real_interrupt(UWORD *xseg, UWORD *ofs, UWORD num)
{
    asm push ebx
    asm mov bx,[num]
    asm mov ax,0x200
    asm int 0x31
    asm pop ebx
    asm mov ecx,-1
    asm jc xout
    asm mov eax,[xseg]
    asm mov [eax],cx
    asm mov eax,[ofs]
    asm mov [eax],dx
    asm sub ecx,ecx
xout:
    asm mov eax, ecx
}
int	_RTL_FUNC dpmi_set_real_interrupt(UWORD num ,UWORD xseg,UWORD ofs)
{
    asm push ebx
    asm mov bx,[num]
    asm mov cx,[xseg]
   asm mov dx,[ofs]
    asm mov ax,0x201
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

int	_RTL_FUNC dpmi_get_protected_except(SELECTOR *sel, ULONG *ofs, UWORD num)
{
    asm push ebx
    asm mov bx,[num]
    asm mov ax,0x202
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm mov eax,[sel]
    asm mov [eax],cx
    asm mov eax,[ofs]
    asm mov [eax],edx
    asm sub eax,eax
xout:
}

int	_RTL_FUNC dpmi_set_protected_except(UWORD num,SELECTOR sel,ULONG ofs)
{
    asm push ebx
    asm mov bx,[num]
    asm mov cx,[sel]
    asm mov edx,[ofs]
    asm mov ax,0x203
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

int	_RTL_FUNC dpmi_get_protected_interrupt(SELECTOR *sel, ULONG *ofs, UWORD num)
{
    asm push ebx
    asm mov bx,[num]
    asm mov ax,0x204
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm mov eax,[sel]
    asm mov [eax],cx
    asm mov eax,[ofs]
    asm mov [eax],edx
    asm sub eax,eax
xout:
}

int	_RTL_FUNC dpmi_set_protected_interrupt(UWORD num,SELECTOR sel,ULONG ofs)
{
    asm push ebx
    asm mov bx,[num]
    asm mov cx,[sel]
  asm mov edx,[ofs]
    asm mov ax,0x205
    asm int 0x31
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

/* group 3 */
int	_RTL_FUNC dpmi_simulate_real_interrupt(UWORD num, DPMI_REGS *val)
{
    asm push ebx
    asm push edi
    asm push esi
    asm push ebp
    asm mov bx,[num]
    asm sub bh,bh
    asm mov edi,[val]
    asm sub ecx,ecx
    asm mov ax,0x300
    asm int 0x31
    asm pop ebp
    asm pop esi
    asm pop edi
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

int	_RTL_FUNC dpmi_simulate_proc_far_ret_frame(DPMI_REGS *val)
{
    asm push ebx
    asm push edi
    asm push esi
    asm push ebp
    asm sub bx,bx
    asm mov edi,[val]
    asm sub ecx,ecx
    asm mov ax,0x301
    asm int 0x31
    asm pop ebp
    asm pop esi
    asm pop edi
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}

int	_RTL_FUNC dpmi_simulate_proc_int_frame(DPMI_REGS *val)
{
    asm push ebx
    asm push edi
    asm push esi
    asm push ebp
    asm sub bx,bx
    asm mov edi,[val]
    asm sub ecx,ecx
    asm mov ax,0x302
    asm int 0x31
    asm pop ebp
    asm pop esi
    asm pop edi
    asm pop ebx
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_real_callback(void *proc, void *buf, UWORD *xseg, UWORD *ofs)
{
    asm push esi
    asm push edi
   asm push ds
   asm push cs
   asm pop ds
    asm mov esi,[proc]
    asm mov edi,[buf]
    asm mov ax,0x303
    asm int 0x31
   asm pop ds
    asm pop edi
   asm pop esi
    asm mov eax,-1
    asm jc xout
    asm mov	eax,[xseg]
    asm mov	[eax],cx
    asm mov eax,[ofs]
    asm mov [eax],dx
    asm sub eax,eax
xout:
}
int	_RTL_FUNC dpmi_free_real_callback(UWORD xseg , UWORD ofs )
{
    asm mov cx,[xseg]
    asm mov dx,[ofs]
    asm mov ax,0x304
    asm int 0x31
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_state_save_restore(SELECTOR *csel, void *cofs, UWORD *xseg, UWORD *ofs, UWORD len)
{
    asm push esi
    asm push edi
    asm mov ax,0x305
    asm int 0x31
    asm push ebx
    asm mov bx,[len]
    asm movzx eax,ax
    asm mov [ebx],eax
    asm pop ebx
    asm mov eax,[csel]
    asm mov [eax],si
    asm mov eax,[cofs]
    asm mov [eax],edi
    asm mov eax,[xseg]
    asm mov [eax],cx
    asm mov eax,[ofs]
    asm mov [eax],dx
    asm pop edi
    asm pop esi
    return 0;
}
int	_RTL_FUNC dpmi_call_state_save_restore(SELECTOR csel, void *proc, void *buf, UWORD flag)
{
    UBYTE fcall[6];
    asm push esi
    asm mov esi,[buf]
    asm mov ax,[csel]
    asm mov word ptr [fcall + 4],ax
    asm mov eax,[proc]
    asm mov dword ptr [fcall],eax
    asm mov ax,[flag]
    asm call fword ptr [fcall]
    asm pop esi
    return 0;
    
}
int	_RTL_FUNC dpmi_get_version(DPMI_VER *ver)
{
    asm mov ax,0x400
    asm int 0x31
    asm push esi
    asm mov esi, [ver]
    asm mov [esi],ax
    asm mov [esi + 2],bx
    asm mov [esi + 4],cl
    asm mov [esi + 5],dx

    asm pop esi
    return 0;
}

/* group 5 */
int	_RTL_FUNC dpmi_get_memory_info( DPMI_FREEMEM_INFO *info)
{
    asm push edi
    asm mov edi,[info]
    asm mov ax,0x500
    asm int 0x31
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:
}
int	_RTL_FUNC dpmi_get_memory( ULONG *linear, DPMI_MEM_HANDLE *handle, ULONG size)		
{
    asm push esi
    asm push edi
    asm push ebx
    asm mov ax,0x501
    asm mov bx,word ptr [size + 2]
    asm mov cx,word ptr [size]
    asm int 0x31
    asm mov eax,-1
    asm jc xout
    asm mov eax,[linear]
    asm mov [eax],cx
    asm mov [eax+2],bx
    asm mov eax,[handle]
    asm mov [eax],di
    asm mov [eax+2],si
    asm sub eax,eax
xout:
    asm pop ebx
    asm pop edi
    asm	pop esi
}

int	_RTL_FUNC dpmi_free_memory(DPMI_MEM_HANDLE handle)
{
    asm push esi
    asm push edi
    asm mov si,word ptr [handle+2]
    asm mov di,word ptr [handle]
    asm mov ax,0x502
    asm int 0x31
    asm pop edi
    asm pop esi
    asm mov eax,-1
    asm jc xout
    asm inc eax
xout:

}
int	_RTL_FUNC dpmi_resize_memory( ULONG *linear, DPMI_MEM_HANDLE *handle, ULONG size)		
{
    asm push esi
    asm push edi
    asm push ebx
    asm mov ax,0x503
    asm mov bx,word ptr [size + 2]
    asm mov cx,word ptr [size]
    asm mov si,word ptr [handle+2]
    asm mov di,word ptr [handle]
    asm int 0x31
    asm mov eax,-1
    asm jc xout
    asm mov eax,[linear]
    asm mov [eax],cx
    asm mov [eax+2],bx
    asm mov eax,[handle]
    asm mov [eax],di
    asm mov [eax+2],si
    asm sub eax,eax
xout:
    asm pop ebx
    asm pop edi
    asm pop esi
}

int	_RTL_FUNC dpmi_get_memory_params(ULONG *base, ULONG *size, DPMI_MEM_HANDLE handle)
{
    asm push esi
    asm push edi
    asm push ebx
    asm mov si,word ptr [handle+2]
    asm mov di,word ptr [handle]
    asm mov ax,0x504
    asm int 0x31
    asm mov eax,-1
    asm jc xout
    asm mov eax,[base]
    asm mov [eax],si
    asm mov [eax+2],di
    asm mov eax,[size]
    asm mov [eax],cx
    asm mov [eax+2],bx
    asm sub eax,eax
xout:
    asm pop ebx
    asm pop edi
    asm pop esi
}
    

/* group 9 */
int	_RTL_FUNC dpmi_enable_virtual_flag(ULONG * rv)
{
  asm mov ax,0x901
    asm int 0x31
    asm movzx eax,al
  asm mov ecx,[rv]
    asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_disable_virtual_flag(ULONG * rv)
{
  asm mov ax,0x900
    asm int 0x31
    asm movzx eax,al
  asm mov ecx,[rv]
    asm mov [ecx],eax
    return 0;
}
int	_RTL_FUNC dpmi_return_virtual_flag(ULONG * rv)
{
  asm mov ax,0x902
    asm int 0x31
    asm movzx eax,al
  asm mov ecx,[rv]
    asm mov [ecx],eax
    return 0;
}