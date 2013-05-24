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
/*
 * This file is included in this release as an alpha version.
 *
 * The only things that have been tested at this time are
 * the series 00xx,01xx,03xx functions
 *
 * Use the rest at your own risk!
 */

#ifndef __DPMI_H
#define __DPMI_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

typedef unsigned long ULONG;
typedef unsigned short UWORD;
typedef unsigned char UBYTE;
typedef UWORD SELECTOR;

typedef unsigned char DESCRIPTOR[8];

typedef struct _multiple_descriptor_ {
    UWORD sel;
    DESCRIPTOR desc;
} MULTIPLE_DESCRIPTOR;

typedef union _dpmi_regs_{
    struct {
        ULONG edi,esi,ebp,res1;
        ULONG ebx,edx,ecx,eax;
    } d;
    struct {
        UWORD di,h_di;
        UWORD si,h_si;
        UWORD bp,h_bp;
        UWORD res1,res2;
        UWORD bx,h_bx;
        UWORD dx,h_dx;
        UWORD cx,h_cx;
        UWORD ax,h_ax;
        UWORD flags;
        UWORD es,ds,fs,gs;
        UWORD ip,cs;
        UWORD sp,ss;
    } h;
    struct {
        UBYTE edi[4],esi[4],ebp[4],res1[4];
        UBYTE bl,bh,b_bh1,b_bh2;
        UBYTE dl,dh,b_dh1,b_dh2;
        UBYTE cl,ch,b_ch1,b_ch2;
        UBYTE al,ah,b_ah1,b_ah2;
    } b;
} DPMI_REGS;

typedef struct _dpmi_ver {
    UBYTE	minor,major;
    UWORD	flags;
    UBYTE	processor;
    UBYTE	master_pic;
    UBYTE	slave_pic;
} DPMI_VER;

typedef struct _dpmi_freemem_info_ {
  ULONG largest_block;
  ULONG reserved[11];
} DPMI_FREEMEM_INFO;

typedef ULONG DPMI_MEM_HANDLE;


/* group 0 */
int   _RTL_FUNC dpmi_alloc_descriptors(SELECTOR *,UWORD);
int   _RTL_FUNC dpmi_free_selector(SELECTOR);
int   _RTL_FUNC dpmi_get_sel_increment(ULONG *);
int     _RTL_FUNC dpmi_get_sel_base(ULONG *, SELECTOR);
int   _RTL_FUNC dpmi_set_sel_base(SELECTOR, ULONG);
int   _RTL_FUNC dpmi_get_sel_limit(ULONG *, SELECTOR);
int   _RTL_FUNC dpmi_set_sel_limit(SELECTOR, ULONG);
int   _RTL_FUNC dpmi_get_sel_access_rights(UWORD *, SELECTOR);
int   _RTL_FUNC dpmi_set_sel_access_rights(SELECTOR, UWORD);
int   _RTL_FUNC dpmi_get_alias_descriptor(SELECTOR *, SELECTOR);
int   _RTL_FUNC dpmi_set_descriptor(SELECTOR, DESCRIPTOR *);
int   _RTL_FUNC dpmi_get_descriptor(DESCRIPTOR *, SELECTOR);
int   _RTL_FUNC dpmi_set_multiple_descriptors(MULTIPLE_DESCRIPTOR *, UWORD);
int   _RTL_FUNC dpmi_get_multiple_descriptors(MULTIPLE_DESCRIPTOR *, UWORD);

/* group 1 */
int   _RTL_FUNC dpmi_alloc_real_memory(SELECTOR *, UWORD *, UWORD);
int   _RTL_FUNC dpmi_dealloc_real_memory(SELECTOR);
int   _RTL_FUNC dpmi_realloc_real_memory(SELECTOR, UWORD);

int   _RTL_FUNC dpmi_arbitrary_copy( SELECTOR, ULONG, SELECTOR, ULONG, ULONG);
int   _RTL_FUNC dpmi_copy_from_ds(SELECTOR,ULONG,void *, ULONG);
int   _RTL_FUNC dpmi_copy_to_ds(void *, SELECTOR, ULONG, ULONG);

/* group 2 */
int   _RTL_FUNC dpmi_get_real_interrupt(UWORD *, UWORD *, UWORD);
int   _RTL_FUNC dpmi_set_real_interrupt(UWORD,UWORD,UWORD);
int   _RTL_FUNC dpmi_get_protected_except(SELECTOR *, ULONG *, UWORD);
int   _RTL_FUNC dpmi_set_protected_except(UWORD,SELECTOR,ULONG);
int   _RTL_FUNC dpmi_get_protected_interrupt(SELECTOR *, ULONG *, UWORD);
int   _RTL_FUNC dpmi_set_protected_interrupt(UWORD,SELECTOR,ULONG);

