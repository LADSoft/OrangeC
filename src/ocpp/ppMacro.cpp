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
#include "PreProcessor.h"
#include "ppMacro.h"
#include "UTF8.h"
#include <fstream>
#include <limits.h>

ppMacro::ppMacro(ppInclude &Include, ppDefine &Define) :
        include(Include), expr(false, &define), define(Define), nextMacro(1) 
{
}
ppMacro::~ppMacro()
{
    while (stack.size())
        HandleExitRep();
}
bool ppMacro::Check(int token, std::string &line)
{
    bool rv = false;
    switch (token)
    {
        case REP:
            rv = HandleRep(line);
            break;
        case EXITREP:
            include.Release();
            rv = HandleExitRep();
            break;
        case ENDREP:
            rv = HandleEndRep();
            break;
        case MACRO:
            rv = HandleMacro(line, false);
            break;
        case IMACRO:
            rv = HandleMacro(line, true);
            break;
        case ENDMACRO:
            rv = HandleEndMacro();
            break;
        case ROTATE:
            rv = HandleRotate(line);
            break;
    }
    return rv;
}
bool ppMacro::GetLine(std::string &line, int &lineno)
{
    while (stack.size())
    {
        MacroData *p = stack.back();
        if (p->offset >= p->lines.size())
        {
            p->offset = 0;
            if (--p->repsLeft == 0)
            {
                include.Drop();
                HandleExitRep();
                continue;
            }
        }
        else
        {
            if (!p->isNative)
                lineno = p->offset + p->begline;
            line = p->lines[p->offset++];
            return true;
        }
    }
    return false;
}
bool ppMacro::HandleRep(std::string &line)
{
    define.Process(line);
    int n = expr.Eval(line);//FIXME PPINT <-> int
    int level = 1;
    MacroData *p = NULL;
    if (n < 0)
    {
        Errors::Error("Negative range in %rep expression");
    }
    else if (n > 0)
    {
        p = new MacroData;
        p-> repsLeft = n;
        p->offset = 0;
        p->id = -1;
        p->begline = 0;
        p->isNative = false;
        p->caseInsensitive = false;
    }
    std::string ll;
    include.SetInProc("%rep");
    while (pp->GetPreLine(ll))
    {
        if (p->begline == 0 )
            p->begline = pp->GetMainLineNo();
        int t = ll.find_first_not_of(" \t\v\r\n");
        if (t != std::string::npos)
        {
            if (ll[t] == '%')
            {
                t = ll.find_first_not_of(" \t\v\r\n", t+1);
                if (ll.substr(t, 3) == "macro")
                    Errors::Error("macro definition in %rep loop not allowed");
                else if (ll.substr(t, 3) == "rep")
                    level ++;
                else if (ll.substr(t,6) == "endrep")
                    if (!--level)
                        break;
            }
            if (p)
                p->lines.push_back(ll);
        }
    }
    include.SetInProc("");
    if (level != 0)
    {
        delete p;
    }
    else
    {
        if (p)
        {
            include.Mark();
            stack.push_back(p);
        }
    }
    return true;
}
bool ppMacro::HandleExitRep()
{
    MacroData *p = stack.back();
    stack.pop_back();
    if (p->id < 0)
        delete p;
    return true;
}
bool ppMacro::HandleEndRep()
{
    Errors::Error("unexpected %endrep");
    return true;
}
std::string ppMacro::ExtractArg(std::string &line)
{
    int npos;
    if (line[0] == '{')
    {
        npos = line.find_first_of('}');
    }
    else
    {
        npos = line.find_first_of(',');
    }
    if (npos == -1)
        npos = line.size();
    std::string rv = line.substr(0, npos);
    line.erase(0, npos + 1);
    if (rv[0] == '{')
    {
        rv.erase(0,1);
        npos = line.find_first_not_of(" \t\r\v\n");
        if (npos != std::string::npos)
            if (line[npos] != ',')
                Errors::Error("Expected ','");
            else
                line.erase(0, npos+1);
        else
            line = "";
    }
    else
    {
        npos = rv.find_last_not_of(" \t\r\n\v");
        if (npos == std::string::npos)
            rv = "";
        else if (npos != rv.size() -1)
            rv.erase(npos+1, rv.size() - npos -1);
            
    }
    return rv;
}
void ppMacro::GetArgs(int max, std::string &line, std::vector<std::string> &vals)
{
    for (int i=0; i < max; i++)
    {
        int npos = line.find_first_not_of(" \t\r\n\v");
        if (npos == std::string::npos)
        {
            line = "";
            break;
        }
        line.erase(0, npos);
        std::string temp = ExtractArg(line);
        vals.push_back(temp);
    }
    int npos = line.find_first_not_of(" \t\r\n\v");
    if (npos != std::string::npos)
    {
        line.erase(0,npos);
        npos = line.find_last_not_of(" \t\r\n\v");
        if (npos != std::string::npos && npos < line.size()-1)
            line.erase(npos + 1, line.size() - npos - 1);
    }
}
bool ppMacro::HandleMacro(std::string &line, bool caseInsensitive)
{
    bool nolist = false;
    bool native = false;
    define.Process(line);
    std::string name;
    int start, end;
    bool plussign;
    Tokenizer tk(line, ppExpr::GetHash());
    const Token *next = tk.Next();
    MacroData *p = NULL;
    bool bailed = false;
    if (next->IsIdentifier())
    {
        name = next->GetId();
        if (caseInsensitive)
            name = UTF8::ToUpper(name);
        next = tk.Next();
        if (next->IsNumeric())
        {
            start = end = next->GetInteger();
            plussign = false;
            next = tk.Next();
            if (next->GetKeyword() == ::minus)
            {
                next = tk.Next();
                if (next->IsNumeric())
                {
                    end = next->GetInteger();
                    if (end < start)
                    {
                        Errors::Error("Macro end limit < start limit");						
                        bailed = true;
                    }
                    else
                    {
                        next = tk.Next();
                        if (next->GetKeyword() == ::plus)
                        {
                            next = tk.Next();
                            plussign = true;
                        }
                    }
                }
                else if (next->GetKeyword() == star)
                {
                    end = INT_MAX;
                }
                else
                {
                    Errors::Error("Macro argument end limit expected");
                    bailed = true;
                }
            }
            else if (next->GetKeyword() == ::plus)
            {
                next = tk.Next();
                plussign = true;
            }
            if (next->IsIdentifier())
            {
                if (next->GetId() == ".nolist")
                {
                    next = tk.Next();
                    nolist = true;
                }
                else if (next->GetId() == ".native")
                {
                    next = tk.Next();
                    native = true;
                }
            }
        }
        else
        {
            Errors::Error("Macro argument count expected");
            bailed = true;
        }	
    }	
    else
    {
        Errors::Error("Macro name expected");
        bailed = true;
    }
    if (!bailed)
    {
        p = new MacroData;
        p-> repsLeft = 0;
        p->offset = 0;
        p->id = 0; // macro
        p->argmin = start;
        p->argmax = end;
        p->plus = plussign;
        p->begline = 0;
        p->isNative = native;
        p->caseInsensitive = caseInsensitive;
        line = next->GetChars() + tk.GetString();
        int count = 0;
        GetArgs(end-start, line, p->defaults);
        if (line.size())
        {
            if (line.find_first_not_of(" \t\r\n\v") == std::string::npos)
                line.erase(0,line.size());
        }
        if ((p->defaults.size() + (line.size() != 0)) > end - start + plussign)
            Errors::Error("Too many default arguments to macro");
        if (line.size())
            p->defaults.push_back(line);
    }
    std::string ll;
    include.SetInProc("%macro");
    while (pp->GetPreLine(ll))
    {
        if (p && p->begline == 0 )
            p->begline = nolist ? INT_MIN : pp->GetMainLineNo();
        int t = ll.find_first_not_of(" \t\v\r\n");
        if (t != std::string::npos)
        {
            if (ll[t] == '%')
            {
                t = ll.find_first_not_of(" \t\v\r\n", t+1);
                if (ll.substr(t, 5) == "macro")
                    Errors::Error("macro definitions may not be nested");
                else if (ll.substr(t,8) == "endmacro")
                        break;
            }
            if (p)
                p->lines.push_back(ll);
        }
    }
    include.SetInProc("");
    if (p)
        macros[name] = p;
    return true;
}
bool ppMacro::HandleEndMacro()
{
    Errors::Error("unexpected %endmacro");
    return true;
}
void ppMacro::reverse(std::vector<std::string> &x, int offs, int len)
{
    for (int i=0; i < len/ 2; i++)
    {
        std::string n = x[offs + i];
        x[offs + i] = x[offs + len - i - 1];
        x[offs + len - i - 1] = n;
    }
}
bool ppMacro::HandleRotate(std::string &line)
{
    MacroData *p = GetTopMacro();
    if (!p)
    {
        Errors::Error("%rotate outside of macro invocation");
    }
    else
    {
        define.Process(line);
        int n = expr.Eval(line);//FIXME PPINT <-> int
        if (n < 0)
        {
            n = - n;
            n %= p->args.size();
            n = p->args.size() - n;
        }
        else
        {
            n %= p->args.size();
        }
        if (n != 0)
        {				
            reverse(p->args, 0, n);
            reverse(p->args, n, p->args.size() - n );
            reverse(p->args, 0, p->args.size());
        }
    }
    return true;
}
bool ppMacro::Invoke(std::string &name, std::string &line)
{
    std::map<std::string, MacroData *>::iterator it = macros.find(name);
    if (it == macros.end())
    {
        std::string name1 = UTF8::ToUpper(name);
        it = macros.find(name1);
        if (it == macros.end() || it->second->caseInsensitive == false)
            return false;
        else
            name = name1;
    }
    MacroData *p = it->second;
    if (p->repsLeft != 0)
    {
        return false;
    }
    p->args.clear();
    GetArgs(p->argmax, line, p->args);
    if (line.size())
    {
        if (line.find_first_not_of(" \t\r\n\v") == std::string::npos)
            line.erase(0,line.size());
    }
    if (p->args.size() < p->argmin)
    {
        Errors::Error("Too few arguments in call to macro");
        return true;
    }
    else if ((p->args.size() + (line.size() != 0)) > (p->argmax + p->plus))
    {
        Errors::Error("Too many arguments in call to macro");
        return true;
    }
    while (p->defaults.size() + p->argmin > p->args.size())
    {
        p->args.push_back(p->defaults[p->args.size() - p->argmin]);
    }
    if (line.size())
        p->args.push_back(line);
    p->id = nextMacro++;
    p->repsLeft = 1;
    include.Mark();
    stack.push_back(p);
    return true;
}
MacroData *ppMacro::GetTopMacro()
{
    for (int i = stack.size()-1; i >= 0; i--)
        if (stack[i]->id != -1)
            return stack[i];
    return NULL;
}
