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
#include <cstdarg>
#include "PreProcessor.h"
#include "Utils.h"
#include "Errors.h"
#include "ccerr.h"
#include "config.h"
#include "declare.h"
#include "lex.h"
#include "template.h"
#include "occparse.h"
#include "stmt.h"
#include "unmangle.h"
#include "OptUtils.h"
#include "memory.h"
#include "mangle.h"
#include "lex.h"
#include "types.h"
#include "help.h"
#include "libcxx.h"
#include "cpplookup.h"
#include "declcons.h"
#include "FNV_hash.h"
#include <cstdio>
#include "symtab.h"

namespace Parser
{

int currentErrorLine;
SYMBOL* theCurrentFunc;

enum e_kw skim_end[] = {end, kw_none};
enum e_kw skim_closepa[] = {closepa, semicolon, end, kw_none};
enum e_kw skim_semi[] = {semicolon, end, kw_none};
enum e_kw skim_semi_declare[] = {semicolon, kw_none};
enum e_kw skim_closebr[] = {closebr, semicolon, end, kw_none};
enum e_kw skim_comma[] = {comma, closepa, closebr, semicolon, end, kw_none};
enum e_kw skim_colon[] = {colon, kw_case, kw_default, semicolon, end, kw_none};
enum e_kw skim_templateend[] = {gt, semicolon, end, kw_none};
std::deque<std::tuple<const char*, int, SYMBOL*>> instantiationList;

static Optimizer::LIST* listErrors;
static const char* currentErrorFile;
static bool disabledNote;

ErrorNamesAndLevels errors[] = {
#define ERRLIST(x, y, z, a) {z, a},
#define ERRWITHWARNFLAG(x, y, z, a, b) {z, a},
#include "errorlist.h"

};
namespace Util = OrangeC::Utils;
// Maps GCC warning names to OCC warning numbers using the FNV algorithm because the string constructor is a strlen call which is
// expensive
static const std::unordered_map<const char*, int, Util::fnv1a64, Util::str_eql> error_name_map{
#define ERRWITHWARNFLAG(x, y, z, a, b) {b, y},
#define ERRWTIHWARNFLAGHELP(x, y, z, a, b, c) {b, y},
#include "errorlist.h"
};
// The difference between the following two maps is that the one immediately below uses the internal error number while the other
// uses the GCC name
static const std::unordered_map<int, std::string> error_help_map{
#define ERRWITHWARNFLAGHELP(x, y, z, a, b, c) {y, c},
#define ERRWITHHELP(x, y, z, a, b) {y, b},
#include "errorlist.h"
};
static const std::unordered_map<const char*, std::string, Util::fnv1a64, Util::str_eql> error_name_help_map{
#define ERRWITHWARNFLAGHELP(x, y, z, a, b, c) {b, c},
#include "errorlist.h"
};
void DumpErrorNameMap()
{
    printf("Error name to number map:\n");
    for (auto a : error_name_map)
    {
        printf("%s: %d\n", a.first, a.second);
    }
    printf("Name to number map end.\n");
}
void DumpErrorNumToHelpMap()
{
    printf("Error number to help map:\n");
    for (auto a : error_help_map)
    {
        printf("%d: %s\n", a.first, a.second.c_str());
    }
    printf("Number to help map end.\n");
}
void DumpErrorNameToHelpMap()
{
    printf("Error name to help map:\n");
    for (auto a : error_name_help_map)
    {
        printf("%s: %s\n", a.first, a.second.c_str());
    }
    printf("Name to help map end.\n");
}

void EnterInstantiation(LEXLIST* lex, SYMBOL* sym)
{
    if (lex)
    {
        instantiationList.push_front(std::tuple<const char*, int, SYMBOL*>(lex->data->errfile, lex->data->errline, sym));
    }
    else
    {
        instantiationList.push_front(std::tuple<const char*, int, SYMBOL*>(sym->sb->declfile, sym->sb->declline, sym));
    }
}
void LeaveInstantiation() { instantiationList.pop_front(); }
static void DumpInstantiations()
{
    for (auto i : instantiationList)
    {
        errorsym(ERR_REFERENCED_IN_INSTANTIATION, std::get<2>(i), std::get<1>(i), std::get<0>(i));
    }
    if (!instantiationList.empty() && preProcessor->GetErrLineNo())
    {
        printerr(ERR_TEMPLATE_INSTANTIATION_STARTED_IN, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo());
    }
}
static bool ValidateWarning(int num)
{
    if (num && num < sizeof(errors) / sizeof(errors[0]))
    {
        if (!(errors[num].level & CE_ERROR))
        {
            return true;
        }
    }
    printf("Warning: /w index %d does not correspond to a warning\n", num);
    return false;
}
static bool ValidateWarning(const char* str)
{
    auto thing = error_name_map.find(str);
    if (thing != error_name_map.end())
    {
        auto val = thing->second;
        if (!(errors[val].level & CE_ERROR))
        {
            return true;
        }
        printf("Warning: Name %s does not correspond to a warning\n", str);
        return false;
    }
    printf("Warning: %s does not pertain to any warning or error\n", str);
    return false;
}
void DisableWarning(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->SetFlag(num, Warning::Disable);
    }
}
void WarningAsError(const char* str)
{
    if (ValidateWarning(str))
    {
        int val = error_name_map.find(str)->second;
        Warning::Instance()->SetFlag(val, Warning::AsError);
    }
}
void EnableWarning(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->ClearFlag(num, Warning::Disable);
    }
}
void WarningOnlyOnce(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->SetFlag(num, Warning::OnlyOnce);
    }
}
void WarningAsError(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->SetFlag(num, Warning::AsError);
    }
}
void AllWarningsAsError()
{
    Errors::WarningsAsErrors(true);
    Errors::SetShowWarnings(true);
    for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
        Warning::Instance()->SetFlag(i, Warning::AsError);
}
void AllWarningsDisable()
{
    Errors::WarningsAsErrors(false);
    Errors::SetShowWarnings(false);
    for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
        Warning::Instance()->SetFlag(i, Warning::Disable);
}
void PushWarnings() { Warning::Instance()->Push(); }
void PopWarnings() { Warning::Instance()->Pop(); }
void DisableTrivialWarnings()
{
    Warning::Instance()->Clear();
    if (!Optimizer::cparams.prm_warning)
        for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
            Warning::Instance()->SetFlag(i, Warning::Disable);
    if (!Optimizer::cparams.prm_extwarning)
        for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
            if (errors[i].level & CE_TRIVIALWARNING)
                Warning::Instance()->SetFlag(i, Warning::Disable);
}
static int total_errors;
int diagcount;

