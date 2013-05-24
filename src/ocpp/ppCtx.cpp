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
#include "ppCtx.h"

ppCtx::~ppCtx()
{
    while (stack.size())
        pop();
}
bool ppCtx::Check(int token, std::string &line)
{
    bool rv = false;
    if (token == PUSH)
        rv = push(line);
    else if (token == POP)
        rv = pop();
    else if (token == REPL)
        rv = repl(line);
    return rv;
}
std::string ppCtx::GetId(std::string &line)
{
    Tokenizer tk(line, NULL);
    const Token *t = tk.Next();
    if (t->IsIdentifier())
    {
        return t->GetId();
    }
    else
    {
        Errors::Error("Identifier expected");
    }
    return "";
}
bool ppCtx::push(std::string &line)
{
    define.Process(line);
    CtxData *p = new CtxData;
    p->id = nextId++;
    p->name = GetId(line);
    stack.push_front(p);
    return true;
}
bool ppCtx::pop()
{
    if (stack.size())
    {
        CtxData *p = stack.front();
        stack.pop_front();
        delete p;
    }
    else
    {
        Errors::Error("%pop without %push");
    }
    return true;
}
bool ppCtx::repl(std::string &line)
{
    if (stack.size())
    {
        define.Process(line);
        CtxData *p = stack.front();
        p->name = GetId(line);
    }
    else
    {
        Errors::Error("%repl without %push");
    }
    return true;
}
