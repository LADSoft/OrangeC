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
#ifndef ppMacro_h
#define ppMacro_h

#include "ppExpr.h"
#include "ppDefine.h"

#include <vector>
#include <string>

class PreProcessor;

struct MacroData
{
    std::vector<std::string> lines;
    int begline;
    int repsLeft;
    int offset;
    int id;	//-1 for rep, else macro invoke # 1- x
    int argmin;
    int argmax;
    bool plus;
    bool isNative;
    bool caseInsensitive;
    std::vector<std::string> defaults;
    std::vector<std::string> args;
};
class ppMacro
{
public:
    ppMacro(ppInclude &Include, ppDefine &Define);
    ~ppMacro();
    bool Check(int token, std::string &line);
    bool GetLine(std::string &line, int &lineno);
    bool Invoke(std::string name, std::string line);
    int GetMacroId() { MacroData *p = GetTopMacro(); if (p) return p->id; else return -1; }
    int GetMacroMax() { MacroData *p = GetTopMacro(); if (p) return p->argmax; else return 0; }
    std::vector<std::string> *GetMacroArgs() { MacroData *p = GetTopMacro(); if (p) return &p->args; else return nullptr; }
    bool InMacro() { return stack.size() != 0; }
    void SetPreProcessor(PreProcessor *PP) { pp = PP; }
protected:
    void GetArgs(int max, std::string &line, std::vector<std::string> &vals);
    std::string ExtractArg(std::string &line);
    bool HandleRep(std::string &line);
    bool HandleExitRep();
    bool HandleEndRep();
    bool HandleMacro(std::string &line, bool caseInsensitive);
    bool HandleEndMacro();
    bool HandleRotate(std::string &line);
    MacroData *GetTopMacro();
    void reverse(std::vector<std::string> &x, int offs, int len);
private:
    ppInclude &include;
    ppDefine &define;
    ppExpr expr;
    PreProcessor *pp;
    
    std::vector<MacroData *> stack;
    std::map<std::string, MacroData *>macros;
    int nextMacro;
} ;

#endif