/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#define _CRT_SECURE_NO_WARNINGS

#include "ppDefine.h"
#include "Token.h"
#include "PreProcessor.h"
#include "ppkw.h"
#include "Errors.h"
#include "SymbolTable.h"
#include "UTF8.h"
#include "Utils.h"
#include <ctime>
#include <climits>
#include <cstdlib>

KeywordHash ppDefine::defTokens = {
    {"(", kw::openpa},
    {")", kw::closepa},
    {",", kw::comma},
    {"...", kw::ellipses},

};
ppDefine::Definition& ppDefine::Definition::operator=(const ppDefine::Definition& old)
{
    caseInsensitive = old.caseInsensitive;
    undefined = old.undefined;
    permanent = old.permanent;
    elipses = old.elipses;
    varargs = old.varargs;
    preprocessing = old.preprocessing;
    value = old.value;
    if (old.argList)
    {
        argList = std::make_unique<DefinitionArgList>();
        for (auto&& a : *old.argList)
            argList->push_back(a);
    }
    return *this;
}
ppDefine::Definition::Definition(const Definition& old) : Symbol(old.GetName())
{
    caseInsensitive = old.caseInsensitive;
    undefined = old.undefined;
    permanent = old.permanent;
    elipses = old.elipses;
    varargs = old.varargs;
    preprocessing = old.preprocessing;
    value = old.value;
    if (old.argList)
    {
        argList = std::make_unique<DefinitionArgList>();
        for (auto&& a : *old.argList)
            argList->push_back(a);
    }
}

