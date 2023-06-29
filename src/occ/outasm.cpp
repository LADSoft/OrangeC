/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include "ObjSymbol.h"
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
#include "stdarg.h"
#include "symfuncs.h"
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
static int nosize = 0;
static int virtual_mode;
static int linepos;
/* Init module */
void oa_ini(void)
{
    linepos = 0;
    oa_gentype = Optimizer::nogen;
    oa_currentSeg = Optimizer::noseg;
    newlabel = false;
    muldivlink = 0;
}
static void ColumnPosition(int n)
{
    if (n <= linepos)
    {
        Optimizer::beputc(' ');
        linepos++;
    }
    else
    {
        char buf[100];
        memset(buf, ' ', n - linepos);
        buf[n - linepos] = 0;
        Optimizer::bePrintf("%s", buf);
        linepos += n - linepos;
    }
}
static void AsmOutput(const char* fmt, ...)
{
    char buf[10000], *p = buf, *q;
    va_list lst;
    va_start(lst, fmt);
    vsprintf(buf, fmt, lst);
    va_end(lst);

    while ((q = (char*)strrchr(p, '\n')) != 0)
    {
        linepos = 0;
        q++;
        p = q;
    }
    int len = strlen(p);
    linepos += len;
    Optimizer::bePrintf("%s", buf);
}
/*-------------------------------------------------------------------------*/

void oa_nl(void)
/*
 * New line
 */
{
    if (Optimizer::cparams.prm_asmfile)
    {
        if (linepos > 0)
        {
            Optimizer::bePrintf("\n");
            oa_gentype = Optimizer::nogen;
        }
    }
}

/* Put an opcode
 */
