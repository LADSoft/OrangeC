/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/

#define _CRT_SECURE_NO_WARNINGS

#include "ppDefine.h"
#include "Token.h"
#include "Preprocessor.h"
#include "Errors.h"
#include "SymbolTable.h"
#include "UTF8.h"
#include <time.h>
#include <strstream>

static const int ellipses = 100;

ppDefine::ppDefine(bool UseExtensions, ppInclude *Include, bool C89, bool Asmpp) : 
        expr(false), include(Include), c89(C89), asmpp(Asmpp), ctx(NULL), macro(NULL)
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
            return NULL;
        }
    }
    size_t n = value.find_first_not_of(" \t\v\n");
    if (n)
        value.erase(0, n);
    n = value.find_last_not_of(" \t\v\n");
    if (n != std::string::npos && n != value.size() - 1)
        value.erase(n+1);
        
    std::string name1 = name;
    if (caseInsensitive)
        name1 = UTF8::ToUpper(name1);
    Definition *d = new Definition(name1, value, args, permanent);
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
            std::strstream aa;
            aa << (int)n;
            std::string value;
            aa >> value;
            Define(name, value, NULL, false, false, false, caseInsensitive);
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
    DefinitionArgList *da = NULL;
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
            if (!next->IsIdentifier() && next->GetKeyword() != closepa)
            {
                failed = true;
            }
            else
            {
                da = new DefinitionArgList();
                bool done = false;
                while (next->IsIdentifier())
                {
                    da->push_back(next->GetId());
                    next = tk.Next();
                    if (next->GetKeyword() != comma)
                    {
                        break;
                    }
                    next = tk.Next();
                    if (next->GetKeyword() == ellipses)
                    {
                        if (c89)
                            Errors::Error("Macro variable argument specifier only allowed in C99");
                        hasEllipses = true;
                        next = tk.Next();
                        break;
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
    Tokenizer tk(line, NULL);
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
    }
}
int ppDefine::LookupDefault(std::string &macro, int begin, int end, const std::string &name)
{
    std::string insert;
    if (name == "__FILE__")
        insert = include->GetFile();
    else if (name == "__LINE__")
    {
        std::strstream str;
        str << include->GetLineNo();
        str >> insert;
    }
    else if (name == "__DATE__")
        insert = date;
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
    while (j < macroname.size() && (IsSymbolChar(macroname.c_str()+j) || macroname[j] == PP_MASTART || macroname[j] == PP_MAEND))
    {
        int n = UTF8::CharSpan(macroname.c_str() + j);
        if (macroname[j] == PP_MASTART)
        {
            if (IsSymbolChar(macroname.c_str() + j + 1))
                break;
        }
        else
        {
            for (int i=0; i < n && macroname[j]; i++)
                j++;
        }
    }
    if (macroname[j] == PP_MAEND)
        j++;
    std::string rv = macroname.substr(i,j - i);
    char vv[3] = { PP_MASTART, PP_MAEND, 0 };
    size_t n = rv.find(vv);
    while (n != std::string::npos)
    {
        rv.erase(n, 1);
        n = rv.find(vv, n);
    }
    if (inctx)
    {
        int n1 = ctx->GetTopId();
        if (n1 != -1)
        {
            i -= 2 + quoted;
            if (quoted)
                if (macroname[j] != '}')
                    Errors::Error("Macro substition: expected '}'");
                else
                    j++;
            char buf[256];
            sprintf(buf, "..@%d$", n1);
            rv = std::string(buf) + rv;
        }
    }
    return rv;
}



/* The next few functions support recursion blocking for macros.
 * Basicall a list of all active macros is kept and if a lookup would 
 * result in one of those macros, no replacement is done.
 */
void ppDefine::nodefines(void)
{
    defineList.clear();
}

/*-------------------------------------------------------------------------*/

bool ppDefine::indefine(const std::string &name)
{
    return defineList.find(name) != defineList.end();
}
/*-------------------------------------------------------------------------*/

void ppDefine::enterdefine(const std::string &name)
{
    defineList.insert(name);
}

/*-------------------------------------------------------------------------*/

void ppDefine::exitdefine(const std::string &name)
{
    std::set<std::string>::iterator it = defineList.find(name);
    if (it != defineList.end())
        defineList.erase(it);
}

int ppDefine::Stringize(std::string &macro, int begin, int end, const std::string &text)
{
    int rbegin = begin;
    while (begin != 0 && isspace(macro[begin-1]))
        begin--;
    if (begin != 0 && (macro[begin-1] == '#' 
                           || (begin > 1 && macro[begin-2] == '%' && macro[begin-1] == ':')) )
    {
        if (macro[begin-1] == '#')
            begin--;
        else
            begin -=2;
        macro.replace(begin, end - begin, "\"\"");
        std::string temp = text;
        char *vv = "\\\"";
        size_t n = temp.find_first_of(vv);
        while (n != std::string::npos)
        {
            temp.insert(n, "\\");
            n = temp.find_first_of(vv, n+2);
        }
        macro.replace(begin + 1, 0, temp);
        return temp.size() + end - begin;
    }
    else
    {
        macro.replace(rbegin, end - rbegin, text);
        return end - rbegin;
    }
}
void ppDefine::Tokenize(std::string &macro)
{
    size_t n = macro.find_first_of(REPLACED_TOKENIZING);
    while (n != std::string::npos)
    {
        int begin,end;
        begin = end = n;
        while (begin && macro[begin-1] == PP_TOKEN_BREAK || macro[begin-1] == PP_MASTART ||
               macro[begin-1] == PP_MAEND || isspace(macro[begin-1]))
            begin--;
        if (begin && macro[begin-1] == NULL_TOKENIZER)
            begin--;
        while (end < macro.size() && macro[end+1] == PP_TOKEN_BREAK || macro[end++] == PP_MASTART ||
               macro[end] == PP_MAEND || isspace(macro[end]))
            end++;
        if (end < macro.size() && macro[end++] == NULL_TOKENIZER)
            end++;
        for (int i=begin; i < end; i++)
        {
            if (macro[i] != PP_MASTART && macro[i] != PP_MAEND)
            {
                int j = i;
                while (macro[j] != PP_MASTART && macro[j] != PP_MAEND)
                    j++;
                macro.erase(i,j-i);
                end-=j;
            }
        }
        n = macro.find_first_of(REPLACED_TOKENIZING);
    }
}
bool ppDefine::NotSlashed(const std::string &macro, int pos)
{
    int count = 0;
    while (pos && macro[--pos] == '\\')
        count++;
    return !(count & 1);
}
/* replace macro args */
void ppDefine::ReplaceArgs(const std::string id, std::string &macro, 
                 const DefinitionArgList &oldargs, const DefinitionArgList &newArgs, const std::string varargs)
{
    int instring = 0;
    for (int i=0; i < macro.size(); i++)
    {
        if (instring)
        {
            if (macro[i] == instring && NotSlashed(macro, i))
            {
                instring = 0;
            }
        }
        else if (!instring && (macro[i] == '\'' || macro[i] == '"') &&
                 NotSlashed(macro, i))
        {
            instring = macro[i];
        }
        else if (!instring && IsSymbolStartChar(macro.c_str() + i))
        {
            int j = i;
            std::string name = defid(macro, i, j);
            if (!c89 && name == "__VA_ARGS__")
            {
                if (!varargs.size()) 
                {
                    Errors::Error("Incorrect macro arguments");
                }
                else
                {
                    int count = Stringize(macro, i, j, varargs);
                    i += count-1;
                }
            }
            else for (int m = 0; m < oldargs.size(); m++) 
            {
                if (name == oldargs[m])
                {
                    if (newArgs[m].size())
                    {
                        int count = Stringize(macro, i, j, newArgs[m]);
                        i += count-1;
                    }
                    else
                    {
                        static char nullarg[] = { NULL_TOKENIZER, 0 };
                        static char emptyarg[] = { 0 } ;
                        char *arg = emptyarg;
                        int k = i;
                        while (k && isspace(macro[k-1]))
                               k--;
                        if (k && macro[k-1] == REPLACED_TOKENIZING)
                        {
                            arg = nullarg;
                        }
                        else
                        {
                            k = i+1;
                            while (k < macro.size() && isspace(macro[k]))
                                k++;
                            if (k < macro.size() && macro[k] == REPLACED_TOKENIZING)
                                arg = nullarg;
                        }
                        int count = Stringize(macro, i, j, arg);
                        i += count-1;
                    }
                    break;
                }
            }
        }
    }
}
void ppDefine::SyntaxError(const std::string &name)
{
    Errors::Error(std::string("Wrong number of arguments in call to macro ") +name);
}
#include <fstream>

void ppDefine::replacesegment(std::string &line, int begin, int end, bool outer)
{
    int lineno;
    int instring = 0;
    int nestedArg = 0;
    for (int i=0; i < line.size(); i++)
    {
        if (instring)
        {
            if (line[i] == instring && NotSlashed(line, i))
            {
                instring = 0;
            }
        }
        else if (!instring && (line[i] == '\'' || line[i] == '"') &&
                 NotSlashed(line, i))
        {
            instring = line[i];
        }
        else if (line[i] == PP_MASTART)
        {
            nestedArg++;			
        }
        else if (line[i] == PP_MAEND && nestedArg)
        {
            nestedArg--;
        }
        else if (!nestedArg && IsSymbolStartChar(line.c_str()+i))
        {
            int j = i;
            std::string name = defid(line, i, j);
            Symbol *sym = symtab.Lookup(name);
            Definition *d = static_cast<Definition *>(sym);
            if (!d)
            {
                if (asmpp)
                {
                    std::string name1 = UTF8::ToUpper(name);
                    sym = symtab.Lookup(name1);
                    d = static_cast<Definition *>(sym);
                    if (d && d->IsCaseInsensitive())
                    {
                        name = name1;
                        goto join;
                    }
                }
                int n = LookupDefault(line, i, j, name);
                if (n)
                    i += n - 1;
                else
                    i = j;
            }
            else if (d->IsUndefined())
            {
                i = j-1;
            }
            else
            {
join:
                if (indefine(name))
                {
                    char vv1[2] = { PP_MASTART, 0 };
                    char vv2[2] = { PP_MAEND, 0 };
                    std::string temp = std::string(vv1) + name + std::string(vv2);
                    line.replace(i, j - i, temp);
                    i+= temp.size() - (j - i) - 1;
                }
                else
                {
                    std::string changed;
                    int	pos = j;
                    if (d->GetArgCount())
                    {
                        std::string va;
                        DefinitionArgList list;
                        int count = 0;
                        pos = line.find_first_not_of(" \t\n\v", pos);
                        while (pos == std::string::npos)
                        {
                            pos = line.size();
                            std::string newline;
                            if (!include->GetLine(newline, lineno))
                                return;
                            line += newline;
                            pos = line.find_first_not_of(" \t\n\v", pos);
                        }
                        if (line[pos] != '(')
                        {
                            i += LookupDefault(line, i, j, name);
                        }
                        else
                        {
                            pos++;
                            bool done;
                            do
                            {
                                int nestedparen = 0, nestedstring = 0;
                                int pos2 = line.find_first_not_of(" \t\n\v", pos);
                                while (pos2 == std::string::npos)
                                {
                                    pos2 = line.size();
                                    std::string newline;
                                    if (!include->GetLine(newline, lineno))
                                    {
                                        SyntaxError(name);
                                        return;
                                    }
                                    line += newline;
                                    pos2 = line.find_first_not_of(" \t\n\v", pos2);
                                }
                                while (line[pos2] && 
                                       ((line[pos2] != ',' &&  line[pos2] != ')') ||
                                    nestedparen || nestedstring))
                                {
                                    if (nestedstring)
                                    {
                                        if (line[pos2] == nestedstring && NotSlashed(line, pos2))
                                            nestedstring = 0;
                                    }
                                    else if ((line[pos2] == '\'' ||  line[pos2] == '"')  && NotSlashed(line, pos2))
                                        nestedstring =  line[pos2];
                                    else if (line[pos2] == '(')
                                        nestedparen++;
                                    else if (line[pos2] == ')' && nestedparen)
                                        nestedparen--;
                                    pos2++;
                                    if (pos2 == line.size())
                                    {
                                        std::string newline;
                                        if (!include->GetLine(newline, lineno))
                                        {
                                            SyntaxError(name);
                                            return;
                                        }
                                        line += newline;
                                    }
                                }
                                std::string subst = line.substr(pos, pos2-pos);
                                size_t n = subst.find_first_of(PP_TOKEN_BREAK);
                                while (n != std::string::npos)
                                {
                                    subst.erase(n, 1);
                                    n = subst.find_first_of(PP_TOKEN_BREAK);
                                }
                                n = subst.find_last_not_of(" \t\n\v");
                                if (n != std::string::npos && n != subst.size()-1)
                                    subst.erase(n+1);
                                if (!outer)
                                {
                                    replacesegment(subst, 0, subst.size(), false);
                                }
                                list.push_back(subst);
                                count++;
                                pos = pos2;
                                done = line[pos] != ',';
                                if (!done)
                                {
                                    pos++;
                                    if (pos == line.size())
                                    {
                                        std::string newline;
                                        if (!include->GetLine(newline, lineno))
                                        {
                                            SyntaxError(name);
                                            return;
                                        }
                                        line += newline;
                                    }
                                }
                            } while (pos != line.size() && !done && count != d->GetArgCount());
                            if (pos == std::string::npos || line[pos] != ')' || d->GetArgCount())
                            {
                                if (count == d->GetArgCount() && !c89 && d->HasVarArgs()) 
                                {
                                    int nestedparen=0;
                                    int oldpos = pos;
                                    while ((line[pos] != ')' || nestedparen)) {
                                        if (line[pos] == '(')nestedparen++;
                                        if (line[pos] == ')' && nestedparen)
                                            nestedparen--;
                                        pos++;
                                        if (pos == line.size())
                                        {
                                            std::string newline;
                                            if (!include->GetLine(newline, lineno))
                                            {
                                                SyntaxError(name);
                                                return;
                                            }
                                            line += newline;
                                        }
                                    }   
                                    va = line.substr(oldpos, pos-oldpos);
                                }
                                if (line[pos] != ')' || count != d->GetArgCount())
                                {
                                    if (pos == std::string::npos)
                                    {
                                        return;
                                    }
                                    SyntaxError(name);
                                    return;
                                }
                                pos++;
                            }
                            else if (d->HasVarArgs())
                            {
                                SyntaxError(name);
                            }
                            if (outer)
                            {
                                char vv[2] = { PP_TOKEN_BREAK, 0 } ;
                                changed = std::string(vv) + d->GetValue() + std::string(vv);	
                            }
                            else
                            {
                                changed = d->GetValue();
                            }
                            if (count != 0 || va.size())
                                ReplaceArgs(name, changed, *d->GetArgList(), list, va);
                        }
                    } else {
                        if (outer)
                        {
                            char vv[2] = { PP_TOKEN_BREAK, 0 } ;
                            changed = std::string(vv) + d->GetValue() + std::string(vv);
                        }
                        else
                        {
                            changed = d->GetValue();
                        }
                    }
                    Tokenize(changed);
                    int sz = line.size();
                    line.replace(i, pos - i, changed);
                    enterdefine(name);
                    replacesegment(line, i, changed.size() - (pos - i), false);
                    exitdefine(name);
                    i += line.size() - sz - 1;
                }
            }
        }
    }
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
#include <fstream>
void ppDefine::Process(std::string &line)
{	
    char tk[2] = { REPLACED_TOKENIZING, 0 };
    nodefines();
    size_t n = line.find("##");
    while (n != std::string::npos)
    {
        line.replace(n, 2, tk);
        n = line.find("##", n+1);
    }
    n = line.find("%:%:");
    while (n != std::string::npos)
    {
        line.replace(n, 4, tk);
        n = line.find("%:%:", n+1);
    }
    if (asmpp)
    {
        ParseAsmSubstitutions(line);
    }
    replacesegment(line, 0, line.size(), true);
    char buf[4] = { PP_MASTART, PP_MAEND, PP_TOKEN_BREAK };
    n = line.find_first_of(buf);
    while (n != std::string::npos)
    {
        int t = line.find_first_not_of(buf, n);
        line.erase(n, t - n);
        n = line.find_first_of(buf, n);
    }
    if (asmpp)
    {
        ReplaceAsmMacros(line);
    }
}
ppDefine::Definition *ppDefine::Lookup(const std::string &name)
{
    return static_cast<Definition *>(symtab.Lookup(name));
}