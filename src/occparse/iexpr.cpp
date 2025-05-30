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
 * iexpr.c
 *
 * routies to take an enode list and generate icode
 */
#include <cstdio>
#include <cstdlib>
#include "compiler.h"
#include "assert.h"
#include "ppPragma.h"
#include "rtti.h"
#include "optmodules.h"
#include "Utils.h"
#include "istmt.h"
#include "init.h"

/*
 *      this module contains all of the code generation routines
 *      for evaluating expressions and conditions.
 */

#include "config.h"
#include "ccerr.h"
#include "inline.h"
#include "istmt.h"
#include "declare.h"
#include "initbackend.h"
#include "iblock.h"
#include "expr.h"
#include "declcons.h"
#include "iinline.h"
#include "ildata.h"
#include "stmt.h"
#include "OptUtils.h"
#include "lex.h"
#include "help.h"
#include "memory.h"
#include "OptUtils.h"
#include "ifloatconv.h"
#include "beinterf.h"
#include "iexpr.h"
#include "ioptimizer.h"
#ifndef ORANGE_NO_MSIL
#    include "using.h"
#endif
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "constopt.h"
#include "FNV_hash.h"
#include "types.h"

namespace Parser
{
Optimizer::SimpleSymbol* baseThisPtr;

int calling_inline;

Optimizer::IMODE* inlinereturnap;
Optimizer::IMODE* structret_imode;

static std::unordered_map<Optimizer::QUAD*, Optimizer::IMODE*, OrangeC::Utils::fnv1a32_binary<DAGCOMPARE>,
                          OrangeC::Utils::bin_eql<DAGCOMPARE>>
    name_value_hash;
static Optimizer::LIST* incdecList;
static Optimizer::LIST* incdecListLast;
static int push_nesting;
static EXPRESSION* this_bound;
static int inline_level;
static int stackblockOfs;

Optimizer::IMODE* gen_expr(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
Optimizer::IMODE* gen_void_(EXPRESSION* node, SYMBOL* funcsp);
Optimizer::IMODE* gen_relat(EXPRESSION* node, SYMBOL* funcsp);
void truejmp(EXPRESSION* node, SYMBOL* funcsp, int label);
void falsejmp(EXPRESSION* node, SYMBOL* funcsp, int label);

void iexpr_init(void)
{
    stackblockOfs = 0;
    calling_inline = 0;
    push_nesting = 0;
    this_bound = 0;
    inline_level = 0;
    if (Optimizer::architecture != ARCHITECTURE_MSIL)
    {
        Optimizer::externalSet.clear();
        Optimizer::externals.clear();
    }
}
void iexpr_func_init(void)
{
    name_value_hash.clear();
    Optimizer::loadHash.clear();
    Optimizer::castHash.clear();
    incdecList = nullptr;
}

bool HasIncDec() { return incdecList; }
void DumpIncDec(SYMBOL* funcsp)
{
    Optimizer::LIST* l = incdecList;
    incdecList = nullptr;
    incdecListLast = incdecList;
    while (l)
    {
        gen_void_((EXPRESSION*)l->data, funcsp);
        l = l->next;
    }
}
Optimizer::IMODE* LookupExpression(Optimizer::i_ops op, int size, Optimizer::IMODE* left, Optimizer::IMODE* right)
{
    if (right && right->mode == Optimizer::i_immed && right->size != left->size)
    {
        // if it is an address mode with a right constant we do this so that we can
        // find the expression later...
        if (size == ISZ_ADDR && op != Optimizer::i_lsl && op != Optimizer::i_asr && op != Optimizer::i_lsr)
        {
            if (left->mode == Optimizer::i_immed && Optimizer::isintconst(left->offset))
            {
                left = Optimizer::make_immed(right->size, left->offset->i);
                if (op == Optimizer::i_add)
                {
                    auto temp = left;
                    left = right;
                    right = temp;
                }
            }
            else if (isintconst(right->offset))
            {
                right = Optimizer::make_immed(left->size, right->offset->i);
            }
        }
    }
    Optimizer::IMODE* ap = nullptr;
    Optimizer::QUAD head = {op, left, right};
    if (!left->bits && (!right || !right->bits))
    {
        auto it = name_value_hash.find(&head);
        if (it != name_value_hash.end())
            ap = it->second;
    }
    if (!ap)
    {
        bool vol = false;
        bool rest = false;
        int pragmas = 0;
        ap = Optimizer::tempreg(size, 0);
        Optimizer::gen_icode(op, ap, left, right);
        if (!left->offset->unionoffset && (!right || !right->offset->unionoffset))
            if (!left->bits && (!right || !right->bits))
                name_value_hash[Optimizer::intermed_tail] = ap;
        if (right && right->offset)
        {
            vol = left->vol | right->vol;
            rest = left->restricted & right->restricted;
            pragmas = left->offset->pragmas & right->offset->pragmas;
        }
        else
        {
            vol = left->vol;
            rest = left->restricted;
            pragmas = left->offset->pragmas;
        }
        ap->vol = vol;
        ap->restricted = rest;
        ap->offset->pragmas = pragmas;
        Optimizer::intermed_tail->isvolatile = vol;
        Optimizer::intermed_tail->isrestrict = rest;
        Optimizer::intermed_tail->cxlimited = pragmas & STD_PRAGMA_CXLIMITED;
    }
    else
        Optimizer::gen_icode(op, ap, left, right);
    return ap;
}
int chksize(int lsize, int rsize)
/*
 * compare two sizes, ignoring sign during comparison
 */
{
    int l, r;
    l = lsize;
    r = rsize;
    if (l < 0)
        l = -l;
    if (r < 0)
        r = -r;
    return (l > r);
}

Optimizer::IMODE* make_ioffset(EXPRESSION* node)
/*
 *      make a direct reference to a node.
 */
{
    Optimizer::SimpleExpression* node1 = Optimizer::SymbolManager::Get(node->left);
    Optimizer::IMODE* ap;
    Optimizer::SimpleSymbol* sym = varsp(node1);
    if (sym && sym->imvalue && sym->imvalue->size == natural_size(node))
        return sym->imvalue;
    ap = Allocate<Optimizer::IMODE>();
    ap->offset = node1;
    ap->mode = Optimizer::i_direct;
    ap->size = natural_size(node);
    if (sym && !sym->imvalue)
        sym->imvalue = ap;
    return ap;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* make_bf(EXPRESSION* node, Optimizer::IMODE* ap, int size)
/*
 *      construct a bit field reference
 */
{
    Optimizer::IMODE* ap1 = Allocate<Optimizer::IMODE>();
    Optimizer::SimpleSymbol* sym = varsp(ap->offset);
    if (node->startbit == -1)
        diag("Illegal bit field");
    *ap1 = *ap;
    ap1->startbit = node->startbit;
    ap1->bits = node->bits;
    ap1->size = size;
    ap1->retval = false;
    if (sym)
        sym->usedasbit = true;
    return ap1;
}

static Optimizer::IMODE* gen_bit_load(Optimizer::IMODE* ap)
{
    Optimizer::IMODE* result1 = Optimizer::tempreg(ap->size, false);
    Optimizer::IMODE* result2 = Optimizer::tempreg(ap->size, false);
    int n = (1 << ap->bits) - 1;
    Optimizer::gen_icode(ap->size < 0 ? Optimizer::i_asr : Optimizer::i_lsr, result1, ap,
                         Optimizer::make_immed(-ISZ_UINT, ap->startbit));
    Optimizer::gen_icode(Optimizer::i_and, result2, result1, Optimizer::make_immed(-ISZ_UINT, n));
    ap->bits = ap->startbit = 0;
    return result2;
}
static Optimizer::IMODE* gen_bit_mask(Optimizer::IMODE* ap)
{
    Optimizer::IMODE* result = Optimizer::tempreg(ap->size, false);
    int n = ~(((1 << ap->bits) - 1) << ap->startbit);
    Optimizer::gen_icode(Optimizer::i_and, result, ap, Optimizer::make_immed(-ISZ_UINT, n));
    ap->bits = ap->startbit = 0;
    return result;
}

static int bitintbits(EXPRESSION* node)
{
    while (node)
    {
        switch (node->type)
        {
            case ExpressionNode::l_bitint_:
            case ExpressionNode::c_bitint_:
            case ExpressionNode::x_bitint_:
                return -node->v.b.bits;
            case ExpressionNode::l_ubitint_:
            case ExpressionNode::c_ubitint_:
            case ExpressionNode::x_ubitint_:
                return node->v.b.bits;
            case ExpressionNode::callsite_: {
                if (node->v.func->returnSP && node->v.func->returnSP->tp->IsBitInt())
                {
                    return (node->v.func->returnSP->tp->type == BasicType::bitint_ ? -1 : 1) *
                           node->v.func->returnSP->tp->bitintbits;
                }
                return 0;
            }
            case ExpressionNode::not_:
                return 0;
            default:
                if (IsCastValue(node))
                    return 0;
                node = node->left;
                break;
        }
    }
    return 0;
}
static Optimizer::IMODE* bitint_unary(EXPRESSION* node)
{
    static std::unordered_map<ExpressionNode, const char*, EnumClassHash> funcs = {{ExpressionNode::uminus_, "___biminus"},
                                                                                   {ExpressionNode::compl_, "___bicompl"}};
    int b;
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, b = bitintbits(node->left)), nullptr);
    auto left = gen_expr(nullptr, node->left, 0, ISZ_ADDR);  // address
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
    auto ans1 = anonymousBits(StorageClass::auto_, node->left->type == ExpressionNode::l_ubitint_, abs(b));
    auto ans = gen_expr(nullptr, ans1, 0, ISZ_ADDR);
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, ans, nullptr);
    auto c = funcs[node->type];
    if (c == nullptr)
    {
        diag("Invalid bitint unary math function");
        return nullptr;
    }
    call_library(c, -getSize(BasicType::pointer_) * 2 - getSize(BasicType::int_));
    return ans;
}
static Optimizer::IMODE* bitint_binary(EXPRESSION* node)
{
    static std::unordered_map<ExpressionNode, const char*, EnumClassHash> funcs = {
        {ExpressionNode::add_, "___biadd"},   {ExpressionNode::sub_, "___bisub"},   {ExpressionNode::and_, "___biand"},
        {ExpressionNode::or_, "___bior"},     {ExpressionNode::xor_, "___bixor"},   {ExpressionNode::mul_, "___bimul"},
        {ExpressionNode::umul_, "___biumul"}, {ExpressionNode::div_, "___bidiv"},   {ExpressionNode::udiv_, "___biudiv"},
        {ExpressionNode::mod_, "___bimod"},   {ExpressionNode::umod_, "___biumod"}, {ExpressionNode::lsh_, "___bilsh"},
        {ExpressionNode::rsh_, "___birsh"},   {ExpressionNode::ursh_, "___biursh"}};
    auto c = funcs[node->type];
    if (c == nullptr)
    {
        diag("Invalid bitint binary math function");
        return nullptr;
    }
    switch (node->type)
    {
        case ExpressionNode::lsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::ursh_: {
            int isz = natural_size(node->right);
            auto right = gen_expr(nullptr, node->right, 0, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, right, nullptr);
            int b;
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, b = bitintbits(node->left)), nullptr);
            auto left = gen_expr(nullptr, node->left, 0, ISZ_ADDR);  // address
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
            auto ans1 = anonymousBits(StorageClass::auto_, node->left->type == ExpressionNode::l_ubitint_, abs(b));
            auto ans = gen_expr(nullptr, ans1, 0, ISZ_ADDR);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, ans, nullptr);
            call_library(c, -getSize(BasicType::pointer_) * 2 - getSize(BasicType::int_) * 2);
            return ans;
        }
        default: {
            int b;
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, b = bitintbits(node->left)), nullptr);
            auto right = gen_expr(nullptr, node->right, 0, ISZ_ADDR);  // address
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, right, nullptr);
            auto left = gen_expr(nullptr, node->left, 0, ISZ_ADDR);  // address
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
            auto ans1 = anonymousBits(StorageClass::auto_, node->left->type == ExpressionNode::l_ubitint_, abs(b));
            auto ans = gen_expr(nullptr, ans1, 0, ISZ_ADDR);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, ans, nullptr);
            call_library(c, -getSize(BasicType::pointer_) * 3 - getSize(BasicType::int_));
            return ans;
        }
    }
}
static Optimizer::IMODE* bitint_compare(EXPRESSION* node, Optimizer::i_ops btype, int label)
{
    const char* c = nullptr;
    switch (node->type)
    {
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::lt_:
        case ExpressionNode::ge_:
        case ExpressionNode::le_:
        case ExpressionNode::gt_:
            c = "___bicompares";
            break;
        case ExpressionNode::ult_:
        case ExpressionNode::uge_:
        case ExpressionNode::ule_:
        case ExpressionNode::ugt_:
            c = "___bicompareu";
            switch (btype)
            {
                case Optimizer::i_jbe:
                    btype = Optimizer::i_jle;
                    break;
                case Optimizer::i_ja:
                    btype = Optimizer::i_jg;
                    break;
                case Optimizer::i_jc:
                    btype = Optimizer::i_jl;
                    break;
                case Optimizer::i_jnc:
                    btype = Optimizer::i_jge;
                    break;
            }
            break;
        default:
            diag("invalid bitint comparison");
            return nullptr;
    }
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, bitintbits(node->left)), nullptr);
    auto right = gen_expr(nullptr, node->right, 0, ISZ_ADDR);  // address
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, right, nullptr);
    auto left = gen_expr(nullptr, node->left, 0, ISZ_ADDR);  // address
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
    auto imv = call_library(c, -getSize(BasicType::pointer_) * 2 - getSize(BasicType::int_));
    if (label >= 0)
    {
        gen_icgoto(btype, label, imv, Optimizer::make_immed(ISZ_UINT, 0));
    }
    else
    {
        auto im = Optimizer::tempreg(ISZ_UINT, 0);
        Optimizer::gen_icode(btype, im, imv, nullptr);
        imv = im;
    }
    return imv;
}
static Optimizer::IMODE* bitint_iszero(EXPRESSION* node, bool match, int label)
{
    const char* c = "___biiszero";
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, bitintbits(node)), nullptr);
    auto left = gen_expr(nullptr, node, 0, ISZ_ADDR);  // address
    Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
    auto imv = call_library(c, -getSize(BasicType::pointer_) - getSize(BasicType::int_));
    if (label >= 0)
    {
        gen_icgoto(match ? Optimizer::i_jne : Optimizer::i_je, label, imv, Optimizer::make_immed(ISZ_UINT, 0));
    }
    else
    {
        auto im = Optimizer::tempreg(ISZ_UINT, 0);
        Optimizer::gen_icode(match ? Optimizer::i_setne : Optimizer::i_sete, im, imv, Optimizer::make_immed(ISZ_UINT, 0));
        imv = im;
    }
    return imv;
}
static Optimizer::IMODE* bitint_convert(EXPRESSION* node, int size)
{
    auto sz = 0;
    int isz = natural_size(node);
    if (abs(isz) == ISZ_BITINT)
    {
        int isz1 = natural_size(node->left);
        if (abs(isz1) == ISZ_BITINT)
        {
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, bitintbits(node->left)), nullptr);
            auto left = gen_expr(nullptr, node->left, 0, ISZ_ADDR);  // address
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, bitintbits(node)), nullptr);
            auto ans1 = anonymousBits(StorageClass::auto_, node->left->type == ExpressionNode::l_ubitint_, node->v.b.bits);
            auto ans = gen_expr(nullptr, ans1, 0, ISZ_ADDR);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, ans, nullptr);
            call_library("___biconvertb", -getSize(BasicType::pointer_) * 2 - getSize(BasicType::int_) * 2);
            return ans;
        }
        else
        {
            sz = Optimizer::sizeFromISZ(isz1) * CHAR_BIT * (isz1 < 0 ? -1 : 1);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, sz), nullptr);
            auto exp = node->left;
            cast(&stdlonglong, &exp);
            auto left = gen_expr(nullptr, exp, 0, ISZ_ULONGLONG);  // address
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, bitintbits(node)), nullptr);
            auto ans1 = anonymousBits(StorageClass::auto_, node->left->type == ExpressionNode::l_ubitint_, abs(bitintbits(node)));
            auto ans = gen_expr(nullptr, ans1, 0, ISZ_ADDR);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, ans, nullptr);
            call_library("___biconvertw",
                         -getSize(BasicType::pointer_) - getSize(BasicType::int_) * 2 - getSize(BasicType::long_long_));
            return ans;
        }
    }
    else
    {
        // isz1 == +-ISZ_BITINT
        sz = Optimizer::sizeFromISZ(isz);
        Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, sz * CHAR_BIT * (isz < 0 ? -1 : 1)),
                             nullptr);
        Optimizer::gen_icode(Optimizer::i_parm, nullptr, Optimizer::make_immed(ISZ_UINT, bitintbits(node->left)), nullptr);
        auto left = gen_expr(nullptr, node->left, 0, ISZ_ADDR);  // address
        Optimizer::gen_icode(Optimizer::i_parm, nullptr, left, nullptr);
        int size = getSize(BasicType::pointer_) + getSize(BasicType::int_) * 2;

        // this is call_library with a long long return result
        Optimizer::IMODE *result, *ap1;
        result = set_symbol("___biconvert", 1);
        Optimizer::gen_icode(Optimizer::i_gosub, 0, result, 0);
        ap1 = Optimizer::tempreg(isz, 0);
        ap1->retval = true;
        result = Optimizer::tempreg(isz, 0);
        gen_icode(Optimizer::i_assn, result, ap1, nullptr);
        Optimizer::gen_icode(Optimizer::i_parmadj, 0, Optimizer::make_parmadj(0),
                             Optimizer::make_parmadj(getSize(BasicType::pointer_) + getSize(BasicType::int_) * 2));
        return result;
    }
}
static Optimizer::IMODE* bitint_assign(EXPRESSION* node)
{
    auto bytes = abs(bitintbits(node->right)) + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
    bytes /= Optimizer::chosenAssembler->arch->bitintunderlying;
    bytes *= Optimizer::chosenAssembler->arch->bitintunderlying;
    bytes /= CHAR_BIT;
    auto ap6 = Optimizer::make_immed(ISZ_UINT, bytes);
    auto left = gen_expr(nullptr, node->right, 0, ISZ_ADDR);  // address
    auto ans = gen_expr(nullptr, node->left, 0, ISZ_ADDR);    // address
    Optimizer::gen_icode(Optimizer::i_assnblock, ap6, ans, left);
    return ans;
}
static Optimizer::IMODE* make_bitint(EXPRESSION* node)
{
    auto var = anonymousBits(StorageClass::static_, natural_size(node) > 0, node->v.b.bits);
    insertInitSym(var->v.sp);
    InsertInitializer(&var->v.sp->sb->init, var->v.sp->tp, node, 0, false);
    auto im = Allocate<Optimizer::IMODE>();
    im->mode = Optimizer::i_immed;
    im->offset = Optimizer::SymbolManager::Get(var);
    im->size = natural_size(node);

    return im;
}
static int bitint_push(EXPRESSION* node)
{
    // constructor or other function creating a structure on the stack
    int rv = abs(bitintbits(node)) + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
    rv /= Optimizer::chosenAssembler->arch->bitintunderlying;
    rv *= Optimizer::chosenAssembler->arch->bitintunderlying;
    rv /= CHAR_BIT;
    if (rv % Optimizer::chosenAssembler->arch->stackalign)
        rv = rv + Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
    Optimizer::IMODE *ap, *ap1, *ap2, *ap6;
    Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0), Optimizer::make_immed(ISZ_UINT, rv),
                         nullptr);

    ap2 = gen_expr(nullptr, node, 0, ISZ_UINT);
    ap1 = Optimizer::LookupLoadTemp(nullptr, ap2);
    if (ap1 != ap2)
        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
    ap6 = Optimizer::make_immed(ISZ_UINT, rv);
    Optimizer::gen_icode(Optimizer::i_assnblock, ap6, ap, ap2);
    return rv;
}
/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_deref(EXPRESSION* node, SYMBOL* funcsp, int flags)
/*
 *      return the addressing mode of a dereferenced node.
 */
{
    Optimizer::IMODE *ap1, *ap2, *ap3;
    int siz1;
    Optimizer::SimpleSymbol* sym = nullptr;
    ExpressionNode nt = node->left->type;
    int store = flags & F_STORE;
    flags &= ~F_STORE;
    (void)flags;
    /* get size */
    switch (node->type) /* get load size */
    {
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
            siz1 = ISZ_ADDR;
            break;
        case ExpressionNode::l_bool_:
            siz1 = ISZ_BOOLEAN;
            break;
        case ExpressionNode::l_uc_:
            siz1 = ISZ_UCHAR;
            break;
        case ExpressionNode::l_c_:
            siz1 = -ISZ_UCHAR;
            break;
        case ExpressionNode::l_u16_:
            siz1 = ISZ_U16;
            break;
        case ExpressionNode::l_u32_:
            siz1 = ISZ_U32;
            break;
        case ExpressionNode::l_wc_:
            siz1 = ISZ_WCHAR;
            break;
        case ExpressionNode::l_us_:
            siz1 = ISZ_USHORT;
            break;
        case ExpressionNode::l_s_:
            siz1 = -ISZ_USHORT;
            break;
        case ExpressionNode::l_i_:
            siz1 = -ISZ_UINT;
            break;
        case ExpressionNode::l_inative_:
            siz1 = -ISZ_UNATIVE;
            break;
        case ExpressionNode::l_unative_:
            siz1 = ISZ_UNATIVE;
            break;
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
            siz1 = ISZ_ADDR;
            break;
        case ExpressionNode::l_fp_:
            siz1 = ISZ_FARPTR;
            break;
        case ExpressionNode::l_sp_:
            siz1 = ISZ_SEG;
            break;
        case ExpressionNode::l_l_:
            siz1 = -ISZ_ULONG;
            break;
        case ExpressionNode::l_bitint_:
            siz1 = -ISZ_BITINT;
            break;
        case ExpressionNode::l_ubitint_:
            siz1 = ISZ_BITINT;
            break;
        case ExpressionNode::add_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::structadd_:
            siz1 = ISZ_ADDR;
            break;
        case ExpressionNode::l_ui_:
            siz1 = ISZ_UINT;
            break;
        case ExpressionNode::l_ul_:
            siz1 = ISZ_ULONG;
            break;
        case ExpressionNode::l_ll_:
            siz1 = -ISZ_ULONGLONG;
            break;
        case ExpressionNode::l_ull_:
            siz1 = ISZ_ULONGLONG;
            break;
        case ExpressionNode::l_f_:
            siz1 = ISZ_FLOAT;
            break;
        case ExpressionNode::l_d_:
            siz1 = ISZ_DOUBLE;
            break;
        case ExpressionNode::l_ld_:
            siz1 = ISZ_LDOUBLE;
            break;
        case ExpressionNode::l_fi_:
            siz1 = ISZ_IFLOAT;
            break;
        case ExpressionNode::l_di_:
            siz1 = ISZ_IDOUBLE;
            break;
        case ExpressionNode::l_ldi_:
            siz1 = ISZ_ILDOUBLE;
            break;
        case ExpressionNode::l_fc_:
            siz1 = ISZ_CFLOAT;
            break;
        case ExpressionNode::l_dc_:
            siz1 = ISZ_CDOUBLE;
            break;
        case ExpressionNode::l_ldc_:
            siz1 = ISZ_CLDOUBLE;
            break;
        case ExpressionNode::l_bit_:
            siz1 = ISZ_BIT;
            break;
        case ExpressionNode::l_string_:
            siz1 = ISZ_STRING;
            break;
        case ExpressionNode::l_object_:
            siz1 = ISZ_OBJECT;
            break;
        default:
            siz1 = ISZ_UINT;
    }
    EXPRESSION* esym = GetSymRef(node->left);
    if (Optimizer::architecture != ARCHITECTURE_MSIL && esym && esym->type != ExpressionNode::labcon_ &&
        !esym->v.sp->tp->IsStructured() && (!esym->v.sp->tp->IsRef() || !esym->v.sp->tp->BaseType()->btp->IsStructured()) &&
        !esym->v.sp->sb->thisPtr &&
        //    if (Optimizer::architecture != ARCHITECTURE_MSIL && esym && esym->type != ExpressionNode::labcon_ &&
        //    !esym->v.sp->tp->IsStructured() && !esym->v.sp->sb->thisPtr &&
        siz1 != SizeFromType(esym->v.sp->tp))
    {
        // natural size of the symbol isn't the same as the size we are saving/loading
        // we can't do the simple dereference here
        ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
        ap2 = Optimizer::LookupLoadTemp(nullptr, ap1);
        if (ap2 != ap1)
        {
            Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
            ap1 = ap2;
        }
        ap1 = Optimizer::indnode(ap1, siz1);
    }
    /* Dereference for add nodes */
    else if ((Optimizer::architecture == ARCHITECTURE_MSIL) &&
             (node->left->type == ExpressionNode::bits_ ||
              (node->left->type == ExpressionNode::structadd_ && node->left->right->type == ExpressionNode::structelem_)))
    {
        // prepare for the MSIL ldfld instruction
        Optimizer::IMODE *aa1, *aa2;
        EXPRESSION* bitnode = node->left->type == ExpressionNode::bits_ ? node->left : nullptr;
        if (bitnode)
            node = node->left;
        aa1 = gen_expr(funcsp, node->left->left, 0, ISZ_ADDR);
        aa1->msilObject = true;

        aa2 = Optimizer::tempreg(aa1->size, 0);
        Optimizer::gen_icode(Optimizer::i_assn, aa2, aa1, nullptr);

        aa1 = Allocate<Optimizer::IMODE>();
        aa1->offset = Optimizer::SymbolManager::Get(node->left->right);
        aa1->mode = Optimizer::i_direct;
        aa1->size = aa2->size;
        ap1 = LookupExpression(Optimizer::i_add, aa1->size, aa2, aa1);
        ap1 = Optimizer::indnode(ap1, siz1);
        ap1->fieldname = true;
        ap1->vararg = aa1->offset;
        ap1->vol = node->isvolatile;
        ap1->restricted = node->isrestrict;
        if (bitnode)
        {
            ap1->bits = bitnode->bits;
            ap1->startbit = bitnode->startbit;
        }
    }
    else if (node->left->type == ExpressionNode::add_ || node->left->type == ExpressionNode::arrayadd_ ||
             node->left->type == ExpressionNode::structadd_)
    {
        if (node->left->type == ExpressionNode::structadd_ && isconstzero(&stdint, node->left->right))
        {
            if (node->left->left->type == ExpressionNode::callsite_ || node->left->left->type == ExpressionNode::thisref_)
            {
                ap1 = gen_expr(funcsp, node->left->left, store ? 0 : F_RETURNREFBYVALUE, ISZ_ADDR);
            }
            else if (node->left->left->type == ExpressionNode::l_p_ && node->left->left->left->type == ExpressionNode::auto_)
            {
                if (node->left->left->left->v.sp->sb->thisPtr && inlineSymThisPtr.size() && inlineSymThisPtr.back())
                {
                    EXPRESSION* exp = inlineSymThisPtr.back();
                    if (IsLValue(exp) && exp->left->type == ExpressionNode::paramsubstitute_ && exp->left->left)
                    {
                        return exp->left->v.imode;
                    }
                    else
                    {
                        ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
                    }
                }
                else
                {
                    auto sym = Optimizer::SymbolManager::Get(node->left->left->left->v.sp);
                    if (sym->paramSubstitute && sym->paramSubstitute->left->left)
                    {
                        return sym->paramSubstitute->left->v.imode;
                    }
                    else
                    {
                        ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
                    }
                }
            }
            else if (node->left->left->type == ExpressionNode::l_p_ &&
                     node->left->left->left->type == ExpressionNode::paramsubstitute_ && node->left->left->left->left)
            {
                return node->left->left->left->v.imode;
            }
            else
            {
                ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
            }
        }
        else if (node->left->type == ExpressionNode::structadd_ && isconstzero(&stdint, node->left->left))
        {
            if (node->left->right->type == ExpressionNode::callsite_ || node->left->right->type == ExpressionNode::thisref_)
            {
                ap1 = gen_expr(funcsp, node->left->right, store ? 0 : F_RETURNREFBYVALUE, ISZ_ADDR);
            }
            else if (node->left->right->type == ExpressionNode::l_p_ && node->left->right->left->type == ExpressionNode::auto_ &&
                     node->left->right->left->v.sp->sb->thisPtr && inlineSymThisPtr.size() && inlineSymThisPtr.back())
            {
                EXPRESSION* exp = inlineSymThisPtr.back();
                if (IsLValue(exp) && exp->left->type == ExpressionNode::paramsubstitute_ && exp->left->left)
                {
                    return exp->left->v.imode;
                }
                else
                {
                    ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
                }
            }
            else
            {
                ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
            }
        }
        else
        {
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
        }
        ap2 = Optimizer::LookupLoadTemp(nullptr, ap1);
        if (ap2 != ap1)
        {
            Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
            ap1 = ap2;
        }
        if (!ap1->returnRefByVal)
        {
            ap1 = Optimizer::indnode(ap1, siz1);
        }
        if (ap1->offset)
        {
            ap1->vol = node->isvolatile;
            ap1->restricted = node->isrestrict;
        }
    }
    else if ((Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND) &&
             (node->left->type == ExpressionNode::global_ || node->left->type == ExpressionNode::auto_) &&
             ((node->left->v.sp->tp->IsArray() && node->left->v.sp->sb->storage_class != StorageClass::parameter_ &&
               !node->left->v.sp->tp->BaseType()->msil) ||
              node->left->v.sp->tp->IsStructured()))
    {
        ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
        ap2 = Optimizer::LookupLoadTemp(nullptr, ap1);
        if (ap2 != ap1)
        {
            Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
            ap1 = ap2;
        }
        ap1 = Optimizer::indnode(ap1, siz1);
    }
    else if (node->left->type == ExpressionNode::const_)
    {
        ap1 = gen_expr(funcsp, node->left->v.sp->sb->init->front()->exp, 0, 0);
    }
    /* Dereference for auto variables */
    else if (node->left->type == ExpressionNode::imode_)
    {
        ap1 = (Optimizer::IMODE*)node->left->v.imode;
    }
    else
    {
        /* other Dereference */
        switch (nt)
        {
            EXPRESSION* node1;
            Optimizer::IMODE* ap2;
            case ExpressionNode::labcon_:
                ap1 = make_ioffset(node);
                if (!store)
                {
                    ap2 = Optimizer::LookupLoadTemp(nullptr, ap1);
                    if (ap2 != ap1)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                        ap1 = ap2;
                    }
                }
                break;
            case ExpressionNode::threadlocal_:
                sym = Optimizer::SymbolManager::Get(node->left->v.sp);
                ap1 = make_ioffset(node);
                ap2 = Optimizer::LookupLoadTemp(ap1, ap1);
                if (ap1 != ap2)
                    Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = Optimizer::indnode(ap2, siz1);
                Optimizer::EnterExternal(sym);
                break;
            case ExpressionNode::auto_:
                if (inlineSymStructPtr.size())
                {
                    if (node->left->v.sp->sb->retblk)
                    {
                        auto exp = inlineSymStructPtr.back();
                        if (IsLValue(exp) && exp->left->type == ExpressionNode::paramsubstitute_)
                        {
                            return gen_expr(funcsp, exp->left->left, 0, ISZ_ADDR);
                        }
                        return gen_expr(funcsp, inlineSymStructPtr.back(), 0, ISZ_ADDR);
                    }
                }
                sym = Optimizer::SymbolManager::Get(node->left->v.sp);
                if (sym->thisPtr)
                {
                    if (inlineSymThisPtr.size())
                    {
                        if (sym != baseThisPtr)
                        {
                            EXPRESSION* exp = inlineSymThisPtr.back();
                            if (exp)
                            {
                                if (IsLValue(exp) && exp->left->type == ExpressionNode::paramsubstitute_)
                                {
                                    if (exp->left->left)
                                        return gen_expr(funcsp, exp->left->left, 0, natural_size(exp->left->left));
                                    return exp->left->v.imode;
                                }
                                return gen_expr(funcsp, exp, 0, natural_size(exp));
                            }
                        }
                    }
                    else if (baseThisPtr)  // for this ptrs inherited in a constructor
                    {
                        if (sym != baseThisPtr)
                        {
                            sym = baseThisPtr;
                        }
                    }
                }
                if (sym->storage_class == Optimizer::scc_parameter && sym->paramSubstitute)
                {
                    node = sym->paramSubstitute;
                    if (node->left && node->left->type == ExpressionNode::paramsubstitute_)
                    {
                        ap1 = node->left->v.imode;
                        break;
                    }
                    // paramsubstitute is a NORMAL expression which is either an IsLValue reference
                    // or the address of a symbol
                    if (!node->left)
                    {
                        return gen_expr(funcsp, node, 0, ISZ_ADDR);
                    }
                    sym = Optimizer::SymbolManager::Get(sym->paramSubstitute->left->v.sp);
                }
                sym->allocate = true;
                if (catchLevel)
                    sym->inCatch = true;
                if (!sym->imaddress && catchLevel)
                {
                    ap1 = make_ioffset(node);
                    ap1->runtimeData = node->left->runtimeData;
                    if (ap1->runtimeData)
                    {
                        ap1->runtimeData->runtimeSym = Optimizer::SymbolManager::Get(ap1->runtimeData->runtimeSymOrig);
                    }
                    break;
                }
                // fall through
            case ExpressionNode::global_:
            case ExpressionNode::pc_:
            case ExpressionNode::absolute_:
                if (nt == ExpressionNode::global_ || nt == ExpressionNode::pc_ || nt == ExpressionNode::absolute_)
                {
                    sym = Optimizer::SymbolManager::Get(node->left->v.sp);
                    Optimizer::EnterExternal(sym);
                }
                if (!sym->stackblock)
                {
                    ap1 = make_ioffset(node);
                    ap1->runtimeData = node->left->runtimeData;
                    if (ap1->runtimeData)
                    {
                        ap1->runtimeData->runtimeSym = Optimizer::SymbolManager::Get(ap1->runtimeData->runtimeSymOrig);
                    }
                    if (!store)
                    {
                        ap2 = Optimizer::LookupLoadTemp(nullptr, ap1);
                        if (ap2 != ap1)
                        {
                            Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                            ap1 = ap2;
                        }
                    }
                    break;
                }
            // fall through
            default:
                ap1 = gen_expr(funcsp, node->left, store ? 0 : F_RETURNREFBYVALUE, natural_size(node->left)); /* generate address */
                ap2 = Optimizer::LookupLoadTemp(nullptr, ap1);
                if (!ap1->returnRefByVal)
                {
                    if (ap2 != ap1)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                        ap2 = Optimizer::indnode(ap2, siz1);
                        ap2->bits = ap1->bits;
                        ap2->startbit = ap1->startbit;
                        ap1 = ap2;
                    }
                    else
                    {
                        ap2 = Optimizer::indnode(ap1, siz1);
                        ap2->bits = ap1->bits;
                        ap2->startbit = ap1->startbit;
                        ap1 = ap2;
                    }
                }
                break;
            case ExpressionNode::paramsubstitute_:
                ap1 = node->left->v.imode;
                break;
        }
    }
    if (!store)
    {
        ap2 = Optimizer::LookupLoadTemp(nullptr, ap1);
        if (ap2 != ap1)
        {
            Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
            if (ap1->bits > 0 && (Optimizer::chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
            {
                ap2->bits = ap1->bits;
                ap2->startbit = ap1->startbit;
                ap1->bits = ap1->startbit = 0;
                ap2 = gen_bit_load(ap2);
            }
            ap1 = ap2;
        }
    }
    if (ap1->offset)
    {
        ap1->vol = node->isvolatile;
        ap1->restricted = node->isrestrict;
    }
    return ap1;
}
/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_unary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op)
/*
 *      generate code to evaluate a unary Keyword::minus_ or complement.
 */
{
    if (bitintbits(node->left))
        return bitint_unary(node);
    Optimizer::IMODE *ap, *ap1;
    (void)flags;
    ap1 = gen_expr(funcsp, node->left, F_VOL, size);
    ap = Optimizer::LookupLoadTemp(nullptr, ap1);
    if (ap1 != ap)
        Optimizer::gen_icode(Optimizer::i_assn, ap, ap1, nullptr);
    ap = LookupExpression(op, size, ap, nullptr);
    ap->vol = node->left->isvolatile;
    ap->restricted = node->left->isrestrict;
    return ap;
}