void outop(const char* name)
{
    ColumnPosition(8);
    while (*name)
    {
        Optimizer::beputc(*name++);
        linepos++;
    }
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
                    AsmOutput("-0%lxh", -offset->sp->offset);
                else
                    AsmOutput("+0%lxh", m);
            }
            else
                AsmOutput("0%lxh", m);

            break;
        case Optimizer::se_i:
        case Optimizer::se_ui:
        case Optimizer::se_absolute:
            if (doSign)
            {
                if (offset->i == 0)
                    break;
                Optimizer::beputc('+');
                linepos++;
            }
            {
                int n = offset->i;
                if (op == op_mov)
                {
                    if (sz == ISZ_UCHAR || sz == -ISZ_UCHAR)
                        n &= 0xff;
                    if (sz == ISZ_USHORT || sz == -ISZ_USHORT || sz == ISZ_U16 || sz == ISZ_WCHAR)
                        n &= 0xffff;
                }
                AsmOutput("0%xh", n);
            }
            break;
        case Optimizer::se_fc:
            if (doSign)
            {
                Optimizer::beputc('+');
                linepos++;
            }
            AsmOutput("%s,%s", ((std::string)offset->c.r).c_str(), ((std::string)offset->c.i).c_str());
            break;
        case Optimizer::se_f:
        case Optimizer::se_fi:
            if (doSign)
            {
                Optimizer::beputc('+');
                linepos++;
            }
            AsmOutput("%s", ((std::string)offset->f).c_str());
            break;
        case Optimizer::se_labcon:
            if (doSign)
            {
                Optimizer::beputc('+');
                linepos++;
            }
            AsmOutput("L_%d", offset->i);
            break;
        case Optimizer::se_pc:
        case Optimizer::se_global:
        case Optimizer::se_threadlocal:
            if (doSign)
            {
                Optimizer::beputc('+');
                linepos++;
            }
            sym = offset->sp;
            strcpy(buf, sym->outputName);
            AsmOutput("%s", buf);
            break;
        case Optimizer::se_add:
            oa_putconst(0, ISZ_ADDR, offset->left, doSign);
            oa_putconst(0, ISZ_ADDR, offset->right, true);
            break;
        case Optimizer::se_sub:
            oa_putconst(0, ISZ_ADDR, offset->left, doSign);
            AsmOutput("-");
            oa_putconst(0, ISZ_ADDR, offset->right, false);
            break;
        case Optimizer::se_uminus:
            AsmOutput("-");
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
        AsmOutput(string, longregs[reg]);
    else if (size == ISZ_BOOLEAN || size == ISZ_UCHAR)
    {
        AsmOutput(string, byteregs[reg]);
    }
    else
        AsmOutput(string, wordregs[reg]);
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
            AsmOutput("qword ");
            break;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            if (!uses_float)
            {
                if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                    Optimizer::cparams.prm_assembler == pa_fasm)
                    AsmOutput("dword far ");
                else
                    AsmOutput("fword ");
                break;
            }
        case ISZ_U32:
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_ADDR:
            AsmOutput("dword ");
            break;
        case ISZ_U16:
        case ISZ_USHORT:
        case ISZ_WCHAR:
            AsmOutput("word ");
            break;
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            AsmOutput("byte ");
            break;
        case ISZ_NONE:
            /* for NASM with certain FP ops */
            break;
        case ISZ_FARPTR:
            AsmOutput("far ");
            break;
        default:
            diag("Bad pointer");
            break;
    }
    if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
          Optimizer::cparams.prm_assembler == pa_fasm) &&
        size)
        AsmOutput("ptr ");
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
    linepos += 2;
    if (usecolon)
    {
        Optimizer::beputc(':');
        linepos++;
    }
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
            AsmOutput("cr%d", ap->preg);
            break;
        case am_sdreg:
            AsmOutput("dr%d", ap->preg);
            break;
        case am_streg:
            AsmOutput("tr%d", ap->preg);
            break;
        case am_immed:
            if (ap->length > 0 && islabeled(ap->offset))
            {
                if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                      Optimizer::cparams.prm_assembler == pa_fasm))
                    AsmOutput("offset ");
                else if (!nosize)
                {
                    if (ap->length == -ISZ_UCHAR || ap->length == ISZ_UCHAR)
                    {
                        AsmOutput("byte ");
                    }
                    else if (ap->length == -ISZ_USHORT || ap->length == ISZ_USHORT || ap->length == ISZ_U16 ||
                             ap->length == ISZ_WCHAR)
                    {
                        AsmOutput("word ");
                    }
                    else
                    {
                        AsmOutput("dword ");
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
            linepos++;
            oldnasm = Optimizer::cparams.prm_assembler;
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                putseg(ap->seg, true);
            Optimizer::cparams.prm_assembler = pa_nasm;
            oa_putconst(0, ap->length, ap->offset, false);
            Optimizer::beputc(']');
            linepos++;
            Optimizer::cparams.prm_assembler = oldnasm;
            break;
        case am_dreg:
            putsizedreg("%s", ap->preg, ap->length);
            break;
        case am_xmmreg:
            AsmOutput("xmm%d", ap->preg);
            break;
        case am_mmreg:
            AsmOutput("mm%d", ap->preg);
            break;
        case am_freg:
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                AsmOutput("st%d", ap->preg);
            else
                AsmOutput("st(%d)", ap->preg);
            break;
        case am_indisp:
            pointersize(ap->length);
            if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                  Optimizer::cparams.prm_assembler == pa_fasm))
                putseg(ap->seg, true);
            Optimizer::beputc('[');
            linepos++;
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                putseg(ap->seg, true);
            putsizedreg("%s", ap->preg, ISZ_ADDR);
            if (ap->offset)
            {
                oa_putconst(0, ap->length, ap->offset, true);
            }
            Optimizer::beputc(']');
            linepos++;
            break;
        case am_indispscale: {
            int scale = 1, t = ap->scale;

            while (t--)
                scale <<= 1;
            pointersize(ap->length);
            if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                  Optimizer::cparams.prm_assembler == pa_fasm))
                putseg(ap->seg, true);
            Optimizer::beputc('[');
            linepos++;
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                putseg(ap->seg, true);
            if (ap->preg != -1)
                putsizedreg("%s+", ap->preg, ISZ_ADDR);
            putsizedreg("%s", ap->sreg, ISZ_ADDR);
            if (scale != 1)
                AsmOutput("*%d", scale);
            if (ap->offset)
            {
                oa_putconst(0, ap->length, ap->offset, true);
            }
            Optimizer::beputc(']');
            linepos++;
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
        oa_nl();
        auto ld = (Optimizer::LINEDATA*)aps;
        AsmOutput("; Line %d:", ld->lineno);
        ColumnPosition(8);
        AsmOutput("%s\n", ld->line);
        return;
    }
    else if (op == op_comment)
    {
        if (!Optimizer::cparams.prm_lines)
            return;
        AsmOutput("%s", aps);
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
                        ColumnPosition(16);
                        AsmOutput("short");
                    }
                    else
                    {
                        ColumnPosition(16);
                        AsmOutput("near");
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

        ColumnPosition(16);
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
            linepos++;
            oa_putamode(op, aps->length, apd);
        }
        if (ap3 != 0)
        {
            Optimizer::beputc(separator);
            linepos++;
            oa_putamode(op, aps->length, ap3);
        }
    }
    Optimizer::SimpleExpression* nmexp = nullptr;
    if (aps && aps->offset)
    {
        nmexp = Optimizer::GetSymRef(aps->offset);
    }
    else if (!nmexp && apd && apd->offset)
    {
        nmexp = Optimizer::GetSymRef(apd->offset);
    }
    if (nmexp && nmexp->type == Optimizer::se_pc && nmexp->sp->tp->type == Optimizer::st_func)
    {
        AsmOutput(" ; %s", ObjSymbol(nmexp->sp->outputName, ObjSymbol::eGlobal, 0).GetDisplayName().c_str());
    }
    AsmOutput("\n");
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
        if (sym && sym->tp->type == Optimizer::st_func)
            AsmOutput("; %s\n", ObjSymbol(sym->outputName, ObjSymbol::eGlobal, 0).GetDisplayName().c_str());
        if (oa_currentSeg == Optimizer::dataseg || oa_currentSeg == Optimizer::bssxseg)
        {
            newlabel = true;
            AsmOutput("\n%s", buf);
        }
        else
            AsmOutput("%s:\n", buf);
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
            AsmOutput("\nL_%ld", lab);
        }
        else
            AsmOutput("L_%ld:\n", lab);
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
                    ColumnPosition(8);
                    AsmOutput("db");
                    ColumnPosition(16);
                    for (i = 0; i < 4; i++)
                    {
                        AsmOutput("0%02XH", dta[i]);
                        if (i != 3)
                            AsmOutput(", ");
                    }
                    Optimizer::beputc('\n');
                    linepos = 0;
                }
                else
                    ColumnPosition(8);
                AsmOutput("dd");
                ColumnPosition(16);
                AsmOutput("%s\n", buf);
                break;
            case Optimizer::doublegen:
            case Optimizer::longdoublegen:
                if (!strcmp(buf, "inf") || !strcmp(buf, "nan") || !strcmp(buf, "-inf") || !strcmp(buf, "-nan"))
                {
                    Optimizer::UBYTE dta[8];
                    int i;
                    val->ToDouble(dta);
                    ColumnPosition(8);
                    AsmOutput("db");
                    ColumnPosition(16);
                    for (i = 0; i < 8; i++)
                    {
                        AsmOutput("0%02XH", dta[i]);
                        if (i != 7)
                            AsmOutput(", ");
                    }
                    Optimizer::beputc('\n');
                    linepos = 0;
                }
                else
                {
                    ColumnPosition(8);
                    AsmOutput("dq");
                    ColumnPosition(16);
                    AsmOutput("%s\n", buf);
                }
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
        while (nlen)
        {
            for (int i = 0; i < 80 && nlen; i++, nlen--)
            {
                if (*str >= ' ' && *str < 0x7f && *str != '\'' && *str != '\"')
                {
                    if (!instring)
                    {
                        oa_gentype = Optimizer::nogen;
                        oa_nl();
                        ColumnPosition(8);
                        AsmOutput("db");
                        ColumnPosition(8);
                        AsmOutput("\"");
                        instring = true;
                    }
                    AsmOutput("%c", *str++);
                }
                else
                {
                    if (instring)
                    {
                        AsmOutput("\"\n");
                        instring = false;
                    }
                    oa_genint(Optimizer::chargen, *str++);
                }
            }
            if (instring)
                AsmOutput("\"\n");
            instring = false;
        }
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
                ColumnPosition(8);
                AsmOutput("db");
                ColumnPosition(16);
                AsmOutput("0%xh\n", val & 0x00ff);
                break;
            case Optimizer::shortgen:
            case Optimizer::u16gen:
                ColumnPosition(8);
                AsmOutput("dw");
                ColumnPosition(16);
                AsmOutput("0%xh\n", val & 0x0ffff);
                break;
            case Optimizer::longgen:
            case Optimizer::enumgen:
            case Optimizer::intgen:
            case Optimizer::u32gen:
                ColumnPosition(8);
                AsmOutput("dd");
                ColumnPosition(16);
                AsmOutput("0%lxh\n", val);
                break;
            case Optimizer::longlonggen:
                ColumnPosition(8);
                AsmOutput("dd");
                ColumnPosition(16);
