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
#include "ppCond.h"
#include "Errors.h"
#include "PreProcessor.h"
#include "UTF8.h"
#include <ctype.h>

ppCond::~ppCond()
{
    if (current)
        delete current;
    while (skipList.size())
    {
        current = skipList.front();
        skipList.pop_front();
        delete current;
    }
}
bool ppCond::Check(int token, const std::string &line, int lineno)
{
    std::string line1 = line;
    switch(token)
    {
        case ELSE:
            HandleElse(line1);
            break;
        case ENDIF:
            HandleEndIf(line1);
            break;
        case IF:
            define->replaceDefined(line1);
            define->Process(line1);
            HandleIf(expr.Eval(line1), line1, lineno);
            break;
        case ELIF:
            define->replaceDefined(line1);
            define->Process(line1);
            HandleElif(expr.Eval(line1), line1);
            break;
        case IFDEF:
            HandleDef(line1, false, false, lineno);
            break;
        case IFNDEF:
            HandleDef(line1, false, true, lineno);
            break;
        case ELIFDEF:
            HandleDef(line1, true, false, lineno);
            break;
        case ELIFNDEF:
            HandleDef(line1, true, true, lineno);
            break;
        case IFIDN:
            HandleIdn(line1, false, false, true, lineno);
            break;
        case ELIFIDN:
            HandleIdn(line1, true, false, true, lineno);
            break;
        case IFNIDN:
            HandleIdn(line1, false, true, true, lineno);
            break;
        case ELIFNIDN:
            HandleIdn(line1, true, true, true, lineno);
            break;
        case IFIDNI:
            HandleIdn(line1, false, false, false, lineno);
            break;
        case ELIFIDNI:
            HandleIdn(line1, true, false, false, lineno);
            break;
        case IFNIDNI:
            HandleIdn(line1, false, true, false, lineno);
            break;
        case ELIFNIDNI:
            HandleIdn(line1, true, true, false, lineno);
            break;
        case IFID:
            HandleId(line1, false, false, lineno);
            break;
        case ELIFID:
            HandleId(line1, true, false, lineno);
            break;
        case IFNID:
            HandleId(line1, false, true, lineno);
            break;
        case ELIFNID:
            HandleId(line1, true, true, lineno);
            break;
        case IFNUM:
            HandleNum(line1, false, false, lineno);
            break;
        case ELIFNUM:
            HandleNum(line1, true, false, lineno);
            break;
        case IFNNUM:
            HandleNum(line1, false, true, lineno);
            break;
        case ELIFNNUM:
            HandleNum(line1, true, true, lineno);
            break;
        case IFSTR:
            HandleStr(line1, false, false, lineno);
            break;
        case ELIFSTR:
            HandleStr(line1, true, false, lineno);
            break;
        case IFNSTR:
            HandleStr(line1, false, true, lineno);
            break;
        case ELIFNSTR:
            HandleStr(line1, true, true, lineno);
            break;
        case IFCTX:
            HandleCtx(line1, false, false, lineno);
            break;
        case ELIFCTX:
            HandleCtx(line1, true, false, lineno);
            break;
        case IFNCTX:
            HandleCtx(line1, false, true, lineno);
            break;
        case ELIFNCTX:
            HandleCtx(line1, true, true, lineno);
            break;
        default:
            return current && current->skipping;
    }
    return true;
}
void ppCond::HandleIf(int val, const std::string &line, int lineno)
{
    skip *old = current;
    if (current)
        skipList.push_front(current);
    current = new skip;
    current->line = lineno;
    if (old && old->skipping)
    {
        current->skipping = true;
    }
    else if (!val)
    {
        current->skipping = true;
        current->takeElse = true;
    }
}	
void ppCond::HandleElif(int val, const std::string &line)
{
    ansieol(line);
    if (!current || current->elseSeen)
    {
        Errors::Error("Misplaced elif directive");
    }
    else 
    {
        skip *old = NULL;
        if (skipList.size())
            old = skipList.front();
        if ((!old || !old->skipping) && current->takeElse && val)
        {
            current->skipping = false;
            current->takeElse = false;
        }
        else				
        {
            current->skipping = true;
        }
    }
}
void ppCond::HandleElse(std::string &line)
{
    ansieol(line);
    if (!current)
    {
        Errors::Error("Misplaced else directive");
    }
    else
    {
        if (current->takeElse)
        {
            current->skipping = false;
        }
        else	
        {
            current->skipping = true;
        }
        if (current->elseSeen)
        {
            Errors::Error("Misplaced else directive");
        }
        current->elseSeen = true;
    }
}
void ppCond::HandleEndIf(std::string &line)
{
    ansieol(line);
    if (!current)
    {
        Errors::Error("Misplaced endif directive");			
    }
    else
    {
        delete current;
        if (skipList.size())
        {
            current = skipList.front();
            skipList.pop_front();
        }
        else
        {
            current = NULL;
        }
    }
}
void ppCond::HandleDef(std::string &line, bool Else, bool negate, int lineno)
{
    Tokenizer tk(line, NULL);
    const Token *t = tk.Next();
    if (!t->IsIdentifier())
    {
        Errors::Error("Identifier expected");
        if (Else)
            HandleElif(false,"");
        else
            HandleIf(false,"", lineno);
    }
    else
    {
        int v = !!define->Lookup(t->GetId());
        if (asmpp && !v)
        {
            std::string one = UTF8::ToUpper(t->GetId());
            Symbol *t = define->Lookup(one);
            if (t)
            {
                ppDefine::Definition *p = static_cast<ppDefine::Definition *>(t);
                v = p->IsCaseInsensitive();
            }
        }
        if (negate)
            v = !v;
        if (Else)
            HandleElif(v,tk.GetString());
        else
            HandleIf(v, tk.GetString(), lineno);
    }
}
void ppCond::HandleIdn(std::string &line, bool Else, bool negate, bool caseSensitive, int lineno)
{
    define->Process(line);
    size_t n = line.find_first_of(',');
    if (n == std::string::npos)
    {
        Errors::Error("Syntax error in string comparison directive");
        if (Else)
            HandleElif(false, "");
        else
            HandleIf(false,"", lineno);
    }
    else
    {
        std::string line1 = line.substr(0, n);
        line.erase(0, n+1);
        n = line.find_first_not_of(" \t\v\n");
        if (n && n != std::string::npos)
            line.erase(0, n);
        n = line.find_last_not_of(" \t\v\n");
        if (n != std::string::npos && n != line.size()-1)
            line.erase(n+1, line.size()-n-1);
        n = line1.find_first_not_of(" \t\v\n");
        if (n && n != std::string::npos)
            line1.erase(0, n);
        n = line1.find_last_not_of(" \t\v\n");
        if (n != std::string::npos && n != line1.size()-1)
            line1.erase(n+1, line1.size()-n-1);
        int pos=0, pos1 = 0;
        int z = 0;
        if (!caseSensitive)
        {
            line = UTF8::ToUpper(line);
            line1 = UTF8::ToUpper(line);
        }
        while (true)
        {
            if (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\v' || line[pos] == '\n')
            {
                z ^= 1;
                pos = line.find_first_not_of(" \t\v\n", pos);
                if (pos == std::string::npos)
                    pos = line.size();
            }
            if (line1[pos1] == ' ' || line1[pos1] == '\t' || line1[pos1] == '\v' || line1[pos1] == '\n')
            {
                z ^= 1;
                pos1 = line1.find_first_not_of(" \t\v\n", pos);
                if (pos1 == std::string::npos)
                    pos1 = line1.size();
            }
            if (z || pos == line.size() || pos1 == line1.size())
                break;
            if (line[pos] != line1[pos1])
                break;
            pos++, pos1++;
            if (pos == line.size() || pos1 == line1.size())
                break;
        }
        int v = !z && pos == line.size() && pos1 == line1.size();
        if (negate)
            v = !v;
        if (Else)
            HandleElif(v,"");
        else
            HandleIf(v, "", lineno);
    }
}
void ppCond::HandleId(std::string &line, bool Else, bool negate, int lineno)
{
    define->Process(line);
    Tokenizer tk(line, NULL);
    const Token *t = tk.Next();
    int v = t->IsIdentifier();
    if (negate)
        v = !v;
    if (Else)
        HandleElif(v, tk.GetString());
    else
        HandleIf(v, tk.GetString(), lineno);
}
void ppCond::HandleNum(std::string &line, bool Else, bool negate, int lineno)
{
    define->Process(line);
    Tokenizer tk(line, NULL);
    const Token *t = tk.Next();
    int v = t->IsNumeric();
    if (negate)
        v = !v;
    if (Else)
        HandleElif(v, tk.GetString());
    else
        HandleIf(v, tk.GetString(), lineno);
}
void ppCond::HandleStr(std::string &line, bool Else, bool negate, int lineno)
{
    define->Process(line);
    Tokenizer tk(line, NULL);
    const Token *t = tk.Next();
    int v = t->IsString();
    if (negate)
        v = !v;
    if (Else)
        HandleElif(v, tk.GetString());
    else
        HandleIf(v, tk.GetString(), lineno);
}
void ppCond::HandleCtx(std::string &line, bool Else, bool negate, int lineno)
{
    define->Process(line);
    Tokenizer tk(line, NULL);
    const Token *t = tk.Next();
    int v = false;
    if (t->IsIdentifier())
    {
        v = ctx.Matches(t->GetId());
        if (negate)
            v = !v;
    }
    else
    {
        Errors::Error("Identifier expected");
    }
    if (Else)
        HandleElif(v, tk.GetString());
    else
        HandleIf(v, tk.GetString(), lineno);
}
void ppCond::ansieol(const std::string &line)
{
    if (!extensions)
    {
        const char *p = line.c_str();
        while (*p)
        {
            if (!isspace(*p))
            {
                Errors::Error("Extra characters past end of preprocessor directive");
                break;
            }
            p++;
        }
    }
}
void ppCond::CheckErrors()
{
    if (skipList.size() || current)
    {		
        Errors::Error("Non-terminated preprocessor conditional started in line " + Errors::ToNum(current->line));
    }
}
