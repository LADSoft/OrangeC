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
 * this module combines constants at compile time.  It is used e.g.
 * for evaluating static assignments, but an initial folding of variable
 * expressions is done as well to slightly improve the code generation
 */
#include <cstdio>
#include <cstring>
#include <cfloat>
#include "compiler.h"
#include "ppPragma.h"

#include "config.h"
#include "initbackend.h"
#include "init.h"
#include "declare.h"
#include "template.h"
#include "floatconv.h"
#include "memory.h"
#include "help.h"
#include "expr.h"
#include "ifloatconv.h"
#include "constopt.h"
#include "OptUtils.h"
#include "declcpp.h"
#include "cpplookup.h"
#include "beinterf.h"
#include "exprcpp.h"
#include "lex.h"
#include "dsw.h"
#include "constexpr.h"
#include "ccerr.h"
#include "rtti.h"
#include "symtab.h"

namespace Parser
{
unsigned long long reint(EXPRESSION* node);

int inConstantExpression;
static EXPRESSION *asidehead, **asidetail;
static unsigned long long shifts[sizeof(long long) * 8];
static int optimizerfloatconst(EXPRESSION* en)
{
    if ((en->pragmas & STD_PRAGMA_FENV) && !initializingGlobalVar)
        return false;

    return isfloatconst(en) || isimaginaryconst(en) || iscomplexconst(en);
}

/*-------------------------------------------------------------------------*/

static int isoptconst(EXPRESSION* en)
{
    if (!en)
        return false;
    return isintconst(en) || optimizerfloatconst(en);
}

/*-------------------------------------------------------------------------*/

static ExpressionNode maxinttype(EXPRESSION* ep1, EXPRESSION* ep2)
{
    ExpressionNode type1 = ep1->type;
    ExpressionNode type2 = ep2->type;
    if (type1 == ExpressionNode::c_ull || type2 == ExpressionNode::c_ull)
        return ExpressionNode::c_ull;
    if (type1 == ExpressionNode::c_ll || type2 == ExpressionNode::c_ll)
        return ExpressionNode::c_ll;
    if (type1 == ExpressionNode::c_ul || type2 == ExpressionNode::c_ul)
        return ExpressionNode::c_ul;
    if (type1 == ExpressionNode::c_l || type2 == ExpressionNode::c_l)
        return ExpressionNode::c_l;
    if (type1 == ExpressionNode::c_u32 || type2 == ExpressionNode::c_u32)
        return ExpressionNode::c_u32;
    if (type1 == ExpressionNode::c_ui || type2 == ExpressionNode::c_ui)
        return ExpressionNode::c_ui;
    if (type1 == ExpressionNode::c_i || type2 == ExpressionNode::c_i)
        return ExpressionNode::c_i;
    if (type1 == ExpressionNode::c_u16 || type2 == ExpressionNode::c_u16)
        return ExpressionNode::c_u16;
    if (type1 == ExpressionNode::c_wc || type2 == ExpressionNode::c_wc)
        return ExpressionNode::c_wc;
    if (type1 == ExpressionNode::c_uc || type2 == ExpressionNode::c_uc)
        return ExpressionNode::c_uc;
    if (type1 == ExpressionNode::c_c || type2 == ExpressionNode::c_c)
        return ExpressionNode::c_c;
    return ExpressionNode::c_bool;
}

/*-------------------------------------------------------------------------*/

static int isunsignedexpr(EXPRESSION* ep1)
{
    switch (ep1->type)
    {
        case ExpressionNode::c_ul:
        case ExpressionNode::c_ui:
        case ExpressionNode::c_ull:
            return true;
        default:
            return false;
    }
}

/*-------------------------------------------------------------------------*/

static ExpressionNode maxfloattype(EXPRESSION* ep1, EXPRESSION* ep2)
{
    ExpressionNode type1 = ep1->type;
    ExpressionNode type2 = ep2->type;
    if (type1 == ExpressionNode::c_ld || type2 == ExpressionNode::c_ld || type1 == ExpressionNode::c_ldi || type2 == ExpressionNode::c_ldi)
        return ExpressionNode::c_ld;
    if (type1 == ExpressionNode::c_d || type2 == ExpressionNode::c_d || type1 == ExpressionNode::c_di || type2 == ExpressionNode::c_di)
        return ExpressionNode::c_d;
    return ExpressionNode::c_f;
}

/*-------------------------------------------------------------------------*/

static ExpressionNode maximaginarytype(EXPRESSION* ep1, EXPRESSION* ep2)
{
    ExpressionNode type1 = ep1->type;
    ExpressionNode type2 = ep2->type;
    if (type1 == ExpressionNode::c_ld || type2 == ExpressionNode::c_ld || type1 == ExpressionNode::c_ldi || type2 == ExpressionNode::c_ldi)
        return ExpressionNode::c_ldi;
    if (type1 == ExpressionNode::c_d || type2 == ExpressionNode::c_d || type1 == ExpressionNode::c_di || type2 == ExpressionNode::c_di)
        return ExpressionNode::c_di;
    return ExpressionNode::c_fi;
}

/*-------------------------------------------------------------------------*/

static ExpressionNode maxcomplextype(EXPRESSION* ep1, EXPRESSION* ep2)
{
    ExpressionNode type1 = ep1->type;
    ExpressionNode type2 = ep2->type;
    if (type1 == ExpressionNode::c_ld || type2 == ExpressionNode::c_ld || type1 == ExpressionNode::c_ldi || type2 == ExpressionNode::c_ldi || type1 == ExpressionNode::c_ldc || type2 == ExpressionNode::c_ldc)
        return ExpressionNode::c_ldc;
    if (type1 == ExpressionNode::c_d || type2 == ExpressionNode::c_d || type1 == ExpressionNode::c_di || type2 == ExpressionNode::c_di || type1 == ExpressionNode::c_dc || type2 == ExpressionNode::c_dc)
        return ExpressionNode::c_dc;
    return ExpressionNode::c_fc;
}
static bool hasFloats(EXPRESSION* node)
/*
 * Go through a node and see if it will be promoted to type FLOAT
 */
{
    if (!node)
        return 0;
    switch (node->type)
    {
        case ExpressionNode::x_f:
        case ExpressionNode::x_d:
        case ExpressionNode::x_ld:
        case ExpressionNode::x_fc:
        case ExpressionNode::x_dc:
        case ExpressionNode::x_ldc:
        case ExpressionNode::x_fi:
        case ExpressionNode::x_di:
        case ExpressionNode::x_ldi:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::l_fi:
        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::c_f:
        case ExpressionNode::c_d:
        case ExpressionNode::c_ld:
        case ExpressionNode::c_fc:
        case ExpressionNode::c_dc:
        case ExpressionNode::c_ldc:
        case ExpressionNode::c_fi:
        case ExpressionNode::c_di:
        case ExpressionNode::c_ldi:
            return 1;
        case ExpressionNode::assign:
        case ExpressionNode::auto_inc:
        case ExpressionNode::auto_dec:
        case ExpressionNode::add:
        case ExpressionNode::sub:
        case ExpressionNode::arrayadd:
        case ExpressionNode::arraymul:
        case ExpressionNode::arraydiv:
        case ExpressionNode::structadd:
        case ExpressionNode::umul:
        case ExpressionNode::udiv:
        case ExpressionNode::umod:
        case ExpressionNode::mul:
        case ExpressionNode::div:
        case ExpressionNode::mod:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::ursh:
        case ExpressionNode::lsh:
        case ExpressionNode::arraylsh:
        case ExpressionNode::rsh:
        case ExpressionNode::land:
        case ExpressionNode::lor:
        case ExpressionNode::void_:
        case ExpressionNode::void_nz:
        case ExpressionNode::dot:
        case ExpressionNode::pointsto:
            return (hasFloats(node->left) || hasFloats(node->right));
        case ExpressionNode::cond:
            return hasFloats(node->right);
        case ExpressionNode::argnopush:
        case ExpressionNode::not__lvalue:
        case ExpressionNode::thisref:
        case ExpressionNode::literalclass:
        case ExpressionNode::lvalue:
        case ExpressionNode::select:
            return hasFloats(node->left);
        default:
            return 0;
    }
}
/*-------------------------------------------------------------------------*/

static int getmode(EXPRESSION* ep1, EXPRESSION* ep2)
/*
 * get the constant mode of a pair of nodes
 * 0 = Neither node is a constant
 * 1 = icon,icon
 * 2 = icon,rcon
 * 3 = rcon,icon
 * 4 = rcon,rcon
 * 5 = icon,nothing
 * 6 = rcon,nothing
 * 7 = nothing,icon
 * 8 = nothing,rcon
 * 9 = rcon, ricon
 * 10 = ricon, rcon
 * 11 = ricon, ricon
 * 12 = ricon, nothing
 * 13 = nothing, ricon
 * 14 = ricon, icon
 * 15 = icon, ricon
 * 16 = ricon, cmplxcon
 * 17 = cmplxcon, ricon
 * 18 = rcon, cmplxcon
 * 19 = cmplxcon, rcon
 * 20 = cmplxcon, cmplxcon
 * 21 = cmplxcon, nothing
 * 22 = nothing, cmplxcon
 * 23 = cmplxcon, icon
 * 24 = icon, cmplxcon
 */
{
    int mode = 0;
    if (!initializingGlobalVar && (ep1->pragmas & STD_PRAGMA_FENV) &&
        (isfloatconst(ep1) || isimaginaryconst(ep1) || iscomplexconst(ep1)))
        return 0;
    if (!initializingGlobalVar && ep2 && (ep2->pragmas & STD_PRAGMA_FENV) &&
        (isfloatconst(ep2) || isimaginaryconst(ep2) || iscomplexconst(ep2)))
        return 0;
    if (isintconst(ep1))
        if (ep2)
        {
            if (isintconst(ep2))
                mode = 1;
            else if (isfloatconst(ep2))
            {
                mode = 2;
            }
            else if (isimaginaryconst(ep2))
                mode = 15;
            else if (iscomplexconst(ep2))
                mode = 24;
            else
                mode = 5;
        }
        else
            mode = 5;
    else if (isfloatconst(ep1))
        if (ep2)
        {
            if (isintconst(ep2))
                mode = 3;
            else if (isfloatconst(ep2))
                mode = 4;
            else if (isimaginaryconst(ep2))
                mode = 9;
            else if (iscomplexconst(ep2))
                mode = 18;
            else
                mode = 6;
        }
        else
            mode = 6;
    else if (isimaginaryconst(ep1))
        if (ep2)
        {
            if (isfloatconst(ep2))
                mode = 10;
            else if (isimaginaryconst(ep2))
                mode = 11;
            else if (iscomplexconst(ep2))
                mode = 16;
            else if (isintconst(ep2))
                mode = 14;
            else
                mode = 12;
        }
        else
            mode = 12;
    else if (iscomplexconst(ep1))
        if (ep2)
        {
            if (isfloatconst(ep2))
                mode = 17;
            else if (isimaginaryconst(ep2))
                mode = 19;
            else if (iscomplexconst(ep2))
                mode = 20;
            else if (isintconst(ep2))
                mode = 23;
            else
                mode = 21;
        }
        else
            mode = 21;
    else if (ep2)
    {
        if (isintconst(ep2))
            mode = 7;
        else if (ep2->type == ExpressionNode::c_d || ep2->type == ExpressionNode::c_ld || ep2->type == ExpressionNode::c_f)
            mode = 8;
        else if (ep2->type == ExpressionNode::c_di || ep2->type == ExpressionNode::c_fi || ep2->type == ExpressionNode::c_ldi)
            mode = 13;
        else if (ep2->type == ExpressionNode::c_dc || ep2->type == ExpressionNode::c_fc || ep2->type == ExpressionNode::c_ldc)
            mode = 22;
    }
    return (mode);
}
long long MaxOut(BasicType size, long long value)
{
    int bits;
    int plus, minus;
    switch (size)
    {
        case BasicType::char_:
            if (Optimizer::cparams.prm_charisunsigned)
                bits = 8;
            else
                bits = 7;
            break;
        case BasicType::signed_char:
            bits = 7;
            break;
        case BasicType::unsigned_char:
            bits = 8;
            break;
        case BasicType::short_:
            bits = getSize(BasicType::short_) * 8 - 1;
            break;
        case BasicType::unsigned_short:
        case BasicType::wchar_t_:
            bits = getSize(BasicType::short_) * 8;
            break;
        default:
            return value;
    }
    plus = Optimizer::mod_mask(bits);
    minus = -plus;
    if (value < minus)
        value = minus;
    if (value > plus)
        value = plus;
    return value;
}
/*-------------------------------------------------------------------------*/

void dooper(EXPRESSION** node, int mode)
/*
 *      dooper will execute a constant operation in a node and
 *      modify the node to be the result of the operation.
 *			It will also cast integers to floating point values when
 *			necessary
 */
{
    EXPRESSION *ep, *ep1, *ep2;
    FPF temp;
    ep = *node;
    ep1 = ep->left;
    ep2 = ep->right;
    if (mode == 5)
    {
        if (hasFloats(ep2))
        {
            if (isunsignedexpr(ep1))
                (*ep1->v.f) = (unsigned long long)ep1->v.i;
            else
                (*ep1->v.f) = (long long)ep1->v.i;
            ep1->type = ExpressionNode::c_d;
            refloat(ep1);
        }
        return;
    }
    else if (mode == 7)
    {
        if (hasFloats(ep1))
        {
            auto f = Allocate<FPF>();
            if (isunsignedexpr(ep2))
                *f = (unsigned long long)ep2->v.i;
            else
                *f = (long long)ep2->v.i;
            ep2->type = ExpressionNode::c_d;
            ep2->v.f = f;
            refloat(ep2);
        }
        return;
    }
    else if (mode == 6 || mode == 12 || mode == 13 || mode == 21 || mode == 22)
        return;
    else
        switch (ep->type)
        {
            case ExpressionNode::structadd:
                if (ep->right->type == ExpressionNode::structelem)
                    break;
                if (ep->right->keepZero)
                    break;
            case ExpressionNode::add:
            case ExpressionNode::arrayadd:
                switch (mode)
                {
                    case 1:
                        ep->type = maxinttype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.i = ep1->v.i + ep2->v.i;
                        ep->v.i = reint(ep);
                        break;
                    case 2:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = temp + (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 3:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) + temp;
                        refloat(ep);
                        break;
                    case 4:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) + (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 9:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep1->v.f);
                        ep->v.c->i = (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 10:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep2->v.f);
                        ep->v.c->i = (*ep1->v.f);
                        refloat(ep);
                        break;
                    case 11:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) + (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 14:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (long long)ep2->v.i;
                        ep->v.c->i = (*ep1->v.f);
                        refloat(ep);
                        break;
                    case 15:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (long long)ep1->v.i;
                        ep->v.c->i = (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 16:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep2->v.c->r;
                        ep->v.c->i = (*ep1->v.f) + ep2->v.c->i;
                        refloat(ep);
                        break;
                    case 17:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r;
                        ep->v.c->i = (*ep2->v.f) + ep1->v.c->i;
                        refloat(ep);
                        break;
                    case 18:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep1->v.f) + ep2->v.c->r;
                        ep->v.c->i = ep2->v.c->i;
                        refloat(ep);
                        break;
                    case 19:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep2->v.f) + ep1->v.c->r;
                        ep->v.c->i = ep1->v.c->i;
                        refloat(ep);
                        break;
                    case 20:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r + ep2->v.c->r;
                        ep->v.c->i = ep1->v.c->i + ep2->v.c->i;
                        refloat(ep);
                        break;
                    case 23:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r + temp;
                        ep->v.c->i = ep1->v.c->i;
                        refloat(ep);
                        break;
                    case 24:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep2->v.c->r + temp;
                        ep->v.c->i = ep2->v.c->i;
                        refloat(ep);
                        break;
                    default:
                        break;
                }
                break;
            case ExpressionNode::sub:
                switch (mode)
                {
                    case 1:
                        ep->type = maxinttype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.i = ep1->v.i - ep2->v.i;
                        ep->v.i = reint(ep);
                        break;
                    case 2:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = temp - (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 3:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) - temp;
                        refloat(ep);
                        break;
                    case 4:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) - (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 9:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep1->v.f);
                        ep->v.c->i = (*ep2->v.f);
                        ep->v.c->i.Negate();
                        refloat(ep);
                        break;
                    case 10:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep2->v.f);
                        ep->v.c->r.Negate();
                        ep->v.c->i = (*ep1->v.f);
                        refloat(ep);
                        break;
                    case 11:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) - (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 14:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        if (isunsignedexpr(ep2))
                            ep->v.c->r = (unsigned long long)ep2->v.i;
                        else
                            ep->v.c->r = (long long)ep2->v.i;
                        ep->v.c->i = (*ep1->v.f);
                        refloat(ep);
                        break;
                    case 15:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        if (isunsignedexpr(ep1))
                            ep->v.c->r = (unsigned long long)ep1->v.i;
                        else
                            ep->v.c->r = (long long)ep1->v.i;
                        ep->v.c->i = (*ep2->v.f);
                        ep->v.c->i.Negate();
                        refloat(ep);
                        break;
                    case 16:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep2->v.c->r;
                        ep->v.c->r.Negate();
                        ep->v.c->i = (*ep1->v.f) - ep2->v.c->i;
                        refloat(ep);
                        break;
                    case 17:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r;
                        ep->v.c->i = ep1->v.c->i - (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 18:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep1->v.f) - ep2->v.c->r;
                        ep->v.c->i = ep2->v.c->i;
                        ep->v.c->i.Negate();
                        refloat(ep);
                        break;
                    case 19:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r - (*ep2->v.f);
                        ep->v.c->i = ep1->v.c->i;
                        refloat(ep);
                        break;
                    case 20:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r - ep2->v.c->r;
                        ep->v.c->i = ep1->v.c->i - ep2->v.c->i;
                        refloat(ep);
                        break;
                    case 23:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r - temp;
                        ep->v.c->i = ep1->v.c->i;
                        refloat(ep);
                        break;
                    case 24:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = temp - ep2->v.c->r;
                        ep->v.c->i = ep2->v.c->i;
                        ep->v.c->i.Negate();
                        refloat(ep);
                        break;
                    default:
                        break;
                }
                break;
            case ExpressionNode::arraymul:
            case ExpressionNode::umul:
            case ExpressionNode::mul:
                switch (mode)
                {
                    case 1:
                        ep->type = maxinttype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.i = ep1->v.i * ep2->v.i;
                        ep->v.i = reint(ep);
                        break;
                    case 2:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = temp * (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 3:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) * temp;
                        refloat(ep);
                        break;
                    case 4:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) * (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 9:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) * (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 10:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) * (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 11:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) * (*ep2->v.f);
                        ep->v.f->Negate();
                        refloat(ep);
                        break;
                    case 14:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) * temp;
                        refloat(ep);
                        break;
                    case 15:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = temp * (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 16:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep1->v.f) * ep2->v.c->i;
                        ep->v.c->r.Negate();
                        ep->v.c->i = (*ep1->v.f) * ep2->v.c->r;
                        refloat(ep);
                        break;
                    case 17:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep2->v.f) * ep1->v.c->i;
                        ep->v.c->r.Negate();
                        ep->v.c->i = (*ep2->v.f) * ep1->v.c->r;
                        refloat(ep);
                        break;
                    case 18:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep1->v.f) * ep2->v.c->r;
                        ep->v.c->i = (*ep1->v.f) * ep2->v.c->i;
                        refloat(ep);
                        break;
                    case 19:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = (*ep2->v.f) * ep1->v.c->r;
                        ep->v.c->i = (*ep2->v.f) * ep1->v.c->i;
                        refloat(ep);
                        break;
                    case 20:
                        if (ep1->v.c->r.ValueIsZero())
                        {
                            ep->type = maxcomplextype(ep1, ep2);
                            ep->left = ep->right = nullptr;
                            ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                               ? STD_PRAGMA_CXLIMITED
                                               : 0;
                            ep->v.c = Allocate<_COMPLEX_S>();
                            ep->v.c->i = ep2->v.c->r * ep1->v.c->i;
                            ep->v.c->r = ep2->v.c->i * ep1->v.c->i;
                            ep->v.c->r.Negate();
                        }
                        else if (ep1->v.c->i.ValueIsZero())
                        {
                            ep->type = maxcomplextype(ep1, ep2);
                            ep->left = ep->right = nullptr;
                            ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                               ? STD_PRAGMA_CXLIMITED
                                               : 0;
                            ep->v.c = Allocate<_COMPLEX_S>();
                            ep->v.c->r = ep2->v.c->r * ep1->v.c->r;
                            ep->v.c->i = ep2->v.c->i * ep1->v.c->r;
                        }
                        else if (ep2->v.c->r.ValueIsZero())
                        {
                            ep->type = maxcomplextype(ep1, ep2);
                            ep->left = ep->right = nullptr;
                            ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                               ? STD_PRAGMA_CXLIMITED
                                               : 0;
                            ep->v.c = Allocate<_COMPLEX_S>();
                            ep->v.c->i = ep1->v.c->r * ep2->v.c->i;
                            ep->v.c->r = ep1->v.c->i * ep2->v.c->i;
                            ep->v.c->r.Negate();
                        }
                        else if (ep2->v.c->i.ValueIsZero())
                        {
                            ep->type = maxcomplextype(ep1, ep2);
                            ep->left = ep->right = nullptr;
                            ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                               ? STD_PRAGMA_CXLIMITED
                                               : 0;
                            ep->v.c = Allocate<_COMPLEX_S>();
                            ep->v.c->r = ep1->v.c->r * ep2->v.c->r;
                            ep->v.c->i = ep1->v.c->i * ep2->v.c->r;
                        }
                        refloat(ep);
                        break;
                    case 24:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = temp * ep2->v.c->r;
                        ep->v.c->i = temp * ep2->v.c->i;
                        refloat(ep);
                        break;
                    default:
                        break;
                }
                break;
            case ExpressionNode::arraydiv:
            case ExpressionNode::div:
            case ExpressionNode::udiv:
                switch (mode)
                {
                    case 1:
                        if (ep2->v.i)
                        {
                            ep->type = maxinttype(ep1, ep2);
                            ep->left = ep->right = nullptr;
                            if (isunsignedexpr(ep1) || isunsignedexpr(ep2))
                                ep->v.i = (unsigned long long)ep1->v.i / (unsigned long long)ep2->v.i;
                            else
                                ep->v.i = ep1->v.i / ep2->v.i;

                            ep->v.i = reint(ep);
                        }
                        break;
                    case 2:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = temp / (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 3:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) / temp;
                        refloat(ep);
                        break;
                    case 4:
                        ep->type = maxfloattype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) / (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 8:  // convert to a multiply
                    {
                        /*
                        FPF temp1;
                        temp1 = (unsigned long long)1;
                        temp1 = temp1 / (*ep2->v.f);
                        (*ep2->v.f) = temp1;
                        ep->type = ExpressionNode::mul;
                        ep->left = ep->right = nullptr;
                        */
                    }
                    break;
                    case 9:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) / (*ep2->v.f);
                        ep->v.f->Negate();
                        refloat(ep);
                        break;
                    case 10:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) / (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 11:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) / (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 14:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = (*ep1->v.f) / temp;
                        refloat(ep);
                        break;
                    case 15:
                        ep->type = maximaginarytype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        if (isunsignedexpr(ep1))
                            temp = (unsigned long long)ep1->v.i;
                        else
                            temp = (long long)ep1->v.i;
                        ep->v.f = Allocate<FPF>();
                        (*ep->v.f) = temp / (*ep2->v.f);
                        ep->v.f->Negate();
                        refloat(ep);
                        break;
                    case 17:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->i / (*ep2->v.f);
                        ep->v.c->i = ep1->v.c->r / (*ep2->v.f);
                        ep->v.c->i.Negate();
                        refloat(ep);
                        break;
                    case 19:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->i / (*ep2->v.f);
                        ep->v.c->i = ep1->v.c->r / (*ep2->v.f);
                        refloat(ep);
                        break;
                    case 20:
                        if (ep2->v.c->r.ValueIsZero())
                        {
                            ep->type = maxcomplextype(ep1, ep2);
                            ep->left = ep->right = nullptr;
                            ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                               ? STD_PRAGMA_CXLIMITED
                                               : 0;
                            ep->v.c = Allocate<_COMPLEX_S>();
                            ep->v.c->i = ep1->v.c->r / ep2->v.c->i;
                            ep->v.c->r = ep1->v.c->i / ep2->v.c->i;
                            ep->v.c->i.Negate();
                        }
                        else if (ep2->v.c->i.ValueIsZero())
                        {
                            ep->type = maxcomplextype(ep1, ep2);
                            ep->left = ep->right = nullptr;
                            ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                               ? STD_PRAGMA_CXLIMITED
                                               : 0;
                            ep->v.c = Allocate<_COMPLEX_S>();
                            ep->v.c->r = ep1->v.c->r / ep2->v.c->r;
                            ep->v.c->i = ep1->v.c->i / ep2->v.c->r;
                        }
                        refloat(ep);
                        break;
                    case 23:
                        ep->type = maxcomplextype(ep1, ep2);
                        ep->left = ep->right = nullptr;
                        ep->pragmas |= ((ep1->pragmas & STD_PRAGMA_CXLIMITED) && (ep2->pragmas & STD_PRAGMA_CXLIMITED))
                                           ? STD_PRAGMA_CXLIMITED
                                           : 0;
                        if (isunsignedexpr(ep2))
                            temp = (unsigned long long)ep2->v.i;
                        else
                            temp = (long long)ep2->v.i;
                        ep->v.c = Allocate<_COMPLEX_S>();
                        ep->v.c->r = ep1->v.c->r / temp;
                        ep->v.c->i = ep1->v.c->i / temp;
                        refloat(ep);
                        break;
                    default:
                        break;
                }
                break;
            case ExpressionNode::mod:
            case ExpressionNode::umod:
                if (ep2->v.i)
                {
                    ep->type = maxinttype(ep1, ep2);
                    ep->left = ep->right = nullptr;

                    if (isunsignedexpr(ep1) || isunsignedexpr(ep2))
                        ep->v.i = (unsigned long long)ep1->v.i % (unsigned long long)ep2->v.i;
                    else
                        ep->v.i = ep1->v.i % ep2->v.i;
                    ep->v.i = reint(ep);
                }
                break;
            case ExpressionNode::arraylsh:
            case ExpressionNode::lsh:
                ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
                ep->left = ep->right = nullptr;
                ep->v.i = ep1->v.i << ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case ExpressionNode::ursh:
                ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
                ep->left = ep->right = nullptr;
                ep1->v.i = reint(ep1);
                ep->v.i = ((unsigned long long)ep1->v.i) >> ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case ExpressionNode::rsh:
                ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
                ep->left = ep->right = nullptr;
                ep->v.i = ep1->v.i >> ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case ExpressionNode::and_:
                ep->type = maxinttype(ep1, ep2);
                ep->left = ep->right = nullptr;
                ep->v.i = ep1->v.i & ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case ExpressionNode::or_:
                ep->type = maxinttype(ep1, ep2);
                ep->left = ep->right = nullptr;
                ep->v.i = ep1->v.i | ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case ExpressionNode::xor_:
                ep->type = maxinttype(ep1, ep2);
                ep->left = ep->right = nullptr;
                ep->v.i = ep1->v.i ^ ep2->v.i;
                ep->v.i = reint(ep);
                break;
            default:
                break;
        }
}

