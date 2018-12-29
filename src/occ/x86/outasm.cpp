/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"
#include "Instruction.h"

#define IEEE

extern "C" ARCH_ASM* chosenAssembler;
extern "C" int prm_nodos;
extern "C" int prm_flat;
extern "C" int fastcallAlias;
extern "C" SYMBOL* theCurrentFunc;
extern "C" int segAligns[];
extern "C" BOOLEAN usingEsp;
int skipsize = 0;
int addsize = 0;

/*      variable initialization         */

char segregs[] = "csdsesfsgsss";

extern "C" int prm_assembler;

static int uses_float;

extern "C" MULDIV* muldivlink = 0;
static enum e_gt oa_gentype = nogen;        /* Current DC type */
extern "C" enum e_sg oa_currentSeg = noseg; /* Current seg */
static int oa_outcol = 0;                   /* Curront col (roughly) */
int newlabel;
int needpointer;
static int nosize = 0;
static int virtual_mode;

/* Init module */
void oa_ini(void)
{
    oa_gentype = nogen;
    oa_currentSeg = noseg;
    oa_outcol = 0;
    newlabel = FALSE;
    muldivlink = 0;
}

/*-------------------------------------------------------------------------*/

void oa_nl(void)
/*
 * New line
 */
{
    if (cparams.prm_asmfile)
    {
        if (oa_outcol > 0)
        {
            beputc('\n');
            oa_outcol = 0;
            oa_gentype = nogen;
        }
    }
}

/* Put an opcode
 */
void outop(const char* name)
{
    beputc('\t');
    while (*name)
        beputc(*name++);
}

/*-------------------------------------------------------------------------*/

void putop(enum e_op op, AMODE* aps, AMODE* apd, int nooptx)
{
    if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
    {
        skipsize = FALSE;
        addsize = FALSE;
        switch (op)
        {
            case op_lea:
                skipsize = TRUE;
                break;
            case op_push:
                addsize = TRUE;
                if (!aps->length)
                    aps->length = ISZ_UINT;
                if (aps->mode == am_immed && isintconst(aps->offset) && aps->offset->v.i >= CHAR_MIN &&
                    aps->offset->v.i <= CHAR_MAX)
                    aps->length = ISZ_UCHAR;
                break;
            case op_add:
            case op_sub:
            case op_adc:
            case op_sbb:
            case op_imul:
                /* yes you can size an imul constant !!!! */
            case op_cmp:
            case op_and:
            case op_or:
            case op_xor:
            case op_idiv:

                if (apd)
                {
                    if (apd->mode == am_immed && isintconst(apd->offset) && apd->offset->v.i >= CHAR_MIN &&
                        apd->offset->v.i <= CHAR_MAX)
                        apd->length = ISZ_UCHAR;
                    addsize = apd->length != 0;
                }
                else
                {
                    addsize = TRUE;
                    if (!aps->length)
                        aps->length = ISZ_UINT;
                }
                break;
            default:
                break;
        }
        if (op == op_fwait)
        {
            outop(opcodeTable[op_wait]);
            return;
        }
        if (op == op_dd)
        {
            outop("dd");
            return;
        }
        if (!nooptx)
        {
            switch (op)
            {
                case op_iretd:
                    outop("iret");
                    return;
                case op_pushad:
                    outop("pusha");
                    return;
                case op_popad:
                    outop("popa");
                    return;
                case op_pushfd:
                    outop("pushf");
                    return;
                case op_popfd:
                    outop("popf");
                    return;
                default:
                    break;
            }
        }
    }
    if (op > sizeof(opcodeTable) / sizeof(opcodeTable[0]))
        diag("illegal opcode.");
    else
        outop(opcodeTable[op]);
    uses_float = (op >= op_f2xm1);
}

/*-------------------------------------------------------------------------*/