#ifndef USE_LONGLONG
                AsmOutput("0%lxh,0%lxh\n", val, val < 0 ? -1 : 0);
#else
                AsmOutput("0%lxh,0%lxh\n", val, val >> 32);
#endif
                break;
            case Optimizer::wchar_tgen:
                ColumnPosition(8);
                AsmOutput("d2");
                ColumnPosition(16);
                AsmOutput("0%lxh\n", val);
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
        ColumnPosition(8);
        AsmOutput("db");
        ColumnPosition(16);
        AsmOutput("0,%d\n", val);
        ColumnPosition(8);
        AsmOutput("dd");
        ColumnPosition(16);
        AsmOutput("%s\n", buf);
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
            ColumnPosition(8);
            AsmOutput("dd");
            ColumnPosition(16);
            AsmOutput("%s\n", buf1);
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
        {
            ColumnPosition(8);
            AsmOutput("resb");
            ColumnPosition(16);
            AsmOutput("0%xh\n", nbytes);
        }
        else
        {
            ColumnPosition(8);
            AsmOutput("db");
            ColumnPosition(16);
            AsmOutput("0%xh DUP (?)\n", nbytes);
        }
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
        ColumnPosition(8);
        AsmOutput("dd");
        ColumnPosition(16);
        AsmOutput("L_%d\n", n);
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
        ColumnPosition(8);
        AsmOutput("dd");
        ColumnPosition(16);
        AsmOutput("L_%d-L_%d\n", n1, n2);
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
                ColumnPosition(8);
                AsmOutput("cstartup");
                ColumnPosition(16);
                AsmOutput("ENDS\n");
            }
            else if (seg == Optimizer::rundownxseg)
            {
                ColumnPosition(8);
                AsmOutput("crundown");
                ColumnPosition(16);
                AsmOutput("ENDS\n");
            }
            else if (seg == Optimizer::constseg)
            {
                ColumnPosition(8);
                AsmOutput("_CONST");
                ColumnPosition(16);
                AsmOutput("ENDS\n");
            }
            else if (seg == Optimizer::stringseg)
            {
                ColumnPosition(8);
                AsmOutput("_STRING");
                ColumnPosition(16);
                AsmOutput("ENDS\n");
            }
            else if (seg == Optimizer::tlsseg)
            {
                ColumnPosition(8);
                AsmOutput("_TLS");
                ColumnPosition(16);
                AsmOutput("ENDS\n");
            }
            else if (seg == Optimizer::tlssuseg)
            {
                ColumnPosition(8);
                AsmOutput("tstartup");
                ColumnPosition(16);
                AsmOutput("ENDS\n");
            }
            else if (seg == Optimizer::tlsrdseg)
            {
                ColumnPosition(8);
                AsmOutput("trundown");
                ColumnPosition(16);
                AsmOutput("ENDS\n");
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
                    AsmOutput("section code\n");
                else
                {
                    AsmOutput("section .text\n");
                    AsmOutput("[bits 32]\n");
                }
            else
            {
                ColumnPosition(8);
                AsmOutput(".code");
            }
        }
        else if (seg == Optimizer::constseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section const\n");
                else
                {
                    AsmOutput("section .text\n");
                    AsmOutput("[bits 32]\n");
                }
            }
            else
            {
                ColumnPosition(8);
                AsmOutput("_CONST");
                ColumnPosition(16);
                AsmOutput("segment use32 public dword \042CONST\042\n");
            }
        }
        else if (seg == Optimizer::stringseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section string\n");
                else
                {
                    AsmOutput("section .data\n");
                    AsmOutput("[bits 32]\n");
                }
            }
            else
            {
                ColumnPosition(8);
                AsmOutput("_STRING");
                ColumnPosition(16);
                AsmOutput("segment use32 public dword \042STRING\042\n");
            }
        }
        else if (seg == Optimizer::dataseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section data\n");
                else
                {
                    AsmOutput("section .data\n");
                    AsmOutput("[bits 32]\n");
                }
            else
            {
                ColumnPosition(8);
                AsmOutput(".DATA\n");
            }
        }
        else if (seg == Optimizer::tlsseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section tls\n");
                else
                {
                    AsmOutput("section .data\n");
                    AsmOutput("[bits 32]\n");
                }
            else
            {
                ColumnPosition(8);
                AsmOutput("_TLS");
                ColumnPosition(16);
                AsmOutput("segment use32 public dword \042TLS\042\n");
            }
        }
        else if (seg == Optimizer::tlssuseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section tstartup\n");
                else
                {
                    AsmOutput("section .data\n");
                    AsmOutput("[bits 32]\n");
                }
            else
            {
                ColumnPosition(8);
                AsmOutput("_TLS");
                ColumnPosition(16);
                AsmOutput("segment use32 public dword \042TLS\042\n");
            }
        }
        else if (seg == Optimizer::tlsrdseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section trundown\n");
                else
                {
                    AsmOutput("section .data\n");
                    AsmOutput("[bits 32]\n");
                }
            else
            {
                ColumnPosition(8);
                AsmOutput("_TLS");
                ColumnPosition(16);
                AsmOutput("segment use32 public dword \042TLS\042\n");
            }
        }
        else if (seg == Optimizer::bssxseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section bss\n");
                else
                    AsmOutput("section .bss\n");
            else
            {
                ColumnPosition(8);
                AsmOutput(".data?\n");
            }
        }
        else if (seg == Optimizer::startupxseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section cstartup\n");
                else
                {
                    AsmOutput("section .text\n");
                    AsmOutput("[bits 32]\n");
                }
            }
            else
            {
                ColumnPosition(8);
                AsmOutput("cstartup");
                ColumnPosition(16);
                AsmOutput("segment use32 public dword \042INITDATA\042\n");
            }
        }
        else if (seg == Optimizer::rundownxseg)
        {
            if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
                Optimizer::cparams.prm_assembler == pa_fasm)
            {
                if (!Optimizer::cparams.prm_nodos)
                    AsmOutput("section crundown\n");
                else
                {
                    AsmOutput("section .text\n");
                    AsmOutput("[bits 32]\n");
                }
            }
            else
            {
                ColumnPosition(8);
                AsmOutput("crundown");
                ColumnPosition(16);
                AsmOutput("segment use32 public dword \042EXITDATA\042\n");
            }
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
            ColumnPosition(8);
