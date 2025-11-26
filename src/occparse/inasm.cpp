/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "namespace.h"
#include "symtab.h"
#include "types.h"

#define ASM_DEST_REG 0
#define ASM_SRC_REG_START 1

namespace Parser
{
static Optimizer::ASMREG* regimage;
static Optimizer::ASMNAME* insdata;

static SYMBOL* lastsym;
static e_opcode op;

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
#ifndef ORANGE_NO_INASM
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
#endif
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
    getsym();
}
static void setreg(std::vector<int>& srcRegs, Optimizer::ASMREG* regImage)
{
    int reg = regImage->regnum;
    switch (regImage->size)
    {
    case ISZ_UCHAR:
        reg += R_AL;
        break;
    case ISZ_USHORT:
        reg += R_AX;
        break;
    default:
    case ISZ_UINT:
        reg += R_EAX;
        break;
    }
    srcRegs.push_back(reg);
}
static void inasm_txsym(void)
{
    if (currentLex && ISID())
    {
        ASM_HASH_ENTRY* e = search(asmHash, currentLex->value.s.a);
        if (e)
        {
            if (e->instruction)
            {
                currentLex->type = LexType::l_asmInstruction_;
                insdata = (Optimizer::ASMNAME*)e->data;
            }
            else
            {
                currentLex->type = LexType::l_asmRegister_;
                regimage = (Optimizer::ASMREG*)e->data;
            }
        }
    }
}
static void inasm_getsym(void)
{
    getsym();
    inasm_txsym();
}
static void inasm_needkw( Keyword Kw)
{
    needkw(Kw);
    inasm_txsym();
}
static AMODE* inasm_const(void)
{
    AMODE* rv = nullptr;
    Type* tp = nullptr;
    EXPRESSION* exp = nullptr;
    optimized_expression(theCurrentFunc, nullptr, &tp, &exp, false);
    if (!tp)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (!tp->IsInt() || !isintconst(exp))
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

    if (currentLex->type != LexType::l_id_)
        error(ERR_IDENTIFIER_EXPECTED);
    else
    {
        SYMBOL* sym;
        char nm[256];
        Utils::StrCpy(nm, currentLex->value.s.a);
        inasm_getsym();
        /* No such identifier */
        /* label, put it in the symbol table */
        if ((sym = search(labelSyms, nm)) == 0 && (sym = gsearch(nm)) == 0)
        {
            sym = SymAlloc();
            sym->sb->storage_class = StorageClass::ulabel_;
            sym->name = litlate(nm);
            sym->sb->declfile = sym->sb->origdeclfile = currentLex->sourceFileName;
            sym->sb->declline = sym->sb->origdeclline = currentLex->sourceLineNumber;
            sym->sb->declcharpos = currentLex->charindex;
            sym->sb->realcharpos = currentLex->realcharindex;
            sym->sb->realdeclline = currentLex->linedata->lineno;
            sym->sb->declfilenum = currentLex->linedata->fileindex;
            sym->sb->attribs.inheritable.used = true;
            sym->tp = Type::MakeType(BasicType::unsigned_);
            sym->sb->offset = codeLabel++;
            labelSyms->Add(sym);
            node = MakeIntExpression(ExpressionNode::labcon_, sym->sb->offset);
        }
        else
        {
            /* If we get here the symbol was already in the table
             */
            sym->sb->attribs.inheritable.used = true;
            switch (sym->sb->storage_class)
            {
                case StorageClass::absolute_:
                    Optimizer::SymbolManager::Get(sym);
                    node = MakeExpression(ExpressionNode::absolute_, sym);
                    break;
                case StorageClass::overloads_:
                    node = MakeExpression(ExpressionNode::pc_, (SYMBOL*)sym->tp->syms->front());
                    sym->tp->syms->front()->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                    InsertInline(sym->tp->syms->front());
                    break;
                case StorageClass::localstatic_:
                case StorageClass::global_:
                case StorageClass::external_:
                case StorageClass::static_: {
                    Optimizer::SimpleSymbol* sym1 = Optimizer::SymbolManager::Get(sym);
                    node = MakeExpression(ExpressionNode::global_, sym);
                    InsertGlobal(sym);
                    if (sym->tp->IsFunction())
                    {
                        sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                        InsertInline(sym);
                    }
                    Optimizer::EnterExternal(sym1);
                    break;
                }
                case StorageClass::const_:
                    /* constants and enums */
                    node = MakeIntExpression(ExpressionNode::c_i_, sym->sb->value.i);
                    break;
                case StorageClass::label_:
                case StorageClass::ulabel_:
                    node = MakeIntExpression(ExpressionNode::labcon_, sym->sb->offset);
                    break;
                case StorageClass::auto_:
                case StorageClass::register_:
                    sym->sb->allocate = true;
                    node = MakeExpression(ExpressionNode::auto_, sym);
                    sym->sb->inasm = true;
                    break;
                case StorageClass::parameter_:
                    node = MakeExpression(ExpressionNode::auto_, sym);
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
    if (!ISID())
    {
        getsym();
        return nullptr;
    }
    /* No such identifier */
    /* label, put it in the symbol table */
    if ((sym = search(labelSyms, currentLex->value.s.a)) == 0)
    {
        sym = SymAlloc();
        sym->sb->storage_class = StorageClass::label_;
        sym->name = litlate(currentLex->value.s.a);
        sym->sb->declfile = sym->sb->origdeclfile = currentLex->sourceFileName;
        sym->sb->declline = sym->sb->origdeclline = currentLex->sourceLineNumber;
        sym->sb->declcharpos = currentLex->charindex;
        sym->sb->realcharpos = currentLex->realcharindex;
        sym->sb->realdeclline = currentLex->linedata->lineno;
        sym->sb->declfilenum = currentLex->linedata->fileindex;
        sym->tp = Type::MakeType(BasicType::unsigned_);
        sym->sb->offset = codeLabel++;
        SetLinkerNames(sym, Linkage::none_);
        labelSyms->Add(sym);
    }
    else
    {
        if (sym->sb->storage_class == StorageClass::label_)
        {
            errorsym(ERR_DUPLICATE_LABEL, sym);
            inasm_getsym();
            return 0;
        }
        if (sym->sb->storage_class != StorageClass::ulabel_)
        {
            inasm_err(ERR_LABEL_EXPECTED);
            return 0;
        }
        sym->sb->storage_class = StorageClass::label_;
    }
    inasm_getsym();
    if (currentLex->type == LexType::l_asmInstruction_)
    {
        if (insdata->atype == op_reserved)
        {
            node = MakeIntExpression(ExpressionNode::labcon_, sym->sb->offset);
            return node;
        }
        else if (insdata->atype != op_label)
        {
            inasm_err(ERR_LABEL_EXPECTED);
            return 0;
        }
    }
    else if (!MATCHKW(Keyword::colon_))
    {
        inasm_err(ERR_LABEL_EXPECTED);
        return 0;
    }
    inasm_getsym();
    node = MakeIntExpression(ExpressionNode::labcon_, sym->sb->offset);
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
    if (MATCHTYPE(LexType::l_asmRegister_))
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
    if (!currentLex || (currentLex->type != LexType::l_asmRegister_ && !ISID() && !MATCHKW(Keyword::openbr_)))
    {
        inasm_err(ERR_ADDRESS_MODE_EXPECTED);
        return 0;
    }
    return sz;
}

/*-------------------------------------------------------------------------*/

static int getscale(int* scale)
{
    if (MATCHKW(Keyword::star_))
    {
        inasm_getsym();
        if (currentLex)
        {
            if ((MATCHTYPE(LexType::i_) || MATCHTYPE(LexType::ui_)) && !*scale)
            {
                switch ((int)currentLex->value.i)
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
    if (node && node->type == ExpressionNode::auto_)
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
    if (lastsym->tp->BaseType()->type == BasicType::struct_)
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
        switch (lastsym->tp->BaseType()->type)
        {
            case BasicType::char_:
            case BasicType::unsigned_char_:
            case BasicType::signed_char_:
            case BasicType::char8_t_:
                return ISZ_UCHAR;
            case BasicType::bool_:
                return ISZ_BOOLEAN;
            case BasicType::short_:
            case BasicType::unsigned_short_:
            case BasicType::char16_t_:
                return ISZ_USHORT;
            case BasicType::wchar_t_:
                return ISZ_USHORT;
            case BasicType::long_:
            case BasicType::unsigned_long_:
                return ISZ_UINT;
            case BasicType::long_long_:
            case BasicType::unsigned_long_long_:
                return ISZ_ULONGLONG;
            case BasicType::int_:
            case BasicType::unsigned_:
            case BasicType::char32_t_:
                return ISZ_UINT;
            case BasicType::enum_:
                return ISZ_UINT;
            case BasicType::pointer_:
                return ISZ_UINT;
            case BasicType::seg_:
                return ISZ_USHORT;
            case BasicType::far_:
                return ISZ_FARPTR;
                //        case BasicType::memberptr_:
                //            return ISZ_UINT;
            default:
                return 1000;
        }
    }
}

/*-------------------------------------------------------------------------*/

static AMODE* inasm_mem(std::vector<int>& srcRegs)
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
        if (currentLex)
        {
            switch (currentLex->type)
            {
                case LexType::l_asmRegister_:
                    // well regimage != null but...
                    if (regimage)
                    {
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
                            if (!MATCHKW(Keyword::colon_))
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
                        setreg(srcRegs, regimage);
                    }
                    break;
                case LexType::l_wchr_:
                case LexType::l_achr_:
                case LexType::i_:
                case LexType::ui_:
                case LexType::l_:
                case LexType::ul_:
                    if (node)
                        node = MakeExpression(subtract ? ExpressionNode::sub_ : ExpressionNode::add_, node,
                                              MakeIntExpression(ExpressionNode::c_i_, currentLex->value.i));
                    else if (subtract)
                        node = MakeIntExpression(ExpressionNode::c_i_, -currentLex->value.i);
                    else
                        node = MakeIntExpression(ExpressionNode::c_i_, currentLex->value.i);
                    inasm_getsym();
                    break;
                case LexType::l_kw_:
                    if (MATCHKW(Keyword::plus_) || MATCHKW(Keyword::minus_))
                    {
                        if (node)
                            node = MakeExpression(ExpressionNode::add_, node, MakeIntExpression(ExpressionNode::c_i_, 0));
                        else
                            node = MakeIntExpression(ExpressionNode::c_i_, 0);
                        break;
                    }
                    /* fallthrough */
                default:
                    inasm_err(ERR_INVALID_INDEX_MODE);
                    return 0;
                case LexType::l_id_:
                    if (gotident || subtract)
                    {
                        inasm_err(ERR_INVALID_INDEX_MODE);
                        return 0;
                    }
                    node = inasm_ident();
                    gotident = true;
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
        if (MATCHKW(Keyword::closebr_))
        {
            inasm_getsym();
            break;
        }
        if (!MATCHKW(Keyword::plus_) && !MATCHKW(Keyword::minus_))
        {
            inasm_err(ERR_INVALID_INDEX_MODE);
            return 0;
        }
        if (MATCHKW(Keyword::minus_))
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

static AMODE* inasm_amode(int nosegreg, std::vector<int>& srcRegs, std::vector<int>& destRegs)
{
    AMODE* rv = beLocalAllocate<AMODE>();
    int sz = 0, seg = 0;
    bool done = false;
    lastsym = 0;
    inasm_txsym();
    if (currentLex)
    {
        switch (currentLex->type)
        {
            case LexType::l_wchr_:
            case LexType::l_achr_:
            case LexType::i_:
            case LexType::ui_:
            case LexType::l_:
            case LexType::ul_:
            case LexType::l_id_:
            case LexType::l_asmRegister_:
                break;
            case LexType::l_kw_:
                switch (KW())
                {
                    case Keyword::openbr_:
                    case Keyword::minus_:
                    case Keyword::plus_:
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
    if (MATCHTYPE(LexType::l_asmRegister_))
    {
        if (regimage->regtype == am_ext)
        {
            sz = inasm_getsize();
        }
    }
    while (!done)
    {
        done = true;
        if (currentLex)
        {
            switch (currentLex->type)
            {
                case LexType::l_asmRegister_:
                    if (regimage->regtype == am_ext)
                    {
                        inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                        return 0;
                    }
                    if (regimage->regtype == am_freg)
                    {
                        inasm_getsym();
                        if (MATCHKW(Keyword::openpa_))
                        {
                            inasm_getsym();
                            if (!currentLex || (currentLex->type != LexType::i_ && currentLex->type != LexType::ui_) ||
                                currentLex->value.i < 0 || currentLex->value.i > 7)
                            {
                                inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                                return 0;
                            }
                            inasm_getsym();
                            inasm_needkw(Keyword::closepa_);
                        }
                        else
                            currentLex->value.i = 0;
                        rv->preg = currentLex->value.i;
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
                        if (MATCHKW(Keyword::colon_))
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
                        if (destRegs.size() == 0)
                            setreg(destRegs, regimage);
                        else
                            setreg(srcRegs, regimage);
                        rv->preg = regimage->regnum;
                        rv->mode = (e_am)regimage->regtype;
                        sz = rv->length = regimage->size;
                        inasm_getsym();
                    }
                    break;
                case LexType::l_id_:
                    rv->mode = am_immed;
                    rv->offset = Optimizer::SymbolManager::Get(inasm_ident());
                    rv->length = ISZ_UINT;
                    if (rv->offset->type == Optimizer::se_auto)
                    {
                        inasm_err(ERR_USE_LEA);
                        return 0;
                    }
                    break;
                case LexType::l_kw_:
                    switch (KW())
                    {
                        case Keyword::openbr_:
                            rv = inasm_mem(srcRegs);
                            if (rv && rv->seg)
                                seg = rv->seg;
                            break;
                        case Keyword::minus_:
                        case Keyword::plus_:
                            rv = inasm_const();
                            break;
                        default:
                            inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                            return 0;
                    }
                    break;
                case LexType::i_:
                case LexType::ui_:
                case LexType::l_:
                case LexType::ul_:
                case LexType::l_wchr_:
                case LexType::l_achr_:
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
    rv->offset = (Optimizer::SimpleExpression*)MakeIntExpression(ExpressionNode::c_i_, i);
    return rv;
}
static AMODE* inasm_immed(void)
{
    AMODE* rv;
    if (currentLex)
        switch (currentLex->type)
        {
            case LexType::i_:
            case LexType::ui_:
            case LexType::l_:
            case LexType::ul_:
            case LexType::l_wchr_:
            case LexType::l_achr_:
                rv = aimmed(currentLex->value.i);
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
    if (currentLex)
        switch (currentLex->type)
        {
            case LexType::i_:
            case LexType::ui_:
            case LexType::l_:
            case LexType::ul_:
            case LexType::l_wchr_:
            case LexType::l_achr_:
                rv = aimmed(currentLex->value.i);
                inasm_getsym();
                return rv;
            default:
                break;
        }
    return nullptr;
}

/*-------------------------------------------------------------------------*/

e_opcode inasm_op(void)
{
    int op;
    if (!currentLex || currentLex->type != LexType::l_asmInstruction_)
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
    return o;//////////
}
static int getData(Statement* snp)
{
    int size = insdata->amode;
    EXPRESSION** newExpr = &snp->select;
    do
    {
        Type* tp;
        EXPRESSION* expr;
        getsym();
        optimized_expression(nullptr, nullptr, &tp, &expr, false);
        if (tp && (isintconst(expr) || isfloatconst(expr)))
        {
            switch (size)
            {
                case ISZ_UCHAR:
                    expr->v.i = reint(expr);
                    expr->type = ExpressionNode::c_uc_;
                    break;
                case ISZ_USHORT:
                    expr->v.i = reint(expr);
                    expr->type = ExpressionNode::c_us_;
                    break;
                case ISZ_ULONG:
                    if (isintconst(expr))
                    {
                        expr->v.i = reint(expr);
                        expr->type = ExpressionNode::c_ul_;
                    }
                    else
                    {
                        expr->v.f = Allocate<FPF>();
                        *expr->v.f = refloat(expr);
                        expr->type = ExpressionNode::c_f_;
                    }
                    break;
                case ISZ_ULONGLONG:
                    if (isintconst(expr))
                    {
                        expr->v.i = reint(expr);
                        expr->type = ExpressionNode::c_ull_;
                    }
                    else
                    {
                        expr->v.f = Allocate<FPF>();
                        *expr->v.f = refloat(expr);
                        expr->type = ExpressionNode::c_d_;
                    }
                    break;
                case ISZ_LDOUBLE:
                    expr->v.f = Allocate<FPF>();
                    *expr->v.f = refloat(expr);
                    expr->type = ExpressionNode::c_ld_;
                    break;
            }
            *newExpr = expr;
            newExpr = &(*newExpr)->left;
        }
        else
        {
            error(ERR_CONSTANT_VALUE_EXPECTED);
            SkipToNextLine();
            break;
        }
    } while (currentLex && MATCHKW(Keyword::comma_));
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
            std::shared_ptr<Instruction> newIns;
            std::list<Numeric*> operands;
            Optimizer::assembling = true;

            asmError err =
#ifndef ORANGE_NO_INASM
                instructionParser->GetInstruction(&ins1, newIns, operands);
#else
                AERR_SYNTAX;
#endif
            Optimizer::assembling = false;
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
void inlineAsm(std::list<FunctionBlock*>& parent)
{
    Statement* snp;
    OCODE* rv;
    EXPRESSION* node;
    lastsym = 0;
    inasm_txsym();
    std::vector<int> srcRegs;
    std::vector<int> destRegs;
    do
    {
        snp = Statement::MakeStatement(parent, StatementNode::passthrough_);
        if (!currentLex)
        {
            return;
        }
        bool atend;
        if (currentLex->type != LexType::l_asmInstruction_)
        {
            if (MATCHKW(Keyword::int_))
            {
                inasm_getsym();
                op = op_int;
                rv = beLocalAllocate<OCODE>();
                rv->oper1 = inasm_amode(true, srcRegs, destRegs);
                goto join;
            }
            node = inasm_label();
            if (!node)
                return;
            if (MATCHKW(Keyword::semicolon_))
                inasm_getsym();

            snp->type = StatementNode::label_;
            snp->label = node->v.i;
            return;
        }
        if (insdata->atype == op_reserved)
        {
            getData(snp);
            return;
        }
        atend = AtEol();
        op = inasm_op();
        if (op == (e_opcode)-1)
        {

            return;
        }
        {
            rv = beLocalAllocate<OCODE>();
            if (!(op == op_rep || op == op_repnz || op == op_repz || op == op_repe || op == op_repne || op == op_lock))
            {
                if (!atend && !MATCHKW(Keyword::semicolon_))
                {
                    rv->oper1 = inasm_amode(false, srcRegs, destRegs);
                    if (MATCHKW(Keyword::comma_))
                    {
                        inasm_getsym();
                        rv->oper2 = inasm_amode(false, srcRegs, destRegs);
                        if (MATCHKW(Keyword::comma_))
                        {
                            inasm_getsym();
                            rv->oper3 = inasm_amode(false, srcRegs, destRegs);
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
                return;
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
        switch (op)
        {
        case op_div:
        case op_idiv:
        case op_mul:
            if (destRegs.size())
                destRegs[0] = R_EAXEDX;
            break;
        case op_imul:
            if (!rv->oper2 && destRegs.size())
                destRegs[0] = R_EAXEDX;
            break;
        default:
            break;
        }
        unsigned char* regs = Allocate<unsigned char>(srcRegs.size() + 2);
        if (destRegs.size())
            regs[ASM_DEST_REG] = destRegs[0] + 1;
        else
            regs[ASM_DEST_REG] = 255;
        for (int i = 0; i < srcRegs.size(); i++)
            regs[i+ ASM_SRC_REG_START] = srcRegs[i] + 1;
        snp->assemblyRegs = regs;
        if (theCurrentFunc)
            theCurrentFunc->sb->noinline = true;
    } while (op == op_rep || op == op_repnz || op == op_repz || op == op_repe || op == op_repne || op == op_lock);
    return;
}
void adjustcodelab(void* select, int offset)
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