void oa_putconst(int op, int sz, EXPRESSION* offset, BOOLEAN doSign)
/*
 *      put a constant to the outputFile file.
 */
{
    char buf[4096];
    SYMBOL* sp;
    char buf1[100];
    int toffs;
    switch (offset->type)
    {
        int m;
        case en_auto:
            m = offset->v.sp->offset;
            if (!usingEsp && m > 0)
                m += 4;
            if (offset->v.sp->storage_class == sc_parameter && fastcallAlias)
            {
                if (!isstructured(basetype(theCurrentFunc->tp)->btp) || offset->v.sp->offset != chosenAssembler->arch->retblocksize)
                {
                    m -= fastcallAlias * chosenAssembler->arch->parmwidth;
                    if (m < chosenAssembler->arch->retblocksize)
                    {
                        m = 0;
                    }
                }
            }
            if (doSign)
            {
                if ((int)offset->v.sp->offset < 0)
                    bePrintf("-0%lxh", -offset->v.sp->offset);
                else
                    bePrintf("+0%lxh", m);
            }
            else
                bePrintf("0%lxh", m);

            break;
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_ll:
        case en_c_ull:
        case en_absolute:
        case en_c_c:
        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_bool:
        case en_c_s:
        case en_c_us:
        case en_c_wc:
            if (doSign)
            {
                if (offset->v.i == 0)
                    break;
                beputc('+');
            }
            {
                int n = offset->v.i;
                if (op == op_mov)
                {
                    if (sz == ISZ_UCHAR || sz == -ISZ_UCHAR)
                        n &= 0xff;
                    if (sz == ISZ_USHORT || sz == -ISZ_USHORT)
                        n &= 0xffff;
                }
                bePrintf("0%xh", n);
            }
            break;
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            if (doSign)
                beputc('+');
            FPFToString(buf, &offset->v.c.r);
            FPFToString(buf1, &offset->v.c.i);
            bePrintf("%s,%s", buf, buf1);
            break;
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            if (doSign)
                beputc('+');
            FPFToString(buf, &offset->v.f);
            bePrintf("%s", buf);
            break;
        case en_labcon:
            if (doSign)
                beputc('+');
            bePrintf("L_%d", offset->v.i);
            break;
        case en_pc:
        case en_global:
        case en_threadlocal:
            if (doSign)
                beputc('+');
            sp = offset->v.sp;
            beDecorateSymName(buf, sp);
            bePrintf("%s", buf);
            break;
        case en_add:
        case en_structadd:
        case en_arrayadd:
            oa_putconst(0, ISZ_ADDR, offset->left, doSign);
            oa_putconst(0, ISZ_ADDR, offset->right, TRUE);
            break;
        case en_sub:
            oa_putconst(0, ISZ_ADDR, offset->left, doSign);
            bePrintf("-");
            oa_putconst(0, ISZ_ADDR, offset->right, FALSE);
            break;
        case en_uminus:
            bePrintf("-");
            oa_putconst(0, ISZ_ADDR, offset->left, FALSE);
            break;
        default:
            diag("illegal constant node.");
            break;
    }
}

/*-------------------------------------------------------------------------*/

void oa_putlen(int l)
/*
 *      append the length field to an instruction.
 */
{
    if (l < 0)
        l = -l;
    switch (l)
    {
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_UINT:
        case ISZ_U16:
        case ISZ_U32:
        case ISZ_ULONG:
        case ISZ_ULONGLONG:
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
        case ISZ_ADDR:
        case ISZ_SEG:
        case 0:
            break;
        default:
            diag("oa_putlen: illegal length field.");
    }
}

/*-------------------------------------------------------------------------*/