/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_binary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op)
/*
 *      generate code to evaluate a binary node and return
 *      the addressing mode of the result.
 */
{
    if (bitintbits(node->left))
        return bitint_binary(node);
    Optimizer::IMODE *ap, *ap1, *ap2, *ap3;
    int vol, rest, pragmas;
    (void)flags;
    ap3 = gen_expr(funcsp, node->left, flags, natural_size(node->left));
    ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);

    if (op == Optimizer::i_lsl || op == Optimizer::i_lsr || op == Optimizer::i_asr)
        Optimizer::flush_dag();

    ap3 = gen_expr(funcsp, node->right, flags, natural_size(node->right));
    ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
    if (ap1->size >= ISZ_CFLOAT || ap2->size >= ISZ_CFLOAT)
        size = imax(ap1->size, ap2->size);
    else if (ap1->size >= ISZ_IFLOAT && ap2->size < ISZ_IFLOAT)
    {
        if (op == Optimizer::i_add)
            size = ap1->size - ISZ_IFLOAT + ISZ_CFLOAT;
        else
            size = ap1->size;
    }
    else if (ap2->size >= ISZ_IFLOAT && ap1->size < ISZ_IFLOAT)
    {
        if (op == Optimizer::i_add)
            size = ap2->size - ISZ_IFLOAT + ISZ_CFLOAT;
        else
            size = ap2->size;
    }
    else if (ap1->size >= ISZ_IFLOAT && ap2->size >= ISZ_IFLOAT)
    {
        if (op == Optimizer::i_mul || op == Optimizer::i_sdiv)
            size = ap2->size - ISZ_IFLOAT + ISZ_FLOAT;
    }
    ap = LookupExpression(op, size, ap1, ap2);
    return ap;
}
// int size is assumed to be 32 bits, needs fixing...
#define N 32
#define oneshl32 (((unsigned long long)1) << N)
static int ChooseMultiplier(unsigned d, int prec, unsigned long long* m, int* sh, int* l)
{
    if (d != 0)
    {
        unsigned long long ml, mh;
        unsigned n = d;
        int l1 = 0;
        while (n != 1)
            l1++, n >>= 1;
        if ((1 << l1) != d)
            l1++;
        *sh = *l = l1;
        ml = (oneshl32 << l1) / d;
        mh = ((oneshl32 << l1) + (oneshl32 >> (prec - l1))) / d;
        while (*sh && ml / 2 < mh / 2)
            (*sh)--, ml /= 2, mh /= 2;
        *m = mh;
        return 1;
    }
    return 0;
}

