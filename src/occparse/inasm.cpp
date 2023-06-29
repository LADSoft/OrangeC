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

/*
 * inline assembler (386)
 */
#include <cstdio>
#include <cstring>
#include "compiler.h"
#include "../occ/be.h"
#include "Instruction.h"
#include <set>
#include "inasm.h"
#include "ccerr.h"
#include "stmt.h"
#include "ildata.h"
#include "occparse.h"
#include "expr.h"
#include "declare.h"
#include "lex.h"
#include "mangle.h"
#include "floatconv.h"
#include "constopt.h"
#include "memory.h"
#include "inline.h"
#include "help.h"
#include "template.h"
#include "symtab.h"

namespace Parser
{
static Optimizer::ASMREG* regimage;
static Optimizer::ASMNAME* insdata;
static LEXLIST* lex;

static SYMBOL* lastsym;
static enum e_opcode op;

static const char* assembler_errors[] = {"Lable expected",
                                         "Illegal address mode",
                                         "Address mode expected",
                                         "Invalid opcode",
                                         "Invalid instruction size",
                                         "Invalid index mode",
                                         "Invalid scale specifier",
                                         "Use LEA to take address of auto variable",
                                         "Too many segment specifiers",
                                         "Syntax error while parsing instruction",
                                         "Unknown operand",
                                         "Invalid combination of operands",
                                         "Invalid use of instruction"

};
static Optimizer::ASMNAME directiveLst[] = {{"db", op_reserved, ISZ_UCHAR, 0},
                                            {"dw", op_reserved, ISZ_USHORT, 0},
                                            {"dd", op_reserved, ISZ_ULONG, 0},
                                            {"dq", op_reserved, ISZ_ULONGLONG, 0},
                                            {"dt", op_reserved, ISZ_LDOUBLE, 0},
                                            {"label", op_label, 0, 0},
                                            {0}};
static Optimizer::ASMREG reglst[] = {{"cs", am_seg, 1, ISZ_USHORT},     {"ds", am_seg, 2, ISZ_USHORT},
                                     {"es", am_seg, 3, ISZ_USHORT},     {"fs", am_seg, 4, ISZ_USHORT},
                                     {"gs", am_seg, 5, ISZ_USHORT},     {"ss", am_seg, 6, ISZ_USHORT},
                                     {"al", am_dreg, 0, ISZ_UCHAR},     {"cl", am_dreg, 1, ISZ_UCHAR},
                                     {"dl", am_dreg, 2, ISZ_UCHAR},     {"bl", am_dreg, 3, ISZ_UCHAR},
                                     {"ah", am_dreg, 4, ISZ_UCHAR},     {"ch", am_dreg, 5, ISZ_UCHAR},
                                     {"dh", am_dreg, 6, ISZ_UCHAR},     {"bh", am_dreg, 7, ISZ_UCHAR},
                                     {"ax", am_dreg, 0, ISZ_USHORT},    {"cx", am_dreg, 1, ISZ_USHORT},
                                     {"dx", am_dreg, 2, ISZ_USHORT},    {"bx", am_dreg, 3, ISZ_USHORT},
                                     {"sym", am_dreg, 4, ISZ_USHORT},   {"bp", am_dreg, 5, ISZ_USHORT},
                                     {"si", am_dreg, 6, ISZ_USHORT},    {"di", am_dreg, 7, ISZ_USHORT},
                                     {"eax", am_dreg, 0, ISZ_UINT},     {"ecx", am_dreg, 1, ISZ_UINT},
                                     {"edx", am_dreg, 2, ISZ_UINT},     {"ebx", am_dreg, 3, ISZ_UINT},
                                     {"esp", am_dreg, 4, ISZ_UINT},     {"ebp", am_dreg, 5, ISZ_UINT},
                                     {"esi", am_dreg, 6, ISZ_UINT},     {"edi", am_dreg, 7, ISZ_UINT},
                                     {"st", am_freg, 0, ISZ_LDOUBLE},   {"cr0", am_screg, 0, ISZ_UINT},
                                     {"cr1", am_screg, 1, ISZ_UINT},    {"cr2", am_screg, 2, ISZ_UINT},
                                     {"cr3", am_screg, 3, ISZ_UINT},    {"cr4", am_screg, 4, ISZ_UINT},
                                     {"cr5", am_screg, 5, ISZ_UINT},    {"cr6", am_screg, 6, ISZ_UINT},
                                     {"cr7", am_screg, 7, ISZ_UINT},    {"dr0", am_sdreg, 0, ISZ_UINT},
                                     {"dr1", am_sdreg, 1, ISZ_UINT},    {"dr2", am_sdreg, 2, ISZ_UINT},
                                     {"dr3", am_sdreg, 3, ISZ_UINT},    {"dr4", am_sdreg, 4, ISZ_UINT},
                                     {"dr5", am_sdreg, 5, ISZ_UINT},    {"dr6", am_sdreg, 6, ISZ_UINT},
                                     {"dr7", am_sdreg, 7, ISZ_UINT},    {"tr0", am_streg, 0, ISZ_UINT},
                                     {"tr1", am_streg, 1, ISZ_UINT},    {"tr2", am_streg, 2, ISZ_UINT},
                                     {"tr3", am_streg, 3, ISZ_UINT},    {"tr4", am_streg, 4, ISZ_UINT},
                                     {"tr5", am_streg, 5, ISZ_UINT},    {"tr6", am_streg, 6, ISZ_UINT},
                                     {"tr7", am_streg, 7, ISZ_UINT},    {"byte", am_ext, akw_byte, 0},
                                     {"word", am_ext, akw_word, 0},     {"dword", am_ext, akw_dword, 0},
                                     {"fword", am_ext, akw_fword, 0},   {"qword", am_ext, akw_qword, 0},
                                     {"tbyte", am_ext, akw_tbyte, 0},   {"ptr", am_ext, akw_ptr, 0},
                                     {"offset", am_ext, akw_offset, 0}, {0, 0, 0}};

static int floating;
static SymbolTable<ASM_HASH_ENTRY>* asmHash;
static SymbolTableFactory<ASM_HASH_ENTRY> asmSymbolFactory;

void inlineAsmInit(void)
{
    bool old = Optimizer::cparams.prm_extwarning;
    Optimizer::cparams.prm_extwarning = false;
    Optimizer::assembling = false;
    Optimizer::ASMREG* r = reglst;
    ASM_HASH_ENTRY* s;
    asmSymbolFactory.Reset();
    asmHash = asmSymbolFactory.CreateSymbolTable();
    while (r->name)
    {
        s = Allocate<ASM_HASH_ENTRY>();
        s->data = r;
        s->name = r->name;
        s->instruction = false;
        asmHash->Add(s);
        r++;
    }
    int i = 0;
    if (IsCompiler())
    {
        for (auto v : opcodeTable)
        {
            if (v[0] != 0)
            {
                s = Allocate<ASM_HASH_ENTRY>();
                s->name = v;
                s->data = Allocate<Optimizer::ASMNAME>();
                ((Optimizer::ASMNAME*)s->data)->name = v;
                ((Optimizer::ASMNAME*)s->data)->atype = i;
                s->instruction = true;
                asmHash->Add(s);
            }
            i++;
        }
    }
    if (Optimizer::cparams.prm_assemble)
    {
        Optimizer::ASMNAME* o = directiveLst;
        while (o->name)
        {
            s = Allocate<ASM_HASH_ENTRY>();
            s->data = o;
            s->name = o->name;
            s->instruction = true;
            asmHash->Add(s);
            o++;
        }
    }
    Optimizer::cparams.prm_extwarning = old;
}
static void inasm_err(int errnum)
{
    //    *lptr = 0;
    //    lastch = ' ';
    errorstr(ERR_ASM, assembler_errors[errnum]);
    lex = getsym();
}

static void inasm_txsym(void)
{
    if (lex && ISID(lex))
    {
        ASM_HASH_ENTRY* e = search(asmHash, lex->data->value.s.a);
        if (e)
        {
            if (e->instruction)
            {
                lex->data->type = l_asminst;
                insdata = (Optimizer::ASMNAME*)e->data;
            }
            else
            {
                lex->data->type = l_asmreg;
                regimage = (Optimizer::ASMREG*)e->data;
            }
        }
    }
}
static void inasm_getsym(void)
{
    lex = getsym();
    inasm_txsym();
}
static void inasm_needkw(LEXLIST** lex, int Keyword)
{
    needkw(lex, (e_kw)Keyword);
    inasm_txsym();
}
static AMODE* inasm_const(void)
{
    AMODE* rv = nullptr;
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    lex = optimized_expression(lex, theCurrentFunc, nullptr, &tp, &exp, false);
    if (!tp)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (!isint(tp) || !isintconst(exp))
    {
        error(ERR_CONSTANT_VALUE_EXPECTED);
    }
    else
    {
        rv = Allocate<AMODE>();
        rv->mode = am_immed;
        rv->offset = Optimizer::SymbolManager::Get(exp);
    }
    return rv;
}
/*-------------------------------------------------------------------------*/

static EXPRESSION* inasm_ident(void)
{
    EXPRESSION* node = 0;

    if (lex->data->type != l_id)
        error(ERR_IDENTIFIER_EXPECTED);
    else
    {
        SYMBOL* sym;
        char nm[256];
        strcpy(nm, lex->data->value.s.a);
        inasm_getsym();
        /* No such identifier */
        /* label, put it in the symbol table */
        if ((sym = search(labelSyms, nm)) == 0 && (sym = gsearch(nm)) == 0)
        {
            sym = SymAlloc();
            sym->sb->storage_class = sc_ulabel;
            sym->name = litlate(nm);
            sym->sb->declfile = sym->sb->origdeclfile = lex->data->errfile;
            sym->sb->declline = sym->sb->origdeclline = lex->data->errline;
            sym->sb->realdeclline = lex->data->linedata->lineno;
            sym->sb->declfilenum = lex->data->linedata->fileindex;
            sym->sb->attribs.inheritable.used = true;
            sym->tp = MakeType(bt_unsigned);
            sym->sb->offset = codeLabel++;
            labelSyms->Add(sym);
            node = intNode(en_labcon, sym->sb->offset);
        }
        else
        {
            /* If we get here the symbol was already in the table
             */
            sym->sb->attribs.inheritable.used = true;
            switch (sym->sb->storage_class)
            {
                case sc_absolute:
                    Optimizer::SymbolManager::Get(sym);
                    node = varNode(en_absolute, sym);
                    break;
                case sc_overloads:
                    node = varNode(en_pc, (SYMBOL*)sym->tp->syms->front());
                    InsertInline(sym->tp->syms->front());
                    break;
                case sc_localstatic:
                case sc_global:
                case sc_external:
                case sc_static: {
                    Optimizer::SimpleSymbol* sym1 = Optimizer::SymbolManager::Get(sym);
                    node = varNode(en_global, sym);
                    InsertGlobal(sym);
                    if (isfunction(sym->tp))
                        InsertInline(sym);
                    Optimizer::EnterExternal(sym1);
                    break;
                }
                case sc_const:
                    /* constants and enums */
                    node = intNode(en_c_i, sym->sb->value.i);
                    break;
                case sc_label:
                case sc_ulabel:
                    node = intNode(en_labcon, sym->sb->offset);
                    break;
                case sc_auto:
                case sc_register:
                    sym->sb->allocate = true;
                case sc_parameter:
                    node = varNode(en_auto, sym);
                    sym->sb->inasm = true;
                    break;
                default:
                    errorstr(ERR_INVALID_STORAGE_CLASS, "");
                    break;
            }
        }
        lastsym = sym;
    }
    return node;
}

/*-------------------------------------------------------------------------*/

static EXPRESSION* inasm_label(void)
{
    EXPRESSION* node;
    SYMBOL* sym;
    if (!ISID(lex))
    {
        lex = getsym();
        return nullptr;
    }
    /* No such identifier */
    /* label, put it in the symbol table */
    if ((sym = search(labelSyms, lex->data->value.s.a)) == 0)
    {
        sym = SymAlloc();
        sym->sb->storage_class = sc_label;
        sym->name = litlate(lex->data->value.s.a);
        sym->sb->declfile = sym->sb->origdeclfile = lex->data->errfile;
        sym->sb->declline = sym->sb->origdeclline = lex->data->errline;
        sym->sb->realdeclline = lex->data->linedata->lineno;
        sym->sb->declfilenum = lex->data->linedata->fileindex;
        sym->tp = MakeType(bt_unsigned);
        sym->sb->offset = codeLabel++;
        SetLinkerNames(sym, lk_none);
        labelSyms->Add(sym);
    }
    else
    {
        if (sym->sb->storage_class == sc_label)
        {
            errorsym(ERR_DUPLICATE_LABEL, sym);
            inasm_getsym();
            return 0;
        }
        if (sym->sb->storage_class != sc_ulabel)
        {
            inasm_err(ERR_LABEL_EXPECTED);
            return 0;
        }
        sym->sb->storage_class = sc_label;
    }
    inasm_getsym();
    if (lex->data->type == l_asminst)
    {
        if (insdata->atype == op_reserved)
        {
            node = intNode(en_labcon, sym->sb->offset);
            return node;
        }
        else if (insdata->atype != op_label)
        {
            inasm_err(ERR_LABEL_EXPECTED);
            return 0;
        }
    }
    else if (!MATCHKW(lex, colon))
    {
        inasm_err(ERR_LABEL_EXPECTED);
        return 0;
    }
    inasm_getsym();
    node = intNode(en_labcon, sym->sb->offset);
    return node;
}

/*-------------------------------------------------------------------------*/

static int inasm_getsize(void)
{
    int sz = ISZ_NONE;
    switch (regimage->regnum)
    {
        case akw_byte:
            sz = ISZ_UCHAR;
            break;
        case akw_word:
            sz = ISZ_USHORT;
            break;
        case akw_dword:
            sz = floating ? ISZ_FLOAT : ISZ_UINT;
            break;
        case akw_fword:
            sz = ISZ_FARPTR;
            break;
        case akw_qword:
            sz = ISZ_ULONGLONG;
            break;
        case akw_tbyte:
            sz = ISZ_LDOUBLE;
            break;
        case akw_offset:
            sz = ISZ_UINT; /* not differntiating addresses at this level*/
            break;
    };
    inasm_getsym();
    if (MATCHTYPE(lex, l_asmreg))
    {
        if (regimage->regtype == am_ext)
        {
            if (regimage->regnum != akw_ptr)
            {
                inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                return 0;
            }
            inasm_getsym();
        }
    }
    if (!lex || (lex->data->type != l_asmreg && !ISID(lex) && !MATCHKW(lex, openbr)))
    {
        inasm_err(ERR_ADDRESS_MODE_EXPECTED);
        return 0;
    }
    return sz;
}

/*-------------------------------------------------------------------------*/

static int getscale(int* scale)
{
    if (MATCHKW(lex, star))
    {
        inasm_getsym();
        if (lex)
        {
            if ((MATCHTYPE(lex, l_i) || MATCHTYPE(lex, l_ui)) && !*scale)
            {
                switch ((int)lex->data->value.i)
                {
                    case 1:
                        *scale = 0;
                        break;
                    case 2:
                        *scale = 1;
                        break;
                    case 4:
                        *scale = 2;
                        break;
                    case 8:
                        *scale = 3;
                        break;
                    default:
                        inasm_err(ERR_INVALID_SCALE_SPECIFIER);
                        *scale = -1;
                        return 1;
                }
                inasm_getsym();
                return 1;
            }
        }
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

int inasm_enterauto(EXPRESSION* node, int* reg1, int* reg2)
{
    if (node && node->type == en_auto)
    {
        int* vreg;
        if (*reg1 >= 0 && *reg2 >= 0)
        {
            inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
            return 0;
        }
        vreg = *reg1 < 0 ? reg1 : reg2;
        *vreg = EBP;
        return 1;
    }
    return 2;
}

/*-------------------------------------------------------------------------*/

static int inasm_structsize(void)
{
    if (basetype(lastsym->tp)->type == bt_struct)
    {
        if (lastsym->tp->size == 6)
            return ISZ_FARPTR;
        if (lastsym->tp->size == 4)
            return ISZ_UINT;
        if (lastsym->tp->size == 1)
            return ISZ_UCHAR;
        if (lastsym->tp->size == 2)
            return ISZ_USHORT;
        return 0;
    }
    else
    {
        switch (basetype(lastsym->tp)->type)
        {
            case bt_char:
            case bt_unsigned_char:
            case bt_signed_char:
                return ISZ_UCHAR;
            case bt_bool:
                return ISZ_BOOLEAN;
            case bt_short:
            case bt_unsigned_short:
            case bt_char16_t:
                return ISZ_USHORT;
            case bt_wchar_t:
                return ISZ_USHORT;
            case bt_long:
            case bt_unsigned_long:
                return ISZ_UINT;
            case bt_long_long:
            case bt_unsigned_long_long:
                return ISZ_ULONGLONG;
            case bt_int:
            case bt_unsigned:
            case bt_char32_t:
                return ISZ_UINT;
            case bt_enum:
                return ISZ_UINT;
            case bt_pointer:
                return ISZ_UINT;
            case bt_seg:
                return ISZ_USHORT;
            case bt_far:
                return ISZ_FARPTR;
                //        case bt_memberptr:
                //            return ISZ_UINT;
            default:
                return 1000;
        }
    }
}

/*-------------------------------------------------------------------------*/

static AMODE* inasm_mem(void)
{
    int reg1 = -1, reg2 = -1, scale = 0, seg = 0;
    bool subtract = false;
    EXPRESSION* node = 0;
    AMODE* rv;
    int gotident = false; /*, autonode = false;*/
    inasm_getsym();
    while (true)
    {
        int rg = -1;
        if (regimage)
            rg = regimage->regnum;
        if (lex)
        {
            switch (lex->data->type)
            {
                case l_asmreg:
                    if (subtract)
                    {
                        inasm_err(ERR_INVALID_INDEX_MODE);
                        return 0;
                    }
                    if (regimage->regtype == am_seg)
                    {
                        if (seg)
                        {
                            inasm_err(ERR_INVALID_INDEX_MODE);
                            return 0;
                        }
                        seg = rg;
                        inasm_getsym();
                        if (!MATCHKW(lex, colon))
                        {
                            inasm_err(ERR_INVALID_INDEX_MODE);
                            return 0;
                        }
                        inasm_getsym();
                        continue;
                    }
                    if (regimage->regtype != am_dreg || regimage->size != ISZ_UINT)
                    {
                        inasm_err(ERR_INVALID_INDEX_MODE);
                        return 0;
                    }
                    if (reg1 >= 0)
                    {
                        if (reg2 >= 0)
                        {
                            inasm_err(ERR_INVALID_INDEX_MODE);
                            return 0;
                        }
                        reg2 = rg;
                        inasm_getsym();
                        getscale(&scale);
                        if (scale == -1)
                            return 0;
                    }
                    else
                    {
                        inasm_getsym();
                        if (getscale(&scale))
                        {
                            if (scale == -1)
                                return 0;
                            if (reg2 >= 0)
                            {
                                reg1 = reg2;
                            }
                            reg2 = rg;
                        }
                        else
                        {
                            reg1 = rg;
                        }
                    }
                    break;
                case l_wchr:
                case l_achr:
                case l_i:
                case l_ui:
                case l_l:
                case l_ul:
                    if (node)
                        node = exprNode(subtract ? en_sub : en_add, node, intNode(en_c_i, lex->data->value.i));
                    else if (subtract)
                        node = intNode(en_c_i, -lex->data->value.i);
                    else
                        node = intNode(en_c_i, lex->data->value.i);
                    inasm_getsym();
                    break;
                case l_kw:
                    if (MATCHKW(lex, plus) || MATCHKW(lex, minus))
                    {
                        if (node)
                            node = exprNode(en_add, node, intNode(en_c_i, 0));
                        else
                            node = intNode(en_c_i, 0);
                        break;
                    }
                    /* fallthrough */
                default:
                    inasm_err(ERR_INVALID_INDEX_MODE);
                    return 0;
                case l_id:
                    if (gotident || subtract)
                    {
                        inasm_err(ERR_INVALID_INDEX_MODE);
                        return 0;
                    }
                    node = inasm_ident();
                    gotident = true;
                    inasm_structsize();
                    switch (inasm_enterauto(node, &reg1, &reg2))
                    {
                        case 0:
                            return 0;
                        case 1:
                            /*autonode = true;*/
                            break;
                        case 2:
                            /*autonode = false;*/
                            break;
                    }
                    break;
            }
        }
        if (MATCHKW(lex, closebr))
        {
            inasm_getsym();
            break;
        }
        if (!MATCHKW(lex, plus) && !MATCHKW(lex, minus))
        {
            inasm_err(ERR_INVALID_INDEX_MODE);
            return 0;
        }
        if (MATCHKW(lex, minus))
            subtract = true;
        else
            subtract = false;
        inasm_getsym();
    }
    if ((reg2 == 4 || reg2 == 5) && scale > 1)
    {
        inasm_err(ERR_INVALID_INDEX_MODE);
        return 0;
    }
    rv = beLocalAllocate<AMODE>();
    if (node)
    {
        rv->offset = Optimizer::SymbolManager::Get(node);
    }
    if (reg1 >= 0)
    {
        rv->preg = reg1;
        if (reg2 >= 0)
        {
            rv->sreg = reg2;
            rv->scale = scale;
            rv->mode = am_indispscale;
        }
        else
        {
            rv->mode = am_indisp;
        }
    }
    else if (reg2 >= 0)
    {
        rv->preg = -1;
        rv->sreg = reg2;
        rv->scale = scale;
        rv->mode = am_indispscale;
    }
    else
        rv->mode = am_direct;
    rv->seg = seg;
    return rv;
}

/*-------------------------------------------------------------------------*/

static AMODE* inasm_amode(int nosegreg)
{
    AMODE* rv = beLocalAllocate<AMODE>();
    int sz = 0, seg = 0;
    bool done = false;
    lastsym = 0;
    inasm_txsym();
    if (lex)
    {
        switch (lex->data->type)
        {
            case l_wchr:
            case l_achr:
            case l_i:
            case l_ui:
            case l_l:
            case l_ul:
            case l_id:
            case l_asmreg:
                break;
            case l_kw:
                switch (KW(lex))
                {
                    case openbr:
                    case minus:
                    case plus:
                        break;
                    default:
                        inasm_err(ERR_ADDRESS_MODE_EXPECTED);
                        return 0;
                }
                break;
            default:
                inasm_err(ERR_ADDRESS_MODE_EXPECTED);
                return 0;
        }
    }
    if (MATCHTYPE(lex, l_asmreg))
    {
        if (regimage->regtype == am_ext)
        {
            sz = inasm_getsize();
        }
    }
    while (!done)
    {
        done = true;
        if (lex)
        {
            switch (lex->data->type)
            {
                case l_asmreg:
                    if (regimage->regtype == am_ext)
                    {
                        inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                        return 0;
                    }
                    if (regimage->regtype == am_freg)
                    {
                        inasm_getsym();
                        if (MATCHKW(lex, openpa))
                        {
                            inasm_getsym();
                            if (!lex || (lex->data->type != l_i && lex->data->type != l_ui) || lex->data->value.i < 0 ||
                                lex->data->value.i > 7)
                            {
                                inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                                return 0;
                            }
                            inasm_getsym();
                            inasm_needkw(&lex, closepa);
                        }
                        else
                            lex->data->value.i = 0;
                        rv->preg = lex->data->value.i;
                        rv->mode = am_freg;
                        sz = ISZ_LDOUBLE;
                    }
                    else if (regimage->regtype == am_seg)
                    {
                        if (rv->seg)
                        {
                            inasm_err(ERR_TOO_MANY_SEGMENTS);
                            return 0;
                        }
                        rv->seg = seg = regimage->regnum;
                        inasm_getsym();
                        if (MATCHKW(lex, colon))
                        {
                            inasm_getsym();
                            done = false;
                            continue;
                        }
                        rv->mode = am_seg;
                        sz = regimage->size;
                    }
                    else
                    {
                        rv->preg = regimage->regnum;
                        rv->mode = (e_am)regimage->regtype;
                        sz = rv->length = regimage->size;
                        inasm_getsym();
                    }
                    break;
                case l_id:
                    rv->mode = am_immed;
                    rv->offset = Optimizer::SymbolManager::Get(inasm_ident());
                    rv->length = ISZ_UINT;
                    if (rv->offset->type == Optimizer::se_auto)
                    {
                        inasm_err(ERR_USE_LEA);
                        return 0;
                    }
                    break;
                case l_kw:
                    switch (KW(lex))
                    {
                        case openbr:
                            rv = inasm_mem();
                            if (rv && rv->seg)
                                seg = rv->seg;
                            break;
                        case minus:
                        case plus:
                            rv = inasm_const();
                            break;
                        default:
                            inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                            return 0;
                    }
                    break;
                case l_i:
                case l_ui:
                case l_l:
                case l_ul:
                case l_wchr:
                case l_achr:
                    rv = inasm_const();
                    break;
                default:
                    inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                    return 0;
            }
        }
    }
    if (rv)
    {
        if (rv->seg)
            if (nosegreg || rv->mode != am_dreg)
                if (rv->mode != am_direct && rv->mode != am_indisp && rv->mode != am_indispscale && rv->mode != am_seg)
                {
                    inasm_err(ERR_TOO_MANY_SEGMENTS);
                    return 0;
                }
        if (!rv->length)
        {
            if (sz)
                rv->length = sz;
            else if (lastsym)
                rv->length = inasm_structsize();
        }
        if (rv->length < 0)
            rv->length = -rv->length;
        rv->seg = seg;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

static AMODE* aimmed(long long i)
{
    AMODE* rv = Allocate<AMODE>();
    rv->mode = am_immed;
    rv->offset = (Optimizer::SimpleExpression*)intNode(en_c_i, i);
    return rv;
}
static AMODE* inasm_immed(void)
{
    AMODE* rv;
    if (lex)
        switch (lex->data->type)
        {
            case l_i:
            case l_ui:
            case l_l:
            case l_ul:
            case l_wchr:
            case l_achr:
                rv = aimmed(lex->data->value.i);
                rv->length = ISZ_UINT;
                inasm_getsym();
                return rv;
            default:
                break;
        }

    return nullptr;
}

/*-------------------------------------------------------------------------*/

int isrm(AMODE* ap, int dreg_allowed)
{
    switch (ap->mode)
    {
        case am_dreg:
            return dreg_allowed;
        case am_indisp:
        case am_direct:
        case am_indispscale:
            return 1;
        default:
            return 0;
    }
}

/*-------------------------------------------------------------------------*/

AMODE* getimmed(void)
{
    AMODE* rv;
    if (lex)
        switch (lex->data->type)
        {
            case l_i:
            case l_ui:
            case l_l:
            case l_ul:
            case l_wchr:
            case l_achr:
                rv = aimmed(lex->data->value.i);
                inasm_getsym();
                return rv;
            default:
                break;
        }
    return nullptr;
}

/*-------------------------------------------------------------------------*/

enum e_opcode inasm_op(void)
{
    int op;
    if (!lex || lex->data->type != l_asminst)
    {
        inasm_err(ERR_INVALID_OPCODE);
        return (e_opcode)-1;
    }
    op = insdata->atype;
    inasm_getsym();
    floating = op >= op_f2xm1 && op <= op_fyl2xp1;
    return (e_opcode)op;
}

/*-------------------------------------------------------------------------*/

static OCODE* make_ocode(AMODE* ap1, AMODE* ap2, AMODE* ap3)
{
    OCODE* o = beLocalAllocate<OCODE>();
    if (ap1 && (ap1->length == ISZ_UCHAR || ap1->length == -ISZ_UCHAR))
        if (ap2 && ap2->mode == am_immed)
            ap2->length = ap1->length;
    o->oper1 = ap1;
    o->oper2 = ap2;
    o->oper3 = ap3;
    return o;
}
static int getData(STATEMENT* snp)
{
    int size = insdata->amode;
    EXPRESSION** newExpr = &snp->select;
    do
    {
        TYPE* tp;
        EXPRESSION* expr;
        lex = getsym();
        lex = optimized_expression(lex, nullptr, nullptr, &tp, &expr, false);
        if (tp && (isintconst(expr) || isfloatconst(expr)))
        {
            switch (size)
            {
                case ISZ_UCHAR:
                    expr->v.i = reint(expr);
                    expr->type = en_c_uc;
                    break;
                case ISZ_USHORT:
                    expr->v.i = reint(expr);
                    expr->type = en_c_us;
                    break;
                case ISZ_ULONG:
                    if (isintconst(expr))
                    {
                        expr->v.i = reint(expr);
                        expr->type = en_c_ul;
                    }
                    else
                    {
                        expr->v.f = Allocate<FPF>();
                        *expr->v.f = refloat(expr);
                        expr->type = en_c_f;
                    }
                    break;
                case ISZ_ULONGLONG:
                    if (isintconst(expr))
                    {
                        expr->v.i = reint(expr);
                        expr->type = en_c_ull;
                    }
                    else
                    {
                        expr->v.f = Allocate<FPF>();
                        *expr->v.f = refloat(expr);
                        expr->type = en_c_d;
                    }
                    break;
                case ISZ_LDOUBLE:
                    expr->v.f = Allocate<FPF>();
                    *expr->v.f = refloat(expr);
                    expr->type = en_c_ld;
                    break;
            }
            *newExpr = expr;
            newExpr = &(*newExpr)->left;
        }
        else
        {
            error(ERR_CONSTANT_VALUE_EXPECTED);
            lex = SkipToNextLine();
            break;
        }
    } while (lex && MATCHKW(lex, comma));
    return 1;
}
static void AssembleInstruction(OCODE* ins)
{
    if (ins->opcode >= op_aaa)
    {
        if (IsCompiler())
        {
            OCODE ins1 = *ins;
            AMODE oper1;
            AMODE oper2;
            AMODE oper3;
            Instruction* newIns = nullptr;
            std::list<Numeric*> operands;
            Optimizer::assembling = true;
            asmError err = instructionParser->GetInstruction(&ins1, newIns, operands);
            Optimizer::assembling = false;
            delete newIns;
            switch (err)
            {
                case AERR_NONE:
                    break;
                case AERR_SYNTAX:
                    inasm_err(ERR_SYNTAX);
                    break;
                case AERR_OPERAND:
                    inasm_err(ERR_UNKNOWN_OP);
                    break;
                case AERR_BADCOMBINATIONOFOPERANDS:
                    inasm_err(ERR_BAD_OPERAND_COMBO);
                    break;
                case AERR_UNKNOWNOPCODE:
                    inasm_err(ERR_INVALID_OPCODE);
                    break;
                case AERR_INVALIDINSTRUCTIONUSE:
                    inasm_err(ERR_INVALID_USE_OF_INSTRUCTION);
                    break;
                default:
                    break;
            }
        }
    }
}
LEXLIST* inlineAsm(LEXLIST* inlex, std::list<BLOCKDATA*>& parent)
{
    STATEMENT* snp;
    OCODE* rv;
    EXPRESSION* node;
    lastsym = 0;
    lex = inlex; /* patch to not have to rewrite entire module for new frontend */
    inasm_txsym();
    do
    {
        snp = stmtNode(lex, parent, st_passthrough);
        if (!lex)
        {
            return lex;
        }
        bool atend;
        if (lex->data->type != l_asminst)
        {
            if (MATCHKW(lex, kw_int))
            {
                inasm_getsym();
                op = op_int;
                rv = beLocalAllocate<OCODE>();
                rv->oper1 = inasm_amode(true);
                goto join;
            }
            node = inasm_label();
            if (!node)
                return lex;
            if (MATCHKW(lex, semicolon))
                inasm_getsym();

            snp->type = st_label;
            snp->label = node->v.i;
            return lex;
        }
        if (insdata->atype == op_reserved)
        {
            getData(snp);
            return lex;
        }
        atend = AtEol();
        op = inasm_op();
        if (op == (enum e_opcode) - 1)
        {

            return lex;
        }
        {
            rv = beLocalAllocate<OCODE>();
            if (!(op == op_rep || op == op_repnz || op == op_repz || op == op_repe || op == op_repne || op == op_lock))
            {
                if (!atend && !MATCHKW(lex, semicolon))
                {
                    rv->oper1 = inasm_amode(false);
                    if (MATCHKW(lex, comma))
                    {
                        inasm_getsym();
                        rv->oper2 = inasm_amode(false);
                        if (MATCHKW(lex, comma))
                        {
                            inasm_getsym();
                            rv->oper3 = inasm_amode(false);
                        }
                    }
                }
            }
        join:
            if (!rv || rv == (OCODE*)-1 || rv == (OCODE*)-2)
            {
                if (rv == (OCODE*)-1)
                    inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                if (rv == (OCODE*)-2)
                    inasm_err(ERR_INVALID_SIZE);
                return lex;
            }
        }
        if (rv->oper1 && rv->oper2)
        {
            if (!rv->oper1->length)
            {
                if (!rv->oper2->length)
                {
                    inasm_err(ERR_INVALID_SIZE);
                }
                else
                    rv->oper1->length = rv->oper2->length;
            }
        }
        rv->noopt = true;
        rv->opcode = op;
        rv->fwd = rv->back = 0;
        AssembleInstruction(rv);
        snp->select = (EXPRESSION*)rv;

    } while (op == op_rep || op == op_repnz || op == op_repz || op == op_repe || op == op_repne || op == op_lock);
    return lex;
}
void adjust_codelab(void* select, int offset)
{
    OCODE* peep = (OCODE*)select;
    if (peep->oper1 && peep->oper1->offset && peep->oper1->offset->type == Optimizer::se_labcon)
        if (peep->oper1->offset->i < 0)
            peep->oper1->offset->i += offset;
    if (peep->oper2 && peep->oper2->offset && peep->oper2->offset->type == Optimizer::se_labcon)
        if (peep->oper2->offset->i < 0)
            peep->oper2->offset->i += offset;
    if (peep->oper3 && peep->oper3->offset && peep->oper3->offset->type == Optimizer::se_labcon)
        if (peep->oper3->offset->i < 0)
            peep->oper3->offset->i += offset;
}
void* inlineAsmStmt(void* param)
{
    OCODE* rv = beLocalAllocate<OCODE>();
    memcpy(rv, param, sizeof(*rv));
    if ((e_op)rv->opcode != op_label && (e_op)rv->opcode != op_line)
    {
        AMODE* ap = rv->oper1;
        if (ap && ap->offset)
            ap->offset = (Optimizer::SimpleExpression*)inlineexpr((EXPRESSION*)ap->offset, nullptr);
        ap = rv->oper2;
        if (ap && ap->offset)
            ap->offset = (Optimizer::SimpleExpression*)inlineexpr((EXPRESSION*)ap->offset, nullptr);
        ap = rv->oper3;
        if (ap && ap->offset)
            ap->offset = (Optimizer::SimpleExpression*)inlineexpr((EXPRESSION*)ap->offset, nullptr);
    }
    return rv;
}
}  // namespace Parser