void putsizedreg(char* string, int reg, int size)
{
    static char* byteregs[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
    static char* wordregs[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
    static char* longregs[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
    if (size < 0)
        size = -size;
    if (size == ISZ_UINT || size == ISZ_ULONG || size == ISZ_ADDR || size == ISZ_U32)
        bePrintf(string, longregs[reg]);
    else if (size == ISZ_BOOLEAN || size == ISZ_UCHAR)
    {
        bePrintf(string, byteregs[reg]);
    }
    else
        bePrintf(string, wordregs[reg]);
}

/*-------------------------------------------------------------------------*/

void pointersize(int size)
{
    if ((prm_assembler == pa_nasm || prm_assembler == pa_fasm) && skipsize)
        return;
    if (size < 0)
        size = -size;
    /*      if (needpointer)
     */
    switch (size)
    {
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
        case ISZ_ULONGLONG:
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            bePrintf("qword ");
            break;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            if (!uses_float)
            {
                if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                    bePrintf("dword far ");
                else
                    bePrintf("fword ");
                break;
            }
        case ISZ_U32:
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_ADDR:
            bePrintf("dword ");
            break;
        case ISZ_U16:
        case ISZ_USHORT:
        case ISZ_WCHAR:
            bePrintf("word ");
            break;
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            bePrintf("byte ");
            break;
        case ISZ_NONE:
            /* for NASM with certain FP ops */
            break;
        case ISZ_FARPTR:
            bePrintf("far ");
            break;
        default:
            diag("Bad pointer");
            break;
    }
    if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm) && size)
        bePrintf("ptr ");
}

/*-------------------------------------------------------------------------*/

void putseg(int seg, int usecolon)
{
    if (!seg)
        return;
    seg -= 1;
    seg <<= 1;
    beputc(segregs[seg]);
    beputc(segregs[seg + 1]);
    if (usecolon)
        beputc(':');
}

/*-------------------------------------------------------------------------*/

int islabeled(EXPRESSION* n)
{
    int rv = 0;
    switch (n->type)
    {
        case en_add:
        case en_structadd:
        case en_arrayadd:
        case en_sub:
            //        case en_addstruc:
            rv |= islabeled(n->left);
            rv |= islabeled(n->right);
            break;
        case en_c_i:
        case en_c_c:
        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_l:
        case en_c_ul:
        case en_c_ui:
        case en_c_bool:
        case en_c_wc:
        case en_c_s:
        case en_c_us:
            return 0;
        case en_labcon:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_pc:
        case en_threadlocal:
            return 1;
        default:
            diag("Unexpected node type in islabeled");
            break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

void oa_putamode(int op, int szalt, AMODE* ap)
/*
 *      output a general addressing mode.
 */
{
    int oldnasm;

    switch (ap->mode)
    {
        case am_seg:
            putseg(ap->seg, 0);
            break;
        case am_screg:
            bePrintf("cr%d", ap->preg);
            break;
        case am_sdreg:
            bePrintf("dr%d", ap->preg);
            break;
        case am_streg:
            bePrintf("tr%d", ap->preg);
            break;
        case am_immed:
            if (ap->length > 0 && islabeled(ap->offset))
            {
                if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                    bePrintf("offset ");
                else if (!nosize)
                {
                    if (ap->length == -ISZ_UCHAR || ap->length == ISZ_UCHAR)
                    {
                        bePrintf("byte ");
                    }
                    else if (ap->length == -ISZ_USHORT || ap->length == ISZ_USHORT)
                    {
                        bePrintf("word ");
                    }
                    else
                    {
                        bePrintf("dword ");
                    }
                }
            }
            else if ((prm_assembler == pa_nasm) && addsize)
                pointersize(ap->length);
            oa_putconst(op, op == op_mov ? szalt : ap->length, ap->offset, FALSE);
            break;
        case am_direct:
            pointersize(ap->length);
            if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                putseg(ap->seg, TRUE);
            beputc('[');
            oldnasm = prm_assembler;
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                putseg(ap->seg, TRUE);
            prm_assembler = pa_nasm;
            oa_putconst(0, ap->length, ap->offset, FALSE);
            beputc(']');
            prm_assembler = oldnasm;
            break;
        case am_dreg:
            putsizedreg("%s", ap->preg, ap->length);
            break;
        case am_xmmreg:
            bePrintf("xmm%d", ap->preg);
            break;
        case am_mmreg:
            bePrintf("mm%d", ap->preg);
            break;
        case am_freg:
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                bePrintf("st%d", ap->preg);
            else
                bePrintf("st(%d)", ap->preg);
            break;
        case am_indisp:
            pointersize(ap->length);
            if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                putseg(ap->seg, TRUE);
            beputc('[');
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                putseg(ap->seg, TRUE);
            putsizedreg("%s", ap->preg, ISZ_ADDR);
            if (ap->offset)
            {
                oa_putconst(0, ap->length, ap->offset, TRUE);
            }
            beputc(']');
            break;
        case am_indispscale:
        {
            int scale = 1, t = ap->scale;

            while (t--)
                scale <<= 1;
            pointersize(ap->length);
            if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                putseg(ap->seg, TRUE);
            beputc('[');
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                putseg(ap->seg, TRUE);
            if (ap->preg != -1)
                putsizedreg("%s+", ap->preg, ISZ_ADDR);
            putsizedreg("%s", ap->sreg, ISZ_ADDR);
            if (scale != 1)
                bePrintf("*%d", scale);
            if (ap->offset)
            {
                oa_putconst(0, ap->length, ap->offset, TRUE);
            }
            beputc(']');
        }
        break;
        default:
            diag("illegal address mode.");
            break;
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_code(OCODE* cd)
/*
 *      outputFile a generic instruction.
 */
{
    int op = cd->opcode, len = 0, len2 = 0;
    AMODE *aps = cd->oper1, *apd = cd->oper2, *ap3 = cd->oper3;

    if (!cparams.prm_asmfile)
        return;
    if (op == op_blockstart || op == op_blockend || op == op_varstart || op == op_funcstart || op == op_funcend)
        return;
    if (op == op_line)
    {
        LINEDATA* ld = (LINEDATA*)aps;
        oa_nl();
        while (ld)
        {
            bePrintf("; Line %d:\t%s\n", ld->lineno, ld->line);
            ld = ld->next;
        }
        return;
    }
    else if (op == op_comment)
    {
        if (!cparams.prm_lines)
            return;
        bePrintf("%s", aps);
        return;
    }
    else if (op == op_align)
    {
        oa_align(aps->offset->v.i);
        return;
    }
    else if (op == op_void)
        return;
    if (aps)
        len = aps->length;
    if (apd)
        len2 = apd->length;
    needpointer = (len != len2) || ((!aps || aps->mode != am_dreg) && (!apd || apd->mode != am_dreg));
    putop((e_op)op, aps, apd, cd->noopt);
    if ((prm_assembler == pa_nasm || prm_assembler == pa_fasm) &&
        (op >= op_ja && op <= op_jz && op != op_jecxz && (op != op_jmp || aps->mode == am_immed)))
    {
        if (cd->ins)
        {
            if ((((Instruction*)cd->ins)->GetData()[0] & 0xf0) == 0x70 || ((Instruction*)cd->ins)->GetData()[0] == 0xeb)
            {
                bePrintf("\tshort");
            }
            else
            {
                bePrintf("\tnear");
            }
        }
        nosize = TRUE;
    }
    else if (op == op_jmp && aps->mode == am_immed && aps->offset->type == en_labcon)
    {
        if (cd->ins)
        {
            if (((Instruction*)cd->ins)->GetData()[0] == 0xeb)
            {
                bePrintf("\tshort");
                nosize = TRUE;
            }
        }
    }
    switch (op)
    {
        case op_rep:
        case op_repz:
        case op_repe:
        case op_repnz:
        case op_repne:
        case op_lock:
            return;
    }
    oa_putlen(len);
    if (aps != 0)
    {
        int separator;
        bePrintf("\t");
        if ((op == op_jmp || op == op_call) && aps && apd)
        {
            separator = ':';
        }
        else
        {
            separator = ',';
        }
        if (op == op_dd)
            nosize = TRUE;
        oa_putamode(op, aps->length, aps);
        nosize = FALSE;
        if (apd != 0)
        {
            beputc(separator);
            oa_putamode(op, aps->length, apd);
        }
        if (ap3 != 0)
        {
            beputc(separator);
            oa_putamode(op, aps->length, ap3);
        }
    }
    bePrintf("\n");
}

/*-------------------------------------------------------------------------*/

void oa_gen_strlab(SYMBOL* sp)
/*
 *      generate a named label.
 */
{
    char buf[4096];
    beDecorateSymName(buf, sp);
    if (cparams.prm_asmfile)
    {
        if (oa_currentSeg == dataseg || oa_currentSeg == bssxseg)
        {
            newlabel = TRUE;
            bePrintf("\n%s", buf);
            oa_outcol = strlen(buf) + 1;
        }
        else
            bePrintf("%s:\n", buf);
    }
    outcode_gen_strlab(sp);
}

/*-------------------------------------------------------------------------*/

void oa_put_label(int lab)
/*
 *      outputFile a compiler generated label.
 */
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (oa_currentSeg == dataseg || oa_currentSeg == bssxseg)
        {
            newlabel = TRUE;
            bePrintf("\nL_%ld", lab);
            oa_outcol = 8;
        }
        else
            bePrintf("L_%ld:\n", lab);
    }
    else
        outcode_put_label(lab);
}
void oa_put_string_label(int lab, int type) { oa_put_label(lab); }

/*-------------------------------------------------------------------------*/

void oa_genfloat(enum e_gt type, FPFC* val)
/*
 * Output a float value
 */
{
    if (cparams.prm_asmfile)
    {
        char buf[256];
        FPFToString(buf, val);
        switch (type)
        {
            case floatgen:
                if (!strcmp(buf, "inf") || !strcmp(buf, "nan") || !strcmp(buf, "-inf") || !strcmp(buf, "-nan"))
                {
                    UBYTE dta[4];
                    int i;
                    FPFToFloat(dta, val);
                    bePrintf("\tdb\t");
                    for (i = 0; i < 4; i++)
                    {
                        bePrintf("0%02XH", dta[i]);
                        if (i != 3)
                            bePrintf(", ");
                    }
                }
                else
                    bePrintf("\tdd\t%s\n", buf);
                break;
            case doublegen:
                if (!strcmp(buf, "inf") || !strcmp(buf, "nan") || !strcmp(buf, "-inf") || !strcmp(buf, "-nan"))
                {
                    UBYTE dta[8];
                    int i;
                    FPFToDouble(dta, val);
                    bePrintf("\tdb\t");
                    for (i = 0; i < 8; i++)
                    {
                        bePrintf("0%02XH", dta[i]);
                        if (i != 7)
                            bePrintf(", ");
                    }
                }
                else
                    bePrintf("\tdq\t%s\n", buf);
                break;
            case longdoublegen:
                if (!strcmp(buf, "inf") || !strcmp(buf, "nan") || !strcmp(buf, "-inf") || !strcmp(buf, "-nan"))
                {
                    UBYTE dta[10];
                    int i;
                    FPFToLongDouble(dta, val);
                    bePrintf("\tdb\t");
                    for (i = 0; i < 10; i++)
                    {
                        bePrintf("0%02XH", dta[i]);
                        if (i != 9)
                            bePrintf(", ");
                    }
                }
                else
                    bePrintf("\tdt\t%s\n", buf);
                break;
            default:
                diag("floatgen - invalid type");
                break;
        }
    }
    else
        switch (type)
        {
            case floatgen:
                outcode_genfloat(val);
                break;
            case doublegen:
                outcode_gendouble(val);
                break;
            case longdoublegen:
                outcode_genlongdouble(val);
                break;
            default:
                diag("floatgen - invalid type");
                break;
        }
}
/*-------------------------------------------------------------------------*/

void oa_genstring(LCHAR* str, int len)
/*
 * Generate a string literal
 */
{
    BOOLEAN instring = FALSE;
    if (cparams.prm_asmfile)
    {
        int nlen = len;
        while (nlen--)
            if (*str >= ' ' && *str < 0x7f && *str != '\'' && *str != '\"')
            {
                if (!instring)
                {
                    oa_gentype = nogen;
                    oa_nl();
                    bePrintf("\tdb\t\"");
                    instring = TRUE;
                }
                bePrintf("%c", *str++);
            }
            else
            {
                if (instring)
                {
                    bePrintf("\"\n");
                    instring = FALSE;
                }
                oa_genint(chargen, *str++);
            }
        if (instring)
            bePrintf("\"\n");
    }
    else
        outcode_genstring(str, len);
}

/*-------------------------------------------------------------------------*/

void oa_genint(enum e_gt type, LLONG_TYPE val)
{
    if (cparams.prm_asmfile)
    {
        switch (type)
        {
            case chargen:
                bePrintf("\tdb\t0%xh\n", val & 0x00ff);
                break;
            case shortgen:
            case u16gen:
                bePrintf("\tdw\t0%xh\n", val & 0x0ffff);
                break;
            case longgen:
            case enumgen:
            case intgen:
            case u32gen:
                bePrintf("\tdd\t0%lxh\n", val);
                break;
            case longlonggen:
#ifndef USE_LONGLONG
                bePrintf("\tdd\t0%lxh,0%lxh\n", val, val < 0 ? -1 : 0);
#else
                bePrintf("\tdd\t0%lxh,0%lxh\n", val, val >> 32);
#endif
                break;
            case wchar_tgen:
                bePrintf("\tdw\t0%lxh\n", val);
                break;
            default:
                diag("genint - unknown type");
                break;
        }
    }
    else
    {
        switch (type)
        {
                /*            case chargen:*/
            case chargen:
                outcode_genbyte(val);
                break;
            case shortgen:
            case u16gen:
                outcode_genword(val);
                break;
            case longgen:
            case enumgen:
            case intgen:
            case u32gen:
                outcode_genlong(val);
                break;
            case longlonggen:
                outcode_genlonglong(val);
                break;
            case wchar_tgen:
                outcode_genword(val);
                break;
            default:
                diag("genint - unknown type");
        }
    }
}
void oa_genaddress(ULLONG_TYPE val) { oa_genint(longgen, val); }
/*-------------------------------------------------------------------------*/

void oa_gensrref(SYMBOL* sp, int val, int type)
{
    char buf[4096];
    if (cparams.prm_asmfile)
    {
        beDecorateSymName(buf, sp);
        oa_nl();
        bePrintf("\tdb\t0,%d\n", val);
        bePrintf("\tdd\t%s\n", buf);
        oa_gentype = srrefgen;
    }
    else
        outcode_gensrref(sp, val);
}

/*-------------------------------------------------------------------------*/

void oa_genref(SYMBOL* sp, int offset)
/*
 * Output a reference to the data area (also gens fixups )
 */
{
    char sign;
    char buf[4096], buf1[4096];
    if (cparams.prm_asmfile)
    {
        if (offset < 0)
        {
            sign = '-';
            offset = -offset;
        }
        else
            sign = '+';
        beDecorateSymName(buf, sp);
        sprintf(buf1, "%s%c%d", buf, sign, offset);
        if (cparams.prm_asmfile)
        {
            if (!newlabel)
                oa_nl();
            else
                newlabel = FALSE;
            bePrintf("\tdd\t%s\n", buf1);
            oa_gentype = longgen;
        }
    }
    else
        outcode_genref(sp, offset);
}

/*-------------------------------------------------------------------------*/

void oa_genpcref(SYMBOL* sp, int offset)
/*
 * Output a reference to the code area (also gens fixups )
 */
{
    oa_genref(sp, offset);
}

/*-------------------------------------------------------------------------*/

void oa_genstorage(int nbytes)
/*
 * Output bytes of storage
 */
{
    if (cparams.prm_asmfile)
    {
        if (!newlabel)
            oa_nl();
        else
            newlabel = FALSE;
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf("\tresb\t0%xh\n", nbytes);
        else
            bePrintf("\tdb\t0%xh DUP (?)\n", nbytes);
        oa_gentype = nogen;
    }
    else
        outcode_genstorage(nbytes);
}

/*-------------------------------------------------------------------------*/

void oa_gen_labref(int n)
/*
 * Generate a reference to a label
 */
{
    if (cparams.prm_asmfile)
    {

        if (!newlabel)
            oa_nl();
        else
            newlabel = FALSE;
        bePrintf("\tdd\tL_%d\n", n);
        oa_gentype = longgen;
    }
    else
        outcode_gen_labref(n);
}

/*-------------------------------------------------------------------------*/

void oa_gen_labdifref(int n1, int n2)
{
    if (cparams.prm_asmfile)
    {
        if (!newlabel)
            oa_nl();
        else
            newlabel = FALSE;
        bePrintf("\tdd\tL_%d-L_%d\n", n1, n2);
        oa_gentype = longgen;
    }
    else
        outcode_gen_labdifref(n1, n2);
}

/*
 * Exit if from a special segment
 */
void oa_exitseg(enum e_sg seg)
{
    if (cparams.prm_asmfile)
    {
        if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
        {
            if (seg == startupxseg)
            {
                bePrintf("cstartup\tENDS\n");
            }
            else if (seg == rundownxseg)
            {
                bePrintf("crundown\tENDS\n");
            }
            else if (seg == constseg)
            {
                bePrintf("_CONST\tENDS\n");
            }
            else if (seg == stringseg)
            {
                bePrintf("_STRING\tENDS\n");
            }
            else if (seg == tlsseg)
            {
                bePrintf("_TLS\tENDS\n");
            }
            else if (seg == tlssuseg)
            {
                bePrintf("tlsstartup\tENDS\n");
            }
            else if (seg == tlsrdseg)
            {
                bePrintf("tlsrundown\tENDS\n");
            }
        }
        oa_nl();
    }
}

/*
 * Switch to cseg
 */
void oa_enterseg(enum e_sg seg)
{
    oa_currentSeg = seg;
    outcode_enterseg(seg);
    if (cparams.prm_asmfile)
    {
        if (seg == codeseg)
        {
            oa_nl();
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf("section code\n");
                else
                {
                    bePrintf("section .text\n");
                    bePrintf("[bits 32]\n");
                }
            else
                bePrintf("\t.code\n");
        }
        else if (seg == constseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf("section const\n");
                else
                {
                    bePrintf("section .text\n");
                    bePrintf("[bits 32]\n");
                }
            }
            else
                bePrintf("_CONST\tsegment use32 public dword \042CONST\042\n");
        }
        else if (seg == stringseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf("section string\n");
                else
                {
                    bePrintf("section .data\n");
                    bePrintf("[bits 32]\n");
                }
            }
            else
                bePrintf("_STRING\tsegment use32 public dword \042STRING\042\n");
        }
        else if (seg == dataseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf("section data\n");
                else
                {
                    bePrintf("section .data\n");
                    bePrintf("[bits 32]\n");
                }
            else
                bePrintf("\t.DATA\n");
        }
        else if (seg == tlsseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf("section tls\n");
                else
                {
                    bePrintf("section .data\n");
                    bePrintf("[bits 32]\n");
                }
            else
                bePrintf("_TLS\tsegment use32 public dword \042TLS\042\n");
        }
        else if (seg == tlssuseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf("section tlsstartup\n");
                else
                {
                    bePrintf("section .data\n");
                    bePrintf("[bits 32]\n");
                }
            else
                bePrintf("_TLS\tsegment use32 public dword \042TLS\042\n");
        }
        else if (seg == tlsrdseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf("section tlsrundown\n");
                else
                {
                    bePrintf("section .data\n");
                    bePrintf("[bits 32]\n");
                }
            else
                bePrintf("_TLS\tsegment use32 public dword \042TLS\042\n");
        }
        else if (seg == bssxseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf("section bss\n");
                else
                    bePrintf("section .bss\n");
            else
                bePrintf("\t.data?\n");
        }
        else if (seg == startupxseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf("section cstartup\n");
                else
                {
                    bePrintf("section .text\n");
                    bePrintf("[bits 32]\n");
                }
            }
            else
                bePrintf("cstartup\tsegment use32 public dword \042INITDATA\042\n");
        }
        else if (seg == rundownxseg)
        {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf("section crundown\n");
                else
                {
                    bePrintf("section .text\n");
                    bePrintf("[bits 32]\n");
                }
            }
            else
                bePrintf("crundown\tsegment use32 public dword \042EXITDATA\042\n");
        }
    }
}