Optimizer::IMODE* gen_udivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op, bool mod)
{
    if (bitintbits(node->left))
        return bitint_binary(node);
    int n = natural_size(node);
    if (!(Optimizer::chosenAssembler->arch->denyopts & DO_NOFASTDIV) &&
        (n == ISZ_UINT || n == -ISZ_UINT || n == ISZ_ULONG || n == -ISZ_ULONG) && isintconst(node->right))
    {
        unsigned long long m;
        int post, l, pre = 0;
        unsigned d = node->right->v.i;
        if (d == 1)
            return gen_expr(funcsp, node->left, flags, size);
        if (ChooseMultiplier(d, N, &m, &post, &l))
        {
            Optimizer::IMODE *num = gen_expr(funcsp, node->left, F_VOL, size), *ap, *ap1, *ap2, *ap3, *ap4, *ap5;
            Optimizer::QUAD* q;
            ap1 = Optimizer::LookupLoadTemp(nullptr, num);
            ap = Optimizer::tempreg(n, 0);
            if (ap1 != num)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap1, num, nullptr);
                num = ap1;
            }
            if (m >= oneshl32 && !(d & 1))
            {
                int ld;
                unsigned x = d & (oneshl32 - d);
                while (!(x & 1))
                {
                    x >>= 1;
                    pre++;
                }
                ChooseMultiplier(d >> pre, N - pre, &m, &post, &ld);
            }

            if (d == 1 << l)
            {
                if (mod)
                {
                    Optimizer::gen_icode(Optimizer::i_and, ap, num, Optimizer::make_immed(ISZ_UINT, (1 << l) - 1));
                    return ap;
                }
                ap5 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_lsr, ap5, num, Optimizer::make_immed(ISZ_UINT, l));

                //                return n >> l;
            }
            else if (m >= oneshl32)
            {
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode_with_conflict(Optimizer::i_assn, ap3, num, nullptr, true);
                ap1 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_muluh, ap1, ap3, Optimizer::make_immed(ISZ_UINT, m - oneshl32));
                ap2 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_sub, ap2, num, ap1);
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_lsr, ap3, ap2, Optimizer::make_immed(ISZ_UINT, 1));
                ap4 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_add, ap4, ap1, ap3);
                ap5 = Optimizer::tempreg(n, 0);
                if (post - 1)
                    Optimizer::gen_icode(Optimizer::i_lsr, ap5, ap4, Optimizer::make_immed(ISZ_UINT, post - 1));
                else
                    Optimizer::gen_icode(Optimizer::i_assn, ap5, ap4, nullptr);
                /*
                unsigned t1 = ((m-oneshl32) *n) >> 32;
                unsigned q = (n-t1)>>1;
                return (t1 + q) >> (post-1);
                */
            }
            else
            {
                ap1 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode_with_conflict(Optimizer::i_assn, ap1, num, nullptr, true);
                ap3 = Optimizer::tempreg(n, 0);
                if (pre)
                {
                    Optimizer::gen_icode(Optimizer::i_lsr, ap3, ap1, Optimizer::make_immed(ISZ_UINT, pre));
                }
                else
                {
                    Optimizer::gen_icode(Optimizer::i_assn, ap3, ap1, nullptr);
                }
                ap2 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_muluh, ap2, ap3, Optimizer::make_immed(ISZ_UINT, m - oneshl32));
                ap5 = Optimizer::tempreg(n, 0);
                if (post)
                    Optimizer::gen_icode(Optimizer::i_lsr, ap5, ap2, Optimizer::make_immed(ISZ_UINT, post));
                else
                    Optimizer::gen_icode(Optimizer::i_assn, ap5, ap2, nullptr);
                /*
                unsigned t1 = n >> pre;
                t1 = (m * t1)>>32;
                return t1 >> post;
                */
            }
            if (mod)
            {
                ap2 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_mul, ap2, ap5, Optimizer::make_immed(ISZ_UINT, d));
                Optimizer::gen_icode(Optimizer::i_sub, ap, num, ap2);
            }
            else
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap, ap5, nullptr);
            }
            return ap;
        }
    }
    return gen_binary(funcsp, node, flags, size, op);
}
Optimizer::IMODE* gen_sdivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op, bool mod)
{
    if (bitintbits(node->left))
        return bitint_binary(node);
    int n = natural_size(node);
    if (!(Optimizer::chosenAssembler->arch->denyopts & DO_NOFASTDIV) && (n == ISZ_UINT || n == -ISZ_UINT) &&
        node->right->type == ExpressionNode::c_i_)
    {
        int d = node->right->v.i;
        int ad = d < 0 ? -d : d, q;
        unsigned long long m;
        int post, l, pre = 0;
        if (ChooseMultiplier(ad, 31, &m, &post, &l))
        {
            Optimizer::IMODE *num = gen_expr(funcsp, node->left, F_VOL, size), *ap, *ap1, *ap2, *ap3, *ap4, *ap5 = nullptr;
            Optimizer::QUAD* q;
            ap1 = Optimizer::LookupLoadTemp(nullptr, num);
            ap = Optimizer::tempreg(n, 0);
            if (ap1 != num)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap1, num, nullptr);
                num = ap1;
            }
            if (ad == 1)
            {
                if (d < 0)
                    Optimizer::gen_icode(Optimizer::i_neg, num, nullptr, nullptr);
                return num;
            }
            //                q = ad;
            else if (ad == 1 << l)
            {
                ap2 = Optimizer::tempreg(n, 0);
                if (l > 1)
                    Optimizer::gen_icode(Optimizer::i_asr, ap2, num, Optimizer::make_immed(ISZ_UINT, l - 1));
                else
                {
                    Optimizer::gen_icode(Optimizer::i_assn, ap2, num, nullptr);
                }
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_lsr, ap3, ap2, Optimizer::make_immed(ISZ_UINT, N - l));
                ap4 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_add, ap4, ap3, num);
                ap5 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_asr, ap5, ap4, Optimizer::make_immed(ISZ_UINT, l));
                /*
                q = n >> (l-1);
                q = (unsigned)q >> (32 - l);
                q = (n + q) >> l ;
                */
            }
            else if (m < oneshl32 / 2)
            {
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode_with_conflict(Optimizer::i_assn, ap3, num, nullptr, true);
                ap1 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_mulsh, ap1, ap3, Optimizer::make_immed(ISZ_UINT, m));
                ap2 = Optimizer::tempreg(n, 0);
                if (post)
                    Optimizer::gen_icode(Optimizer::i_asr, ap2, ap1, Optimizer::make_immed(ISZ_UINT, post));
                else
                    Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap4 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_asr, ap4, num, Optimizer::make_immed(ISZ_UINT, N - 1));
                ap5 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_sub, ap5, ap2, ap4);
                /*
                q = ((LLONG)m) * n >> 32;
                q = q >> post;
                q = q - (n < 0 ? -1 : 0);
                */
            }
            else
            {
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode_with_conflict(Optimizer::i_assn, ap3, num, nullptr, true);
                ap1 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_mulsh, ap1, ap3, Optimizer::make_immed(ISZ_UINT, m - oneshl32));
                ap2 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_add, ap2, ap1, num);
                ap4 = Optimizer::tempreg(n, 0);
                if (post)
                    Optimizer::gen_icode(Optimizer::i_asr, ap4, ap2, Optimizer::make_immed(ISZ_UINT, post));
                else
                    Optimizer::gen_icode(Optimizer::i_assn, ap4, ap2, nullptr);
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_asr, ap3, num, Optimizer::make_immed(ISZ_UINT, N - 1));
                ap5 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_sub, ap5, ap4, ap3);
                /*
                q = ((LLONG)(m-oneshl32))*n>>32;
                q = (n + q) >> post;
                q = q - (n < 0 ? -1 : 0);
                */
            }
            if (d < 0)
            {
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_neg, ap3, ap5, nullptr);
                ap5 = ap3;
            }
            if (mod)
            {
                ap3 = Optimizer::tempreg(n, 0);
                Optimizer::gen_icode(Optimizer::i_mul, ap3, ap5, Optimizer::make_immed(ISZ_UINT, d));
                Optimizer::gen_icode(Optimizer::i_sub, ap, num, ap3);
            }
            else
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap, ap5, nullptr);
            }
            return ap;
        }
    }
    return gen_binary(funcsp, node, flags, size, op);
}
/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_pdiv(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *			generate code for an array/structure size compensation
 */
{
    int n = natural_size(node);
    if (!(Optimizer::chosenAssembler->arch->denyopts & DO_NOFASTDIV) && n == ISZ_ADDR && node->right->type == ExpressionNode::c_i_)
    {
        int d = node->right->v.i;
        if (d)
        {
            int dinv;
            int e = 0;
            Optimizer::IMODE *num = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT), *ap, *ap1;
            ap1 = Optimizer::LookupLoadTemp(nullptr, num);
            if (ap1 != num)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap1, num, nullptr);  // DAL fixed
                num = ap1;
            }
            while (!(d & 1))
                e++, d >>= 1;
            dinv = d;
            while (((dinv * d) & 0xffffffff) != 1)
            {
                long long dis = dinv, ds = d;
                dinv = dis * (2 - dis * ds);
            }
            ap = Optimizer::tempreg(n, 0);
            Optimizer::gen_icode(Optimizer::i_mul, ap, num, Optimizer::make_immed(ISZ_UINT, dinv));
            if (e)
                Optimizer::gen_icode(Optimizer::i_asr, ap, ap, Optimizer::make_immed(ISZ_UINT, e));
            return ap;
            //            return (dinv * n) >> e;
        }
    }
    return gen_binary(funcsp, node, flags, size, Optimizer::i_sdiv);
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_pmul(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *			generate code for an array/structure size compensation
 */
{
    Optimizer::IMODE *ap, *ap1, *ap2, *ap3;
    (void)flags;
    (void)size;
    ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
    ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
    ap3 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
    ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
    ap = LookupExpression(Optimizer::i_mul, ISZ_UINT, ap1, ap2);
    return ap;
}

/*-------------------------------------------------------------------------*/
static void DumpLogicalDestructors(SYMBOL* funcsp, std::list<EXPRESSION*>* node)
{
    for (auto node1 : *node)
    {
        gen_void_(node1, funcsp);
    }
}
Optimizer::IMODE* gen_hook(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *      generate code to evaluate a condition operator node (?:)
 */
{
    Optimizer::IMODE *ap1, *ap2, *ap3;
    int false_label, end_label;
    Optimizer::LIST* idl;
    false_label = Optimizer::nextLabel++;
    end_label = Optimizer::nextLabel++;
    flags = flags | F_VOL;
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
        flags &= ~F_NOVALUE;
    DumpIncDec(funcsp);
    falsejmp(node->left, funcsp, false_label);
    node = node->right;
    ap1 = Optimizer::tempreg(natural_size(node), 0);
    ap1->offset->sp->pushedtotemp = true;  // needed to make the global opts happy
    ap3 = gen_expr(funcsp, node->left, flags, size);
    ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
    Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, 0);
    Optimizer::intermed_tail->hook = true;
    DumpIncDec(funcsp);
    Optimizer::gen_igoto(Optimizer::i_goto, end_label);
    Optimizer::gen_label(false_label);
    ap3 = gen_expr(funcsp, node->right, flags, size);
    ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
    Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, 0);
    Optimizer::intermed_tail->hook = true;
    DumpIncDec(funcsp);
    Optimizer::gen_label(end_label);
    if (node->v.logicaldestructors.left)
        DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
    return ap1;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_moveblock(EXPRESSION* node, SYMBOL* funcsp)
/*
 * Generate code to copy one structure to another
 */
{
    Optimizer::IMODE *ap1, *ap2, *ap3, *ap6, *ap7, *ap8;
    if (!node->size)
        return (0);
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        int mode;
        if (node->left->type == ExpressionNode::msil_array_access_)
        {
            gen_expr(funcsp, node->left, F_STORE, ISZ_OBJECT);
            ap2 = gen_expr(funcsp, node->right, F_OBJECT, ISZ_OBJECT);
            Optimizer::gen_icode(Optimizer::i_parm, 0, ap2, 0);
            ap1 = Allocate<Optimizer::IMODE>();
            ap1->mode = Optimizer::i_immed;
            ap1->offset = Optimizer::SymbolManager::Get(node->left);
            ap1->size = ap2->size;
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, nullptr);
        }
        else
        {
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            mode = ap1->mode;
            ap7 = Optimizer::tempreg(ISZ_UINT, 0);
            *ap7 = *ap1;
            ap1 = ap7;
            ap1->mode = Optimizer::i_ind;
            if (isconstzero(&stdpointer, node->right))
            {
                ap3 = Optimizer::make_immed(ISZ_OBJECT, 0);  // LDNULL
            }
            else
                ap3 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
            ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap2 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, nullptr);
            Optimizer::intermed_tail->alttp = Optimizer::SymbolManager::Get((Type*)node->altdata);
            Optimizer::intermed_tail->blockassign = true;
            Optimizer::intermed_tail->oldmode = mode;
        }
    }
    else
    {
        bool candidate = node->size && node->size->IsStructured() && node->size->BaseType()->sp &&
                         node->size->BaseType()->sp->sb->structuredAliasType;
        if (candidate)
        {
            auto epx = node->right;
            if (epx->type == ExpressionNode::thisref_)
            {
                epx = epx->left;
            }
            candidate = epx->type != ExpressionNode::callsite_ || epx->v.sp->tp->BaseType()->btp->IsStructured();
        }
        if (candidate)
        {
            EXPRESSION* varl = node->left;
            EXPRESSION* varr = node->right;
            if (varr->type != ExpressionNode::callsite_ && varr->type != ExpressionNode::thisref_)
                Dereference(node->size->BaseType()->sp->sb->structuredAliasType, &varr);
            int size = SizeFromType(node->size->BaseType()->sp->sb->structuredAliasType);
            ap1 = gen_expr(funcsp, varl, 0, size);
            ap6 = indnode(ap1, SizeFromType(node->size->BaseType()->sp->sb->structuredAliasType));
            ap3 = gen_expr(funcsp, varr, F_VOL | F_RETURNSTRUCTBYVALUE, size);
            ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap2 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
            Optimizer::gen_icode(Optimizer::i_assn, ap6, ap2, nullptr);
        }
        else
        {
            ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
            ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            ap3 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
            ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap2 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
            ap6 = Optimizer::make_immed(ISZ_UINT, node->size->size);
            Optimizer::gen_icode(Optimizer::i_assnblock, ap6, ap1, ap2);
        }
    }
    return (ap1);
}
Optimizer::IMODE* gen_clearblock(EXPRESSION* node, SYMBOL* funcsp)
/*
 * Generate code to copy one structure to another
 */
{
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        // clrblock only generated for AUTO vars, should be safe to use initobj this way.
        Optimizer::IMODE* ap = gen_expr(funcsp, node->left, 0, ISZ_UINT);
        Optimizer::IMODE* ap1 = Optimizer::LookupLoadTemp(nullptr, ap);
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap, nullptr);
        Optimizer::intermed_tail->alwayslive = true;
        ap = Allocate<Optimizer::IMODE>();
        ap->mode = Optimizer::i_immed;
        ap->offset = Optimizer::SymbolManager::Get(node->left);
        ap->size = ISZ_UINT;
        Optimizer::gen_icode(Optimizer::i__initobj, nullptr, ap, nullptr);
        return ap1;
    }
    else if (node->size && node->size->IsStructured() && node->size->BaseType()->sp &&
             node->size->BaseType()->sp->sb->structuredAliasType)
    {
        Optimizer::IMODE* ap1;
        EXPRESSION* var = node->left;
        if (var->type != ExpressionNode::callsite_ && var->type != ExpressionNode::thisref_)
            Dereference(node->size->BaseType()->sp->sb->structuredAliasType, &var);
        int size = SizeFromType(node->size->BaseType()->sp->sb->structuredAliasType);
        ap1 = gen_expr(funcsp, var, F_STORE, size);
        Optimizer::gen_icode(Optimizer::i_assn, ap1, Optimizer::make_immed(ISZ_UINT, 0), nullptr);
        return (ap1);
    }
    else
    {
        Optimizer::IMODE *ap1, *ap3, *ap4, *ap5, *ap6, *ap7, *ap8;
        if (node->size)
        {
            ap4 = Optimizer::make_immed(ISZ_UINT, node->size->size);
        }
        else if (node->right)
        {
            ap5 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
            ap4 = Optimizer::LookupLoadTemp(nullptr, ap5);
            if (ap5 != ap4)
                Optimizer::gen_icode(Optimizer::i_assn, ap4, ap5, nullptr);
        }
        else
        {
            return (0);
        }
        ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
        ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
        if (ap1 != ap3)
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
        Optimizer::gen_icode(Optimizer::i_clrblock, nullptr, ap1, ap4);
        return (ap1);
    }
}
Optimizer::IMODE* gen_cpinitblock(EXPRESSION* node, SYMBOL* funcsp, bool cp, int flags)
/*
 * Generate code to copy one structure to another
 */
{
    Optimizer::IMODE *ap1, *ap3, *ap4, *ap5, *ap6, *ap7, *ap8;
    ap6 = gen_expr(funcsp, node->left->left, F_VOL, ISZ_UINT);
    ap7 = Optimizer::LookupLoadTemp(nullptr, ap6);
    if (ap7 != ap6)
        Optimizer::gen_icode(Optimizer::i_assn, ap7, ap6, nullptr);
    ap8 = Allocate<Optimizer::IMODE>();
    memcpy(ap8, ap7, sizeof(Optimizer::IMODE));
    ap8->mode = Optimizer::i_ind;
    ap3 = gen_expr(funcsp, node->left->right, F_VOL, ISZ_UINT);
    ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
    ap5 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
    ap4 = Optimizer::LookupLoadTemp(nullptr, ap5);
    if (ap5 != ap4)
        Optimizer::gen_icode(Optimizer::i_assn, ap4, ap5, nullptr);
    if (cp)
        Optimizer::gen_icode(Optimizer::i__cpblk, ap8, ap1, ap4);
    else
        Optimizer::gen_icode(Optimizer::i__initblk, ap8, ap1, ap4);
    Optimizer::intermed_tail->alwayslive = true;

    if (!(flags & F_NOVALUE))
    {

        ap6 = gen_expr(funcsp, node->left->left, F_VOL, ISZ_UINT);
        ap7 = Optimizer::LookupLoadTemp(nullptr, ap6);
        if (ap7 != ap6)
            Optimizer::gen_icode(Optimizer::i_assn, ap7, ap6, nullptr);
    }
    return (ap7);
}

Optimizer::IMODE* gen_cpinitobj(EXPRESSION* node, SYMBOL* funcsp, bool cp, int flags)
{
    Optimizer::IMODE* ap = gen_expr(funcsp, node->left, 0, ISZ_UINT);
    Optimizer::IMODE* ap1 = Optimizer::LookupLoadTemp(nullptr, ap);
    Optimizer::gen_icode(Optimizer::i_assn, ap1, ap, nullptr);
    Optimizer::intermed_tail->alwayslive = true;
    ap = Allocate<Optimizer::IMODE>();
    ap->mode = Optimizer::i_immed;
    ap->offset = Optimizer::SymbolManager::Get(node->left);
    ap->size = ISZ_UINT;
    Optimizer::gen_icode(Optimizer::i__initobj, nullptr, ap, nullptr);
    return nullptr;
}

Optimizer::IMODE* gen_cpsizeof(EXPRESSION* node, SYMBOL* funcsp, bool cp, int flags)
{
    Optimizer::IMODE* ap1 = Optimizer::tempreg(-ISZ_UINT, false);
    Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
    ap->mode = Optimizer::i_immed;
    ap->offset = Optimizer::SymbolManager::Get(node->left);
    ap->size = ISZ_UINT;
    Optimizer::gen_icode(Optimizer::i__sizeof, ap1, ap, nullptr);
    return ap1;
}
/*-------------------------------------------------------------------------*/
static void PushArrayLimits(SYMBOL* funcsp, Type* tp)
{
    if (!tp || !tp->IsArray())
        return;
    int n1 = tp->BaseType()->size;
    int n2 = tp->BaseType()->btp->BaseType()->size;
    if (n1 && n2)
    {
        Optimizer::IMODE* ap = Optimizer::tempreg(-ISZ_UINT, 0);
        Optimizer::gen_icode(Optimizer::i_assn, ap, Optimizer::make_immed(-ISZ_UINT, n1 / n2), nullptr);
        Optimizer::gen_icode(Optimizer::i_parm, 0, ap, 0);
    }
    else
    {
        Optimizer::IMODE* ap;
        if (tp->BaseType()->esize)
            ap = gen_expr(funcsp, tp->BaseType()->esize, 0, -ISZ_UINT);
        else
            ap = Optimizer::make_immed(-ISZ_UINT, 1);  // not really creating something, but give it a size so we can proceed.
        Optimizer::gen_icode(Optimizer::i_parm, 0, ap, 0);
    }
    PushArrayLimits(funcsp, tp->BaseType()->btp);
}

Optimizer::IMODE* gen_assign(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *      generate code for an assignment node. if the size of the
 *      assignment destination is larger than the size passed then
 *      everything below this node will be evaluated with the
 *      assignment size.
 */
{
    if (bitintbits(node->left))
        return bitint_assign(node);
    Optimizer::IMODE *ap1, *ap2, *ap3, *ap4;
    EXPRESSION *enode, *temp;
    Optimizer::LIST *l2, *lp;
    (void)flags;
    (void)size;
    if (node->right->type == ExpressionNode::msil_array_init_)
    {
        Type* base = node->right->v.tp;
        PushArrayLimits(funcsp, node->right->v.tp);
        while (base->IsArray())
            base = base->BaseType()->btp;
        ap1 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, base->IsStructured() ? ISZ_OBJECT : SizeFromType(base));
        ap2 = Allocate<Optimizer::IMODE>();
        ap2->mode = Optimizer::i_immed;
        ap2->offset = Optimizer::SymbolManager::Get(node->right);
        ap2->size = ap1->size;
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, nullptr);
    }
    else if (node->left->type == ExpressionNode::msil_array_access_)
    {
        Type* base = node->left->v.msilArray->tp;
        while (base->IsArray())
            base = base->BaseType()->btp;
        gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, SizeFromType(base));
        ap2 = gen_expr(funcsp, node->right, (flags & ~F_NOVALUE), SizeFromType(base));
        // Optimizer::gen_icode(Optimizer::i_parm, 0, ap2, 0);
        ap1 = Allocate<Optimizer::IMODE>();
        ap1->mode = Optimizer::i_immed;
        ap1->offset = Optimizer::SymbolManager::Get(node->left);
        ap1->size = ap2->size;
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, nullptr);
    }
    else
    {
        if (Optimizer::chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        {
            EXPRESSION* ex = node->right;
            int n = 0, m;
            ap1 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
            if (ap1->bits > 0 && (Optimizer::chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
            {
                n = ap1->startbit;
                m = ap1->bits;
                ap1->bits = ap1->startbit = 0;
                ap3 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
                ap4 = Optimizer::LookupLoadTemp(ap3, ap3);
                if (ap4 != ap3)
                {
                    ap4->bits = ap3->bits;
                    ap4->startbit = ap3->startbit;
                    ap3->bits = ap3->startbit = 0;
                    Optimizer::gen_icode(Optimizer::i_assn, ap4, ap3, nullptr);
                }
                ap3 = gen_bit_mask(ap4);
            }
            else
            {
                ap3 = nullptr;
            }
            while (IsCastValue(ex))
                ex = ex->left;
            if ((ap1->size == ISZ_OBJECT || ap1->size == ISZ_STRING) && isconstzero(&stdint, ex))
            {
                ap2 = Optimizer::make_immed(ISZ_OBJECT, 0);  // LDNULL
            }
            else
            {
                switch (node->right->type)
                {
                    case ExpressionNode::auto_:
                    case ExpressionNode::global_:
                    case ExpressionNode::pc_:
                    case ExpressionNode::threadlocal_:
                        if (node->right->v.sp->tp->IsStructured())
                        {
                            ap2 = gen_expr(funcsp, node->right, (flags & ~F_NOVALUE) | F_OBJECT, natural_size(node->left));
                            break;
                        }
                    default:
                        ap2 = gen_expr(funcsp, node->right, (flags & ~F_NOVALUE), natural_size(node->left));
                        break;
                }
            }
            ap4 = Optimizer::LookupLoadTemp(ap2, ap2);
            if (ap4 != ap2)
                Optimizer::gen_icode(Optimizer::i_assn, ap4, ap2, nullptr);
            if (ap3)
            {
                Optimizer::gen_icode(Optimizer::i_and, ap4, ap4, Optimizer::make_immed(-ISZ_UINT, (1 << m) - 1));
                if (n)
                    Optimizer::gen_icode(Optimizer::i_lsl, ap4, ap4, Optimizer::make_immed(-ISZ_UINT, n));
                Optimizer::gen_icode(Optimizer::i_or, ap4, ap3, ap4);
            }
        }
        else
        {
            auto n = node->right;
            while (IsCastValue(n))
                n = n->left;
            if (n->structByAddress)
                flags &= ~F_RETURNSTRUCTBYVALUE;
            ap2 = gen_expr(funcsp, node->right, flags & ~F_NOVALUE, natural_size(node->left));
            ap4 = Optimizer::LookupLoadTemp(ap2, ap2);
            if (ap4 != ap2)
                Optimizer::gen_icode(Optimizer::i_assn, ap4, ap2, nullptr);
            ap1 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
        }
        ap1->vol = ap2->vol;
        ap2->restricted = ap4->restricted;
        ap1->offset->pragmas = ap4->offset->pragmas;
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap4, nullptr);
    }
    if (node->left->isatomic)
    {
        Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr,
                             Optimizer::make_immed(ISZ_UINT, Optimizer::mo_seq_cst | 0x80), nullptr);
    }
    if (!(flags & F_NOVALUE) && (Optimizer::chosenAssembler->arch->preferopts & OPT_REVERSESTORE) && ap1->mode == Optimizer::i_ind)
    {
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), (flags & ~F_NOVALUE), natural_size(node->left));
    }
    if (node->v.logicaldestructors.left)
        DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
    ap1->vol = node->left->isvolatile;
    ap1->restricted = node->left->isrestrict;
    return ap1;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_aincdec(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op)