ppDefine::ppDefine(bool UseExtensions, ppInclude* Include, bool C89, bool Asmpp) :
    expr(false), include(Include), c89(C89), asmpp(Asmpp), ctx(nullptr), macro(nullptr), source_date_epoch((time_t)-1)
{
    char* sde = getenv("SOURCE_DATE_EPOCH");
    if (sde)
        source_date_epoch = (time_t)strtoul(sde, nullptr, 10);
    SetDefaults();
    expr.SetDefine(this);
}
bool ppDefine::Check(kw token, std::string& line)
{
    bool rv = true;
    switch (token)
    {
        case kw::DEFINE:
            DoDefine(line, false);
            break;
        case kw::IDEFINE:
            DoDefine(line, true);
            break;
        case kw::UNDEF:
            DoUndefine(line);
            break;
            //		case ASSIGN:
            //			DoAssign(line, false);
            //			break;
            //		case IASSIGN:
            //			DoAssign(line, true);
            //			break;
        default:
            rv = false;
            break;
    }
    return rv;
}
#include <iostream>
ppDefine::Definition* ppDefine::Define(const std::string& name, std::string& value, DefinitionArgList* args, bool permanent,
                                       bool varargs, bool errors, bool caseInsensitive)
{
    if (asmpp)
    {
        ParseAsmSubstitutions(value);
    }
    Symbol* definesym = symtab.Lookup(name);
    Definition* old = static_cast<Definition*>(definesym);
    size_t n = value.find_first_not_of(" \t\v\n");
    if (n)
        value.erase(0, n);
    n = value.find_last_not_of(" \t\v\n");
    if (n != std::string::npos && n != value.size() - 1)
        value.erase(n + 1);

    int last = 0, pos;
    std::string x;
    int instr = 0;
    for (pos = 0; pos < value.size(); pos++)
    {
        if (!instr && (value[pos] == '"' || value[pos] == '\''))
        {
            instr = value[pos];
        }
        else if (instr == value[pos])
        {
            instr = 0;
        }
        else if (!instr)
        {
            if (isspace(value[pos]))
            {
                x += value.substr(last, pos - last) + " ";
                // already stripped spaces off the end so this is safe.
                while (isspace(value[pos]))
                    pos++;
                last = pos;
                pos--;
            }
        }
    }
    x += value.substr(last, pos - last);
    value = x;
    static char tk[2] = {REPLACED_TOKENIZING, 0};

    n = value.find("##");
    while (n != std::string::npos)
    {
        value.replace(n, 2, tk);
        n = value.find("##", n + 1);
    }
    n = value.find("%:%:");
    while (n != std::string::npos)
    {
        value.replace(n, 4, tk);
        n = value.find("%:%:", n + 1);
    }

    static char mp[2] = {MACRO_PLACEHOLDER, 0};
    value = std::string(mp) + value + mp;

    std::string name1 = name;
    if (caseInsensitive)
        name1 = UTF8::ToUpper(name1);
    Definition* d = new Definition(name1, value, args, permanent);
    if (value[1] == REPLACED_TOKENIZING)
        d->DefinedError("macro definition begins with tokenizing token");
    d->SetCaseInsensitive(caseInsensitive);
    d->SetLocation(include->GetRealFile(), include->GetRealLineNo());
    if (varargs)
        d->SetHasVarArgs();
    if (errors && old)
    {
        bool failed = false;
        if (old->GetArgCount() != d->GetArgCount())
            failed = true;
        else if (old->HasVarArgs() != d->HasVarArgs())
            failed = true;
        else
        {
            int n = old->GetArgCount();
            for (int i = 0; i < n; i++)
                if (old->GetArg(i) != d->GetArg(i))
                {
                    failed = true;
                    break;
                }
            const char* p = old->GetValue().c_str();
            const char* q = d->GetValue().c_str();
            while (*p && *q)
            {
                if (isspace(*p) && isspace(*q))
                {
                    while (isspace(*p))
                        p++;
                    while (isspace(*q))
                        q++;
                }
                if (*p != *q)
                    break;
                p++, q++;
            }
            while (isspace(*p))
                p++;
            while (isspace(*q))
                q++;
            if (*p || *q)
            {
                failed = true;
            }
        }
        if (failed)
        {
            auto it = macroStacks.find(d->GetName());
            if (it == macroStacks.end())  // only give the error if never been involved in a push
                old->DefinedWarning("macro redefinition changes value");
        }
        symtab.Remove(old);
    }
    symtab.Add(d);
    return d;
}
void ppDefine::Undefine(const std::string& name)
{
    Symbol* define = symtab.Lookup(name);

    if (define)
    {
        symtab.Remove(define);
    }
}
void ppDefine::DoAssign(std::string& line, bool caseInsensitive)
{
    bool failed = false;
    Tokenizer tk(line, &defTokens);
    const Token* next = tk.Next();
    std::string name;
    if (!next->IsIdentifier())
    {
        failed = true;
    }
    else
    {
        name = next->GetId();
        std::string line = tk.GetString();
        Process(line);
        PPINT n = expr.Eval(line);
        if (n < INT_MIN || n >= UINT_MAX)
        {
            Errors::Error("%assign does not support long long");
            n = 0;
        }
        failed = line.find_first_not_of(" \t\v\r\n") != std::string::npos;
        if (!failed)
        {
            std::string value = Utils::NumberToString((int)n);
            Define(name, value, nullptr, false, false, false, caseInsensitive);
        }
    }
    if (failed)
    {
        Errors::Error("Error in %assign directive");
    }
}
void ppDefine::DoDefine(std::string& line, bool caseInsensitive)
{
    bool inctx = false;
    if (asmpp)
    {
        int n = line.find_first_not_of(" \t\r\n\v");
        if (n != std::string::npos)
        {
            if (n < line.size() - 2)
            {
                if (line[n] == '%' && line[n + 1] == '$')
                {
                    inctx = true;
                    line.erase(0, n + 2);
                }
            }
        }
    }
    Tokenizer tk(line, &defTokens);
    const Token* next = tk.Next();
    bool failed = false;
    bool hasEllipses = false;
    std::unique_ptr<DefinitionArgList> da;
    std::string name;
    if (!next->IsIdentifier())
    {
        failed = true;
    }
    else
    {
        int n1;
        if (inctx && (n1 = ctx->GetTopId()) != -1)
        {
            char buf[256];
            sprintf(buf, "..@%d$", n1);
            name = std::string(buf) + next->GetId();
        }
        else
        {
            name = next->GetId();
        }
        if (tk.GetString()[0] == '(')  // yes it HAS to be the first character, no spaces
                                       // or other characters allowed
        {
            // the below is ok because the first one gets the '(' and the next one gets the next token
            next = tk.Next();  // get '('
            next = tk.Next();  // past '('
            if (!next->IsIdentifier() && next->GetKeyword() != kw::closepa && next->GetKeyword() != kw::ellipses)
            {
                failed = true;
            }
            else
            {
                bool hascomma = true;
                da = std::make_unique<DefinitionArgList>();
                bool done = false;
                while (next->IsIdentifier())
                {
                    da->push_back(next->GetId());
                    next = tk.Next();
                    if (next->GetKeyword() != kw::comma)
                    {
                        hascomma = false;
                        break;
                    }
                    next = tk.Next();
                }
                if (hascomma)
                {
                    if (next->GetKeyword() == kw::ellipses)
                    {
                        if (c89)
                            Errors::Error("Macro variable argument specifier only allowed in C99");
                        hasEllipses = true;
                        next = tk.Next();
                    }
                }
                if (next->GetKeyword() != kw::closepa)
                {
                    failed = true;
                }
            }
        }
    }
    if (!failed)
    {
        std::string ref = tk.GetString();
        Define(name, ref, da.release(), false, hasEllipses, !asmpp, caseInsensitive);
    }
    else
        Errors::Error("Macro argument syntax error");
}
void ppDefine::DoUndefine(std::string& line)
{
    Tokenizer tk(line, nullptr);
    const Token* t = tk.Next();
    if (!t->IsIdentifier())
    {
        Errors::Error("Expected symbol to undefine");
    }
    else
    {
        Undefine(t->GetId());
    }
}
void ppDefine::SetDefaults()
{
    char string[256];
    struct tm* t1;
    time_t t2;
    if (source_date_epoch != (time_t)-1)
        t2 = source_date_epoch;
    else
        t2 = ::time(0);
    t1 = localtime(&t2);
    if (t1)
    {
        strftime(string, 40, "\"%b %d %Y\"", t1);
        // first character of day must be space or non-zero
        if (string[5] == '0')
            string[5] = ' ';
        date = string;
        strftime(string, 40, "\"%H:%M:%S\"", t1);
        time = string;
        strftime(string, 40, "\"%Y-%m-%d\"", t1);
        dateiso = string;
    }
}
int ppDefine::LookupDefault(std::string& macro, int begin, int end, const std::string& name)
{
    std::string insert;
    if (name == "__FILE__")
    {
        std::string errfile = include->GetErrFile();
        int n = errfile.find('\\');
        while (n != std::string::npos)
        {
            errfile.replace(n, 1, "\\\\");
            n = errfile.find('\\', n + 2);
        }
        insert = std::string("\"") + errfile + "\"";
    }
    else if (name == "__LINE__")
    {
        insert = Utils::NumberToString(include->GetErrLineNo());
    }
    else if (name == "__DATE__")
        insert = date;
    else if (name == "__DATEISO__")
        insert = dateiso;
    else if (name == "__TIME__")
        insert = time;
    else
        return 0;
    macro.replace(begin, end - begin, insert);
    return insert.size();
}
std::string ppDefine::defid(const std::string& macroname, int& i, int& j)
/*
 * Get an identifier during macro replacement
 */
{
    //    char name[2048];
    bool inctx = false;
    bool quoted = false;
    if (asmpp && j >= 2 && macroname[j - 1] == '$')
    {
        if (macroname[j - 2] == '%')
        {
            inctx = true;
        }
        else if (j >= 3 && macroname[j - 2] == '{' && macroname[j - 3] == '%')
        {
            quoted = true;
            inctx = true;
        }
    }
    while (j < macroname.size() && Tokenizer::IsSymbolChar(macroname.c_str() + j, false))
    {
        int n = UTF8::CharSpan(macroname.c_str() + j);
        for (int i = 0; i < n && macroname[j]; i++)
            j++;
    }
    std::string rv = macroname.substr(i, j - i);
    if (inctx)
    {
        int n1 = ctx->GetTopId();
        if (n1 != -1)
        {
            i -= 2 + quoted;
            if (quoted)
            {
                if (macroname[j] != '}')
                    Errors::Error("Macro substition: expected '}'");
                else
                    j++;
            }
            char buf[256];
            sprintf(buf, "..@%d$", n1);
            rv = std::string(buf) + rv;
        }
    }
    return rv;
}
void ppDefine::Stringize(std::string& macro)
{
    std::string repl;
    int waiting = 0;
    int last = 0, pos;

    for (pos = 0; pos < macro.size(); pos++)
    {
        if (!waiting && (macro[pos] == '"' || macro[pos] == '\'') && NotSlashed(macro, pos))
        {
            waiting = macro[pos];
        }
        else if (waiting)
        {
            if (macro[pos] == waiting && NotSlashed(macro, pos))
                waiting = 0;
        }
        else if (macro[pos] == '#' && (pos == 0 || macro[pos - 1] != '#') &&
                 (pos == macro.size() - 1 || macro[pos + 1] != '#')) /* # ## # */
        {
            repl += macro.substr(last, pos - last);

            pos++;
            for (; pos < macro.size() && isspace(macro[pos]); ++pos)
                ;
            static char mp[2] = {STRINGIZING_PLACEHOLDER, 0};
            size_t n = macro.find(mp, pos);
            if (n == std::string::npos)
                n = macro.size();
            std::string candidate = macro.substr(pos, n - pos);
            pos = n - 1;
            last = n;
            if (n != macro.size())
                last++;
            n = candidate.find_first_of("\\\"");
            while (n != std::string::npos)
            {
                candidate.insert(n, "\\");
                n = candidate.find_first_of("\\\"", n + 2);
            }
            n = candidate.find_first_of(" \t\v\n");
            while (n != std::string::npos)
            {
                size_t m = n + 1;
                while (isspace(candidate[m]))
                    m++;
                candidate.replace(n, m - n, " ");
                n = candidate.find_first_of(" \t\v\n", n + 1);
            }
            repl += std::string("\"") + candidate + "\"";
        }
    }
    if (last < macro.size())
        repl += macro.substr(last);
    macro = repl;
}
bool ppDefine::Tokenize(std::string& macro)
{
    bool rv = false;
    int waiting = 0;
    for (int i = 0; i < macro.size(); i++)
    {
        if (!waiting && (macro[i] == '"' || macro[i] == '\'') && NotSlashed(macro, i))
        {
            waiting = macro[i];
        }
        else if (waiting)
        {
            if (macro[i] == waiting && NotSlashed(macro, i))
                waiting = 0;
        }
        else if (macro[i] == REPLACED_TOKENIZING)
        {
            int b = i, e = i;
            while (b > 0 && (isspace(macro[b - 1]) || macro[b - 1] == MACRO_PLACEHOLDER))
                b--;

            while (++e < macro.size() && (isspace(macro[e]) || macro[e] == MACRO_PLACEHOLDER))
                ;
            if (b > 0 && macro[b - 1] == TOKENIZING_PLACEHOLDER && macro[e] != TOKENIZING_PLACEHOLDER)
                macro[b - 1] = MACRO_PLACEHOLDER;
            if (e < macro.size() && macro[e] == TOKENIZING_PLACEHOLDER)
            {
                macro[e] = MACRO_PLACEHOLDER;
            }
            macro.erase(b, e - b);
            rv = true;
            i = b - 1;
        }
    }
    return rv;
}
int ppDefine::InsertReplacementString(std::string& macro, int end, int begin, std::string text, std::string etext)
{
    int q;
    static char nullptrTOKEN[] = {TOKENIZING_PLACEHOLDER, 0};
    static char STRINGIZERTOKEN[] = {STRINGIZING_PLACEHOLDER, 0};
    int stringizing = false;
    q = end;
    while (q < macro.size() - 1 && isspace(macro[q]))
        q++;
    if (macro[q] == REPLACED_TOKENIZING)
    {
        if (text.empty())
        {
            text = nullptrTOKEN;
        }
    }
    else
    {
        if (begin == 0)
        {
            text = etext;
        }
        else
        {
            q = begin - 1;

            while (q > 0 && isspace(macro[q]))
                q--;
            if (macro[q] == REPLACED_TOKENIZING)
            {
                if (text.empty())
                {
                    text = nullptrTOKEN;
                }
            }
            else if (macro[q] == '#')
            {
                stringizing = true;
            }
            else
            {
                text = etext;
            }
        }
    }
    if (stringizing)
        text = text + STRINGIZERTOKEN;
    macro.replace(begin, end - begin, text);
    return text.size();
}
bool ppDefine::NotSlashed(const std::string& macro, int pos)
{
    int count = 0;
    while (pos && macro[--pos] == '\\')
        count++;
    return !(count & 1);
}
bool ppDefine::ppNumber(const std::string& macro, int start, int pos)
{
    int x = pos;
    if (pos <= 0)
        return false;
    if (macro[pos] == '+' || macro[pos] == '-' ||
        isdigit(macro[pos]))  // we would get here with the first alpha char following the number
    {
        // backtrack through all characters that could possibly be part of the number
        while (pos >= start && (Tokenizer::IsSymbolChar(macro.c_str() + pos, false) || macro[pos] == '.' ||
                                ((macro[pos] == '-' || macro[pos] == '+') && (macro[pos - 1] == 'e' || macro[pos - 1] == 'E' ||
                                                                              macro[pos - 1] == 'p' || macro[pos - 1] == 'P'))))
        {
            if (macro[pos] == '-' || macro[pos] == '+')
                pos--;
            pos--;
        }
        // go forward, skipping sequences that couldn't actually start a number
        pos++;
        if (!isdigit(macro[pos]))
        {
            while (pos < x && (macro[pos] != '.' || isdigit(macro[pos - 1]) || !isdigit(macro[pos + 1])))
                pos++;
        }
        // if we didn't get back where we started we have a number
        return pos < x && (macro[pos] != '0' || (macro[pos + 1] != 'x' && macro[pos + 1] != 'X'));
    }
    return false;
}
/* replace macro args */
bool ppDefine::ReplaceArgs(std::string& macro, const DefinitionArgList& oldargs, const DefinitionArgList& newargs,
                           const DefinitionArgList& expandedargs, const std::string varargs)
{
    std::string name;
    int waiting = 0;
    for (int p = 0; p < macro.size(); p++)
    {
        if (!waiting && (macro[p] == '"' || macro[p] == '\'') && NotSlashed(macro, p))
        {
            waiting = macro[p];
        }
        else if (waiting)
        {
            if (macro[p] == waiting && NotSlashed(macro, p))
                waiting = 0;
        }
        else if (Tokenizer::IsSymbolChar(macro.c_str() + p, false))
        {
            int q = p;
            name = defid(macro, q, p);
            if (!c89 && name == "__VA_ARGS__")
            {
                if (varargs.empty())
                {
                    int rv;
                    if ((rv = InsertReplacementString(macro, p, q, "", "")) < -MACRO_REPLACE_SIZE)
                        return (false);
                    else
                        p = q + rv - 1;
                }
                else
                {
                    int rv;
                    if ((rv = InsertReplacementString(macro, p, q, varargs, varargs)) < -MACRO_REPLACE_SIZE)
                        return (false);
                    else
                        p = q + rv - 1;
                }
            }
            else
                for (int i = 0; i < oldargs.size(); i++)
                {
                    if (name == oldargs[i])
                    {
                        int rv;
                        if ((rv = InsertReplacementString(macro, p, q, newargs[i], expandedargs[i])) < -MACRO_REPLACE_SIZE)
                            return (false);
                        else
                        {
                            p = q + rv - 1;
                            break;
                        }
                    }
                }
        }
    }
    return (true);
}
void ppDefine::SyntaxError(const std::string& name)
{
    Errors::Error(std::string("Wrong number of arguments in call to macro ") + name);
}

