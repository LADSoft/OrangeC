/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#ifndef BCC32
	#define USE_LONGLONG
    #define ULLONG_TYPE unsigned long long
    #define LLONG_TYPE long long
#else 
    #define ULLONG_TYPE unsigned __int64
    #define LLONG_TYPE __int64
#endif 
#include "floating.h"
/*      expression tree descriptions    */

/* the int types can be negative, which means 'signed' vs 'unsigned' */
#define ISZ_NONE 0
#define ISZ_BIT 1
#define ISZ_BOOL 2
#define ISZ_UCHAR 3
#define ISZ_USHORT 4
#define ISZ_UINT 5
#define ISZ_ULONG 6
#define ISZ_ULONGLONG 7
#define ISZ_ADDR 8
#define ISZ_FARPTR 9
#define ISZ_SEG 10
#define ISZ_REG 11
/* */
#define ISZ_FLOAT 12
#define ISZ_DOUBLE 13
#define ISZ_LDOUBLE 14

#define ISZ_IFLOAT 15
#define ISZ_IDOUBLE 16
#define ISZ_ILDOUBLE 17

#define ISZ_CFLOAT 18
#define ISZ_CDOUBLE 19
#define ISZ_CLDOUBLE 20


#ifndef ICODE
#define BESZ_FARPTR  -11
/* the next few can be negative, which means 'signed' vs 'unsigned' */
#define BESZ_NONE 0
#define BESZ_BYTE 1
#define BESZ_WORD 2
#define BESZ_UINT 3
#define BESZ_DWORD 4
#define BESZ_QWORD 6
/* */
#define BESZ_BOOL 5
#define BESZ_FLOAT 7
#define BESZ_BEFARPTR 7 /* used internally to signal the output routines for the x86 */
#define BESZ_DOUBLE 8
#define BESZ_LDOUBLE 10

#define BESZ_IFLOAT 15
#define BESZ_IDOUBLE 16
#define BESZ_ILDOUBLE 17

#define BESZ_CFLOAT 20
#define BESZ_CDOUBLE 21
#define BESZ_CLDOUBLE 22

#else
#define BESZ_FARPTR  ISZ_FARPTR
/* the next few can be negative, which means 'signed' vs 'unsigned' */
#define BESZ_NONE ISZ_NONE
#define BESZ_BYTE ISZ_UCHAR
#define BESZ_WORD ISZ_USHORT
#define BESZ_UINT ISZ_UINT
#define BESZ_DWORD ISZ_ULONG
#define BESZ_QWORD ISZ_ULONGLONG
/* */
#define BESZ_BOOL ISZ_BOOL
#define BESZ_FLOAT ISZ_FLOAT
#define BESZ_DOUBLE ISZ_DOUBLE
#define BESZ_LDOUBLE ISZ_LDOUBLE

#define BESZ_IFLOAT ISZ_IFLOAT
#define BESZ_IDOUBLE ISZ_IDOUBLE
#define BESZ_ILDOUBLE ISZ_ILDOUBLE

#define BESZ_CFLOAT ISZ_CFLOAT
#define BESZ_CDOUBLE ISZ_CDOUBLE
#define BESZ_CLDOUBLE ISZ_CLDOUBLE

#endif

typedef struct
{ 
    FPF r;
    FPF i;
} _COMPLEX_S;

