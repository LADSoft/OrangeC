/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#    include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned long ULONG;
    typedef unsigned short UWORD;
    typedef unsigned char UBYTE;
    typedef UWORD SELECTOR;

    typedef unsigned char DESCRIPTOR[8];

    typedef struct _multiple_descriptor_
    {
        UWORD sel;
        DESCRIPTOR desc;
    } MULTIPLE_DESCRIPTOR;

    typedef union _dpmi_regs_
    {
        struct
        {
            ULONG edi, esi, ebp, res1;
            ULONG ebx, edx, ecx, eax;
        } d;
        struct
        {
            UWORD di, h_di;
            UWORD si, h_si;
            UWORD bp, h_bp;
            UWORD res1, res2;
            UWORD bx, h_bx;
            UWORD dx, h_dx;
            UWORD cx, h_cx;
            UWORD ax, h_ax;
            UWORD flags;
            UWORD es, ds, fs, gs;
            UWORD ip, cs;
            UWORD sp, ss;
        } h;
        struct
        {
            UBYTE edi[4], esi[4], ebp[4], res1[4];
            UBYTE bl, bh, b_bh1, b_bh2;
            UBYTE dl, dh, b_dh1, b_dh2;
            UBYTE cl, ch, b_ch1, b_ch2;
            UBYTE al, ah, b_ah1, b_ah2;
        } b;
    } DPMI_REGS;

    typedef struct _dpmi_ver
    {
        UBYTE minor, major;
        UWORD flags;
        UBYTE processor;
        UBYTE master_pic;
        UBYTE slave_pic;
    } DPMI_VER;

    typedef struct _dpmi_freemem_info_
    {
        ULONG largest_block;
        ULONG reserved[11];
    } DPMI_FREEMEM_INFO;

    typedef ULONG DPMI_MEM_HANDLE;

    /* group 0 */
    int _RTL_FUNC _IMPORT dpmi_alloc_descriptors(SELECTOR*, UWORD);
    int _RTL_FUNC _IMPORT dpmi_free_selector(SELECTOR);
    int _RTL_FUNC _IMPORT dpmi_get_sel_increment(ULONG*);
    int _RTL_FUNC _IMPORT dpmi_get_sel_base(ULONG*, SELECTOR);
    int _RTL_FUNC _IMPORT dpmi_set_sel_base(SELECTOR, ULONG);
    int _RTL_FUNC _IMPORT dpmi_get_sel_limit(ULONG*, SELECTOR);
    int _RTL_FUNC _IMPORT dpmi_set_sel_limit(SELECTOR, ULONG);
    int _RTL_FUNC _IMPORT dpmi_get_sel_access_rights(UWORD*, SELECTOR);
    int _RTL_FUNC _IMPORT dpmi_set_sel_access_rights(SELECTOR, UWORD);
    int _RTL_FUNC _IMPORT dpmi_get_alias_descriptor(SELECTOR*, SELECTOR);
    int _RTL_FUNC _IMPORT dpmi_set_descriptor(SELECTOR, DESCRIPTOR*);
    int _RTL_FUNC _IMPORT dpmi_get_descriptor(DESCRIPTOR*, SELECTOR);
    int _RTL_FUNC _IMPORT dpmi_set_multiple_descriptors(MULTIPLE_DESCRIPTOR*, UWORD);
    int _RTL_FUNC _IMPORT dpmi_get_multiple_descriptors(MULTIPLE_DESCRIPTOR*, UWORD);

    /* group 1 */
    int _RTL_FUNC _IMPORT dpmi_alloc_real_memory(SELECTOR*, UWORD*, UWORD);
    int _RTL_FUNC _IMPORT dpmi_dealloc_real_memory(SELECTOR);
    int _RTL_FUNC _IMPORT dpmi_realloc_real_memory(SELECTOR, UWORD);

    int _RTL_FUNC _IMPORT dpmi_arbitrary_copy(SELECTOR, ULONG, SELECTOR, ULONG, ULONG);
    int _RTL_FUNC _IMPORT dpmi_copy_from_ds(SELECTOR, ULONG, void*, ULONG);
    int _RTL_FUNC _IMPORT dpmi_copy_to_ds(void*, SELECTOR, ULONG, ULONG);

    /* group 2 */
    int _RTL_FUNC _IMPORT dpmi_get_real_interrupt(UWORD*, UWORD*, UWORD);
    int _RTL_FUNC _IMPORT dpmi_set_real_interrupt(UWORD, UWORD, UWORD);
    int _RTL_FUNC _IMPORT dpmi_get_protected_except(SELECTOR*, ULONG*, UWORD);
    int _RTL_FUNC _IMPORT dpmi_set_protected_except(UWORD, SELECTOR, ULONG);
    int _RTL_FUNC _IMPORT dpmi_get_protected_interrupt(SELECTOR*, ULONG*, UWORD);
    int _RTL_FUNC _IMPORT dpmi_set_protected_interrupt(UWORD, SELECTOR, ULONG);

    /* group 3 */
    int _RTL_FUNC _IMPORT dpmi_simulate_real_interrupt(UWORD, DPMI_REGS*);
    int _RTL_FUNC _IMPORT dpmi_simulate_proc_far_ret_frame(DPMI_REGS*);
    int _RTL_FUNC _IMPORT dpmi_simulate_proc_int_frame(DPMI_REGS*);
    int _RTL_FUNC _IMPORT dpmi_get_real_callback(void*, void*, UWORD*, UWORD*);
    int _RTL_FUNC _IMPORT dpmi_free_real_callback(UWORD, UWORD);
    int _RTL_FUNC _IMPORT dpmi_get_state_save_restore(SELECTOR*, void*, UWORD*, UWORD*, UWORD);
    int _RTL_FUNC _IMPORT dpmi_call_state_save_restore(SELECTOR, void*, void*, UWORD);

    /* group 4 */
    int _RTL_FUNC _IMPORT dpmi_get_version(DPMI_VER*);

    /* group 5 */
    int _RTL_FUNC _IMPORT dpmi_get_memory_info(DPMI_FREEMEM_INFO*);
    int _RTL_FUNC _IMPORT dpmi_get_memory(ULONG*, DPMI_MEM_HANDLE*, ULONG);
    int _RTL_FUNC _IMPORT dpmi_free_memory(DPMI_MEM_HANDLE);
    int _RTL_FUNC _IMPORT dpmi_resize_memory(ULONG*, DPMI_MEM_HANDLE*, ULONG);
    int _RTL_FUNC _IMPORT dpmi_get_memory_params(ULONG*, ULONG*, DPMI_MEM_HANDLE);

    /* group 9 */
    int _RTL_FUNC _IMPORT dpmi_enable_virtual_flag(ULONG*);
    int _RTL_FUNC _IMPORT dpmi_disable_virtual_flag(ULONG*);
    int _RTL_FUNC _IMPORT dpmi_return_virtual_flag(ULONG*);

    extern SELECTOR _RTL_DATA __seg0000;
    extern SELECTOR _RTL_DATA __seg0040;
    extern SELECTOR _RTL_DATA __seg0050;
    extern SELECTOR _RTL_DATA __segA000;
    extern SELECTOR _RTL_DATA __segB000;
    extern SELECTOR _RTL_DATA __segB800;

    ULONG __aliasRealAddress(ULONG);

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __DPMI_H */
