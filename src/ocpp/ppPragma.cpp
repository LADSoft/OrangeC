/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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

#include "ppPragma.h"
#include "PreProcessor.h"
#include "Errors.h"

Packing *Packing::instance;
FenvAccess *FenvAccess::instance;
CXLimitedRange *CXLimitedRange::instance;
FPContract *FPContract::instance;
Libraries *Libraries::instance;
Aliases *Aliases::instance;
Startups *Startups::instance;

void ppPragma::InitHash()
{
    hash["("] = openpa;
    hash[")"] = closepa;
    hash[","] = comma;
    hash["="] = eq;
}
bool ppPragma::Check(int token, const std::string &args)
{
    if (token == PRAGMA)
    {
        ParsePragma(args);
        return true;
    }
    return false;
}
void ppPragma::ParsePragma( const std::string &args)
{
    Tokenizer tk(args, &hash);
    const Token *id = tk.Next();
    if (id->IsIdentifier())
    {
        if (*id == "STDC")
            HandleSTDC(tk);
        else if (*id == "AUX")
            HandleAlias(tk);
        else if (*id == "PACK")
            HandlePack(tk);
        else if (*id == "LIBRARY")
            HandleLibrary(tk);
        else if (*id == "STARTUP")
            HandleStartup(tk);
        else if (*id == "RUNDOWN")
            HandleRundown(tk);
        else if (*id == "WARNING")
            HandleWarning(tk);
        else if (*id == "ERROR")
            HandleError(tk);
        else if (*id == "FARKEYWORD")
            HandleFar(tk);
        // unmatched is not an error
            
    }
}
void ppPragma::HandleSTDC(Tokenizer &tk)
{
    const Token *token = tk.Next();
    if (token && token->IsIdentifier())
    {
        std::string name = token->GetId();
        const Token *tokenCmd = tk.Next();
        if (tokenCmd && tokenCmd->IsIdentifier())
        {
            const char *val = tokenCmd->GetId().c_str();
            bool on;
            bool valid = false;
            if (!strcmp(val, "ON"))
            {
                valid = true;
                on = true;
            }
            else if (!strcmp(val, "OFF"))
            {
                valid = true;
                on = false;
                
            }
            if (valid)
            {
                if (name == "FENV_ACCESS")
                    FenvAccess::Instance()->Add(on);
                else if (name == "CX_LIMITED_RANGE")
                    CXLimitedRange::Instance()->Add(on);
                else if (name == "FP_CONTRACT")
                    FPContract::Instance()->Add(on);
            }
        }
    }
}
void ppPragma::HandlePack(Tokenizer &tk)
{
    const Token *tok = tk.Next();
    if (tok && tok->GetKeyword() == openpa)
    {
        tok = tk.Next();
        int val = -1;
        if (tok)
            if (tok->IsNumeric())
            {
                val = tok->GetInteger();
            }
        tok = tk.Next();
        if (tok)
            if (tok->GetKeyword() == closepa)
            {
                if (val <= 0)
                {
                    Packing::Instance()->Remove();
                }
                else
                {
                    Packing::Instance()->Add(val);
                }
            }
    }
}
void ppPragma::HandleError(Tokenizer &tk)
{
    Errors::Error(tk.GetString());
}
void ppPragma::HandleWarning(Tokenizer &tk)
{
    // check for microsoft warning pragma
    const char *p = tk.GetString().c_str();
    while (isspace(*p))
        p++;
    if (*p != '(')
        Errors::Warning(p);
}
void ppPragma::HandleSR(Tokenizer &tk, bool startup)
{
    const Token *name = tk.Next();
    if (name && name->IsIdentifier())
    {
        std::string id = name->GetId();
        const Token *prio = tk.Next();
        if (prio && prio->IsNumeric() && !prio->IsFloating())
        {
            Startups::Instance()->Add(id, prio->GetInteger(), startup);
        }
    }
}
Startups::~Startups()
{
    for (StartupIterator it = begin(); it != end(); ++it)
    {
        Properties *x = it->second;
        delete x;
    }
    list.clear();
}
void ppPragma::HandleLibrary(Tokenizer &tk)
{
    char buf[260 + 10];
    char *p = buf;
    char *q = p;
    strcpy(buf, tk.GetString().c_str());
    while (isspace(*p))
        p++;
    if (*p == '(')
    {
        do
        {
            p++;
        } while (isspace(*p));
        q= strchr(p, ')');
        if (q)
        {
            while (q != p && isspace(q[-1]))
                q--;
            *q = 0;
            if (*p)
                Libraries::Instance()->Add(p);
        }
    }
}
void ppPragma::HandleAlias(Tokenizer &tk)
{
    const Token *name = tk.Next();
    if (name && name->IsIdentifier())
    {
        std::string id = name->GetId();
        const Token *alias = tk.Next();
        if (alias && alias->GetKeyword() == eq)
        {
            alias = tk.Next();
            if (alias && alias->IsIdentifier())
            {
                Aliases::Instance()->Add(id, alias->GetId());
            }
        }
    }
}
void ppPragma::HandleFar(Tokenizer &tk)
{
    // fixme
}
