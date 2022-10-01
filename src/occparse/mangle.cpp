/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compilepr package.
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
namespace Parser
{

const char* overloadNameTab[] = {"$bctr",  "$bdtr",   "$bcast",  "$bnew",   "$bdel",   "$badd",   "$bsub",   "$bmul",    "$bdiv",
                                 "$bshl",  "$bshr",   "$bmod",   "$bequ",   "$bneq",   "$blt",    "$bleq",   "$bgt",     "$bgeq",
                                 "$basn",  "$basadd", "$bassub", "$basmul", "$basdiv", "$basmod", "$basshl", "$bsasshr", "$basand",
                                 "$basor", "$basxor", "$binc",   "$bdec",   "$barray", "$bcall",  "$bstar",  "$barrow",  "$bcomma",
                                 "$blor",  "$bland",  "$bnot",   "$bor",    "$band",   "$bxor",   "$bcpl",   "$bnwa",    "$bdla",
                                 "$blit",  "$badd",   "$bsub",   "$bmul",   "$band"};
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
                                     "$basn",
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
                                     "$binc",
                                     "$bdec",
                                     "$barray",
                                     "$bcall",
                                     "$bstar",
                                     "op_MemberSelection",
                                     "op_Comma",
                                     "op_LogicalOr",
                                     "op_LogicalAnd",
                                     "op_LogicalNot",
                                     "op_BitwiseOr",
                                     "op_BitwiseAnd",
                                     "op_ExclusiveOr",
                                     "op_OnesComplement",
                                     "$bnwa",
                                     "$bdla",
                                     "$blit",
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
static bool inKeyCreation;
void mangleInit()
{
    uniqueID = 0;
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        memcpy(overloadNameTab, msiloverloadNameTab, sizeof(msiloverloadNameTab));
        memcpy(cpp_funcname_tab, msiloverloadNameTab, sizeof(msiloverloadNameTab));
    }
    inKeyCreation = false;
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
        if (exp->type == en_const)
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
            case en_nullptr:
                *buf++ = 'n';
                *buf = 0;
                break;
            case en_arrayadd:
            case en_structadd:
            case en_add:
                *buf++ = 'p';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_sub:
                *buf++ = 's';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_mul:
            case en_umul:
            case en_arraymul:
                *buf++ = 'm';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_umod:
            case en_mod:
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_dot:
                *buf++ = 'D';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_pointsto:
                *buf++ = 'P';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_div:
            case en_udiv:
            case en_arraydiv:
                *buf++ = 'd';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_lsh:
            case en_arraylsh:
                *buf++ = 'h';
                *buf++ = 'l';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_rsh:
            case en_ursh:
                *buf++ = 'h';
                *buf++ = 'r';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_cond:
                *buf++ = 'C';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right->left);
                buf = mangleExpressionInternal(buf, exp->right->right);
                *buf = 0;
                break;
            case en_assign:
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_eq:
                *buf++ = 'c';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_ne:
                *buf++ = 'c';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_uminus:
                *buf++ = 'u';
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case en_not:
                *buf++ = 'l';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case en_compl:
                *buf++ = 'b';
                *buf++ = 'n';
                buf = mangleExpressionInternal(buf, exp->left);
                *buf = 0;
                break;
            case en_ult:
            case en_lt:
                *buf++ = 'c';
                *buf++ = 'l';
                *buf++ = 't';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_ule:
            case en_le:
                *buf++ = 'c';
                *buf++ = 'l';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_ugt:
            case en_gt:
                *buf++ = 'c';
                *buf++ = 'g';
                *buf++ = 't';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_uge:
            case en_ge:
                *buf++ = 'c';
                *buf++ = 'g';
                *buf++ = 'e';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_and:
                *buf++ = 'b';
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_land:
                *buf++ = 'l';
                *buf++ = 'a';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_or:
                *buf++ = 'b';
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_lor:
                *buf++ = 'l';
                *buf++ = 'o';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_xor:
                *buf++ = 'b';
                *buf++ = 'x';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_autoinc:
                *buf++ = 'i';
                *buf++ = 'p';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_autodec:
                *buf++ = 'i';
                *buf++ = 's';
                buf = mangleExpressionInternal(buf, exp->left);
                buf = mangleExpressionInternal(buf, exp->right);
                *buf = 0;
                break;
            case en_select:
                buf = mangleExpressionInternal(buf, exp->left);
                break;
            case en_templateselector: {
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
                for (; find != (*tsl).end(); ++find)
                {
                    *buf++ = 't';
                    buf = lookupName(buf, find->name);
                    buf += strlen(buf);
                }
                *buf = 0;
                break;
            }
            case en_templateparam:
            case en_auto:
                *buf++ = 't';
                *buf++ = 'p';
                buf = lookupName(buf, exp->v.sp->name);
                buf += strlen(buf);
                *buf = 0;
                break;
            case en_thisref:
            case en_funcret:
                buf = mangleExpressionInternal(buf, exp->left);
                *buf = 0;
                break;
            case en_func: {
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
                    *buf++ = '$';
                    buf = mangleType(buf, exp->v.func->sp->tp, true);
                }
                break;
            }
            case en_pc:
            case en_global:
            case en_const:
                if (isfunction(exp->v.sp->tp))
                {
                    *buf++ = 'e';
                    *buf++ = '?';
                    strcpy(buf, exp->v.sp->name);
                    buf += strlen(buf);
                    *buf++ = '$';
                    buf = mangleType(buf, exp->v.sp->tp, true);
                }
                else
                {
                    *buf++ = 'g';
                    if (!nonpointer)
                        *buf++ = '?';
                    strcpy(buf, exp->v.sp->name);
                    *buf++ = '$';
                    *buf = 0;
                }
                break;
            case en_sizeofellipse:
                *buf++ = 'z';
                buf = getName(buf, exp->v.templateParam->first);
                buf += strlen(buf);
                break;
            case en_void:
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
    if (params && params->front().second->type == kw_new &&
        ((sym->sb->instantiated && !sym->sb->templateLevel) || (params && params->front().second->bySpecialization.types)))
    {
        params = params->front().second->bySpecialization.types;
        bySpecial = true;
    }
    if (sym->tp->type == bt_templateparam && sym->tp->templateParam->second->type == kw_template)
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
        *buf++ = '$';
        *buf = 0;
    }
    else
    {
        *buf++ = '#';
        strcpy(buf, sym->name);
        strcat(buf, "$");
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
            case kw_typename:
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
                    if (it->second->byClass.dflt)
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
            case kw_template:
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
            case kw_int:
                if (it->second->packed)
                {
                    *buf++ = 'e';
                    if (it->second->byPack.pack)
                    {
                        tps.push(it);
                        tps.push(ite);
                        it = it->second->byPack.pack->begin();
                        ite = it->second->byPack.pack->end();
                        continue;
                    }
                }
                else
                {
                    buf = mangleType(buf, it->second->byNonType.tp, true);
                    if (bySpecial || sym->sb->instantiated)
                    {
                        EXPRESSION* exp = bySpecial ? it->second->byNonType.dflt : it->second->byNonType.val;
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
char* mangleType(char* in, TYPE* tp, bool first)
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
        while (tp->type == bt_typedef)
            tp = tp->btp;
        if (isstructured(tp) && basetype(tp)->sp->sb && basetype(tp)->sp->sb->templateLevel)
        {
            {
                if (isconst(tp))
                    *in++ = 'x';
                if (isvolatile(tp))
                    *in++ = 'y';
                if (islrqual(tp))
                    *in++ = 'r';
                if (isrrqual(tp))
                    *in++ = 'R';
            }
            in = mangleTemplate(in, basetype(tp)->sp, basetype(tp)->sp->templateParams);
            return in;
        }
        else
        {
            if (isconst(tp))
                *in++ = 'x';
            if (isvolatile(tp))
                *in++ = 'y';
            if (islrqual(tp))
                *in++ = 'r';
            if (isrrqual(tp))
                *in++ = 'R';
            tp = basetype(tp);
            if (isint(tp) && tp->btp && tp->btp->type == bt_enum)
                tp = tp->btp;
            switch (tp->type)
            {
                case bt_func:
                case bt_ifunc:
                    if (basetype(tp)->sp && ismember(basetype(tp)->sp) && !first)
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
                    *in++ = '$';
                    // return value comes next
                    break;
                case bt_memberptr:
                    *in++ = 'M';
                    in = getName(in, tp->sp);
                    if (isfunction(tp->btp))
                    {
                        *in++ = 'q';
                        for (auto sym : *basetype(tp->btp)->syms)
                        {
                            if (!sym->sb->thisPtr)
                                in = mangleType(in, sym->tp, true);
                        }
                        *in++ = '$';
                        tp = tp->btp;  // so we can get to tp->btp->btp
                    }
                    else
                    {
                        *in++ = '$';
                    }
                    break;
                case bt_enum:
                case bt_struct:
                case bt_union:
                case bt_class:
                    in = getName(in, tp->sp);
                    return in;
                case bt_bool:
                    in = lookupName(in, "bool");
                    while (*in)
                        in++;
                    break;
                case bt___string:
                    in = lookupName(in, "__string");
                    while (*in)
                        in++;
                    break;
                case bt___object:
                    in = lookupName(in, "__object");
                    while (*in)
                        in++;
                    break;
                case bt_unsigned_short:
                    *in++ = 'u';
                case bt_short:
                    *in++ = 's';
                    break;
                case bt_unsigned:
                    *in++ = 'u';
                case bt_int:
                    *in++ = 'i';
                    break;
                case bt_unative:
                    *in++ = 'u';
                case bt_inative:
                    *in++ = 'N';
                    break;
                case bt_char16_t:
                    *in++ = 'h';
                    break;
                case bt_char32_t:
                    *in++ = 'H';
                    break;
                case bt_unsigned_long:
                    *in++ = 'u';
                case bt_long:
                    *in++ = 'l';
                    break;
                case bt_unsigned_long_long:
                    *in++ = 'u';
                case bt_long_long:
                    *in++ = 'L';
                    break;
                case bt_unsigned_char:
                    *in++ = 'u';
                case bt_char:
                    *in++ = 'c';
                    break;
                case bt_signed_char:
                    *in++ = 'S';
                    *in++ = 'c';
                    break;
                case bt_wchar_t:
                    *in++ = 'C';
                    break;
                case bt_float_complex:
                    *in++ = 'F';
                    break;
                case bt_double_complex:
                    *in++ = 'D';
                    break;
                case bt_long_double_complex:
                    *in++ = 'G';
                    break;
                case bt_float:
                    *in++ = 'f';
                    break;
                case bt_double:
                    *in++ = 'd';
                    break;
                case bt_long_double:
                    *in++ = 'g';
                    break;
                case bt_pointer:
                    if (tp->nullptrType)
                    {
                        in = lookupName(in, "nullptr_t");
                        while (*in)
                            in++;
                        return in;
                    }
                    else
                    {
                        if (!tp->array || first && !inKeyCreation)
                        {
                            *in++ = 'p';
                        }
                        else
                        {
                            Optimizer::my_sprintf(in, "A%ld", tp->btp->size ? tp->size / tp->btp->size : 0);
                            while (*in)
                                in++;
                        }
                    }
                    break;
                case bt_far:
                    *in++ = 'P';
                    break;
                case bt_lref:
                    *in++ = 'r';
                    break;
                case bt_rref:
                    *in++ = 'R';
                    break;
                case bt_ellipse:
                    *in++ = 'e';
                    break;
                case bt_void:
                case bt_any:
                    *in++ = 'v';
                    break;
                case bt_templateparam:
                    if (inKeyCreation && tp->templateParam->second->type == kw_typename && tp->templateParam->second->byClass.val &&
                        basetype(tp->templateParam->second->byClass.val)->type != bt_templateparam)
                        in = mangleType(in, tp->templateParam->second->byClass.val, false);
                    else
                        in = getName(in, tp->templateParam->first);
                    break;
                case bt_templateselector: {
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
                        ++s;
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
                case bt_templatedecltype:
                    *in++ = 'E';
                    in = mangleExpression(in, tp->templateDeclType);
                    break;
                case bt_aggregate:
                    in = getName(in, tp->sp);
                    break;
                case bt_auto:
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
static bool validType(TYPE* tp, bool byVal)
{
    tp = basetype(tp);
    switch (tp->type)
    {
    case bt_templateselector:
        return byVal;
    case bt_templateparam:
    case bt_any:
    case bt_aggregate:
        return false;
    case bt_pointer:
        if (tp->array)
        {
            if (tp->size == 0)
                return false;

        }
        if (tp->vla)
            return false;
    case bt_lref:
    case bt_rref:
    case bt_memberptr:
        return validType(tp->btp, byVal);
    case bt_func:
    case bt_ifunc:
        if (!validType(tp->btp, byVal))
            return false;
        for (auto sp : *tp->syms)
            if (!validType(sp->tp, byVal))
                return false;
        break;
    case bt_struct:
    case bt_class:
    case bt_union:
        if (tp->sp->templateParams)
        {
            for (auto tpl : *tp->sp->templateParams)
            {
                if (tpl.second->type == kw_typename)
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
    if (!params)
        result = "v";
    else
        result = "";
    if (params)
    {
        for (auto&& param : *params)
        {
            if (param.second->type != kw_new)
            {
                char buf[8000];
                void* dflt;
                if (param.second->packed)
                {
                    if (!param.second->byPack.pack)
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
                            case kw_typename:
                                if (!validType((TYPE*)dflt, byVal))
                                    return false;
                                result += 'c';
                                *(mangleType(buf, (TYPE*)dflt, true)) = 0;
                                break;
                            case kw_int:
                                result += 'i';
                                *mangleExpression(buf, (EXPRESSION*)dflt) = 0;
                                break;
                            case kw_template:
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
                        case kw_typename:
                            if (!validType((TYPE*)dflt, byVal))
                                return false;
                            result += 'c';
                            *(mangleType(buf, (TYPE*)dflt, true)) = 0;
                            break;
                        case kw_int:
                            if (((EXPRESSION*)dflt)->type == en_templateparam)
                                return false;
                            result += 'i';
                            *mangleExpression(buf, (EXPRESSION*)dflt) = 0;
                            break;
                        case kw_template:
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
    inKeyCreation = true;
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
    inKeyCreation = false;
}
void SetLinkerNames(SYMBOL* sym, enum e_lk linkage, bool isTemplateDefinition)
{
    char errbuf[8192], *p = errbuf;
    memset(errbuf, 0, 8192);
    SYMBOL* lastParent;
    mangledNamesCount = 0;
    if (Optimizer::cparams.prm_cplusplus && !sym->sb->parentClass && !sym->sb->parentNameSpace && sym->name[0] == 'm' &&
        !strcmp(sym->name, "main"))
        linkage = lk_c;
    if (linkage == lk_none || linkage == lk_cdecl)
    {
        if (Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL))
        {
            if (sym->sb->storage_class != sc_label && sym->sb->storage_class != sc_parameter &&
                sym->sb->storage_class != sc_namespace && sym->sb->storage_class != sc_namespacealias &&
                sym->sb->storage_class != sc_ulabel &&
                (isfunction(sym->tp) || istype(sym) || sym->sb->parentNameSpace || sym->sb->parentClass || sym->sb->templateLevel))
                linkage = lk_cpp;
            else
                linkage = lk_c;
        }
        else
        {
            linkage = lk_c;
        }
    }
    else if (linkage == lk_stdcall)
    {
        if (sym->sb->parentClass)
            linkage = lk_cpp;
    }
    if (linkage == lk_auto && !Optimizer::cparams.prm_cplusplus)
        linkage = lk_c;
    if (linkage == lk_c && !Optimizer::cparams.prm_cmangle)
        linkage = lk_stdcall;
    if (sym->sb->attribs.inheritable.linkage4 == lk_virtual || linkage == lk_fastcall)
    {
        if (Optimizer::cparams.prm_cplusplus)
            linkage = lk_cpp;
        else
            linkage = lk_c;
    }
    switch (linkage)
    {
        case lk_auto:
            if (sym->sb->parent)
                if (sym->sb->uniqueID == 0)
                    sym->sb->uniqueID = uniqueID++;
            p = mangleClasses(p, theCurrentFunc);
            Optimizer::my_sprintf(p, "@%s", sym->name);

            break;
        case lk_pascal:
            strcpy(errbuf, sym->name);
            while (*p)
            {
                *p = toupper(*p);
                p++;
            }
            break;
        case lk_stdcall:
            strcpy(errbuf, sym->name);
            break;
        case lk_c:
        default:
            if (sym->sb->parent)
                if (sym->sb->uniqueID == 0)
                    sym->sb->uniqueID = uniqueID++;
            if (sym->sb->storage_class == sc_localstatic && sym->sb->parent)
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
        case lk_cpp:
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
            if (isfunction(sym->tp))
            {
                *p++ = '$';
                if (sym->sb->castoperator)
                {
                    int tmplCount = 0;
                    *p++ = 'o';
                    p = mangleType(p, basetype(sym->tp)->btp, true);  // cast operators get their cast type in the name
                    *p++ = '$';
                    p = mangleType(p, sym->tp, true);  // add the $qv
                    while (p > errbuf && (*--p != '$' || tmplCount))
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
                        while (p > errbuf && (*--p != '$' || tmplCount))
                            if (*p == '~')
                                tmplCount++;
                            else if (*p == '#')
                                tmplCount--;
                        if (basetype(sym->tp)->btp->type == bt_memberptr)
                        {
                            while (p > errbuf && (*--p != '$' || tmplCount))
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