#ifdef IEEE
            if (sym->outputName[0] == '@')
                if (virtual_mode)
                    AsmOutput("section vsd%s virtual\n", sym->outputName);
                else
                    AsmOutput("section vsc%s virtual\n", sym->outputName);
            else if (virtual_mode)
                AsmOutput("section vsd@%s virtual\n", sym->outputName);
            else
                AsmOutput("section vsc@%s virtual\n", sym->outputName);
#else
            AsmOutput("SECTION @%s VIRTUAL\n", sym->outputName);
#endif
        }
        else
        {
            AsmOutput("@%s", sym->outputName);
            ColumnPosition(8);
            AsmOutput("segment virtual\n");
        }
        ColumnPosition(8);
        AsmOutput("[bits 32]\n");
        if (Optimizer::cparams.prm_assembler != pa_oasm && !sym->isinternal)
        {
            oa_globaldef(sym);
        }
        if (sym && sym->tp->type == Optimizer::st_func)
            AsmOutput("; %s\n", ObjSymbol(sym->outputName, ObjSymbol::eGlobal, 0).GetDisplayName().c_str());
        AsmOutput("%s:\n", sym->outputName);
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
            AsmOutput("@%s", sym->outputName);
            ColumnPosition(8);
            AsmOutput("ends\n");
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
        ColumnPosition(8);
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
        /* NASM 0.91 wouldn't let me use parenthesis but this should work
         * according to the documented precedence levels
         */
        {
            AsmOutput("times $$-$ & %d nop\n", size - 1);
        }
        else
        {
            AsmOutput("align");
            ColumnPosition(8);
            AsmOutput("%d\n", size);
        }
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
    p = beGlobalAllocate<MULDIV>();
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
        p = beGlobalAllocate<MULDIV>();
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
    p = beGlobalAllocate<MULDIV>();
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
        AsmOutput("\n");
        if (muldivlink)
        {
            tag = true;
        }
        while (muldivlink)
        {
            oa_align(8);
            if (muldivlink->label)
                oa_put_label(muldivlink->label);
            ColumnPosition(8);
            if (muldivlink->size == ISZ_NONE)
            {
                AsmOutput("dd");
                ColumnPosition(16);
                AsmOutput("0%xh\n", muldivlink->value);
            }
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
                AsmOutput("db");
                ColumnPosition(16);
                for (i = 0; i < len; i++)
                {
                    AsmOutput("0%02XH", data[i]);
                    if (i != len - 1)
                        AsmOutput(",");
                }
                AsmOutput("\n");
            }
            muldivlink = muldivlink->next;
        }
        if (tag)
            AsmOutput("\n");
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
    AsmOutput(";File %s\n", filename);
    AsmOutput(";Compiler version %s\n", compiler_version);
    if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
        Optimizer::cparams.prm_assembler == pa_fasm)
    {
        if (!Optimizer::cparams.prm_nodos)
        {
            ColumnPosition(8);
            AsmOutput("section code align=%-8d use32\n", segAligns[Optimizer::codeseg]);
            ColumnPosition(8);
            AsmOutput("section data align=%-8d use32\n", segAligns[Optimizer::dataseg]);
            ColumnPosition(8);
            AsmOutput("section bss  align=%-8d use32\n", segAligns[Optimizer::bssxseg]);
            ColumnPosition(8);
            AsmOutput("section const  align=%-8d use32\n", segAligns[Optimizer::constseg]);
            ColumnPosition(8);
            AsmOutput("section string  align=%-8d use32\n", segAligns[Optimizer::stringseg]);
            ColumnPosition(8);
            AsmOutput("section tls  align=8 use32\n");
            ColumnPosition(8);
            AsmOutput("section cstartup align=2 use32\n");
            ColumnPosition(8);
            AsmOutput("section crundown align=2 use32\n");
            ColumnPosition(8);
            AsmOutput("section tstartup align=2 use32\n");
            ColumnPosition(8);
            AsmOutput("section trundown align=2 use32\n");
        }
        else
        {
            ColumnPosition(8);
            AsmOutput("section .text\n");
            ColumnPosition(8);
            AsmOutput("section .data\n");
            ColumnPosition(8);
            AsmOutput("section .bss\n");
        }
    }
    else
    {
        ColumnPosition(8);
        AsmOutput("title");
        ColumnPosition(16);
        AsmOutput("'%s'\n", filename);
        ColumnPosition(8);
        AsmOutput(".486p\n");
        ColumnPosition(8);
        if (Optimizer::cparams.prm_flat)
        {
            AsmOutput(".model flat\n\n");
        }
        else if (Optimizer::cparams.prm_assembler == pa_masm)
        {
            AsmOutput(".model small\n\n");
        }
        else
        {
            AsmOutput(".model use32 small\n\n");
        }
    }
}
void oa_trailer(void)
{
    if (!(Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
          Optimizer::cparams.prm_assembler == pa_fasm))
    {
        ColumnPosition(8);
        AsmOutput("end\n");
    }
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
        {
            AsmOutput("[global");
            ColumnPosition(8);
            AsmOutput("%s]\n", buf);
        }
        else
        {
            ColumnPosition(8);
            AsmOutput("public");
            ColumnPosition(16);
            AsmOutput("%s\n", buf);
        }
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
        {
            AsmOutput("%%define %s %s\n", name, alias);
        }
        else
        {
            AsmOutput("%s equ", name);
            ColumnPosition(8);
            AsmOutput("<%s>\n", alias);
        }
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
            AsmOutput("[extern");
            ColumnPosition(8);
            AsmOutput("%s]\n", buf);
        }
        else
        {
            ColumnPosition(8);
            AsmOutput("extrn");
            ColumnPosition(16);
            if (code)
                AsmOutput("%s:proc\n", buf);
            else
                AsmOutput("%s:byte\n", buf);
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
        ColumnPosition(8);
        AsmOutput("import %s %s\n", buf, file);
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
        ColumnPosition(8);
        if (Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm ||
            Optimizer::cparams.prm_assembler == pa_fasm)
            AsmOutput("export %s\n", buf);
        else
            AsmOutput("publicdll %s\n", buf);
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
        {
            ColumnPosition(8);
            AsmOutput("includelib %s\n", name);
        }
    }
    else
        omf_put_includelib(name);
}
void oa_end_generation(void) { dump_muldivval(); }
}  // namespace occx86