void ppDefine::SetupAlreadyReplaced(std::string& macro)
{
    bool instr = false;
    for (int p = 0; p < macro.size(); p++)
    {
        if ((macro[p] == '"' || macro[p] == '\'') && NotSlashed(macro, p))
            instr = !instr;
        if (Tokenizer::IsSymbolChar(macro.c_str() + p, true) && !instr)
        {
            int q = p;
            std::string name;
            name = defid(macro, q, p);
            Symbol* sym = symtab.Lookup(name);
            Definition* d = static_cast<Definition*>(sym);
            if (d && d->IsPreprocessing())
            {
                static char ra[2] = {REPLACED_ALREADY, 0};
                macro.insert(q, ra);
            }
            else
            {
                p--;
            }
        }
    }
}
int ppDefine::ReplaceSegment(std::string& line, int begin, int end, int& pptr, bool eol, std::deque<Definition*>& definitions,
                             std::deque<TokenPos>* positions)
{
    std::string name;
    int waiting = 0;

    int orig_end = end;
    int rv;
    int p;
    int insize, rv1;
    int origPos = begin;
    for (p = begin; p < end;)
    {
        int q = p;
        if (!waiting && (line[p] == '"' || line[p] == '\'') && NotSlashed(line, p))
        {
            waiting = line[p++];
            origPos++;
        }
        else if (waiting)
        {
            if (line[p] == waiting && NotSlashed(line, p))
                waiting = 0;
            p++;
            origPos++;
        }
        else if (Tokenizer::IsSymbolChar(line.c_str() + p, true) &&
                 (p == begin || line[p - 1] == '$' || !Tokenizer::IsSymbolChar(line.c_str() + p - 1, false)))
        {
            TokenPos tokenPos;
            tokenPos.origStart = origPos;
            tokenPos.newStart = q;
            int origStart = origPos;
            name = defid(line, q, p);
            origPos += p - q;
            tokenPos.origEnd = origPos;
            tokenPos.newEnd = q;
            Symbol* sym = symtab.Lookup(name);
            Definition* d = static_cast<Definition*>(sym);
            bool tokenized = false;
            if (!d && asmpp)
            {
                std::string name1 = UTF8::ToUpper(name);
                sym = symtab.Lookup(name1);
                d = static_cast<Definition*>(sym);
                if (d && d->IsCaseInsensitive())
                {
                    name = name1;
                }
                else
                {
                    sym = nullptr;
                    d = nullptr;
                }
            }
            if (
#ifndef NOCPLUSPLUS
                (name != "R" || line[p] != '"') &&
#endif
                d != nullptr && (!q || line[q - 1] != REPLACED_ALREADY) && !ppNumber(line, 0, p - 1 - name.size()))
            {
                std::string macro;
                if (d->GetArgList() != nullptr)
                {
                    int q1 = p;
                    int count = 0;

                    while (q1 < line.size() && (isspace(line[q1]) || line[q1] == MACRO_PLACEHOLDER))
                        q1++;
                    if (q1 == line.size() && eol)
                    {
                        // continues on the next line, get more text..
                        return INT_MIN + 1;
                    }
                    if (line[q1++] != '(')
                    {
                        int n = LookupDefault(line, q1, p, name);
                        if (n)
                            p = q1 + n - 1;
                        else
                            p = q1 - 1;
                        continue;
                    }
                    p = q1;
                    DefinitionArgList args, expandedargs;
                    std::string varargs;
                    if (d->GetArgCount() > 0)
                    {
                        int ln = name.size();
                        do
                        {
                            int pb = p;
                            int nestedparen = 0, nestedstring = 0;
                            while (p < line.size() && isspace(line[p]))
                                p++;
                            while (p < line.size() &&
                                   (((line[p] != ',' && line[p] != ')') || nestedparen || nestedstring) && line[p] != '\n'))
                            {
                                if (nestedstring)
                                {
                                    if (line[p] == nestedstring && NotSlashed(line, p))
                                        nestedstring = 0;
                                }
                                else if ((line[p] == '\'' || line[p] == '"') && NotSlashed(line, p))
                                    nestedstring = line[p];
                                else if (line[p] == '(')
                                    nestedparen++;
                                else if (line[p] == ')' && nestedparen)
                                    nestedparen--;
                                p++;
                            }
                            q1 = p;
                            while (q1 && isspace(line[q1 - 1]))
                                q1--;
                            std::string temp = line.substr(pb, q1 - pb);
                            args.push_back(temp);
                            expandedargs.push_back(temp);
                            int sv;
                            rv = ReplaceSegment(expandedargs[count], 0, expandedargs[count].size(), sv, p == line.size(),
                                                definitions, nullptr);
                            if (rv < -MACRO_REPLACE_SIZE)
                            {
                                return rv;
                            }
                            count++;
                        } while (line[p] && line[p++] == ',' && count != d->GetArgCount());
                    }
                    else
                    {
                        count = 0;
                        while (p < line.size() - 1 && isspace(line[p]))
                            p++;
                        if (line[p] == ')')
                            p++;
                    }
                    if (line[p - 1] != ')' || count != d->GetArgCount())
                    {
                        if (count == d->GetArgCount() && !c89 && d->HasVarArgs())
                        {
                            q1 = p;
                            int nestedparen = 0, nestedstring = 0;
                            while (p < line.size() && line[p] != '\n' && (nestedstring || line[p] != ')' || nestedparen))
                            {
                                if (nestedstring)
                                {
                                    if (line[p] == nestedstring && NotSlashed(line, p))
                                        nestedstring = 0;
                                }
                                else if ((line[p] == '\'' || line[p] == '"') && NotSlashed(line, p))
                                    nestedstring = line[p];
                                else if (line[p] == '(')
                                    nestedparen++;
                                else if (line[p] == ')' && nestedparen)
                                    nestedparen--;
                                p++;
                            }
                            varargs = line.substr(q1, p - q1);
                            p++;
                        }
                        if (line[p - 1] != ')' || count != d->GetArgCount())
                        {
                            if (p >= line.size())
                            {
                                // continues on the next line, get more text
                                return INT_MIN + 1;
                            }
                            SyntaxError(name);
                            return INT_MIN;
                        }
                    }

                    macro = d->GetValue();
                    if (count != 0 || !varargs.empty() || d->HasVarArgs())
                        if (!ReplaceArgs(macro, *d->GetArgList(), args, expandedargs, varargs))
                            return INT_MIN;
                    tokenized = Tokenize(macro);
                    Stringize(macro);
                    static char tk[2] = {TOKENIZING_PLACEHOLDER, 0};
                    size_t n = macro.find(tk);
                    while (n != std::string::npos)
                    {
                        macro.erase(n, 1);
                        n = macro.find(tk, n);
                    }
                    if (tokenized)
                    {
                        for (auto&& d : definitions)
                            d->SetPreprocessing(false);
                        definitions.clear();
                    }
                }
                else
                {
                    macro = d->GetValue();
                }
                if (!tokenized)
                {
                    d->SetPreprocessing(true);
                    definitions.push_back(d);
                }
                SetupAlreadyReplaced(macro);
                rv1 = InsertReplacementString(line, p, q, macro, macro);
                if (rv1 < -MACRO_REPLACE_SIZE)
                {
                    if (!tokenized)
                    {
                        if (definitions.size())
                            definitions.pop_back();
                        d->SetPreprocessing(false);
                    }
                    return rv1;
                }
                insize = rv1 - (p - q);
                end += insize;
                p += insize;
                insize = 0;
                tokenPos.newEnd += rv1;
                rv = ReplaceSegment(line, q, p, p, false, definitions, nullptr);
                if (!tokenized)
                {
                    if (definitions.size())
                        definitions.pop_back();
                    d->SetPreprocessing(false);
                }
                if (rv < -MACRO_REPLACE_SIZE)
                {
                    return rv;
                }
                tokenPos.newEnd += rv;
                end += rv;
                insize = 0;
                if (positions)
                    positions->push_back(tokenPos);
            }
            else
            {
                int n = LookupDefault(line, q, p, name);
                if (n)
                {
                    insize = n - (p - q);
                    end += insize;
                    p += insize;
                    insize = 0;
                    tokenPos.newEnd += n;
                    if (positions)
                        positions->push_back(tokenPos);
                }
            }
        }
        else
        {
            p++;
            origPos++;
        }
    }
    pptr = p;
    return end - orig_end;
}

