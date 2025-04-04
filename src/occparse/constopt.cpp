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
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "floatconv.h"
#include "memory.h"
#include "lex.h"
#include "help.h"
#include "expr.h"
#include "ifloatconv.h"
#include "constopt.h"
#include "OptUtils.h"
#include "declcpp.h"
#include "beinterf.h"
#include "exprcpp.h"
#include "dsw.h"
#include "constexpr.h"
#include "ccerr.h"
#include "rtti.h"
#include "namespace.h"
#include "symtab.h"
#include "types.h"
#include "stmt.h"
#include "libcxx.h"
#include "overload.h"
#include "class.h"
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
    return isintconst(en) || optimizerfloatconst(en) || en->type == ExpressionNode::c_bitint_ ||
           en->type == ExpressionNode::c_ubitint_;
}

/*-------------------------------------------------------------------------*/

static ExpressionNode maxinttype(EXPRESSION* ep1, EXPRESSION* ep2)
{
    ExpressionNode type1 = ep1->type;
    ExpressionNode type2 = ep2->type;
    if (type1 == ExpressionNode::c_ull_ || type2 == ExpressionNode::c_ull_)
        return ExpressionNode::c_ull_;
    if (type1 == ExpressionNode::c_ll_ || type2 == ExpressionNode::c_ll_)
        return ExpressionNode::c_ll_;
    if (type1 == ExpressionNode::c_ul_ || type2 == ExpressionNode::c_ul_)
        return ExpressionNode::c_ul_;
    if (type1 == ExpressionNode::c_l_ || type2 == ExpressionNode::c_l_)
        return ExpressionNode::c_l_;
    if (type1 == ExpressionNode::c_u32_ || type2 == ExpressionNode::c_u32_)
        return ExpressionNode::c_u32_;
    if (type1 == ExpressionNode::c_ui_ || type2 == ExpressionNode::c_ui_)
        return ExpressionNode::c_ui_;
    if (type1 == ExpressionNode::c_i_ || type2 == ExpressionNode::c_i_)
        return ExpressionNode::c_i_;
    if (type1 == ExpressionNode::c_u16_ || type2 == ExpressionNode::c_u16_)
        return ExpressionNode::c_u16_;
    if (type1 == ExpressionNode::c_wc_ || type2 == ExpressionNode::c_wc_)
        return ExpressionNode::c_wc_;
    if (type1 == ExpressionNode::c_uc_ || type2 == ExpressionNode::c_uc_)
        return ExpressionNode::c_uc_;
    if (type1 == ExpressionNode::c_c_ || type2 == ExpressionNode::c_c_)
        return ExpressionNode::c_c_;
    return ExpressionNode::c_bool_;
}

/*-------------------------------------------------------------------------*/