/*-------------------------------------------------------------------------*/

void oa_gen_virtual(SYMBOL* sp, int data)
{
    virtual_mode = data;
    oa_currentSeg = virtseg;
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
        {
            oa_currentSeg = noseg;
#ifdef IEEE
            if (sp->decoratedName[0] == '@')
                if (virtual_mode)
                    bePrintf("\tsection vsd%s virtual\n", sp->decoratedName);
                else
                    bePrintf("\tsection vsc%s virtual\n", sp->decoratedName);
            else if (virtual_mode)
                bePrintf("\tsection vsd@%s virtual\n", sp->decoratedName);
            else
                bePrintf("\tsection vsc@%s virtual\n", sp->decoratedName);
#else
            bePrintf("\tSECTION @%s VIRTUAL\n", sp->decoratedName);
#endif
        }
        else
            bePrintf("@%s\tsegment virtual\n", sp->decoratedName);
        bePrintf("%s:\n", sp->decoratedName);
    }
    outcode_start_virtual_seg(sp, data);
}
void oa_gen_endvirtual(SYMBOL* sp)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
        {
            bePrintf("@%s\tends\n", sp->decoratedName);
        }
        else if (virtual_mode)
            oa_enterseg(dataseg);
        else
            oa_enterseg(codeseg);
    }
    else
    {
        if (virtual_mode)
            oa_enterseg(dataseg);
        else
            oa_enterseg(codeseg);
        outcode_end_virtual_seg(sp);
    }
}
/*
 * Align
 * not really honorign the size... all alignments are mod 4...
 */
