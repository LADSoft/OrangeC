/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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
#include "beinterfdefs.h"
#include "config.h"
#include "occ.h"
#include "outcode.h"
#include "ildata.h"
#include "output.h"
#include "outasm.h"
#include "memory.h"
#define IEEE

namespace occx86
{
int skipsize = 0;
int addsize = 0;

/*      variable initialization         */

char segregs[] = "csdsesfsgsss";

MULDIV* muldivlink = 0;
enum Optimizer::e_sg oa_currentSeg = Optimizer::noseg; /* Current seg */
int newlabel;
int needpointer;
static enum Optimizer::e_gt oa_gentype = Optimizer::nogen; /* Current DC type */
static int uses_float;
static int oa_outcol = 0; /* Curront col (roughly) */
static int nosize = 0;
static int virtual_mode;

/* Init module */
void oa_ini(void)
{
    oa_gentype = Optimizer::nogen;
    oa_currentSeg = Optimizer::noseg;
    oa_outcol = 0;
    newlabel = false;
    muldivlink = 0;
}

/*-------------------------------------------------------------------------*/

void oa_nl(void)
/*
 * New line
 */
{
    if (Optimizer::cparams.prm_asmfile)
    {
        if (oa_outcol > 0)
        {
            Optimizer::beputc('\n');
            oa_outcol = 0;
            oa_gentype = Optimizer::nogen;
        }
    }
}

/* Put an opcode
 */
void outop(const char* name)
{
    Optimizer::beputc('\t');
    while (*name)
        Optimizer::beputc(*name++);
}

/*-------------------------------------------------------------------------*/

void putop(enum e_opcode op, AMODE* aps, AMODE* apd, int nooptx)
{
    if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
        Optimizer::cparams.prm_assembler == pa_fasm)
    {
        skipsize = false;
        addsize = false;
        switch (op)
        {
            case op_lea:
                skipsize = true;
                break;
            case op_push:
                addsize = true;
                if (!aps->length)
                    aps->length = ISZ_UINT;
                if (aps->mode == am_immed && (aps->offset->type == Optimizer::se_i || aps->offset->type == Optimizer::se_ui) &&
                    aps->offset->i >= CHAR_MIN && aps->offset->i <= CHAR_MAX)
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
                    if (apd->mode == am_immed && (apd->offset->type == Optimizer::se_i || apd->offset->type == Optimizer::se_ui) &&
                        apd->offset->i >= CHAR_MIN && apd->offset->i <= CHAR_MAX)
                        apd->length = ISZ_UCHAR;
                    addsize = apd->length != 0;
                }
                else
                {
                    addsize = true;
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
        if ((e_op)op == op_dd)
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

void oa_putconst(int op, int sz, Optimizer::SimpleExpression* offset, bool doSign)
/*
 *      put a constant to the outputFile file.
 */
{
    char buf[4096];
    Optimizer::SimpleSymbol* sym;
    int toffs;
    switch (offset->type)
    {
        int m;
        case Optimizer::se_auto:
            m = offset->sp->offset;
            if (!usingEsp && m > 0)
                m += 4;
            if (offset->sp->storage_class == Optimizer::scc_parameter && Optimizer::fastcallAlias)
            {
                if ((currentFunction->tp->btp->type != Optimizer::st_struct &&
                     currentFunction->tp->btp->type != Optimizer::st_union) ||
                    offset->sp->offset != Optimizer::chosenAssembler->arch->retblocksize)
                {
                    m -= Optimizer::fastcallAlias * Optimizer::chosenAssembler->arch->parmwidth;
                    if (m < Optimizer::chosenAssembler->arch->retblocksize)
                    {
                        m = 0;
                    }
                }
            }
            if (doSign)
            {
                if ((int)offset->sp->offset < 0)
                    Optimizer::bePrintf("-0%lxh", -offset->sp->offset);
                else
                    Optimizer::bePrintf("+0%lxh", m);
            }
            else
                Optimizer::bePrintf("0%lxh", m);

            break;
        case Optimizer::se_i:
        case Optimizer::se_ui:
        case Optimizer::se_absolute:
            if (doSign)
            {
                if (offset->i == 0)
                    break;
                Optimizer::beputc('+');
            }
            {
                int n = offset->i;
                if (op == op_mov)
                {
                    if (sz == ISZ_UCHAR || sz == -ISZ_UCHAR)
                        n &= 0xff;
                    if (sz == ISZ_USHORT || sz == -ISZ_USHORT)
                        n &= 0xffff;
                }
                Optimizer::bePrintf("0%xh", n);
            }
            break;
        case Optimizer::se_fc:
            if (doSign)
                Optimizer::beputc('+');
            Optimizer::bePrintf("%s,%s", ((std::string)offset->c.r).c_str(), ((std::string)offset->c.i).c_str());
            break;
        case Optimizer::se_f:
        case Optimizer::se_fi:
            if (doSign)
                Optimizer::beputc('+');
            Optimizer::bePrintf("%s", ((std::string)offset->f).c_str());
            break;
        case Optimizer::se_labcon:
            if (doSign)
                Optimizer::beputc('+');
            Optimizer::bePrintf("L_%d", offset->i);
            break;
        case Optimizer::se_pc:
        case Optimizer::se_global:
        case Optimizer::se_threadlocal:
            if (doSign)
                Optimizer::beputc('+');
            sym = offset->sp;
            strcpy(buf, sym->outputName);
            Optimizer::bePrintf("%s", buf);
            break;
        case Optimizer::se_add:
            oa_putconst(0, ISZ_ADDR, offset->left, doSign);
            oa_putconst(0, ISZ_ADDR, offset->right, true);
            break;
        case Optimizer::se_sub:
            oa_putconst(0, ISZ_ADDR, offset->left, doSign);
            Optimizer::bePrintf("-");
            oa_putconst(0, ISZ_ADDR, offset->right, false);
            break;
        case Optimizer::se_uminus:
            Optimizer::bePrintf("-");
            oa_putconst(0, ISZ_ADDR, offset->left, false);
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

void putsizedreg(const char* string, int reg, int size)
{
    static const char* byteregs[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
    static const char* wordregs[] = {"ax", "cx", "dx", "bx", "sym", "bp", "si", "di"};
    static const char* longregs[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
    if (size < 0)
        size = -size;
    if (size == ISZ_UINT || size == ISZ_ULONG || size == ISZ_ADDR || size == ISZ_U32)
        Optimizer::bePrintf(string, longregs[reg]);
    else if (size == ISZ_BOOLEAN || size == ISZ_UCHAR)
    {
        Optimizer::bePrintf(string, byteregs[reg]);
    }
    else
        Optimizer::bePrintf(string, wordregs[reg]);
}

/*-------------------------------------------------------------------------*/

void pointersize(int size)
{
    if ((Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
         Optimizer::cparams.prm_assembler == pa_fasm) &&
        skipsize)
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
            Optimizer::bePrintf("qword ");
            break;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            if (!uses_float)
            {
                if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                    Optimizer::cparams.prm_assembler == pa_fasm)
                    Optimizer::bePrintf("dword far ");
                else
                    Optimizer::bePrintf("fword ");
                break;
            }
        case ISZ_U32:
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_ADDR:
            Optimizer::bePrintf("dword ");
            break;
        case ISZ_U16:
        case ISZ_USHORT:
        case ISZ_WCHAR:
            Optimizer::bePrintf("word ");
            break;
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            Optimizer::bePrintf("byte ");
            break;
        case ISZ_NONE:
            /* for NASM with certain FP ops */
            break;
        case ISZ_FARPTR:
            Optimizer::bePrintf("far ");
            break;
        default:
            diag("Bad pointer");
            break;
    }
    if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
          Optimizer::cparams.prm_assembler == pa_fasm) &&
        size)
        Optimizer::bePrintf("ptr ");
}

/*-------------------------------------------------------------------------*/

void putseg(int seg, int usecolon)
{
    if (!seg)
        return;
    seg -= 1;
    seg <<= 1;
    Optimizer::beputc(segregs[seg]);
    Optimizer::beputc(segregs[seg + 1]);
    if (usecolon)
        Optimizer::beputc(':');
}

/*-------------------------------------------------------------------------*/

int islabeled(Optimizer::SimpleExpression* n)
{
    int rv = 0;
    switch (n->type)
    {
        case Optimizer::se_add:
        case Optimizer::se_sub:
            //        case Optimizer::se_addstruc:
            rv |= islabeled(n->left);
            rv |= islabeled(n->right);
            break;
        case Optimizer::se_i:
        case Optimizer::se_ui:
            return 0;
        case Optimizer::se_labcon:
        case Optimizer::se_global:
        case Optimizer::se_auto:
        case Optimizer::se_absolute:
        case Optimizer::se_pc:
        case Optimizer::se_threadlocal:
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
    enum asmTypes oldnasm;

    switch (ap->mode)
    {
        case am_seg:
            putseg(ap->seg, 0);
            break;
        case am_screg:
            Optimizer::bePrintf("cr%d", ap->preg);
            break;
        case am_sdreg:
            Optimizer::bePrintf("dr%d", ap->preg);
            break;
        case am_streg:
            Optimizer::bePrintf("tr%d", ap->preg);
            break;
        case am_immed:
            if (ap->length > 0 && islabeled(ap->offset))
            {
                if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                      Optimizer::cparams.prm_assembler == pa_fasm))
                    Optimizer::bePrintf("offset ");
                else if (!nosize)
                {
                    if (ap->length == -ISZ_UCHAR || ap->length == ISZ_UCHAR)
                    {
                        Optimizer::bePrintf("byte ");
                    }
                    else if (ap->length == -ISZ_USHORT || ap->length == ISZ_USHORT)
                    {
                        Optimizer::bePrintf("word ");
                    }
                    else
                    {
                        Optimizer::bePrintf("dword ");
                    }
                }
            }
            else if ((Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm) && addsize)
                pointersize(ap->length);
            oa_putconst(op, op == op_mov ? szalt : ap->length, ap->offset, false);
            break;
        case am_direct:
            pointersize(ap->length);
            if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                  Optimizer::cparams.prm_assembler == pa_fasm))
                putseg(ap->seg, true);
            Optimizer::beputc('[');
            oldnasm = Optimizer::cparams.prm_assembler;
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                putseg(ap->seg, true);
            Optimizer::cparams.prm_assembler = pa_nasm;
            oa_putconst(0, ap->length, ap->offset, false);
            Optimizer::beputc(']');
            Optimizer::cparams.prm_assembler = oldnasm;
            break;
        case am_dreg:
            putsizedreg("%s", ap->preg, ap->length);
            break;
        case am_xmmreg:
            Optimizer::bePrintf("xmm%d", ap->preg);
            break;
        case am_mmreg:
            Optimizer::bePrintf("mm%d", ap->preg);
            break;
        case am_freg:
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                Optimizer::bePrintf("st%d", ap->preg);
            else
                Optimizer::bePrintf("st(%d)", ap->preg);
            break;
        case am_indisp:
            pointersize(ap->length);
            if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                  Optimizer::cparams.prm_assembler == pa_fasm))
                putseg(ap->seg, true);
            Optimizer::beputc('[');
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                putseg(ap->seg, true);
            putsizedreg("%s", ap->preg, ISZ_ADDR);
            if (ap->offset)
            {
                oa_putconst(0, ap->length, ap->offset, true);
            }
            Optimizer::beputc(']');
            break;
        case am_indispscale:
        {
            int scale = 1, t = ap->scale;

            while (t--)
                scale <<= 1;
            pointersize(ap->length);
            if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                  Optimizer::cparams.prm_assembler == pa_fasm))
                putseg(ap->seg, true);
            Optimizer::beputc('[');
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                putseg(ap->seg, true);
            if (ap->preg != -1)
                putsizedreg("%s+", ap->preg, ISZ_ADDR);
            putsizedreg("%s", ap->sreg, ISZ_ADDR);
            if (scale != 1)
                Optimizer::bePrintf("*%d", scale);
            if (ap->offset)
            {
                oa_putconst(0, ap->length, ap->offset, true);
            }
            Optimizer::beputc(']');
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

    if (!Optimizer::cparams.prm_asmfile)
        return;
    if (op == op_blockstart || op == op_blockend || op == op_varstart || op == op_funcstart || op == op_funcend)
        return;
    if (op == op_line)
    {
        Optimizer::LINEDATA* ld = (Optimizer::LINEDATA*)aps;
        oa_nl();
        while (ld)
        {
            Optimizer::bePrintf("; Line %d:\t%s\n", ld->lineno, ld->line);
            ld = ld->next;
        }
        return;
    }
    else if (op == op_comment)
    {
        if (!Optimizer::cparams.prm_lines)
            return;
        Optimizer::bePrintf("%s", aps);
        return;
    }
    else if (op == op_align)
    {
        oa_align(aps->offset->i);
        return;
    }
    else if (op == op_void)
        return;
    if (aps)
        len = aps->length;
    if (apd)
        len2 = apd->length;
    needpointer = (len != len2) || ((!aps || aps->mode != am_dreg) && (!apd || apd->mode != am_dreg));
    putop((e_opcode)op, aps, apd, cd->noopt);
    if ((Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
         Optimizer::cparams.prm_assembler == pa_fasm) &&
        (op >= op_ja && op <= op_jz && op != op_jecxz && (op != op_jmp || aps->mode == am_immed)))
    {
        /*
                if (cd->ins)
                {
                    if ((((Instruction*)cd->ins)->GetBytes()[0] & 0xf0) == 0x70 || ((Instruction*)cd->ins)->GetBytes()[0] == 0xeb)
                    {
                        Optimizer::bePrintf("\tshort");
                    }
                    else
                    {
                        Optimizer::bePrintf("\tnear");
                    }
                }
        */
        nosize = true;
    }
    else if (op == op_jmp && aps->mode == am_immed && aps->offset->type == Optimizer::se_labcon)
    {
        if (cd->ins)
        {
            if (((Instruction*)cd->ins)->GetBytes()[0] == 0xeb)
            {
                //                Optimizer::bePrintf("\tshort");
                nosize = true;
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
        Optimizer::bePrintf("\t");
        if ((op == op_jmp || op == op_call) && aps && apd)
        {
            separator = ':';
        }
        else
        {
            separator = ',';
        }
        if (op == op_dd)
            nosize = true;
        oa_putamode(op, aps->length, aps);
        nosize = false;
        if (apd != 0)
        {
            Optimizer::beputc(separator);
            oa_putamode(op, aps->length, apd);
        }
        if (ap3 != 0)
        {
            Optimizer::beputc(separator);
            oa_putamode(op, aps->length, ap3);
        }
    }
    Optimizer::bePrintf("\n");
}

/*-------------------------------------------------------------------------*/

void oa_gen_strlab(Optimizer::SimpleSymbol* sym)
/*
 *      generate a named label.
 */
{
    char buf[4096];
    strcpy(buf, sym->outputName);
    if (Optimizer::cparams.prm_asmfile)
    {
        if (oa_currentSeg == Optimizer::dataseg || oa_currentSeg == Optimizer::bssxseg)
        {
            newlabel = true;
            Optimizer::bePrintf("\n%s", buf);
            oa_outcol = strlen(buf) + 1;
        }
        else
            Optimizer::bePrintf("%s:\n", buf);
    }
    outcode_gen_strlab(sym);
}

/*-------------------------------------------------------------------------*/

void oa_put_label(int lab)
/*
 *      outputFile a compiler generated label.
 */
{
    if (Optimizer::cparams.prm_asmfile)
    {
        oa_nl();
        if (oa_currentSeg == Optimizer::dataseg || oa_currentSeg == Optimizer::bssxseg)
        {
            newlabel = true;
            Optimizer::bePrintf("\nL_%ld", lab);
            oa_outcol = 8;
        }
        else
            Optimizer::bePrintf("L_%ld:\n", lab);
    }
    else
        outcode_put_label(lab);
}
void oa_put_string_label(int lab, int type) { oa_put_label(lab); }

/*-------------------------------------------------------------------------*/

void oa_genfloat(enum Optimizer::e_gt type, FPF* val)
/*
 * Output a float value
 */
{
    if (Optimizer::cparams.prm_asmfile)
    {
        char buf[256];
        strcpy(buf, ((std::string)*val).c_str());
        switch (type)
        {
            case Optimizer::floatgen:
                if (!strcmp(buf, "inf") || !strcmp(buf, "nan") || !strcmp(buf, "-inf") || !strcmp(buf, "-nan"))
                {
                    Optimizer::UBYTE dta[4];
                    int i;
                    val->ToFloat(dta);
                    Optimizer::bePrintf("\tdb\t");
                    for (i = 0; i < 4; i++)
                    {
                        Optimizer::bePrintf("0%02XH", dta[i]);
                        if (i != 3)
                            Optimizer::bePrintf(", ");
                    }
                    Optimizer::beputc('\n');
                }
                else
                    Optimizer::bePrintf("\tdd\t%s\n", buf);
                break;
            case Optimizer::doublegen:
            case Optimizer::longdoublegen:
                if (!strcmp(buf, "inf") || !strcmp(buf, "nan") || !strcmp(buf, "-inf") || !strcmp(buf, "-nan"))
                {
                    Optimizer::UBYTE dta[8];
                    int i;
                    val->ToDouble(dta);
                    Optimizer::bePrintf("\tdb\t");
                    for (i = 0; i < 8; i++)
                    {
                        Optimizer::bePrintf("0%02XH", dta[i]);
                        if (i != 7)
                            Optimizer::bePrintf(", ");
                    }
                    Optimizer::beputc('\n');
                }
                else
                    Optimizer::bePrintf("\tdq\t%s\n", buf);
                break;
            default:
                diag("floatgen - invalid type");
                break;
        }
    }
    else
        switch (type)
        {
            case Optimizer::floatgen:
                outcode_genfloat(val);
                break;
            case Optimizer::doublegen:
            case Optimizer::longdoublegen:
                outcode_gendouble(val);
                break;
            default:
                diag("floatgen - invalid type");
                break;
        }
}
/*-------------------------------------------------------------------------*/

void oa_genstring(char* str, int len)
/*
 * Generate a string literal
 */
{
    bool instring = false;
    if (Optimizer::cparams.prm_asmfile)
    {
        int nlen = len;
        while (nlen--)
            if (*str >= ' ' && *str < 0x7f && *str != '\'' && *str != '\"')
            {
                if (!instring)
                {
                    oa_gentype = Optimizer::nogen;
                    oa_nl();
                    Optimizer::bePrintf("\tdb\t\"");
                    instring = true;
                }
                Optimizer::bePrintf("%c", *str++);
            }
            else
            {
                if (instring)
                {
                    Optimizer::bePrintf("\"\n");
                    instring = false;
                }
                oa_genint(Optimizer::chargen, *str++);
            }
        if (instring)
            Optimizer::bePrintf("\"\n");
    }
    else
        outcode_genstring(str, len);
}

/*-------------------------------------------------------------------------*/

void oa_genint(enum Optimizer::e_gt type, long long val)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        switch (type)
        {
            case Optimizer::chargen:
                Optimizer::bePrintf("\tdb\t0%xh\n", val & 0x00ff);
                break;
            case Optimizer::shortgen:
            case Optimizer::u16gen:
                Optimizer::bePrintf("\tdw\t0%xh\n", val & 0x0ffff);
                break;
            case Optimizer::longgen:
            case Optimizer::enumgen:
            case Optimizer::intgen:
            case Optimizer::u32gen:
                Optimizer::bePrintf("\tdd\t0%lxh\n", val);
                break;
            case Optimizer::longlonggen:
#ifndef USE_LONGLONG
                Optimizer::bePrintf("\tdd\t0%lxh,0%lxh\n", val, val < 0 ? -1 : 0);
#else
                Optimizer::bePrintf("\tdd\t0%lxh,0%lxh\n", val, val >> 32);
#endif
                break;
            case Optimizer::wchar_tgen:
                Optimizer::bePrintf("\tdw\t0%lxh\n", val);
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
            case Optimizer::chargen:
                outcode_genbyte(val);
                break;
            case Optimizer::shortgen:
            case Optimizer::u16gen:
                outcode_genword(val);
                break;
            case Optimizer::longgen:
            case Optimizer::enumgen:
            case Optimizer::intgen:
            case Optimizer::u32gen:
                outcode_genlong(val);
                break;
            case Optimizer::longlonggen:
                outcode_genlonglong(val);
                break;
            case Optimizer::wchar_tgen:
                outcode_genword(val);
                break;
            default:
                diag("genint - unknown type");
        }
    }
}
void oa_genaddress(unsigned long long val) { oa_genint(Optimizer::longgen, val); }
/*-------------------------------------------------------------------------*/

