/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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
#include "Preprocessor.h"
#include "Errors.h"
#include "SymbolTable.h"
#include "UTF8.h"
#include "Utils.h"
#include <time.h>
#include <limits.h>
#include <stdlib.h>

static const int ellipses = 100;

ppDefine::ppDefine(bool UseExtensions, ppInclude *Include, bool C89, bool Asmpp) : 
        expr(false), include(Include), c89(C89), asmpp(Asmpp), ctx(nullptr), macro(nullptr)
{ 
    SetDefaults(); 
    InitHash(); 
    expr.SetDefine(this); 
}
bool ppDefine::Check(int token, std::string &line)
{
    bool rv = true;
    switch (token)
    {
        case DEFINE:
            DoDefine(line, false);
            break;
        case IDEFINE:
            DoDefine(line, true);
            break;
        case UNDEF:
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
void ppDefine::InitHash()
{
    defTokens["("] = openpa;
    defTokens[")"] = closepa;
    defTokens[","] = comma;
    defTokens["..."] = ellipses;
}
#include <iostream>
ppDefine::Definition *ppDefine::Define(const std::string &name, std::string &value, DefinitionArgList *args, bool permanent, bool varargs, bool errors,
                                       bool caseInsensitive)
{
    if (asmpp)
    {
        ParseAsmSubstitutions(value);
    }
    Symbol *definesym = symtab.Lookup(name);
    Definition *old = static_cast<Definition *>(definesym);
    if (old)
    {
        if (old->IsUndefined()) // undefined forever?
        {
            delete args;
            return nullptr;
        }
    }
    size_t n = value.find_first_not_of(" \t\v\n");
    if (n)
        value.erase(0, n);
    n = value.find_last_not_of(" \t\v\n");
    if (n != std::string::npos && n != value.size() - 1)
        value.erase(n+1);

    int last = 0, pos;
    std::string x;
    int instr = 0;
    for (pos=0; pos < value.size(); pos++)
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
    static char tk[2] = { REPLACED_TOKENIZING, 0 };
    
    n = value.find("##");
    while (n != std::string::npos)
    {
        value.replace(n, 2, tk);
        n = value.find("##", n+1);
    }
    n = value.find("%:%:");
    while (n != std::string::npos)
    {
        value.replace(n, 4, tk);
        n = value.find("%:%:",n+1);
    }
        
    static char mp[2] = { MACRO_PLACEHOLDER, 0 };
    value = std::string(mp) + value + mp;

    std::string name1 = name;
    if (caseInsensitive)
        name1 = UTF8::ToUpper(name1);
    Definition *d = new Definition(name1, value, args, permanent);
    if (value[1] == REPLACED_TOKENIZING)
        d->DefinedError("macro definition begins with tokenizing token");
    else if (value[value.size()-2] == REPLACED_TOKENIZING || value[value.size() - 2] == '#')
        d->DefinedError("macro definition ends with tokenizing or stringizing token");
    d->SetCaseInsensitive(caseInsensitive);
    if (varargs)
        d->SetHasVarArgs();
    if (errors && old)
    {
        bool failed = false;
        if (old->GetArgCount() != d->GetArgCount())
            failed = true;
        else if (old->HasVarArgs() != d->HasVarArgs())
            failed = true;
        else if (old->GetValue() != d->GetValue())
            failed = true;
        else 
        {
            int n = old->GetArgCount();
            for (int i=0; i < n; i++)
                if (old->GetArg(i) != d->GetArg(i))
                {
                    failed = true;
                    break;
                }
        }			
        if (failed)
            d->DefinedError("macro redefinition changes value");
        symtab.Remove(old);
    }
    symtab.Add(d);
    return d;
}
void ppDefine::Undefine(const std::string &name, bool forever)
{
    Symbol *define = symtab.Lookup(name);
    
    if (define)
    {
        if (forever)
        {
            Definition *d = static_cast<Definition *>(define);
            d->Undefine();
        }
        else
        {
            symtab.Remove(define);
        }
    }
}
void ppDefine::DoAssign(std::string &line, bool caseInsensitive)
{
    bool failed = false;
    Tokenizer tk(line, &defTokens);
    const Token *next = tk.Next();
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
void ppDefine::DoDefine(std::string &line, bool caseInsensitive)
{
    bool inctx = false;
    if (asmpp)
    {
        int n = line.find_first_not_of(" \t\r\n\v");
        if (n != std::string::npos)
        {
            if (n < line.size()-2)
            {
                if (line[n] == '%' && line[n+1] == '$')
                {
                    inctx = true;
                    line.erase(0, n+2);
                }
            }
        }
    }
    Tokenizer tk(line, &defTokens);
    const Token *next = tk.Next();
    bool failed = false;
    bool hasEllipses = false;
    DefinitionArgList *da = nullptr;
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
        if (tk.GetString().c_str()[0] == '(') // yes it HAS to be the first character, no spaces
                                  // or other characters allowed
        {
			// the below is ok because the first one gets the '(' and the next one gets the next token
            next = tk.Next(); // get '('
            next = tk.Next(); // past '(' 
            if (!next->IsIdentifier() && next->GetKeyword() != closepa && next->GetKeyword() != ellipses)
            {
                failed = true;
            }
            else
            {
                bool hascomma = true;
                da = new DefinitionArgList();
                bool done = false;
                while (next->IsIdentifier())
                {
                    da->push_back(next->GetId());
                    next = tk.Next();
                    if (next->GetKeyword() != comma)
                    {
                        hascomma = false;
                        break;
                    }
                    next = tk.Next();
                }
                if (hascomma) 
                { 
                    if (next->GetKeyword() == ellipses)
                    {
                        if (c89)
                            Errors::Error("Macro variable argument specifier only allowed in C99");
                        hasEllipses = true;
                        next = tk.Next();
                    }
                }
                if (next->GetKeyword() != closepa)
                {
                    failed = true;
                }
            }
        }
    }
    if (!failed)
        Define(name, tk.GetString(), da, false, hasEllipses, !asmpp, caseInsensitive);
    else
        Errors::Error("Macro argument syntax error");
}
void ppDefine::DoUndefine(std::string &line)
{
    Tokenizer tk(line, nullptr);
    const Token *t = tk.Next();
    if (!t->IsIdentifier())
    {
        Errors::Error("Expected symbol to undefine");
    }
    else
    {
        Undefine(t->GetId(), false);
    }
}
void ppDefine::SetDefaults()
{
    char string[256];
    struct tm *t1;
    time_t t2;
    t2 = ::time(0);
    t1 = localtime(&t2);
    if (t1)
    {
        strftime(string, 40, "\"%b %d %Y\"", t1);
        date = string;
        strftime(string, 40, "\"%H:%M:%S\"", t1);
        time = string;
        strftime(string, 40, "\"%Y-%m-%d\"", t1);
        dateiso = string;
    }
}
int ppDefine::LookupDefault(std::string &macro, int begin, int end, const std::string &name)
{
    std::string insert;
    if (name == "__FILE__")
        insert = std::string("\"") + include->GetFile() + "\"";
    else if (name == "__LINE__")
    {
        insert = Utils::NumberToString(include->GetLineNo());
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
std::string ppDefine::defid(const std::string &macroname, int &i, int &j)
/*
 * Get an identifier during macro replacement
 */
{
//    char name[2048];
    bool inctx = false;
    bool quoted = false;
    if (asmpp && j >= 2 && macroname[j-1] == '$')
    {
        if (macroname[j-2] == '%')
        {
            inctx = true;
        }
        else if (j >=3 && macroname[j-2] == '{' && macroname[j-3] == '%')
        {
            quoted = true;
            inctx = true;
        }
    }
    while (j < macroname.size() && IsSymbolChar(macroname.c_str()+j) )
    {
        int n = UTF8::CharSpan(macroname.c_str() + j);
        for (int i=0; i < n && macroname[j]; i++)
            j++;
    }
    std::string rv = macroname.substr(i,j - i);
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
void ppDefine::Stringize(std::string &macro)
{
    std::string repl;
    int waiting = 0;
    int last = 0, pos;
    for (pos = 0; pos < macro.size(); pos++)
    {
        if (!waiting && (macro[pos] == '"' ||  macro[pos] == '\'') && NotSlashed(macro, pos))
        {
            waiting =  macro[pos];
        }
        else if (waiting)
        {
            if (macro[pos] == waiting && NotSlashed(macro, pos))
                waiting = 0;
        }
        else if (macro[pos] == '#' && (pos == 0 || macro[pos-1] != '#') && (pos == macro.size()-1 || macro[pos+1] != '#'))  /* # ## # */
        {
            repl += macro.substr(last, pos);
            
            pos++;
            for ( ; pos < macro.size() && isspace(macro[pos]); ++pos) ;
            static char mp[2] = { STRINGIZING_PLACEHOLDER, 0 };
            size_t n = macro.find(mp, pos);
            if (n == std::string::npos)
                n = macro.size();
            std::string candidate = macro.substr(pos, n - pos);
            pos = n-1;
            last = n;
            if (n != macro.size())
                last++;
            n = candidate.find_first_of("\\\"");
            while (n != std::string::npos)
            {
                candidate.insert(n, "\\");
                n = candidate.find_first_of("\\\"", n+2);
            }
            n = candidate.find_first_of(" \t\v\n");
            while (n != std::string::npos)
            {
                size_t m = n+1;
                while (isspace(candidate[m])) m++;
                candidate.replace(n, m-n, " ");
                n = candidate.find_first_of(" \t\v\n", n+1);
            }
            repl += std::string("\"") + candidate + "\"";
        }
    }	
    if (last < macro.size())
        repl += macro.substr(last);
    macro = repl;
}
void ppDefine::Tokenize(std::string &macro)
{
    int waiting = 0;
    for (int i=0; i < macro.size(); i++)
    {
        if (!waiting && (macro[i] == '"' 
            ||  macro[i] == '\'') && NotSlashed(macro, i))
        {
            waiting =  macro[i];
        }
        else if (waiting)
        {
            if (macro[i] == waiting && NotSlashed(macro, i))
                waiting = 0;
        }
        else if (macro[i] == REPLACED_TOKENIZING)
        {
            int b = i, e = i;
            while (b > 0 && (isspace(macro[b-1]) || macro[b-1] == MACRO_PLACEHOLDER))
                b--;
                
                
            while (++e < macro.size () && (isspace(macro[e]) || macro[e] == MACRO_PLACEHOLDER));
            if (b > 0 && macro[b-1] == TOKENIZING_PLACEHOLDER && macro[e] != TOKENIZING_PLACEHOLDER)
                b--;
            if (e < macro.size() && macro[e] == TOKENIZING_PLACEHOLDER)
            {
                e++;
            }
            macro.erase(b, e-b);
            
            i = b-1;
        }
    }
}
int ppDefine::InsertReplacementString(std::string &macro, int end, int begin, std::string text, std::string etext)
{
    int q;
    static char nullptrTOKEN[] = { TOKENIZING_PLACEHOLDER, 0 };
    static char STRINGIZERTOKEN[] = { STRINGIZING_PLACEHOLDER, 0 };
    int  p, r;
    int val;
    int stringizing = false;
    q = end;
    while (q < macro.size()-1 && isspace(macro[q])) q++;
    if (macro[q] == REPLACED_TOKENIZING)
    {
        if (!text.size())
        {
            text = nullptrTOKEN;
        }
    }
    else
    {
        q = begin-1;
        while (q > 0 && isspace(macro[q])) q--;
        if (macro[q] == REPLACED_TOKENIZING)
        {
            if (!text.size())
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
    if (stringizing)
        text = text + STRINGIZERTOKEN;
    macro.replace(begin, end-begin, text);
    return text.size();
}
bool ppDefine::NotSlashed(const std::string &macro, int pos)
{
    int count = 0;
    while (pos && macro[--pos] == '\\')
        count++;
    return !(count & 1);
}
bool ppDefine::ppNumber(const std::string &macro, int start, int pos)
{
    int x = pos;
    if (macro[pos] == '+' || macro[pos] == '-' || isdigit(macro[pos])) // we would get here with the first alpha char following the number
    {
        // backtrack through all characters that could possibly be part of the number
        while (pos >= start &&
               (IsSymbolChar(macro.c_str() + pos) || macro[pos] == '.' ||
               ((macro[pos] == '-' || macro[pos] == '+') 
                && (macro[pos-1] == 'e' || macro[pos-1] == 'E' 
                    || macro[pos-1] == 'p' || macro[pos-1] == 'P'))))
        {
            if (macro[pos] == '-' || macro[pos] == '+') pos--;
            pos--;
        }
        // go forward, skipping sequences that couldn't actually start a number
        pos++;
        if (!isdigit(macro[pos]))
        {
            while (pos < x && (macro[pos] != '.' || isdigit(macro[pos-1]) || !isdigit(macro[pos+1]))) pos++;
        }
        // if we didn't get back where we started we have a number
        return pos < x && (macro[pos] != '0' || (macro[pos+1] != 'x' && macro[pos+1] != 'X')) ;
    }
    return false;
}
/* replace macro args */
bool ppDefine::ReplaceArgs(std::string &macro, 
                 const DefinitionArgList &oldargs, const DefinitionArgList &newargs,
                 const DefinitionArgList &expandedargs,  const std::string varargs)
{
    std::string name;
    int waiting = 0;
    for (int p =0; p < macro.size(); p++)
    {
        if (!waiting && (macro[p] == '"' 
            ||  macro[p] == '\'') && NotSlashed(macro, p))
        {
            waiting =  macro[p];
        }
        else if (waiting)
        {
            if (macro[p] == waiting && NotSlashed(macro, p))
                waiting = 0;
        }
        else if (IsSymbolChar(macro.c_str() + p))
        {
            int q = p;
            name = defid(macro, q ,p);
            if (!c89 && name == "__VA_ARGS__")
            {
                if (!varargs.size()) {
                    int rv;
                    if ((rv = InsertReplacementString(macro, p, q, "", ""))
                            < -MACRO_REPLACE_SIZE)
                        return (false);
                    else
                        p = q + rv-1;
                }
                else
                {
                    int rv;
                    if ((rv = InsertReplacementString(macro, p, q, varargs, varargs))
                            < -MACRO_REPLACE_SIZE)
                        return (false);
                    else
                        p = q + rv-1;
                }
            }
            else for (int i = 0; i < oldargs.size(); i++) 
            {
                if (name == oldargs[i])
                {
                    int rv;
                    if ((rv = InsertReplacementString(macro, p, q, newargs[i], expandedargs[i])) 
                        < - MACRO_REPLACE_SIZE)
                        return (false);
                    else
                    {
                        p = q + rv-1;
                        break;
                    }
                }
            }
        }
    }
    return (true);
}
void ppDefine::SyntaxError(const std::string &name)
{
    Errors::Error(std::string("Wrong number of arguments in call to macro ") +name);
}

void ppDefine::SetupAlreadyReplaced(std::string &macro)
{
    int instr = false;
    for (int p= 0; p < macro.size(); p++)        
    {
        if ((macro[p] == '"' || macro[p] == '\'') && NotSlashed(macro, p))
            instr = !instr;
        if (IsSymbolStartChar(macro.c_str() + p) && !instr)
        {
            int q = p;
            std::string name;
            name = defid(macro, q, p);
            Symbol *sym = symtab.Lookup(name);
            Definition *d = static_cast<Definition *>(sym);
            if (d && d->IsPreprocessing())
            {
                static char ra[2] = { REPLACED_ALREADY, 0 }; 
                macro.insert(q, ra);
            }
            else
            {
                p--;
            }
        }
    }	
}
int ppDefine::ReplaceSegment(std::string &line, int begin, int end, int &pptr)
{
    std::string name;
    int waiting = 0;
    int orig_end = end;
    int rv;
    int size;
    int p;
    int insize,rv1;
    for (p=begin; p < end; p++)
    {
        int q = p;
        if (!waiting && (line[p] == '"' 
            ||  line[p] == '\'') && NotSlashed(line, p))
        {
            waiting =  line[p];
        }
        else if (waiting)
        {
            if (line[p] == waiting && NotSlashed(line, p))
                waiting = 0;
        }
        else if (IsSymbolStartChar(line.c_str() + p) && (p == begin || line[p-1] == '$' || !IsSymbolChar(line.c_str() + p - 1)))
        {
            name = defid(line, q, p);
            Symbol *sym = symtab.Lookup(name);
            Definition *d = static_cast<Definition *>(sym);
            if (!d && asmpp)
            {
                std::string name1 = UTF8::ToUpper(name);
                sym = symtab.Lookup(name1);
                d = static_cast<Definition *>(sym);
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
                d != nullptr && !d->IsUndefined() && 
                (!q || line[q-1] != REPLACED_ALREADY) && !ppNumber(line, q, p-1))
            {
                std::string macro;
                if (d->GetArgList() != nullptr)
                {
                    int q1 = p;
                    int count = 0;
                    
                    
                    while (q1 < line.size()&& (isspace(line[q1]) || line[q1] == MACRO_PLACEHOLDER)) q1++ ;
                    if (q1 == line.size())
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
                            p = q1-1;
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
                            while (p < line.size() && isspace(line[p])) p++;
                            while (p < line.size()  && (((line[p] != ',' &&  line[p] != ')') ||
                                nestedparen || nestedstring) &&  line[p] != '\n'))
                            {
                                if (nestedstring)
                                {
                                    if (line[p] == nestedstring && NotSlashed(line, p))
                                        nestedstring = 0;
                                }
                                else if ((line[p] == '\'' ||  line[p] == '"') 
                                    && NotSlashed(line, p))
                                    nestedstring =  line[p];
                                else if (line[p] == '(')
                                    nestedparen++;
                                else if (line[p] == ')' && nestedparen)
                                    nestedparen--;
                                p++;
                            }
                            q1 = p;
                            while (q1 && isspace(line[q1-1])) q1--;
                            std::string temp = line.substr(pb, q1 - pb);
                            args.push_back(temp);
                            expandedargs.push_back(temp);
                            int sv;
                            rv = ReplaceSegment(expandedargs[count], 0, expandedargs[count].size(), sv);
                            if (rv <-MACRO_REPLACE_SIZE) {
                                return rv;
                            }
                            count++;
                        }
                        while (line[p] && line[p++] == ',' && count != d->GetArgCount())
                            ;
                    }
                    else 
                    {
                        count = 0;
                        while (p < line.size()-1 && isspace(line[p])) p++;
                        if (line[p] == ')')
                            p++;
                    }
                    if (line[p - 1] != ')' || count != d->GetArgCount())
                    {
                        if (count == d->GetArgCount() && !c89 && d->HasVarArgs()) 
                        {
                            q1 = p;
                            int nestedparen=0;
                            while (p < line.size() && (line[p] != ')' || nestedparen)) {
                                if (line[p] == '(')nestedparen++;
                                if (line[p] == ')' && nestedparen)
                                    nestedparen--;
                                p++; 
                            }
                            varargs = line.substr(q1, p - q1);
                            p++ ;
                        }
                        if (line[p - 1] != ')' || count != d->GetArgCount())
                        {
                            if (p >= line.size())
                            {
                                // continues on the next line, get more text
                                return  INT_MIN + 1;
                            }
                            SyntaxError(name);
                            return  INT_MIN;
                        }
                    }

                    macro = d->GetValue();
                    if (count != 0 || varargs.size())
                        if (!ReplaceArgs(macro, *d->GetArgList(), args, expandedargs, varargs))
                            return  INT_MIN;
                    Tokenize(macro);
                    Stringize(macro);
                    static char tk[2] = { TOKENIZING_PLACEHOLDER, 0 };
                    size_t n = macro.find(tk);
                    while (n != std::string::npos)
                    {
                        macro.erase(n, 1);
                        n = macro.find(tk,n);
                    }
                } else {
                    macro = d->GetValue();
                }
                d->SetPreprocessing(true);
                SetupAlreadyReplaced(macro);
                rv1 = InsertReplacementString(line, p, q, macro, macro);
                if (rv1 < -MACRO_REPLACE_SIZE)
                {
                    d->SetPreprocessing(false);
                    return  rv1;
                }
                insize = rv1 - (p - q);
                end += insize;
                p += insize;
                insize = 0;
                rv = ReplaceSegment(line, q, p, p);
                d->SetPreprocessing(false);
                if (rv <-MACRO_REPLACE_SIZE) {
                    return rv;
                }
                end += rv;
                insize = 0;
            }
            else
            {
                int n = LookupDefault(line, q, p, name);
                if (n)
                    p = q + n - 1;
                else
                    p = q;
            }
        }
    }
    pptr = p;
    return end - orig_end ;
}

void ppDefine::ParseAsmSubstitutions(std::string &line)
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
                if (line.size() - 7 >= n && line.substr(n+1, 6) == "assign")
                {
                    found = true;
                }
                else if (line[n+1] == '%')
                {
                    int n1 =macro->GetMacroId();
                    if (n1 != -1)
                    {
                        char buf[256];
                        sprintf(buf, "..@%d.", n1);
                        line.replace(n, 2, buf);
                        n += strlen(buf)-1-2;
                    }
                    else
                    {
                        line.erase(n, 1);
                    }
                    found = true;
                }
                else if (n == 0)
                {
                    if ((n < line.size()-2) && line[n+1] == '$' && IsSymbolStartChar(line.c_str() + n+2))
                    {
                        int n1 = ctx->GetTopId();
                        if (n1 != -1)
                        {
                            char buf[256];
                            sprintf(buf, "..@%d@", n1);
                            line.replace(n, 2, buf);
                            n += strlen(buf)-2-1;
                        }
                        else
                        {
                            line.erase(n, 1);
                            n--;
                        }
                        found = true;
                    }					
                }
                if (!found && (n < line.size()-1) &&  (isdigit(line[n+1]) || line[n+1] == '+' || line[n+1] == '-'))
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
                            char *c;
                            int val = strtoul(line.c_str() + n1, &c, 10);
                            n1 = c - line.c_str();
                            if (macro->GetMacroId() == -1)
                            {
                                Errors::Error("Macro evaluator used outside macro invocation");
                                break;
                            }
                            else
                            {
                                std::vector<std::string> *args = macro->GetMacroArgs();
                                if (val == 0)
                                {
                                    char buf[256];
                                    sprintf(buf,"%d", (int)args->size());
                                    line.replace(n, n1-n, buf);
                                    n = n1 -1;
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
                                            line.erase(n, n1-n);
                                            n--;
                                        }
                                    }
                                    else
                                    {
                                        line.replace(n, n1-n, (*args)[val]);
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

void ppDefine::ReplaceAsmMacros(std::string &line)
{
    
    int n = line.find_first_not_of(" \t\r\v\n");
    while (n != std::string::npos && IsSymbolStartChar(line.c_str() + n))
    {
        int n1 = n;
        while (n1 != line.size() && IsSymbolChar(line.c_str() + n1)) 
        {
            int n5 = UTF8::CharSpan(line.c_str() + n1);
            for (int i=0; i < n5 && n1 < line.size(); i++)
                n1++;
        }
        bool rv;
        if (n1 == line.size())
            rv = macro->Invoke(line.substr(n,n1-n), std::string(""));
        else
            rv = macro->Invoke(line.substr(n, n1-n), line.substr(n1));
        if (rv)
        {
            line.erase(n, line.size()-n);
            break;
        }
        n = line.find_first_not_of(" \t\r\n\v:",n1);
    }
}
void ppDefine::replaceDefined(std::string &line)
{
    
    size_t n = line.find("defined");
    while (n != std::string::npos)
    {
        size_t m = n + 7;
        char *val = "";
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
        if (IsSymbolStartChar(line.c_str() + m))
        {
            int q = m++;
            while (IsSymbolChar(line.c_str() + m))
                m++;
            if (Lookup(line.substr(q, m-q)))
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
        line.replace(n,m-n, val);
        n = line.find("defined", n);
    }
}

int ppDefine::Process(std::string &line)
{	
    if (asmpp)
    {
        ParseAsmSubstitutions(line);
    }
    int sv = 0;
    int rvi = ReplaceSegment(line, 0, line.size(), sv);
    if (rvi == INT_MIN + 1)
        return rvi;
    std::string rv;
    int p, last = 0;
    for (p=0; p < line.size(); p++)
    {
        if (line[p] == REPLACED_TOKENIZING || line[p] == MACRO_PLACEHOLDER || line[p] == REPLACED_ALREADY)
        {
            if (p != last)
                rv += line.substr(last, p - last);
            while (line[p] == REPLACED_TOKENIZING || line[p] == MACRO_PLACEHOLDER || line[p] == REPLACED_ALREADY) p++;
            last = p;
        }
    }
    if (last != p)
        rv += line.substr(last, p- last);
    line = rv;
    if (asmpp)
    {
        ReplaceAsmMacros(line);
    }
    return rvi;
}
ppDefine::Definition *ppDefine::Lookup(const std::string &name)
{
    return static_cast<Definition *>(symtab.Lookup(name));
}