void oa_align(int size)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            /* NASM 0.91 wouldn't let me use parenthesis but this should work
             * according to the documented precedence levels
             */
            bePrintf("\ttimes $$-$ & %d nop\n", size - 1);
        else
            bePrintf("\talign\t%d\n", size);
    }
    else
    {
        outcode_align(size);
    }
}
void oa_setalign(int code, int data, int bss, int constant)
{
    if (code > 0)
        segAligns[codeseg] = code;
    if (data > 0)
        segAligns[dataseg] = data;
    if (bss > 0)
        segAligns[bssxseg] = bss;
    if (constant > 0)
        segAligns[constseg] = constant;
}

/*
 * queue muldiv val
 */
long queue_muldivval(long number)
{
    MULDIV *p = muldivlink, **q = &muldivlink;
    if (cparams.prm_mergestrings)
    {
        while (p)
        {
            if (p->size == 0 && p->value == number)
                return p->label;
            p = p->next;
        }
    }
    p = (MULDIV*)beGlobalAlloc(sizeof(MULDIV));
    p->next = 0;
    p->value = number;
    p->size = 0;
    while (*q)
        q = &(*q)->next;
    *q = p;
    return p->label = beGetLabel;
}

long queue_large_const(unsigned constant[], int count)
{
    int lbl = beGetLabel;
    MULDIV *p, **q = &muldivlink;

    while (*q) q = &(*q)->next;
    for (int i = 0; i < count; i++, q=&(*q)->next)
    {
        p = (MULDIV*)beGlobalAlloc(sizeof(MULDIV));
        p->value = constant[i];
        if (i == 0)
            p->label = lbl;

    }
    return lbl;
}
/*-------------------------------------------------------------------------*/