void oa_gensrref(Optimizer::SimpleSymbol* sym, int val, int type)
{
    char buf[4096];
    if (Optimizer::cparams.prm_asmfile)
    {
        strcpy(buf, sym->outputName);
        oa_nl();
        Optimizer::bePrintf("\tdb\t0,%d\n", val);
        Optimizer::bePrintf("\tdd\t%s\n", buf);
        oa_gentype = Optimizer::srrefgen;
    }
    else
        outcode_gensrref(sym, val);
}

/*-------------------------------------------------------------------------*/

void oa_genref(Optimizer::SimpleSymbol* sym, int offset)
/*
 * Output a reference to the data area (also gens fixups )
 */
{
    char sign;
    char buf[4096], buf1[4096];
    if (Optimizer::cparams.prm_asmfile)
    {
        if (offset < 0)
        {
            sign = '-';
            offset = -offset;
        }
        else
            sign = '+';
        strcpy(buf, sym->outputName);
        sprintf(buf1, "%s%c%d", buf, sign, offset);
        if (Optimizer::cparams.prm_asmfile)
        {
            if (!newlabel)
                oa_nl();
            else
                newlabel = false;
            Optimizer::bePrintf("\tdd\t%s\n", buf1);
            oa_gentype = Optimizer::longgen;
        }
    }
    else
        outcode_genref(sym, offset);
}