/* group 3 */
int   _RTL_FUNC dpmi_simulate_real_interrupt(UWORD, DPMI_REGS *);
int   _RTL_FUNC dpmi_simulate_proc_far_ret_frame(DPMI_REGS *);
int   _RTL_FUNC dpmi_simulate_proc_int_frame(DPMI_REGS *);
int   _RTL_FUNC dpmi_get_real_callback(void *, void *, UWORD *, UWORD *);
int   _RTL_FUNC dpmi_free_real_callback(UWORD, UWORD);
int   _RTL_FUNC dpmi_get_state_save_restore(SELECTOR *, void *, UWORD *, UWORD *, UWORD);
int   _RTL_FUNC dpmi_call_state_save_restore(SELECTOR, void *, void *, UWORD);

/* group 4 */
int   _RTL_FUNC dpmi_get_version(DPMI_VER *);

/* group 5 */
int   _RTL_FUNC dpmi_get_memory_info( DPMI_FREEMEM_INFO *);
int   _RTL_FUNC dpmi_get_memory( ULONG *, DPMI_MEM_HANDLE *, ULONG);     
int   _RTL_FUNC dpmi_free_memory(DPMI_MEM_HANDLE);
int   _RTL_FUNC dpmi_resize_memory( ULONG *, DPMI_MEM_HANDLE *, ULONG);     
int   _RTL_FUNC dpmi_get_memory_params(ULONG *, ULONG *, DPMI_MEM_HANDLE);

/* group 9 */
int   _RTL_FUNC dpmi_enable_virtual_flag(ULONG *);
int   _RTL_FUNC dpmi_disable_virtual_flag(ULONG *);
int   _RTL_FUNC dpmi_return_virtual_flag(ULONG *);

extern SELECTOR _RTL_DATA __seg0000 ;
extern SELECTOR _RTL_DATA __seg0040 ;
extern SELECTOR _RTL_DATA __seg0050 ;
extern SELECTOR _RTL_DATA __segA000 ;
extern SELECTOR _RTL_DATA __segB000 ;
extern SELECTOR _RTL_DATA __segB800 ;

ULONG __aliasRealAddress(ULONG);

#ifdef __cplusplus
};
};
#endif

#pragma pack()

#endif /* __DPMI_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__DPMI_H_USING_LIST)
#define __DPMI_H_USING_LIST
    using std::dpmi_alloc_descriptors;
    using std::dpmi_free_selector;
    using std::dpmi_get_sel_increment;
    using std::dpmi_get_sel_base;
    using std::dpmi_set_sel_base;
    using std::dpmi_get_sel_limit;
    using std::dpmi_set_sel_limit;
    using std::dpmi_get_sel_access_rights;
    using std::dpmi_set_sel_access_rights;
    using std::dpmi_get_alias_descriptor;
    using std::dpmi_set_descriptor;
    using std::dpmi_get_descriptor;
    using std::dpmi_set_multiple_descriptors;
    using std::dpmi_get_multiple_descriptors;
    using std::dpmi_alloc_real_memory;
    using std::dpmi_dealloc_real_memory;
    using std::dpmi_realloc_real_memory;
    using std::dpmi_arbitrary_copy;
    using std::dpmi_copy_from_ds;
    using std::dpmi_copy_to_ds;
    using std::dpmi_get_real_interrupt;
    using std::dpmi_set_real_interrupt;
    using std::dpmi_get_protected_except;
    using std::dpmi_set_protected_except;
    using std::dpmi_get_protected_interrupt;
    using std::dpmi_set_protected_interrupt;
    using std::dpmi_simulate_real_interrupt;
    using std::dpmi_simulate_proc_far_ret_frame;
    using std::dpmi_simulate_proc_int_frame;
    using std::dpmi_get_real_callback;
    using std::dpmi_free_real_callback;
    using std::dpmi_get_state_save_restore;
    using std::dpmi_call_state_save_restore;
    using std::dpmi_get_version;
    using std::dpmi_get_memory_info;
    using std::dpmi_get_memory;
    using std::dpmi_free_memory;
    using std::dpmi_resize_memory;
    using std::dpmi_get_memory_params;
    using std::dpmi_enable_virtual_flag;
    using std::dpmi_disable_virtual_flag;
    using std::dpmi_return_virtual_flag;
    using std::__aliasRealAddress;
    using std::__seg0000;
    using std::__seg0040;
    using std::__seg0050;
    using std::__segA000;
    using std::__segB000;
    using std::__segB800;
    using std::ULONG;
    using std::UWORD;
    using std::UBYTE;
    using std::SELECTOR;
    using std::DESCRIPTOR;
    using std::MULTIPLE_DESCRIPTOR;
    using std::DPMI_REGS;
    using std::DPMI_VER;
    using std::DPMI_FREEMEM_INFO;
    using std::DPMI_MEM_HANDLE;
#endif