long queue_floatval(FPFC* number, int size)
{
    MULDIV *p = muldivlink, **q = &muldivlink;
    if (cparams.prm_mergestrings)
    {
        while (p)
        {
            if (p->size == size && !memcmp(&p->floatvalue, number, sizeof(*number)))
                return p->label;
            p = p->next;
        }
    }
    p = (MULDIV*)beGlobalAlloc(sizeof(MULDIV));
    p->next = 0;
    p->floatvalue = *number;
    p->size = size;
    while (*q)
        q = &(*q)->next;
    *q = p;
    return p->label = beGetLabel;
}

/*-------------------------------------------------------------------------*/

void dump_muldivval(void)
{
    int tag = FALSE;
    xconstseg();
    if (cparams.prm_asmfile)
    {
        bePrintf("\n");
        if (muldivlink)
        {
            tag = TRUE;
        }
        while (muldivlink)
        {
            oa_align(8);
            if (muldivlink->label)
                oa_put_label(muldivlink->label);
            if (muldivlink->size == ISZ_NONE)
                bePrintf("\tdd\t0%xh\n", muldivlink->value);
            else
            {
                char buf[256];
                if (muldivlink->floatvalue.type == IFPF_IS_INFINITY || muldivlink->floatvalue.type == IFPF_IS_NAN)
                {
                    UBYTE data[12];
                    int len = 0;
                    int i;
                    switch (muldivlink->size)
                    {
                        case ISZ_FLOAT:
                        case ISZ_IFLOAT:
                            FPFToFloat(data, &muldivlink->floatvalue);
                            len = 4;
                            break;
                        case ISZ_DOUBLE:
                        case ISZ_IDOUBLE:
                        case ISZ_LDOUBLE:
                        case ISZ_ILDOUBLE:
                            FPFToDouble(data, &muldivlink->floatvalue);
                            len = 8;
                            break;
                    }
                    bePrintf("\tdb\t");
                    for (i = 0; i < len; i++)
                    {
                        bePrintf("0%02XH", data[i]);
                        if (i != len - 1)
                            bePrintf(",");
                    }
                    bePrintf("\n");
                }
                else
                {
                    FPFToString(buf, &muldivlink->floatvalue);
                    if (muldivlink->size == ISZ_FLOAT || muldivlink->size == ISZ_IFLOAT)

                    {
                        bePrintf("\tdd\t%s\n", buf);
                    }
                    else
                        bePrintf("\tdq\t%s\n", buf);
                }
            }
            muldivlink = muldivlink->next;
        }
        if (tag)
            bePrintf("\n");
    }
    else
        outcode_dump_muldivval();
}

