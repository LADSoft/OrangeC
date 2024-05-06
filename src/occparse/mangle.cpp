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

#include "compiler.h"
#include <stack>
#include "ccerr.h"
#include "config.h"
#include "unmangle.h"
#include "OptUtils.h"
#include "help.h"
#include "memory.h"
#include "mangle.h"
#include "declare.h"
#include "types.h"
#include <algorithm>
namespace Parser
{

const char* overloadNameTab[] = {".bctr",  ".bdtr",   ".bcast",  ".bnew",   ".bdel",   ".badd",   ".bsub",   ".bmul",    ".bdiv",
                                 ".bshl",  ".bshr",   ".bmod",   ".bequ",   ".bneq",   ".blt",    ".bleq",   ".bgt",     ".bgeq",
                                 ".basn",  ".basadd", ".bassub", ".basmul", ".basdiv", ".basmod", ".basshl", ".bsasshr", ".basand",
                                 ".basor", ".basxor", ".binc",   ".bdec",   ".barray", ".bcall",  ".bstar",  ".barrow",  ".bcomma",
                                 ".blor",  ".bland",  ".bnot",   ".bor",    ".band",   ".bxor",   ".bcpl",   ".bnwa",    ".bdla",
                                 ".blit",  ".badd",   ".bsub",   ".bmul",   ".band"};
const char* msiloverloadNameTab[] = {".ctor",
                                     ".dtor",
                                     ".bcast",
                                     ".new",
                                     ".delete",
                                     "op_Addition",
                                     "op_Subtraction",
                                     "op_Multiply",
                                     "op_Division",
                                     "op_LeftShift",
                                     "op_RightShift",
                                     "op_Modulus",
                                     "op_Equality",
                                     "op_Inequality",
                                     "op_LessThan",
                                     "op_LessThanOrEqual",
                                     "op_GreaterThan",
                                     "op_GreaterThanOrEqual",
                                     ".basn",
                                     "op_AdditionAssignment",
                                     "op_SubtractionAssignment",
                                     "op_MultiplicationAssignment",
                                     "op_DivisionAssignment",
                                     "op_ModulusAssignment",
                                     "op_LeftShiftAssignment",
                                     "op_RightShiftAssignment",
                                     "op_BitwiseAndAssignment",
                                     "op_BitwiseOrAssignment",
                                     "op_ExclusiveOrAssignment",
                                     ".binc",
                                     ".bdec",
                                     ".barray",
                                     ".bcall",
                                     ".bstar",
                                     "op_MemberSelection",
                                     "op_Comma",
                                     "op_LogicalOr",
                                     "op_LogicalAnd",
                                     "op_LogicalNot",
                                     "op_BitwiseOr",
                                     "op_BitwiseAnd",
                                     "op_ExclusiveOr",
                                     "op_OnesComplement",
                                     ".bnwa",
                                     ".bdla",
                                     ".blit",
                                     "op_UnaryPlus",
                                     "op_UnaryMinus",
                                     "op_PointerDereference",
                                     "op_AddressOf"};
const char* overloadXlateTab[] = {
    0,    0,   0,    "new", "delete", "+",  "-",  "*",   "/",   "<<",    ">>",       "%",    "==", "!=", "<",  "<=", ">",
    ">=", "=", "+=", "-=",  "*=",     "/=", "%=", "<<=", ">>=", "&=",    "|=",       "^=",   "++", "--", "[]", "()", "->*",
    "->", ",", "||", "&&",  "!",      "|",  "&",  "^",   "~",   "new[]", "delete[]", "\"\"", "+",  "-",  "*",  "&"};
#define IT_THRESHOLD 2
#define IT_OV_THRESHOLD 2
#define IT_SIZE (sizeof(cpp_funcname_tab) / sizeof(char*))

#define MAX_MANGLE_NAME_COUNT 36

static char mangledNames[MAX_MANGLE_NAME_COUNT][256];
int mangledNamesCount;

static char* lookupName(char* in, const char* name);
static int uniqueID;
void mangleInit()
{
    uniqueID = 0;
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        memcpy(overloadNameTab, msiloverloadNameTab, sizeof(msiloverloadNameTab));
        memcpy(cpp_funcname_tab, msiloverloadNameTab, sizeof(msiloverloadNameTab));
    }
}
char* mangleNameSpaces(char* in, SYMBOL* sym)
{
    if (!sym)
        return in;
    in = mangleNameSpaces(in, sym->sb->parentNameSpace);
    if (sym->sb->parentNameSpace && !strcmp(sym->name, "__1") && !strcmp(sym->sb->parentNameSpace->name, "std"))
        return in;
    Optimizer::my_sprintf(in, "@%s", sym->name);
    return in + strlen(in);
}
static char* mangleTemplate(char* buf, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
static char* getName(char* in, SYMBOL* sym);
static char* mangleClasses(char* in, SYMBOL* sym)
{
    if (!sym)
        return in;
    if (sym->sb->parentClass)
        in = mangleClasses(in, sym->sb->parentClass);
    if (sym->sb->castoperator)
    {
        strcat(in, "@");
    }
    else if (sym->sb->templateLevel && sym->templateParams)
    {
        *in++ = '@';
        mangleTemplate(in, sym, sym->templateParams);
    }
    else
        Optimizer::my_sprintf(in, "@%s", sym->name);
    return in + strlen(in);
}
static char* mangleExpressionInternal(char* buf, EXPRESSION* exp)
{
    while (castvalue(exp))
        exp = exp->left;
    if (isintconst(exp))
    {
        if (exp->type == ExpressionNode::const_)
        {
            Optimizer::my_sprintf(buf, "%lld?", exp->v.sp->sb->value.i);
        }
        else
        {
            Optimizer::my_sprintf(buf, "%lld?", exp->v.i);
        }
        if (buf[0] == '-')
            buf[0] = '_';
    }
    else
    {
        bool nonpointer = false;
        while (lvalue(exp))
        {
            nonpointer = true;
            exp = exp->left;
        }
        switch (exp->type)
        {
            case ExpressionNode::nullptr_:
                *buf++ = 'n';
                *buf = 0;
                break;
            case ExpressionNode::arrayadd_:
            case ExpressionNode::structadd_:
            case ExpressionNode::add_:
                *buf++ = 'p';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::sub_:
                *buf++ = 's';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::mul_:
            case ExpressionNode::umul_:
            case ExpressionNode::arraymul_:
                *buf++ = 'm';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::umod_:
            case ExpressionNode::mod_:
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::dot_:
                *buf++ = 'D';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::pointsto_:
                *buf++ = 'P';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::div_:
            case ExpressionNode::udiv_:
            case ExpressionNode::arraydiv_:
                *buf++ = 'd';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::lsh_:
            case ExpressionNode::arraylsh_:
                *buf++ = 'h';
                *buf++ = 'l';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::rsh_:
            case ExpressionNode::ursh_:
                *buf++ = 'h';
                *buf++ = 'r';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::hook_:
                *buf++ = 'C';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right->left);
                buf = mangleExpressionInternal(buf, exp->right->right);
                *buf = 0;
                break;
            case ExpressionNode::assign_:
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::eq_:
                *buf++ = 'c';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::ne_:
                *buf++ = 'c';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::uminus_:
                *buf++ = 'u';
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case ExpressionNode::not_:
                *buf++ = 'l';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case ExpressionNode::compl_:
                *buf++ = 'b';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                *buf = 0;
                break;
            case ExpressionNode::ult_:
            case ExpressionNode::lt_:
                *buf++ = 'c';
                *buf++ = 'l';
                *buf++ = 't';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::ule_:
            case ExpressionNode::le_:
                *buf++ = 'c';
                *buf++ = 'l';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::ugt_:
            case ExpressionNode::gt_:
                *buf++ = 'c';
                *buf++ = 'g';
                *buf++ = 't';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::uge_:
            case ExpressionNode::ge_:
                *buf++ = 'c';
                *buf++ = 'g';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::and_:
                *buf++ = 'b';
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::land_:
                *buf++ = 'l';
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::or_:
                *buf++ = 'b';
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::lor_:
                *buf++ = 'l';
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::xor_:
                *buf++ = 'b';
                *buf++ = 'x';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::auto_inc_:
                *buf++ = 'i';
                *buf++ = 'p';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::auto_dec_:
                *buf++ = 'i';
                *buf++ = 's';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::select_:
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case ExpressionNode::templateselector_: {
                auto tsl = exp->v.templateSelector;
                SYMBOL* ts = (*tsl)[1].sp;
                *buf++ = 't';
                *buf++ = 's';
                if ((*tsl)[1].isTemplate && (*tsl)[1].templateParams)  // may be an empty variadic
                {
                    buf = mangleTemplate(buf, ts, (*tsl)[1].templateParams);
                }
                auto find = (*tsl).begin();
                ++find;
                ++find;
                for (; find != (*tsl).end(); ++find)
                {
                    *buf++ = 't';
                    buf = lookupName(buf, find->name);
                    buf += strlen(buf);
                }
                *buf = 0;
                break;
            }
            case ExpressionNode::templateparam_:
            case ExpressionNode::auto_:
                *buf++ = 't';
                *buf++ = 'p';
                buf = lookupName(buf, exp->v.sp->name);
                buf += strlen(buf);
                *buf = 0;
                break;
            case ExpressionNode::thisref_:
            case ExpressionNode::funcret_:
            case ExpressionNode::constexprconstructor_:
                buf = mangleExpressionInternal(buf, exp->left);
                *buf = 0;
                break;
            case ExpressionNode::func_: {
                if (exp->v.func->ascall)
                {
                    *buf++ = 'f';
                    buf = getName(buf, exp->v.func->sp);
                    if (exp->v.func->arguments)
                    {
                        for (auto arg : *exp->v.func->arguments)
                            if (arg->exp)
                            {
                                *buf++ = 'F';
                                buf = mangleExpressionInternal(buf, arg->exp);
                            }
                    }
                }
                else
                {
                    *buf++ = 'e';
                    *buf++ = '?';
                    buf = getName(buf, exp->v.func->sp);
                    buf += strlen(buf);
                    *buf++ = '.';
                    buf = mangleType(buf, exp->v.func->sp->tp, true);
                }
                break;
            }
            case ExpressionNode::pc_:
            case ExpressionNode::global_:
            case ExpressionNode::const_:
                if (exp->v.sp->tp->IsFunction())
                {
                    *buf++ = 'e';
                    *buf++ = '?';
                    strcpy(buf, exp->v.sp->name);
                    buf += strlen(buf);
                    *buf++ = '.';
                    buf = mangleType(buf, exp->v.sp->tp, true);
                }
                else
                {
                    *buf++ = 'g';
                    if (!nonpointer)
                        *buf++ = '?';
                    strcpy(buf, exp->v.sp->name);
                    *buf++ = '.';
                    *buf = 0;
                }
                break;
            case ExpressionNode::sizeofellipse_:
                *buf++ = 'z';
                buf = getName(buf, exp->v.templateParam->first);
                buf += strlen(buf);
                break;
            case ExpressionNode::comma_:
                *buf++ = 'v';
                // ignoring args to void...
                *buf = 0;
                break;
            default:
                *buf = 0;
                break;
        }
    }
    buf += strlen(buf);
    return buf;
}
static char* mangleExpression(char* buf, EXPRESSION* exp)
{
    if (exp)
    {
        *buf++ = '?';
        buf = mangleExpressionInternal(buf, exp);
    }
    return buf;
}
static char* mangleTemplate(char* buf, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params)
{
    bool bySpecial = false;
    if (params &&  params->size() && params->front().second->type == TplType::new_ &&
        ((sym->sb->instantiated && !sym->sb->templateLevel) || (params && params->front().second->bySpecialization.types)))
    {
        params = params->front().second->bySpecialization.types;
        bySpecial = true;
    }
    if (sym->tp->type == BasicType::templateparam_ && sym->tp->templateParam->second->type == TplType::template_)
    {
        auto sp = sym->tp->templateParam->second->byTemplate.val;
        if (sp)
            sym = sp;
    }
    if (sym->sb && (sym->sb->isConstructor || sym->sb->isDestructor) &&
        sym->sb->templateLevel == sym->sb->parentClass->sb->templateLevel)
    {
        strcpy(buf, sym->name);
        while (*buf)
            buf++;
        *buf++ = '.';
        *buf = 0;
    }
    else
    {
        *buf++ = '#';
        strcpy(buf, sym->name);
        strcat(buf, ".");
        buf += strlen(buf);
    }
    static StackList<std::list<TEMPLATEPARAMPAIR>::iterator> nestedStack;
    NestedStack<std::list<TEMPLATEPARAMPAIR>::iterator> tps(nestedStack);

    if (params)
    {
        auto it = params->begin();
        auto ite = params->end();
        for ( ;it != ite; )
        {
            switch (it->second->type)
            {
            case TplType::typename_:
                if (it->second->packed)
                {
                    if (it->second->byPack.pack)
                    {
                        for (auto pack : *it->second->byPack.pack)
                            buf = mangleType(buf, pack.second->byClass.val, true);
                    }
                    else
                    {
                        *buf++ = 'e';
                        buf = getName(buf, it->first);
                    }
                }
                else if (bySpecial)
                {
                    if (it->second->byClass.val)
                    {
                        buf = mangleType(buf, it->second->byClass.val, true);
                    }
                    else if (it->second->byClass.dflt)
                    {
                        buf = mangleType(buf, it->second->byClass.dflt, true);
                    }
                    else
                    {
                        buf = getName(buf, it->first);
                    }
                }
                else if (sym->sb && sym->sb->instantiated && it->second->byClass.val)
                {
                    buf = mangleType(buf, it->second->byClass.val, true);
                }
                else
                {
                    if (it->second->byClass.dflt)
                    {
                        buf = mangleType(buf, it->second->byClass.dflt, true);
                    }
                    else
                    {
                        buf = getName(buf, it->first);
                    }
                }
                break;
            case TplType::template_:
                if (it->second->packed)
                    *buf++ = 'e';
                /*
                if (bySpecial && it->second->byTemplate.dflt && it->second->byTemplate.val)
                {
                    buf = mangleTemplate(buf, it->second->byTemplate.dflt, it->second->byTemplate.val->templateit);
                }
                else
                */
                if (sym->sb->instantiated && it->second->byTemplate.val)
                {
                    buf = mangleTemplate(buf, it->second->byTemplate.val, it->second->byTemplate.val->templateParams);
                }
                else if (it->first)
                {
                    buf = getName(buf, it->first);
                }
                else
                {
                    buf = getName(buf, it->second->byTemplate.dflt);
                }
                break;
            case TplType::int_:
                if (it->second->packed)
                {
                    *buf++ = 'e';
                    if (it->second->byPack.pack)
                    {
                        tps.push(it);
                        tps.push(ite);
                        ite = it->second->byPack.pack->end();
                        it = it->second->byPack.pack->begin();
                        continue;
                    }
                }
                else
                {
                    buf = mangleType(buf, it->second->byNonType.tp, true);
                    if (bySpecial || sym->sb->instantiated)
                    {
                        EXPRESSION* exp = bySpecial && it->second->byNonType.dflt ? it->second->byNonType.dflt : it->second->byNonType.val;
                        buf = mangleExpression(buf, exp);
                    }
                    else if (it->second->byNonType.dflt)
                    {
                        buf = mangleExpression(buf, it->second->byNonType.dflt);
                    }
                }
                break;
            default:
                break;
            }
            ++it;
            if (it == ite && !tps.empty())
            {
                ite = tps.top();
                tps.pop();
                it = tps.top();
                tps.pop();
                ++it;
            }
        }
    }
    *buf++ = '~';
    *buf = 0;
    return buf;
}
static char* lookupName(char* in, const char* name)
{
    int i;
    for (i = 0; i < mangledNamesCount; i++)
        if (!strcmp(name, mangledNames[i]))
            break;
    if (i < mangledNamesCount)
    {
        Optimizer::my_sprintf(in, "n%c", i < 10 ? i + '0' : i - 10 + 'A');
    }
    else
    {
        if (mangledNamesCount < MAX_MANGLE_NAME_COUNT)
            strcpy(mangledNames[mangledNamesCount++], name);
        Optimizer::my_sprintf(in, "%d%s", strlen(name), name);
    }
    return in;
}
static char* getName(char* in, SYMBOL* sym)
{
    if (!sym)
    {
        strcpy(in, "????");
    }
    else if (!sym->sb)
    {
        in = lookupName(in, sym->name);
    }
    else
    {
        int i;
        char buf[4096], *p;
        p = mangleNameSpaces(buf, sym->sb->parentNameSpace);
        p = mangleClasses(p, sym->sb->parentClass);
        if (p != buf)
            *p++ = '@';
        if (sym->sb->templateLevel && sym->templateParams)
        {
            p = mangleTemplate(p, sym, sym->templateParams);
        }
        else
        {
            strcpy(p, sym->name);
        }
        in = lookupName(in, buf);
    }
    while (*in)
        in++;
    return in;
}
char* mangleType(char* in, Type* tp, bool first)
{
    char nm[4096];
    int i;
    if (!tp)
    {
        Optimizer::my_sprintf(in, "%d%s", strlen("initializer-list"), "initializer-list");
        while (*in)
            in++;
        return in;
    }
    while (tp)
    {
        while (tp->type == BasicType::typedef_)
            tp = tp->btp;
        if (tp->IsStructured() && tp->BaseType()->sp->sb && tp->BaseType()->sp->sb->templateLevel)
        {
            {
                if (tp->IsConst())
                    *in++ = 'x';
                if (tp->IsVolatile())
                    *in++ = 'y';
                if (tp->IsLRefQual())
                    *in++ = 'r';
                if (tp->IsRRefQual())
                    *in++ = 'R';
            }
            in = mangleTemplate(in, tp->BaseType()->sp, tp->BaseType()->sp->templateParams);
            return in;
        }
        else
        {
            if (tp->IsConst())
                *in++ = 'x';
            if (tp->IsVolatile())
                *in++ = 'y';
            if (tp->IsLRefQual())
                *in++ = 'r';
            if (tp->IsRRefQual())
                *in++ = 'R';
            tp = tp->BaseType();
            if (tp->IsInt() && tp->btp && tp->btp->type == BasicType::enum_)
                tp = tp->btp;
            switch (tp->type)
            {
                case BasicType::func_:
                case BasicType::ifunc_:
                    if (tp->BaseType()->sp && ismember(tp->BaseType()->sp) && !first)
                    {
                        *in++ = 'M';
                        in = getName(in, tp->sp->sb->parentClass);
                        while (*in)
                            in++;
                    }
                    *in++ = 'q';
                    for (auto sym : *tp->syms)
                    {
                        if (!sym->sb->thisPtr)
                            in = mangleType(in, sym->tp, true);
                    }
                    *in++ = '.';
                    // return value comes next
                    break;
                case BasicType::memberptr_:
                    *in++ = 'M';
                    in = getName(in, tp->sp);
                    if (tp->btp->IsFunction())
                    {
                        *in++ = 'q';
                        for (auto sym : *tp->btp->BaseType()->syms)
                        {
                            if (!sym->sb->thisPtr)
                                in = mangleType(in, sym->tp, true);
                        }
                        *in++ = '.';
                        tp = tp->btp;  // so we can get to tp->btp->btp
                    }
                    else
                    {
                        *in++ = '.';
                    }
                    break;
                case BasicType::enum_:
                case BasicType::struct_:
                case BasicType::union_:
                case BasicType::class_:
                    in = getName(in, tp->sp);
                    return in;
                case BasicType::bool_:
                    in = lookupName(in, "bool");
                    while (*in)
                        in++;
                    break;
                case BasicType::string_:
                    in = lookupName(in, "__string");
                    while (*in)
                        in++;
                    break;
                case BasicType::object_:
                    in = lookupName(in, "__object");
                    while (*in)
                        in++;
                    break;
                case BasicType::unsigned_short_:
                    *in++ = 'u';
                case BasicType::short_:
                    *in++ = 's';
                    break;
                case BasicType::unsigned_:
                    *in++ = 'u';
                case BasicType::int_:
                    *in++ = 'i';
                    break;
                case BasicType::unsigned_bitint_:
                    *in++ = 'u';
                case BasicType::bitint_:
                    *in++ = 'B';
                    Optimizer::my_sprintf(in, "%d?", tp->bitintbits);
                    in += strlen(in);
                    break;
                case BasicType::unative_:
                    *in++ = 'u';
                case BasicType::inative_:
                    *in++ = 'N';
                    break;
                case BasicType::char16_t_:
                    *in++ = 'h';
                    break;
                case BasicType::char32_t_:
                    *in++ = 'H';
                    break;
                case BasicType::unsigned_long_:
                    *in++ = 'u';
                case BasicType::long_:
                    *in++ = 'l';
                    break;
                case BasicType::unsigned_long_long_:
                    *in++ = 'u';
                case BasicType::long_long_:
                    *in++ = 'L';
                    break;
                case BasicType::char8_t_:
                case BasicType::unsigned_char_:
                    *in++ = 'u';
                case BasicType::char_:
                    *in++ = 'c';
                    break;
                case BasicType::signed_char_:
                    *in++ = 'S';
                    *in++ = 'c';
                    break;
                case BasicType::wchar_t_:
                    *in++ = 'C';
                    break;
                case BasicType::float__complex_:
                    *in++ = 'F';
                    break;
                case BasicType::double__complex_:
                    *in++ = 'D';
                    break;
                case BasicType::long_double_complex_:
                    *in++ = 'G';
                    break;
                case BasicType::float_:
                    *in++ = 'f';
                    break;
                case BasicType::double_:
                    *in++ = 'd';
                    break;
                case BasicType::long_double_:
                    *in++ = 'g';
                    break;
                case BasicType::pointer_:
                    if (tp->nullptrType)
                    {
                        in = lookupName(in, "nullptr_t");
                        while (*in)
                            in++;
                        return in;
                    }
                    else
                    {
                        if (!tp->array)
                        {
                            *in++ = 'p';
                        }
                        else
                        {
                            *in++ = 'A';
                        }
                    }
                    break;
                case BasicType::far_:
                    *in++ = 'P';
                    break;
                case BasicType::lref_:
                    *in++ = 'r';
                    break;
                case BasicType::rref_:
                    *in++ = 'R';
                    break;
                case BasicType::ellipse_:
                    *in++ = 'e';
                    break;
                case BasicType::void_:
                case BasicType::any_:
                    *in++ = 'v';
                    break;
                case BasicType::templateparam_:
                    if (tp->templateParam->second->type == TplType::typename_ && tp->templateParam->second->byClass.val &&
                        tp->templateParam->second->byClass.val->BaseType()->type != BasicType::templateparam_)
                        in = mangleType(in, tp->templateParam->second->byClass.val, false);
                    else
                        in = getName(in, tp->templateParam->first);
                    break;
                case BasicType::templateselector_: {
                    auto s = (*tp->sp->sb->templateSelector).begin();
                    auto se = (*tp->sp->sb->templateSelector).end();
                    char* p;
                    ++s;
                    if (s->isTemplate)
                        p = mangleTemplate(nm, s->sp, s->sp->sb->instantiated ? s->sp->templateParams : s->templateParams);
                    else
                        p = getName(nm, s->sp);
                    p[0] = 0;
                    if (strlen(nm) > sizeof(nm))
                        p = mangleTemplate(nm, s->sp, s->templateParams);
                    for (++s; s != se; ++s)
                    {
                        strcat(nm, "@");
                        strcat(nm, s->name);
                    }
                    p = nm;
                    while (isdigit(*p))
                        p++;
                    Optimizer::my_sprintf(in, "%d%s", strlen(p), p);
                    while (*in)
                        in++;
                    return in;
                }
                break;
                case BasicType::templatedecltype_:
                    *in++ = 'E';
                    in = mangleExpression(in, tp->templateDeclType);
                    break;
                case BasicType::aggregate_:
                    in = getName(in, tp->sp);
                    break;
                case BasicType::auto_:
                    *in++ = 'a';
                    break;
                default:
                    diag("mangleType: unknown type");
                    break;
            }
        }
        tp = tp->btp;
    }
    *in = 0;
    return in;
}
static bool validType(Type* tp, bool byVal)
{
    tp = tp->BaseType();
    switch (tp->type)
    {
    case BasicType::templateselector_:
        return byVal;
    case BasicType::templateparam_:
    case BasicType::any_:
    case BasicType::aggregate_:
        return false;
    case BasicType::pointer_:
        if (tp->array)
        {
            if (tp->size == 0)
                return false;

        }
        if (tp->vla)
            return false;
    case BasicType::lref_:
    case BasicType::rref_:
    case BasicType::memberptr_:
        return validType(tp->btp, byVal);
    case BasicType::func_:
    case BasicType::ifunc_:
        if (!validType(tp->btp, byVal))
            return false;
        for (auto sp : *tp->syms)
            if (!validType(sp->tp, byVal))
                return false;
        break;
    case BasicType::struct_:
    case BasicType::class_:
    case BasicType::union_:
        if (tp->sp->templateParams)
        {
            for (auto tpl : *tp->sp->templateParams)
            {
                if (tpl.second->type == TplType::typename_)
                {
                    if (tpl.second->packed)
                    {
                        if (tpl.second->byPack.pack)
                        for (auto tpl2 : *tpl.second->byPack.pack)
                        {
                            auto dflt = tpl2.second->byClass.dflt;
                            if (!dflt)
                                dflt = tpl2.second->byClass.val;
                            if (!dflt || !validType(dflt, byVal))
                                return false;
                        }
                    }
                    else
                    {
                        auto dflt = tpl.second->byClass.dflt;
                        if (!dflt)
                            dflt = tpl.second->byClass.val;
                        if (!dflt || !validType(dflt, byVal))
                            return false;
                    }
                }
            }
        }
        break;
    default:
    {
	break;
    }
    }
    return true;
}
bool GetTemplateArgumentName(std::list<TEMPLATEPARAMPAIR>* params, std::string& result, bool byVal)
{

    mangledNamesCount = 0;
    if (!params || !params->size() || (params->size() == 1 && params->front().second->type == TplType::new_))
        result = "v";
    else
        result = "";
    if (params)
    {
        for (auto&& param : *params)
        {
            if (param.second->type != TplType::new_)
            {
                char buf[8000];
                void* dflt;
                if (param.second->packed)
                {
                    if (!param.second->byPack.pack || !param.second->byPack.pack->size())
                    {
                        return false;
                    }
                    for (auto tpl : *param.second->byPack.pack)
                    {
                        buf[0] = 0;
                        if (byVal)
                        {
                            dflt = tpl.second->byClass.val;
                            if (!dflt)
                                dflt = tpl.second->byClass.dflt;
                        }
                        else
                        {
                            dflt = tpl.second->byClass.dflt;
                            if (!dflt)
                                dflt = tpl.second->byClass.val;
                        }
                        if (!dflt)
                            return false;
                        switch (param.second->type)
                        {
                            case TplType::typename_:
                                if (!validType((Type*)dflt, byVal))
                                    return false;
                                result += 'c';
                                *(mangleType(buf, (Type*)dflt, true)) = 0;
                                break;
                            case TplType::int_:
                                result += 'i';
                                *mangleExpression(buf, (EXPRESSION*)dflt) = 0;
                                break;
                            case TplType::template_:
                                result += 't';
                                *mangleTemplate(buf, (SYMBOL*)dflt, tpl.second->byTemplate.args) = 0;
                                break;
                        }

                        result += buf;
                    }
                }
                else
                {
                    if (byVal)
                    {
                        dflt = param.second->byClass.val;
                        if (!dflt)
                            dflt = param.second->byClass.dflt;
                    }
                    else
                    {
                        dflt = param.second->byClass.dflt;
                        if (!dflt)
                            dflt = param.second->byClass.val;
                    }
                    if (!dflt)
                        return false;
                    buf[0] = 0;
                    switch (param.second->type)
                    {
                        case TplType::typename_:
                            if (!validType((Type*)dflt, byVal))
                                return false;
                            result += 'c';
                            *(mangleType(buf, (Type*)dflt, true)) = 0;
                            break;
                        case TplType::int_:
                            if (((EXPRESSION*)dflt)->type == ExpressionNode::templateparam_)
                                return false;
                            result += 'i';
                            *mangleExpression(buf, (EXPRESSION*)dflt) = 0;
                            break;
                        case TplType::template_:
                            result += 't';
                            *mangleTemplate(buf, (SYMBOL*)dflt, param.second->byTemplate.args) = 0;
                            break;
                    }
                    result += buf;
                }
            }
        }
    }
    return true;
}

