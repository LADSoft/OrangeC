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
#include "osutil.h"

namespace Parser
{

int currentErrorLine;
SYMBOL* theCurrentFunc;

Keyword skim_end[] = {Keyword::_end, Keyword::_none};
Keyword skim_closepa[] = {Keyword::_closepa, Keyword::_semicolon, Keyword::_end, Keyword::_none};
Keyword skim_semi[] = {Keyword::_semicolon, Keyword::_end, Keyword::_none};
Keyword skim_semi_declare[] = {Keyword::_semicolon, Keyword::_none};
Keyword skim_closebr[] = {Keyword::_closebr, Keyword::_semicolon, Keyword::_end, Keyword::_none};
Keyword skim_comma[] = {Keyword::_comma, Keyword::_closepa, Keyword::_closebr, Keyword::_semicolon, Keyword::_end, Keyword::_none};
Keyword skim_colon[] = {Keyword::_colon, Keyword::_case, Keyword::_default, Keyword::_semicolon, Keyword::_end, Keyword::_none};
Keyword skim_templateend[] = {Keyword::_gt, Keyword::_semicolon, Keyword::_end, Keyword::_none};
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
    printf("Name to number map Keyword::_end.\n");
}
void DumpErrorNumToHelpMap()
{
    printf("Error number to help map:\n");
    for (auto a : error_help_map)
    {
        printf("%d: %s\n", a.first, a.second.c_str());
    }
    printf("Number to help map Keyword::_end.\n");
}
void DumpErrorNameToHelpMap()
{
    printf("Error name to help map:\n");
    for (auto a : error_name_help_map)
    {
        printf("%s: %s\n", a.first, a.second.c_str());
    }
    printf("Name to help map Keyword::_end.\n");
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

static char kwtosym(Keyword kw)
{
    switch (kw)
    {
        case Keyword::_openpa:
            return '(';
        case Keyword::_closepa:
            return ')';
        case Keyword::_closebr:
            return ']';
        case Keyword::_semicolon:
            return ';';
        case Keyword::_begin:
            return '{';
        case Keyword::_end:
            return '}';
        case Keyword::_assign:
            return '=';
        case Keyword::_colon:
            return ':';
        case Keyword::_lt:
            return '<';
        case Keyword::_gt:
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
    if (prm_cppfile.GetValue())
        return false;
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
    if (!nssym)
    {
        strcpy(buf, "<globals>");
    }
    else
    {
        if (nssym->sb->parentNameSpace)
        {
            getns(buf, nssym->sb->parentNameSpace);
            strcat(buf, "::");
        }
        strcat(buf, nssym->name);
    }
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
            SetLinkerNames(sym, Linkage::cdecl_);
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
        SetLinkerNames(sym, Linkage::cdecl_);
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
void errorConversionOrCast(bool convert, TYPE* tp1, TYPE* tp2)
{
    if (isstructured(tp1))
    {
        errortype(ERR_CANNOT_CALL_CONVERSION_FUNCTION_UD, tp1, tp2);
    }
    else if (isstructured(tp2))
    {
        errortype(ERR_CANNOT_CALL_CONVERSION_FUNCTION_CONS, tp1, tp2);
    }
    else
    {
        errortype(convert ? ERR_CANNOT_CONVERT_TYPE : ERR_CANNOT_CAST_TYPE, tp1, tp2);
    }
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
    if (KW(lex) == Keyword::_openpa)
        rv->type = BAL_PAREN;
    else if (KW(lex) == Keyword::_openbr)
        rv->type = BAL_BRACKET;
    else if (KW(lex) == Keyword::_lt)
        rv->type = BAL_LT;
    else
        rv->type = BAL_BEGIN;
    return (rv);
}
static void setbalance(LEXLIST* lex, BALANCE** bal, bool assumeTemplate)
{
    switch (KW(lex))
    {
        case Keyword::_end:
            while (*bal && (*bal)->type != BAL_BEGIN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case Keyword::_closepa:
            while (*bal && (*bal)->type != BAL_PAREN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case Keyword::_closebr:
            while (*bal && (*bal)->type != BAL_BRACKET)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case Keyword::_gt:
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
        case Keyword::_begin:
            if (!*bal || (*bal)->type != BAL_BEGIN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
        case Keyword::_openpa:
            if (!*bal || (*bal)->type != BAL_PAREN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;

        case Keyword::_openbr:
            if (!*bal || (*bal)->type != BAL_BRACKET)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
        case Keyword::_lt:
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

void errskim(LEXLIST** lex, Keyword* skimlist, bool assumeTemplate)
{
    BALANCE* bal = 0;
    while (true)
    {
        if (!*lex)
            break;
        if (!bal)
        {
            int i;
            Keyword kw = KW(*lex);
            for (i = 0; skimlist[i] != Keyword::_none; i++)
                if (kw == skimlist[i])
                    return;
        }
        setbalance(*lex, &bal, assumeTemplate);
        *lex = getsym();
    }
}
void skip(LEXLIST** lex, Keyword kw)
{
    if (MATCHKW(*lex, kw))
        *lex = getsym();
}
bool needkw(LEXLIST** lex, Keyword kw)
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
            case StatementNode::block:
            case StatementNode::switch_:
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                if (hasGoto(stmt->lower))
                    return true;
                break;
            case StatementNode::declare:
            case StatementNode::expr:
                break;
            case StatementNode::goto_:
                return true;
            case StatementNode::return_:
            case StatementNode::select:
            case StatementNode::notselect:
            case StatementNode::label:
            case StatementNode::line:
            case StatementNode::passthrough:
            case StatementNode::datapassthrough:
            case StatementNode::asmcond:
            case StatementNode::varstart:
            case StatementNode::dbgblock:
                break;
            case StatementNode::nop:
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
            case StatementNode::block:
            case StatementNode::switch_:
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                if (hasDeclarations(stmt->lower))
                    return true;
                break;
            case StatementNode::declare:
            case StatementNode::expr:
                if (stmt->hasvla || stmt->hasdeclare)
                    return true;
                break;
            case StatementNode::return_:
            case StatementNode::goto_:
            case StatementNode::select:
            case StatementNode::notselect:
            case StatementNode::label:
            case StatementNode::line:
            case StatementNode::passthrough:
            case StatementNode::datapassthrough:
            case StatementNode::asmcond:
            case StatementNode::varstart:
            case StatementNode::dbgblock:
                break;
            case StatementNode::nop:
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
            case StatementNode::block:
            case StatementNode::switch_:
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                labelIndexes(stmt->lower, min, max);
                break;
            case StatementNode::declare:
            case StatementNode::expr:
                break;
            case StatementNode::goto_:
                if (stmt->indirectGoto)
                    break;
            case StatementNode::select:
            case StatementNode::notselect:
            case StatementNode::label:
                if (stmt->label < *min)
                    *min = stmt->label;
                if (stmt->label > *max)
                    *max = stmt->label;
                break;
            case StatementNode::return_:
            case StatementNode::line:
            case StatementNode::passthrough:
            case StatementNode::datapassthrough:
            case StatementNode::asmcond:
            case StatementNode::varstart:
            case StatementNode::dbgblock:
                break;
            case StatementNode::nop:
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
            case StatementNode::switch_: {
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
            case StatementNode::block:
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                if (stmt->lower && stmt->lower->front()->type == StatementNode::goto_ && !stmt->lower->front()->indirectGoto)
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
            case StatementNode::declare:
            case StatementNode::expr:
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
            case StatementNode::return_:
                *branched = true;
                rv.push_back(mkshim(v_return, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                last = rv.back();
                break;
            case StatementNode::select:
            case StatementNode::notselect:
                *branched = true;
                rv.push_back(mkshim(v_branch, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                last = rv.back();
                nextParent = last;
                break;
            case StatementNode::line:
            case StatementNode::passthrough:
            case StatementNode::datapassthrough:
            case StatementNode::asmcond:
            case StatementNode::varstart:
            case StatementNode::dbgblock:
                break;
            case StatementNode::nop:
                break;
            case StatementNode::goto_:
                if (!stmt->indirectGoto)
                {
                    *branched = true;
                    rv.push_back(mkshim(v_goto, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++));
                    last = rv.back();
                    last->checkme = stmt->explicitGoto;
                }
                break;
            case StatementNode::label:
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
        if (sp->sb->storage_class == StorageClass::ulabel)
        {
            STATEMENT* st;
            specerror(ERR_UNDEFINED_LABEL, sp->name, sp->sb->declfile, sp->sb->declline);
            sp->sb->storage_class = StorageClass::label;
            st = stmtNode(nullptr, block, StatementNode::label);
            st->label = sp->sb->offset;
        }
    }
}
void checkUnused(SymbolTable<SYMBOL>* syms)
{
    int i;
    for (auto sp : *syms)
    { 
        if (sp->sb->storage_class == StorageClass::overloads)
            sp = *sp->tp->syms->begin();
        if (!sp->sb->attribs.inheritable.used && !sp->sb->anonymous)
        {
            if (sp->sb->assigned || sp->sb->altered)
            {
                if (sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_ ||
                    sp->sb->storage_class == StorageClass::parameter)
                    errorsym(ERR_SYM_ASSIGNED_VALUE_NEVER_USED, sp);
            }
            else
            {
                if (sp->sb->storage_class == StorageClass::parameter)
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
            if (sp->sb->storage_class == StorageClass::namespace_)
            {
                findUnusedStatics(sp->sb->nameSpaceValues);
            }
            else
            {
                if (sp->sb->storage_class == StorageClass::overloads)
                {
                    for (auto sp1 : *sp->tp->syms)
                    {
                        if (sp1->sb->attribs.inheritable.isInline && !sp1->sb->inlineFunc.stmt && !sp1->sb->deferredCompile &&
                            !sp1->sb->templateLevel)
                        {
                            errorsym(ERR_UNDEFINED_IDENTIFIER, sp1);
                        }
                        else if (sp1->sb->attribs.inheritable.linkage2 == Linkage::internal_ ||
                            (sp1->sb->storage_class == StorageClass::static_ && !sp1->sb->inlineFunc.stmt &&
                                !(sp1->sb->templateLevel || sp1->sb->instantiated)))
                        {
                            if (sp1->sb->attribs.inheritable.used)
                                errorsym(ERR_UNDEFINED_STATIC_FUNCTION, sp1, eofLine, eofFile);
                            else if (sp1->sb->attribs.inheritable.linkage2 != Linkage::internal_)
                                errorsym(ERR_STATIC_FUNCTION_USED_BUT_NOT_DEFINED, sp1, eofLine, eofFile);
                        }
                    }
                }
                else
                {
                    currentErrorLine = 0;
                    if (sp->sb->storage_class == StorageClass::static_ && !sp->sb->attribs.inheritable.used)
                        errorsym(ERR_UNUSED_STATIC, sp);
                    currentErrorLine = 0;
                    if (sp->sb->storage_class == StorageClass::global || sp->sb->storage_class == StorageClass::static_ ||
                        sp->sb->storage_class == StorageClass::localstatic)
                        /* void will be caught earlier */
                        if (!isfunction(sp->tp) && !isarray(sp->tp) && sp->tp->size == 0 && !isvoid(sp->tp) &&
                            sp->tp->type != BasicType::any && !sp->sb->templateLevel)
                            errorsym(ERR_UNSIZED, sp);
                }
            }
        }
    }
}
static void usageErrorCheck(SYMBOL* sp)
{
    if ((sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_ || sp->sb->storage_class == StorageClass::localstatic) &&
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
        case ExpressionNode::global:
        case ExpressionNode::auto_:
            return exp->v.sp;
        case ExpressionNode::add:
        case ExpressionNode::structadd:
        case ExpressionNode::arrayadd:
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
            if (sp->sb->storage_class == StorageClass::auto_ || sp->sb->storage_class == StorageClass::register_ || sp->sb->storage_class == StorageClass::parameter)
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
        case ExpressionNode::auto_:
            if (node->v.sp->sb)
                node->v.sp->sb->attribs.inheritable.used = true;
            break;
        case ExpressionNode::const_:
        case ExpressionNode::msil_array_access:
            break;
        case ExpressionNode::c_ll:
        case ExpressionNode::c_ull:
        case ExpressionNode::c_d:
        case ExpressionNode::c_ld:
        case ExpressionNode::c_f:
        case ExpressionNode::c_dc:
        case ExpressionNode::c_ldc:
        case ExpressionNode::c_fc:
        case ExpressionNode::c_di:
        case ExpressionNode::c_ldi:
        case ExpressionNode::c_fi:
        case ExpressionNode::c_i:
        case ExpressionNode::c_l:
        case ExpressionNode::c_ui:
        case ExpressionNode::c_ul:
        case ExpressionNode::c_c:
        case ExpressionNode::c_bool:
        case ExpressionNode::c_uc:
        case ExpressionNode::c_wc:
        case ExpressionNode::c_u16:
        case ExpressionNode::c_u32:
        case ExpressionNode::c_string:
        case ExpressionNode::nullptr_:
        case ExpressionNode::memberptr:
        case ExpressionNode::structelem:
            break;
        case ExpressionNode::global:
        case ExpressionNode::pc:
        case ExpressionNode::labcon:
        case ExpressionNode::absolute:
        case ExpressionNode::threadlocal:
            break;
        case ExpressionNode::l_sp:
        case ExpressionNode::l_fp:
        case ExpressionNode::bits:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_fi:
        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::l_c:
        case ExpressionNode::l_wc:
        case ExpressionNode::l_u16:
        case ExpressionNode::l_u32:
        case ExpressionNode::l_s:
        case ExpressionNode::l_ul:
        case ExpressionNode::l_l:
        case ExpressionNode::l_p:
        case ExpressionNode::l_ref:
        case ExpressionNode::l_i:
        case ExpressionNode::l_ui:
        case ExpressionNode::l_inative:
        case ExpressionNode::l_unative:
        case ExpressionNode::l_uc:
        case ExpressionNode::l_us:
        case ExpressionNode::l_bool:
        case ExpressionNode::l_bit:
        case ExpressionNode::l_ll:
        case ExpressionNode::l_ull:
        case ExpressionNode::l_string:
        case ExpressionNode::l_object:
            if (node->left->type == ExpressionNode::auto_)
            {
                if (!first)
                    usageErrorCheck(node->left->v.sp);
            }
            else
            {
                assignmentUsages(node->left, false);
            }
            break;
        case ExpressionNode::uminus:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f:
        case ExpressionNode::x_d:
        case ExpressionNode::x_ld:
        case ExpressionNode::x_fi:
        case ExpressionNode::x_di:
        case ExpressionNode::x_ldi:
        case ExpressionNode::x_fc:
        case ExpressionNode::x_dc:
        case ExpressionNode::x_ldc:
        case ExpressionNode::x_ll:
        case ExpressionNode::x_ull:
        case ExpressionNode::x_i:
        case ExpressionNode::x_ui:
        case ExpressionNode::x_c:
        case ExpressionNode::x_u16:
        case ExpressionNode::x_u32:
        case ExpressionNode::x_wc:
        case ExpressionNode::x_uc:
        case ExpressionNode::x_bool:
        case ExpressionNode::x_bit:
        case ExpressionNode::x_inative:
        case ExpressionNode::x_unative:
        case ExpressionNode::x_s:
        case ExpressionNode::x_us:
        case ExpressionNode::x_l:
        case ExpressionNode::x_ul:
        case ExpressionNode::x_p:
        case ExpressionNode::x_fp:
        case ExpressionNode::x_sp:
        case ExpressionNode::x_string:
        case ExpressionNode::x_object:
        case ExpressionNode::trapcall:
        case ExpressionNode::shiftby:
            /*        case ExpressionNode::movebyref: */
        case ExpressionNode::substack:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack:
        case ExpressionNode::savestack:
        case ExpressionNode::literalclass:
            assignmentUsages(node->left, false);
            break;
        case ExpressionNode::assign:
        case ExpressionNode::_initblk:
        case ExpressionNode::_cpblk:
            assignmentUsages(node->right, false);
            assignmentUsages(node->left, true);
            assignmentAssign(node->left, true);
            break;
        case ExpressionNode::auto_inc:
        case ExpressionNode::auto_dec:
            assignmentUsages(node->left, false);
            assignmentAssign(node->left, true);
            break;
        case ExpressionNode::add:
        case ExpressionNode::sub:
            /*        case ExpressionNode::addcast: */
        case ExpressionNode::lsh:
        case ExpressionNode::arraylsh:
        case ExpressionNode::rsh:
        case ExpressionNode::void_:
        case ExpressionNode::void_nz:
            /*        case ExpressionNode::dvoid: */
        case ExpressionNode::arraymul:
        case ExpressionNode::arrayadd:
        case ExpressionNode::arraydiv:
        case ExpressionNode::structadd:
        case ExpressionNode::mul:
        case ExpressionNode::div:
        case ExpressionNode::umul:
        case ExpressionNode::udiv:
        case ExpressionNode::umod:
        case ExpressionNode::ursh:
        case ExpressionNode::mod:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
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
        case ExpressionNode::cond:
        case ExpressionNode::intcall:
        case ExpressionNode::stackblock:
        case ExpressionNode::blockassign:
        case ExpressionNode::mp_compare:
        case ExpressionNode::dot:
        case ExpressionNode::pointsto:
            /*		case ExpressionNode::array: */
            assignmentUsages(node->left, false);
            assignmentUsages(node->right, false);
            break;
        case ExpressionNode::mp_as_bool:
        case ExpressionNode::blockclear:
        case ExpressionNode::argnopush:
        case ExpressionNode::not__lvalue:
        case ExpressionNode::thisref:
        case ExpressionNode::lvalue:
        case ExpressionNode::funcret:
        case ExpressionNode::select:
            assignmentUsages(node->left, false);
            break;
        case ExpressionNode::atomic:
            assignmentUsages(node->v.ad->flg, false);
            assignmentUsages(node->v.ad->memoryOrder1, false);
            assignmentUsages(node->v.ad->memoryOrder2, false);
            assignmentUsages(node->v.ad->address, false);
            assignmentUsages(node->v.ad->third, false);
            break;
        case ExpressionNode::func:
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
        case ExpressionNode::stmt:
        case ExpressionNode::templateparam:
        case ExpressionNode::templateselector:
        case ExpressionNode::packedempty:
        case ExpressionNode::sizeofellipse:
        case ExpressionNode::_initobj:
        case ExpressionNode::_sizeof:
        case ExpressionNode::const_ruct:
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
        case ExpressionNode::auto_:
            if (!node->v.sp->sb->anonymous)
                rv = true;
            break;
        case ExpressionNode::const_:
            break;
        case ExpressionNode::c_ll:
        case ExpressionNode::c_ull:
        case ExpressionNode::c_d:
        case ExpressionNode::c_ld:
        case ExpressionNode::c_f:
        case ExpressionNode::c_dc:
        case ExpressionNode::c_ldc:
        case ExpressionNode::c_fc:
        case ExpressionNode::c_di:
        case ExpressionNode::c_ldi:
        case ExpressionNode::c_fi:
        case ExpressionNode::c_i:
        case ExpressionNode::c_l:
        case ExpressionNode::c_ui:
        case ExpressionNode::c_ul:
        case ExpressionNode::c_c:
        case ExpressionNode::c_bool:
        case ExpressionNode::c_uc:
        case ExpressionNode::c_wc:
        case ExpressionNode::c_u16:
        case ExpressionNode::c_u32:
        case ExpressionNode::nullptr_:
        case ExpressionNode::structelem:
        case ExpressionNode::c_string:
            break;
        case ExpressionNode::global:
        case ExpressionNode::pc:
        case ExpressionNode::labcon:
        case ExpressionNode::absolute:
        case ExpressionNode::threadlocal:
            break;
        case ExpressionNode::l_sp:
        case ExpressionNode::l_fp:
        case ExpressionNode::bits:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_fi:
        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::l_c:
        case ExpressionNode::l_wc:
        case ExpressionNode::l_u16:
        case ExpressionNode::l_u32:
        case ExpressionNode::l_s:
        case ExpressionNode::l_ul:
        case ExpressionNode::l_l:
        case ExpressionNode::l_p:
        case ExpressionNode::l_ref:
        case ExpressionNode::l_i:
        case ExpressionNode::l_ui:
        case ExpressionNode::l_inative:
        case ExpressionNode::l_unative:
        case ExpressionNode::l_uc:
        case ExpressionNode::l_us:
        case ExpressionNode::l_bool:
        case ExpressionNode::l_bit:
        case ExpressionNode::l_ll:
        case ExpressionNode::l_ull:
        case ExpressionNode::l_string:
        case ExpressionNode::l_object:
        case ExpressionNode::literalclass:
            rv |= checkDefaultExpression(node->left);
            break;
        case ExpressionNode::uminus:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f:
        case ExpressionNode::x_d:
        case ExpressionNode::x_ld:
        case ExpressionNode::x_fi:
        case ExpressionNode::x_di:
        case ExpressionNode::x_ldi:
        case ExpressionNode::x_fc:
        case ExpressionNode::x_dc:
        case ExpressionNode::x_ldc:
        case ExpressionNode::x_ll:
        case ExpressionNode::x_ull:
        case ExpressionNode::x_i:
        case ExpressionNode::x_ui:
        case ExpressionNode::x_inative:
        case ExpressionNode::x_unative:
        case ExpressionNode::x_c:
        case ExpressionNode::x_u16:
        case ExpressionNode::x_u32:
        case ExpressionNode::x_wc:
        case ExpressionNode::x_uc:
        case ExpressionNode::x_bool:
        case ExpressionNode::x_bit:
        case ExpressionNode::x_s:
        case ExpressionNode::x_us:
        case ExpressionNode::x_l:
        case ExpressionNode::x_ul:
        case ExpressionNode::x_p:
        case ExpressionNode::x_fp:
        case ExpressionNode::x_sp:
        case ExpressionNode::x_string:
        case ExpressionNode::x_object:
        case ExpressionNode::trapcall:
        case ExpressionNode::shiftby:
            /*        case ExpressionNode::movebyref: */
        case ExpressionNode::substack:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack:
        case ExpressionNode::savestack:
            rv |= checkDefaultExpression(node->left);
            break;
        case ExpressionNode::assign:
        case ExpressionNode::_initblk:
        case ExpressionNode::_cpblk:
        case ExpressionNode::dot:
        case ExpressionNode::pointsto:
            rv |= checkDefaultExpression(node->right);
            rv |= checkDefaultExpression(node->left);
            break;
        case ExpressionNode::auto_inc:
        case ExpressionNode::auto_dec:
            rv |= checkDefaultExpression(node->left);
            break;
        case ExpressionNode::add:
        case ExpressionNode::sub:
            /*        case ExpressionNode::addcast: */
        case ExpressionNode::lsh:
        case ExpressionNode::arraylsh:
        case ExpressionNode::rsh:
        case ExpressionNode::void_:
        case ExpressionNode::void_nz:
            /*        case ExpressionNode::dvoid: */
        case ExpressionNode::arraymul:
        case ExpressionNode::arrayadd:
        case ExpressionNode::arraydiv:
        case ExpressionNode::structadd:
        case ExpressionNode::mul:
        case ExpressionNode::div:
        case ExpressionNode::umul:
        case ExpressionNode::udiv:
        case ExpressionNode::umod:
        case ExpressionNode::ursh:
        case ExpressionNode::mod:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
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
        case ExpressionNode::cond:
        case ExpressionNode::intcall:
        case ExpressionNode::stackblock:
        case ExpressionNode::blockassign:
        case ExpressionNode::mp_compare:
            /*		case ExpressionNode::array: */
            rv |= checkDefaultExpression(node->right);
        case ExpressionNode::mp_as_bool:
        case ExpressionNode::blockclear:
        case ExpressionNode::argnopush:
        case ExpressionNode::not__lvalue:
        case ExpressionNode::thisref:
        case ExpressionNode::lvalue:
        case ExpressionNode::select:
            rv |= checkDefaultExpression(node->left);
            break;
        case ExpressionNode::atomic:
            rv |= checkDefaultExpression(node->v.ad->flg);
            rv |= checkDefaultExpression(node->v.ad->memoryOrder1);
            rv |= checkDefaultExpression(node->v.ad->memoryOrder2);
            rv |= checkDefaultExpression(node->v.ad->address);
            rv |= checkDefaultExpression(node->v.ad->third);
            break;
        case ExpressionNode::func:
            fp = node->v.func;
            if (fp->arguments)
                for (auto args : *fp->arguments)
                    rv |= checkDefaultExpression(args->exp);
            if (fp->sp->sb->parentClass && fp->sp->sb->parentClass->sb->islambda)
                rv |= 2;
            break;
        case ExpressionNode::stmt:
        case ExpressionNode::templateparam:
        case ExpressionNode::templateselector:
        case ExpressionNode::_initobj:
        case ExpressionNode::_sizeof:
        case ExpressionNode::const_ruct:
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