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
#ifndef ppInclude_h
#define ppInclude_h

#include <list>
#include <string>

#include "ppExpr.h"
#include "ppFile.h"

class ppFile;
class ppDefine;
class ppCtx;

class ppInclude
{
public:
    ppInclude(bool Fullname, bool Trigraph, bool extended, 
              bool isunsignedchar, bool C89, const std::string &SrchPth, const std::string &SysSrchPth, bool Asmpp) : 
        unsignedchar(isunsignedchar), c89(C89), trigraphs(Trigraph), asmpp(Asmpp), define(nullptr), ctx(nullptr),
        extendedComment(extended || !C89), fullname(Fullname), current(nullptr), expr(unsignedchar),
        srchPath(SrchPth), sysSrchPath(SysSrchPth)
        {
        }
              
    ~ppInclude() ;
	void SetParams(const std::string &Name, ppDefine *Define, ppCtx *Ctx)
	{
		define = Define;
		ctx = Ctx;
		expr.SetParams(define);
        pushFile(Name);
	}
    bool Check(int token, const std::string &line);
    int GetLineNo() 
    { 
        if (current)
            return current->GetErrorLine();
        else
            return 0;
    }
    std::string GetFile()
    {
        if (current)
            return current->GetErrorFile();
        else
            return "";
    }
    bool GetLine(std::string &line, int &lineno);
    bool Skipping() { if (current) return current->Skipping(); else return false; }
    void IncludeFile(const std::string &name) { pushFile(name); }
    void SetInProc(const std::string &name) { inProc = name; }
    void Mark() { current->Mark(); }
    void Drop() { current->Drop(); }
    void Release() { current->Release(); }
protected:
    void StripAsmComment(std::string &line);
    bool CheckInclude(int token, const std::string &line);
    bool CheckLine(int token, const std::string &line);
    void pushFile(const std::string &name);
    bool popFile();
    void ParseName(const std::string &args);
    void FindFile(const std::string &args);
    bool SrchPath(bool system);
    const char *RetrievePath(char *buf, const char *path);
    void AddName(char *buf);
private:
    std::string name;
    bool system;
    std::list<ppFile *> files;
    ppFile *current;
    ppDefine *define;
    bool unsignedchar;
    bool c89;
    bool trigraphs;
    bool extendedComment;
    bool fullname;
    ppExpr expr;
    std::string srchPath, sysSrchPath;
    ppCtx *ctx;
    std::string inProc;
    bool asmpp;
} ;
#endif