int TotalErrors() { return total_errors + Errors::GetErrorCount(); }
void errorinit(void)
{
    total_errors = diagcount = 0;
    currentErrorFile = nullptr;
    currentLex = nullptr;
    disabledNote = false;
    instantiationList.clear();
}

static char kwtosym(enum e_kw kw)
{
    switch (kw)
    {
        case openpa:
            return '(';
        case closepa:
            return ')';
        case closebr:
            return ']';
        case semicolon:
            return ';';
        case begin:
            return '{';
        case end:
            return '}';
        case assign:
            return '=';
        case colon:
            return ':';
        case lt:
            return '<';
        case gt:
            return '>';
        default:
            return '?';
    }
};
static bool IsReturnErr(int err)
{
    switch (err)
    {
        case ERR_CALL_FUNCTION_NO_PROTO:
        case ERR_RETURN_MUST_RETURN_VALUE:
        case ERR_RETURN_NO_VALUE:
        case ERR_RETMISMATCH:
        case ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE:
            return Optimizer::chosenAssembler->arch->erropts & EO_RETURNASERR;
        default:
            return false;
    }
}
static bool alwaysErr(int err)
{
    switch (err)
    {
        case ERR_TOO_MANY_ERRORS:
        case ERR_UNDEFINED_IDENTIFIER:
        case ERR_ABSTRACT_BECAUSE:
        case ERR_REF_MUST_INITIALIZE:
        case ERR_CONSTANT_MUST_BE_INITIALIZED:
        case ERR_REF_MEMBER_MUST_INITIALIZE:
        case ERR_CONSTANT_MEMBER_MUST_BE_INITIALIZED:
        case ERR_CANNOT_ACCESS:
        case ERR_REF_CLASS_NO_CONSTRUCTORS:
        case ERR_CONST_CLASS_NO_CONSTRUCTORS:
        case ERR_CONSTEXPR_MUST_INITIALIZE:
        case ERR_PREVIOUS:
            return true;
        default:
            return false;
    }
}
static bool ignoreErrtemplateNestingCount(int err)
{
    switch (err)
    {
        case ERR_NEED_CONSTANT_OR_ADDRESS:
        case ERR_NEED_INTEGER_TYPE:
        case ERR_NEED_INTEGER_EXPRESSION:
        case ERR_MISMATCHED_STRUCTURED_TYPE_IN_REDEFINITION:
        case ERR_STRUCT_MAY_NOT_CONTAIN_INCOMPLETE_STRUCT:
        case ERR_ORIGINAL_TYPE_NOT_ENUMERATION:
        case ERR_BIT_FIELD_INTEGER_TYPE:
        case ERR_MEMBER_NAME_EXPECTED:
        case ERR_POINTER_TO_STRUCTURE_EXPECTED:
        case ERR_STRUCTURED_TYPE_EXPECTED:
        case ERR_INCOMPATIBLE_TYPE_CONVERSION:
        case ERR_TYPE_MISMATCH_IN_REDECLARATION:
        case ERR_TWO_OPERANDS_SAME_TYPE:
        case ERR_ARRAY_INDEX_INTEGER_TYPE:
        case ERR_TYPE_MISMATCH_IN_ARGUMENT:
        case ERR_NOT_AN_ALLOWED_TYPE:
        case ERR_SWITCH_SELECTION_INTEGRAL:
        case ERR_RETMISMATCH:
        case ERR_CANNOT_CONVERT_TYPE:
        case ERR_CANNOT_CAST_TYPE:
        case ERR_NO_OVERLOAD_MATCH_FOUND:
        case ERR_POINTER_TYPE_EXPECTED:
        case ERR_NEED_TYPEINFO_H:
        case ERR_EXPRESSION_HAS_NO_EFFECT:
        case ERR_FUNCTION_SHOULD_RETURN_VALUE:
        case ERR_AMBIGUITY_BETWEEN:
        case ERR_NO_DEFAULT_CONSTRUCTOR:
        case ERR_NO_APPROPRIATE_CONSTRUCTOR:
        case ERR_ILL_STRUCTURE_ASSIGNMENT:
        case ERR_ILL_STRUCTURE_OPERATION:
            return true;
    }
    return false;
}
bool IsNothrowError(int err)
{
    switch (err)
    {
        case ERR_NO_OVERLOAD_MATCH_FOUND:
        case ERR_NO_DEFAULT_CONSTRUCTOR:
        case ERR_NO_APPROPRIATE_CONSTRUCTOR:
        case ERR_NAME_IS_NOT_A_MEMBER_OF_NAME:
        case ERR_CALL_OF_NONFUNCTION:
        case ERR_CANNOT_CONVERT_TYPE:
        case ERR_ILL_STRUCTURE_ASSIGNMENT:
        case ERR_ILL_STRUCTURE_OPERATION:
        case ERR_DELETED_FUNCTION_REFERENCED:
            return true;
        default:
            return false;
    }
}
bool printerrinternal(int err, const char* file, int line, va_list args)
{
    if (inNothrowHandler && IsNothrowError(err))
    {
        noExcept = false;
        return false;
    }
    if (!errors[err].level)
        return false;
    if (!file)
    {
        if (currentLex)
        {
            file = currentLex->data->errfile;
            line = currentLex->data->errline;
        }
        else
        {
            file = preProcessor->GetErrFile().c_str();
            line = preProcessor->GetErrLineNo();
        }
    }
    char buf[10000];
    char infunc[10000];
    const char* listerr;
    char nameb[265], *name = nameb;
    if (Optimizer::cparams.prm_makestubs || inDeduceArgs || (templateNestingCount && ignoreErrtemplateNestingCount(err)))
        if (err != ERR_STATIC_ASSERT && err != ERR_DELETED_FUNCTION_REFERENCED && !(errors[err].level & CE_NOTE))
        {
            return false;
        }
    if (!file)
    {
        if (context && context->last->data->type != l_none)
        {
            LEXLIST* lex = context->cur ? context->cur->prev : context->last;
            line = lex->data->errline;
            file = lex->data->errfile;
        }
        else
        {
            file = "unknown";
        }
    }
    strcpy(nameb, file);
    if (strchr(infile, '\\') != 0 || strchr(infile, ':') != 0)
    {
        name = Utils::FullQualify(nameb);
    }
    if (TotalErrors() > Optimizer::cparams.prm_maxerr)
        return false;
    if (!(errors[err].level & CE_NOTE) && !alwaysErr(err) && currentErrorFile &&
        !strcmp(currentErrorFile, preProcessor->GetRealFile().c_str()) && preProcessor->GetRealLineNo() == currentErrorLine)
    {
        disabledNote = true;
        return false;
    }
    if (err >= sizeof(errors) / sizeof(errors[0]))
    {
        Optimizer::my_sprintf(buf, "Error %d", err);
    }
    else
    {
        vsprintf(buf, errors[err].name, args);
    }
    if (errors[err].level & CE_NOTE)
    {
        if (!disabledNote)
        {
            if (!Optimizer::cparams.prm_quiet)
                printf("note:        ");
            if (Optimizer::cparams.prm_errfile)
                fprintf(errFile, "note:   ");
        }
    }
    else if (IsReturnErr(err) || (errors[err].level & CE_ERROR) ||
             (Optimizer::cparams.prm_ansi && (errors[err].level & CE_ANSIERROR)) ||
             (Optimizer::cparams.prm_cplusplus && (errors[err].level & CE_CPLUSPLUSERROR)))
    {
        if (!Optimizer::cparams.prm_quiet)
            printf("Error(%3d)   ", err);
        if (Optimizer::cparams.prm_errfile)
            fprintf(errFile, "Error   ");
        listerr = "CE_ERROR";
        total_errors++;
        currentErrorFile = preProcessor->GetRealFile().c_str();
        currentErrorLine = preProcessor->GetRealLineNo();
        disabledNote = false;
    }
    else
    {
        disabledNote = true;    
        if (Warning::Instance()->IsSet(err, Warning::Disable))
            return false;
        if (Warning::Instance()->IsSet(err, Warning::OnlyOnce))
            if (Warning::Instance()->IsSet(err, Warning::Emitted))
                return false;
        disabledNote = false;
        Warning::Instance()->SetFlag(err, Warning::Emitted);
        if (Warning::Instance()->IsSet(err, Warning::AsError))
        {
            if (!Optimizer::cparams.prm_quiet)
                printf("Error(%3d)   ", err);
            if (Optimizer::cparams.prm_errfile)
                fprintf(errFile, "Error   ");
            listerr = "CE_ERROR";
            total_errors++;
            currentErrorFile = preProcessor->GetRealFile().c_str();
            currentErrorLine = preProcessor->GetRealLineNo();
        }

        else
        {
            if (!Optimizer::cparams.prm_quiet)
                printf("Warning(%3d) ", err);
            if (Optimizer::cparams.prm_errfile)
                fprintf(errFile, "Warning ");
            listerr = "CE_WARNING";
        }
    }
    infunc[0] = 0;
    if (!Optimizer::cparams.prm_quiet)
        printf(" %s(%d):  %s%s\n", name, line, buf, infunc);
    if (Optimizer::cparams.prm_errfile)
        fprintf(errFile, " %s(%d):  %s%s\n", name, line, buf, infunc);
    if (TotalErrors() >= Optimizer::cparams.prm_maxerr)
    {
        error(ERR_TOO_MANY_ERRORS);
        exit(IsCompiler() ? 1 : 0);
    }
    if (!(errors[err].level & CE_NOTE))
    {
        DumpInstantiations();
    }
    return true;
}
int printerr(int err, const char* file, int line, ...)
{
    bool canprint = false;
    va_list arg;
    va_start(arg, line);
    canprint = printerrinternal(err, file, line, arg);
    va_end(arg);
    return canprint;
}
void pperror(int err, int data) { printerr(err, nullptr, 0, data); }
void pperrorstr(int err, const char* str) { printerr(err, nullptr, 0, str); }
void preverror(int err, const char* name, const char* origFile, int origLine)
{
    if (printerr(err, nullptr, 0, name))
        if (origFile && origLine)
            printerr(ERR_PREVIOUS, origFile, origLine, name);
}
void preverrorsym(int err, SYMBOL* sp, const char* origFile, int origLine)
{
    char buf[10000];
    unmangle(buf, sp->sb->decoratedName);
    if (origFile && origLine)
        preverror(err, buf, origFile, origLine);
}
void errorat(int err, const char* name, const char* file, int line) { printerr(err, file, line, name); }
void errorcurrent(int err) { printerr(err, nullptr, 0); }
void getns(char* buf, SYMBOL* nssym)
{
    if (nssym->sb->parentNameSpace)
    {
        getns(buf, nssym->sb->parentNameSpace);
        strcat(buf, "::");
    }
    strcat(buf, nssym->name);
}
void getcls(char* buf, SYMBOL* clssym)
{
    if (clssym->sb->parentClass)
    {
        getcls(buf, clssym->sb->parentClass);
        strcat(buf, "::");
    }
    else if (clssym->sb->parentNameSpace)
    {
        getns(buf, clssym->sb->parentNameSpace);
        strcat(buf, "::");
    }
    strcat(buf, clssym->name);
}
void errorqualified(int err, SYMBOL* strSym, NAMESPACEVALUEDATA* nsv, const char* name)
{
    char buf[4096];
    char unopped[10000];
    const char* last = "typename";
    char lastb[10000];
    memset(buf, 0, sizeof(buf));
    if (strSym)
    {
        if (strSym->sb->decoratedName)
            unmangle(lastb, strSym->sb->decoratedName);
        else
            strcpy(lastb, strSym->name);
        last = strrchr(lastb, ':');
        if (last)
            last++;
        else
            last = lastb;
    }
    if (*name == '.')
    {
        *unopped = 0;
        if (name[1] == 'b' || name[1] == 'o')
        {
            unmang_intrins(unopped, name, last);
        }
        else
        {
            unmang1(unopped, name + 1, last, false);
        }
    }
    else
    {
        strcpy(unopped, name);
    }
    Optimizer::my_sprintf(buf, "'%s' is not a member of '", unopped);
    if (strSym)
    {
        typeToString(buf + strlen(buf), strSym->tp);
    }
    else if (nsv)
    {
        getns(buf, nsv->name);
    }
    strcat(buf, "'");
    if (strSym && !strSym->tp->syms)
        strcat(buf, " because the type is not defined");
    printerr(err, nullptr, 0, buf);
}
void errorNotMember(SYMBOL* strSym, NAMESPACEVALUEDATA* nsv, const char* name)
{
    errorqualified(ERR_NAME_IS_NOT_A_MEMBER_OF_NAME, strSym, nsv, name);
}
void error(int err) { printerr(err, nullptr, 0); }
void errorint(int err, int val) { printerr(err, nullptr, 0, val); }
void errorstr(int err, const char* val) { printerr(err, nullptr, 0, (char*)val); }
void errorstr2(int err, const char* val, const char* two) { printerr(err, nullptr, 0, (char*)val, (char*)two); }
void errorsym(int err, SYMBOL* sym)
{
    char buf[5000];
    if (sym->sb)
    {
        if (!sym->sb->decoratedName)
        {
            SetLinkerNames(sym, lk_cdecl);
        }
        unmangle(buf, sym->sb->decoratedName);
    }
    else
    {
        strcpy(buf, sym->name);
    }
    printerr(err, nullptr, 0, buf);
}
void errorsym(int err, SYMBOL* sym, int line, const char* file)
{
    char buf[10000];
    if (!sym->sb->decoratedName)
    {
        SetLinkerNames(sym, lk_cdecl);
    }
    unmangle(buf, sym->sb->decoratedName);
    printerr(err, file, line, buf);
}
void errorsym2(int err, SYMBOL* sym1, SYMBOL* sym2)
{
    char one[10000], two[10000];
    unmangle(one, sym1->sb->decoratedName);
    unmangle(two, sym2->sb->decoratedName);
    printerr(err, nullptr, 0, one, two);
}
void errorstrsym(int err, const char* name, SYMBOL* sym2)
{
    char two[10000];
    unmangle(two, sym2->sb->decoratedName);
    printerr(err, nullptr, 0, name, two);
}
void errorstringtype(int err, char* str, TYPE* tp1)
{
    char tpb1[4096];
    memset(tpb1, 0, sizeof(tpb1));
    typeToString(tpb1, tp1);
    printerr(err, nullptr, 0, str, tpb1);
}