/*
 *      generate an auto increment or decrement node. op should be
 *      either op_add (for increment) or op_sub (for decrement).
 */
{
    Optimizer::IMODE *ap1 = nullptr, *ap2, *ap3 = nullptr, *ap4, *ap5, *ap6, *ap7 = nullptr;
    int siz1;
    EXPRESSION* ncnode;
    Optimizer::LIST* l;
    int n, m;
    (void)size;
    siz1 = natural_size(node->left);
    if (flags & F_NOVALUE)
    {
        ncnode = node->left;
        while (IsCastValue(ncnode))
            ncnode = ncnode->left;
        ap6 = gen_expr(funcsp, ncnode, F_STORE, siz1);
        if (ap6->bits > 0 && (Optimizer::chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
        {
            n = ap6->startbit;
            m = ap6->bits;
            ap6->bits = ap6->startbit = 0;
            ap7 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
            ap4 = Optimizer::LookupLoadTemp(ap7, ap7);
            if (ap4 != ap7)
            {
                ap4->bits = ap7->bits;
                ap4->startbit = ap7->startbit;
                ap7->bits = ap7->startbit = 0;
                Optimizer::gen_icode(Optimizer::i_assn, ap4, ap7, nullptr);
            }
            ap7 = gen_bit_mask(ap4);
        }
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = Optimizer::LookupLoadTemp(ap1, ap1);
        if (ap5 != ap1)
            Optimizer::gen_icode(Optimizer::i_assn, ap5, ap1, nullptr);
        ap2 = gen_expr(funcsp, node->right, 0, siz1 == ISZ_ADDR ? ISZ_UINT : siz1);
        ap2 = LookupExpression(op, siz1, ap5, ap2);
        if (ap7)
        {
            Optimizer::gen_icode(Optimizer::i_and, ap2, ap2, Optimizer::make_immed(-ISZ_UINT, (1 << m) - 1));
            if (n)
                Optimizer::gen_icode(Optimizer::i_lsl, ap2, ap2, Optimizer::make_immed(-ISZ_UINT, n));
            Optimizer::gen_icode(Optimizer::i_or, ap2, ap7, ap2);
        }
        ap4 = Optimizer::LookupStoreTemp(ap6, ap6);
        if (ap4 != ap6)
        {
            Optimizer::gen_icode(Optimizer::i_assn, ap4, ap2, nullptr);
            Optimizer::gen_icode(Optimizer::i_assn, ap6, ap4, nullptr);
        }
        else
            Optimizer::gen_icode(Optimizer::i_assn, ap6, ap2, nullptr);
        return nullptr;
    }
    else if (flags & F_COMPARE)
    {
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = Optimizer::LookupLoadTemp(ap1, ap1);
        if (ap1 != ap5)
            Optimizer::gen_icode(Optimizer::i_assn, ap5, ap1, nullptr);
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            ap3 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
            ap5 = Optimizer::LookupLoadTemp(ap3, ap3);
            if (ap5 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap5, ap3, nullptr);
            ap3 = ap1;
        }
        ncnode = node->left;
        while (IsCastValue(ncnode))
            ncnode = ncnode->left;
        ap6 = gen_expr(funcsp, ncnode, F_STORE, siz1);
        if (ap6->bits > 0 && (Optimizer::chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
        {
            n = ap6->startbit;
            m = ap6->bits;
            ap6->bits = ap6->startbit = 0;
            ap7 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
            ap4 = Optimizer::LookupLoadTemp(ap7, ap7);
            if (ap4 != ap7)
            {
                ap4->bits = ap7->bits;
                ap4->startbit = ap7->startbit;
                ap7->bits = ap7->startbit = 0;
                Optimizer::gen_icode(Optimizer::i_assn, ap4, ap7, nullptr);
            }
            ap7 = gen_bit_mask(ap4);
        }
        if (!(Optimizer::architecture == ARCHITECTURE_MSIL))
        {
            ap3 = Optimizer::tempreg(siz1, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap5, nullptr);
            Optimizer::intermed_tail->needsOCP = true;
        }
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = Optimizer::LookupLoadTemp(ap1, ap1);
        if (ap5 != ap1)
            Optimizer::gen_icode(Optimizer::i_assn, ap5, ap1, nullptr);
        ap2 = gen_expr(funcsp, node->right, 0, siz1 == ISZ_ADDR ? ISZ_UINT : siz1);
        ap2 = LookupExpression(op, siz1, ap5, ap2);
        if (ap7)
        {
            Optimizer::gen_icode(Optimizer::i_and, ap2, ap2, Optimizer::make_immed(-ISZ_UINT, (1 << m) - 1));
            if (n)
                Optimizer::gen_icode(Optimizer::i_lsl, ap2, ap2, Optimizer::make_immed(-ISZ_UINT, n));
            Optimizer::gen_icode(Optimizer::i_or, ap2, ap7, ap2);
        }
        ap4 = Optimizer::LookupStoreTemp(ap6, ap6);
        if (ap4 != ap6)
        {
            Optimizer::gen_icode(Optimizer::i_assn, ap4, ap2, nullptr);
            Optimizer::gen_icode(Optimizer::i_assn, ap6, ap4, nullptr);
        }
        else
            Optimizer::gen_icode(Optimizer::i_assn, ap6, ap2, nullptr);
        return ap3;
    }
    else
    {
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = Optimizer::LookupLoadTemp(ap1, ap1);
        if (ap5 != ap1)
            Optimizer::gen_icode(Optimizer::i_assn, ap5, ap1, nullptr);
        l = Allocate<Optimizer::LIST>();
        l->data = (void*)node;
        if (!incdecList)
            incdecList = incdecListLast = l;
        else
            incdecListLast = incdecListLast->next = l;
        return ap5;
    }
}
/*-------------------------------------------------------------------------*/
static EXPRESSION* getAddress(EXPRESSION* exp)
{
    EXPRESSION* rv = nullptr;
    if (exp->type == ExpressionNode::add_)
    {
        rv = getAddress(exp->left);
        if (!rv)
            rv = getAddress(exp->right);
    }
    else if (exp->type == ExpressionNode::auto_)
    {
        return exp;
    }
    return rv;
}
static EXPRESSION* aliasToTemp(SYMBOL* funcsp, EXPRESSION* in)
{
    auto expt = in;
    if (expt->type == ExpressionNode::thisref_)
        expt = expt->left;
    if (expt->type == ExpressionNode::callsite_)
    {
        if (!expt->v.func->sp->sb->isConstructor)
        {
            if (expt->v.func->sp->tp->BaseType()->btp->IsStructured())
            {
                auto srp = expt->v.func->sp->tp->BaseType()->btp->BaseType();
                if (srp && srp->sp->sb->structuredAliasType)
                {
                    auto expx = tempVar(srp);
                    expx = MakeExpression(ExpressionNode::assign_, expx, in);
                    in = MakeExpression(ExpressionNode::comma_, expx, expx->left);
                }
            }
        }
    }
    return in;
}
/*-------------------------------------------------------------------------*/
int push_param(EXPRESSION* ep, SYMBOL* funcsp, EXPRESSION* valist, Type* argtp, int flags)
/*
 *      push the operand expression onto the stack.
 */
{
    if (bitintbits(ep))
        return bitint_push(ep);
    Optimizer::IMODE *ap, *ap3;
    int temp;
    int rv = 0;
    EXPRESSION* exp = getFunc(ep);
    bool complexblock = false;
    if (!exp && ep->type == ExpressionNode::comma_)
    {
        exp = ep->left;
        if (exp && exp->type != ExpressionNode::blockassign_ && exp->type != ExpressionNode::blockclear_)
        {
            if (exp->type != ExpressionNode::assign_ || exp->right->type != ExpressionNode::auto_ ||
                !exp->right->v.sp->sb->stackblock)
            {
                exp = nullptr;
            }
            else
            {
                complexblock = true;
                exp = exp->right;
            }
        }
    }
    if (exp)
    {
        EXPRESSION* ep1 = exp;
        if (exp->type == ExpressionNode::callsite_)
        {

            exp = ep1->v.func->returnEXP;
            if (!exp)
                exp = ep1->v.func->thisptr;
            if (exp)
                exp = getAddress(exp);
        }
        else if (ep->type == ExpressionNode::auto_)
        {
            exp = ep;
        }
        else if (!complexblock)
        {
            exp = exp->left;
        }
        if (exp && exp->type == ExpressionNode::auto_ && exp->v.sp->sb->stackblock)
        {
            // constructor or other function creating a structure on the stack
            rv = exp->v.sp->tp->size;
            if (rv % Optimizer::chosenAssembler->arch->stackalign)
                rv = rv + Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
            Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0), Optimizer::make_immed(ISZ_UINT, rv),
                                 nullptr);
            Optimizer::SymbolManager::Get(exp->v.sp)->imvalue = ap;
            gen_expr(funcsp, ep, flags, ISZ_UINT);
        }
        else
        {
            temp = natural_size(ep);
            ap3 = gen_expr(funcsp, ep, flags, temp);
            ap = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
            if (ap->size == ISZ_NONE)
                ap->size = temp;
            Optimizer::gen_nodag(Optimizer::i_parm, 0, ap, 0);
            Optimizer::intermed_tail->valist = valist && valist->type == ExpressionNode::l_p_;
            rv = Optimizer::sizeFromISZ(ap->size);
        }
    }
    else
    {
        EXPRESSION* exp1 = ep;
        switch (ep->type)
        {
            case ExpressionNode::comma_:
                while (ep->type == ExpressionNode::comma_)
                {
                    gen_expr(funcsp, ep->left, flags | F_RETURNSTRUCTNOADJUST, ISZ_UINT);
                    ep = ep->right;
                }
                // the next handles structures that have been used with constexpr...
                if (exp1->left->type != ExpressionNode::assign_ || exp1->left->right->type != ExpressionNode::substack_)
                {
                    push_param(ep, funcsp, valist, argtp, flags);
                }
                break;
            case ExpressionNode::argnopush_:
                gen_expr(funcsp, ep->left, flags, ISZ_UINT);
                break;
            case ExpressionNode::imode_:
                ap = (Optimizer::IMODE*)ep->left;
                Optimizer::gen_nodag(Optimizer::i_parm, 0, ap, 0);
                Optimizer::intermed_tail->valist = valist && valist->type == ExpressionNode::l_p_;
                rv = Optimizer::sizeFromISZ(ap->size);
                break;
            default:
                while (IsCastValue(exp1))
                    exp1 = exp1->left;
                temp = natural_size(ep);
                if (temp == ISZ_OBJECT && isconstzero(&stdint, exp1))
                {
                    ap3 = Optimizer::make_immed(ISZ_OBJECT, 0);  // LDNULL
                }
                else
                {
                    if (ep->type == ExpressionNode::structadd_ && isconstzero(&stdint, ep->right))
                    {
                        ep = ep->left;
                    }
                    ap3 = gen_expr(funcsp, ep, flags, temp);
                }
                if (ap3->bits > 0)
                    ap3 = gen_bit_load(ap3);
                ap = Optimizer::LookupLoadTemp(nullptr, ap3);
                if (ap != ap3)
                    Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
                if (ap->size == ISZ_NONE)
                    ap->size = temp;

                Optimizer::gen_nodag(Optimizer::i_parm, 0, ap, 0);
                Optimizer::intermed_tail->valist = valist && valist->type == ExpressionNode::l_p_;
                rv = Optimizer::sizeFromISZ(ap->size);
                break;
        }
        if (rv % Optimizer::chosenAssembler->arch->stackalign)
            rv = rv + Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

static int push_stackblock(Type* tp, EXPRESSION* ep, SYMBOL* funcsp, int sz, EXPRESSION* valist)
/*
 * Push a structure on the stack
 */
{
    Optimizer::IMODE *ap, *ap1, *ap2, *ap3;
    if (!sz)
        return 0;
    if (sz % Optimizer::chosenAssembler->arch->stackalign)
        sz = sz + Optimizer::chosenAssembler->arch->stackalign - sz % Optimizer::chosenAssembler->arch->stackalign;
    bool allocated = false;
    switch (ep->type)
    {
        case ExpressionNode::imode_:
            ap = ep->v.imode;
            break;
        default:
            if (ep->type == ExpressionNode::comma_ && ep->left->type == ExpressionNode::blockclear_)
            {
                int offset;
                auto exp = relptr(ep->left->left, offset);
                if (exp && exp->type == ExpressionNode::auto_ && exp->v.sp->sb->stackblock)
                {
                    // if we get here we have a parmstack without a constructor
                    // or a parmstack not used as part of an argument
                    int rv = exp->v.sp->tp->size;
                    if (rv % Optimizer::chosenAssembler->arch->stackalign)
                        rv = rv + Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
                    Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0),
                                         Optimizer::make_immed(ISZ_UINT, rv), nullptr);
                    Optimizer::SymbolManager::Get(exp->v.sp)->imvalue = ap;
                    allocated = true;
                }
            }
            if (ep->type == ExpressionNode::c_i_)
            {
                // trivial structure
                allocated = true;
                Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0),
                                     Optimizer::make_immed(ISZ_UINT, sz), nullptr);
                Optimizer::gen_icode(Optimizer::i_clrblock, nullptr, ap, Optimizer::make_immed(ISZ_UINT, sz));
            }
            else
            {
                // if it would be a parmblock copied from a parmblock mark it as allocated
                if (ep->type == ExpressionNode::thisref_ && ep->left->v.func->thisptr->type == ExpressionNode::auto_ &&
                    ep->left->v.func->thisptr->v.sp->sb->stackblock)
                {
                    allocated = true;
                }
                ap3 = gen_expr(funcsp, ep, F_ADDR, ISZ_UINT);
                if (ap3->mode != Optimizer::i_direct && (Optimizer::chosenAssembler->arch->preferopts & OPT_ARGSTRUCTREF))
                {
                    ap = oAllocate<Optimizer::IMODE>();
                    *ap = *ap3;
                    ap3 = ap;
                    ap3->mode = Optimizer::i_direct;
                }
                ap = Optimizer::LookupLoadTemp(nullptr, ap3);
                if (ap != ap3)
                    Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
            }
            break;
    }
    if (!allocated)
    {
        // nontrivial structure
        Optimizer::gen_nodag(Optimizer::i_parmblock, 0, ap, Optimizer::make_immed(ISZ_UINT, sz));
        Optimizer::intermed_tail->alttp = Optimizer::SymbolManager::Get(tp);
        Optimizer::intermed_tail->valist = valist && valist->type == ExpressionNode::l_p_;
    }
    return sz;
}

/*-------------------------------------------------------------------------*/

static int gen_parm(Argument* a, SYMBOL* funcsp)
/*
 *      push a list of parameters onto the stack and return the
 *      size of parameters pushed.
 */
{
    int rv;
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        // varargs set up initially
        if (a->vararg)
            return 0;
    }

    if (a->tp->IsPtr() || a->tp->IsRef())
    {
        Type* btp = a->tp->BaseType();
        if (btp->vla || btp->btp->BaseType()->vla)
        {
            rv = push_stackblock(a->tp, a->exp->left, funcsp, a->tp->size, a->valist ? a->exp : nullptr);
            DumpIncDec(funcsp);
            push_nesting += rv;
            return rv;
        }
    }
    if ((!Optimizer::cparams.prm_cplusplus && a->tp->IsStructured()) ||
        ((Optimizer::architecture == ARCHITECTURE_MSIL) && a->tp->IsArray() && a->tp->BaseType()->msil))
    {
        if (a->exp->type != ExpressionNode::stackblock_ && (Optimizer::architecture == ARCHITECTURE_MSIL))
        {
            EXPRESSION* exp1 = a->exp;
            while (IsCastValue(exp1))
                exp1 = exp1->left;

            if (isconstzero(&stdint, exp1))
            {
                Optimizer::IMODE *ap4 = Optimizer::tempreg(-ISZ_UINT, 0), *ap3 = gen_expr(funcsp, exp1, 0, -ISZ_UINT);
                ap3->size = ISZ_OBJECT;  // only time we will set the OBJECT size for an int constant is if it is to be an LDNULL
                Optimizer::gen_icode(Optimizer::i_assn, ap4, ap3, nullptr);
                Optimizer::gen_nodag(Optimizer::i_parm, 0, ap3, 0);
                rv = 1;
            }
            else
            {
                rv = push_param(a->exp, funcsp, a->valist ? a->exp : nullptr, a->tp, F_OBJECT);
            }
        }
        else if (a->tp->BaseType()->sp->sb->structuredAliasType)
        {
            EXPRESSION* val = a->exp->left;
            if (val->type != ExpressionNode::callsite_ && val->type != ExpressionNode::thisref_)
                Dereference(a->tp->BaseType()->sp->sb->structuredAliasType, &val);
            rv = push_param(val, funcsp, nullptr, a->tp, 0);
        }
        else
        {
            rv = push_stackblock(a->tp, a->exp->left, funcsp, a->exp->size->size, a->valist ? a->exp : nullptr);
        }
    }
    else if (a->exp->type == ExpressionNode::stackblock_)
    {
        if (a->tp->type != BasicType::memberptr_ && a->tp->BaseType()->sp->sb->structuredAliasType)
        {
            EXPRESSION *val = a->exp->left, *val2 = val;
            if (val2->type == ExpressionNode::comma_)
                val2 = val2->right;
            if (val2->type != ExpressionNode::callsite_ && val2->type != ExpressionNode::thisref_ && !isarithmeticconst(val2))
            {
                Dereference(a->tp->BaseType()->sp->sb->structuredAliasType, &val);
            }
            rv = push_param(val, funcsp, nullptr, a->tp, 0);
        }
        else
        {
            rv = push_stackblock(a->tp, a->exp->left, funcsp, a->tp->BaseType()->size, a->valist ? a->exp : nullptr);
        }
    }
    else if (a->tp->IsStructured() && a->exp->type == ExpressionNode::thisref_)  // constructor
    {
        EXPRESSION* ths = a->exp->v.t.thisptr;
        if (ths && ths->type == ExpressionNode::auto_ && ths->v.sp->sb->stackblock)
        {
            Optimizer::IMODE* ap;
            // constructor or other function creating a structure on the stack
            ths = a->exp->left->v.func->thisptr;
            ths->v.sp->sb->stackblock = true;
            rv = ths->v.sp->tp->size;
            if (rv % Optimizer::chosenAssembler->arch->stackalign)
                rv = rv + Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
            Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0), Optimizer::make_immed(ISZ_UINT, rv),
                                 nullptr);
            Optimizer::SymbolManager::Get(ths->v.sp)->imvalue = ap;
            gen_expr(funcsp, a->exp, 0, ISZ_UINT);
        }
        else
        {
            // could be a cast operator
            auto ret = a->exp->left->type == ExpressionNode::callsite_ ? a->exp->left->v.func->returnEXP : nullptr;
            Optimizer::IMODE* ap = gen_expr(funcsp, a->exp, 0, ISZ_ADDR);
            if (!ret || ret->type != ExpressionNode::auto_ || !ret->v.sp->sb->stackblock)
            {
                Optimizer::gen_nodag(Optimizer::i_parm, 0, ap, 0);
                Optimizer::intermed_tail->valist = a->valist ? a->exp->type == ExpressionNode::l_p_ : 0;
            }
            rv = a->tp->size;
        }
    }
    else
    {
        int flags = 0;
        if (a->tp->IsStructured() && a->tp->BaseType()->sp->sb->structuredAliasType)
            flags = F_RETURNSTRUCTBYVALUE;
        rv = push_param(a->exp, funcsp, a->valist ? a->exp : nullptr, a->tp, flags);
    }
    DumpIncDec(funcsp);
    push_nesting += rv;
    return rv;
}
static int sizeParam(Argument* a, SYMBOL* funcsp)
{
    int rv;
    if (a->tp->IsBitInt())
    {
        rv = a->tp->BaseType()->bitintbits + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
        rv /= Optimizer::chosenAssembler->arch->bitintunderlying;
        rv *= Optimizer::chosenAssembler->arch->bitintunderlying;
        rv /= CHAR_BIT;
    }
    else if (a->tp->IsPtr() || a->tp->IsRef() || a->tp->type == BasicType::func_ || a->tp->type == BasicType::ifunc_ || a->byRef)
        rv = Optimizer::sizeFromISZ(ISZ_ADDR);
    else
        rv = a->tp->BaseType()->size;
    if (rv % Optimizer::chosenAssembler->arch->stackalign)
        rv += Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
    return rv;
}
static int genCdeclArgs(std::list<Argument*>* args, SYMBOL* funcsp)
{
    int rv = 0;
    if (args)
    {
        for (auto it = args->end(); it != args->begin();)
        {
            --it;
            int n = gen_parm((*it), funcsp);
            rv += n;
            stackblockOfs -= n;
            if ((*it)->exp->type == ExpressionNode::auto_)
                if ((*it)->exp->v.sp->sb->stackblock)
                    (*it)->exp->v.sp->sb->offset = stackblockOfs;
        }
    }
    return rv;
}
static void genCallLab(std::list<Argument*>* args, int callLab)
{
    if (args)
    {
        for (auto it = args->end(); it != args->begin();)
        {
            --it;
            if ((*it)->exp->type == ExpressionNode::thisref_)
            {
                (*it)->exp->v.t.thisptr->xcDest = callLab;
            }
        }
    }
}
static int genPascalArgs(std::list<Argument*>* args, SYMBOL* funcsp)
{
    int rv = 0;
    if (args)
    {
        for (auto it = args->begin(); it != args->end(); ++it)
            rv += gen_parm((*it), funcsp);
    }
    return rv;
}
static int sizeParams(std::list<Argument*>* args, SYMBOL* funcsp)
{
    int rv = 0;
    if (args)
    {
        for (auto it = args->end(); it != args->begin();)
        {
            --it;
            rv += sizeParam(*it, funcsp);
        }
    }
    return rv;
}

Optimizer::IMODE* gen_trapcall(SYMBOL* funcsp, EXPRESSION* node, int flags)
{
    /* trap call */
    (void)funcsp;
    (void)flags;
    Optimizer::gen_igosub(Optimizer::i_trap, Optimizer::make_immed(ISZ_UINT, node->v.i));
    return 0;
}
Optimizer::IMODE* gen_stmt_from_expr(SYMBOL* funcsp, EXPRESSION* node, int flags)
{
    Optimizer::IMODE* rv;
    (void)flags;
    /* relies on stmt only being used for inlines */
    rv = genstmt(node->v.stmt, funcsp, 0);
    if (node->left)
        rv = gen_expr(funcsp, node->left, 0, natural_size(node->left));
    return rv;
}
/*-------------------------------------------------------------------------*/