/*-------------------------------------------------------------------------*/

void addaside(EXPRESSION* node)
{
    *asidetail = exprNode(ExpressionNode::void_, node, 0);
    asidetail = &(*asidetail)->right;
}

/*-------------------------------------------------------------------------*/
EXPRESSION* relptr(EXPRESSION* node, int& offset, bool add)
{
    EXPRESSION* rv;
    switch (node->type)
    {
        case ExpressionNode::global:
        case ExpressionNode::auto_:
        case ExpressionNode::threadlocal:
            return node;
        case ExpressionNode::add: {
            auto rv1 = relptr(node->left, offset, true);
            auto rv2 = relptr(node->right, offset, true);
            if (rv1)
                return rv1;
            else
                return rv2;
            break;
        }
        case ExpressionNode::sub: {
            auto rv1 = rv = relptr(node->left, offset, false);
            auto rv2 = relptr(node->right, offset, false);
            if (rv1)
                return rv1;
            else
                return rv2;
            break;
        }
        default:
            if (castvalue(node))
            {
                return relptr(node->left, offset, add);
            }
            if (isintconst(node))
            {
                if (add)
                    offset += node->v.i;
                else
                    offset -= node->v.i;
            }
            return nullptr;
    }
    return rv;
}
bool expressionHasSideEffects(EXPRESSION *exp)
{
    std::stack<EXPRESSION*> stk;
    stk.push(exp);
    while (!stk.empty())
    {
        auto p = stk.top();
        stk.pop();
        switch (p->type)
        {
            case ExpressionNode::atomic:
            case ExpressionNode::func:
            case ExpressionNode::thisref:
            case ExpressionNode::assign:
            case ExpressionNode::auto_inc:
            case ExpressionNode::auto_dec:
               return true;
        }
        if (p->right)
            stk.push(p->right);
        if (p->left)
            stk.push(p->left);
    }
    return false;
}
int opt0(EXPRESSION** node)
/*
 *      opt0 - delete useless expressions and combine constants.
 *
 *      opt0 will delete expressions such as x + 0, x - 0, x * 0,
 *      x * 1, 0 / x, x / 1, x Keyword::_mod 0, etc from the tree pointed to
 *      by node and combine obvious constant operations. It cannot
 *      combine name and label constants but will combine icon type
 *      nodes.
 *
 * ExpressionNode::arrayadd is not optimized to keep from optimizing struct members
 * into regs later...  just en: ExpressionNode::arrayadd is now equiv to ExpressionNode::add
 * again.  I have fixed the optimize struct thing another way...
 * leaving ExpressionNode::arrayadd in in case we need it later.
 */
{
    EXPRESSION* ep;
    long long val;
    int rv = false;
    int mode;
    FPF dval;
    ExpressionNode negtype = ExpressionNode::uminus;

    ep = *node;

    if (ep == 0)
        return false;
    switch (ep->type)
    {
        case ExpressionNode::const_ruct:
            break;
        case ExpressionNode::l_sp:
        case ExpressionNode::l_fp:
        case ExpressionNode::l_bool:
        case ExpressionNode::l_bit:
        case ExpressionNode::l_wc:
        case ExpressionNode::l_u16:
        case ExpressionNode::l_u32:
        case ExpressionNode::l_c:
        case ExpressionNode::l_s:
            /* optimize unary node */
        case ExpressionNode::l_uc:
        case ExpressionNode::l_us:
            /* optimize unary node */
        case ExpressionNode::l_p:
        case ExpressionNode::l_ref:
        case ExpressionNode::l_i:
        case ExpressionNode::l_ui:
        case ExpressionNode::l_inative:
        case ExpressionNode::l_unative:
        case ExpressionNode::l_l:
        case ExpressionNode::l_ul:
        case ExpressionNode::l_ll:
        case ExpressionNode::l_ull:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_fi:
        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::l_string:
        case ExpressionNode::l_object: {
            rv |= opt0(&((*node)->left));
            ConstExprStructElemEval(node);
            if (lvalue(*node) && !(*node)->left->init && inConstantExpression)
            {
                int offset = 0;

                auto ref = relptr((*node)->left, offset);
                if (ref)
                {
                    // this will only do one level, so if we have an array of pointers to arrays
                    // it won't work.
                    if (ref->v.sp->sb->constexpression && ref->v.sp->sb->init)
                    {
                        auto its = ref->v.sp->sb->init->begin();
                        if (isarray(ref->v.sp->tp))
                        {
                            int n = offset / ref->v.sp->tp->btp->size;
                            while (n-- && its != ref->v.sp->sb->init->end())
                                ++its;
                            if (its != ref->v.sp->sb->init->end())
                            {
                                **node = *(*its)->exp;
                                rv = true;
                            }
                        }
                        else
                        {
                            **node = *ref->v.sp->sb->init->front()->exp;
                            rv = true;
                        }
                    }
                }
            }
        }
        break;
        case ExpressionNode::x_bool:
        case ExpressionNode::x_bit:
        case ExpressionNode::x_wc:
        case ExpressionNode::x_c:
        case ExpressionNode::x_uc:
        case ExpressionNode::x_u16:
        case ExpressionNode::x_u32:
        case ExpressionNode::x_s:
        case ExpressionNode::x_us:
        case ExpressionNode::x_l:
        case ExpressionNode::x_ul:
        case ExpressionNode::x_i:
        case ExpressionNode::x_ui:
        case ExpressionNode::x_inative:
        case ExpressionNode::x_unative:
        case ExpressionNode::x_ll:
        case ExpressionNode::x_ull:
        case ExpressionNode::x_f:
        case ExpressionNode::x_d:
        case ExpressionNode::x_p:
        case ExpressionNode::x_ld:
        case ExpressionNode::x_fp:
        case ExpressionNode::x_sp:
        case ExpressionNode::x_dc:
        case ExpressionNode::x_fc:
        case ExpressionNode::x_ldc:
        case ExpressionNode::x_di:
        case ExpressionNode::x_fi:
        case ExpressionNode::x_ldi:
        case ExpressionNode::x_string:
        case ExpressionNode::x_object:
        case ExpressionNode::shiftby:
        case ExpressionNode::bits:
        case ExpressionNode::literalclass:
            rv |= opt0(&((*node)->left));
            return rv;
        case ExpressionNode::compl_:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                rv = true;
                ep->type = ep->left->type;
                ep->v.i = ~ep->left->v.i;
                ep->v.i = reint(ep);
                ep->unionoffset = ep->left->unionoffset;
                ep->left = ep->right = nullptr;
            }
            return rv;

        case ExpressionNode::uminus:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                *node = intNode(ep->left->type, -ep->left->v.i);
                rv = true;
            }
            else if (isfloatconst(ep->left))
            {
                *node = intNode(ep->left->type, 0);
                (*node)->v.f = ep->left->v.f;
                (*node)->v.f->Negate();
                rv = true;
            }
            else if (ep->left->type == ExpressionNode::c_d || ep->left->type == ExpressionNode::c_f || ep->left->type == ExpressionNode::c_ld ||
                     ep->left->type == ExpressionNode::c_di || ep->left->type == ExpressionNode::c_fi || ep->left->type == ExpressionNode::c_ldi)
            {
                rv = true;
                ep->type = ep->left->type;
                ep->v.f = Allocate<FPF>();
                (*ep->v.f) = *ep->left->v.f;
                ep->v.f->Negate();
                *node = ep;
                ep->left = ep->right = nullptr;
            }
            else if (ep->left->type == ExpressionNode::c_dc || ep->left->type == ExpressionNode::c_fc || ep->left->type == ExpressionNode::c_ldc)
            {
                rv = true;
                ep->type = ep->left->type;
                ep->v.c = Allocate<_COMPLEX_S>();
                ep->v.c->r = ep->left->v.c->r;
                ep->v.c->r.Negate();
                ep->v.c->i = ep->left->v.c->i;
                ep->v.c->i.Negate();
                *node = ep;
                ep->left = ep->right = nullptr;
            }
            return rv;
        case ExpressionNode::structadd:
        case ExpressionNode::add:
        case ExpressionNode::arrayadd:
        case ExpressionNode::sub:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            if (ep->right->type == ExpressionNode::structelem || ep->left->type == ExpressionNode::structadd)
                break;
            {
                // this next will normalize expressions of the form:
                // z = (a + 5) - a
                // to z = 5;
                // regardless of whether a can be evaluated
                auto exp = ep->left;
                while (castvalue(exp))
                    exp = exp->left;
                if (exp->type == ExpressionNode::add)
                {
                    if (!expressionHasSideEffects(ep))
                    {
                        auto expr = ep->right;
                        while (castvalue(expr))
                            expr = expr->left;
                        if (equalnode(exp->left, expr))
                        {
                            *node = exp->right;
                            rv = true;
                            break;
                        }
                    }
                }
            }
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                default:
                    rv = true;
                    dooper(node, mode);
                    break;
                case 5:
                    if (ep->left->v.i == 0)
                    {
                        if (ep->left->type != ExpressionNode::auto_ && ep->type != ExpressionNode::structadd)
                        {
                            if (ep->type == ExpressionNode::sub)
                                *node = exprNode(ExpressionNode::uminus, ep->right, 0);
                            else
                                *node = ep->right;
                            rv = true;
                        }
                    }
                    break;
                case 6:
                    if (ep->left->v.f->ValueIsZero())
                    {
                        if (ep->type == ExpressionNode::sub)
                        {
                            *node = exprNode(ExpressionNode::uminus, ep->right, 0);
                        }
                        *node = ep->right;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 7:
                    if (ep->right->v.i == 0)
                    {
                        if (ep->left->type != ExpressionNode::auto_ && ep->type != ExpressionNode::structadd)
                        {
                            *node = ep->left;
                            rv = true;
                        }
                    }
                    else
                    {
                        dooper(node, mode);
                        rv = true;
                    }
                    break;
                case 8:
                    if (ep->right->v.f->ValueIsZero())
                    {
                        *node = ep->left;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 22:
                    if (ep->right->v.c->r.ValueIsZero() && ep->right->v.c->i.ValueIsZero())
                    {
                        *node = ep->left;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
            }
            return rv;
        case ExpressionNode::mul:
        case ExpressionNode::umul:
        case ExpressionNode::arraymul:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                default:
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 9:
                case 10:
                case 11:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                    dooper(node, mode);
                    rv = true;
                    break;
                case 5:
                    if (!hasFloats(ep->right))
                    {
                        val = ep->left->v.i;
                        if (val == 0)
                        {
                            if (ep->right->type != ExpressionNode::_sizeof)
                            {
                                addaside(ep->right);
                                *node = ep->left;
                                rv = true;
                            }
                        }
                        else if (val == 1)
                            *node = ep->right;
                        else if (val == -1)
                            *node = exprNode(negtype, ep->right, 0);
                        else
                        {
                            long long i = Optimizer::pwrof2(val);
                            if (i != -1)
                            {
                                EXPRESSION* x = ep->left;
                                ep->left = ep->right;
                                ep->right = x;
                                ep->right->v.i = i;
                                ep->right->type = ExpressionNode::c_i;
                                ep->right->left = ep->right->right = nullptr;
                                rv = true;
                                switch (ep->type)
                                {
                                    case ExpressionNode::mul:
                                        ep->type = ExpressionNode::lsh;
                                        break;
                                    case ExpressionNode::umul:
                                        ep->type = ExpressionNode::lsh;
                                        break;
                                    case ExpressionNode::arraymul:
                                        ep->type = ExpressionNode::arraylsh;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            }
                        }
                    }
                    break;
                case 6:
                    dval = *ep->left->v.f;
#ifdef XXXXX
                    if (dval.ValueIsZero())
                    {
                        addaside(ep->right);
                        *node = ep->left;
                    }
                    else
#endif
                        if (!dval.ValueIsNegative() && dval.ValueIsOne())
                        *node = ep->right;
                    else if (dval.ValueIsNegative() && dval.ValueIsOne())
                        *node = exprNode(negtype, ep->right, 0);
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 7:
                    if (!hasFloats(ep->left))
                    {
                        val = ep->right->v.i;
                        if (val == 0)
                        {
                            addaside(ep->left);
                            *node = ep->right;
                        }
                        else if (val == 1)
                        {
                            *node = ep->left;
                        }
                        else if (val == -1)
                        {
                            *node = exprNode(negtype, ep->left, 0);
                        }
                        else
                        {
                            long long i = Optimizer::pwrof2(val);
                            if (i != -1)
                            {
                                ep->right->v.i = i;
                                ep->right->type = ExpressionNode::c_i;
                                ep->right->left = ep->right->right = nullptr;
                                rv = true;
                                switch (ep->type)
                                {
                                    case ExpressionNode::mul:
                                        ep->type = ExpressionNode::lsh;
                                        break;
                                    case ExpressionNode::umul:
                                        ep->type = ExpressionNode::lsh;
                                        break;
                                    case ExpressionNode::arraymul:
                                        ep->type = ExpressionNode::arraylsh;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            }
                        }
                    }
                    dooper(node, mode);
                    rv = true;
                    break;
                case 8:
                    dval = *ep->right->v.f;
                    if (!dval.ValueIsNegative() && dval.ValueIsOne())
                    {
                        *node = ep->left;
                    }
                    else if (dval.ValueIsNegative() && dval.ValueIsOne())
                    {
                        *node = exprNode(negtype, ep->left, 0);
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 22:
                    dval = ep->right->v.c->r;
                    if (ep->right->v.c->i.ValueIsZero())
                    {
                        if (!dval.ValueIsNegative() && dval.ValueIsOne())
                            *node = ep->left;
                        else if (dval.ValueIsNegative() && dval.ValueIsOne())
                        {
                            *node = exprNode(negtype, ep->left, 0);
                        }
                        else
                            dooper(node, mode);
                    }
            }
            break;
        case ExpressionNode::arraydiv:
        case ExpressionNode::div:
        case ExpressionNode::udiv:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 20:
                    dooper(node, mode);
                    rv = true;
                    break;
                case 5:
                    if (ep->left->v.i == 0)
                    {
                        addaside(ep->right);
                        *node = ep->left;
                        rv = true;
                    }
                    break;
                case 6:
                    dooper(node, mode);
                    rv = true;
                    break;
                case 7:
                    if (!hasFloats(ep->left))
                    {
                        val = ep->right->v.i;
                        if (val == 1)
                            *node = ep->left;
                        else if (val == -1)
                            *node = exprNode(negtype, ep->left, 0);
                    }
                    dooper(node, mode);
                    rv = true;
                    break;
                case 8:
                    dval = *ep->right->v.f;
                    if (!dval.ValueIsNegative() && dval.ValueIsOne())
                        *node = ep->left;
                    if (dval.ValueIsNegative() && dval.ValueIsOne())
                        *node = exprNode(negtype, ep->left, 0);
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 22:
                    dval = ep->right->v.c->r;
                    if (ep->right->v.c->i.ValueIsZero())
                    {
                        if (!dval.ValueIsNegative() && dval.ValueIsOne())
                            *node = ep->left;
                        else if (dval.ValueIsNegative() && dval.ValueIsOne())
                        {
                            *node = exprNode(negtype, ep->left, 0);
                        }
                        else
                            dooper(node, mode);
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
            }
            break;
        case ExpressionNode::mod:
        case ExpressionNode::umod:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                default:
                    break;
                case 7:
                    if (ep->right->v.i == 1)
                    {
                        ep->v.i = 0;
                        ep->type = ExpressionNode::c_i;
                        ep->left = ep->right = nullptr;
                    }
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 8:
                    dooper(node, mode);
                    rv = true;
                    break;
            }
            break;
        case ExpressionNode::and_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                default:
                    break;
                case 1:
                    dooper(node, mode);
                    rv = true;
                    break;
                case 5:
                    if (ep->left->v.i == 0)
                    {
                        addaside(ep->right);
                        *node = ep->left;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 7:
                    if (ep->right->v.i == 0)
                    {
                        addaside(ep->left);
                        *node = ep->right;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
            }
            break;
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                default:
                    break;
                case 1:
                    dooper(node, mode);
                    rv = true;
                    break;
                case 5:
                    if (ep->left->v.i == 0)
                    {
                        *node = ep->right;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 7:
                    if (ep->right->v.i == 0)
                    {
                        *node = ep->left;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
            }
            break;
        case ExpressionNode::rsh:
        case ExpressionNode::lsh:
        case ExpressionNode::arraylsh:
        case ExpressionNode::ursh:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left) && ep->right->type == ExpressionNode::shiftby)
                if (isintconst(ep->right->left))
                {
                    ep->right = ep->right->left;
                    rv = true;
                }
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                default:
                    break;
                case 1:
                    dooper(node, mode);
                    rv = true;
                    break;
                case 5:
                    if (ep->left->v.i == 0)
                    {
                        addaside(ep->right);
                        *node = ep->left;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 7:
                    if (ep->right->v.i == 0)
                    {
                        *node = ep->left;
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
            }
            break;
        case ExpressionNode::land:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                if (!ep->left->v.i)
                {
                    rv = true;
                    *node = intNode(ExpressionNode::c_i, 0);
                    break;
                }
                else
                {
                    goto join_land;
                }
            }
            else
                switch (ep->left->type)
                {
                    case ExpressionNode::pc:
                    case ExpressionNode::threadlocal:
                    case ExpressionNode::global:
                    case ExpressionNode::labcon:
                    case ExpressionNode::auto_: {
                    join_land:
                        rv |= opt0(&(ep->right));
                        if (isintconst(ep->right))
                        {
                            rv = true;
                            *node = intNode(ExpressionNode::c_i, !!ep->right->v.i);
                        }
                        else
                            switch (ep->right->type)
                            {
                                case ExpressionNode::pc:
                                case ExpressionNode::threadlocal:
                                case ExpressionNode::global:
                                case ExpressionNode::labcon:
                                case ExpressionNode::auto_:
                                    /* assumes nothing can be relocated to address 0 */
                                    *node = intNode(ExpressionNode::c_i, 1);
                                    rv = true;
                                    break;
                                default:
                                    break;
                            }
                    }
                    default:
                        rv |= opt0(&(ep->right));
                        break;
                }
            break;
        case ExpressionNode::lor:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                if (ep->left->v.i)
                {
                    rv = true;
                    *node = intNode(ExpressionNode::c_i, 1);
                    break;
                }
                else
                {
                    goto join_lor;
                }
            }
            else
                switch (ep->left->type)
                {
                    case ExpressionNode::pc:
                    case ExpressionNode::threadlocal:
                    case ExpressionNode::global:
                    case ExpressionNode::labcon:
                    case ExpressionNode::auto_:
                        /* assumes nothing can be relocated to address 0 */
                        *node = intNode(ExpressionNode::c_i, 1);
                        rv = true;
                        break;
                    default: {
                    join_lor:
                        rv |= opt0(&(ep->right));
                        if (isintconst(ep->right))
                        {
                            if (ep->right->v.i)
                            {
                                rv = true;
                                *node = intNode(ExpressionNode::c_i, 1);
                            }
                            else if (isintconst(ep->left) && !ep->left->v.i)
                            {
                                rv = true;
                                *node = intNode(ExpressionNode::c_i, 0);
                            }
                        }
                        else
                            switch (ep->right->type)
                            {
                                case ExpressionNode::pc:
                                case ExpressionNode::threadlocal:
                                case ExpressionNode::global:
                                case ExpressionNode::labcon:
                                case ExpressionNode::auto_:
                                    /* assumes nothing can be relocated to address 0 */
                                    *node = intNode(ExpressionNode::c_i, 1);
                                    rv = true;
                                    break;
                                default:
                                    break;
                            }
                    }
                }
            break;
        case ExpressionNode::not_:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                *node = intNode(ExpressionNode::c_i, (!ep->left->v.i));
                rv = true;
            }
            else if (isfloatconst(ep->left))
            {
                *node = intNode(ExpressionNode::c_i, ep->left->v.f->ValueIsZero());
                rv = true;
            }
            break;
        case ExpressionNode::eq:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, (ep->left->v.i == ep->right->v.i));
                    rv = true;

                    break;
                case 4:
                    *node = intNode(ExpressionNode::c_i, (*ep->left->v.f == *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2)
                    {
                        if (rv1->v.sp != rv2->v.sp || offset1 != offset2)
                        {
                            *node = intNode(ExpressionNode::c_i, 0);
                        }
                        else
                        {
                            *node = intNode(ExpressionNode::c_i, 1);
                        }
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ne:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, (ep->left->v.i != ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = intNode(ExpressionNode::c_i, (*ep->left->v.f != *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2)
                    {
                        if (rv1->v.sp != rv2->v.sp || offset1 != offset2)
                        {
                            *node = intNode(ExpressionNode::c_i, 1);
                        }
                        else
                        {
                            *node = intNode(ExpressionNode::c_i, 0);
                        }
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::lt:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, (ep->left->v.i < ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = intNode(ExpressionNode::c_i, (*ep->left->v.f < *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 < offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::le:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, (ep->left->v.i <= ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = intNode(ExpressionNode::c_i, (*ep->left->v.f <= *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 <= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ugt:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, ((unsigned long long)ep->left->v.i > (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 > offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::uge:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, ((unsigned long long)ep->left->v.i >= (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 >= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ult:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, ((unsigned long long)ep->left->v.i < (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 < offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ule:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, ((unsigned long long)ep->left->v.i <= (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 <= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::gt:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, (ep->left->v.i > ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = intNode(ExpressionNode::c_i, (*ep->left->v.f > *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 > offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ge:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = intNode(ExpressionNode::c_i, (ep->left->v.i >= ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = intNode(ExpressionNode::c_i, (*ep->left->v.f > *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = intNode(ExpressionNode::c_i, offset1 >= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;

        case ExpressionNode::cond:
            rv |= opt0(&(ep->right));
            rv |= opt0(&(ep->left));
            if (isoptconst(ep->left) && (parsingPreprocessorConstant || isoptconst(ep->right->left) && isoptconst(ep->right->right)))
            {
                if (isfloatconst(ep->left))
                {
                    if (!ep->left->v.f->ValueIsZero())
                        *node = ep->right->left;
                    else
                        *node = ep->right->right;
                    break;
                }
                else if (isintconst(ep->left))
                {
                    if (ep->left->v.i)
                        *node = ep->right->left;
                    else
                        *node = ep->right->right;
                    break;
                }
            }
            break;
        case ExpressionNode::void_:
        case ExpressionNode::intcall:
        case ExpressionNode::void_nz:
        case ExpressionNode::assign:
        case ExpressionNode::blockassign:
        case ExpressionNode::stackblock:
        case ExpressionNode::auto_inc:
        case ExpressionNode::auto_dec:
        case ExpressionNode::_initblk:
        case ExpressionNode::_cpblk:
            rv |= opt0(&(ep->right));
        case ExpressionNode::trapcall:
        case ExpressionNode::substack:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack:
        case ExpressionNode::savestack:
        case ExpressionNode::blockclear:
        case ExpressionNode::argnopush:
        case ExpressionNode::not__lvalue:
        case ExpressionNode::lvalue:
        case ExpressionNode::thisref:
        case ExpressionNode::funcret:
        case ExpressionNode::_initobj:
        case ExpressionNode::_sizeof:
        case ExpressionNode::select:
            rv |= opt0(&(ep->left));
            break;
        case ExpressionNode::dot:
        case ExpressionNode::pointsto:
            if (!templateNestingCount || instantiatingTemplate)
            {
                EXPRESSION* newExpr = ep->left;
                EXPRESSION* next = ep->right;
                TYPE* tp = LookupTypeFromExpression(ep->left, nullptr, false);
                while (ep->type == ExpressionNode::dot || ep->type == ExpressionNode::pointsto)
                {
                    rv |= opt0(&(ep->right));
                    rv |= opt0(&(ep->left));
                    if (ep->type == ExpressionNode::pointsto)
                    {
                        if (!ispointer(tp))
                            break;
                        tp = basetype(tp->btp);
                        deref(&stdpointer, &newExpr);
                    }
                    if (!isstructured(tp))
                        break;
                    if (next->type == ExpressionNode::dot || next->type == ExpressionNode::pointsto)
                    {
                        next = next->left;
                    }
                    STRUCTSYM s;
                    s.str = basetype(tp)->sp;
                    addStructureDeclaration(&s);
                    if (next->type == ExpressionNode::func)
                    {
                        TYPE* ctype = tp;
                        SYMBOL* sym = classsearch(next->v.func->sp->name, false, false, false);
                        if (!sym)
                        {
                            dropStructureDeclaration();
                            break;
                        }
                        FUNCTIONCALL* func = Allocate<FUNCTIONCALL>();
                        *func = *next->v.func;
                        func->sp = sym;
                        func->thistp = MakeType(BasicType::pointer, tp);
                        func->thisptr = newExpr;
                        sym = GetOverloadedFunction(&ctype, &func->fcall, sym, func, nullptr, true, false, 0);
                        if (!sym)
                        {
                            dropStructureDeclaration();
                            break;
                        }
                        EXPRESSION* temp = varNode(ExpressionNode::func, sym);
                        temp->v.func = next->v.func;
                        temp = exprNode(ExpressionNode::thisref, temp, nullptr);
                        temp->v.t.thisptr = newExpr;
                        temp->v.t.tp = tp;
                        newExpr = temp;
                        tp = basetype(sym->tp)->btp;
                    }
                    else
                    {
                        SYMBOL* sym = classsearch(GetSymRef(next)->v.sp->name, false, false, false);
                        if (!sym)
                        {
                            dropStructureDeclaration();
                            break;
                        }
                        EXPRESSION* temp = intNode(ExpressionNode::c_i, 0);
                        if (sym->sb->parentClass != basetype(tp)->sp)
                        {
                            temp = baseClassOffset(basetype(tp)->sp, sym->sb->parentClass, temp);
                        }
                        newExpr = exprNode(ExpressionNode::structadd, newExpr, temp);
                        if (!isstructured(sym->tp))
                            deref(sym->tp, &newExpr);
                        tp = sym->tp;
                    }
                    dropStructureDeclaration();
                    ep = ep->right;
                }
                if (ep->type == ExpressionNode::dot || ep->type == ExpressionNode::pointsto)
                    rv = false;
                else
                    *node = newExpr;
            }
            break;
        case ExpressionNode::func:
            // rv |= opt0(&((*node)->v.func->fcall));
            if ((*node)->v.func->thisptr)
                rv |= opt0(&((*node)->v.func->thisptr));
            return rv;
        case ExpressionNode::atomic:
            rv |= opt0(&((*node)->v.ad->flg));
            rv |= opt0(&((*node)->v.ad->memoryOrder1));
            rv |= opt0(&((*node)->v.ad->memoryOrder2));
            rv |= opt0(&((*node)->v.ad->third));
            rv |= opt0(&((*node)->v.ad->address));
            rv |= opt0(&((*node)->v.ad->value));
            return rv;
        case ExpressionNode::sizeofellipse:
            if (!templateNestingCount || instantiatingTemplate)
            {
                int n = 0;
                if (!(*node)->v.templateParam->second->packed)
                {
                    n = 1;
                }
                else if (!(*node)->v.templateParam->second->byPack.pack)
                {
                    n = 0;
                }
                else
                {
                    n = (*node)->v.templateParam->second->byPack.pack->size();
                }
                *node = intNode(ExpressionNode::c_i, n);
            }
            break;
        case ExpressionNode::templateselector:
            if (!templateNestingCount || instantiatingTemplate)
            {
                auto tsl = (*node)->v.templateSelector;
                SYMBOL* ts = (*tsl)[1].sp;
                SYMBOL* sym = ts;
                if ((*tsl)[1].isDeclType)
                {
                    TYPE* tp = TemplateLookupTypeFromDeclType((*tsl)[1].tp);
                    if (tp && isstructured(tp))
                        sym = basetype(tp)->sp;
                    else
                        sym = nullptr;
                }
                else
                {
                    if (ts->tp->type == BasicType::templateparam)
                    {
                        if (ts->tp->templateParam->second->type != Keyword::_template)
                            break;
                        ts = ts->tp->templateParam->second->byTemplate.val;
                        if (!ts)
                            break;
                    }
                    if ((*tsl)[1].isTemplate && (*tsl)[1].templateParams)
                    {
                        std::list<TEMPLATEPARAMPAIR>* current = SolidifyTemplateParams((*tsl)[1].templateParams);
                        if (ts->sb->storage_class == StorageClass::typedef_)
                        {
                            sym = GetTypeAliasSpecialization(sym, current);
                        }
                        else
                        {
                            sym = GetClassTemplate(ts, current, true);
                        }
                    }
                    if (sym && sym->tp->type == BasicType::templateselector)
                    {
                        TYPE* tp = sym->tp;
                        tp = SynthesizeType(tp, nullptr, false);
                        if (tp && isstructured(tp))
                            sym = basetype(tp)->sp;
                    }
                }
                if (sym)
                {
                    sym = basetype(PerformDeferredInitialization(sym->tp, nullptr))->sp;
                    if (sym)
                    {
                        auto find = (*tsl).begin();
                        ++find;
                        ++find;
                        while (find != (*tsl).end() && sym)
                        {
                            SYMBOL* spo = sym;
                            if (!isstructured(spo->tp))
                                break;

                            sym = search(spo->tp->syms, (*find).name);
                            if (!sym)
                            {
                                sym = classdata((*find).name, spo, nullptr, false, false);
                                if (sym == (SYMBOL*)-1)
                                    sym = nullptr;
                            }
                            if (sym && (*find).asCall)
                            {
                                if ((*find).arguments)
                                    for (auto i : *(*find).arguments)
                                    {
                                        i->tp = SynthesizeType(i->tp, nullptr, false);
                                    }
                                TYPE* ctype = sym->tp;
                                EXPRESSION* exp = intNode(ExpressionNode::c_i, 0);
                                FUNCTIONCALL funcparams = { };
                                funcparams.arguments = (*find).arguments;
                                auto sp1 = GetOverloadedFunction(&ctype, &exp, sym, &funcparams, nullptr, false, false, 0);
                                if (sp1)
                                {
                                    sym = sp1;
                                }
                            }
                            ++find;
                        }
                        if (find == (*tsl).end() && sym)
                        {
                            if (sym->sb->storage_class == StorageClass::const_ant)
                            {
                                optimize_for_constants(&sym->sb->init->front()->exp);
                                *node = sym->sb->init->front()->exp;
                                return true;
                            }
                        }
                    }
                }
                return false;
            }
            break;
        case ExpressionNode::templateparam:
            if ((!templateNestingCount || instantiatingTemplate) && (*node)->v.sp->tp->templateParam->second->type == Keyword::_int)
            {
                SYMBOL* sym = (*node)->v.sp;
                TEMPLATEPARAMPAIR* found = (*node)->v.sp->tp->templateParam;
                if (!found || !found->second->byNonType.val)
                    found = nullptr;
                for (auto&& search : structSyms)
                {
                    if (found)
                        break;
                    if (search.tmpl)
                    {
                        for (auto&& tpl : *search.tmpl)
                        {
                            if (tpl.first && !strcmp(tpl.first->name, sym->name))
                            {
                                found = &tpl;
                                if (found)
                                    break;
                            }
                        }
                    }
                }
                if (found && found->second->type == Keyword::_int)
                {
                    if (found->second->byNonType.val && !found->second->packed)
                        *node = found->second->byNonType.val;
                }
            }
            break;
        default:
            break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

void enswap(EXPRESSION** one, EXPRESSION** two)
{
    EXPRESSION* temp = *one;
    *one = *two;
    *two = temp;
}

/*-------------------------------------------------------------------------*/

int fold_const(EXPRESSION* node)
/*
 *      fold_const will remove constant nodes and return the values to
 *      the calling routines.
 */
{
    int rv = false;
    if (node == 0)
        return 0;
    switch (node->type)
    {
        case ExpressionNode::add:
        case ExpressionNode::arrayadd:
        case ExpressionNode::structadd:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (!inConstantExpression && (node->right->type == ExpressionNode::structelem || node->left->type == ExpressionNode::structadd))
                break;
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                    case ExpressionNode::add:
                    case ExpressionNode::arrayadd:
                    case ExpressionNode::structadd:
                        if (isoptconst(node->left->left))
                        {
                            ExpressionNode type = ExpressionNode::add;
                            if (node->type == ExpressionNode::arrayadd || node->left->type == ExpressionNode::arrayadd)
                                type = ExpressionNode::arrayadd;
                            if (node->type == ExpressionNode::structadd || node->left->type == ExpressionNode::structadd)
                                type = ExpressionNode::structadd;
                            node->type = type;
                            node->left->type = ExpressionNode::add;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            ExpressionNode type = ExpressionNode::add;

                            if (node->type == ExpressionNode::arrayadd || node->left->type == ExpressionNode::arrayadd)
                                type = ExpressionNode::arrayadd;
                            if (node->type == ExpressionNode::structadd || node->left->type == ExpressionNode::structadd)
                                type = ExpressionNode::structadd;
                            node->type = type;
                            node->left->type = ExpressionNode::add;
                            enswap(&node->left->left, &node->right);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub:
                        if (isoptconst(node->left->left))
                        {
                            ExpressionNode type = ExpressionNode::add;
                            if (node->type == ExpressionNode::arrayadd)
                                type = ExpressionNode::arrayadd;
                            if (node->type == ExpressionNode::structadd)
                                type = ExpressionNode::structadd;
                            node->type = ExpressionNode::sub;
                            node->left->type = type;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            ExpressionNode type = ExpressionNode::add;
                            if (node->type == ExpressionNode::arrayadd)
                                type = ExpressionNode::arrayadd;
                            if (node->type == ExpressionNode::structadd)
                                type = ExpressionNode::structadd;
                            node->type = type;
                            node->left->type = ExpressionNode::sub;
                            enswap(&node->left->left, &node->right);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            else if (isoptconst(node->left))
            {
                switch (node->right->type)
                {
                    case ExpressionNode::add:
                    case ExpressionNode::arrayadd:
                    case ExpressionNode::structadd:
                        if (isoptconst(node->right->left))
                        {
                            ExpressionNode type = ExpressionNode::add;
                            if (node->type == ExpressionNode::arrayadd || node->right->type == ExpressionNode::arrayadd)
                                type = ExpressionNode::arrayadd;
                            if (node->type == ExpressionNode::structadd || node->right->type == ExpressionNode::structadd)
                                type = ExpressionNode::structadd;
                            node->type = type;
                            node->right->type = ExpressionNode::add;
                            enswap(&node->right->right, &node->left);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            ExpressionNode type = ExpressionNode::add;
                            if (node->type == ExpressionNode::arrayadd || node->right->type == ExpressionNode::arrayadd)
                                type = ExpressionNode::arrayadd;
                            if (node->type == ExpressionNode::structadd || node->right->type == ExpressionNode::structadd)
                                type = ExpressionNode::structadd;
                            node->type = type;
                            node->right->type = ExpressionNode::add;
                            enswap(&node->right->left, &node->left);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub:
                        if (isoptconst(node->right->left))
                        {
                            node->type = ExpressionNode::sub;
                            node->right->type = ExpressionNode::add;
                            enswap(&node->left, &node->right);
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            ExpressionNode type = ExpressionNode::add;
                            if (node->type == ExpressionNode::arrayadd)
                                type = ExpressionNode::arrayadd;
                            if (node->type == ExpressionNode::structadd)
                                type = ExpressionNode::structadd;
                            node->type = type;
                            node->right->type = ExpressionNode::sub;
                            enswap(&node->right->left, &node->left);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case ExpressionNode::sub:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                    case ExpressionNode::add:
                    case ExpressionNode::arrayadd:
                    case ExpressionNode::structadd:
                        if (isoptconst(node->left->left))
                        {
                            node->type = ExpressionNode::add;
                            node->left->type = ExpressionNode::sub;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            node->type = ExpressionNode::add;
                            node->left->type = ExpressionNode::sub;
                            enswap(&node->left->left, &node->left->right);
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub:
                        if (isoptconst(node->left->left))
                        {
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            enswap(&node->left, &node->right);
                            enswap(&node->right->left, &node->left);
                            node->type = ExpressionNode::add;
                            node->right->left = exprNode(ExpressionNode::uminus, node->right->left, 0);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            else if (isoptconst(node->left))
            {
                switch (node->right->type)
                {
                    case ExpressionNode::add:
                    case ExpressionNode::arrayadd:
                    case ExpressionNode::structadd:
                        if (isoptconst(node->right->left))
                        {
                            node->type = ExpressionNode::sub;
                            node->right->type = ExpressionNode::sub;
                            enswap(&node->left, &node->right);
                            enswap(&node->left->right, &node->right);
                            enswap(&node->left->left, &node->left->right);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            node->type = ExpressionNode::sub;
                            node->right->type = ExpressionNode::sub;
                            enswap(&node->left, &node->right);
                            enswap(&node->left->left, &node->right);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub:
                        if (isoptconst(node->right->left))
                        {
                            enswap(&node->right->right, &node->left);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {

                            enswap(&node->left, &node->right);
                            enswap(&node->left->left, &node->right);
                            node->left->right = exprNode(ExpressionNode::uminus, node->left->right, 0);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case ExpressionNode::mul:
        case ExpressionNode::umul:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                    case ExpressionNode::mul:
                    case ExpressionNode::umul:
                        if (isoptconst(node->left->left))
                        {
                            node->type = ExpressionNode::mul;
                            node->left->type = ExpressionNode::mul;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            node->type = ExpressionNode::mul;
                            node->left->type = ExpressionNode::mul;
                            enswap(&node->left->left, &node->right);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            else if (isoptconst(node->left))
            {
                switch (node->right->type)
                {
                    case ExpressionNode::mul:
                    case ExpressionNode::umul:
                        if (isoptconst(node->right->left))
                        {
                            node->type = ExpressionNode::mul;
                            node->right->type = ExpressionNode::mul;
                            enswap(&node->right->right, &node->left);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            node->type = ExpressionNode::mul;
                            node->right->type = ExpressionNode::mul;
                            enswap(&node->right->left, &node->left);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (node->left->type == node->type && isoptconst(node->right))
            {
                if (isoptconst(node->left->left))
                {
                    enswap(&node->left->right, &node->right);
                    rv = true;
                }
                else if (isoptconst(node->left->right))
                {
                    enswap(&node->left->left, &node->right);
                    rv = true;
                }
            }
            else if (node->right->type == node->type && isoptconst(node->left))
            {
                if (isoptconst(node->right->left))
                {
                    enswap(&node->right->right, &node->left);
                    rv = true;
                }
                else if (isoptconst(node->right->right))
                {
                    enswap(&node->right->left, &node->left);
                    rv = true;
                }
            }
            break;
        case ExpressionNode::l_sp:
        case ExpressionNode::l_fp:
        case ExpressionNode::l_fi:
        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::x_fi:
        case ExpressionNode::x_di:
        case ExpressionNode::x_ldi:
        case ExpressionNode::x_fc:
        case ExpressionNode::x_dc:
        case ExpressionNode::x_ldc:
        case ExpressionNode::x_bit:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_ll:
        case ExpressionNode::l_ull:
        case ExpressionNode::l_wc:
        case ExpressionNode::l_u16:
        case ExpressionNode::l_u32:
        case ExpressionNode::l_c:
        case ExpressionNode::l_s:
        case ExpressionNode::l_p:
        case ExpressionNode::l_ref:
        case ExpressionNode::l_i:
        case ExpressionNode::l_ui:
        case ExpressionNode::l_inative:
        case ExpressionNode::l_unative:
        case ExpressionNode::l_ul:
        case ExpressionNode::l_l:
        case ExpressionNode::l_uc:
        case ExpressionNode::l_us:
        case ExpressionNode::l_bool:
        case ExpressionNode::l_bit:
        case ExpressionNode::l_string:
        case ExpressionNode::l_object:
        case ExpressionNode::uminus:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f:
        case ExpressionNode::x_d:
        case ExpressionNode::x_ld:
        case ExpressionNode::x_wc:
        case ExpressionNode::x_c:
        case ExpressionNode::x_uc:
        case ExpressionNode::x_u16:
        case ExpressionNode::x_u32:
        case ExpressionNode::x_bool:
        case ExpressionNode::x_s:
        case ExpressionNode::x_us:
        case ExpressionNode::x_ll:
        case ExpressionNode::x_ull:
        case ExpressionNode::x_l:
        case ExpressionNode::x_ul:
        case ExpressionNode::x_i:
        case ExpressionNode::x_ui:
        case ExpressionNode::x_inative:
        case ExpressionNode::x_unative:
        case ExpressionNode::x_p:
        case ExpressionNode::x_fp:
        case ExpressionNode::x_sp:
        case ExpressionNode::x_string:
        case ExpressionNode::x_object:
        case ExpressionNode::trapcall:
        case ExpressionNode::shiftby:
        case ExpressionNode::substack:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack:
        case ExpressionNode::savestack:
        case ExpressionNode::bits:
        case ExpressionNode::literalclass:
            rv |= fold_const(node->left);
            break;
        case ExpressionNode::cond:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right->left);
            rv |= fold_const(node->right->right);
            break;
        case ExpressionNode::atomic:
            rv |= fold_const(node->v.ad->flg);
            rv |= fold_const(node->v.ad->memoryOrder1);
            rv |= fold_const(node->v.ad->memoryOrder2);
            rv |= fold_const(node->v.ad->address);
            rv |= fold_const(node->v.ad->value);
            rv |= fold_const(node->v.ad->third);
            break;
        case ExpressionNode::ursh:
        case ExpressionNode::assign:
        case ExpressionNode::void_:
        case ExpressionNode::void_nz:
        case ExpressionNode::arraymul:
        case ExpressionNode::arraydiv:
        case ExpressionNode::div:
        case ExpressionNode::udiv:
        case ExpressionNode::umod:
        case ExpressionNode::lsh:
        case ExpressionNode::arraylsh:
        case ExpressionNode::rsh:
        case ExpressionNode::mod:
        case ExpressionNode::lor:
        case ExpressionNode::land:
        case ExpressionNode::eq:
        case ExpressionNode::ne:
        case ExpressionNode::gt:
        case ExpressionNode::ge:
        case ExpressionNode::lt:
        case ExpressionNode::le:
        case ExpressionNode::ugt:
        case ExpressionNode::uge:
        case ExpressionNode::ult:
        case ExpressionNode::ule:
        case ExpressionNode::intcall:
        case ExpressionNode::blockassign:
        case ExpressionNode::stackblock:
        case ExpressionNode::auto_inc:
        case ExpressionNode::auto_dec:
        case ExpressionNode::mp_compare:
        case ExpressionNode::_initblk:
        case ExpressionNode::_cpblk:
        case ExpressionNode::dot:
        case ExpressionNode::pointsto:
            rv |= fold_const(node->right);
        case ExpressionNode::blockclear:
        case ExpressionNode::argnopush:
        case ExpressionNode::not__lvalue:
        case ExpressionNode::lvalue:
        case ExpressionNode::_initobj:
        case ExpressionNode::_sizeof:
        case ExpressionNode::select:
            rv |= fold_const(node->left);
            break;
        case ExpressionNode::funcret:
            rv |= fold_const(node->left);
            if (node->left->type != ExpressionNode::func && node->left->type != ExpressionNode::funcret)
                *node = *node->left;
            break;
        case ExpressionNode::thisref:
            rv |= fold_const(node->left);
            if (node->left->type != ExpressionNode::func && node->left->type != ExpressionNode::funcret)
                *node = *node->left;
            break;
        case ExpressionNode::const_ruct: {
            node->v.construct.tp = SynthesizeType(node->v.construct.tp, nullptr, false);
            LEXLIST* lex = SetAlternateLex(node->v.construct.deferred);
            if (isarithmetic(node->v.construct.tp))
            {

                std::list<INITIALIZER*> *init = nullptr, *dest = nullptr;
                lex = initType(lex, nullptr, 0, StorageClass::auto_, &init, &dest, node->v.construct.tp, nullptr, false, 0);
                if (init)
                    *node = *init->front()->exp;
            }
            else
            {
                EXPRESSION* exp = anonymousVar(StorageClass::auto_, node->v.construct.tp);
                lex = initType(lex, nullptr, 0, StorageClass::auto_, &exp->v.sp->sb->init, &exp->v.sp->sb->dest, node->v.construct.tp,
                               exp->v.sp, false, 0);
            }
            SetAlternateLex(nullptr);
        }
        break;
        case ExpressionNode::func:
            if (node->v.func->sp && node->v.func->sp->sb->constexpression)
            {
                inConstantExpression++;
                rv = EvaluateConstexprFunction(node);
                inConstantExpression--;
            }
            if (!rv)
                rv |= fold_const(node->v.func->fcall);
            break;
        case ExpressionNode::mp_as_bool:
            if (node->left->type == ExpressionNode::memberptr)
            {
                node->type = ExpressionNode::c_i;
                node->left = nullptr;
                node->v.i = 1;
                rv = true;
            }
            break;
        case ExpressionNode::stmt:
        {   // constructor thunks
            auto its = node->v.stmt->begin();
            auto itse = node->v.stmt->end();
            while (its != itse && (*its)->type == StatementNode::expr)
                ++its;
            // thi originally modified the node statement...
            if (its != itse && (*its)->type == StatementNode::block)
            {
                its = (*its)->lower->begin();
                while ((*its)->type == StatementNode::varstart)
                    ++its;
                auto its1 = its;
                ++its1;
                if ((*its)->type == StatementNode::block && its1 == itse)
                {
                    its = (*its)->lower->begin();
                    while ((*its)->type == StatementNode::line || (*its)->type == StatementNode::dbgblock)
                        ++its;
                    if ((*its)->type == StatementNode::expr || (*its)->type == StatementNode::return_)
                    {
                        EXPRESSION* exp = (*its)->select;
                        optimize_for_constants(&(*its)->select);
                        if (IsConstantExpression((*its)->select, true, false))
                        {
                            *node = *(*its)->select;
                            node->noexprerr = true;
                            rv = true;
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    return rv;
}

/*

 * remove type casts from constant nodes and change their size
 */
int typedconsts(EXPRESSION* node1)
{
    int rv = false;
    if (!node1)
        return rv;
    switch (node1->type)
    {
        case ExpressionNode::nullptr_:
            node1->type = ExpressionNode::c_ui;  // change the nullptr to an int
            rv = true;
            break;
        case ExpressionNode::const_:
            optimize_for_constants(&node1->v.sp->sb->init->front()->exp);
            *node1 = *node1->v.sp->sb->init->front()->exp;
            rv = true;
            break;
        default:
            break;
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::uminus:
        case ExpressionNode::argnopush:
        case ExpressionNode::not__lvalue:
        case ExpressionNode::lvalue:
            rv |= typedconsts(node1->left);
            break;
        case ExpressionNode::cond:
            rv |= typedconsts(node1->left);
            rv |= typedconsts(node1->right->left);
            rv |= typedconsts(node1->right->right);
            break;
        case ExpressionNode::atomic:
            rv |= typedconsts(node1->v.ad->flg);
            rv |= typedconsts(node1->v.ad->memoryOrder1);
            rv |= typedconsts(node1->v.ad->memoryOrder2);
            rv |= typedconsts(node1->v.ad->address);
            rv |= typedconsts(node1->v.ad->value);
            rv |= typedconsts(node1->v.ad->third);
            break;
        case ExpressionNode::add:
        case ExpressionNode::arrayadd:
        case ExpressionNode::structadd:
        case ExpressionNode::sub:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::mul:
        case ExpressionNode::arraymul:
        case ExpressionNode::umul:
        case ExpressionNode::lsh:
        case ExpressionNode::arraylsh:
        case ExpressionNode::ursh:
        case ExpressionNode::rsh:
        case ExpressionNode::div:
        case ExpressionNode::arraydiv:
        case ExpressionNode::mod:
        case ExpressionNode::land:
        case ExpressionNode::lor:
        case ExpressionNode::and_:
        case ExpressionNode::void_:
        case ExpressionNode::void_nz:
        case ExpressionNode::assign:
        case ExpressionNode::blockassign:
        case ExpressionNode::stackblock:
        case ExpressionNode::intcall:
        case ExpressionNode::udiv:
        case ExpressionNode::umod:
        case ExpressionNode::eq:
        case ExpressionNode::ne:
        case ExpressionNode::lt:
        case ExpressionNode::le:
        case ExpressionNode::ugt:
        case ExpressionNode::uge:
        case ExpressionNode::ult:
        case ExpressionNode::ule:
        case ExpressionNode::gt:
        case ExpressionNode::ge:
        case ExpressionNode::auto_inc:
        case ExpressionNode::auto_dec:
        case ExpressionNode::mp_compare:
        case ExpressionNode::_initblk:
        case ExpressionNode::_cpblk:
        case ExpressionNode::dot:
        case ExpressionNode::pointsto:
            rv |= typedconsts(node1->right);
        case ExpressionNode::trapcall:
        case ExpressionNode::shiftby:
        case ExpressionNode::substack:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack:
        case ExpressionNode::savestack:
        case ExpressionNode::blockclear:
        case ExpressionNode::mp_as_bool:
        case ExpressionNode::thisref:
        case ExpressionNode::literalclass:
        case ExpressionNode::funcret:
        case ExpressionNode::_initobj:
        case ExpressionNode::_sizeof:
        case ExpressionNode::select:
            rv |= typedconsts(node1->left);
            break;
        case ExpressionNode::func:
            rv |= typedconsts(node1->v.func->fcall);
            break;
        case ExpressionNode::l_bool:
        case ExpressionNode::l_bit:
        case ExpressionNode::l_wc:
        case ExpressionNode::l_u16:
        case ExpressionNode::l_u32:
        case ExpressionNode::l_c:
        case ExpressionNode::l_s:
        case ExpressionNode::l_sp:
        case ExpressionNode::l_fp:
        case ExpressionNode::l_uc:
        case ExpressionNode::l_us:
        case ExpressionNode::l_p:
        case ExpressionNode::l_ref:
        case ExpressionNode::l_i:
        case ExpressionNode::l_ui:
        case ExpressionNode::l_inative:
        case ExpressionNode::l_unative:
        case ExpressionNode::l_l:
        case ExpressionNode::l_ul:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_fi:
        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::l_ull:
        case ExpressionNode::l_ll:
        case ExpressionNode::l_string:
        case ExpressionNode::l_object:
        case ExpressionNode::bits:
            if (node1->left->type == ExpressionNode::cshimref)
            {
                *node1 = *node1->left->v.exp;
            }
            else if (node1->left->type == ExpressionNode::structadd)
            {
                if (node1->left->left->type == ExpressionNode::cshimref)
                {
                    node1->left->left = node1->left->left->v.exp;
                    optimize_for_constants(&node1->left);
                    *node1 = *node1->left;
                }
                else if (node1->left->right->type == ExpressionNode::cshimref)
                {
                    node1->left->right = node1->left->right->v.exp;
                    optimize_for_constants(&node1->left);
                    *node1 = *node1->left;
                }
            }
            else if (node1->left->type == ExpressionNode::global)
            {
                if (node1->left->v.sp->sb->storage_class == StorageClass::const_ant && isintconst(node1->left->v.sp->sb->init->front()->exp))
                {
                    optimize_for_constants(&node1->v.sp->sb->init->front()->exp);
                    *node1 = *node1->left->v.sp->sb->init->front()->exp;
                    rv = true;
                }
            }
            else
            {
                rv |= typedconsts(node1->left);
            }
            break;
        case ExpressionNode::x_p:
            rv |= typedconsts(node1->left);
            if (isconstaddress(node1->left) || isintconst(node1->left))
            {
                *node1 = *node1->left;
                rv = true;
            }
            break;
        case ExpressionNode::x_ull:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_ULONGLONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ull;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ll:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_ULONGLONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ll;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_bit:
        case ExpressionNode::x_bool:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                if (isfloatconst(node1->left) || isimaginaryconst(node1->left))
                    node1->v.i = !node1->left->v.f->ValueIsZero();
                else if (iscomplexconst(node1->left))
                    node1->v.i = !node1->left->v.c->r.ValueIsZero() || !node1->left->v.c->i.ValueIsZero();
                else
                {
                    node1->v.i = Optimizer::CastToInt(ISZ_BOOLEAN, !!reint(node1->left));
                    node1->unionoffset = node1->left->unionoffset;
                }
                node1->type = ExpressionNode::c_bool;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_c:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_UCHAR, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_c;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_u16:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_U16, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_u16;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_u32:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_U32, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_u32;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_wc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_WCHAR, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_wc;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_uc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_UCHAR, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_uc;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_s:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_USHORT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_i;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_us:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_USHORT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ui;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_i:
        case ExpressionNode::x_inative:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_UINT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_i;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ui:
        case ExpressionNode::x_unative:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_UINT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ul;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_l:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_ULONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_i;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ul:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_ULONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ul;
                node1->left = nullptr;
            }
            break;
            /*#ifdef XXXXX */
        case ExpressionNode::x_f:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_FLOAT, &temp);
                node1->type = ExpressionNode::c_f;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_fi:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_IFLOAT, &temp);
                node1->type = ExpressionNode::c_fi;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_d:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_DOUBLE, &temp);
                node1->type = ExpressionNode::c_d;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_di:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_IDOUBLE, &temp);
                node1->type = ExpressionNode::c_di;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ld:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_LDOUBLE, &temp);
                node1->type = ExpressionNode::c_ld;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ldi:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                node1->type = ExpressionNode::c_ldi;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_fc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                if (isintconst(node1->left) || isfloatconst(node1->left))
                {
                    FPF temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->r = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->i.SetZero(0);
                }
                else if (isimaginaryconst(node1->left))
                {
                    FPF temp;
                    node1->left->type = ExpressionNode::c_ld;
                    temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->i = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->r.SetZero(0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = ExpressionNode::c_fc;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_dc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                if (isintconst(node1->left) || isfloatconst(node1->left))
                {
                    FPF temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->r = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->i.SetZero(0);
                }
                else if (isimaginaryconst(node1->left))
                {
                    FPF temp;
                    node1->left->type = ExpressionNode::c_ld;
                    temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->i = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->r.SetZero(0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = ExpressionNode::c_dc;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ldc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                if (isintconst(node1->left) || isfloatconst(node1->left))
                {
                    FPF temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->r = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->i.SetZero(0);
                }
                else if (isimaginaryconst(node1->left))
                {
                    FPF temp;
                    node1->left->type = ExpressionNode::c_ld;
                    temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->i = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->r.SetZero(0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = ExpressionNode::c_ldc;
                node1->left = nullptr;
            }
            break;
            /*#endif */
    }
    return rv;
}
bool toConsider(EXPRESSION* exp1, EXPRESSION* exp2)
{
    switch (exp2->type)
    {
        case ExpressionNode::add:
            // fixme : without the (architecture == ARCHITECTURE_MSIL) some complex expressions inside loops
            // get optimized wrong see x264::analyse.c around line 849
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
                if (!isarithmeticconst(exp2->left) || !isarithmeticconst(exp2->right))
                    return false;
            return true;
        case ExpressionNode::mul:
        case ExpressionNode::umul:
        case ExpressionNode::arraymul:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::eq:
        case ExpressionNode::ne:
            return true;
        default:
            return false;
    }
}
void rebalance(EXPRESSION** exp)
{
    if ((*exp)->type == ExpressionNode::void_)
    {
        rebalance(&(*exp)->left);
        if ((*exp)->right)
            rebalance(&(*exp)->right);
    }
    else
    {
        std::stack<EXPRESSION*> stk;
        stk.push(*exp);
        while (!stk.empty())
        {
            auto top = stk.top();
            stk.pop();
            top->treesize = 0;
            if (top->left)
                stk.push(top->left);
            if (top->right)
                stk.push(top->right);
        }
        *exp = Rebalance(*exp, toConsider);
    }
}
bool msilConstant(EXPRESSION* exp)
{
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        while (castvalue(exp))
            exp = exp->left;
        if (exp->type == ExpressionNode::_sizeof)
            return true;
        if (exp->type == ExpressionNode::func)
        {
            if (exp->v.func->arguments && exp->v.func->arguments->size() == 1 && !strcmp(exp->v.func->sp->name, "ToPointer"))
                return true;
        }
        if (exp->left && !msilConstant(exp->left))
            return false;
        if (exp->right && !msilConstant(exp->right))
            return false;
        switch (exp->type)
        {
            case ExpressionNode::add:
            case ExpressionNode::mul:
            case ExpressionNode::umul:
            case ExpressionNode::div:
            case ExpressionNode::udiv:
            case ExpressionNode::global:
                return true;
            default:
                return isarithmeticconst(exp);
        }
    }
    return false;
}
void RemoveSizeofOperators(EXPRESSION* constant)
{
    if (constant->left)
        RemoveSizeofOperators(constant->left);
    if (constant->right)
        RemoveSizeofOperators(constant->right);
    if (constant->type == ExpressionNode::_sizeof)
    {
        TYPE* tp = constant->left->v.tp;
        constant->type = ExpressionNode::c_ui;
        constant->left = nullptr;
        constant->v.i = tp->size;
    }
}

void optimize_for_constants(EXPRESSION** expr)
{
    if ((*expr)->type == ExpressionNode::select)
        expr = &(*expr)->left;
    int rv = true, count = 8;
    EXPRESSION* oldasidehead = asidehead;
    EXPRESSION** oldasidetail = asidetail;

    asidehead = 0;
    asidetail = &asidehead;
    while (rv && count--)
        rv = typedconsts(*expr) | fold_const(*expr) | opt0(expr);
    if (asidehead)
    {
        *asidetail = *expr;
        *expr = asidehead;
    }
    asidehead = oldasidehead;
    asidetail = oldasidetail;
    if (Optimizer::architecture != ARCHITECTURE_MSIL)
    {
        rebalance(expr);
    }
}
LEXLIST* optimized_expression(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** expr, bool commaallowed)
{
    if (commaallowed)
        lex = expression(lex, funcsp, atp, tp, expr, 0);
    else
        lex = expression_no_comma(lex, funcsp, atp, tp, expr, nullptr, 0);
    if (*tp)
    {
        optimize_for_constants(expr);
    }
    ConstExprPatch(expr);
    return lex;
}
}  // namespace Parser