void errortype(int err, TYPE* tp1, TYPE* tp2)
{
    char tpb1[4096], tpb2[4096];
    memset(tpb1, 0, sizeof(tpb1));
    memset(tpb2, 0, sizeof(tpb2));
    typeToString(tpb1, tp1);
    if (tp2)
        typeToString(tpb2, tp2);
    printerr(err, nullptr, 0, tpb1, tpb2);
}
void errorabstract(int error, SYMBOL* sp)
{
    SYMBOL* sp1;
    errorsym(error, sp);
    sp1 = calculateStructAbstractness(sp, sp);
    if (sp)
    {
        errorsym2(ERR_ABSTRACT_BECAUSE, sp, sp1);
    }
}
void errorarg(int err, int argnum, SYMBOL* declsp, SYMBOL* funcsp)
{
    char argbuf[10000];
    char buf[10000];
    if (declsp->sb->anonymous)
        Optimizer::my_sprintf(argbuf, "%d", argnum);
    else if (declsp->sb->decoratedName)
    {
        unmangle(argbuf, declsp->sb->decoratedName);
    }
    else
    {
        strcpy(argbuf, declsp->name);
    }
    unmangle(buf, funcsp->sb->decoratedName);
    currentErrorLine = 0;
    printerr(err, nullptr, 0, argbuf, buf);
}
static BALANCE* newbalance(LEXLIST* lex, BALANCE* bal)
{
    BALANCE* rv = Allocate<BALANCE>();
    rv->back = bal;
    rv->count = 0;
    if (KW(lex) == openpa)
        rv->type = BAL_PAREN;
    else if (KW(lex) == openbr)
        rv->type = BAL_BRACKET;
    else if (KW(lex) == lt)
        rv->type = BAL_LT;
    else
        rv->type = BAL_BEGIN;
    return (rv);
}
static void setbalance(LEXLIST* lex, BALANCE** bal, bool assumeTemplate)
{
    switch (KW(lex))
    {
        case end:
            while (*bal && (*bal)->type != BAL_BEGIN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case closepa:
            while (*bal && (*bal)->type != BAL_PAREN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case closebr:
            while (*bal && (*bal)->type != BAL_BRACKET)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case gt:
            if (assumeTemplate)
            {
                while (*bal && (*bal)->type != BAL_LT)
                {
                    (*bal) = (*bal)->back;
                }
                if (*bal && !(--(*bal)->count))
                    (*bal) = (*bal)->back;
                break;
            }
        case begin:
            if (!*bal || (*bal)->type != BAL_BEGIN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
        case openpa:
            if (!*bal || (*bal)->type != BAL_PAREN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;

        case openbr:
            if (!*bal || (*bal)->type != BAL_BRACKET)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
        case lt:
            if (assumeTemplate)
            {
                if (!*bal || (*bal)->type != BAL_LT)
                    *bal = newbalance(lex, *bal);
                (*bal)->count++;
            }
            break;
        default:
            break;
    }
}

/*-------------------------------------------------------------------------*/

void errskim(LEXLIST** lex, enum e_kw* skimlist, bool assumeTemplate)
{
    BALANCE* bal = 0;
    while (true)
    {
        if (!*lex)
            break;
        if (!bal)
        {
            int i;
            enum e_kw kw = KW(*lex);
            for (i = 0; skimlist[i] != kw_none; i++)
                if (kw == skimlist[i])
                    return;
        }
        setbalance(*lex, &bal, assumeTemplate);
        *lex = getsym();
    }
}
void skip(LEXLIST** lex, enum e_kw kw)
{
    if (MATCHKW(*lex, kw))
        *lex = getsym();
}
bool needkw(LEXLIST** lex, enum e_kw kw)
{
    if (lex && MATCHKW(*lex, kw))
    {
        *lex = getsym();
        return true;
    }
    else
    {
        errorint(ERR_NEEDY, kwtosym(kw));
        return false;
    }
}
void specerror(int err, const char* name, const char* file, int line) { printerr(err, file, line, name); }

static bool hasGoto(std::list<STATEMENT*>* statements)
{
    if (!statements)
        return false;
    for (auto stmt : *statements)
    {
        switch (stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                if (hasGoto(stmt->lower))
                    return true;
                break;
            case st_declare:
            case st_expr:
                break;
            case st_goto:
                return true;
            case st_return:
            case st_select:
            case st_notselect:
            case st_label:
            case st_line:
            case st_passthrough:
            case st_datapassthrough:
            case st_asmcond:
            case st_varstart:
            case st_dbgblock:
                break;
            case st_nop:
                break;
            default:
                diag("unknown stmt type in hasgoto");
                break;
        }
    }
    return false;
}
static bool hasDeclarations(std::list<STATEMENT*>* statements)
{
    if (!statements)
        return false;
    for (auto stmt : *statements)
    {
        switch (stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                if (hasDeclarations(stmt->lower))
                    return true;
                break;
            case st_declare:
            case st_expr:
                if (stmt->hasvla || stmt->hasdeclare)
                    return true;
                break;
            case st_return:
            case st_goto:
            case st_select:
            case st_notselect:
            case st_label:
            case st_line:
            case st_passthrough:
            case st_datapassthrough:
            case st_asmcond:
            case st_varstart:
            case st_dbgblock:
                break;
            case st_nop:
                break;
            default:
                diag("unknown stmt type in hasDeclarations");
                break;
        }
    }
    return false;
}
static void labelIndexes(std::list<STATEMENT*>* statements, int* min, int* max)
{
    if (!statements)
        return;
    for (auto stmt : *statements)
    {
        switch (stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                labelIndexes(stmt->lower, min, max);
                break;
            case st_declare:
            case st_expr:
                break;
            case st_goto:
                if (stmt->indirectGoto)
                    break;
            case st_select:
            case st_notselect:
            case st_label:
                if (stmt->label < *min)
                    *min = stmt->label;
                if (stmt->label > *max)
                    *max = stmt->label;
                break;
            case st_return:
            case st_line:
            case st_passthrough:
            case st_datapassthrough:
            case st_asmcond:
            case st_varstart:
            case st_dbgblock:
                break;
            case st_nop:
                break;
            default:
                diag("unknown stmt type in hasDeclarations");
                break;
        }
    }
}
static VLASHIM* mkshim(_vlaTypes type, int level, int label, STATEMENT* stmt, VLASHIM* last, VLASHIM* parent, int blocknum,
                       int blockindex)
{
    VLASHIM* rv = Allocate<VLASHIM>();
    if (last && last->type != v_return && last->type != v_goto)
    {
        rv->backs.push_front(last);
    }
    rv->type = type;
    rv->level = level;
    rv->label = label;
    rv->line = stmt->line;
    rv->file = stmt->file;
    rv->stmt = stmt;
    rv->blocknum = blocknum;
    rv->blockindex = blockindex;
    rv->parent = parent;
    return rv;
}
/* thisll be sluggish if there are lots of gotos & labels... */
static std::list<VLASHIM*> getVLAList(std::list<STATEMENT*>* statements, VLASHIM* last, VLASHIM* parent, VLASHIM** labels, int minLabel, int* blocknum,
                           int level, bool* branched)
{
    std::list<VLASHIM*> rv;
    if (!statements)
        return rv;
    int curBlockNum = (*blocknum)++;
    int curBlockIndex = 0;
    VLASHIM *nextParent = nullptr;
    for (auto stmt : *statements)
    {
        switch (stmt->type)
        {
            case st_switch: {
                bool first = true;
                for (auto cases : *stmt->cases)
                {
                    rv.push_back(mkshim(v_branch, level, cases->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                    last = rv.back();
                    if (first)
                        nextParent = last;
                    first = false;
                }
            }
                // fallthrough
            case st_block:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                if (stmt->lower && stmt->lower->front()->type == st_goto && !stmt->lower->front()->indirectGoto)
                {
                    // unwrap the goto for purposes of these diagnostics
                    rv.push_back(mkshim(v_goto, level, stmt->lower->front()->label, stmt->lower->front(), last, parent, curBlockNum, curBlockIndex++));
                    last = rv.back();
                    last->checkme = stmt->lower->front()->explicitGoto;
                    *branched = true;
                }
                else
                {
                    rv.push_back(mkshim(v_blockstart, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                    last = rv.back();
                    if (!nextParent)
                        nextParent = last;
                    last->lower = getVLAList(stmt->lower, last, nextParent, labels, minLabel, blocknum, level + 1, branched);
                    if (stmt->blockTail)
                    {
                        rv.push_back(mkshim(v_blockend, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                        last = rv.back();
                    }
                }
                break;
            case st_declare:
            case st_expr:
                if (*branched)
                {
                    if (stmt->hasvla)
                    {
                        rv.push_back(mkshim(v_vla, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                        last = rv.back();
                    }
                    else if (stmt->hasdeclare)
                    {
                        rv.push_back(mkshim(v_declare, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                        last = rv.back();
                    }
                    nextParent = last;
                }
                break;
            case st_return:
                *branched = true;
                rv.push_back(mkshim(v_return, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                last = rv.back();
                break;
            case st_select:
            case st_notselect:
                *branched = true;
                rv.push_back(mkshim(v_branch, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                last = rv.back();
                nextParent = last;
                break;
            case st_line:
            case st_passthrough:
            case st_datapassthrough:
            case st_asmcond:
            case st_varstart:
            case st_dbgblock:
                break;
            case st_nop:
                break;
            case st_goto:
                if (!stmt->indirectGoto)
                {
                    *branched = true;
                    rv.push_back(mkshim(v_goto, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                    last = rv.back();
                    last->checkme = stmt->explicitGoto;
                }
                break;
            case st_label:
                rv.push_back(mkshim(v_label, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                last = rv.back();
                labels[stmt->label - minLabel] = last;
                break;
            default:
                diag("unknown stmt type in checkvla");
                break;
        }
    }
    return rv;
}
static void fillPrevious(std::list<VLASHIM*>& vlashims, VLASHIM** labels, int minLabel)
{
    for (auto shim : vlashims)
    {
        VLASHIM* selected;
        Optimizer::LIST* prev;
        switch (shim->type)
        {
            case v_blockstart:
                fillPrevious(shim->lower, labels, minLabel);
                break;
            case v_declare:
            case v_vla:
            case v_return:
            case v_label:
            case v_blockend:
                break;
            case v_goto:
            case v_branch:
                selected = labels[shim->label - minLabel];
                if (selected)
                {
                    selected->backs.push_front(shim);
                    shim->fwd = selected;
                }
                break;
            default:
                diag("unknown shim type in fillPrevious");
                break;
        }
    }
}
static void vlaError(VLASHIM* gotoShim, VLASHIM* errShim)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "%d", gotoShim->line);
    currentErrorLine = 0;
    specerror(ERR_GOTO_BYPASSES_VLA_INITIALIZATION, buf, errShim->file, errShim->line);
}
static void declError(VLASHIM* gotoShim, VLASHIM* errShim)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "%d", gotoShim->line);
    currentErrorLine = 0;
    specerror(ERR_GOTO_BYPASSES_INITIALIZATION, buf, errShim->file, errShim->line);
}
static bool scanGoto(VLASHIM* shim, VLASHIM* gotoshim, VLASHIM* matchshim, int* currentLevel)
{
    if (shim == matchshim || shim->level < matchshim->level)
        return true;
    if (shim && !shim->mark && shim != gotoshim)
    {
        shim->mark = true;
        if (shim->level <= *currentLevel)
        {
            *currentLevel = shim->level;
            if (shim->type == v_declare)
                declError(gotoshim, shim);
            if (shim->type == v_vla)
                vlaError(gotoshim, shim);
        }
        for (auto lst : shim->backs)
            if (lst == matchshim)
                return true;
        for (auto s : shim->backs)
            if (!s->checkme && scanGoto(s, gotoshim, matchshim, currentLevel))
                return true;
    }
    return false;
}
void unmarkGotos(std::list<VLASHIM*>& vlashims)
{
    for (auto shim : vlashims)
    {
        shim->mark = false;
        if (shim->type == v_blockstart)
            unmarkGotos(shim->lower);
    }
}
static void validateGotos(std::list<VLASHIM*>& vlashims, std::list<VLASHIM*>& root)
{
    for (auto shim : vlashims)
    {
        VLASHIM* selected;
        Optimizer::LIST* prev;
        switch (shim->type)
        {
            case v_blockstart:
                validateGotos(shim->lower, root);
                break;
            case v_declare:
            case v_vla:
            case v_return:
            case v_label:
            case v_blockend:
            case v_branch:
                break;
            case v_goto:
                if (shim->checkme && shim->fwd)
                {
                    VLASHIM* sgoto = shim;
                    VLASHIM* fwd = sgoto->fwd;
                    while (sgoto->level > fwd->level)
                        sgoto = sgoto->parent;
                    while (sgoto->level < fwd->level)
                        fwd = fwd->parent;
                    while (sgoto->blocknum != fwd->blocknum)
                    {
                        int n = sgoto->level, m = fwd->level;
                        if (n >= m)
                            sgoto = sgoto->parent;
                        if (n <= m)
                            fwd = fwd->parent;
                    }
                    unmarkGotos(root);
                    int level = shim->fwd->level;
                    if (sgoto->blockindex > fwd->blockindex)
                    {
                        // goto is after label
                        // drill up in the label block until we hit the label
                        scanGoto(shim->fwd, shim, fwd, &level);
                    }
                    else
                    {
                        // goto is before label
                        // drill up in the label block until we hit the goto
                        scanGoto(shim->fwd, shim, sgoto, &level);
                    }
                }
                break;
            default:
                diag("unknown shim type in validateGotos");
                break;
        }
    }
}
void checkGotoPastVLA(std::list<STATEMENT*>*statements, bool first)
{
    if (hasGoto(statements) && hasDeclarations(statements))
    {
        int min = INT_MAX, max = INT_MIN;
        labelIndexes(statements, &min, &max);
        if (min > max)
            return;
        VLASHIM** labels = Allocate<VLASHIM*>(max + 1 - min);

        int blockNum = 0;
        bool branched = false;
        std::list<VLASHIM*> list = getVLAList(statements, nullptr, nullptr, labels, min, &blockNum, 0, &branched);
        fillPrevious(list, labels, min);
        validateGotos(list, list);
    }
}
void checkUnlabeledReferences(std::list<BLOCKDATA*>& block)
{
    int i;
    for (auto sp : *labelSyms)
    {
        if (sp->sb->storage_class == sc_ulabel)
        {
            STATEMENT* st;
            specerror(ERR_UNDEFINED_LABEL, sp->name, sp->sb->declfile, sp->sb->declline);
            sp->sb->storage_class = sc_label;
            st = stmtNode(nullptr, block, st_label);
            st->label = sp->sb->offset;
        }
    }
}
void checkUnused(SymbolTable<SYMBOL>* syms)
{
    int i;
    for (auto sp : *syms)
    { 
        if (sp->sb->storage_class == sc_overloads)
            sp = *sp->tp->syms->begin();
        if (!sp->sb->attribs.inheritable.used && !sp->sb->anonymous)
        {
            if (sp->sb->assigned || sp->sb->altered)
            {
                if (sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register ||
                    sp->sb->storage_class == sc_parameter)
                    errorsym(ERR_SYM_ASSIGNED_VALUE_NEVER_USED, sp);
            }
            else
            {
                if (sp->sb->storage_class == sc_parameter)
                    errorsym(ERR_UNUSED_PARAMETER, sp);
                else
                    errorsym(ERR_UNUSED_VARIABLE, sp);
            }
        }
    }
}
void findUnusedStatics(std::list<NAMESPACEVALUEDATA*>* nameSpace)
{
    for (auto sp : *nameSpace->front()->syms)
    {
        if (sp)
        {
            if (sp->sb->storage_class == sc_namespace)
            {
                findUnusedStatics(sp->sb->nameSpaceValues);
            }
            else
            {
                if (sp->sb->storage_class == sc_overloads)
                {
                    for (auto sp1 : *sp->tp->syms)
                    {
                        if (sp1->sb->attribs.inheritable.isInline && !sp1->sb->inlineFunc.stmt && !sp1->sb->deferredCompile &&
                            !sp1->sb->templateLevel)
                        {
                            errorsym(ERR_UNDEFINED_IDENTIFIER, sp1);
                        }
                        else if (sp1->sb->attribs.inheritable.linkage2 == lk_internal ||
                            (sp1->sb->storage_class == sc_static && !sp1->sb->inlineFunc.stmt &&
                                !(sp1->sb->templateLevel || sp1->sb->instantiated)))
                        {
                            if (sp1->sb->attribs.inheritable.used)
                                errorsym(ERR_UNDEFINED_STATIC_FUNCTION, sp1, eofLine, eofFile);
                            else if (sp1->sb->attribs.inheritable.linkage2 != lk_internal)
                                errorsym(ERR_STATIC_FUNCTION_USED_BUT_NOT_DEFINED, sp1, eofLine, eofFile);
                        }
                    }
                }
                else
                {
                    currentErrorLine = 0;
                    if (sp->sb->storage_class == sc_static && !sp->sb->attribs.inheritable.used)
                        errorsym(ERR_UNUSED_STATIC, sp);
                    currentErrorLine = 0;
                    if (sp->sb->storage_class == sc_global || sp->sb->storage_class == sc_static ||
                        sp->sb->storage_class == sc_localstatic)
                        /* void will be caught earlier */
                        if (!isfunction(sp->tp) && !isarray(sp->tp) && sp->tp->size == 0 && !isvoid(sp->tp) &&
                            sp->tp->type != bt_any && !sp->sb->templateLevel)
                            errorsym(ERR_UNSIZED, sp);
                }
            }
        }
    }
}
static void usageErrorCheck(SYMBOL* sp)
{
    if ((sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register || sp->sb->storage_class == sc_localstatic) &&
        !sp->sb->assigned && !sp->sb->attribs.inheritable.used && !sp->sb->altered)
    {
        if (!structLevel || !sp->sb->deferredCompile)
            errorsym(ERR_USED_WITHOUT_ASSIGNMENT, sp);
    }
    sp->sb->attribs.inheritable.used = true;
}
static SYMBOL* getAssignSP(EXPRESSION* exp)
{
    SYMBOL* sp;
    switch (exp->type)
    {
        case en_global:
        case en_auto:
            return exp->v.sp;
        case en_add:
        case en_structadd:
        case en_arrayadd:
            if ((sp = getAssignSP(exp->left)) != 0)
                return sp;
            return getAssignSP(exp->right);
        default:
            return nullptr;
    }
}
static void assignmentAssign(EXPRESSION* left, bool assign)
{
    while (castvalue(left))
    {
        left = left->left;
    }
    if (lvalue(left))
    {
        SYMBOL* sp;
        sp = getAssignSP(left->left);
        if (sp)
        {
            if (sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register || sp->sb->storage_class == sc_parameter)
            {
                if (assign)
                    sp->sb->assigned = true;
                sp->sb->altered = true;
                //				sp->sb->attribs.inheritable.used = false;
            }
        }
    }
}
void assignmentUsages(EXPRESSION* node, bool first)
{
    FUNCTIONCALL* fp;
    if (node == 0)
        return;
    switch (node->type)
    {
        case en_auto:
            if (node->v.sp->sb)
                node->v.sp->sb->attribs.inheritable.used = true;
            break;
        case en_const:
        case en_msil_array_access:
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:
        case en_c_string:
        case en_nullptr:
        case en_memberptr:
        case en_structelem:
            break;
        case en_global:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            break;
        case en_l_sp:
        case en_l_fp:
        case en_bits:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_c:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_s:
        case en_l_ul:
        case en_l_l:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_inative:
        case en_l_unative:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
        case en_l_string:
        case en_l_object:
            if (node->left->type == en_auto)
            {
                if (!first)
                    usageErrorCheck(node->left->v.sp);
            }
            else
            {
                assignmentUsages(node->left, false);
            }
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_inative:
        case en_x_unative:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_x_string:
        case en_x_object:
        case en_trapcall:
        case en_shiftby:
            /*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_literalclass:
            assignmentUsages(node->left, false);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
            assignmentUsages(node->right, false);
            assignmentUsages(node->left, true);
            assignmentAssign(node->left, true);
            break;
        case en_autoinc:
        case en_autodec:
            assignmentUsages(node->left, false);
            assignmentAssign(node->left, true);
            break;
        case en_add:
        case en_sub:
            /*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_void:
        case en_voidnz:
            /*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
        case en_dot:
        case en_pointsto:
            /*		case en_array: */
            assignmentUsages(node->left, false);
            assignmentUsages(node->right, false);
            break;
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_thisref:
        case en_lvalue:
        case en_funcret:
        case en_select:
            assignmentUsages(node->left, false);
            break;
        case en_atomic:
            assignmentUsages(node->v.ad->flg, false);
            assignmentUsages(node->v.ad->memoryOrder1, false);
            assignmentUsages(node->v.ad->memoryOrder2, false);
            assignmentUsages(node->v.ad->address, false);
            assignmentUsages(node->v.ad->third, false);
            break;
        case en_func:
            fp = node->v.func;
            {
                if (fp->arguments)
                {
                    for (auto args : *fp->arguments)
                    {
                        assignmentUsages(args->exp, false);
                    }
                }
                if (Optimizer::cparams.prm_cplusplus && fp->thisptr && !fp->fcall)
                {
                    error(ERR_MUST_CALL_OR_TAKE_ADDRESS_OF_MEMBER_FUNCTION);
                }
            }
            break;
        case en_stmt:
        case en_templateparam:
        case en_templateselector:
        case en_packedempty:
        case en_sizeofellipse:
        case en__initobj:
        case en__sizeof:
        case en_construct:
            break;
        default:
            diag("assignmentUsages");
            break;
    }
}
static int checkDefaultExpression(EXPRESSION* node)
{
    FUNCTIONCALL* fp;
    int rv = false;
    if (node == 0)
        return 0;
    switch (node->type)
    {
        case en_auto:
            if (!node->v.sp->sb->anonymous)
                rv = true;
            break;
        case en_const:
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:
        case en_nullptr:
        case en_structelem:
        case en_c_string:
            break;
        case en_global:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            break;
        case en_l_sp:
        case en_l_fp:
        case en_bits:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_c:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_s:
        case en_l_ul:
        case en_l_l:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_inative:
        case en_l_unative:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
        case en_l_string:
        case en_l_object:
        case en_literalclass:
            rv |= checkDefaultExpression(node->left);
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_inative:
        case en_x_unative:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_x_string:
        case en_x_object:
        case en_trapcall:
        case en_shiftby:
            /*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
            rv |= checkDefaultExpression(node->left);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
        case en_dot:
        case en_pointsto:
            rv |= checkDefaultExpression(node->right);
            rv |= checkDefaultExpression(node->left);
            break;
        case en_autoinc:
        case en_autodec:
            rv |= checkDefaultExpression(node->left);
            break;
        case en_add:
        case en_sub:
            /*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_void:
        case en_voidnz:
            /*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
            /*		case en_array: */
            rv |= checkDefaultExpression(node->right);
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_thisref:
        case en_lvalue:
        case en_select:
            rv |= checkDefaultExpression(node->left);
            break;
        case en_atomic:
            rv |= checkDefaultExpression(node->v.ad->flg);
            rv |= checkDefaultExpression(node->v.ad->memoryOrder1);
            rv |= checkDefaultExpression(node->v.ad->memoryOrder2);
            rv |= checkDefaultExpression(node->v.ad->address);
            rv |= checkDefaultExpression(node->v.ad->third);
            break;
        case en_func:
            fp = node->v.func;
            if (fp->arguments)
                for (auto args : *fp->arguments)
                    rv |= checkDefaultExpression(args->exp);
            if (fp->sp->sb->parentClass && fp->sp->sb->parentClass->sb->islambda)
                rv |= 2;
            break;
        case en_stmt:
        case en_templateparam:
        case en_templateselector:
        case en__initobj:
        case en__sizeof:
        case en_construct:
            break;
        default:
            diag("rv |= checkDefaultExpression");
            break;
    }
    return rv;
}
void checkDefaultArguments(SYMBOL* spi)
{
    int r = 0;
    if (spi->sb->init)
        for (auto p : *spi->sb->init)
            r |= checkDefaultExpression(p->exp);
    if (r & 1)
    {
        error(ERR_NO_LOCAL_VAR_OR_PARAMETER_DEFAULT_ARGUMENT);
    }
    if (r & 2)
    {
        error(ERR_LAMBDA_CANNOT_CAPTURE);
    }
}
}  // namespace Parser