static bool has_arg_destructors(std::list<Argument*>* arg)
{
    if (arg)
        for (auto t : *arg)
            if (!!t->destructors)
                return true;
    return false;
}
static void gen_arg_destructors(SYMBOL* funcsp, std::list<Argument*>* arg, std::list<EXPRESSION*>* assignDestructors)
{
    if (arg)
    {
        for (auto it = arg->end(); it != arg->begin();)
        {
            --it;
            if ((*it)->destructors)
                for (auto e : *(*it)->destructors)
                    gen_expr(funcsp, e, F_NOVALUE, ISZ_UINT);
        }
    }
    if (assignDestructors)
        for (auto e : *assignDestructors)
            gen_expr(funcsp, e, F_NOVALUE, ISZ_UINT);
}
static int MarkFastcall(SYMBOL* sym, Type* functp, bool thisptr)
{
#ifndef CPPTHISCALL
    if (sym->sb->attribs.inheritable.linkage != Linkage::fastcall_)
        return 0;
#endif

    if (!Optimizer::dontOptimizeFunction)
    {
        /* if there is a setjmp in the function, no variable gets moved into a reg */
        if (!(Optimizer::setjmp_used))
        {

            Optimizer::QUAD* tail = Optimizer::intermed_tail;
            int i = 0;
            if (functp->IsFunction())
            {
                if (functp->BaseType()->sp)
                    sym = functp->BaseType()->sp;
                if (!ismember(sym) && sym->sb->attribs.inheritable.linkage != Linkage::fastcall_ &&
                    sym->tp->BaseType()->type != BasicType::memberptr_)
                    return 0;
            }
            else
            {
                return 0;
            }

            int structret = !!sym->tp->BaseType()->btp->IsStructured();

            auto it = functp->BaseType()->syms->begin();
            auto itend = functp->BaseType()->syms->end();
            while (it != itend && tail && tail->dc.opcode != Optimizer::i_block)
            {

                if (tail->dc.opcode == Optimizer::i_parm)
                {
                    // change it to a move
                    SYMBOL* sp = *it;
                    Type* tp = sp->tp->BaseType();
                    if (thisptr ||
                        ((tp->type < BasicType::float_ ||
                          (tp->type == BasicType::pointer_ && tp->BaseType()->btp->BaseType()->type != BasicType::func_) ||
                          tp->IsRef()) &&
                         sp->sb->offset - (Optimizer::chosenAssembler->arch->fastcallRegCount + structret) *
                                              Optimizer::chosenAssembler->arch->parmwidth <
                             Optimizer::chosenAssembler->arch->retblocksize))
                    {
                        Optimizer::IMODE* temp = Optimizer::tempreg(tail->dc.left->size, 0);
                        Optimizer::QUAD* q = Allocate<Optimizer::QUAD>();
                        *q = *tail;
                        q->dc.opcode = Optimizer::i_assn;
                        q->ans = temp;
                        tail->dc.left = temp;
                        tail->fastcall = ++i;
                        if (tail->dc.left->size == ISZ_ULONGLONG || tail->dc.left->size == -ISZ_ULONGLONG)
                            ++i;  // long long is presently meant to be the one and only arg...
                        q->back = tail->back;
                        q->fwd = tail;
                        q->back->fwd = q;
                        q->fwd->back = q;
                        //                        q->genConflict = true;
                    }
                    else
                    {
                        break;
                    }
                    if (sym->sb->attribs.inheritable.linkage != Linkage::fastcall_)
                        break;
                    if (thisptr)
                    {
                        if (sp->sb->thisPtr)
                            ++it;
                        thisptr = false;
                    }
                    else
                    {
                        ++it;
                    }
                }
                else if (tail->dc.opcode == Optimizer::i_gosub)
                {
                    break;
                }
                tail = tail->back;
            }
            return i * Optimizer::chosenAssembler->arch->parmwidth;
        }
    }
    return 0;
}
Optimizer::SimpleExpression* CreateMsilVarargs(SYMBOL* funcsp, CallSite* f)
{
    Optimizer::SimpleExpression* rv = nullptr;

    if (f->vararg)
    {
        int count = 0;
        if (f->arguments)
            for (auto a : *f->arguments)
                if (a->vararg)
                    count++;
        if (count)
        {
            auto tp = Type::MakeType(BasicType::pointer_, Type::MakeType(BasicType::object_));
            tp->size = 0;
            tp->array = tp->msil = true;
            rv = Optimizer::SymbolManager::Get(AnonymousVar(StorageClass::auto_, tp));
            rv->sp->anonymous = false;
            rv->sp->msilObjectArray = true;

            // this is for the initialization of the object array
            Optimizer::QUAD* q2 = Allocate<Optimizer::QUAD>();
            Optimizer::QUAD* q3 = Allocate<Optimizer::QUAD>();
            Optimizer::IMODE* ap4 = Optimizer::tempreg(ISZ_ADDR, 0);
            Optimizer::IMODE* ap5;
            Optimizer::IMODE* ap6 = Allocate<Optimizer::IMODE>();
            ap6->offset = rv;
            ap6->mode = Optimizer::i_direct;
            ap6->size = ISZ_ADDR;
            ap5 = Optimizer::make_immed(-ISZ_UINT, count);
            Optimizer::gen_icode(Optimizer::i_assn, ap4, ap5, nullptr);
            Optimizer::gen_icode(Optimizer::i_assn, ap6, ap4, nullptr);

            int parmIndex = 0;
            for (auto a : *f->arguments)
            {
                if (a->vararg)
                {
                    Optimizer::IMODE* ap = Optimizer::tempreg(ISZ_ADDR, 0);
                    Optimizer::IMODE* ap2 = Optimizer::tempreg(-ISZ_UINT, 0);
                    Optimizer::IMODE* ap3 = Optimizer::make_immed(-ISZ_UINT, parmIndex++);
                    Optimizer::IMODE* ap8;
                    Optimizer::gen_icode(Optimizer::i_assn, ap, ap6, nullptr);
                    Optimizer::intermed_tail->alwayslive = true;
                    Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
                    Optimizer::intermed_tail->alwayslive = true;
                    Optimizer::IMODE* ap1 = gen_expr(funcsp, a->exp, 0, natural_size(a->exp));
                    ap8 = Optimizer::LookupLoadTemp(nullptr, ap1);
                    if (ap8 != ap1)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap8, ap1, nullptr);
                        ap1 = ap8;
                    }
                    Optimizer::gen_icode(Optimizer::i_parm, nullptr, ap1, nullptr);
                    Optimizer::intermed_tail->vararg = true;
                }
            }
        }
    }
    return rv;
}
Optimizer::IMODE* gen_funccall(SYMBOL* funcsp, EXPRESSION* node, int flags)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    int fastcallSize = 0;
    Optimizer::IMODE* ap3 = nullptr;
    CallSite* f = node->v.func;
    bool managed = false;
    Optimizer::IMODE* stobj = nullptr;
    Optimizer::IMODE* ap = nullptr;
    int adjust = 0;
    int adjust2 = 0;
    Optimizer::QUAD* gosub = nullptr;
    Optimizer::SimpleExpression* varargarray = nullptr;
    if (!f->sp || !f->fcall)
        return Optimizer::make_immed(ISZ_UINT, 0);
    if (!f->ascall)
    {
        InsertInline(f->sp);
        return gen_expr(funcsp, f->fcall, 0, ISZ_ADDR);
    }
    if (f->rttiType)
        InsertRttiType(f->rttiType);
    if (f->rttiType2)
        InsertRttiType(f->rttiType2);
    if (f->sp->sb->attribs.inheritable.isInline || f->sp->sb->attribs.inheritable.excludeFromExplicitInstantiation)
    {
        if (CompileInline(f->sp, false) && !f->sp->sb->noinline)
        {
            ap = gen_inline(funcsp, node, flags);
            if (ap)
            {
                if (has_arg_destructors(f->arguments))
                {
                    Optimizer::IMODE* ap1 = Optimizer::tempreg(ap->size, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, ap1, ap, nullptr);
                    ap->wasinlined = ap1->wasinlined;
                    ap = ap1;
                }
                gen_arg_destructors(funcsp, f->arguments, f->destructors);
                return ap;
            }
        }
    }
    InsertInline(f->sp);
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) &&
        (f->sp->sb->attribs.inheritable.linkage2 != Linkage::unmanaged_ && msilManaged(f->sp)))
        managed = true;
    if (f->returnEXP && managed && (f->functp->BaseType()->btp->IsStructured() || f->functp->BaseType()->btp->IsBitInt()))
    {
        switch (f->returnEXP->type)
        {
            case ExpressionNode::auto_:
            case ExpressionNode::pc_:
            case ExpressionNode::global_:
            case ExpressionNode::threadlocal_:
                break;
            default:
                stobj = gen_expr(funcsp, f->returnEXP, 0, ISZ_UINT);
                break;
        }
    }
    {
        int n = sizeParams(f->arguments, funcsp);
        int v = Optimizer::sizeFromISZ(ISZ_ADDR);
        if (v % Optimizer::chosenAssembler->arch->stackalign)
            v = v + Optimizer::chosenAssembler->arch->stackalign - v % Optimizer::chosenAssembler->arch->stackalign;
        if (f->functp->IsFunction() && (f->functp->BaseType()->btp->IsStructured() || f->functp->BaseType()->btp->IsBitInt() ||
                                        f->functp->BaseType()->btp->BaseType()->type == BasicType::memberptr_))
        {
            if (f->returnEXP)
                n += v;
        }
        if (f->thisptr)
        {
            n += v;
        }
        adjust += n;
    }
    if (Optimizer::cparams.prm_stackalign)
    {
        int k = push_nesting + adjust;
        if (k % Optimizer::cparams.prm_stackalign)
        {
            int n = Optimizer::cparams.prm_stackalign - k % Optimizer::cparams.prm_stackalign;
            adjust2 = n;
            adjust += n;
            push_nesting += n;
            // make an instruction for adjustment.
            Optimizer::gen_icode(Optimizer::i_substack, nullptr, Optimizer::make_immed(ISZ_UINT, n), nullptr);
        }
    }

    int cdeclare = stackblockOfs;
    if (Optimizer::delegateforfuncptr)
    {
        Optimizer::IMODE* ap10;

        ap = ap3 = gen_expr(funcsp, f->fcall, 0, ISZ_UINT);
        if (ap->mode != Optimizer::i_immed)
        {
            Optimizer::gen_icode(Optimizer::i_assn, ap10 = Optimizer::tempreg(-ISZ_UINT, 0),
                                 Optimizer::make_immed(-ISZ_UINT, adjust), nullptr);
            Optimizer::gen_icode(Optimizer::i_parm, nullptr, ap10, nullptr);
        }
    }
    if (f->sp->sb->attribs.inheritable.linkage == Linkage::pascal_)
    {
        if (f->functp->BaseType()->btp->IsStructured() || f->functp->BaseType()->btp->IsBitInt() ||
            f->functp->BaseType()->btp->BaseType()->type == BasicType::memberptr_)
        {
            if (f->returnEXP)
                push_param(f->returnEXP, funcsp, nullptr, f->returnSP->tp, F_OBJECT);
        }
        genPascalArgs(f->arguments, funcsp);
    }
    else
    {
        int n = 0;
        if (f->thisptr && f->thisptr->type == ExpressionNode::auto_ && f->thisptr->v.sp->sb->stackblock && !f->sp->sb->isDestructor)
        {
            EXPRESSION* exp = f->thisptr;
            // constructor or other function creating a structure on the stack
            int rv = exp->v.sp->tp->size;
            if (rv % Optimizer::chosenAssembler->arch->stackalign)
                rv = rv + Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
            Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0), Optimizer::make_immed(ISZ_UINT, rv),
                                 nullptr);
            Optimizer::SimpleSymbol* sym = Optimizer::SymbolManager::Get(exp->v.sp);
            sym->imvalue = ap;
            sym->offset = -rv + stackblockOfs;
            genCdeclArgs(f->arguments, funcsp);
            ap3 = gen_expr(funcsp, exp, 0, ISZ_UINT);
            ap = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
            if (ap->size == ISZ_NONE)
                ap->size = ISZ_ADDR;
            Optimizer::gen_nodag(Optimizer::i_parm, 0, ap, 0);
        }
        else if (Optimizer::chosenAssembler->arch->preferopts & OPT_REVERSEPARAM)
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
                varargarray = CreateMsilVarargs(funcsp, f);
            if (f->functp->BaseType()->btp->IsStructured() || f->functp->BaseType()->btp->IsBitInt() ||
                f->functp->BaseType()->btp->BaseType()->type == BasicType::memberptr_)
            {
                if (f->returnEXP && !managed)
                    push_param(f->returnEXP, funcsp, nullptr, f->returnSP->tp, 0);
            }
            if (f->thisptr)
            {
                push_param(f->thisptr, funcsp, nullptr, f->thistp, F_OBJECT);
            }
            genPascalArgs(f->arguments, funcsp);
            if (f->vararg && strcmp(f->sp->name, "__va_arg__"))
            {
                if (varargarray)
                {
                    Optimizer::IMODE* ap2 = Optimizer::tempreg(ISZ_ADDR, 0);
                    Optimizer::IMODE* ap3 = Allocate<Optimizer::IMODE>();
                    ap3->size = ISZ_ADDR;
                    ap3->mode = Optimizer::i_direct;
                    ap3->offset = varargarray;
                    Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
                    Optimizer::gen_icode(Optimizer::i_parm, nullptr, ap2, nullptr);
                }
                else
                {
                    Optimizer::IMODE* ap3 = Optimizer::make_immed(ISZ_OBJECT, 0);  // LDNULL
                    ap3->size = ISZ_ADDR;
                    Optimizer::gen_icode(Optimizer::i_parm, nullptr, ap3, nullptr);
                    Optimizer::intermed_tail->ptrbox = true;
                }
            }
        }
        else
        {
            if (f->returnEXP && f->returnEXP->type == ExpressionNode::auto_ && f->returnEXP->v.sp->sb->stackblock)
            {
                // cast operator creating a structure on the stack....
                auto rv = f->returnEXP->v.sp->tp->size;
                if (rv % Optimizer::chosenAssembler->arch->stackalign)
                    rv = rv + Optimizer::chosenAssembler->arch->stackalign - rv % Optimizer::chosenAssembler->arch->stackalign;
                Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0),
                                     Optimizer::make_immed(ISZ_UINT, rv), nullptr);
                Optimizer::SymbolManager::Get(f->returnEXP->v.sp)->imvalue = ap;
            }
            genCdeclArgs(f->arguments, funcsp);
            if (f->thisptr)
            {
                // now we have to check, is the thisptr being derived from a structuredAliasType?
                // if so we have to create a temporary, store the value, and generate a thisptr off the
                // address of the temporary
                // this can only happen if we are using a function as the this pointer...
                // auto ths = aliasToTemp(funcsp, f->thisptr);
                push_param(f->thisptr, funcsp, nullptr, f->thistp, F_OBJECT);
            }
        }
        if (f->callLab == -1)
        {
            f->callLab = ++consIndex;
            genCallLab(f->arguments, f->callLab);
        }
        fastcallSize = MarkFastcall(f->sp, f->functp, !!f->thisptr);
        if (!(Optimizer::chosenAssembler->arch->preferopts & OPT_REVERSEPARAM))
        {
            if (f->functp->IsFunction() && (f->functp->BaseType()->btp->IsStructured() || f->functp->BaseType()->btp->IsBitInt() ||
                                            f->functp->BaseType()->btp->BaseType()->type == BasicType::memberptr_))
            {
                if (f->returnEXP && !managed)
                    push_param(f->returnEXP, funcsp, nullptr, f->returnSP->tp, 0);
            }
        }
    }
    Optimizer::gen_icode(Optimizer::i_tag, nullptr, nullptr, nullptr);
    Optimizer::intermed_tail->beforeGosub = true;
    Optimizer::intermed_tail->ignoreMe = true;

    /* named function */
    if (f->fcall->type == ExpressionNode::imode_)
    {
        ap = f->fcall->v.imode;
        if (f->callLab && xcexp)
        {
            gen_xcexp_expression(f->callLab);
        }
        gosub = Optimizer::gen_igosub(node->type == ExpressionNode::intcall_ ? Optimizer::i_int : Optimizer::i_gosub, ap);
    }
    else
    {
        Optimizer::i_ops type = Optimizer::i_gosub;
        if (node->type == ExpressionNode::intcall_)
            type = Optimizer::i_int;
        if (!Optimizer::delegateforfuncptr)
        {
            ap = ap3 = gen_expr(funcsp, f->fcall, 0, ISZ_UINT);
        }
        if (ap->mode == Optimizer::i_immed && ap->offset->type == Optimizer::se_pc)
        {
        }
        else if (f->sp && f->sp->sb->attribs.inheritable.linkage2 == Linkage::import_ &&
                 f->sp->sb->storage_class != StorageClass::virtual_)
        {
            Optimizer::IMODE* ap1 = ap;
            Optimizer::gen_icode(Optimizer::i_assn, ap = Optimizer::tempreg(ISZ_ADDR, 0), ap1, 0);
            ap1 = Allocate<Optimizer::IMODE>();
            *ap1 = *ap;
            ap1->retval = false;
            ap = ap1;
            ap->mode = Optimizer::i_direct;
        }
        if (f->callLab && xcexp)
        {
            gen_xcexp_expression(f->callLab);
        }
        gosub = Optimizer::gen_igosub(type, ap);
    }
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        bool vaarg = false;
        if (f->sp->name[0] == '_' && !strcmp(f->sp->name, "__va_arg__"))
            vaarg = true;
        Optimizer::ArgList** p = &gosub->altargs;
        if (f->arguments)
            for (auto il : *f->arguments)
            {
                *p = Allocate<Optimizer::ArgList>();
                (*p)->tp = Optimizer::SymbolManager::Get(il->tp);
                if (vaarg && !IsLValue(il->exp) && !IsCastValue(il->exp))
                    (*p)->exp = Optimizer::SymbolManager::Get(il->exp);
                p = &(*p)->next;
            }
    }
    gosub->altsp = Optimizer::SymbolManager::Get(f->sp);
    if ((f->sp->sb->storage_class == StorageClass::typedef_ || f->sp->sb->storage_class == StorageClass::cast_) &&
        f->sp->tp->IsFunctionPtr())
    {
        Optimizer::typedefs.push_back(gosub->altsp);
    }
    gosub->fastcall = !!fastcallSize;

    if ((flags & F_NOVALUE) && !f->functp->BaseType()->btp->IsStructured() &&
        f->functp->BaseType()->btp->type != BasicType::memberptr_)
    {
        if (f->functp->BaseType()->btp->type == BasicType::void_)
            gosub->novalue = 0;
        else
            gosub->novalue = SizeFromType(f->functp->BaseType()->btp);
    }
    else if (f->functp->IsFunction() && f->functp->BaseType()->btp->IsStructured())
    {
        if ((flags & F_NOVALUE) && !managed)
            gosub->novalue = -2;
        else
            gosub->novalue = 0;
    }
    else
    {
        gosub->novalue = -1;
    }
    stackblockOfs = cdeclare;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
    {
        int n = f->thisptr ? 1 : 0;
        if (f->arguments)
            for (auto args : *f->arguments)
                ++n;
        if (f->returnEXP && !managed)
            n++;
        Optimizer::gen_nodag(Optimizer::i_parmadj, 0, Optimizer::make_parmadj(n),
                             Optimizer::make_parmadj(!f->functp->BaseType()->btp->IsVoid()));
    }
    if (f->returnEXP && managed && f->functp->IsFunction() &&
        (f->functp->BaseType()->btp->IsStructured() || f->functp->BaseType()->btp->IsBitInt()))
    {
        if (!(flags & F_INRETURN))
        {
            Optimizer::IMODE* ap1;
            int siz1;
            if (stobj)
            {
                ap1 = Allocate<Optimizer::IMODE>();
                *ap1 = *stobj;
                ap1->mode = Optimizer::i_ind;
                ap1->size = ISZ_OBJECT;
                ap1->offset->sp->tp = Optimizer::SymbolManager::Get(f->returnSP->tp->BaseType());
            }
            else
            {
                ap1 = gen_expr(funcsp, f->returnEXP, F_OBJECT, ISZ_OBJECT);
            }
            ap = Optimizer::tempreg(ISZ_OBJECT, 0);
            ap->retval = true;
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap, 0);
            ap = ap1;
        }
        else
        {
            gosub->novalue = -3;
            ap = nullptr;
        }
    }
    else if (!(flags & F_NOVALUE) && f->functp->IsFunction() && f->functp->BaseType()->btp->IsArray())
    {
        Optimizer::IMODE* ap1;
        int siz1;
        if (stobj)
        {
            ap1 = Allocate<Optimizer::IMODE>();
            *ap1 = *stobj;
            ap1->mode = Optimizer::i_ind;
            ap1->size = ISZ_OBJECT;
        }
        else
        {
            ap1 = Optimizer::tempreg(ISZ_OBJECT, 0);
        }
        ap1->offset->sp->tp = Optimizer::SymbolManager::Get(f->functp->BaseType()->btp->BaseType());
        ap = Optimizer::tempreg(ISZ_OBJECT, 0);
        ap->retval = true;
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap, 0);
        ap = ap1;
    }
    else if (!(flags & F_NOVALUE) && f->functp->IsFunction() && !f->functp->BaseType()->btp->IsVoid())
    {
        /* structures handled by callee... */
        if ((flags & F_RETURNSTRUCTBYVALUE) && f->sp->sb->isConstructor && f->sp->sb->parentClass->sb->structuredAliasType)
        {
            Optimizer::IMODE* ap1;
            ap1 = Optimizer::tempreg(ISZ_ADDR, 0);
            ap1->retval = true;
            Optimizer::gen_icode(Optimizer::i_assn, ap = Optimizer::tempreg(ISZ_ADDR, 0), ap1, 0);
            auto tpr = f->sp->sb->parentClass->sb->structuredAliasType;
            ap1 = Optimizer::indnode(ap, SizeFromType(tpr));
            ap = LookupLoadTemp(nullptr, ap1);
            if (ap1 != ap)
                gen_icode(Optimizer::i_assn, ap, ap1, nullptr);
        }
        else if (!f->functp->BaseType()->btp->IsStructured() && f->functp->BaseType()->btp->type != BasicType::memberptr_ &&
                 !f->functp->BaseType()->btp->IsBitInt())
        {
            Optimizer::IMODE *ap1, *ap2;
            int siz1 = SizeFromType(f->functp->BaseType()->btp);
            ap1 = Optimizer::tempreg(siz1, 0);
            ap1->retval = true;
            Optimizer::gen_icode(Optimizer::i_assn, ap = Optimizer::tempreg(siz1, 0), ap1, 0);
        }
        else
        {
            Optimizer::IMODE* ap1;
            ap1 = Optimizer::tempreg(ISZ_ADDR, 0);
            ap1->retval = true;
            Optimizer::gen_icode(Optimizer::i_assn, ap = Optimizer::tempreg(ISZ_ADDR, 0), ap1, 0);
            if (!(flags & (F_RETURNSTRUCTBYVALUE | F_RETURNSTRUCTNOADJUST)) && f->functp->BaseType()->btp->IsStructured())
            {
                if (f->functp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType)
                {
                    auto tpr = f->functp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType;
                    auto expr = tempVar(tpr, node->structByAddress);
                    Optimizer::IMODE* ap2 = gen_expr(funcsp, expr, F_STORE, natural_size(expr));
                    gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                    ap1 = gen_expr(funcsp, expr->left, 0, natural_size(expr->left));
                    ap = ap1;
                }
            }
        }
    }
    else
    {
        ap = Optimizer::tempreg(ISZ_ADDR, 0);
        ap->retval = true;
    }
    if (has_arg_destructors(f->arguments))
    {
        Optimizer::IMODE* ap1 = Optimizer::tempreg(ap->size, 0);
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap, nullptr);
        ap = ap1;
    }
    gen_arg_destructors(funcsp, f->arguments, f->destructors);
    /* undo pars and make a temp for the result */
    if (!(Optimizer::chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE))
    {
        adjust -= fastcallSize;
        if (adjust < 0)
            adjust = 0;
        adjust2 -= fastcallSize;
        if (adjust2 < 0)
            adjust2 = 0;
        if (f->sp->sb->attribs.inheritable.linkage != Linkage::stdcall_ &&
            f->sp->sb->attribs.inheritable.linkage != Linkage::pascal_)
            Optimizer::gen_nodag(Optimizer::i_parmadj, 0, Optimizer::make_parmadj(adjust), Optimizer::make_parmadj(adjust));
        else
            Optimizer::gen_nodag(Optimizer::i_parmadj, 0, Optimizer::make_parmadj(adjust2), Optimizer::make_parmadj(adjust));
    }

    push_nesting -= adjust;
    return ap;
}
Optimizer::IMODE* gen_atomic_barrier(SYMBOL* funcsp, ATOMICDATA* ad, Optimizer::IMODE* addr, Optimizer::IMODE* barrier)
{
    Optimizer::IMODE *left, *right;
    if (needsAtomicLockFromType(ad->tp))
    {
        Optimizer::IMODE* right;
        if (barrier)
        {
            left = Optimizer::make_immed(ISZ_UINT, -ad->memoryOrder1->v.i);
        }
        else
        {
            left = Optimizer::make_immed(ISZ_ADDR, ad->memoryOrder1->v.i);
            barrier = Optimizer::tempreg(ISZ_ADDR, 0);
            right = Optimizer::make_immed(ISZ_ADDR, ad->tp->size - ATOMIC_FLAG_SPACE);
            Optimizer::gen_icode(Optimizer::i_add, barrier, addr, right);
        }
        Optimizer::gen_icode(Optimizer::i_atomic_flag_fence, nullptr, left, barrier);
        return barrier;
    }
    else
    {
        if (barrier)
        {
            left = Optimizer::make_immed(ISZ_UINT, -ad->memoryOrder1->v.i);
        }
        else
        {
            left = Optimizer::make_immed(ISZ_UINT, ad->memoryOrder1->v.i);
        }
        Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
        return (Optimizer::IMODE*)-1;
    }
}
Optimizer::IMODE* gen_atomic(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
{
    Optimizer::IMODE* rv = nullptr;
    switch (node->v.ad->atomicOp)
    {
        Optimizer::IMODE *left, *right;
        Optimizer::IMODE* av;
        Optimizer::IMODE* barrier;
        int sz;
        Optimizer::i_ops op;
        case Optimizer::ao_init:
            if (node->v.ad->tp->IsStructured())
            {
                Optimizer::IMODE *ap1, *ap2, *ap6;
                av = gen_expr(funcsp, node->v.ad->address, F_VOL, ISZ_UINT);
                ap1 = Optimizer::LookupLoadTemp(nullptr, av);
                if (ap1 != av)
                    Optimizer::gen_icode(Optimizer::i_assn, ap1, av, nullptr);
                right = gen_expr(funcsp, node->v.ad->value, F_VOL | F_ADDR, ISZ_UINT);
                ap2 = Optimizer::LookupLoadTemp(nullptr, right);
                if (ap2 != right)
                    Optimizer::gen_icode(Optimizer::i_assn, ap2, right, nullptr);
                ap6 = Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->size);
                Optimizer::gen_icode(Optimizer::i_assnblock, ap6, ap1, ap2);
            }
            else
            {
                sz = SizeFromType(node->v.ad->tp);
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                left = Optimizer::indnode(av, sz);
                right = gen_expr(funcsp, node->v.ad->value, 0, sz);
                Optimizer::gen_icode(Optimizer::i_assn, left, right, nullptr);
            }
            if (needsAtomicLockFromType(node->v.ad->tp))
            {
                Optimizer::IMODE* temp = Optimizer::tempreg(ISZ_ADDR, 0);
                Optimizer::gen_icode(Optimizer::i_add, temp, av,
                                     Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->size - ATOMIC_FLAG_SPACE));
                temp = Optimizer::indnode(temp, ISZ_UINT);
                Optimizer::gen_icode(Optimizer::i_atomic_flag_clear, nullptr,
                                     Optimizer::make_immed(ISZ_UINT, Optimizer::mo_relaxed), temp);
            }
            rv = right;
            break;
        case Optimizer::ao_kill_dependency:
            rv = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            Optimizer::gen_icode_with_atomic(Optimizer::i_kill_dependency, nullptr, rv, nullptr);
            Optimizer::intermed_tail->alwayslive = true;
            break;
        case Optimizer::ao_flag_set_test:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            right = gen_expr(funcsp, node->v.ad->flg, F_STORE, ISZ_UINT);
            rv = Optimizer::tempreg(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_atomic_flag_test_and_set, rv, left, right);
            Optimizer::intermed_tail->alwayslive = true;
            break;
        case Optimizer::ao_flag_clear:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            right = gen_expr(funcsp, node->v.ad->flg, F_STORE, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_atomic_flag_clear, nullptr, left, right);
            break;
        case Optimizer::ao_thread_fence:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
            break;
        case Optimizer::ao_signal_fence:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_atomic_signal_fence, nullptr, left, nullptr);
            break;
        case Optimizer::ao_load:
            av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            if (node->v.ad->tp->IsStructured())
            {
                if ((Optimizer::chosenAssembler->arch->isLockFreeSize >= node->v.ad->tp->size &&
                     (node->v.ad->tp->size & (node->v.ad->tp->size - 1)) == 0))
                {
                    int sz = ISZ_U32;
                    switch (node->v.ad->tp->size)
                    {
                        case 1:
                            sz = ISZ_UCHAR;
                            break;
                        case 2:
                            sz = ISZ_U16;
                            break;
                        case 4:
                            sz = ISZ_U32;
                            break;
                        case 8:
                            sz = ISZ_ULONGLONG;
                            break;
                        default:
                            Utils::Fatal("ao_load: invalid structure size");
                            break;
                    }
                    left = Optimizer::indnode(av, sz);
                    auto expr = AnonymousVar(StorageClass::auto_, node->v.ad->tp);
                    auto store = Allocate<Optimizer::IMODE>();
                    store->mode = Optimizer::i_direct;
                    store->size = sz;
                    store->offset = Optimizer::SymbolManager::Get(expr);
                    auto temp = Optimizer::tempreg(sz, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, temp, left, nullptr);
                    barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, store, temp, nullptr);
                    Optimizer::intermed_tail->alwayslive = true;
                    gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
                    rv = Allocate<Optimizer::IMODE>();
                    rv->mode = Optimizer::i_immed;
                    rv->size = ISZ_ADDR;
                    rv->offset = store->offset;
                }
                else
                {
                    left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                    Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
                    EXPRESSION* exp = AnonymousVar(StorageClass::auto_, node->v.ad->tp);
                    rv = Allocate<Optimizer::IMODE>();
                    rv->mode = Optimizer::i_immed;
                    rv->size = ISZ_ADDR;
                    rv->offset = Optimizer::SymbolManager::Get(exp);
                    barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                    Optimizer::gen_icode(Optimizer::i_assnblock,
                                         Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->size - ATOMIC_FLAG_SPACE), rv, av);
                    gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
                }
            }
            else
            {
                left = Optimizer::indnode(av, SizeFromType(node->v.ad->tp));
                rv = Optimizer::tempreg(SizeFromType(node->v.ad->tp), 0);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                Optimizer::gen_icode_with_atomic(Optimizer::i_assn, rv, left, nullptr);
                Optimizer::intermed_tail->alwayslive = true;
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            break;
        case Optimizer::ao_store:
            if (node->v.ad->tp->IsStructured())
            {
                if ((Optimizer::chosenAssembler->arch->isLockFreeSize >= node->v.ad->tp->size &&
                     (node->v.ad->tp->size & (node->v.ad->tp->size - 1)) == 0))
                {
                    int sz = ISZ_U32;
                    switch (node->v.ad->tp->size)
                    {
                        case 1:
                            sz = ISZ_UCHAR;
                            break;
                        case 2:
                            sz = ISZ_U16;
                            break;
                        case 4:
                            sz = ISZ_U32;
                            break;
                        case 8:
                            sz = ISZ_ULONGLONG;
                            break;
                        default:
                            Utils::Fatal("ao_store: invalid structure size");
                            break;
                    }
                    left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                    Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
                    right = gen_expr(funcsp, node->v.ad->value, F_STORE, ISZ_ADDR);
                    right = Optimizer::indnode(right, sz);
                    right->size = sz;
                    auto temp = Optimizer::tempreg(sz, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, temp, right, 0);
                    right = temp;
                    av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                    left = Optimizer::indnode(av, sz);
                    left->size = sz;
                    Optimizer::gen_icode_with_atomic(Optimizer::i_assn, left, right, nullptr);
                    rv = right;
                    barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                    gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
                }
                else
                {
                    left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                    Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
                    right = gen_expr(funcsp, node->v.ad->value, F_STORE, ISZ_ADDR);
                    av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                    Optimizer::gen_icode(Optimizer::i_assnblock,
                                         Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->size - ATOMIC_FLAG_SPACE), av, right);
                    rv = right;
                    barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                    gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
                }
            }
            else
            {
                sz = SizeFromType(node->v.ad->tp);
                int sz1 = sz;
                if (sz == ISZ_FLOAT || sz == ISZ_DOUBLE)
                    sz1 = (node->v.ad->tp->size == 4 ? ISZ_U32 : ISZ_ULONGLONG);
                right = gen_expr(funcsp, node->v.ad->value, 0, sz);
                if (sz1 != sz && right->mode == Optimizer::i_direct && right->offset->type == Optimizer::se_tempref)
                {
                    // floating useing int registers, move the value into a temporary
                    auto expr = AnonymousVar(StorageClass::auto_, node->v.ad->tp);
                    auto temp = Allocate<Optimizer::IMODE>();
                    temp->mode = Optimizer::i_direct;
                    temp->size = sz;
                    temp->offset = Optimizer::SymbolManager::Get(expr);
                    Optimizer::gen_icode(Optimizer::i_assn, temp, right, nullptr);
                    right = Allocate<Optimizer::IMODE>();
                    right->mode = Optimizer::i_direct;
                    right->size = sz1;
                    right->offset = Optimizer::SymbolManager::Get(expr);
                }
                if (right->mode != Optimizer::i_direct || right->offset->type != Optimizer::se_tempref)
                {
                    right->size = sz1;
                    auto temp = Optimizer::tempreg(sz1, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, temp, right, nullptr);
                    right = temp;
                }
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                left = Optimizer::indnode(av, sz1);
                auto temp = Optimizer::tempreg(right->size, 0);
                Optimizer::gen_icode_with_conflict(Optimizer::i_assn, temp, right, nullptr, true);
                right = temp;
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                Optimizer::gen_icode_with_atomic(Optimizer::i_assn, left, right, nullptr);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
                rv = right;
            }
            break;
        case Optimizer::ao_modify_fetch:
        case Optimizer::ao_fetch_modify:
            if (node->v.ad->tp->IsStructured())
            {
                left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
                // presumed xchg
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                right = gen_expr(funcsp, node->v.ad->value, F_STORE, ISZ_ADDR);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                EXPRESSION* exp = AnonymousVar(StorageClass::auto_, node->v.ad->tp);
                rv = Allocate<Optimizer::IMODE>();
                rv->mode = Optimizer::i_immed;
                rv->size = ISZ_ADDR;
                rv->offset = Optimizer::SymbolManager::Get(exp);
                Optimizer::gen_icode(Optimizer::i_assnblock, Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->btp->size), rv, av);
                Optimizer::gen_icode(Optimizer::i_assnblock, Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->btp->size), av, right);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            else
            {
                switch ((Keyword)node->v.ad->third->v.i)
                {
                    default:
                    case Keyword::asplus_:
                        op = Optimizer::i_add;
                        break;
                    case Keyword::asminus_:
                        op = Optimizer::i_sub;
                        break;
                    case Keyword::asor_:
                        op = Optimizer::i_or;
                        break;
                    case Keyword::asand_:
                        op = Optimizer::i_and;
                        break;
                    case Keyword::asxor_:
                        op = Optimizer::i_eor;
                        break;
                    case Keyword::assign_:
                        op = Optimizer::i_xchg;
                        break;
                }
                sz = SizeFromType(node->v.ad->tp);
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                left = Optimizer::indnode(av, sz);
                right = gen_expr(funcsp, node->v.ad->value, F_STORE, sz);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                rv = Optimizer::tempreg(sz, 0);
                if (needsAtomicLockFromType(node->v.ad->tp))
                {
                    Optimizer::IMODE* tv = Optimizer::tempreg(sz, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, rv, left, nullptr);
                    if (op == Optimizer::i_xchg)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, tv, right, nullptr);
                    }
                    else
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, tv, rv, nullptr);
                        Optimizer::gen_icode(op, tv, tv, right);
                    }
                    if (node->v.ad->atomicOp == Optimizer::ao_modify_fetch)
                        Optimizer::gen_icode(Optimizer::i_assn, rv, tv, nullptr);
                    Optimizer::gen_icode(Optimizer::i_assn, left, tv, nullptr);
                }
                else
                {
                    Optimizer::gen_icode_with_atomic(op, rv, left, right);
                    Optimizer::intermed_tail->atomicpostfetch = node->v.ad->atomicOp == Optimizer::ao_modify_fetch;
                    Optimizer::intermed_tail->alwayslive = true;
                }
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            break;
        case Optimizer::ao_cmpxchgweak:
        case Optimizer::ao_cmpxchgstrong:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
            if (node->v.ad->tp->IsStructured())
            {

                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                right = gen_expr(funcsp, node->v.ad->third, 0, ISZ_ADDR);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                rv = Optimizer::tempreg(ISZ_UINT, 0);
                Optimizer::gen_icode(Optimizer::i_assn, rv, Optimizer::make_immed(ISZ_UINT, 0), nullptr);
                int labno = Optimizer::nextLabel++, labno2 = Optimizer::nextLabel++;
                Optimizer::gen_icgoto(Optimizer::i_cmpblock, labno, right, av);
                Optimizer::QUAD* q = Optimizer::intermed_tail;
                while (q->dc.opcode != Optimizer::i_cmpblock)
                    q = q->back;
                q->ans = Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->btp->size);
                left = gen_expr(funcsp, node->v.ad->value, F_VOL, ISZ_ADDR);
                Optimizer::gen_icode(Optimizer::i_assnblock, Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->btp->size), av, left);
                Optimizer::gen_icode(Optimizer::i_assn, rv, Optimizer::make_immed(ISZ_UINT, 1), nullptr);
                Optimizer::gen_igoto(Optimizer::i_goto, labno2);
                Optimizer::gen_label(labno);
                Optimizer::gen_icode(Optimizer::i_assnblock, Optimizer::make_immed(ISZ_UINT, node->v.ad->tp->btp->size), right, av);
                Optimizer::gen_icode(Optimizer::i_assn, rv, Optimizer::make_immed(ISZ_UINT, 0), nullptr);
                Optimizer::gen_label(labno2);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            else
            {
                left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
                sz = SizeFromType(node->v.ad->tp);
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                left = Optimizer::indnode(av, sz);
                right = gen_expr(funcsp, node->v.ad->third, 0, ISZ_ADDR);
                right = Optimizer::indnode(right, sz);
                rv = gen_expr(funcsp, node->v.ad->value, F_VOL, sz);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                if (needsAtomicLockFromType(node->v.ad->tp))
                {
                    Optimizer::IMODE* temp = Optimizer::tempreg(sz, 0);
                    Optimizer::IMODE* asnfrom = rv;
                    rv = Optimizer::tempreg(ISZ_UINT, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, temp, left, nullptr);
                    int lbno = Optimizer::nextLabel++, lbno2 = Optimizer::nextLabel++;
                    ;
                    Optimizer::gen_icode(Optimizer::i_sete, rv, temp, right);
                    Optimizer::gen_icgoto(Optimizer::i_je, lbno, rv, Optimizer::make_immed(ISZ_UINT, 0));
                    Optimizer::gen_icode(Optimizer::i_assn, temp, asnfrom, nullptr);
                    Optimizer::gen_icode(Optimizer::i_assn, left, temp, nullptr);
                    Optimizer::gen_igoto(Optimizer::i_goto, lbno2);
                    Optimizer::gen_label(lbno);
                    Optimizer::gen_icode(Optimizer::i_assn, right, temp, nullptr);
                    Optimizer::gen_label(lbno2);
                }
                else
                {
                    Optimizer::gen_icode(node->v.ad->atomicOp == Optimizer::ao_cmpxchgweak ? Optimizer::i_cmpxchgweak
                                                                                           : Optimizer::i_cmpxchgstrong,
                                         left, rv, right);
                    rv = Optimizer::tempreg(ISZ_UINT, 0);
                    rv->retval = true;
                    av = Optimizer::tempreg(ISZ_UINT, 0);
                    Optimizer::gen_icode(Optimizer::i_assn, av, rv, nullptr);
                    rv = av;
                }
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            left = gen_expr(funcsp, node->v.ad->memoryOrder2, 0, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_atomic_thread_fence, nullptr, left, nullptr);
            break;
    }
    return rv;
}
/*-------------------------------------------------------------------------*/
Optimizer::IMODE* gen_expr(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *      general expression evaluation. returns the addressing mode
 *      of the result.
 *
 *			The first argument of this and other routines is a target for
 *			assignmennt.  If this is 0 and a target is needed a temp will be
 *			used; otherwise the target will be evaluated annd used.
 */
{
    Optimizer::IMODE *ap1, *ap2, *ap3;
    Optimizer::IMODE* rv;
    Optimizer::IMODE *lbarrier, *rbarrier;
    int lab0;
    int siz1;
    int store = flags & F_STORE;
    Optimizer::SimpleSymbol* sym;

    flags &= ~F_STORE;
    if (node == 0)
    {
        /*        diag("null node in gen_expr.");*/
        return 0;
    }
    while (node->type == ExpressionNode::not__lvalue_ || node->type == ExpressionNode::lvalue_)
        node = node->left;
    //    lbarrier = doatomicFence(funcsp, node, node->left, 0);
    //    rbarrier = doatomicFence(funcsp, nullptr, node->right, 0);
    if (flags & F_NOVALUE)
    {
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            switch (node->type)
            {
                case ExpressionNode::auto_inc_:
                case ExpressionNode::auto_dec_:
                case ExpressionNode::assign_:
                case ExpressionNode::callsite_:
                case ExpressionNode::thisref_:
                case ExpressionNode::intcall_:
                case ExpressionNode::blockassign_:
                case ExpressionNode::blockclear_:
                case ExpressionNode::comma_:
                case ExpressionNode::cpblk_:
                case ExpressionNode::initblk_:
                case ExpressionNode::initobj_:
                case ExpressionNode::constexprconstructor_:
                default:
                    Optimizer::gen_nodag(Optimizer::i_expressiontag, 0, 0, 0);
                    Optimizer::intermed_tail->dc.v.label = 1;
                    Optimizer::intermed_tail->ignoreMe = true;
                    break;
                case ExpressionNode::select_:
                    break;
            }
        }
    }
    switch (node->type)
    {
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
            // usually this would be found in a template argument list, but just in case...
            optimize_for_constants(&node);
            if (node->type == ExpressionNode::dot_ || node->type == ExpressionNode::pointsto_)
            {
                node = MakeIntExpression(ExpressionNode::c_i_, 0);
                diag("gen_expr: member reference not valid");
            }
            rv = gen_expr(funcsp, node, flags, size);
            break;
        case ExpressionNode::shiftby_:
            ap3 = gen_expr(funcsp, node->left, flags, size);
            ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            rv = ap1;
            break;
        case ExpressionNode::x_bitint_:
        case ExpressionNode::x_ubitint_:
            return bitint_convert(node, size);
        case ExpressionNode::x_wc_:
            siz1 = ISZ_WCHAR;
            goto castjoin;
        case ExpressionNode::x_c_:
            siz1 = -ISZ_UCHAR;
            goto castjoin;
        case ExpressionNode::x_u16_:
            siz1 = ISZ_U16;
            goto castjoin;
        case ExpressionNode::x_u32_:
            siz1 = ISZ_U32;
            goto castjoin;
        case ExpressionNode::x_bool_:
            siz1 = ISZ_BOOLEAN;
            goto castjoin;
        case ExpressionNode::x_bit_:
            siz1 = ISZ_BIT;
            goto castjoin;
        case ExpressionNode::x_uc_:
            siz1 = ISZ_UCHAR;
            goto castjoin;
        case ExpressionNode::x_s_:
            siz1 = -ISZ_USHORT;
            goto castjoin;
        case ExpressionNode::x_us_:
            siz1 = ISZ_USHORT;
            goto castjoin;
        case ExpressionNode::x_i_:
            siz1 = -ISZ_UINT;
            goto castjoin;
        case ExpressionNode::x_ui_:
            siz1 = ISZ_UINT;
            goto castjoin;
        case ExpressionNode::x_inative_:
            siz1 = -ISZ_UNATIVE;
            goto castjoin;
        case ExpressionNode::x_unative_:
            siz1 = ISZ_UNATIVE;
            goto castjoin;
        case ExpressionNode::x_l_:
            siz1 = -ISZ_ULONG;
            goto castjoin;
        case ExpressionNode::x_ul_:
            siz1 = ISZ_ULONG;
            goto castjoin;
        case ExpressionNode::x_ll_:
            siz1 = -ISZ_ULONGLONG;
            goto castjoin;
        case ExpressionNode::x_ull_:
            siz1 = ISZ_ULONGLONG;
            goto castjoin;

        case ExpressionNode::x_string_:
            siz1 = ISZ_STRING;
            goto castjoin;
        case ExpressionNode::x_object_:
            siz1 = ISZ_OBJECT;
            goto castjoin;

        case ExpressionNode::x_f_:
            siz1 = ISZ_FLOAT;
            goto castjoin;
        case ExpressionNode::x_d_:
            siz1 = ISZ_DOUBLE;
            goto castjoin;
        case ExpressionNode::x_ld_:
            siz1 = ISZ_LDOUBLE;
            goto castjoin;
        case ExpressionNode::x_fi_:
            siz1 = ISZ_IFLOAT;
            goto castjoin;
        case ExpressionNode::x_di_:
            siz1 = ISZ_IDOUBLE;
            goto castjoin;
        case ExpressionNode::x_ldi_:
            siz1 = ISZ_ILDOUBLE;
            goto castjoin;
        case ExpressionNode::x_fc_:
            siz1 = ISZ_CFLOAT;
            goto castjoin;
        case ExpressionNode::x_dc_:
            siz1 = ISZ_CDOUBLE;
            goto castjoin;
        case ExpressionNode::x_ldc_:
            siz1 = ISZ_CLDOUBLE;
            goto castjoin;
        case ExpressionNode::x_fp_:
            siz1 = ISZ_FARPTR;
            goto castjoin;
        case ExpressionNode::x_sp_:
            siz1 = ISZ_SEG;
            goto castjoin;
        case ExpressionNode::x_p_:
            siz1 = ISZ_ADDR;
        castjoin:
            if (bitintbits(node->left))
            {
                return bitint_convert(node, size);
            }
            ap3 = gen_expr(funcsp, node->left, flags & ~F_NOVALUE, natural_size(node->left));
            ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            if (ap1->mode == Optimizer::i_immed)
            {
                if (isintconst(ap1->offset))
                {
                    if (siz1 >= ISZ_FLOAT)
                    {
                        FPF f;
                        Optimizer::IntToFloat(&f, natural_size(node->left), ap1->offset->i);
                        ap1 = Optimizer::make_fimmed(siz1, f);
                    }
                }
                else if (isfloatconst(ap1->offset))
                {
                    if (siz1 < ISZ_FLOAT)
                    {
                        ap1 = Optimizer::make_immed(siz1, (long long)(ap1->offset->f));
                    }
                }
            }
            if (ap1->size == siz1)
                ap2 = ap1;
            else
            {
                ap2 = Optimizer::LookupCastTemp(ap1, siz1);
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                //                Optimizer::gen_icode(Optimizer::i_assn, ap2 = Optimizer::tempreg(siz1, 0), ap1, 0);
            }
            if (ap2->offset)
            {
                ap2->vol = node->left->isvolatile;
                ap2->restricted = node->left->isrestrict;
            }
            rv = ap2;
            break;
        case ExpressionNode::msil_array_access_: {
            int i;
            ap1 = gen_expr(funcsp, node->v.msilArray->base, 0, ISZ_ADDR);
            ap1->msilObject = true;
            Optimizer::gen_icode(Optimizer::i_parm, 0, ap1, 0);
            for (i = 0; i < node->v.msilArray->count; i++)
            {
                ap1 = gen_expr(funcsp, node->v.msilArray->indices[i], 0, ISZ_ADDR);
                Optimizer::gen_icode(Optimizer::i_parm, 0, ap1, 0);
            }
            if (!(flags & F_STORE))
            {
                Type* base = node->v.msilArray->tp;
                while (base->IsArray())
                    base = base->BaseType()->btp;
                rv = Optimizer::tempreg(base->IsStructured() ? ISZ_OBJECT : SizeFromType(base), 0);
                ap1 = Allocate<Optimizer::IMODE>();
                ap1->size = rv->size;
                ap1->mode = Optimizer::i_immed;
                ap1->offset = Optimizer::SymbolManager::Get(node);
                Optimizer::gen_icode(Optimizer::i_assn, rv, ap1, nullptr);
            }
            else
            {
                rv = nullptr;
            }
            break;
        }
        case ExpressionNode::substack_:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_substack, ap2 = Optimizer::tempreg(ISZ_ADDR, 0), ap1, nullptr);
            rv = ap2;
            break;
        case ExpressionNode::alloca_:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            Optimizer::gen_icode(Optimizer::i_substack, ap2 = Optimizer::tempreg(ISZ_ADDR, 0), ap1, nullptr);
            rv = ap2;
            break;
        case ExpressionNode::initblk_:
            rv = gen_cpinitblock(node, funcsp, false, flags);
            break;
        case ExpressionNode::cpblk_:
            rv = gen_cpinitblock(node, funcsp, true, flags);
            break;
        case ExpressionNode::initobj_:
            rv = gen_cpinitobj(node, funcsp, true, flags);
            break;
        case ExpressionNode::sizeof_:
            rv = gen_cpsizeof(node, funcsp, true, flags);
            break;
        case ExpressionNode::loadstack_:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
            Optimizer::gen_icode(Optimizer::i_loadstack, 0, ap1, 0);
            rv = nullptr;
            break;
        case ExpressionNode::savestack_:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
            Optimizer::gen_icode(Optimizer::i_savestack, 0, ap1, 0);
            rv = nullptr;
            break;
        case ExpressionNode::threadlocal_: {
            Optimizer::SimpleExpression* exp = Optimizer::SymbolManager::Get(node);
            ap1 = Allocate<Optimizer::IMODE>();
            ap1->mode = Optimizer::i_direct;
            ap1->offset = exp;
            ap1->size = size;
        }
            ap2 = Optimizer::LookupLoadTemp(ap1, ap1);
            if (ap1 != ap2)
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
            rv = ap2;
            sym = ap1->offset->sp;
            Optimizer::EnterExternal(sym);
            break;
        case ExpressionNode::auto_:
            if (node->v.sp->sb->stackblock)
            {
                rv = Optimizer::SymbolManager::Get(node->v.sp)->imvalue;
                break;
            }
            sym = Optimizer::SymbolManager::Get(node->v.sp);
            if (sym->storage_class == Optimizer::scc_parameter && sym->paramSubstitute)
            {
                return gen_expr(funcsp, sym->paramSubstitute->left, flags, size);
            }
            sym->allocate = true;
            // fallthrough
        case ExpressionNode::pc_:
        case ExpressionNode::global_:
        case ExpressionNode::absolute_:
            sym = Optimizer::SymbolManager::Get(node->v.sp);
            if (node->type == ExpressionNode::pc_ || node->type == ExpressionNode::global_ ||
                node->type == ExpressionNode::absolute_)
            {
                Optimizer::EnterExternal(sym);
                if (sym->inlineSym)
                    InsertInline(sym->inlineSym);
                else if (node->v.sp->tp->IsFunction())
                    InsertInline(node->v.sp);
            }
            if (sym->imaddress && (Optimizer::architecture != ARCHITECTURE_MSIL))
            {
                ap1 = sym->imaddress;
            }
            else
            {
                ap1 = Allocate<Optimizer::IMODE>();
                ap1->offset = Optimizer::SymbolManager::Get(node);
                ap1->mode = Optimizer::i_immed;
                if (flags & F_OBJECT)
                    ap1->msilObject = true;
                ap1->size = size;
            }
            if (node->v.sp->tp->IsArray() && node->v.sp->tp->BaseType()->msil && !store)
                ap1->msilObject = true;
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1; /* return reg */
            rv->runtimeData = node->runtimeData;
            if (rv->runtimeData)
            {
                rv->runtimeData->runtimeSym = Optimizer::SymbolManager::Get(rv->runtimeData->runtimeSymOrig);
            }
            sym->imaddress = ap1;
            sym->addressTaken = true;
            break;
        case ExpressionNode::paramsubstitute_:
            diag("gen_expr: taking address of paramater temporary");
            rv = node->v.imode;
            break;
        case ExpressionNode::labcon_:
            ap1 = Allocate<Optimizer::IMODE>();
            if (node->computedLabel)
            {
                node->computedLabel = false;
                node->v.i += codeLabelOffset;
                Optimizer::computedLabels.insert(node->v.i);
            }
            ap1->offset = Optimizer::SymbolManager::Get(node);
            ap1->mode = Optimizer::i_immed;
            ap1->size = size;
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1; /* return reg */
            break;
        case ExpressionNode::imode_:
            ap2 = (Optimizer::IMODE*)node->left;
            rv = ap2; /* return reg */
            break;
        case ExpressionNode::c_bitint_:
        case ExpressionNode::c_ubitint_:
            ap1 = make_bitint(node);
            ap1->offset->unionoffset = node->unionoffset;
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case ExpressionNode::c_bool_:
        case ExpressionNode::c_c_:
        case ExpressionNode::c_i_:
        case ExpressionNode::c_l_:
        case ExpressionNode::c_ll_:
        case ExpressionNode::nullptr_:
            if (size >= ISZ_FLOAT)
            {
                FPF f;
                f = (long long)node->v.i;
                ap1 = Optimizer::make_fimmed(size, f);
            }
            else
            {
                ap1 = Optimizer::make_immed(size, node->v.i);
                ap1->offset->type = Optimizer::se_i;
            }
            ap1->offset->unionoffset = node->unionoffset;
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;

        case ExpressionNode::c_uc_:
        case ExpressionNode::c_u16_:
        case ExpressionNode::c_u32_:
        case ExpressionNode::c_ul_:
        case ExpressionNode::c_ui_:

        case ExpressionNode::c_ull_:
        case ExpressionNode::c_wc_:
            if (size >= ISZ_FLOAT)
            {
                FPF f;
                f = (long long)node->v.i;
                ap1 = Optimizer::make_fimmed(size, f);
            }
            else
            {
                ap1 = Optimizer::make_immed(size, node->v.i);
                ap1->offset->type = Optimizer::se_ui;
            }
            ap1->offset->unionoffset = node->unionoffset;
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case ExpressionNode::c_string_:
            ap1 = Allocate<Optimizer::IMODE>();
            ap1->mode = Optimizer::i_immed;
            ap1->size = ISZ_STRING;
            ap1->offset = Optimizer::SymbolManager::Get(node);
            rv = ap1;
            break;
        case ExpressionNode::c_f_:
        case ExpressionNode::c_d_:
        case ExpressionNode::c_ld_:
            if (size >= ISZ_FLOAT)
            {
                ap1 = Optimizer::make_fimmed(size, *node->v.f);
            }
            else
            {
                long long a = (long long)*node->v.f;
                ap1 = Optimizer::make_immed(size, a);
            }
            ap1->offset->type = Optimizer::se_f;
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
            if (size >= ISZ_FLOAT)
            {
                ap1 = Optimizer::make_fimmed(size, *node->v.f);
            }
            else
            {
                long long a = (long long)*node->v.f;
                ap1 = Optimizer::make_immed(size, a);
            }
            ap1->offset->type = Optimizer::se_fi;
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case ExpressionNode::c_fc_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::c_ldc_:
            ap1 = Allocate<Optimizer::IMODE>();
            ap1->mode = Optimizer::i_immed;
            ap1->offset = Optimizer::SymbolManager::Get(node);
            switch (node->type)
            {
                case ExpressionNode::c_fc_:
                    ap1->size = ISZ_CFLOAT;
                    break;
                case ExpressionNode::c_dc_:
                    ap1->size = ISZ_CDOUBLE;
                    break;
                case ExpressionNode::c_ldc_:
                    ap1->size = ISZ_CLDOUBLE;
                    break;
                default:
                    break;
            }
            ap2 = Optimizer::LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                Optimizer::gen_icode(Optimizer::i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case ExpressionNode::l_bitint_:
        case ExpressionNode::l_ubitint_:
            // take the address of the bitint, if it is actually used as an lref we will need the address
            // to pass to some function somewhere...
            return gen_expr(funcsp, node->left, 0, ISZ_ADDR);
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_s_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_us_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_fp_:
        case ExpressionNode::l_sp_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_string_: {
            EXPRESSION* ps;
            if (node->left->type == ExpressionNode::l_ref_ && node->left->left->type == ExpressionNode::auto_ &&
                (ps = Optimizer::SymbolManager::Get(node->left->left->v.sp)->paramSubstitute))
            {
                if (ps->left && ps->left->type == ExpressionNode::paramsubstitute_ && ps->left->left)
                {
                    return ps->left->v.imode;
                }
            }
            else if (node->type == ExpressionNode::l_ref_ && node->left->type == ExpressionNode::auto_ &&
                     (ps = Optimizer::SymbolManager::Get(node->left->v.sp)->paramSubstitute))
            {
                if (ps->left && ps->left->type == ExpressionNode::paramsubstitute_ && ps->left->left)
                {
                    return gen_expr(funcsp, ps->left->left, 0, natural_size(ps->left->left));
                }
            }
            else if (node->left->type == ExpressionNode::l_p_ && node->left->left->type == ExpressionNode::auto_)
            {
                if (inlineSymStructPtr.size() && node->left->left->v.sp->sb->retblk)
                {
                    auto exp = inlineSymStructPtr.back();
                    if (IsLValue(exp) && exp->left->type == ExpressionNode::paramsubstitute_)
                    {
                        return exp->left->v.imode;
                    }
                }
                else if (inlineSymThisPtr.size() && inlineSymThisPtr.back() && node->left->left->v.sp->sb->thisPtr)
                {
                    auto exp = inlineSymThisPtr.back();
                    if (IsLValue(exp) && exp->left->type == ExpressionNode::paramsubstitute_ && exp->left->left)
                    {
                        return exp->left->v.imode;
                    }
                }
            }
            else if (node->left->type == ExpressionNode::l_p_ && node->left->left->type == ExpressionNode::paramsubstitute_ &&
                     node->left->left->left)
            {
                return node->left->left->v.imode;
            }
            else if (inlineSymStructPtr.size() && node->type == ExpressionNode::l_p_ && node->left->type == ExpressionNode::l_p_ &&
                     node->left->left->type == ExpressionNode::auto_ && node->left->left->v.sp->sb->retblk)
            {
                auto exp = inlineSymStructPtr.back();
                if (IsLValue(exp) && exp->left->type == ExpressionNode::paramsubstitute_)
                {
                    return exp->left->v.imode;
                }
            }
            ap1 = gen_deref(node, funcsp, flags | store);
            rv = ap1;
            break;
        }
        case ExpressionNode::l_object_:
            ap1 = gen_deref(node, funcsp, flags | store);
            ap1->offset->sp->tp = Optimizer::SymbolManager::Get(node->v.tp);
            rv = ap1;
            break;
        case ExpressionNode::bits_:
            size = natural_size(node->left);
            ap3 = gen_expr(funcsp, node->left, 0, size);
            ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            ap1 = make_bf(node, ap1, size);
            rv = ap1;
            break;
        case ExpressionNode::uminus_:
            rv = gen_unary(funcsp, node, flags, size, Optimizer::i_neg);
            break;
        case ExpressionNode::compl_:
            rv = gen_unary(funcsp, node, flags, size, Optimizer::i_not);
            break;
        case ExpressionNode::add_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_add);
            break;
        case ExpressionNode::sub_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_sub);
            break;
        case ExpressionNode::and_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_and);
            break;
        case ExpressionNode::or_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_or);
            break;
        case ExpressionNode::xor_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_eor);
            break;
        case ExpressionNode::arraymul_:
            rv = gen_pmul(funcsp, node, flags, size);
            break;
        case ExpressionNode::arraydiv_:
            rv = gen_pdiv(funcsp, node, flags, size);
            break;
        case ExpressionNode::mul_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_mul);
            break;
        case ExpressionNode::umul_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_mul);
            break;
        case ExpressionNode::div_:
            rv = gen_sdivide(funcsp, node, flags, size, Optimizer::i_sdiv, false);
            break;
        case ExpressionNode::udiv_:
            rv = gen_udivide(funcsp, node, flags, size, Optimizer::i_udiv, false);
            break;
        case ExpressionNode::mod_:
            rv = gen_sdivide(funcsp, node, flags, size, Optimizer::i_smod, true);
            break;
        case ExpressionNode::umod_:
            rv = gen_udivide(funcsp, node, flags, size, Optimizer::i_umod, true);
            break;
        case ExpressionNode::lsh_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_lsl);
            break;
        case ExpressionNode::arraylsh_:
            rv = gen_binary(funcsp, node, flags, size, /*Optimizer::i_arraylsh*/ Optimizer::i_lsl);
            break;
        case ExpressionNode::rsh_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_asr);
            break;
        case ExpressionNode::ursh_:
            rv = gen_binary(funcsp, node, flags, size, Optimizer::i_lsr);
            break;
        case ExpressionNode::assign_:
            rv = gen_assign(funcsp, node, flags, size);
            break;
        case ExpressionNode::blockassign_:
            rv = gen_moveblock(node, funcsp);
            break;
        case ExpressionNode::blockclear_:
            rv = gen_clearblock(node, funcsp);
            break;
        case ExpressionNode::auto_inc_:
            rv = gen_aincdec(funcsp, node, flags, size, Optimizer::i_add);
            break;
        case ExpressionNode::auto_dec_:
            rv = gen_aincdec(funcsp, node, flags, size, Optimizer::i_sub);
            break;
        case ExpressionNode::land_:
        case ExpressionNode::lor_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::not_:
        case ExpressionNode::mp_compare_:
        case ExpressionNode::mp_as_bool_:
            ap1 = gen_relat(node, funcsp);
            rv = ap1;
            break;
        case ExpressionNode::select_:
            ap1 = gen_expr(funcsp, node->left, flags, size);
            rv = ap1;
            break;
        case ExpressionNode::atomic_:
            ap1 = gen_atomic(funcsp, node, flags, size);
            rv = ap1;
            break;
        case ExpressionNode::hook_:
            ap1 = gen_hook(funcsp, node, flags, size);
            rv = ap1;
            break;
        case ExpressionNode::comma_: {
            EXPRESSION* search = node;
            while (search && search->type == ExpressionNode::comma_)
            {
                gen_void_(search->left, funcsp);
                search = search->right;
            }
            ap1 = gen_expr(funcsp, search, flags, size);
            rv = ap1;
        }
        break;
        case ExpressionNode::constexprconstructor_:
            ap1 = gen_expr(funcsp, node->left, flags, size);
            rv = ap1;
            break;
        case ExpressionNode::thisref_:
            if (node->dest && xcexp)
            {
                if (!node->v.t.tp->BaseType()->sp->sb->pureDest)
                {
                    int offset = 0;
                    auto exp = relptr(node->v.t.thisptr, offset, true);
                    if (exp && exp->type == ExpressionNode::auto_)
                    {
                        Optimizer::SymbolManager::Get(exp->v.sp)->generated = true;
                        Optimizer::SymbolManager::Get(exp->v.sp)->allocate = true;
                    }
                    node->v.t.thisptr->xcDest = ++consIndex;
                    gen_xcexp_expression(consIndex);
                    __xcentry* t = Allocate<__xcentry>();
                    ;
                    t->byStmt = false;
                    t->exp = node;
                    rttiStatements[node->v.t.thisptr->xcInit][node->v.t.thisptr->xcDest] = t;
                }
            }
            ap1 = gen_expr(funcsp, node->left, flags, size);
            if (!node->dest && xcexp)
            {
                if (!node->v.t.tp->BaseType()->sp->sb->pureDest)
                {
                    node->v.t.thisptr->xcInit = ++consIndex;
                    gen_xcexp_expression(consIndex);
                    __xcentry* t = Allocate<__xcentry>();
                    ;
                    t->byStmt = false;
                    t->exp = node;
                    rttiStatements[node->v.t.thisptr->xcInit][node->v.t.thisptr->xcDest] = t;
                }
            }
            if (node->left->type == ExpressionNode::stmt_)
            {
                rv = Optimizer::tempreg(ISZ_ADDR, 0);
                rv->retval = true;
            }
            else
            {
                rv = ap1;
            }
            break;
        case ExpressionNode::structadd_:
            if (node->right->type == ExpressionNode::structelem_)
            {
                // prepare for the MSIL ldflda instruction
                Optimizer::IMODE *aa1, *aa2;
                aa1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
                aa1->msilObject = true;
                aa2 = Optimizer::tempreg(aa1->size, 0);
                Optimizer::gen_icode(Optimizer::i_assn, aa2, aa1, nullptr);
                aa1 = Allocate<Optimizer::IMODE>();
                aa1->offset = Optimizer::SymbolManager::Get(node->right);
                aa1->mode = Optimizer::i_immed;
                aa1->size = aa2->size;
                ap1 = LookupExpression(Optimizer::i_add, aa2->size, aa2, aa1);
                rv = ap1;
                break;
            }
            else
                rv = gen_binary(funcsp, node, flags, ISZ_ADDR, /*Optimizer::i_struct*/ Optimizer::i_add);
            break;
        case ExpressionNode::arrayadd_:
            rv = gen_binary(funcsp, node, flags, ISZ_ADDR, /*Optimizer::i_arrayindex*/ Optimizer::i_add);
            break;
            /*		case ExpressionNode::array_:
                        rv = gen_binary( funcsp, node,flags,ISZ_ADDR,Optimizer::i_array);
            */
        case ExpressionNode::check_nz_:
            lab0 = Optimizer::nextLabel++;
            falsejmp(node->left->left, funcsp, lab0);
            gen_void_(node->left->right, funcsp);
            Optimizer::gen_label(lab0);
            ap3 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
            ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            rv = ap1;
            break;
        case ExpressionNode::trapcall_:
            rv = gen_trapcall(funcsp, node, flags);
            break;
        case ExpressionNode::callsite_:
        case ExpressionNode::intcall_:
            rv = gen_funccall(funcsp, node, flags);
            break;
        case ExpressionNode::stmt_:
            rv = gen_stmt_from_expr(funcsp, node, flags);
            break;
        case ExpressionNode::templateparam_:
            rv = gen_expr(funcsp, node->v.sp->tp->templateParam->second->byNonType.val, 0, ISZ_UINT);
            break;
        case ExpressionNode::const_:
            /* should never get here unless the constant optimizer is turned off */
            ap1 = gen_expr(funcsp, node->v.sp->sb->init->front()->exp, 0, 0);
            rv = ap1;
            break;
        case ExpressionNode::cvarpointer_:
            // the front end generates constexpr structures which may have these in it
            // untile we look into that a little more closely just make sure the code gen doesn't crash
            rv = Optimizer::make_immed(ISZ_UINT, 0);
            break;

        default:
            /* explicitly uncoded */
            diag("uncoded node in gen_expr.");
            rv = 0;
    }
    if (flags & F_SWITCHVALUE)
    {
        DumpIncDec(funcsp);
    }
    if (flags & F_NOVALUE)
    {
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            switch (node->type)
            {
                case ExpressionNode::auto_inc_:
                case ExpressionNode::auto_dec_:
                case ExpressionNode::assign_:
                case ExpressionNode::callsite_:
                case ExpressionNode::thisref_:
                case ExpressionNode::intcall_:
                case ExpressionNode::blockassign_:
                case ExpressionNode::blockclear_:
                case ExpressionNode::comma_:
                case ExpressionNode::cpblk_:
                case ExpressionNode::initblk_:
                case ExpressionNode::initobj_:
                case ExpressionNode::select_:
                case ExpressionNode::constexprconstructor_:
                    break;
                default:
                    Optimizer::gen_nodag(Optimizer::i_expressiontag, 0, 0, 0);
                    Optimizer::intermed_tail->dc.v.label = 0;
                    Optimizer::intermed_tail->ignoreMe = true;
                    break;
            }
        }
        DumpIncDec(funcsp);
    }
    //    doatomicFence(funcsp, nullptr, node->right, rbarrier);
    //    doatomicFence(funcsp, nullptr, node->left, lbarrier);
    return rv;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_void_(EXPRESSION* node, SYMBOL* funcsp)
{
    if (node->type != ExpressionNode::auto_)
        gen_expr(funcsp, node, F_NOVALUE | F_RETURNSTRUCTNOADJUST, natural_size(node));
    return 0;
}

