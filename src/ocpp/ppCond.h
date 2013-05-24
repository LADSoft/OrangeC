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
#ifndef ppCond_h
#define ppCond_h

#include <string>
#include <deque>

#include "ppExpr.h"
class ppDefine;
class ppCtx;

class ppCond
{
public:
    ppCond(ppDefine *Define, ppCtx &Ctx, bool isunsignedchar, bool C89, bool Extensions, bool AsmPP) : current(NULL), 
            define(Define), c89(C89), expr(isunsignedchar, Define), extensions(Extensions), ctx(Ctx), asmpp(AsmPP) { };
    ~ppCond();
    bool Check(int token, const std::string &line, int lineno);
    void CheckErrors() ;
    bool Skipping() { return current && current->skipping; }
    void Mark() { marks.push_front(skipList.size() + current != NULL); }
    void Drop() { marks.pop_front(); }
    void Release() { 
        if (marks.size())
        {
            int n = marks.front(); 
            marks.pop_front(); 
            while ((skipList.size() + (current != NULL)) > n)
            {
                HandleEndIf(std::string(""));
            }
        }
    }
protected:
    ppCond(const ppCond &);
    void HandleIf(int val, const std::string &line, int lineno);
    void HandleElif(int val, const std::string &line);
    void HandleElse(std::string &line);
    void HandleEndIf(std::string &line);
    void HandleDef(std::string &line, bool Else, bool negate, int lineno);
    void HandleIdn(std::string &line, bool Else, bool negate, bool caseSensitive, int lineno);
    void HandleId(std::string &line, bool Else, bool negate, int lineno);
    void HandleNum(std::string &line, bool Else, bool negate, int lineno);
    void HandleStr(std::string &line, bool Else, bool negate, int lineno);
    void HandleCtx(std::string &line, bool Else, bool negate, int lineno);
    void ansieol(const std::string &args);
private:
    class skip
    {
        public:
            skip() : skipping(false), takeElse(false), elseSeen(false), line(0) { }
            bool skipping;
            bool takeElse;
            bool elseSeen;
            int line;
    } ;
    
    std::deque<skip *> skipList;
    std::deque<int> marks;
    skip *current;
    
    ppDefine *define;
    ppExpr expr;
    bool c89;
    bool extensions;
    ppCtx &ctx;
    bool asmpp;
};

#endif