void GetClassKey(char* buf, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params)
{
    mangledNamesCount = 0;
    SYMBOL* lastParent = sym;
    while (lastParent->sb->parentClass)
        lastParent = lastParent->sb->parentClass;
    char* p = buf;
    p = mangleNameSpaces(p, lastParent->sb->parentNameSpace);
    p = mangleClasses(p, sym->sb->parentClass);
    *p++ = '@';
    p = mangleTemplate(p, sym, params);
    *p = 0;
}
void SetLinkerNames(SYMBOL* sym, Linkage linkage, bool isTemplateDefinition)
{
    char errbuf[8192], *p = errbuf;
    memset(errbuf, 0, 8192);
    SYMBOL* lastParent;
    mangledNamesCount = 0;
    if (Optimizer::cparams.prm_cplusplus && !sym->sb->parentClass && !sym->sb->parentNameSpace && sym->name[0] == 'm' &&
        !strcmp(sym->name, "main"))
        linkage = Linkage::c_;
    if (linkage == Linkage::none_ || linkage == Linkage::cdecl_)
    {
        if (Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL))
        {
            if (sym->sb->storage_class != StorageClass::label_ && sym->sb->storage_class != StorageClass::parameter_ &&
                sym->sb->storage_class != StorageClass::namespace_ && sym->sb->storage_class != StorageClass::namespace_alias_ &&
                sym->sb->storage_class != StorageClass::ulabel_ &&
                (sym->tp->IsFunction() || istype(sym) || sym->sb->parentNameSpace || sym->sb->parentClass || sym->sb->templateLevel))
                linkage = Linkage::cpp_;
            else
                linkage = Linkage::c_;
        }
        else
        {
            linkage = Linkage::c_;
        }
    }
    else if (linkage == Linkage::stdcall_)
    {
        if (sym->sb->parentClass)
            linkage = Linkage::cpp_;
    }
    if (linkage == Linkage::auto_ && !Optimizer::cparams.prm_cplusplus)
        linkage = Linkage::c_;
    if (linkage == Linkage::c_ && !Optimizer::cparams.prm_cmangle)
        linkage = Linkage::stdcall_;
    if (sym->sb->attribs.inheritable.linkage4 == Linkage::virtual_ || linkage == Linkage::fastcall_)
    {
        if (Optimizer::cparams.prm_cplusplus)
            linkage = Linkage::cpp_;
        else
            linkage = Linkage::c_;
    }
    switch (linkage)
    {
        case Linkage::auto_:
            if (sym->sb->parent)
                if (sym->sb->uniqueID == 0)
                    sym->sb->uniqueID = uniqueID++;
            p = mangleClasses(p, theCurrentFunc);
            Optimizer::my_sprintf(p, "@%s", sym->name);

            break;
        case Linkage::pascal_:
            if (sym->name[0] == '.')
            {
                errbuf[0] = '_';
                strcpy(errbuf + 1, sym->name);
            }
            else
            {
                strcpy(errbuf, sym->name);
            }
            std::transform(errbuf, errbuf + strlen(errbuf), errbuf, ::toupper);
            break;
        case Linkage::stdcall_:
            if (sym->name[0] == '.')
            {
                errbuf[0] = '_';
                strcpy(errbuf + 1, sym->name);
            }
            else
            {
                strcpy(errbuf, sym->name);
            }
            break;
        case Linkage::c_:
        default:
            if (sym->sb->parent)
                if (sym->sb->uniqueID == 0)
                    sym->sb->uniqueID = uniqueID++;
            if (sym->sb->storage_class == StorageClass::localstatic_ && sym->sb->parent)
            {
                strcpy(errbuf, sym->sb->parent->sb->decoratedName);
                strcat(errbuf, "_");
                strcat(errbuf, sym->name);
                sprintf(errbuf + strlen(errbuf), "_%d", sym->sb->uniqueID);
            }
            else
            {
                errbuf[0] = '_';
                strcpy(errbuf + 1, sym->name);
            }
            break;
        case Linkage::cpp_:
            if (isTemplateDefinition)
                *p++ = '@';
            lastParent = sym;
            while (lastParent->sb->parentClass)
                lastParent = lastParent->sb->parentClass;
            p = mangleNameSpaces(p, lastParent->sb->parentNameSpace);
            p = mangleClasses(p, sym->sb->parentClass);
            *p++ = '@';
            if (sym->sb->templateLevel && sym->templateParams)
            {
                p = mangleTemplate(p, sym, sym->templateParams);
            }
            else
            {
                strcpy(p, sym->name);
                p += strlen(p);
            }
            if (sym->tp->IsFunction())
            {
                *p++ = '.';
                if (sym->sb->castoperator)
                {
                    int tmplCount = 0;
                    *p++ = 'o';
                    p = mangleType(p, sym->tp->BaseType()->btp, true);  // cast operators get their cast type in the name
                    *p++ = '.';
                    p = mangleType(p, sym->tp, true);  // add the $qv
                    while (p > errbuf && (*--p != '.' || tmplCount))
                        if (*p == '~')
                            tmplCount++;
                        else if (*p == '#')
                            tmplCount--;
                    p[1] = 0;
                }
                else
                {
                    p = mangleType(p, sym->tp, true);  // otherwise functions get their parameter list in the name
                                                       //                    if (!sym->sb->templateLevel)
                    {
                        int tmplCount = 0;
                        while (p > errbuf && (*--p != '.' || tmplCount))
                            if (*p == '~')
                                tmplCount++;
                            else if (*p == '#')
                                tmplCount--;
                        if (sym->tp->BaseType()->btp->type == BasicType::memberptr_)
                        {
                            while (p > errbuf && (*--p != '.' || tmplCount))
                                if (*p == '~')
                                    tmplCount++;
                                else if (*p == '#')
                                    tmplCount--;
                        }
                        p[1] = 0;
                    }
                }
            }
            *p = 0;
            break;
    }
    sym->sb->decoratedName = litlate(errbuf);
}
}  // namespace Parser