enum e_node
{
    en_void, en_dvoid, en_voidnz, en_structret,  /* used for parameter lists */
    en_cbit, en_cbool, en_cb, en_cub, en_cw, en_cuw, en_ci, en_cui, en_cl, en_cul, en_cll, en_cull, 
        en_cf, en_cd, en_cp, en_csp, en_cfp, en_cld, en_cfc, en_crc, en_clrc,
        en_cfi, en_cri, en_clri, en_boolcon, en_icon, en_lcon, en_iucon,
        en_lucon, en_llcon, en_llucon, en_rcon, en_fcon, en_lrcon,
        en_rimaginarycon, en_fimaginarycon, en_lrimaginarycon, en_rcomplexcon,
        en_fcomplexcon, en_lrcomplexcon, en_ccon, en_cucon, en_labcon, en_nacon,
        en_autocon, en_absacon, en_nalabcon, en_napccon, en_b_ref, en_w_ref,en_i_ref,en_ui_ref,
        en_l_ref, en_ub_ref, en_uw_ref, en_ll_ref, en_ull_ref, en_bit_ref, en_bool_ref, en_a_ref, en_ua_ref,
        en_fimaginaryref, en_rimaginaryref, en_lrimaginaryref, en_fcomplexref,
        en_rcomplexref, en_lrcomplexref, en_floatref, en_doubleref,
        en_longdoubleref, en_fp_ref, en_sp_ref, en_autoreg, en_trapcall, en_ul_ref,
        en_cl_reg, en_fcall, en_fcallb, en_intcall, en_tempref, en_regref,
        en_pfcallb, en_pfcall, en_pcallblock, en_sfcallb, en_sfcall, 
        en_scallblock, en_array, en_arrayindex, en_add, en_sub, en_mul, en_mod, en_div, en_lsh, en_rsh,
        en_cond, en_assign, en_lassign, en_refassign, en_eq, en_ne, en_asadd,
        en_assub, en_asmul, en_asdiv, en_asmod, en_asrsh, en_asumul, en_asudiv,
        en_asumod, en_pmul, en_aslsh, en_asand, en_asor, en_asxor, en_uminus,
        en_asuminus, en_not, en_compl, en_ascompl, en_lt, en_le, en_gt, en_ge,
        en_and, en_or, en_land, en_lor, en_xor, en_ainc, en_adec, en_umul,
        en_udiv, en_umod, en_ugt, en_uge, en_ule, en_ult, en_moveblock, en_clearblock, 
        en_movebyref, en_stackblock, en_callblock, en_pdiv, en_alsh, en_arsh,
        en_asarsh, en_arshd, en_asarshd, en_asalsh, en_bits, en_addstruc,
        en_addcast, en_thiscall, en_imode, en_ptr_ref, en_repcons, en_substack,
		en_loadstack, en_savestack, en_conslabel, en_destlabel, en_placeholder
};

/*      statement node descriptions     */

enum e_stmt
{
    st_line, st_expr, st_while, st_for, st_do, st_if, st_switch, st_case,
        st_goto, st_break, st_continue, st_label, st_asm, st_return, st_block,
        st__genword, st_abs, st_throw, st_tryblock, st_passthrough
};

struct enode
{
    enum e_node nodetype;
    char bits;
    char startbit;
    int cflags;
    long size; /* For block moves */
    union
    {
        LLONG_TYPE i;
        FPF f;
        _COMPLEX_S c;
        struct sym *sp; /* sym will be defined later */
        char *name; /* name during base class processing */
        struct enode *p[2];
    } v;
};

struct snode
{
    enum e_stmt stype;
    struct snode *next; /* next statement */
    struct enode *exp; /* condition or expression */
    struct snode *s1,  *s2,  *lst; /* internal statements &lineno*/
    /* changed */
    struct snode *label; /* label number for goto */
    int flags ;
    LLONG_TYPE switchid;
#define ST_VARARRAY 1    
};

struct cse
{
    struct cse *next;
    struct enode *exp; /* optimizable expression */
    short uses; /* number of uses */
    short duses; /* number of dereferenced uses */
    char size; /* Size of the expresion */
    char voidf; /* cannot optimize flag */
    char reg; /* allocated register */
    #ifdef i386
        char seg; /* allocated seg, 386 far pointer mode */
    #endif 
};

enum e_cm
{
    cm_none, cm_add, cm_mul, cm_asn
};
#define ENODE struct enode
#define SNODE struct snode
#define CSE struct cse

