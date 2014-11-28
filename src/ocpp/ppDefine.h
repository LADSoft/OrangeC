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
#ifndef ppDefine_h
#define ppDefine_h

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include "SymbolTable.h"
#include "Token.h"
#include "ppExpr.h"
#include "ppCtx.h"
#include "ppMacro.h"

typedef std::vector<std::string> DefinitionArgList;

class ppInclude;
class ppMacro;

class ppDefine
{
public:
    class Definition : public Symbol
    {
    public:
        Definition(const std::string &Name, std::string &Value, DefinitionArgList *List, bool Permanent)
            : Symbol(Name), value(Value), argList(List), undefined(false), permanent(Permanent), varargs(false),
            caseInsensitive(false) { }
        virtual ~Definition() { delete argList;  }
        bool HasVarArgs() const { return varargs; }
        void SetHasVarArgs() { varargs = true; }
        int GetArgCount() { if (!argList) return 0; else return argList->size(); }
        DefinitionArgList *GetArg(int count) 
        { 
            if (!argList) return NULL; 
            else if (count < argList->size()) return &argList[count];
            else return NULL;
        }
        DefinitionArgList *GetArgList() const { return argList; }
        std::string &GetValue() { return value; }
        bool IsUndefined() { return undefined; }
        bool Undefine() { return undefined; }
        bool IsCaseInsensitive() { return caseInsensitive; }
        void  SetCaseInsensitive(bool flag) { caseInsensitive = flag; }
    private:
        bool caseInsensitive;
        bool undefined;
        bool permanent;
        bool elipses;
        bool varargs;
        std::string value;
        DefinitionArgList* argList;
    };
public:
    ppDefine(bool UseExtensions, ppInclude *Include, bool C89, bool Asmpp);
    ~ppDefine() {  }
    
	void SetParams(ppCtx *Ctx, ppMacro *Macro)
	{
		ctx = Ctx;
		macro = Macro;
	}
    bool Check(int token, std::string &line);
    Definition *Define(const std::string &name, std::string &value, DefinitionArgList *args, bool permanent, bool varargs, bool errors, bool caseInsensitive);
    void Undefine(const std::string &name, bool forever);
    Definition *Lookup(const std::string &name);
    void Process(std::string &line);
    void replaceDefined(std::string &line);
    void Assign(const std::string &name, int value, bool caseInsensitive) 
        { 
            std::stringstream aa;
            aa << value;
            std::string v;
            aa >> v;
            Define(name, v, NULL, false, false, false, caseInsensitive);
        }

protected:
    enum { 
        PP_MASTART = 17,
        PP_MAEND = 18,
        PP_TOKEN_BREAK = 19,
        REPLACED_TOKENIZING = 20,
        NULL_TOKENIZER = 26
    } ;
    void InitHash();
    std::string defid(const std::string &macro, int &i, int &j);
    void DoDefine(std::string &line, bool caseInsensitive);
    void DoAssign(std::string &line, bool caseInsensitive);
    void DoUndefine(std::string &line);
    void SetDefaults();
    int LookupDefault(std::string &macro, int begin, int end, const std::string &name);
    void nodefines(void);
    bool indefine(const std::string &name);
    void enterdefine(const std::string &name);
    void exitdefine(const std::string &name);
    int Stringize(std::string &macro, int begin, int end, const std::string &text);
    void Tokenize(std::string &macro);
    bool NotSlashed(const std::string &macro, int pos);
    void ReplaceArgs(const std::string id, std::string &macro, 
                 const DefinitionArgList &oldargs, const DefinitionArgList &newArgs, const std::string varags);
    void replacesegment(std::string &line, int begin, int end, bool outer);
    void SyntaxError(const std::string &name);	
    void ParseAsmSubstitutions(std::string &line);
    void ReplaceAsmMacros(std::string &line);				  
private:
    SymbolTable symtab;
    ppInclude *include;
    std::string date;
    std::string time;
    std::set<std::string> defineList;
    KeywordHash defTokens;
    bool c89;
    ppExpr expr;
    ppCtx *ctx;
    ppMacro *macro;
    bool asmpp;
} ;
#endif