static int isunsignedexpr(EXPRESSION* ep1)
{
    switch (ep1->type)
    {
        case ExpressionNode::c_ul_:
        case ExpressionNode::c_ui_:
        case ExpressionNode::c_ull_:
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
    if (type1 == ExpressionNode::c_ld_ || type2 == ExpressionNode::c_ld_ || type1 == ExpressionNode::c_ldi_ ||
        type2 == ExpressionNode::c_ldi_)
        return ExpressionNode::c_ld_;
    if (type1 == ExpressionNode::c_d_ || type2 == ExpressionNode::c_d_ || type1 == ExpressionNode::c_di_ ||
        type2 == ExpressionNode::c_di_)
        return ExpressionNode::c_d_;
    return ExpressionNode::c_f_;
}

/*-------------------------------------------------------------------------*/

static ExpressionNode maximaginarytype(EXPRESSION* ep1, EXPRESSION* ep2)
{
    ExpressionNode type1 = ep1->type;
    ExpressionNode type2 = ep2->type;
    if (type1 == ExpressionNode::c_ld_ || type2 == ExpressionNode::c_ld_ || type1 == ExpressionNode::c_ldi_ ||
        type2 == ExpressionNode::c_ldi_)
        return ExpressionNode::c_ldi_;
    if (type1 == ExpressionNode::c_d_ || type2 == ExpressionNode::c_d_ || type1 == ExpressionNode::c_di_ ||
        type2 == ExpressionNode::c_di_)
        return ExpressionNode::c_di_;
    return ExpressionNode::c_fi_;
}

/*-------------------------------------------------------------------------*/

static ExpressionNode maxcomplextype(EXPRESSION* ep1, EXPRESSION* ep2)
{
    ExpressionNode type1 = ep1->type;
    ExpressionNode type2 = ep2->type;
    if (type1 == ExpressionNode::c_ld_ || type2 == ExpressionNode::c_ld_ || type1 == ExpressionNode::c_ldi_ ||
        type2 == ExpressionNode::c_ldi_ || type1 == ExpressionNode::c_ldc_ || type2 == ExpressionNode::c_ldc_)
        return ExpressionNode::c_ldc_;
    if (type1 == ExpressionNode::c_d_ || type2 == ExpressionNode::c_d_ || type1 == ExpressionNode::c_di_ ||
        type2 == ExpressionNode::c_di_ || type1 == ExpressionNode::c_dc_ || type2 == ExpressionNode::c_dc_)
        return ExpressionNode::c_dc_;
    return ExpressionNode::c_fc_;
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
        case ExpressionNode::x_f_:
        case ExpressionNode::x_d_:
        case ExpressionNode::x_ld_:
        case ExpressionNode::x_fc_:
        case ExpressionNode::x_dc_:
        case ExpressionNode::x_ldc_:
        case ExpressionNode::x_fi_:
        case ExpressionNode::x_di_:
        case ExpressionNode::x_ldi_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::c_f_:
        case ExpressionNode::c_d_:
        case ExpressionNode::c_ld_:
        case ExpressionNode::c_fc_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::c_ldc_:
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
            return 1;
        case ExpressionNode::assign_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::add_:
        case ExpressionNode::sub_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::arraymul_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::structadd_:
        case ExpressionNode::umul_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::mul_:
        case ExpressionNode::div_:
        case ExpressionNode::mod_:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::ursh_:
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::land_:
        case ExpressionNode::lor_:
        case ExpressionNode::comma_:
        case ExpressionNode::check_nz_:
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
            return (hasFloats(node->left) || hasFloats(node->right));
        case ExpressionNode::hook_:
            return hasFloats(node->right);
        case ExpressionNode::argnopush_:
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::thisref_:
        case ExpressionNode::lvalue_:
        case ExpressionNode::select_:
        case ExpressionNode::constexprconstructor_:
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
        else if (ep2->type == ExpressionNode::c_d_ || ep2->type == ExpressionNode::c_ld_ || ep2->type == ExpressionNode::c_f_)
            mode = 8;
        else if (ep2->type == ExpressionNode::c_di_ || ep2->type == ExpressionNode::c_fi_ || ep2->type == ExpressionNode::c_ldi_)
            mode = 13;
        else if (ep2->type == ExpressionNode::c_dc_ || ep2->type == ExpressionNode::c_fc_ || ep2->type == ExpressionNode::c_ldc_)
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
        case BasicType::signed_char_:
            bits = 7;
            break;
        case BasicType::unsigned_char_:
            bits = 8;
            break;
        case BasicType::short_:
            bits = getSize(BasicType::short_) * 8 - 1;
            break;
        case BasicType::unsigned_short_:
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
            ep1->type = ExpressionNode::c_d_;
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
            ep2->type = ExpressionNode::c_d_;
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
            case ExpressionNode::structadd_:
                if (ep->right->type == ExpressionNode::structelem_)
                    break;
                if (ep->right->keepZero)
                    break;
            case ExpressionNode::add_:
            case ExpressionNode::arrayadd_:
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
            case ExpressionNode::sub_:
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
            case ExpressionNode::arraymul_:
            case ExpressionNode::umul_:
            case ExpressionNode::mul_:
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
            case ExpressionNode::arraydiv_:
            case ExpressionNode::div_:
            case ExpressionNode::udiv_:
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
                        ep->type = ExpressionNode::mul_;
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
            case ExpressionNode::mod_:
            case ExpressionNode::umod_:
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
            case ExpressionNode::arraylsh_:
            case ExpressionNode::lsh_:
                ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
                ep->left = ep->right = nullptr;
                ep->v.i = ep1->v.i << ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case ExpressionNode::ursh_:
                ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
                ep->left = ep->right = nullptr;
                ep1->v.i = reint(ep1);
                ep->v.i = ((unsigned long long)ep1->v.i) >> ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case ExpressionNode::rsh_:
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
    *asidetail = MakeExpression(ExpressionNode::comma_, node);
    asidetail = &(*asidetail)->right;
}

/*-------------------------------------------------------------------------*/
bool expressionHasSideEffects(EXPRESSION* exp)
{
    std::stack<EXPRESSION*> stk;
    stk.push(exp);
    while (!stk.empty())
    {
        auto p = stk.top();
        stk.pop();
        switch (p->type)
        {
            case ExpressionNode::atomic_:
            case ExpressionNode::callsite_:
            case ExpressionNode::thisref_:
            case ExpressionNode::assign_:
            case ExpressionNode::auto_inc_:
            case ExpressionNode::auto_dec_:
            case ExpressionNode::constexprconstructor_:
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
 *      x * 1, 0 / x, x / 1, x Keyword::mod_ 0, etc from the tree pointed to
 *      by node and combine obvious constant operations. It cannot
 *      combine name and label constants but will combine icon type
 *      nodes.
 *
 * ExpressionNode::arrayadd_ is not optimized to keep from optimizing struct members
 * into regs later...  just en: ExpressionNode::arrayadd_ is now equiv to ExpressionNode::add_
 * again.  I have fixed the optimize struct thing another way...
 * leaving ExpressionNode::arrayadd_ in in case we need it later.
 */
{
    EXPRESSION* ep;
    long long val;
    int rv = false;
    int mode;
    FPF dval;
    ExpressionNode negtype = ExpressionNode::uminus_;

    ep = *node;

    if (ep == 0)
        return false;
    switch (ep->type)
    {
        case ExpressionNode::construct_:
            break;
        case ExpressionNode::l_sp_:
        case ExpressionNode::l_fp_:
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_s_:
            /* optimize unary node */
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_us_:
            /* optimize unary node */
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_bitint_:
        case ExpressionNode::l_ubitint_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_string_:
        case ExpressionNode::l_object_: {
            rv |= opt0(&((*node)->left));
            rv |= ResolveConstExprLval(node);
        }
        break;
        case ExpressionNode::x_bool_:
        case ExpressionNode::x_bit_:
        case ExpressionNode::x_wc_:
        case ExpressionNode::x_c_:
        case ExpressionNode::x_uc_:
        case ExpressionNode::x_u16_:
        case ExpressionNode::x_u32_:
        case ExpressionNode::x_s_:
        case ExpressionNode::x_us_:
        case ExpressionNode::x_l_:
        case ExpressionNode::x_ul_:
        case ExpressionNode::x_i_:
        case ExpressionNode::x_ui_:
        case ExpressionNode::x_bitint_:
        case ExpressionNode::x_ubitint_:
        case ExpressionNode::x_inative_:
        case ExpressionNode::x_unative_:
        case ExpressionNode::x_ll_:
        case ExpressionNode::x_ull_:
        case ExpressionNode::x_f_:
        case ExpressionNode::x_d_:
        case ExpressionNode::x_p_:
        case ExpressionNode::x_ld_:
        case ExpressionNode::x_fp_:
        case ExpressionNode::x_sp_:
        case ExpressionNode::x_dc_:
        case ExpressionNode::x_fc_:
        case ExpressionNode::x_ldc_:
        case ExpressionNode::x_di_:
        case ExpressionNode::x_fi_:
        case ExpressionNode::x_ldi_:
        case ExpressionNode::x_string_:
        case ExpressionNode::x_object_:
        case ExpressionNode::shiftby_:
        case ExpressionNode::bits_:
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
            else if (ep->left->type == ExpressionNode::c_bitint_ || ep->left->type == ExpressionNode::c_ubitint_)
            {
                int sz = ep->left->v.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
                sz /= Optimizer::chosenAssembler->arch->bitintunderlying;
                sz *= Optimizer::chosenAssembler->arch->bitintunderlying;
                sz /= CHAR_BIT;
                ep->type = ep->left->type;
                ep->v.b.bits = ep->left->v.b.bits;
                ep->v.b.value = make_bitint(ep->v.b.bits, ep->left->v.b.value);
                for (int i = 0; i < sz; i++)
                {
                    ep->v.b.value[i] = ~ep->v.b.value[i];
                }
            }
            return rv;

        case ExpressionNode::uminus_:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                *node = MakeIntExpression(ep->left->type, -ep->left->v.i);
                rv = true;
            }
            else if (ep->left->type == ExpressionNode::c_bitint_ || ep->left->type == ExpressionNode::c_ubitint_)
            {
                int sz = ep->left->v.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
                sz /= Optimizer::chosenAssembler->arch->bitintunderlying;
                sz *= Optimizer::chosenAssembler->arch->bitintunderlying;
                sz /= CHAR_BIT;
                ep->type = ep->left->type;
                ep->v.b.bits = ep->left->v.b.bits;
                ep->v.b.value = make_bitint(ep->v.b.bits, ep->left->v.b.value);
                int carry = 0, c1 = 0;
                for (int i = 0; i < sz; i++)
                {
                    if (!carry)
                        c1 = ep->v.b.value[i] == 0 ? 0 : 1;
                    ep->v.b.value[i] = -ep->v.b.value[i] - carry;
                    carry = carry ? 1 : c1;
                }
            }
            else if (isfloatconst(ep->left))
            {
                *node = MakeIntExpression(ep->left->type, 0);
                (*node)->v.f = ep->left->v.f;
                (*node)->v.f->Negate();
                rv = true;
            }
            else if (ep->left->type == ExpressionNode::c_d_ || ep->left->type == ExpressionNode::c_f_ ||
                     ep->left->type == ExpressionNode::c_ld_ || ep->left->type == ExpressionNode::c_di_ ||
                     ep->left->type == ExpressionNode::c_fi_ || ep->left->type == ExpressionNode::c_ldi_)
            {
                rv = true;
                ep->type = ep->left->type;
                ep->v.f = Allocate<FPF>();
                (*ep->v.f) = *ep->left->v.f;
                ep->v.f->Negate();
                *node = ep;
                ep->left = ep->right = nullptr;
            }
            else if (ep->left->type == ExpressionNode::c_dc_ || ep->left->type == ExpressionNode::c_fc_ ||
                     ep->left->type == ExpressionNode::c_ldc_)
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
        case ExpressionNode::structadd_:
        case ExpressionNode::arrayadd_:
            /* can't do this here*/
            /*
            while (ep->left->type == ExpressionNode::comma_)
                ep->left = ep->left->right;
            while (ep->right->type == ExpressionNode::comma_)
                ep->right = ep->right->right;
                */
            // fallthrough
        case ExpressionNode::add_:
        case ExpressionNode::sub_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            if (ep->right->type == ExpressionNode::structelem_ || ep->left->type == ExpressionNode::structadd_)
                break;
            if (ep->type == ExpressionNode::sub_)
            {
                // this next will normalize expressions of the form:
                // z = (a + 5) - a
                // to z = 5;
                // regardless of whether a can be evaluated
                int offset1 = 0;
                int offset2 = 0;
                auto rv1 = relptr(ep->left, offset1);
                auto rv2 = relptr(ep->right, offset2);
                if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                {
                    (*node)->type = ExpressionNode::c_i_;
                    (*node)->left = nullptr;
                    (*node)->right = nullptr;
                    (*node)->v.i = offset1 - offset2;
                    break;
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
                        if (ep->left->type != ExpressionNode::auto_ && ep->type != ExpressionNode::structadd_)
                        {
                            if (ep->type == ExpressionNode::sub_)
                                *node = MakeExpression(ExpressionNode::uminus_, ep->right);
                            else
                                *node = ep->right;
                            rv = true;
                        }
                    }
                    break;
                case 6:
                    if (ep->left->v.f->ValueIsZero())
                    {
                        if (ep->type == ExpressionNode::sub_)
                        {
                            *node = MakeExpression(ExpressionNode::uminus_, ep->right);
                        }
                        else
                        {
                            *node = ep->right;
                        }
                    }
                    else
                        dooper(node, mode);
                    rv = true;
                    break;
                case 7:
                    if (ep->right->v.i == 0)
                    {
                        if (ep->left->type != ExpressionNode::auto_ && ep->type != ExpressionNode::structadd_)
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
                    {
                        dooper(node, mode);
                    }
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
        case ExpressionNode::mul_:
        case ExpressionNode::umul_:
        case ExpressionNode::arraymul_:
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
                            if (ep->right->type != ExpressionNode::sizeof_)
                            {
                                addaside(ep->right);
                                *node = ep->left;
                                rv = true;
                            }
                        }
                        else if (val == 1)
                            *node = ep->right;
                        else if (val == -1)
                            *node = MakeExpression(negtype, ep->right);
                        else
                        {
                            long long i = Optimizer::pwrof2(val);
                            if (i != -1)
                            {
                                EXPRESSION* x = ep->left;
                                ep->left = ep->right;
                                ep->right = x;
                                ep->right->v.i = i;
                                ep->right->type = ExpressionNode::c_i_;
                                ep->right->left = ep->right->right = nullptr;
                                rv = true;
                                switch (ep->type)
                                {
                                    case ExpressionNode::mul_:
                                        ep->type = ExpressionNode::lsh_;
                                        break;
                                    case ExpressionNode::umul_:
                                        ep->type = ExpressionNode::lsh_;
                                        break;
                                    case ExpressionNode::arraymul_:
                                        ep->type = ExpressionNode::arraylsh_;
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
                        *node = MakeExpression(negtype, ep->right);
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
                            *node = MakeExpression(negtype, ep->left);
                        }
                        else
                        {
                            long long i = Optimizer::pwrof2(val);
                            if (i != -1)
                            {
                                ep->right->v.i = i;
                                ep->right->type = ExpressionNode::c_i_;
                                ep->right->left = ep->right->right = nullptr;
                                rv = true;
                                switch (ep->type)
                                {
                                    case ExpressionNode::mul_:
                                        ep->type = ExpressionNode::lsh_;
                                        break;
                                    case ExpressionNode::umul_:
                                        ep->type = ExpressionNode::lsh_;
                                        break;
                                    case ExpressionNode::arraymul_:
                                        ep->type = ExpressionNode::arraylsh_;
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
                        *node = MakeExpression(negtype, ep->left);
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
                            *node = MakeExpression(negtype, ep->left);
                        }
                        else
                            dooper(node, mode);
                    }
            }
            break;
        case ExpressionNode::arraydiv_:
        case ExpressionNode::div_:
        case ExpressionNode::udiv_:
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
                            *node = MakeExpression(negtype, ep->left);
                    }
                    dooper(node, mode);
                    rv = true;
                    break;
                case 8:
                    dval = *ep->right->v.f;
                    if (!dval.ValueIsNegative() && dval.ValueIsOne())
                        *node = ep->left;
                    if (dval.ValueIsNegative() && dval.ValueIsOne())
                        *node = MakeExpression(negtype, ep->left);
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
                            *node = MakeExpression(negtype, ep->left);
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
        case ExpressionNode::mod_:
        case ExpressionNode::umod_:
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
                        ep->type = ExpressionNode::c_i_;
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
        case ExpressionNode::rsh_:
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::ursh_:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left) && ep->right->type == ExpressionNode::shiftby_)
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
        case ExpressionNode::land_:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                if (!ep->left->v.i)
                {
                    rv = true;
                    *node = MakeIntExpression(ExpressionNode::c_i_, 0);
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
                    case ExpressionNode::pc_:
                    case ExpressionNode::threadlocal_:
                    case ExpressionNode::global_:
                    case ExpressionNode::labcon_:
                    case ExpressionNode::auto_: {
                    join_land:
                        rv |= opt0(&(ep->right));
                        if (isintconst(ep->right))
                        {
                            rv = true;
                            *node = MakeIntExpression(ExpressionNode::c_i_, !!ep->right->v.i);
                        }
                        else
                            switch (ep->right->type)
                            {
                                case ExpressionNode::pc_:
                                case ExpressionNode::threadlocal_:
                                case ExpressionNode::global_:
                                case ExpressionNode::labcon_:
                                case ExpressionNode::auto_:
                                    /* assumes nothing can be relocated to address 0 */
                                    *node = MakeIntExpression(ExpressionNode::c_i_, 1);
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
        case ExpressionNode::lor_:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                if (ep->left->v.i)
                {
                    rv = true;
                    *node = MakeIntExpression(ExpressionNode::c_i_, 1);
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
                    case ExpressionNode::pc_:
                    case ExpressionNode::threadlocal_:
                    case ExpressionNode::global_:
                    case ExpressionNode::labcon_:
                    case ExpressionNode::auto_:
                        /* assumes nothing can be relocated to address 0 */
                        *node = MakeIntExpression(ExpressionNode::c_i_, 1);
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
                                *node = MakeIntExpression(ExpressionNode::c_i_, 1);
                            }
                            else if (isintconst(ep->left) && !ep->left->v.i)
                            {
                                rv = true;
                                *node = MakeIntExpression(ExpressionNode::c_i_, 0);
                            }
                        }
                        else
                            switch (ep->right->type)
                            {
                                case ExpressionNode::pc_:
                                case ExpressionNode::threadlocal_:
                                case ExpressionNode::global_:
                                case ExpressionNode::labcon_:
                                case ExpressionNode::auto_:
                                    /* assumes nothing can be relocated to address 0 */
                                    *node = MakeIntExpression(ExpressionNode::c_i_, 1);
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
                *node = MakeIntExpression(ExpressionNode::c_i_, (!ep->left->v.i));
                rv = true;
            }
            else if (isfloatconst(ep->left))
            {
                *node = MakeIntExpression(ExpressionNode::c_i_, ep->left->v.f->ValueIsZero());
                rv = true;
            }
            break;
        case ExpressionNode::eq_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (ep->left->v.i == ep->right->v.i));
                    rv = true;

                    break;
                case 4:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (*ep->left->v.f == *ep->right->v.f));
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
                            *node = MakeIntExpression(ExpressionNode::c_i_, 0);
                        }
                        else
                        {
                            *node = MakeIntExpression(ExpressionNode::c_i_, 1);
                        }
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ne_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (ep->left->v.i != ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (*ep->left->v.f != *ep->right->v.f));
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
                            *node = MakeIntExpression(ExpressionNode::c_i_, 1);
                        }
                        else
                        {
                            *node = MakeIntExpression(ExpressionNode::c_i_, 0);
                        }
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::lt_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (ep->left->v.i < ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (*ep->left->v.f < *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 < offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::le_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (ep->left->v.i <= ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (*ep->left->v.f <= *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 <= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ugt_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_,
                                              ((unsigned long long)ep->left->v.i > (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 > offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::uge_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_,
                                              ((unsigned long long)ep->left->v.i >= (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 >= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ult_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_,
                                              ((unsigned long long)ep->left->v.i < (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 < offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ule_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_,
                                              ((unsigned long long)ep->left->v.i <= (unsigned long long)ep->right->v.i));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 <= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::gt_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (ep->left->v.i > ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (*ep->left->v.f > *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 > offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;
        case ExpressionNode::ge_:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
                case 1:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (ep->left->v.i >= ep->right->v.i));
                    rv = true;
                    break;
                case 4:
                    *node = MakeIntExpression(ExpressionNode::c_i_, (*ep->left->v.f > *ep->right->v.f));
                    rv = true;
                    break;
                default: {
                    int offset1 = 0, offset2 = 0;
                    auto rv1 = relptr(ep->left, offset1, true);
                    auto rv2 = relptr(ep->right, offset2, true);
                    if (rv1 && rv2 && rv1->v.sp == rv2->v.sp)
                    {
                        *node = MakeIntExpression(ExpressionNode::c_i_, offset1 >= offset2);
                        rv = true;
                    }
                }
                break;
            }
            break;

        case ExpressionNode::hook_:
            rv |= opt0(&(ep->right));
            rv |= opt0(&(ep->left));
            if (isoptconst(ep->left) &&
                (parsingPreprocessorConstant || isoptconst(ep->right->left) && isoptconst(ep->right->right)))
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
        case ExpressionNode::comma_:
        case ExpressionNode::intcall_:
        case ExpressionNode::check_nz_:
        case ExpressionNode::assign_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::stackblock_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
            rv |= opt0(&(ep->right));
        case ExpressionNode::trapcall_:
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::argnopush_:
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
        case ExpressionNode::thisref_:
        case ExpressionNode::funcret_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
        case ExpressionNode::select_:
        case ExpressionNode::constexprconstructor_:
            rv |= opt0(&(ep->left));
            break;
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
            if (!definingTemplate || instantiatingTemplate)
            {
                EXPRESSION* newExpr = ep->left;
                EXPRESSION* next = ep->right;
                Type* tp = LookupTypeFromExpression(ep->left, nullptr, false);
                while (ep->type == ExpressionNode::dot_ || ep->type == ExpressionNode::pointsto_)
                {
                    rv |= opt0(&(ep->right));
                    rv |= opt0(&(ep->left));
                    if (ep->type == ExpressionNode::pointsto_)
                    {
                        if (!tp->IsPtr())
                            break;
                        tp = tp->btp->BaseType();
                        Dereference(&stdpointer, &newExpr);
                    }
                    if (!tp->IsStructured())
                        break;
                    if (next->type == ExpressionNode::dot_ || next->type == ExpressionNode::pointsto_)
                    {
                        next = next->left;
                    }
                    enclosingDeclarations.Add(tp->BaseType()->sp);
                    if (next->type == ExpressionNode::callsite_)
                    {
                        Type* ctype = tp;
                        SYMBOL* sym = classsearch(next->v.func->sp->name, false, false, false);
                        if (!sym)
                        {
                            enclosingDeclarations.Drop();
                            break;
                        }
                        CallSite* func = Allocate<CallSite>();
                        *func = *next->v.func;
                        func->sp = sym;
                        func->thistp = Type::MakeType(BasicType::pointer_, tp);
                        func->thisptr = newExpr;
                        sym = GetOverloadedFunction(&ctype, &func->fcall, sym, func, nullptr, true, false, 0);
                        if (!sym)
                        {
                            enclosingDeclarations.Drop();
                            break;
                        }
                        EXPRESSION* temp = MakeExpression(next->v.func);
                        temp = MakeExpression(ExpressionNode::thisref_, temp);
                        temp->v.t.thisptr = newExpr;
                        temp->v.t.tp = tp;
                        newExpr = temp;
                        tp = sym->tp->BaseType()->btp;
                    }
                    else
                    {
                        SYMBOL* sym = classsearch(GetSymRef(next)->v.sp->name, false, false, false);
                        if (!sym)
                        {
                            enclosingDeclarations.Drop();
                            break;
                        }
                        EXPRESSION* temp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        if (sym->sb->parentClass != tp->BaseType()->sp)
                        {
                            temp = baseClassOffset(tp->BaseType()->sp, sym->sb->parentClass, temp);
                        }
                        newExpr = MakeExpression(ExpressionNode::structadd_, newExpr, temp);
                        if (!sym->tp->IsStructured())
                            Dereference(sym->tp, &newExpr);
                        tp = sym->tp;
                    }
                    enclosingDeclarations.Drop();
                    ep = ep->right;
                }
                if (ep->type == ExpressionNode::dot_ || ep->type == ExpressionNode::pointsto_)
                    rv = false;
                else
                    *node = newExpr;
            }
            break;
        case ExpressionNode::callsite_:
            // rv |= opt0(&((*node)->v.func->fcall));
            if ((*node)->v.func->thisptr)
                rv |= opt0(&((*node)->v.func->thisptr));
            return rv;
        case ExpressionNode::atomic_:
            rv |= opt0(&((*node)->v.ad->flg));
            rv |= opt0(&((*node)->v.ad->memoryOrder1));
            rv |= opt0(&((*node)->v.ad->memoryOrder2));
            rv |= opt0(&((*node)->v.ad->third));
            rv |= opt0(&((*node)->v.ad->address));
            rv |= opt0(&((*node)->v.ad->value));
            return rv;
        case ExpressionNode::sizeofellipse_:
            if (!definingTemplate || instantiatingTemplate)
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
                *node = MakeIntExpression(ExpressionNode::c_i_, n);
            }
            break;
        case ExpressionNode::templateselector_:
            if (!definingTemplate || instantiatingTemplate)
            {
                auto tsl = (*node)->v.templateSelector;
                SYMBOL* ts = (*tsl)[1].sp;
                SYMBOL* sym = ts;
                if ((*tsl)[1].isDeclType)
                {
                    Type* tp = TemplateLookupTypeFromDeclType((*tsl)[1].tp);
                    if (tp && tp->IsStructured())
                        sym = tp->BaseType()->sp;
                    else
                        sym = nullptr;
                }
                else
                {
                    if (ts->tp->type == BasicType::templateparam_)
                    {
                        if (ts->tp->templateParam->second->type != TplType::template_)
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
                    if (sym && sym->tp->type == BasicType::templateselector_)
                    {
                        Type* tp = sym->tp;
                        tp = SynthesizeType(tp, nullptr, false);
                        if (tp && tp->IsStructured())
                            sym = tp->BaseType()->sp;
                    }
                }
                if (sym)
                {
                    sym->tp = sym->tp->InitializeDeferred();
                    sym = sym->tp->BaseType()->sp;
                    if (sym)
                    {
                        auto find = (*tsl).begin();
                        ++find;
                        ++find;
                        while (find != (*tsl).end() && sym)
                        {
                            SYMBOL* spo = sym;
                            if (!spo->tp->IsStructured())
                                break;

                            sym = search(spo->tp->syms, (*find).name);
                            if (!sym)
                            {
                                sym = classdata((*find).name, spo, nullptr, false, false);
                                if (sym == (SYMBOL*)-1)
                                    sym = nullptr;
                            }
                            if (sym)
                            {
                                sym->tp->InstantiateDeferred();
                            }
                            if (sym && (*find).asCall)
                            {
                                if ((*find).arguments)
                                    for (auto i : *(*find).arguments)
                                    {
                                        i->tp = SynthesizeType(i->tp, nullptr, false);
                                    }
                                Type* ctype = sym->tp;
                                EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                                CallSite funcparams = {};
                                funcparams.arguments = (*find).arguments;
                                funcparams.templateParams = (*find).templateParams;
                                funcparams.ascall = true;

                                auto sp1 = GetOverloadedFunction(&ctype, &exp, sym, &funcparams, nullptr, false, false, 0);
                                if (sp1)
                                {
                                    EXPRESSION exp1 = {}, *exp2 = &exp1;
                                    ;
                                    funcparams.fcall = exp;
                                    funcparams.sp = sp1;
                                    funcparams.functp = sp1->tp;
                                    //                                    funcparams.templateParams = nullptr;
                                    exp1.type = ExpressionNode::callsite_;
                                    exp1.v.func = &funcparams;
                                    optimize_for_constants(&exp2);
                                    if (exp1.type != ExpressionNode::thisref_ && exp1.type != ExpressionNode::callsite_)
                                    {
                                        *node = copy_expression(&exp1);
                                        return true;
                                    }
                                }
                            }
                            ++find;
                        }
                        if (find == (*tsl).end() && sym)
                        {
                            if (sym->sb->storage_class == StorageClass::constant_)
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
        case ExpressionNode::templateparam_:
            if ((!definingTemplate || instantiatingTemplate) &&
                (*node)->v.sp->tp->BaseType()->templateParam->second->type == TplType::int_)
            {
                SYMBOL* sym = (*node)->v.sp;
                TEMPLATEPARAMPAIR* found = (*node)->v.sp->tp->templateParam;
                if (!found || !found->second->byNonType.val)
                    found = nullptr;
                for (auto&& search : enclosingDeclarations)
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
                if (found && found->second->type == TplType::int_)
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
        case ExpressionNode::add_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::structadd_:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (!inConstantExpression &&
                (node->right->type == ExpressionNode::structelem_ || node->left->type == ExpressionNode::structadd_))
                break;
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                    case ExpressionNode::add_:
                    case ExpressionNode::arrayadd_:
                    case ExpressionNode::structadd_:
                        if (isoptconst(node->left->left))
                        {
                            ExpressionNode type = ExpressionNode::add_;
                            if (node->type == ExpressionNode::arrayadd_ || node->left->type == ExpressionNode::arrayadd_)
                                type = ExpressionNode::arrayadd_;
                            if (node->type == ExpressionNode::structadd_ || node->left->type == ExpressionNode::structadd_)
                                type = ExpressionNode::structadd_;
                            node->type = type;
                            node->left->type = ExpressionNode::add_;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            ExpressionNode type = ExpressionNode::add_;

                            if (node->type == ExpressionNode::arrayadd_ || node->left->type == ExpressionNode::arrayadd_)
                                type = ExpressionNode::arrayadd_;
                            if (node->type == ExpressionNode::structadd_ || node->left->type == ExpressionNode::structadd_)
                                type = ExpressionNode::structadd_;
                            node->type = type;
                            node->left->type = ExpressionNode::add_;
                            enswap(&node->left->left, &node->right);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub_:
                        if (isoptconst(node->left->left))
                        {
                            ExpressionNode type = ExpressionNode::add_;
                            if (node->type == ExpressionNode::arrayadd_)
                                type = ExpressionNode::arrayadd_;
                            if (node->type == ExpressionNode::structadd_)
                                type = ExpressionNode::structadd_;
                            node->type = ExpressionNode::sub_;
                            node->left->type = type;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            ExpressionNode type = ExpressionNode::add_;
                            if (node->type == ExpressionNode::arrayadd_)
                                type = ExpressionNode::arrayadd_;
                            if (node->type == ExpressionNode::structadd_)
                                type = ExpressionNode::structadd_;
                            node->type = type;
                            node->left->type = ExpressionNode::sub_;
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
                    case ExpressionNode::add_:
                    case ExpressionNode::arrayadd_:
                    case ExpressionNode::structadd_:
                        if (isoptconst(node->right->left))
                        {
                            ExpressionNode type = ExpressionNode::add_;
                            if (node->type == ExpressionNode::arrayadd_ || node->right->type == ExpressionNode::arrayadd_)
                                type = ExpressionNode::arrayadd_;
                            if (node->type == ExpressionNode::structadd_ || node->right->type == ExpressionNode::structadd_)
                                type = ExpressionNode::structadd_;
                            node->type = type;
                            node->right->type = ExpressionNode::add_;
                            enswap(&node->right->right, &node->left);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            ExpressionNode type = ExpressionNode::add_;
                            if (node->type == ExpressionNode::arrayadd_ || node->right->type == ExpressionNode::arrayadd_)
                                type = ExpressionNode::arrayadd_;
                            if (node->type == ExpressionNode::structadd_ || node->right->type == ExpressionNode::structadd_)
                                type = ExpressionNode::structadd_;
                            node->type = type;
                            node->right->type = ExpressionNode::add_;
                            enswap(&node->right->left, &node->left);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub_:
                        if (isoptconst(node->right->left))
                        {
                            node->type = ExpressionNode::sub_;
                            node->right->type = ExpressionNode::add_;
                            enswap(&node->left, &node->right);
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            ExpressionNode type = ExpressionNode::add_;
                            if (node->type == ExpressionNode::arrayadd_)
                                type = ExpressionNode::arrayadd_;
                            if (node->type == ExpressionNode::structadd_)
                                type = ExpressionNode::structadd_;
                            node->type = type;
                            node->right->type = ExpressionNode::sub_;
                            enswap(&node->right->left, &node->left);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case ExpressionNode::sub_:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                    case ExpressionNode::add_:
                    case ExpressionNode::arrayadd_:
                    case ExpressionNode::structadd_:
                        if (isoptconst(node->left->left))
                        {
                            node->type = ExpressionNode::add_;
                            node->left->type = ExpressionNode::sub_;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            node->type = ExpressionNode::add_;
                            node->left->type = ExpressionNode::sub_;
                            enswap(&node->left->left, &node->left->right);
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub_:
                        if (isoptconst(node->left->left))
                        {
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            enswap(&node->left, &node->right);
                            enswap(&node->right->left, &node->left);
                            node->type = ExpressionNode::add_;
                            node->right->left = MakeExpression(ExpressionNode::uminus_, node->right->left);
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
                    case ExpressionNode::add_:
                    case ExpressionNode::arrayadd_:
                    case ExpressionNode::structadd_:
                        if (isoptconst(node->right->left))
                        {
                            node->type = ExpressionNode::sub_;
                            node->right->type = ExpressionNode::sub_;
                            enswap(&node->left, &node->right);
                            enswap(&node->left->right, &node->right);
                            enswap(&node->left->left, &node->left->right);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            node->type = ExpressionNode::sub_;
                            node->right->type = ExpressionNode::sub_;
                            enswap(&node->left, &node->right);
                            enswap(&node->left->left, &node->right);
                            rv = true;
                        }
                        break;
                    case ExpressionNode::sub_:
                        if (isoptconst(node->right->left))
                        {
                            enswap(&node->right->right, &node->left);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {

                            enswap(&node->left, &node->right);
                            enswap(&node->left->left, &node->right);
                            node->left->right = MakeExpression(ExpressionNode::uminus_, node->left->right);
                            rv = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case ExpressionNode::mul_:
        case ExpressionNode::umul_:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                    case ExpressionNode::mul_:
                    case ExpressionNode::umul_:
                        if (isoptconst(node->left->left))
                        {
                            node->type = ExpressionNode::mul_;
                            node->left->type = ExpressionNode::mul_;
                            enswap(&node->left->right, &node->right);
                            rv = true;
                        }
                        else if (isoptconst(node->left->right))
                        {
                            node->type = ExpressionNode::mul_;
                            node->left->type = ExpressionNode::mul_;
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
                    case ExpressionNode::mul_:
                    case ExpressionNode::umul_:
                        if (isoptconst(node->right->left))
                        {
                            node->type = ExpressionNode::mul_;
                            node->right->type = ExpressionNode::mul_;
                            enswap(&node->right->right, &node->left);
                            rv = true;
                        }
                        else if (isoptconst(node->right->right))
                        {
                            node->type = ExpressionNode::mul_;
                            node->right->type = ExpressionNode::mul_;
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
        case ExpressionNode::l_sp_:
        case ExpressionNode::l_fp_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::x_fi_:
        case ExpressionNode::x_di_:
        case ExpressionNode::x_ldi_:
        case ExpressionNode::x_fc_:
        case ExpressionNode::x_dc_:
        case ExpressionNode::x_ldc_:
        case ExpressionNode::x_bit_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_s_:
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_us_:
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_bitint_:
        case ExpressionNode::l_ubitint_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_string_:
        case ExpressionNode::l_object_:
        case ExpressionNode::uminus_:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f_:
        case ExpressionNode::x_d_:
        case ExpressionNode::x_ld_:
        case ExpressionNode::x_wc_:
        case ExpressionNode::x_c_:
        case ExpressionNode::x_uc_:
        case ExpressionNode::x_u16_:
        case ExpressionNode::x_u32_:
        case ExpressionNode::x_bool_:
        case ExpressionNode::x_s_:
        case ExpressionNode::x_us_:
        case ExpressionNode::x_ll_:
        case ExpressionNode::x_ull_:
        case ExpressionNode::x_l_:
        case ExpressionNode::x_ul_:
        case ExpressionNode::x_i_:
        case ExpressionNode::x_ui_:
        case ExpressionNode::x_bitint_:
        case ExpressionNode::x_ubitint_:
        case ExpressionNode::x_inative_:
        case ExpressionNode::x_unative_:
        case ExpressionNode::x_p_:
        case ExpressionNode::x_fp_:
        case ExpressionNode::x_sp_:
        case ExpressionNode::x_string_:
        case ExpressionNode::x_object_:
        case ExpressionNode::trapcall_:
        case ExpressionNode::shiftby_:
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::bits_:
            rv |= fold_const(node->left);
            break;
        case ExpressionNode::hook_:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right->left);
            rv |= fold_const(node->right->right);
            break;
        case ExpressionNode::atomic_:
            rv |= fold_const(node->v.ad->flg);
            rv |= fold_const(node->v.ad->memoryOrder1);
            rv |= fold_const(node->v.ad->memoryOrder2);
            rv |= fold_const(node->v.ad->address);
            rv |= fold_const(node->v.ad->value);
            rv |= fold_const(node->v.ad->third);
            break;
        case ExpressionNode::ursh_:
        case ExpressionNode::assign_:
        case ExpressionNode::comma_:
        case ExpressionNode::check_nz_:
        case ExpressionNode::arraymul_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::div_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::mod_:
        case ExpressionNode::lor_:
        case ExpressionNode::land_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::intcall_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::stackblock_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::mp_compare_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
            rv |= fold_const(node->right);
        case ExpressionNode::blockclear_:
        case ExpressionNode::argnopush_:
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
        case ExpressionNode::select_:
        case ExpressionNode::constexprconstructor_:
            rv |= fold_const(node->left);
            break;
        case ExpressionNode::funcret_:
            rv |= fold_const(node->left);
            if (node->left->type != ExpressionNode::callsite_ && node->left->type != ExpressionNode::funcret_)
                *node = *node->left;
            break;
        case ExpressionNode::thisref_:
            rv |= fold_const(node->left);
            if (node->left->type != ExpressionNode::callsite_ && node->left->type != ExpressionNode::funcret_)
                *node = *node->left;
            break;
        case ExpressionNode::construct_: {
            node->v.construct.tp = SynthesizeType(node->v.construct.tp, nullptr, false);
            LexList* lex = SetAlternateLex(node->v.construct.deferred);
            if (node->v.construct.tp->IsArithmetic())
            {

                std::list<Initializer*>*init = nullptr, *dest = nullptr;
                lex = initType(lex, nullptr, 0, StorageClass::auto_, &init, &dest, node->v.construct.tp, nullptr, false, false, 0);
                if (init)
                    *node = *init->front()->exp;
            }
            else
            {
                EXPRESSION* exp = AnonymousVar(StorageClass::auto_, node->v.construct.tp);
                lex = initType(lex, nullptr, 0, StorageClass::auto_, &exp->v.sp->sb->init, &exp->v.sp->sb->dest,
                               node->v.construct.tp, exp->v.sp, false, false, 0);
            }
            SetAlternateLex(nullptr);
        }
        break;
        case ExpressionNode::callsite_: {
            if (node->v.func->ascall)
            {
                auto thisptr = node->v.func->thisptr;
                if (thisptr)
                {
                    int offset;
                    thisptr = relptr(thisptr, offset);
                    if (thisptr)
                    {
                        if (node->v.func->sp->sb->isDestructor && node->v.func->sp->sb->defaulted)
                            thisptr = nullptr;
                    }
                }
                if (node->v.func->sp && node->v.func->sp->sb->constexpression && !node->v.func->sp->sb->builtin_constexpression &&
                    (argumentNesting == 0 && !inStaticAssert))
                {
                    if (!rv && node->v.func->thisptr)
                    {
                        fold_const(node->v.func->thisptr);
                    }
                    inConstantExpression++;
                    rv = EvaluateConstexprFunction(node);
                    inConstantExpression--;
                }
            }
            if (!rv)
                rv |= fold_const(node->v.func->fcall);
        }
        break;
#ifdef LIBCXX17
        case ExpressionNode::cppintrinsic_:
            if (!definingTemplate || instantiatingTemplate)
            {
                EvaluateLibcxxConstant(&node);
            }
            break;
#endif
        case ExpressionNode::mp_as_bool_:
            if (node->left->type == ExpressionNode::memberptr_)
            {
                node->type = ExpressionNode::c_i_;
                node->left = nullptr;
                node->v.i = 1;
                rv = true;
            }
            break;
        case ExpressionNode::stmt_: {  // constructor thunks
            auto its = node->v.stmt->begin();
            auto itse = node->v.stmt->end();
            while (its != itse && (*its)->type == StatementNode::expr_)
                ++its;
            // thi originally modified the node statement...
            if (its != itse && (*its)->type == StatementNode::block_)
            {
                its = (*its)->lower->begin();
                while ((*its)->type == StatementNode::varstart_)
                    ++its;
                auto its1 = its;
                ++its1;
                if ((*its)->type == StatementNode::block_ && its1 == itse)
                {
                    its = (*its)->lower->begin();
                    while ((*its)->type == StatementNode::line_ || (*its)->type == StatementNode::dbgblock_)
                        ++its;
                    if ((*its)->type == StatementNode::expr_ || (*its)->type == StatementNode::return_)
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
            node1->type = ExpressionNode::c_ui_;  // change the nullptr to an int
            rv = true;
            break;
        case ExpressionNode::const_:
            if (!node1->v.sp->templateParams || allTemplateArgsSpecified(node1->v.sp, node1->v.sp->templateParams))
            {
                optimize_for_constants(&node1->v.sp->sb->init->front()->exp);
                *node1 = *node1->v.sp->sb->init->front()->exp;
                rv = true;
            }
            break;
        default:
            break;
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::uminus_:
        case ExpressionNode::argnopush_:
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
            rv |= typedconsts(node1->left);
            break;
        case ExpressionNode::hook_:
            rv |= typedconsts(node1->left);
            rv |= typedconsts(node1->right->left);
            rv |= typedconsts(node1->right->right);
            break;
        case ExpressionNode::atomic_:
            rv |= typedconsts(node1->v.ad->flg);
            rv |= typedconsts(node1->v.ad->memoryOrder1);
            rv |= typedconsts(node1->v.ad->memoryOrder2);
            rv |= typedconsts(node1->v.ad->address);
            rv |= typedconsts(node1->v.ad->value);
            rv |= typedconsts(node1->v.ad->third);
            break;
        case ExpressionNode::add_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::structadd_:
        case ExpressionNode::sub_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::mul_:
        case ExpressionNode::arraymul_:
        case ExpressionNode::umul_:
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::ursh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::div_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::mod_:
        case ExpressionNode::land_:
        case ExpressionNode::lor_:
        case ExpressionNode::and_:
        case ExpressionNode::comma_:
        case ExpressionNode::check_nz_:
        case ExpressionNode::assign_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::stackblock_:
        case ExpressionNode::intcall_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::mp_compare_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
            rv |= typedconsts(node1->right);
        case ExpressionNode::trapcall_:
        case ExpressionNode::shiftby_:
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::mp_as_bool_:
        case ExpressionNode::thisref_:
        case ExpressionNode::funcret_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
        case ExpressionNode::select_:
        case ExpressionNode::constexprconstructor_:
            rv |= typedconsts(node1->left);
            break;
        case ExpressionNode::callsite_:
            rv |= typedconsts(node1->v.func->fcall);
            break;
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_s_:
        case ExpressionNode::l_sp_:
        case ExpressionNode::l_fp_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_us_:
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_bitint_:
        case ExpressionNode::l_ubitint_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_string_:
        case ExpressionNode::l_object_:
        case ExpressionNode::bits_:
            rv |= typedconsts(node1->left);
            if (node1->left->type == ExpressionNode::global_)
            {
                if ((node1->left->v.sp->sb->storage_class == StorageClass::constant_ &&
                     isintconst(node1->left->v.sp->sb->init->front()->exp)))
                {
                    optimize_for_constants(&node1->left->v.sp->sb->init->front()->exp);
                    *node1 = *node1->left->v.sp->sb->init->front()->exp;
                    rv = true;
                }
            }
            else
            {
                rv |= typedconsts(node1->left);
            }
            break;
        case ExpressionNode::x_p_:
            rv |= typedconsts(node1->left);
            if (isconstaddress(node1->left) || isintconst(node1->left))
            {
                *node1 = *node1->left;
                rv = true;
            }
            break;
        case ExpressionNode::x_ull_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_ULONGLONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ull_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ll_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_ULONGLONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ll_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_bit_:
        case ExpressionNode::x_bool_:
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
                node1->type = ExpressionNode::c_bool_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_c_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_UCHAR, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_c_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_u16_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_U16, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_u16_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_u32_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_U32, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_u32_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_wc_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_WCHAR, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_wc_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_uc_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_UCHAR, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_uc_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_s_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_USHORT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_i_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_us_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_USHORT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ui_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_bitint_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                auto value = rebitint(node1, node1->left);
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_bitint_;
                node1->v.b.value = value;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ubitint_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                auto value = rebitint(node1, node1->left);
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ubitint_;
                node1->v.b.value = value;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_i_:
        case ExpressionNode::x_inative_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_UINT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_i_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ui_:
        case ExpressionNode::x_unative_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_UINT, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ul_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_l_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(-ISZ_ULONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_i_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ul_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = true;
                node1->v.i = Optimizer::CastToInt(ISZ_ULONG, reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = ExpressionNode::c_ul_;
                node1->left = nullptr;
            }
            break;
            /*#ifdef XXXXX */
        case ExpressionNode::x_f_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_FLOAT, &temp);
                node1->type = ExpressionNode::c_f_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_fi_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_IFLOAT, &temp);
                node1->type = ExpressionNode::c_fi_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_d_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_DOUBLE, &temp);
                node1->type = ExpressionNode::c_d_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_di_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_IDOUBLE, &temp);
                node1->type = ExpressionNode::c_di_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ld_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_LDOUBLE, &temp);
                node1->type = ExpressionNode::c_ld_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ldi_:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = true;
                node1->v.f = Allocate<FPF>();
                *node1->v.f = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                node1->type = ExpressionNode::c_ldi_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_fc_:
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
                    node1->left->type = ExpressionNode::c_ld_;
                    temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->i = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->r.SetZero(0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = ExpressionNode::c_fc_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_dc_:
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
                    node1->left->type = ExpressionNode::c_ld_;
                    temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->i = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->r.SetZero(0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = ExpressionNode::c_dc_;
                node1->left = nullptr;
            }
            break;
        case ExpressionNode::x_ldc_:
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
                    node1->left->type = ExpressionNode::c_ld_;
                    temp = refloat(node1->left);
                    node1->v.c = Allocate<_COMPLEX_S>();
                    node1->v.c->i = Optimizer::CastToFloat(ISZ_ILDOUBLE, &temp);
                    node1->v.c->r.SetZero(0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = ExpressionNode::c_ldc_;
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
        case ExpressionNode::add_:
            // fixme : without the (architecture == ARCHITECTURE_MSIL) some complex expressions inside loops
            // get optimized wrong see x264::analyse.c around line 849
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
                if (!isarithmeticconst(exp2->left) || !isarithmeticconst(exp2->right))
                    return false;
            // fallthrough
        case ExpressionNode::mul_:
        case ExpressionNode::umul_:
        case ExpressionNode::arraymul_:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
            return exp2->left->left || exp2->left->right || exp2->right->left || exp2->right->right;
        default:
            return false;
    }
}
void rebalance(EXPRESSION** exp)
{
    if ((*exp)->type == ExpressionNode::comma_)
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
        while (IsCastValue(exp))
            exp = exp->left;
        if (exp->type == ExpressionNode::sizeof_)
            return true;
        if (exp->type == ExpressionNode::callsite_)
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
            case ExpressionNode::add_:
            case ExpressionNode::mul_:
            case ExpressionNode::umul_:
            case ExpressionNode::div_:
            case ExpressionNode::udiv_:
            case ExpressionNode::global_:
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
    if (constant->type == ExpressionNode::sizeof_)
    {
        Type* tp = constant->left->v.tp;
        constant->type = ExpressionNode::c_ui_;
        constant->left = nullptr;
        constant->v.i = tp->size;
    }
}

void optimize_for_constants(EXPRESSION** expr)
{
    if ((*expr)->type == ExpressionNode::select_)
        expr = &(*expr)->left;
    int rv = true, count = 8;
    EXPRESSION* oldasidehead = asidehead;
    EXPRESSION** oldasidetail = asidetail;

    asidehead = 0;
    asidetail = &asidehead;
    while (rv && count--)
    {
        // this used to be one expression but MSVC reordered the elements in it in some configurtions, leading to subtle bugs...
        rv = typedconsts(*expr);
        rv |= fold_const(*expr);
        rv |= opt0(expr);
    }
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
LexList* optimized_expression(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** expr, bool commaallowed)
{
    if (commaallowed)
        lex = expression(lex, funcsp, atp, tp, expr, 0);
    else
        lex = expression_no_comma(lex, funcsp, atp, tp, expr, nullptr, 0);
    if (*tp)
    {
        optimize_for_constants(expr);
    }
    return lex;
}
}  // namespace Parser