/*-------------------------------------------------------------------------*/

void dump_browsedata(BROWSEINFO* bri)
{
    if (!cparams.prm_asmfile)
        omf_dump_browsedata(bri);
}
void dump_browsefile(BROWSEFILE* brf)
{
    if (!cparams.prm_asmfile)
        omf_dump_browsefile(brf);
}

/*-------------------------------------------------------------------------*/
char asmfile[256];
char compilerversion[256];
void oa_header(char* filename, char* compiler_version)
{
    strcpy(asmfile, filename);
    strcpy(compilerversion, compiler_version);
    oa_nl();
    bePrintf(";File %s\n", filename);
    bePrintf(";Compiler version %s\n", compiler_version);
    if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
    {
        if (!prm_nodos)
        {
            bePrintf("\tsection code align=%-8d class=CODE use32\n", segAligns[codeseg]);
            bePrintf("\tsection data align=%-8d class=DATA use32\n", segAligns[dataseg]);
            bePrintf("\tsection bss  align=%-8d class=BSS use32\n", segAligns[bssxseg]);
            bePrintf("\tsection const  align=%-8d class=CONST use32\n", segAligns[constseg]);
            bePrintf("\tsection string  align=%-8d class=STRING use32\n", segAligns[stringseg]);
            bePrintf("\tsection tls  align=8 class=TLS use32\n");
            bePrintf("\tsection cstartup align=2 class=INITDATA use32\n");
            bePrintf("\tsection crundown align=2 class=EXITDATA use32\n");
            //            bePrintf(
            //               "\tSECTION cppinit  align=4 CLASS=CPPINIT USE32\n");
            //          bePrintf(
            //             "\tSECTION cppexit  align=4 CLASS=CPPEXIT USE32\n");
            //        bePrintf( "\tGROUP DGROUP _DATA _BSS _CONST _STRING\n\n");
        }
        else
        {
            bePrintf("\tsection .text\n");
            bePrintf("\tsection .data\n");
            bePrintf("\tsection .bss\n");
        }
    }
    else
    {
        bePrintf("\ttitle\t'%s'\n", filename);
        if (prm_flat)
            bePrintf("\t.486p\n\t.model flat\n\n");
        else if (prm_assembler == pa_masm)
            bePrintf("\t.486p\n\t.model small\n\n");
        else
            bePrintf("\t.486p\n\t.model use32 small\n\n");
    }
}
void oa_trailer(void)
{
    if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
        bePrintf("\tend\n");
    beRewind();
    oa_header(asmfile, compilerversion);
}
/*-------------------------------------------------------------------------*/
void oa_localdef(SYMBOL* sp)
{
    if (!cparams.prm_asmfile)
    {
        omf_globaldef(sp);
    }
}
void oa_localstaticdef(SYMBOL* sp)
{
    if (!cparams.prm_asmfile)
    {
        omf_globaldef(sp);
    }
}
void oa_globaldef(SYMBOL* sp)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf("[global\t%s]\n", sp->decoratedName);
        else
            bePrintf("\tpublic\t%s\n", sp->decoratedName);
    }
    else
        omf_globaldef(sp);
}