/*-------------------------------------------------------------------------*/

void oa_genpcref(Optimizer::SimpleSymbol* sym, int offset)
/*
 * Output a reference to the code area (also gens fixups )
 */
{
    oa_genref(sym, offset);
}

/*-------------------------------------------------------------------------*/

void oa_genstorage(int nbytes)
/*
 * Output bytes of storage
 */
{
    if (Optimizer::cparams.prm_asmfile)
    {
        if (!newlabel)
            oa_nl();
        else
            newlabel = false;
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
            Optimizer::bePrintf("\tresb\t0%xh\n", nbytes);
        else
            Optimizer::bePrintf("\tdb\t0%xh DUP (?)\n", nbytes);
        oa_gentype = Optimizer::nogen;
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
    if (Optimizer::cparams.prm_asmfile)
    {

        if (!newlabel)
            oa_nl();
        else
            newlabel = false;
        Optimizer::bePrintf("\tdd\tL_%d\n", n);
        oa_gentype = Optimizer::longgen;
    }
    else
        outcode_gen_labref(n);
}

/*-------------------------------------------------------------------------*/

void oa_gen_labdifref(int n1, int n2)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        if (!newlabel)
            oa_nl();
        else
            newlabel = false;
        Optimizer::bePrintf("\tdd\tL_%d-L_%d\n", n1, n2);
        oa_gentype = Optimizer::longgen;
    }
    else
        outcode_gen_labdifref(n1, n2);
}