/*-------------------------------------------------------------------------*/

int natural_size(EXPRESSION* node)
/*
 *      return the natural evaluation size of a node.
 */
{
    int siz0, siz1;
    if (node == 0)
    {
        return 0;
    }
    switch (node->type)
    {
        case ExpressionNode::paramsubstitute_:
            return node->v.imode->size;
        case ExpressionNode::sizeofellipse_:
            return ISZ_UINT;
        case ExpressionNode::pointsto_:
        case ExpressionNode::dot_:
            return natural_size(node->right);
        case ExpressionNode::thisshim_:
            return ISZ_ADDR;
        case ExpressionNode::msil_array_access_:
            return SizeFromType(node->v.msilArray->tp);
        case ExpressionNode::stmt_:
            return natural_size(node->left);
        case ExpressionNode::funcret_:
        case ExpressionNode::callsite_:
        case ExpressionNode::intcall_:
            while (node->type == ExpressionNode::funcret_)
                node = node->left;
            if (!node->v.func->functp || !node->v.func->functp->IsFunction())
                return 0;
            if (node->v.func->functp->BaseType()->btp->IsStructured() ||
                node->v.func->functp->BaseType()->btp->type == BasicType::memberptr_)
                return ISZ_ADDR;
            else if (node->v.func->ascall)
            {
                if (node->v.func->sp && node->v.func->sp->tp->IsFunction() && node->v.func->sp->tp->BaseType()->btp->IsArray())
                    return ISZ_OBJECT;
                return SizeFromType(node->v.func->functp->BaseType()->btp);
            }
            else
                return ISZ_ADDR;
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::nullptr_:
            return ISZ_ADDR;
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::initobj_:
        case ExpressionNode::type_:
            return ISZ_NONE;
        case ExpressionNode::bits_:
        case ExpressionNode::shiftby_:
            return natural_size(node->left);
            ;
        case ExpressionNode::c_c_:
        case ExpressionNode::l_c_:
        case ExpressionNode::x_c_:
            return -ISZ_UCHAR;
        case ExpressionNode::c_uc_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::x_uc_:
            return ISZ_UCHAR;
        case ExpressionNode::c_u16_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::x_u16_:
            return ISZ_U16;
        case ExpressionNode::c_u32_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::x_u32_:
            return ISZ_U32;
        case ExpressionNode::c_s_:
        case ExpressionNode::l_s_:
        case ExpressionNode::x_s_:
            return -ISZ_USHORT;
        case ExpressionNode::l_wc_:
        case ExpressionNode::x_wc_:
        case ExpressionNode::c_wc_:
        case ExpressionNode::c_us_:
        case ExpressionNode::l_us_:
        case ExpressionNode::x_us_:
            return ISZ_USHORT;
        case ExpressionNode::c_l_:
        case ExpressionNode::l_l_:
        case ExpressionNode::x_l_:
            return -ISZ_ULONG;
        case ExpressionNode::c_i_:
        case ExpressionNode::l_i_:
        case ExpressionNode::x_i_:
        case ExpressionNode::structelem_:
        case ExpressionNode::sizeof_:
            return -ISZ_UINT;
        case ExpressionNode::c_ul_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::x_ul_:
            return ISZ_ULONG;
        case ExpressionNode::c_ui_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::x_ui_:
            return ISZ_UINT;
        case ExpressionNode::c_bitint_:
        case ExpressionNode::l_bitint_:
        case ExpressionNode::x_bitint_:
            return -ISZ_BITINT;
        case ExpressionNode::c_ubitint_:
        case ExpressionNode::l_ubitint_:
        case ExpressionNode::x_ubitint_:
            return ISZ_BITINT;
        case ExpressionNode::x_inative_:
        case ExpressionNode::l_inative_:
            return -ISZ_UNATIVE;
        case ExpressionNode::x_unative_:
        case ExpressionNode::l_unative_:
            return ISZ_UNATIVE;
        case ExpressionNode::c_d_:
        case ExpressionNode::l_d_:
        case ExpressionNode::x_d_:
            return ISZ_DOUBLE;
        case ExpressionNode::c_ld_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::x_ld_:
            return ISZ_LDOUBLE;
        case ExpressionNode::l_f_:
        case ExpressionNode::c_f_:
        case ExpressionNode::x_f_:
            return ISZ_FLOAT;
        case ExpressionNode::l_fi_:
        case ExpressionNode::c_fi_:
        case ExpressionNode::x_fi_:
            return ISZ_IFLOAT;
        case ExpressionNode::l_di_:
        case ExpressionNode::c_di_:
        case ExpressionNode::x_di_:
            return ISZ_IDOUBLE;
        case ExpressionNode::l_ldi_:
        case ExpressionNode::c_ldi_:
        case ExpressionNode::x_ldi_:
            return ISZ_ILDOUBLE;
        case ExpressionNode::l_fc_:
        case ExpressionNode::c_fc_:
        case ExpressionNode::x_fc_:
            return ISZ_CFLOAT;
        case ExpressionNode::l_dc_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::x_dc_:
            return ISZ_CDOUBLE;
        case ExpressionNode::l_ldc_:
        case ExpressionNode::c_ldc_:
        case ExpressionNode::x_ldc_:
            return ISZ_CLDOUBLE;
        case ExpressionNode::c_bool_:
            return ISZ_BOOLEAN;
        case ExpressionNode::c_ull_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::x_ull_:
            return ISZ_ULONGLONG;
        case ExpressionNode::c_ll_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::x_ll_:
            return -ISZ_ULONGLONG;
        case ExpressionNode::imode_:
            return ISZ_ADDR;
        case ExpressionNode::c_string_:
        case ExpressionNode::l_string_:
        case ExpressionNode::x_string_:
            return ISZ_STRING;
        case ExpressionNode::l_object_:
        case ExpressionNode::x_object_:
            return ISZ_OBJECT;
        case ExpressionNode::trapcall_:
        case ExpressionNode::auto_:
        case ExpressionNode::pc_:
        case ExpressionNode::threadlocal_:
        case ExpressionNode::global_:
        case ExpressionNode::absolute_:
        case ExpressionNode::labcon_:
            return ISZ_ADDR;
        case ExpressionNode::l_p_:
        case ExpressionNode::x_p_:
        case ExpressionNode::l_ref_:
            return ISZ_ADDR;
        case ExpressionNode::x_fp_:
        case ExpressionNode::l_fp_:
            return ISZ_FARPTR;
        case ExpressionNode::x_sp_:
        case ExpressionNode::l_sp_:
            return ISZ_SEG;
        case ExpressionNode::l_bool_:
        case ExpressionNode::x_bool_:
            return ISZ_BOOLEAN;
        case ExpressionNode::l_bit_:
        case ExpressionNode::x_bit_:
            return ISZ_BIT;
        case ExpressionNode::compl_:
        case ExpressionNode::uminus_:
        case ExpressionNode::assign_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::stackblock_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
            return natural_size(node->left);
        case ExpressionNode::arrayadd_:
        case ExpressionNode::structadd_:
        case ExpressionNode::add_:
        case ExpressionNode::sub_:
        case ExpressionNode::umul_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::arraymul_:
        case ExpressionNode::mul_:
        case ExpressionNode::div_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::mod_:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
            siz0 = natural_size(node->left);
            siz1 = natural_size(node->right);
            if (chksize(siz1, siz0))
                return siz1;
            else
                return siz0;
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::ursh_:
        case ExpressionNode::argnopush_:
        case ExpressionNode::thisref_:
        case ExpressionNode::select_:
        case ExpressionNode::constexprconstructor_:
            return natural_size(node->left);
            /*		case ExpressionNode::array_:
                        return ISZ_ADDR ;
            */
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::land_:
        case ExpressionNode::lor_:
        case ExpressionNode::not_:
        case ExpressionNode::mp_compare_:
        case ExpressionNode::mp_as_bool_:
            return -ISZ_UINT;
        case ExpressionNode::comma_:
            while (node->type == ExpressionNode::comma_ && node->right)
                node = node->right;
            if (node->type == ExpressionNode::comma_)
                return 0;
            else
                return natural_size(node);
        case ExpressionNode::hook_:
            return natural_size(node->right);
        case ExpressionNode::atomic_:
            return -ISZ_UINT;
        case ExpressionNode::check_nz_:
            return natural_size(node->right);
        case ExpressionNode::const_:
            return SizeFromType(node->v.sp->tp);
        case ExpressionNode::templateparam_:  // may get this during the initial parsing of the template
        case ExpressionNode::templateselector_:
            return -ISZ_UINT;
        case ExpressionNode::cvarpointer_:
            return ISZ_UINT;
        default:
            diag("natural size error.");
            break;
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

void gen_compare(EXPRESSION* node, SYMBOL* funcsp, Optimizer::i_ops btype, int label)
/*
 *      generate code to do a comparison of the two operands of
 *      node.
 */
{
    if (bitintbits(node->left))
    {
        bitint_compare(node, btype, label);
        return;
    }
    Optimizer::IMODE *ap1, *ap2, *ap3;
    Optimizer::IMODE* barrier;
    int siz0, siz1, size;
    siz0 = natural_size(node->left);
    siz1 = natural_size(node->right);
    if (chksize(siz1, siz0))
        size = siz1;
    else
        size = siz0;
    // the ordering here is to accomodate the x86 FP stack
    if (Optimizer::chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        ap3 = gen_expr(funcsp, node->left, F_COMPARE | F_OBJECT, size);
    else
        ap3 = gen_expr(funcsp, node->right, F_COMPARE | F_OBJECT, size);
    ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
    {
        //        if (node->right->isatomic)
        //        {
        //            barrier = doatomicFence(funcsp, nullptr, node->right, nullptr);
        //        }
        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
        //        if (node->right->isatomic)
        //        {
        //            doatomicFence(funcsp, nullptr, node->right, barrier);
        //        }
    }
    if (Optimizer::architecture == ARCHITECTURE_MSIL && ap2->size == ISZ_ADDR)
    {
        ap3 = Optimizer::tempreg(-ISZ_UINT, 0);
        Optimizer::gen_icode(Optimizer::i_assn, ap3, ap2, nullptr);
        ap2 = ap3;
    }
    if (Optimizer::chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        ap3 = gen_expr(funcsp, node->right, F_COMPARE, size);
    else
        ap3 = gen_expr(funcsp, node->left, F_COMPARE, size);
    ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
    {
        //        if (node->left->isatomic)
        //        {
        //            barrier = doatomicFence(funcsp, nullptr, node->left, nullptr);
        //        }
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
        //        if (node->left->isatomic)
        //        {
        //            doatomicFence(funcsp, nullptr, node->left, barrier);
        //        }
    }
    if (Optimizer::architecture == ARCHITECTURE_MSIL && ap1->size == ISZ_ADDR)
    {
        ap3 = Optimizer::tempreg(-ISZ_UINT, 0);
        Optimizer::gen_icode(Optimizer::i_assn, ap3, ap1, nullptr);
        ap1 = ap3;
    }
    if (incdecList)
    {
        // this is needed for the optimizer...  the incdec confuses it
        if (ap1->mode != Optimizer::i_immed)
        {
            ap3 = Optimizer::tempreg(ap1->size, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap1, nullptr);
            ap1 = ap3;
        }
        if (ap2->mode != Optimizer::i_immed)
        {
            ap3 = Optimizer::tempreg(ap2->size, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap2, nullptr);
            ap2 = ap3;
        }
        DumpIncDec(funcsp);
    }
    if (Optimizer::chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        Optimizer::gen_icgoto(btype, label, ap2, ap1);
    else
        Optimizer::gen_icgoto(btype, label, ap1, ap2);
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_set(EXPRESSION* node, SYMBOL* funcsp, Optimizer::i_ops btype)
{
    if (bitintbits(node->left))
    {
        return bitint_compare(node, btype, -1);
    }
    Optimizer::IMODE *ap1, *ap2, *ap3;
    int siz0, siz1, size;
    siz0 = natural_size(node->left);
    siz1 = natural_size(node->right);
    if (chksize(siz1, siz0))
        size = siz1;
    else
        size = siz0;
    ap3 = gen_expr(funcsp, node->left, 0, size);
    ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
    if (Optimizer::architecture == ARCHITECTURE_MSIL && ap1->size == ISZ_ADDR)
    {
        ap3 = Optimizer::tempreg(-ISZ_UINT, 0);
        Optimizer::gen_icode(Optimizer::i_assn, ap3, ap1, nullptr);
        ap1 = ap3;
    }
    ap3 = gen_expr(funcsp, node->right, 0, size);
    ap2 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap2, ap3, nullptr);
    if (Optimizer::architecture == ARCHITECTURE_MSIL && ap2->size == ISZ_ADDR)
    {
        ap3 = Optimizer::tempreg(-ISZ_UINT, 0);
        Optimizer::gen_icode(Optimizer::i_assn, ap3, ap2, nullptr);
        ap2 = ap3;
    }
    ap3 = Optimizer::tempreg(ISZ_UINT, 0);
    Optimizer::gen_icode(btype, ap3, ap1, ap2);

    return ap3;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* defcond(EXPRESSION* node, SYMBOL* funcsp)
{
    if (bitintbits(node->left))
    {
        return bitint_iszero(node->left, true, -1);
    }
    Optimizer::IMODE *ap1, *ap2, *ap3;
    int size = natural_size(node);
    ap3 = gen_expr(funcsp, node->left, 0, size);
    ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
    ap2 = Optimizer::make_immed(ap1->size, 0);
    ap3 = Optimizer::tempreg(ISZ_UINT, 0);
    Optimizer::gen_icode(Optimizer::i_sete, ap3, ap1, ap2);
    return ap3;
}

/*-------------------------------------------------------------------------*/

static Optimizer::IMODE* truerelat(EXPRESSION* node, SYMBOL* funcsp)
{
    Optimizer::IMODE *ap1 = nullptr, *ap3;
    int lab0, lab1;
    int siz1, siz2;
    if (node == 0)
        return 0;
    switch (node->type)
    {
        case ExpressionNode::eq_:
            ap1 = gen_set(node, funcsp, Optimizer::i_sete);
            break;
        case ExpressionNode::ne_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setne);
            break;
        case ExpressionNode::lt_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setl);
            break;
        case ExpressionNode::le_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setle);
            break;
        case ExpressionNode::gt_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setg);
            break;
        case ExpressionNode::ge_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setge);
            break;
        case ExpressionNode::ult_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setc);
            break;
        case ExpressionNode::ule_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setbe);
            break;
        case ExpressionNode::ugt_:
            ap1 = gen_set(node, funcsp, Optimizer::i_seta);
            break;
        case ExpressionNode::uge_:
            ap1 = gen_set(node, funcsp, Optimizer::i_setnc);
            break;
        case ExpressionNode::not_:
            ap1 = defcond(node, funcsp);
            break;
        case ExpressionNode::mp_compare_:
        case ExpressionNode::mp_as_bool_:
            ap1 = Optimizer::tempreg(ISZ_UINT, 0);
            lab0 = Optimizer::nextLabel++;
            lab1 = Optimizer::nextLabel++;
            truejmp(node, funcsp, lab0);
            ap3 = Optimizer::make_immed(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            Optimizer::intermed_tail->hook = true;
            Optimizer::gen_igoto(Optimizer::i_goto, lab1);
            Optimizer::gen_label(lab0);
            ap3 = Optimizer::make_immed(ISZ_UINT, 1);
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            Optimizer::intermed_tail->hook = true;
            Optimizer::gen_label(lab1);
            break;
        default:
            diag("True-relat error");
            break;
    }
    //    ap1->size = ISZ_UCHAR;
    return ap1;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* gen_relat(EXPRESSION* node, SYMBOL* funcsp)
{
    long lab1, lab2;
    Optimizer::IMODE* ap1;
    if (node->type != ExpressionNode::land_ && node->type != ExpressionNode::lor_)
    {
        ap1 = truerelat(node, funcsp);
    }
    else
    {
        ap1 = Optimizer::tempreg(ISZ_UINT, 0);
        ap1->offset->sp->pushedtotemp = true;  // don't lazily optimize this temp...
        lab1 = Optimizer::nextLabel++, lab2 = Optimizer::nextLabel++;
        truejmp(node, funcsp, lab1);
        Optimizer::gen_iiconst(ap1, 0);
        Optimizer::intermed_tail->hook = true;
        Optimizer::gen_igoto(Optimizer::i_goto, lab2);
        Optimizer::gen_label(lab1);
        Optimizer::gen_iiconst(ap1, 1);
        Optimizer::intermed_tail->hook = true;
        Optimizer::gen_label(lab2);
    }
    return ap1;
}

/*-------------------------------------------------------------------------*/
static void DoInit(Optimizer::IMODE* loadVar, Optimizer::IMODE* var, int val)
{
    var->offset->sp->pushedtotemp = true;
    gen_icode(Optimizer::i_assn, loadVar, Optimizer::make_immed(ISZ_UINT, val), nullptr);
    gen_icode(Optimizer::i_assn, var, loadVar, nullptr);
}
static void DoOr(Optimizer::IMODE* loadVar, Optimizer::IMODE* var, int val)
{
    gen_icode(Optimizer::i_assn, loadVar, var, nullptr);
    gen_icode(Optimizer::i_or, loadVar, loadVar, Optimizer::make_immed(ISZ_UINT, val));
    gen_icode(Optimizer::i_assn, var, loadVar, nullptr);
}
static void DoTest(Optimizer::IMODE* var, int lab, int val)
{
    Optimizer::IMODE* loadVar = Optimizer::tempreg(var->size, 0);
    gen_icode(Optimizer::i_assn, loadVar, var, nullptr);
    gen_icode(Optimizer::i_and, loadVar, loadVar, Optimizer::make_immed(ISZ_UCHAR, val));
    gen_icgoto(Optimizer::i_je, lab, loadVar, Optimizer::make_immed(ISZ_UCHAR, 0));
}
void truejmp(EXPRESSION* node, SYMBOL* funcsp, int label)
/*
 *      generate a jump to label if the node passed evaluates to
 *      a true condition.
 */
{
    Optimizer::IMODE *ap1, *ap2, *ap3, *ap4;
    int siz1, siz2;
    int lab0, lab1, lab2;
    int i;
    if (node == 0)
        return;
    switch (node->type)
    {
        case ExpressionNode::eq_:
            gen_compare(node, funcsp, Optimizer::i_je, label);
            break;
        case ExpressionNode::ne_:
            gen_compare(node, funcsp, Optimizer::i_jne, label);
            break;
        case ExpressionNode::lt_:
            gen_compare(node, funcsp, Optimizer::i_jl, label);
            break;
        case ExpressionNode::le_:
            gen_compare(node, funcsp, Optimizer::i_jle, label);
            break;
        case ExpressionNode::gt_:
            gen_compare(node, funcsp, Optimizer::i_jg, label);
            break;
        case ExpressionNode::ge_:
            gen_compare(node, funcsp, Optimizer::i_jge, label);
            break;
        case ExpressionNode::ult_:
            gen_compare(node, funcsp, Optimizer::i_jc, label);
            break;
        case ExpressionNode::ule_:
            gen_compare(node, funcsp, Optimizer::i_jbe, label);
            break;
        case ExpressionNode::ugt_:
            gen_compare(node, funcsp, Optimizer::i_ja, label);
            break;
        case ExpressionNode::uge_:
            gen_compare(node, funcsp, Optimizer::i_jnc, label);
            break;
        case ExpressionNode::select_:
            if (node->v.logicaldestructors.left)
            {
                siz1 = natural_size(node);
                ap3 = gen_expr(funcsp, node->left, F_COMPARE, siz1);
                ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
                if (ap1 != ap3)
                    Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
                DumpIncDec(funcsp);
                DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
                Optimizer::gen_icgoto(Optimizer::i_jne, label, ap1, Optimizer::make_immed(ap1->size, 0));
            }
            else
            {
                truejmp(node->left, funcsp, label);
            }
            break;
        case ExpressionNode::land_:
            if (node->v.logicaldestructors.left || node->v.logicaldestructors.right)
            {
                auto loadVar = Optimizer::tempreg(ISZ_UCHAR, 0);
                auto hold = Optimizer::tempreg(ISZ_UCHAR, 0);
                DoInit(loadVar, hold, 0);
                lab0 = Optimizer::nextLabel++;
                lab1 = Optimizer::nextLabel++;
                lab2 = Optimizer::nextLabel++;
                falsejmp(node->left, funcsp, lab0);
                if (node->v.logicaldestructors.right)
                {
                    DoOr(loadVar, hold, 2);
                }
                truejmp(node->right, funcsp, lab1);

                Optimizer::gen_label(lab0);
                // acceptable
                DoOr(loadVar, hold, 1);
                Optimizer::gen_label(lab1);
                if (node->v.logicaldestructors.left)
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
                if (node->v.logicaldestructors.right)
                {
                    DoTest(hold, lab2, 2);
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.right);
                    Optimizer::gen_label(lab2);
                }
                DoTest(hold, label, 1);
            }
            else
            {
                lab0 = Optimizer::nextLabel++;
                falsejmp(node->left, funcsp, lab0);
                truejmp(node->right, funcsp, label);
                Optimizer::gen_label(lab0);
            }
            break;
        case ExpressionNode::lor_:
            if (node->v.logicaldestructors.left || node->v.logicaldestructors.right)
            {
                auto loadVar = Optimizer::tempreg(ISZ_UCHAR, 0);
                auto hold = Optimizer::tempreg(ISZ_UCHAR, 0);
                DoInit(loadVar, hold, 0);
                lab0 = Optimizer::nextLabel++;
                lab1 = Optimizer::nextLabel++;
                lab2 = Optimizer::nextLabel++;
                truejmp(node->left, funcsp, lab0);
                if (node->v.logicaldestructors.right)
                {
                    DoOr(loadVar, hold, 2);
                }
                truejmp(node->right, funcsp, lab0);
                // acceptable
                DoOr(loadVar, hold, 1);
                Optimizer::gen_label(lab0);
                if (node->v.logicaldestructors.left)
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
                if (node->v.logicaldestructors.right)
                {
                    DoTest(hold, lab2, 2);
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.right);
                    Optimizer::gen_label(lab2);
                }
                DoTest(hold, label, 1);
            }
            else
            {
                truejmp(node->left, funcsp, label);
                truejmp(node->right, funcsp, label);
            }
            break;
        case ExpressionNode::not_:
            falsejmp(node->left, funcsp, label);
            break;
        case ExpressionNode::mp_compare_:
            lab0 = Optimizer::nextLabel++;
            siz1 = node->size->size;
            siz2 = getSize(BasicType::int_);
            ap2 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            ap3 = Optimizer::tempreg(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap2, 0);
            ap2 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
            ap1 = Optimizer::tempreg(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, 0);
            DumpIncDec(funcsp);
            for (i = 0; i < siz1 / siz2 - 1; i++)
            {
                ap4 = Optimizer::indnode(ap3, ap3->size);
                ap2 = Optimizer::indnode(ap1, ap1->size);
                Optimizer::gen_icgoto(Optimizer::i_jne, lab0, ap4, ap2);
                Optimizer::IMODE* ap5 = Optimizer::tempreg(ap3->size, 0);
                Optimizer::IMODE* ap6 = Optimizer::tempreg(ap1->size, 0);
                Optimizer::gen_icode(Optimizer::i_add, ap5, ap3, Optimizer::make_immed(ap3->size, siz2));
                Optimizer::gen_icode(Optimizer::i_add, ap6, ap1, Optimizer::make_immed(ap3->size, siz2));
                ap3 = ap5;
                ap1 = ap6;
            }
            ap4 = Optimizer::indnode(ap3, ap3->size);
            ap2 = Optimizer::indnode(ap1, ap1->size);
            Optimizer::gen_icgoto(Optimizer::i_je, label, ap4, ap2);
            Optimizer::gen_label(lab0);
            break;
        case ExpressionNode::mp_as_bool_:
            siz1 = node->size->size;
            siz2 = getSize(BasicType::int_);
            ap2 = gen_expr(funcsp, node->left, F_ADDR, ISZ_UINT);
            ap3 = Optimizer::tempreg(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap2, 0);
            DumpIncDec(funcsp);
            for (i = 0; i < siz1 / siz2; i++)
            {
                ap2 = Optimizer::indnode(ap3, ap3->size);
                Optimizer::gen_icgoto(Optimizer::i_jne, label, ap2, Optimizer::make_immed(ap3->size, 0));
                if (i < siz1 / siz2 - 1)
                {
                    Optimizer::IMODE* ap5 = Optimizer::tempreg(ap3->size, 0);
                    Optimizer::gen_icode(Optimizer::i_add, ap5, ap3, Optimizer::make_immed(ap3->size, siz2));
                    ap3 = ap5;
                }
            }
            break;
        default:
            if (bitintbits(node))
            {
                bitint_iszero(node, false, label);
                return;
            }
            siz1 = natural_size(node);
            ap3 = gen_expr(funcsp, node, F_COMPARE, siz1);
            ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
            DumpIncDec(funcsp);
            Optimizer::gen_icgoto(Optimizer::i_jne, label, ap1, Optimizer::make_immed(ap1->size, 0));
            break;
    }
}

