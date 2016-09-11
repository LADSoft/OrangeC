/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"


/*
 *      code generation structures and constants
 */

/* address mode specifications */

#define MAX_SEGS browseseg + 1 

enum e_op
{

    op_reserved, op_line, op_blockstart, op_blockend, op_varstart, op_funcstart, op_funcend, op_void, 
    op_comment, op_label, op_funclabel, op_seq, op_methodheader, op_methodtrailer,
    op_genword, op_dd, op_maxstack, op_entrypoint, op_locals, op_declfield,
    op_add,op_add_ovf,op_add_ovf_un,op_and,op_arglist,op_beq,op_beq_s,op_bge,
	op_bge_s,op_bge_un,op_bge_un_s,op_bgt,op_bgt_s,op_bgt_un,op_bgt_un_s,op_ble,
	op_ble_s,op_ble_un,op_ble_un_s,op_blt,op_blt_s,op_blt_un,op_blt_un_s,op_bne_un,
	op_bne_un_s,op_box,op_br,op_br_s,op_break,op_brfalse,op_brfalse_s,op_brinst,
	op_brinst_s,op_brnull,op_brnull_s,op_brtrue,op_brtrue_s,op_brzero,op_brzero_s,op_call,
	op_calli,op_callvirt,op_castclass,op_ceq,op_cgt,op_cgt_un,op_ckfinite,op_clt,
	op_clt_un,op_constrained_,op_conv_i,op_conv_i1,op_conv_i2,op_conv_i4,op_conv_i8,op_conv_ovf_i,
	op_conv_ovf_i_un,op_conv_ovf_i1,op_conv_ovf_i1_un,op_conv_ovf_i2,op_conv_ovf_i2_un,op_conv_ovf_i4,op_conv_ovf_i4_un,op_conv_ovf_i8,
	op_conv_ovf_i8_un,op_conv_ovf_u,op_conv_ovf_u_un,op_conv_ovf_u1,op_conv_ovf_u1_un,op_conv_ovf_u2,op_conv_ovf_u2_un,op_conv_ovf_u4,
	op_conv_ovf_u4_un,op_conv_ovf_u8,op_conv_ovf_u8_un,op_conv_r_un,op_conv_r4,op_conv_r8,op_conv_u,op_conv_u1,
	op_conv_u2,op_conv_u4,op_conv_u8,op_cpblk,op_cpobj,op_div,op_div_un,op_dup,
	op_endfault,op_endfilter,op_endfinally,op_initblk,op_initobj,op_isinst,op_jmp,op_ldarg,
	op_ldarg_0,op_ldarg_1,op_ldarg_2,op_ldarg_3,op_ldarg_s,op_ldarga,op_ldarga_s,op_ldc_i4,
	op_ldc_i4_0,op_ldc_i4_1,op_ldc_i4_2,op_ldc_i4_3,op_ldc_i4_4,op_ldc_i4_5,op_ldc_i4_6,op_ldc_i4_7,
	op_ldc_i4_8,op_ldc_i4_m1,op_ldc_i4_M1,op_ldc_i4_s,op_ldc_i8,op_ldc_r4,op_ldc_r8,op_ldelem,
	op_ldelem_i,op_ldelem_i1,op_ldelem_i2,op_ldelem_i4,op_ldelem_i8,op_ldelem_r4,op_ldelem_r8,op_ldelem_ref,
	op_ldelem_u1,op_ldelem_u2,op_ldelem_u4,op_ldelem_u8,op_ldelema,op_ldfld,op_ldflda,op_ldftn,
	op_ldind_i,op_ldind_i1,op_ldind_i2,op_ldind_i4,op_ldind_i8,op_ldind_r4,op_ldind_r8,op_ldind_ref,
	op_ldind_u1,op_ldind_u2,op_ldind_u4,op_ldind_u8,op_ldlen,op_ldloc,op_ldloc_0,op_ldloc_1,
	op_ldloc_2,op_ldloc_3,op_ldloc_s,op_ldloca,op_ldloca_s,op_ldnull,op_ldobj,op_ldsfld,
	op_ldsflda,op_ldstr,op_ldtoken,op_ldvirtftn,op_leave,op_leave_s,op_localloc,op_mkrefany,
	op_mul,op_mul_ovf,op_mul_ovf_un,op_neg,op_newarr,op_newobj,op_no_,op_nop,
	op_not,op_or,op_pop,op_readonly_,op_refanytype,op_refanyval,op_rem,op_rem_un,
	op_ret,op_rethrow,op_shl,op_shr,op_shr_un,op_sizeof,op_starg,op_starg_s,
	op_stelem,op_stelem_i,op_stelem_i1,op_stelem_i2,op_stelem_i4,op_stelem_i8,op_stelem_r4,op_stelem_r8,
	op_stelem_ref,op_stfld,op_stind_i,op_stind_i1,op_stind_i2,op_stind_i4,op_stind_i8,op_stind_r4,
	op_stind_r8,op_stind_ref,op_stloc,op_stloc_0,op_stloc_1,op_stloc_2,op_stloc_3,op_stloc_s,
	op_stobj,op_stsfld,op_sub,op_sub_ovf,op_sub_ovf_un,op_switch,op_tail_,op_throw,
	op_unaligned_,op_unbox,op_unbox_any,op_volatile_,op_xor
};

/*      addressing mode structure       */
enum e_am
{
    am_vars, am_methodheader, am_floatconst, am_intconst, am_funcname, am_stringlabel, am_branchtarget,
    am_field, am_ind, am_local, am_param, am_switch, 
    am_argit_ctor, am_argit_args, am_argit_getnext, am_argit_unmanaged,
    am_type, am_sized, am_objectArray, am_objectArray_ctor, am_ptrbox, am_ptrunbox,
};
typedef struct amode
{
    enum e_am mode;
    union { 
        struct
        {
            struct swlist
            {
                struct swlist *next;
                int lab;
            } *switches, *switchlast;
        } sw;
        struct
        {
            FPF val;
            BOOLEAN r4;
        } f;
        struct {
            int index;
            char *name; 
        } local;
        struct {
            int label;
            char *name;
            TYPE *tp; // temporary
        } field;
        struct _locallist_ {
            struct _locallist_ *next;
            char *name;
            int index;
            TYPE *tp;
        } *vars;
        LLONG_TYPE i;
        int label;
        SYMBOL *funcsp;
        TYPE *tp;
    } u;
    int length;
    void *altdata;
    int directCall : 1;
    int address : 1;
} AMODE;

/*      output code structure   */

typedef struct ocode
{
    struct ocode *fwd,  *back;
    enum e_op opcode;
    struct amode *oper;
} OCODE;

struct asm_details
{
    char *name;
};
enum asmTypes { pa_nasm, pa_fasm, pa_masm, pa_tasm} ; 
#include "be.p"