/*
 * Exit if from a special segment
 */
void oa_exitseg(enum Optimizer::e_sg seg)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
              Optimizer::cparams.prm_assembler == pa_fasm))
        {
            if (seg == Optimizer::startupxseg)
            {
                Optimizer::bePrintf("cstartup\tENDS\n");
            }
            else if (seg == Optimizer::rundownxseg)
            {
                Optimizer::bePrintf("crundown\tENDS\n");
            }
            else if (seg == Optimizer::constseg)
            {
                Optimizer::bePrintf("_CONST\tENDS\n");
            }
            else if (seg == Optimizer::stringseg)
            {
                Optimizer::bePrintf("_STRING\tENDS\n");
            }
            else if (seg == Optimizer::tlsseg)
            {
                Optimizer::bePrintf("_TLS\tENDS\n");
            }
            else if (seg == Optimizer::tlssuseg)
            {
                Optimizer::bePrintf("tlsstartup\tENDS\n");
            }
            else if (seg == Optimizer::tlsrdseg)
            {
                Optimizer::bePrintf("tlsrundown\tENDS\n");
            }
        }
        oa_nl();
    }
}

/*
 * Switch to cseg
 */
void oa_enterseg(enum Optimizer::e_sg seg)
{
    oa_currentSeg = seg;
    outcode_enterseg(seg);
    if (Optimizer::cparams.prm_asmfile)
    {
        if (seg == Optimizer::codeseg)
        {
            oa_nl();
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section code\n");
                else
                {
                    Optimizer::bePrintf("section .text\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            else
                Optimizer::bePrintf("\t.code\n");
        }
        else if (seg == Optimizer::constseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section const\n");
                else
                {
                    Optimizer::bePrintf("section .text\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            }
            else
                Optimizer::bePrintf("_CONST\tsegment use32 public dword \042CONST\042\n");
        }
        else if (seg == Optimizer::stringseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section string\n");
                else
                {
                    Optimizer::bePrintf("section .data\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            }
            else
                Optimizer::bePrintf("_STRING\tsegment use32 public dword \042STRING\042\n");
        }
        else if (seg == Optimizer::dataseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section data\n");
                else
                {
                    Optimizer::bePrintf("section .data\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            else
                Optimizer::bePrintf("\t.DATA\n");
        }
        else if (seg == Optimizer::tlsseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section tls\n");
                else
                {
                    Optimizer::bePrintf("section .data\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            else
                Optimizer::bePrintf("_TLS\tsegment use32 public dword \042TLS\042\n");
        }
        else if (seg == Optimizer::tlssuseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section tlsstartup\n");
                else
                {
                    Optimizer::bePrintf("section .data\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            else
                Optimizer::bePrintf("_TLS\tsegment use32 public dword \042TLS\042\n");
        }
        else if (seg == Optimizer::tlsrdseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section tlsrundown\n");
                else
                {
                    Optimizer::bePrintf("section .data\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            else
                Optimizer::bePrintf("_TLS\tsegment use32 public dword \042TLS\042\n");
        }
        else if (seg == Optimizer::bssxseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section bss\n");
                else
                    Optimizer::bePrintf("section .bss\n");
            else
                Optimizer::bePrintf("\t.data?\n");
        }
        else if (seg == Optimizer::startupxseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section cstartup\n");
                else
                {
                    Optimizer::bePrintf("section .text\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            }
            else
                Optimizer::bePrintf("cstartup\tsegment use32 public dword \042INITDATA\042\n");
        }
        else if (seg == Optimizer::rundownxseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    Optimizer::bePrintf("section crundown\n");
                else
                {
                    Optimizer::bePrintf("section .text\n");
                    Optimizer::bePrintf("[bits 32]\n");
                }
            }
            else
                Optimizer::bePrintf("crundown\tsegment use32 public dword \042EXITDATA\042\n");
        }
    }
}

/*-------------------------------------------------------------------------*/

void oa_gen_virtual(Optimizer::SimpleSymbol* sym, int data)
{
    virtual_mode = data;
    oa_currentSeg = Optimizer::virtseg;
    if (Optimizer::cparams.prm_asmfile)
    {
        oa_nl();
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
        {
            oa_currentSeg = Optimizer::noseg;
#ifdef IEEE
            if (sym->outputName[0] == '@')
                if (virtual_mode)
                    Optimizer::bePrintf("\tsection vsd%s virtual\n", sym->outputName);
                else
                    Optimizer::bePrintf("\tsection vsc%s virtual\n", sym->outputName);
            else if (virtual_mode)
                Optimizer::bePrintf("\tsection vsd@%s virtual\n", sym->outputName);
            else
                Optimizer::bePrintf("\tsection vsc@%s virtual\n", sym->outputName);
#else
            Optimizer::bePrintf("\tSECTION @%s VIRTUAL\n", sym->outputName);
#endif
        }
        else
            Optimizer::bePrintf("@%s\tsegment virtual\n", sym->outputName);
        Optimizer::bePrintf("\t[bits 32]\n");
        if (Optimizer::cparams.prm_assembler != pa_oasm)
        {
            oa_globaldef(sym);
        }
        Optimizer::bePrintf("%s:\n", sym->outputName);
    }
    outcode_start_virtual_seg(sym, data);
}
void oa_gen_endvirtual(Optimizer::SimpleSymbol* sym)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        oa_nl();
        if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
              Optimizer::cparams.prm_assembler == pa_fasm))
        {
            Optimizer::bePrintf("@%s\tends\n", sym->outputName);
        }
        else if (virtual_mode)
            oa_enterseg(Optimizer::dataseg);
        else
            oa_enterseg(Optimizer::codeseg);
    }
    else
    {
        if (virtual_mode)
            oa_enterseg(Optimizer::dataseg);
        else
            oa_enterseg(Optimizer::codeseg);
        outcode_end_virtual_seg(sym);
    }
}
/*
 * Align
 * not really honorign the size... all alignments are mod 4...
 */
void oa_align(int size)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        oa_nl();
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
            /* NASM 0.91 wouldn't let me use parenthesis but this should work
             * according to the documented precedence levels
             */
            Optimizer::bePrintf("\ttimes $$-$ & %d nop\n", size - 1);
        else
            Optimizer::bePrintf("\talign\t%d\n", size);
    }
    else
    {
        outcode_align(size);
    }
}
void oa_setalign(int code, int data, int bss, int constant)
{
    if (code > 0)
        segAligns[Optimizer::codeseg] = code;
    if (data > 0)
        segAligns[Optimizer::dataseg] = data;
    if (bss > 0)
        segAligns[Optimizer::bssxseg] = bss;
    if (constant > 0)
        segAligns[Optimizer::constseg] = constant;
}

/*
 * queue muldiv val
 */
long queue_muldivval(long number)
{
    MULDIV *p = muldivlink, **q = &muldivlink;
    if (Optimizer::cparams.prm_mergestrings)
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

    while (*q)
        q = &(*q)->next;
    for (int i = 0; i < count; i++, q = &(*q)->next)
    {
        p = (MULDIV*)beGlobalAlloc(sizeof(MULDIV));
        p->value = constant[i];
        if (i == 0)
            p->label = lbl;
    }
    return lbl;
}
/*-------------------------------------------------------------------------*/

long queue_floatval(FPF* number, int size)
{
    MULDIV *p = muldivlink, **q = &muldivlink;
    if (Optimizer::cparams.prm_mergestrings)
    {
        while (p)
        {
            if (p->size == size && (number != nullptr) && p->floatvalue == *number)
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
    int tag = false;
    oa_enterseg(Optimizer::constseg);
    if (Optimizer::cparams.prm_asmfile)
    {
        Optimizer::bePrintf("\n");
        if (muldivlink)
        {
            tag = true;
        }
        while (muldivlink)
        {
            oa_align(8);
            if (muldivlink->label)
                oa_put_label(muldivlink->label);
            if (muldivlink->size == ISZ_NONE)
                Optimizer::bePrintf("\tdd\t0%xh\n", muldivlink->value);
            else
            {
                char buf[256];
                Optimizer::UBYTE data[12];
                int len = 0;
                int i;
                switch (muldivlink->size)
                {
                    case ISZ_FLOAT:
                    case ISZ_IFLOAT:
                    case ISZ_CFLOAT:
                        muldivlink->floatvalue.ToFloat(data);
                        len = 4;
                        break;
                    case ISZ_DOUBLE:
                    case ISZ_IDOUBLE:
                    case ISZ_LDOUBLE:
                    case ISZ_ILDOUBLE:
                    case ISZ_CDOUBLE:
                    case ISZ_CLDOUBLE:
                        muldivlink->floatvalue.ToDouble(data);
                        len = 8;
                        break;
                }
                Optimizer::bePrintf("\tdb\t");
                for (i = 0; i < len; i++)
                {
                    Optimizer::bePrintf("0%02XH", data[i]);
                    if (i != len - 1)
                        Optimizer::bePrintf(",");
                }
                Optimizer::bePrintf("\n");
            }
            muldivlink = muldivlink->next;
        }
        if (tag)
            Optimizer::bePrintf("\n");
    }
    else
        outcode_dump_muldivval();
}

/*-------------------------------------------------------------------------*/
char asmfile[256];
char compilerversion[256];
void oa_header(const char* filename, const char* compiler_version)
{
    strcpy(asmfile, filename);
    strcpy(compilerversion, compiler_version);
    oa_nl();
    Optimizer::bePrintf(";File %s\n", filename);
    Optimizer::bePrintf(";Compiler version %s\n", compiler_version);
    if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
        Optimizer::cparams.prm_assembler == pa_fasm)
    {
        if (!Optimizer::cparams.prm_nodos)
        {
            Optimizer::bePrintf("\tsection code align=%-8d use32\n", segAligns[Optimizer::codeseg]);
            Optimizer::bePrintf("\tsection data align=%-8d use32\n", segAligns[Optimizer::dataseg]);
            Optimizer::bePrintf("\tsection bss  align=%-8d use32\n", segAligns[Optimizer::bssxseg]);
            Optimizer::bePrintf("\tsection const  align=%-8d use32\n", segAligns[Optimizer::constseg]);
            Optimizer::bePrintf("\tsection string  align=%-8d use32\n", segAligns[Optimizer::stringseg]);
            Optimizer::bePrintf("\tsection tls  align=8 use32\n");
            Optimizer::bePrintf("\tsection cstartup align=2 use32\n");
            Optimizer::bePrintf("\tsection crundown align=2 use32\n");
            //            Optimizer::bePrintf(
            //               "\tSECTION cppinit  align=4 CLASS=CPPINIT USE32\n");
            //          Optimizer::bePrintf(
            //             "\tSECTION cppexit  align=4 CLASS=CPPEXIT USE32\n");
            //        Optimizer::bePrintf( "\tGROUP DGROUP _DATA _BSS _CONST _STRING\n\n");
        }
        else
        {
            Optimizer::bePrintf("\tsection .text\n");
            Optimizer::bePrintf("\tsection .data\n");
            Optimizer::bePrintf("\tsection .bss\n");
        }
    }
    else
    {
        Optimizer::bePrintf("\ttitle\t'%s'\n", filename);
        if (Optimizer::cparams.prm_flat)
            Optimizer::bePrintf("\t.486p\n\t.model flat\n\n");
        else if (Optimizer::cparams.prm_assembler == pa_masm)
            Optimizer::bePrintf("\t.486p\n\t.model small\n\n");
        else
            Optimizer::bePrintf("\t.486p\n\t.model use32 small\n\n");
    }
}
void oa_trailer(void)
{
    if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
          Optimizer::cparams.prm_assembler == pa_fasm))
        Optimizer::bePrintf("\tend\n");
    Optimizer::beRewind();
    oa_header(asmfile, compilerversion);
}
/*-------------------------------------------------------------------------*/
void oa_localdef(Optimizer::SimpleSymbol* sym)
{
    if (!Optimizer::cparams.prm_asmfile)
    {
        omf_globaldef(sym);
    }
}
void oa_localstaticdef(Optimizer::SimpleSymbol* sym)
{
    if (!Optimizer::cparams.prm_asmfile)
    {
        omf_globaldef(sym);
    }
}
void oa_globaldef(Optimizer::SimpleSymbol* sym)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        char buf[5000];
        strcpy(buf, sym->outputName);
        oa_nl();
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
            Optimizer::bePrintf("[global\t%s]\n", buf);
        else
            Optimizer::bePrintf("\tpublic\t%s\n", buf);
    }
    else
        omf_globaldef(sym);
}

/*-------------------------------------------------------------------------*/

void oa_output_alias(char* name, char* alias)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        oa_nl();
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
            Optimizer::bePrintf("%%define %s %s\n", name, alias);
        else
            Optimizer::bePrintf("%s equ\t<%s>\n", name, alias);
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_extern(Optimizer::SimpleSymbol* sym, int code)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        oa_nl();
        char buf[5000];
        strcpy(buf, sym->outputName);
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
        {
            Optimizer::bePrintf("[extern\t%s]\n", buf);
        }
        else
        {
            if (code)
                Optimizer::bePrintf("\textrn\t%s:proc\n", buf);
            else
                Optimizer::bePrintf("\textrn\t%s:byte\n", buf);
        }
    }
    else
        omf_put_extern(sym, code);
}
/*-------------------------------------------------------------------------*/

void oa_put_impfunc(Optimizer::SimpleSymbol* sym, const char* file)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        char buf[5000];
        strcpy(buf, sym->outputName);
        Optimizer::bePrintf("\timport %s %s\n", buf, file);
    }
    else
    {
        omf_put_impfunc(sym, file);
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_expfunc(Optimizer::SimpleSymbol* sym)
{
    char buf[4096];
    if (Optimizer::cparams.prm_asmfile)
    {
        strcpy(buf, sym->outputName);
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
            Optimizer::bePrintf("\texport %s\n", buf);
        else
            Optimizer::bePrintf("\tpublicdll %s\n", buf);
    }
    else
        omf_put_expfunc(sym);
}

void oa_output_includelib(const char* name)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
              Optimizer::cparams.prm_assembler == pa_fasm))
            Optimizer::bePrintf("\tincludelib %s\n", name);
    }
    else
        omf_put_includelib(name);
}
void oa_end_generation(void) { dump_muldivval(); }
}  // namespace occx86