/*-------------------------------------------------------------------------*/

void falsejmp(EXPRESSION* node, SYMBOL* funcsp, int label)
/*
 *      generate code to execute a jump to label if the expression
 *      passed is false.
 */
{
    Optimizer::IMODE *ap, *ap1, *ap2, *ap3, *ap4;
    int siz1, siz2;
    int lab0, lab1, lab2;
    int i;
    if (node == 0)
        return;
    switch (node->type)
    {
        case ExpressionNode::eq_:
            gen_compare(node, funcsp, Optimizer::i_jne, label);
            break;
        case ExpressionNode::ne_:
            gen_compare(node, funcsp, Optimizer::i_je, label);
            break;
        case ExpressionNode::lt_:
            gen_compare(node, funcsp, Optimizer::i_jge, label);
            break;
        case ExpressionNode::le_:
            gen_compare(node, funcsp, Optimizer::i_jg, label);
            break;
        case ExpressionNode::gt_:
            gen_compare(node, funcsp, Optimizer::i_jle, label);
            break;
        case ExpressionNode::ge_:
            gen_compare(node, funcsp, Optimizer::i_jl, label);
            break;
        case ExpressionNode::ult_:
            gen_compare(node, funcsp, Optimizer::i_jnc, label);
            break;
        case ExpressionNode::ule_:
            gen_compare(node, funcsp, Optimizer::i_ja, label);
            break;
        case ExpressionNode::ugt_:
            gen_compare(node, funcsp, Optimizer::i_jbe, label);
            break;
        case ExpressionNode::uge_:
            gen_compare(node, funcsp, Optimizer::i_jc, label);
            break;
        case ExpressionNode::select_:
            if (node->v.logicaldestructors.left)
            {
                siz1 = natural_size(node);
                ap3 = gen_expr(funcsp, node->left, F_COMPARE, siz1);
                ap1 = Optimizer::LookupLoadTemp(nullptr, ap3);
                if (ap1 != ap3)
                    Optimizer::gen_icode(Optimizer::i_assn, ap1, ap3, nullptr);
                DumpIncDec(funcsp);
                DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
                Optimizer::gen_icgoto(Optimizer::i_je, label, ap1, Optimizer::make_immed(ap1->size, 0));
            }
            else
            {
                falsejmp(node->left, funcsp, label);
            }
            break;
        case ExpressionNode::land_:
            if (node->v.logicaldestructors.left || node->v.logicaldestructors.right)
            {
                auto loadVar = Optimizer::tempreg(ISZ_UCHAR, 0);
                auto hold = Optimizer::tempreg(ISZ_UCHAR, 0);
                DoInit(loadVar, hold, 0);
                lab0 = Optimizer::nextLabel++;
                lab1 = Optimizer::nextLabel++;
                lab2 = Optimizer::nextLabel++;
                falsejmp(node->left, funcsp, lab0);
                if (node->v.logicaldestructors.right)
                {
                    DoOr(loadVar, hold, 2);
                }
                falsejmp(node->right, funcsp, lab0);
                // acceptable
                DoOr(loadVar, hold, 1);
                Optimizer::gen_label(lab0);
                if (node->v.logicaldestructors.left)
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
                if (node->v.logicaldestructors.right)
                {
                    DoTest(hold, lab2, 2);
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.right);
                    Optimizer::gen_label(lab2);
                }
                DoTest(hold, label, 1);
            }
            else
            {
                falsejmp(node->left, funcsp, label);
                falsejmp(node->right, funcsp, label);
            }
            break;
        case ExpressionNode::lor_:
            if (node->v.logicaldestructors.left || node->v.logicaldestructors.right)
            {
                auto loadVar = Optimizer::tempreg(ISZ_UCHAR, 0);
                auto hold = Optimizer::tempreg(ISZ_UCHAR, 0);
                DoInit(loadVar, hold, 0);
                lab0 = Optimizer::nextLabel++;
                lab1 = Optimizer::nextLabel++;
                lab2 = Optimizer::nextLabel++;
                truejmp(node->left, funcsp, lab0);
                if (node->v.logicaldestructors.right)
                {
                    DoOr(loadVar, hold, 2);
                }
                falsejmp(node->right, funcsp, lab1);

                Optimizer::gen_label(lab0);
                // acceptable
                DoOr(loadVar, hold, 1);
                Optimizer::gen_label(lab1);
                if (node->v.logicaldestructors.left)
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.left);
                if (node->v.logicaldestructors.right)
                {
                    DoTest(hold, lab2, 2);
                    DumpLogicalDestructors(funcsp, node->v.logicaldestructors.right);
                    Optimizer::gen_label(lab2);
                }
                DoTest(hold, label, 1);
            }
            else
            {
                lab0 = Optimizer::nextLabel++;
                truejmp(node->left, funcsp, lab0);
                falsejmp(node->right, funcsp, label);
                Optimizer::gen_label(lab0);
            }
            break;
        case ExpressionNode::not_:
            truejmp(node->left, funcsp, label);
            break;
        case ExpressionNode::mp_compare_:
            siz1 = node->size->size;
            siz2 = getSize(BasicType::int_);
            ap2 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            ap3 = Optimizer::tempreg(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap2, 0);
            ap2 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
            ap1 = Optimizer::tempreg(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap1, ap2, 0);
            DumpIncDec(funcsp);
            for (i = 0; i < siz1 / siz2; i++)
            {
                ap4 = Optimizer::indnode(ap3, ap3->size);
                ap2 = Optimizer::indnode(ap1, ap1->size);
                Optimizer::gen_icgoto(Optimizer::i_jne, label, ap4, ap2);
                if (i < siz1 / siz2 - 1)
                {
                    Optimizer::IMODE* ap5 = Optimizer::tempreg(ap3->size, 0);
                    Optimizer::IMODE* ap6 = Optimizer::tempreg(ap1->size, 0);
                    Optimizer::gen_icode(Optimizer::i_add, ap5, ap3, Optimizer::make_immed(ap3->size, siz2));
                    Optimizer::gen_icode(Optimizer::i_add, ap6, ap1, Optimizer::make_immed(ap3->size, siz2));
                    ap3 = ap5;
                    ap1 = ap6;
                }
            }
            break;
        case ExpressionNode::mp_as_bool_:
            lab0 = Optimizer::nextLabel++;
            siz1 = node->size->size;
            siz2 = getSize(BasicType::int_);
            ap2 = gen_expr(funcsp, node->left, F_ADDR, ISZ_UINT);
            ap3 = Optimizer::tempreg(ISZ_UINT, 0);
            Optimizer::gen_icode(Optimizer::i_assn, ap3, ap2, 0);
            DumpIncDec(funcsp);
            for (i = 0; i < siz1 / siz2 - 1; i++)
            {
                ap2 = Optimizer::indnode(ap3, ap3->size);
                Optimizer::gen_icgoto(Optimizer::i_jne, lab0, ap2, Optimizer::make_immed(ap3->size, 0));
                Optimizer::IMODE* ap5 = Optimizer::tempreg(ap3->size, 0);
                Optimizer::gen_icode(Optimizer::i_add, ap5, ap3, Optimizer::make_immed(ap3->size, siz2));
                ap3 = ap5;
            }
            ap4 = Optimizer::indnode(ap3, ap3->size);
            Optimizer::gen_icgoto(Optimizer::i_je, label, ap4, Optimizer::make_immed(ap3->size, 0));
            Optimizer::gen_label(lab0);
            break;
        default:
            if (bitintbits(node))
            {
                bitint_iszero(node, true, label);
                return;
            }
            siz1 = natural_size(node);
            ap3 = gen_expr(funcsp, node, F_COMPARE, siz1);
            ap = Optimizer::LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
                Optimizer::gen_icode(Optimizer::i_assn, ap, ap3, nullptr);
            DumpIncDec(funcsp);
            Optimizer::gen_icgoto(Optimizer::i_je, label, ap, Optimizer::make_immed(ap->size, 0));
            break;
    }
}
}  // namespace Parser