/*-------------------------------------------------------------------------*/

void oa_output_alias(char* name, char* alias)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf("%%define %s %s\n", name, alias);
        else
            bePrintf("%s equ\t<%s>\n", name, alias);
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_extern(SYMBOL* sp, int code)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
        {
            bePrintf("[extern\t%s]\n", sp->decoratedName);
        }
        else
        {
            if (code)
                bePrintf("\textrn\t%s:proc\n", sp->decoratedName);
            else
                bePrintf("\textrn\t%s:byte\n", sp->decoratedName);
        }
    }
    else
        omf_put_extern(sp, code);
}
/*-------------------------------------------------------------------------*/

void oa_put_impfunc(SYMBOL* sp, char* file)
{
    if (cparams.prm_asmfile)
    {
        bePrintf("\timport %s %s\n", sp->decoratedName, file);
    }
    else
    {
        omf_put_impfunc(sp, file);
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_expfunc(SYMBOL* sp)
{
    char buf[4096];
    if (cparams.prm_asmfile)
    {
        beDecorateSymName(buf, sp);
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf("\texport %s\n", buf);
        else
            bePrintf("\tpublicdll %s\n", buf);
    }
    else
        omf_put_expfunc(sp);
}

void oa_output_includelib(char* name)
{
    if (cparams.prm_asmfile)
    {
        if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
            bePrintf("\tincludelib %s\n", name);
    }
    else
        omf_put_includelib(name);
}
void oa_end_generation(void) { dump_muldivval(); }