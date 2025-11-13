/* Software License Agreement
 *
 *     Copyright(C) 1994`-2025 David Lindauer, (LADSoft)
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

#include "compiler.h"
#include <stack>
#include "ccerr.h"
#include "config.h"
#include "unmangle.h"
#include "OptUtils.h"
#include "lex.h"
#include "help.h"
#include "memory.h"
#include "mangle.h"
#include "declare.h"
#include "types.h"
#include <algorithm>
#include "initbackend.h"
#include "Utils.h"

#define MANGLE_SIZE(buf) (n - (buf - orig))
#define PUTCH(buf, ch) { if (n - (buf - orig) > 0) *buf++ = ch; }
#define PUTZERO(buf) { if (n - (buf - orig) > 0) *buf = '\0'; }
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

template <int n>
static char* lookupName(char (&orig)[n], char* in, const char* name);
template <int n>
char* mangleType(char(&orig)[n], char* in, Type* tp, bool first);
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
template <int n>
char* mangleNameSpaces(char(&orig)[n], char* in, SYMBOL* sym)
{
    if (!sym)
        return in;
    in = mangleNameSpaces(orig, in, sym->sb->parentNameSpace);
    if (sym->sb->parentNameSpace && !strcmp(sym->name, "__1") && !strcmp(sym->sb->parentNameSpace->name, "std"))
        return in;
    Optimizer::my_sprintf(in, MANGLE_SIZE(in), "@%s", sym->name);
    return in + strlen(in);
}
char* mangleNameSpaces(char* in, SYMBOL* sym)
{
    char(&buf)[4096] = (char(&)[4096])in;
    return mangleNameSpaces(buf, in, sym);
}

template <int n>
static char* mangleTemplate(char (&orig)[n], char *buf, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
template <int n>
static char* getName(char (&orig)[n], char *in, SYMBOL* sym);

template <int n>
static char* mangleParent(char (&orig)[n], char* in, SYMBOL* sym)
{
    in += strlen(in);
    if (sym->sb->parent->sb->templateLevel && sym->sb->parent->templateParams)
    {
        PUTCH(in, '@');
        in = mangleTemplate(orig, in, sym->sb->parent, sym->sb->parent->templateParams);
   }
    else
    {
        Optimizer::my_sprintf(in, MANGLE_SIZE(in), "@%s", sym->sb->parent->name);
    }
    in += strlen(in);
    return in;
}
template <int n>
static char* mangleClasses(char (&orig)[n], char* in, SYMBOL* sym)
{
    if (!sym)
        return in;
    if (sym->sb->parentClass)
        in = mangleClasses(orig, in, sym->sb->parentClass);
    if (sym->sb->parent)
    {
        in = mangleParent(orig, in, sym);
    }
    if (sym->sb->castoperator)
    {
        Utils::StrCat(in, MANGLE_SIZE(in), "@");
    }
    else if (sym->sb->templateLevel && sym->templateParams)
    {
        PUTCH(in,  '@');
        PUTZERO(in);
        mangleTemplate(orig, in, sym, sym->templateParams);
    }
    else
    {
        Optimizer::my_sprintf(in, MANGLE_SIZE(in), "@%s", sym->name);
    }
    return in + strlen(in);
}
template <int n>
static char* mangleExpressionInternal(char (&orig)[n], char* buf, EXPRESSION* exp)
{
    while (IsCastValue(exp))
        exp = exp->left;
    if (isintconst(exp))
    {
        if (exp->type == ExpressionNode::const_)
        {
            Optimizer::my_sprintf(buf, MANGLE_SIZE(buf), "%lld?", exp->v.sp->sb->value.i);
        }
        else
        {
            Optimizer::my_sprintf(buf, MANGLE_SIZE(buf), "%lld?", exp->v.i);
        }
        if (buf[0] == '-')
            buf[0] = '_';
    }
    else
    {
        bool nonpointer = false;
        while (IsLValue(exp))
        {
            nonpointer = true;
            exp = exp->left;
        }
        switch (exp->type)
        {
            case ExpressionNode::nullptr_:
                PUTCH(buf,  'n');
                PUTZERO(buf);
                *buf = 0;
                break;
            case ExpressionNode::arrayadd_:
            case ExpressionNode::structadd_:
            case ExpressionNode::add_:
                PUTCH(buf, 'p');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::sub_:
                PUTCH(buf, 's');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::mul_:
            case ExpressionNode::umul_:
            case ExpressionNode::arraymul_:
                PUTCH(buf, 'm');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::umod_:
            case ExpressionNode::mod_:
                PUTCH(buf, 'o');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::dot_:
                PUTCH(buf, 'D');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::pointsto_:
                PUTCH(buf, 'P');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::div_:
            case ExpressionNode::udiv_:
            case ExpressionNode::arraydiv_:
                PUTCH(buf, 'd');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::lsh_:
            case ExpressionNode::arraylsh_:
                PUTCH(buf, 'h');
                PUTCH(buf, 'l');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::rsh_:
            case ExpressionNode::ursh_:
                PUTCH(buf, 'h');
                PUTCH(buf, 'r');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::hook_:
                PUTCH(buf, 'C');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right->left);
                buf = mangleExpressionInternal(orig, buf, exp->right->right);
                *buf = 0;
                break;
            case ExpressionNode::assign_:
                PUTCH(buf, 'a');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::eq_:
                PUTCH(buf, 'c');
                PUTCH(buf, 'e');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::ne_:
                PUTCH(buf, 'c');
                PUTCH(buf, 'n');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::uminus_:
                PUTCH(buf, 'u');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                break;
            case ExpressionNode::not_:
                PUTCH(buf, 'l');
                PUTCH(buf, 'n');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                break;
            case ExpressionNode::compl_:
                PUTCH(buf, 'b');
                PUTCH(buf, 'n');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                *buf = 0;
                break;
            case ExpressionNode::ult_:
            case ExpressionNode::lt_:
                PUTCH(buf, 'c');
                PUTCH(buf, 'l');
                PUTCH(buf, 't');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::ule_:
            case ExpressionNode::le_:
                PUTCH(buf, 'c');
                PUTCH(buf, 'l');
                PUTCH(buf, 'e');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::ugt_:
            case ExpressionNode::gt_:
                PUTCH(buf, 'c');
                PUTCH(buf, 'g');
                PUTCH(buf, 't');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::uge_:
            case ExpressionNode::ge_:
                PUTCH(buf, 'c');
                PUTCH(buf, 'g');
                PUTCH(buf, 'e');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::and_:
                PUTCH(buf, 'b');
                PUTCH(buf, 'a');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::land_:
                PUTCH(buf, 'l');
                PUTCH(buf, 'a');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::or_:
                PUTCH(buf, 'b');
                PUTCH(buf, 'o');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::lor_:
                PUTCH(buf, 'l');
                PUTCH(buf, 'o');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::xor_:
                PUTCH(buf, 'b');
                PUTCH(buf, 'x');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::auto_inc_:
                PUTCH(buf, 'i');
                PUTCH(buf, 'p');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::auto_dec_:
                PUTCH(buf, 'i');
                PUTCH(buf, 's');
                PUTZERO(buf);
                buf = mangleExpressionInternal(orig, buf, exp->left);
                buf = mangleExpressionInternal(orig, buf, exp->right);
                *buf = 0;
                break;
            case ExpressionNode::select_:
                buf = mangleExpressionInternal(orig, buf, exp->left);
                break;
            case ExpressionNode::templateselector_: {
                auto tsl = exp->v.templateSelector;
                SYMBOL* ts = (*tsl)[1].sp;
                PUTCH(buf, 't');
                PUTCH(buf, 's');
                PUTZERO(buf);
                if ((*tsl)[1].isTemplate && (*tsl)[1].templateParams)  // may be an empty variadic
                {
                    buf = mangleTemplate(orig, buf, ts, (*tsl)[1].templateParams);
                }
                auto find = (*tsl).begin();
                ++find;
                ++find;
                for (; find != (*tsl).end(); ++find)
                {
                    PUTCH(buf, 't');
                    PUTZERO(buf);
                    if (find->isTemplate && find->templateParams)
                    {
                        char temp[5000], * p = temp;
                        SYMBOL s = {};
                        s.name = find->name;
                        s.tp = &stdint;
                        mangleTemplate(temp, p, &s, find->templateParams);
                        buf = lookupName(orig, buf, p);
                    }
                    else
                    {
                        buf = lookupName(orig, buf, find->name);
                    }
                    buf += strlen(buf);
                }
                *buf = 0;
                break;
            }
            case ExpressionNode::templateparam_:
            case ExpressionNode::auto_:
                PUTCH(buf, 't');
                PUTCH(buf, 'p');
                PUTZERO(buf);
                buf = lookupName(orig, buf, exp->v.sp->name);
                buf += strlen(buf);
                *buf = 0;
                break;
            case ExpressionNode::thisref_:
            case ExpressionNode::funcret_:
            case ExpressionNode::constexprconstructor_:
                buf = mangleExpressionInternal(orig, buf, exp->left);
                *buf = 0;
                break;
            case ExpressionNode::callsite_: {
                if (exp->v.func->ascall)
                {
                    PUTCH(buf, 'f');
                    PUTZERO(buf);
                    buf = getName(orig, buf, exp->v.func->sp);
                    if (exp->v.func->arguments)
                    {
                        for (auto arg : *exp->v.func->arguments)
                            if (arg->exp)
                            {
                                PUTCH(buf, 'F');
                                PUTZERO(buf);
                                buf = mangleExpressionInternal(orig, buf, arg->exp);
                            }
                    }
                }
                else
                {
                    PUTCH(buf, 'e');
                    PUTCH(buf, '?');
                    PUTZERO(buf);
                    buf = getName(orig, buf, exp->v.func->sp);
                    buf += strlen(buf);
                    PUTCH(buf, '.');
                    buf = mangleType(orig, buf, exp->v.func->sp->tp, true);
                }
                break;
            }
            case ExpressionNode::pc_:
            case ExpressionNode::global_:
            case ExpressionNode::const_:
                if (exp->v.sp->tp->IsFunction())
                {
                    PUTCH(buf, 'e');
                    PUTCH(buf, '?');
                    PUTZERO(buf);
                    Utils::StrCpy(buf, MANGLE_SIZE(buf), exp->v.sp->name);
                    buf += strlen(buf);
                    PUTCH(buf, '.');
                    PUTZERO(buf);
                    buf = mangleType(orig, buf, exp->v.sp->tp, true);
                }
                else
                {
                    PUTCH(buf, 'g');
                    if (!nonpointer)
                        PUTCH(buf, '?');
                    PUTZERO(buf);
                    Utils::StrCpy(buf, MANGLE_SIZE(buf), exp->v.sp->name);
                    PUTCH(buf, '.');
                    PUTZERO(buf);
                    *buf = 0;
                }
                break;
            case ExpressionNode::sizeofellipse_:
                PUTCH(buf, 'z');
                PUTZERO(buf);
                buf = getName(orig, buf, exp->v.templateParam->first);
                buf += strlen(buf);
                break;
            case ExpressionNode::comma_:
                PUTCH(buf, 'v');
                PUTZERO(buf);
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
template <int n>
static char* mangleExpression(char (&orig)[n], char* buf, EXPRESSION* exp)
{
    if (exp)
    {
        PUTCH(buf ,'?');
        PUTZERO(buf);
        buf = mangleExpressionInternal(orig, buf, exp);
    }
    return buf;
}
template <int n>
static char* mangleTemplate(char (&orig)[n], char* buf, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params)
{
    bool bySpecial = false;
    if (params && params->size() && params->front().second->type == TplType::new_ &&
        ((sym->sb && (sym->sb->instantiated && !sym->sb->templateLevel)) || (params && params->front().second->bySpecialization.types)))
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
        Utils::StrCpy(buf, MANGLE_SIZE(buf), sym->name);
        while (*buf)
            buf++;
        PUTCH(buf, '.');
        PUTZERO(buf);
    }
    else
    {
        PUTCH(buf, '#');
        PUTZERO(buf);
        Utils::StrCpy(buf, MANGLE_SIZE(buf), sym->name);
        Utils::StrCat(buf, MANGLE_SIZE(buf), ".");
        buf += strlen(buf);
    }
    static StackList<std::list<TEMPLATEPARAMPAIR>::iterator> nestedStack;
    NestedStack<std::list<TEMPLATEPARAMPAIR>::iterator> tps(nestedStack);

    if (params)
    {
        auto it = params->begin();
        auto ite = params->end();
        for (; it != ite;)
        {
            switch (it->second->type)
            {
                case TplType::typename_:
                    if (it->second->packed)
                    {
                        if (it->second->byPack.pack)
                        {
                            for (auto pack : *it->second->byPack.pack)
                                buf = mangleType(orig, buf, pack.second->byClass.val, true);
                        }
                        else
                        {
                            PUTCH(buf, 'e');
                            PUTZERO(buf);
                            buf = getName(orig, buf, it->first);
                        }
                    }
                    else if (bySpecial)
                    {
                        if (it->second->byClass.val)
                        {
                            buf = mangleType(orig, buf, it->second->byClass.val, true);
                        }
                        else if (it->second->byClass.dflt)
                        {
                            buf = mangleType(orig, buf, it->second->byClass.dflt, true);
                        }
                        else
                        {
                            buf = getName(orig, buf, it->first);
                        }
                    }
                    else if (sym->sb && sym->sb->instantiated && it->second->byClass.val)
                    {
                        buf = mangleType(orig, buf, it->second->byClass.val, true);
                    }
                    else
                    {
                        if (it->second->byClass.dflt)
                        {
                            buf = mangleType(orig, buf, it->second->byClass.dflt, true);
                        }
                        else
                        {
                            buf = getName(orig, buf, it->first);
                        }
                    }
                    break;
                case TplType::template_:
                    if (it->second->packed)
                        PUTCH(buf, 'e');
                    PUTZERO(buf);
                    if (sym->sb && sym->sb->instantiated && it->second->byTemplate.val)
                    {
                        buf = mangleTemplate(orig, buf, it->second->byTemplate.val, it->second->byTemplate.val->templateParams);
                    }
                    else if (it->first)
                    {
                        buf = getName(orig, buf, it->first);
                    }
                    else
                    {
                        buf = getName(orig, buf, it->second->byTemplate.dflt);
                    }
                    break;
                case TplType::int_:
                    if (it->second->packed)
                    {
                        PUTCH(buf, 'e');
                        PUTZERO(buf);
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
                        buf = mangleType(orig, buf, it->second->byNonType.tp, true);
                        if ((bySpecial || sym->sb && sym->sb->instantiated) && it->second->byNonType.val)
                        {
                            EXPRESSION* exp =
                                bySpecial && it->second->byNonType.dflt ? it->second->byNonType.dflt : it->second->byNonType.val;
                            buf = mangleExpression(orig, buf, exp);
                        }
                        else if (it->second->byNonType.dflt)
                        {
                            buf = mangleExpression(orig, buf, it->second->byNonType.dflt);
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
    PUTCH(buf, '~');
    PUTZERO(buf);
    return buf;
}
template <int n>
static char* lookupName(char (&orig)[n], char* in, const char* name)
{
    int i;
    for (i = 0; i < mangledNamesCount; i++)
        if (!strcmp(name, mangledNames[i]))
            break;
    if (i < mangledNamesCount)
    {
        Optimizer::my_sprintf(in, MANGLE_SIZE(in), "n%c", i < 10 ? i + '0' : i - 10 + 'A');
    }
    else
    {
        if (mangledNamesCount < MAX_MANGLE_NAME_COUNT)
            Utils::StrCpy(mangledNames[mangledNamesCount++], name);
        Optimizer::my_sprintf(in, MANGLE_SIZE(in), "%d%s", strlen(name), name);
    }
    return in;
}
template <int n>
static char* getName(char (&orig)[n], char* in, SYMBOL* sym)
{
    if (!sym)
    {
        Utils::StrCpy(in, MANGLE_SIZE(in), "????");
    }
    else if (!sym->sb)
    {
        in = lookupName(orig, in, sym->name);
    }
    else
    {
        int i;
        char buf[4096], *p;
        p = mangleNameSpaces(buf, buf, sym->sb->parentNameSpace);
        p = mangleClasses(buf, p, sym->sb->parentClass);
        if (sym->sb->parent)
        {
            p = mangleParent(orig, p, sym);
        }
        if (p != buf)
            PUTCH(p, '@');
        PUTZERO(p);
        if (sym->sb->templateLevel && sym->templateParams)
        {
            p = mangleTemplate(buf, p, sym, sym->templateParams);
        }
        else
        {
            Utils::StrCpy(p, MANGLE_SIZE(buf), sym->name);
        }
        in = lookupName(orig, in, buf);
    }
    while (*in)
        in++;
    return in;
}
char* mangleType(char* buf, int len, Type* tp, bool first)
{
    char nm[8000];
    mangleType(nm, nm, tp, first);
    return Utils::StrCpy(buf, len, nm);
}
template <int n>
char* mangleType(char (&orig)[n], char* in, Type* tp, bool first)
{
    char nm[4096];
    int i;
    if (!tp)
    {
        Optimizer::my_sprintf(in, MANGLE_SIZE(in), "%d%s", strlen("initializer-list"), "initializer-list");
        while (*in)
            in++;
        return in;
    }
    while (tp)
    {
        while (tp->type == BasicType::typedef_ && !tp->sp->templateParams)
            tp = tp->btp;
        if (tp->type == BasicType::typedef_)
        {
            {
                if (tp->IsConst())
                    PUTCH(in,  'x');
                if (tp->IsVolatile())
                    PUTCH(in,  'y');
                if (tp->IsLRefQual())
                    PUTCH(in,  'r');
                if (tp->IsRRefQual())
                    PUTCH(in,  'R');
                PUTZERO(in);
            }
            in = mangleTemplate(orig, in, tp->sp, tp->sp->templateParams);
            return in;
        }
        if (tp->IsStructured() && tp->BaseType()->sp->sb && tp->BaseType()->sp->sb->templateLevel)
        {
            {
                if (tp->IsConst())
                    PUTCH(in,  'x');
                if (tp->IsVolatile())
                    PUTCH(in,  'y');
                if (tp->IsLRefQual())
                    PUTCH(in,  'r');
                if (tp->IsRRefQual())
                    PUTCH(in,  'R');
                PUTZERO(in);
            }
            in = mangleClasses(orig, in, tp->BaseType()->sp->sb->parentClass);
            if (tp->BaseType()->sp->sb->parent)
            {
                in = mangleParent(orig, in, tp->BaseType()->sp);
            }
            in = mangleTemplate(orig, in, tp->BaseType()->sp, tp->BaseType()->sp->templateParams);
            return in;
        }
        else
        {
            if (tp->IsConst())
                PUTCH(in,  'x');
            if (tp->IsVolatile())
                PUTCH(in,  'y');
            if (tp->IsLRefQual())
                PUTCH(in,  'r');
            if (tp->IsRRefQual())
                PUTCH(in,  'R');
            PUTZERO(in);
            tp = tp->BaseType();
            if (tp->IsInt() && tp->btp && tp->btp->type == BasicType::enum_)
                tp = tp->btp;
            switch (tp->type)
            {
                case BasicType::func_:
                case BasicType::ifunc_:
                    if (tp->BaseType()->sp && ismember(tp->BaseType()->sp) && !first)
                    {
                        PUTCH(in,  'M');
                        PUTZERO(in);
                        in = getName(orig, in, tp->sp->sb->parentClass);
                        while (*in)
                            in++;
                    }
                    PUTCH(in,  'q');
                    PUTZERO(in);
                    for (auto sym : *tp->syms)
                    {
                        if (!sym->sb->thisPtr)
                            in = mangleType(orig, in, sym->tp, true);
                    }
                    PUTCH(in,  '.');
                    PUTZERO(in);
                    // return value comes next
                    break;
                case BasicType::memberptr_:
                    PUTCH(in,  'M');
                    PUTZERO(in);
                    in = getName(orig, in, tp->sp);
                    if (tp->btp->IsFunction())
                    {
                        PUTCH(in,  'q');
                        PUTZERO(in);
                        for (auto sym : *tp->btp->BaseType()->syms)
                        {
                            if (!sym->sb->thisPtr)
                                in = mangleType(orig, in, sym->tp, true);
                        }
                        PUTCH(in,  '.');
                        PUTZERO(in);
                        tp = tp->btp;  // so we can get to tp->btp->btp
                    }
                    else
                    {
                        PUTCH(in,  '.');
                        PUTZERO(in);
                    }
                    break;
                case BasicType::enum_:
                case BasicType::struct_:
                case BasicType::union_:
                case BasicType::class_:
                    in = getName(orig, in, tp->sp);
                    return in;
                case BasicType::bool_:
                    in = lookupName(orig, in, "bool");
                    while (*in)
                        in++;
                    break;
                case BasicType::string_:
                    in = lookupName(orig, in, "__string");
                    while (*in)
                        in++;
                    break;
                case BasicType::object_:
                    in = lookupName(orig, in, "__object");
                    while (*in)
                        in++;
                    break;
                case BasicType::unsigned_short_:
                    PUTCH(in,  'u');
                    PUTCH(in, 's');
                    PUTZERO(in);
                    break;
                case BasicType::short_:
                    PUTCH(in,  's');
                    PUTZERO(in);
                    break;
                case BasicType::unsigned_:
                    PUTCH(in,  'u');
                    PUTCH(in, 'i');
                    PUTZERO(in);
                    break;
                case BasicType::int_:
                    PUTCH(in,  'i');
                    PUTZERO(in);
                    break;
                case BasicType::unsigned_bitint_:
                    PUTCH(in,  'u');
                    PUTCH(in, 'B');
                    Optimizer::my_sprintf(in, MANGLE_SIZE(in), "%d?", tp->bitintbits);
                    in += strlen(in);
                    break;
                case BasicType::bitint_:
                    PUTCH(in,  'B');
                    Optimizer::my_sprintf(in, MANGLE_SIZE(in), "%d?", tp->bitintbits);
                    in += strlen(in);
                    break;
                case BasicType::unative_:
                    PUTCH(in,  'u');
                    PUTCH(in, 'N');
                    PUTZERO(in);
                    break;
                case BasicType::inative_:
                    PUTCH(in,  'N');
                    PUTZERO(in);
                    break;
                case BasicType::char16_t_:
                    PUTCH(in,  'h');
                    PUTZERO(in);
                    break;
                case BasicType::char32_t_:
                    PUTCH(in,  'H');
                    PUTZERO(in);
                    break;
                case BasicType::unsigned_long_:
                    PUTCH(in,  'u');
                    PUTCH(in, 'l');
                    PUTZERO(in);
                    break;
                case BasicType::long_:
                    PUTCH(in,  'l');
                    PUTZERO(in);
                    break;
                case BasicType::unsigned_long_long_:
                    PUTCH(in,  'u');
                    PUTCH(in, 'L');
                    PUTZERO(in);
                    break;
                case BasicType::long_long_:
                    PUTCH(in,  'L');
                    PUTZERO(in);
                    break;
                case BasicType::char8_t_:
                case BasicType::unsigned_char_:
                    PUTCH(in,  'u');
                    PUTCH(in, 'c');
                    PUTZERO(in);
                    break;
                case BasicType::char_:
                    PUTCH(in,  'c');
                    PUTZERO(in);
                    break;
                case BasicType::signed_char_:
                    PUTCH(in,  'S');
                    PUTCH(in,  'c');
                    PUTZERO(in);
                    break;
                case BasicType::wchar_t_:
                    PUTCH(in,  'C');
                    PUTZERO(in);
                    break;
                case BasicType::float__complex_:
                    PUTCH(in,  'F');
                    PUTZERO(in);
                    break;
                case BasicType::double__complex_:
                    PUTCH(in,  'D');
                    PUTZERO(in);
                    break;
                case BasicType::long_double_complex_:
                    PUTCH(in,  'G');
                    PUTZERO(in);
                    break;
                case BasicType::float_:
                    PUTCH(in,  'f');
                    PUTZERO(in);
                    break;
                case BasicType::double_:
                    PUTCH(in,  'd');
                    PUTZERO(in);
                    break;
                case BasicType::long_double_:
                    PUTCH(in,  'g');
                    PUTZERO(in);
                    break;
                case BasicType::pointer_:
                    if (tp->nullptrType)
                    {
                        in = lookupName(orig, in, "nullptr_t");
                        while (*in)
                            in++;
                        return in;
                    }
                    else
                    {
                        if (!tp->array)
                        {
                            PUTCH(in,  'p');
                        }
                        else
                        {
                            PUTCH(in,  'A');
                        }
                        PUTZERO(in);
                    }
                    break;
                case BasicType::far_:
                    PUTCH(in,  'P');
                    PUTZERO(in);
                    break;
                case BasicType::lref_:
                    PUTCH(in,  'r');
                    PUTZERO(in);
                    break;
                case BasicType::rref_:
                    PUTCH(in,  'R');
                    PUTZERO(in);
                    break;
                case BasicType::ellipse_:
                    PUTCH(in,  'e');
                    PUTZERO(in);
                    break;
                case BasicType::void_:
                    PUTCH(in,  'v');
                    PUTZERO(in);
                    break;
                case BasicType::any_:
                    PUTCH(in,  'V');  // this needs to be distinct from void for internal matching....
                    PUTZERO(in);
                    break;
                case BasicType::templateparam_:
                    if (tp->templateParam->second->type == TplType::typename_ && tp->templateParam->second->byClass.val &&
                        tp->templateParam->second->byClass.val->BaseType()->type != BasicType::templateparam_)
                        in = mangleType(orig, in, tp->templateParam->second->byClass.val, false);
                    else
                        in = getName(orig, in, tp->templateParam->first);
                    break;
                case BasicType::templateselector_: {
                    auto s = (*tp->sp->sb->templateSelector).begin();
                    auto se = (*tp->sp->sb->templateSelector).end();
                    char* p;
                    ++s;
                    if (s->isTemplate)
                        p = mangleTemplate(nm, nm, s->sp, s->sp->sb && s->sp->sb->instantiated ? s->sp->templateParams : s->templateParams);
                    else
                        p = getName(nm, nm, s->sp);
                    PUTZERO(p);
                    if (strlen(nm) > sizeof(nm))
                        p = mangleTemplate(nm, p, s->sp, s->templateParams);
                    for (++s; s != se; ++s)
                    {
                        Utils::StrCpy(p, sizeof(nm) - (p - nm), "@");
                        p += strlen(p);
                        if (s->isTemplate && s->templateParams)
                        {
                            SYMBOL s2 = {};
                            s2.name = s->name;
                            s2.tp = &stdint;
                            p = mangleTemplate(nm, p, &s2, s->templateParams);
                        }
                        else
                        {
                            Utils::StrCpy(p, sizeof(nm) - (p - nm), s->name);
                            p += strlen(p);
                        }
                    }
                    p = nm;
                    while (isdigit(*p))
                        p++;
                    Optimizer::my_sprintf(in, MANGLE_SIZE(in), "%d%s", strlen(p), p);
                    in += strlen(in);
                    return in;
                }
                break;
                case BasicType::templatedecltype_:
                    PUTCH(in,  'E');
                    PUTZERO(in);
                    in = mangleExpression(orig, in, tp->templateDeclType);
                    break;
                case BasicType::aggregate_:
                    in = getName(orig, in, tp->sp);
                    break;
                case BasicType::auto_:
                    PUTCH(in,  'a');
                    PUTZERO(in);
                    break;
                case BasicType::templatedeferredtype_:
                    // we want something the linkerwill treat as an error,
                    // so we can detect problems with these not being properly replaced
                    PUTCH(in,  MANGLE_DEFERRED_TYPE_CHAR);
                    PUTZERO(in);
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
        default: {
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
                            {
                                if (!validType((Type*)dflt, byVal))
                                    return false;
                                result += 'c';
                                Type* tp1 = (Type*)dflt;
                                while (tp1->IsPtr() || tp1->IsRef())
                                    tp1 = tp1->BaseType()->btp;
                                char* p = buf;
                                if (tp1->IsStructured() && tp1->BaseType()->sp->sb->parentClass)
                                {
                                    p = mangleType(buf, buf, tp1->BaseType()->sp->sb->parentClass->tp, true);
                                    *p = 0;
                                }
                                *(mangleType(buf, p, (Type*)dflt, true)) = 0;
                                break;
                            }
                            case TplType::int_:
                                result += 'i';
                                *mangleExpression(buf, buf, (EXPRESSION*)dflt) = 0;
                                break;
                            case TplType::template_:
                                result += 't';
                                *mangleTemplate(buf, buf, (SYMBOL*)dflt, tpl.second->byTemplate.args) = 0;
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
                        {
                            if (!validType((Type*)dflt, byVal))
                                return false;
                            result += 'c';
                            Type* tp1 = (Type*)dflt;
                            while (tp1->IsPtr() || tp1->IsRef())
                                tp1 = tp1->BaseType()->btp;
                            char* p = buf;
                            if (tp1->IsStructured() && tp1->BaseType()->sp->sb->parentClass)
                            {
                                p = mangleType(buf, buf, tp1->BaseType()->sp->sb->parentClass->tp, true);
                                *p = 0;
                            }
                            *(mangleType(buf, p, (Type*)dflt, true)) = 0;
                            break;
                        }
                        case TplType::int_:
                            if (((EXPRESSION*)dflt)->type == ExpressionNode::templateparam_)
                                return false;
                            result += 'i';
                            *mangleExpression(buf, buf, (EXPRESSION*)dflt) = 0;
                            break;
                        case TplType::template_:
                            result += 't';
                            *mangleTemplate(buf, buf, (SYMBOL*)dflt, param.second->byTemplate.args) = 0;
                            break;
                    }
                    result += buf;
                }
            }
        }
    }
    return true;
}

void GetClassKey(char* buf, int len, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params)
{
    const int n = 8000;
    char orig[n];
    mangledNamesCount = 0;
    SYMBOL* lastParent;
    char* p = orig;
    if (sym->sb->parent)
    {
        lastParent = sym->sb->parent;
        while (lastParent->sb->parentClass)
            lastParent = lastParent->sb->parentClass;
        p = mangleNameSpaces(orig, p, lastParent->sb->parentNameSpace);
        p = mangleClasses(orig, p, sym->sb->parent->sb->parentClass);
        p = mangleParent(orig, p, sym);
    }
    else
    {
        lastParent = sym;
        while (lastParent->sb->parentClass)
            lastParent = lastParent->sb->parentClass;
        p = mangleNameSpaces(orig, p, lastParent->sb->parentNameSpace);
        p = mangleClasses(orig, p, sym->sb->parentClass);

    }
    PUTCH(p, '@');
    PUTZERO(p);
    p = mangleTemplate(orig, p, sym, params);
    *p = 0;
    Utils::StrCpy(buf, len, orig);
}
void SetLinkerNames(SYMBOL* sym, Linkage linkage, bool isTemplateDefinition)
{
    const int n = 8192;
    char orig[n], *p = orig;
    orig[0] = '\0';
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
                (sym->tp->IsFunction() || istype(sym) || sym->sb->parentNameSpace || sym->sb->parentClass ||
                 sym->sb->templateLevel))
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
            p = mangleClasses(orig, p, theCurrentFunc);
            Optimizer::my_sprintf(p, MANGLE_SIZE(p), "@%s", sym->name);

            break;
        case Linkage::pascal_:
            if (sym->name[0] == '.')
            {
                orig[0] = '_';
                Utils::StrCpy(orig + 1, sizeof(orig) - 1, sym->name);
            }
            else
            {
                Utils::StrCpy(orig, sizeof(orig) - 1, sym->name);
            }
            std::transform(orig, orig + strlen(orig), orig, ::toupper);
            break;
        case Linkage::stdcall_:
            if (sym->name[0] == '.')
            {
                orig[0] = '_';
                Utils::StrCpy(orig + 1, sizeof(orig) - 1, sym->name);
            }
            else
            {
                Utils::StrCpy(orig, sym->name);
            }
            break;
        case Linkage::c_:
        default:
            if (sym->sb->parent)
                if (sym->sb->uniqueID == 0)
                    sym->sb->uniqueID = uniqueID++;
            if (sym->sb->storage_class == StorageClass::localstatic_ && sym->sb->parent)
            {
                Utils::StrCpy(orig, sym->sb->parent->sb->decoratedName);
                Utils::StrCat(orig, "_");
                Utils::StrCat(orig, sym->name);
                sprintf(orig + strlen(orig), "_%d", sym->sb->uniqueID);
            }
            else
            {
                orig[0] = '_';
                Utils::StrCpy(orig + 1, sizeof(orig)-1, sym->name);
            }
            break;
        case Linkage::cpp_:
            if (isTemplateDefinition)
                PUTCH(p, '@');
            PUTZERO(p);
            if (sym->sb->parent)
            {
                lastParent = sym->sb->parent;
                while (lastParent->sb->parentClass)
                    lastParent = lastParent->sb->parentClass;
                p = mangleNameSpaces(orig, p, lastParent->sb->parentNameSpace);
                p = mangleClasses(orig, p, sym->sb->parent->sb->parentClass);
                p = mangleParent(orig, p, sym);
            }
            else
            {
                lastParent = sym;
                while (lastParent->sb->parentClass)
                    lastParent = lastParent->sb->parentClass;
                p = mangleNameSpaces(orig, p, lastParent->sb->parentNameSpace);
                p = mangleClasses(orig, p, sym->sb->parentClass);

            }
            PUTCH(p, '@');
            PUTZERO(p);
            if (sym->sb->templateLevel && sym->templateParams)
            {
                p = mangleTemplate(orig, p, sym, sym->templateParams);
            }
            else
            {
                Utils::StrCpy(p, sizeof(orig)- (p - orig), sym->name);
                p += strlen(p);
            }
            if (sym->tp->IsFunction())
            {
                PUTCH(p, '.');
                PUTZERO(p);
                if (sym->sb->castoperator)
                {
                    int tmplCount = 0;
                    PUTCH(p, 'o');
                    PUTZERO(p);
                    p = mangleType(orig, p, sym->tp->BaseType()->btp, true);  // cast operators get their cast type in the name
                    PUTCH(p, '.');
                    PUTZERO(p);
                    p = mangleType(orig, p, sym->tp, true);  // add the $qv
                    while (p > orig && (*--p != '.' || tmplCount))
                        if (*p == '~')
                            tmplCount++;
                        else if (*p == '#')
                            tmplCount--;
                    p[1] = 0;
                }
                else
                {
                    p = mangleType(orig, p, sym->tp, true);  // otherwise functions get their parameter list in the name
                                                       //                    if (!sym->sb->templateLevel)
                    {
                        int tmplCount = 0;
                        while (p > orig && (*--p != '.' || tmplCount))
                            if (*p == '~')
                                tmplCount++;
                            else if (*p == '#')
                                tmplCount--;
                        if (sym->tp->BaseType()->btp->type == BasicType::memberptr_)
                        {
                            while (p > orig && (*--p != '.' || tmplCount))
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
    orig[n - 1] = '\0';
    sym->sb->decoratedName = litlate(orig);
}

}  // namespace Parser