void ppDefine::ParseAsmSubstitutions(std::string& line)
{
    int quote = 0;
    for (int n = 0; n < line.size(); n++)
    {
        if (quote)
        {
            if (line[n] == quote)
                quote = 0;
        }
        else
        {
            if (line[n] == '"' || line[n] == '\'')
            {
                quote = line[n];
            }
            else if (line[n] == ';')
                break;
            else if (line[n] == '%' && n < line.size() - 1)
            {
                bool found = false;
                if (line.size() - 7 >= n && line.substr(n + 1, 6) == "assign")
                {
                    found = true;
                }
                else if (line[n + 1] == '%')
                {
                    int n1 = macro->GetMacroId();
                    if (n1 != -1)
                    {
                        char buf[256];
                        sprintf(buf, "..@%d.", n1);
                        line.replace(n, 2, buf);
                        n += strlen(buf) - 1 - 2;
                    }
                    else
                    {
                        line.erase(n, 1);
                    }
                    found = true;
                }
                else if (n == 0)
                {
                    if ((n < line.size() - 2) && line[n + 1] == '$' && Tokenizer::IsSymbolChar(line.c_str() + n + 2, true))
                    {
                        int n1 = ctx->GetTopId();
                        if (n1 != -1)
                        {
                            char buf[256];
                            sprintf(buf, "..@%d@", n1);
                            line.replace(n, 2, buf);
                            n += strlen(buf) - 2 - 1;
                        }
                        else
                        {
                            line.erase(n, 1);
                            n--;
                        }
                        found = true;
                    }
                }
                if (!found && (n < line.size() - 1) && (isdigit(line[n + 1]) || line[n + 1] == '+' || line[n + 1] == '-'))
                {
                    int mode = 0;
                    int n1 = n + 1;
                    if (line[n1] == '+')
                        mode = 1, n1++;
                    else if (line[n1] == '-')
                        mode = -1, n1++;
                    if (n1 < line.size())
                    {
                        if (!isdigit(line[n1]))
                        {
                            Errors::Error("Invalid macro argument evaluator");
                            break;
                        }
                        else
                        {
                            char* c;
                            int val = std::strtoul(line.c_str() + n1, &c, 10);
                            n1 = c - line.c_str();
                            if (macro->GetMacroId() == -1)
                            {
                                Errors::Error("Macro evaluator used outside macro invocation");
                                break;
                            }
                            else
                            {
                                std::vector<std::string>* args = macro->GetMacroArgs();
                                if (val == 0)
                                {
                                    char buf[256];
                                    sprintf(buf, "%d", (int)args->size());
                                    line.replace(n, n1 - n, buf);
                                    n = n1 - 1;
                                }
                                else
                                {
                                    val--;
                                    if (val >= args->size())
                                    {
                                        if (val > macro->GetMacroMax())
                                        {
                                            Errors::Error("Macro argument evaluator out of range");
                                            break;
                                        }
                                        else
                                        {
                                            line.erase(n, n1 - n);
                                            n--;
                                        }
                                    }
                                    else
                                    {
                                        line.replace(n, n1 - n, (*args)[val]);
                                        n += (*args)[val].length() - (n1 - n) - 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ppDefine::ReplaceAsmMacros(std::string& line)
{

    int n = line.find_first_not_of(" \t\r\v\n");
    while (n != std::string::npos && Tokenizer::IsSymbolChar(line.c_str() + n, true))
    {
        int n1 = n;
        while (n1 != line.size() && Tokenizer::IsSymbolChar(line.c_str() + n1, false))
        {
            int n5 = UTF8::CharSpan(line.c_str() + n1);
            for (int i = 0; i < n5 && n1 < line.size(); i++)
                n1++;
        }
        bool rv;
        if (n1 == line.size())
            rv = macro->Invoke(line.substr(n, n1 - n), std::string(""));
        else
            rv = macro->Invoke(line.substr(n, n1 - n), line.substr(n1));
        if (rv)
        {
            line.erase(n, line.size() - n);
            break;
        }
        n = line.find_first_not_of(" \t\r\n\v:", n1);
    }
}
void ppDefine::replaceDefined(std::string& line)
{

    size_t n = line.find("defined");
    while (n != std::string::npos)
    {
        size_t m = n + 7;
        const char* val = "";
        while (m < line.size() && isspace(line[m]))
            m++;
        bool open = false;
        if (line[m] == '(')
        {
            open = true;
            m++;
        }
        while (m < line.size() && isspace(line[m]))
            m++;
        if (Tokenizer::IsSymbolChar(line.c_str() + m, true))
        {
            int q = m++;
            while (Tokenizer::IsSymbolChar(line.c_str() + m, false))
                m++;
            if (Lookup(line.substr(q, m - q)))
                val = "1";
            else
                val = "0";
        }
        if (open)
        {
            while (m < line.size() && isspace(line[m]))
                m++;
            if (line[m] == ')')
            {
                m++;
            }
        }
        line.replace(n, m - n, val);
        n = line.find("defined", n);
    }
}

int ppDefine::Process(std::string& line, bool leavePlaceholder)
{
    if (asmpp)
    {
        leavePlaceholder = false;
        ParseAsmSubstitutions(line);
    }
    int sv = 0;
    std::deque<Definition*> definitions;
    tokenPositions.clear();
    int rvi = ReplaceSegment(line, 0, line.size(), sv, true, definitions, asmpp ? nullptr : &tokenPositions);
    if (rvi == INT_MIN + 1)
        return rvi;
    std::string rv;
    int p, last = 0;
    int offset = 0;
    TokenPos *current = nullptr, *next = nullptr;
    auto it = tokenPositions.begin();
    if (it != tokenPositions.end())
    {
        current = &(*it);
        ++it;
        if (it != tokenPositions.end())
        {
            next = &(*it);
        }
    }
    for (p = 0; p < line.size(); p++)
    {
        if (next && p >= next->newStart)
        {
            next->newStart -= offset;
            next->newEnd -= offset;
            current = next;
            ++it;
            if (it == tokenPositions.end())
            {
                next = nullptr;
            }
            else
            {
                next = &(*it);
            }
        }
        if (line[p] == REPLACED_TOKENIZING || (!leavePlaceholder && line[p] == MACRO_PLACEHOLDER) || line[p] == REPLACED_ALREADY)
        {
            int pos = p;
            if (p != last)
                rv += line.substr(last, p - last);
            while ((!current || p <= current->newEnd) &&
                   (line[p] == REPLACED_TOKENIZING || (!leavePlaceholder && line[p] == MACRO_PLACEHOLDER) ||
                    line[p] == REPLACED_ALREADY))
                p++;
            last = p--;
            if (current)
                current->newEnd -= p - pos;
            offset += p - pos;
        }
    }
    if (last != p)
        rv += line.substr(last, p - last);
    while (it != tokenPositions.end())
    {
        it->newStart -= offset;
        it->newEnd -= offset;
        it = ++it;
    }
    line = rv;
    if (asmpp)
    {
        ReplaceAsmMacros(line);
    }
    return rvi;
}
void ppDefine::PushPopMacro(std::string name, bool push)
{
    Definition* d = Lookup(name);
    if (d)
    {
        if (push)
        {
            Definition* pushval = new Definition(*d);
            macroStacks[name].push(pushval);
        }
        else
        {
            auto it = macroStacks.find(name);
            if (it != macroStacks.end() && it->second.size())
            {
                Definition* popval = it->second.top();
                it->second.pop();
                *d = *popval;
                delete popval;
            }
        }
    }
}

ppDefine::Definition* ppDefine::Lookup(const std::string& name) { return static_cast<Definition*>(symtab.Lookup(name)); }
