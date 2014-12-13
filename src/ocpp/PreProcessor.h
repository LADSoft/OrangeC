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
#ifndef PreProcessor_h
#define PreProcessor_h

static const int DEFINE = 1;
static const int UNDEF = 2;
static const int PRAGMA = 3;
static const int ERROR = 4;
static const int LINE = 5;
static const int INCLUDE = 6;
static const int IF = 7;
static const int ELIF = 8;
static const int IFDEF = 9;
static const int IFNDEF = 10;
static const int ELSE = 11;
static const int ENDIF = 12;
static const int IFIDN = 20;
static const int ELIFIDN = 21;
static const int IFIDNI = 22;
static const int ELIFIDNI = 23;
static const int IFID = 24;
static const int ELIFID = 25;
static const int IFNUM = 26;
static const int ELIFNUM = 27;
static const int IFSTR = 28;
static const int ELIFSTR = 29;
static const int IFNIDN = 30;
static const int ELIFNIDN = 31;
static const int IFNIDNI = 32;
static const int ELIFNIDNI = 33;
static const int IFNID = 34;
static const int ELIFNID = 35;
static const int IFNNUM = 36;
static const int ELIFNNUM = 37;
static const int IFNSTR = 38;
static const int ELIFNSTR = 39;
static const int ELIFDEF = 50;
static const int ELIFNDEF = 51;
static const int ASSIGN = 52;
static const int REP = 53;
static const int EXITREP = 54;
static const int ENDREP = 55;
static const int MACRO = 56;
static const int ENDMACRO = 57;
static const int ROTATE = 58;
static const int PUSH = 59;
static const int POP = 60;
static const int REPL = 61;
static const int IFCTX = 62;
static const int ELIFCTX = 63;
static const int IFNCTX = 64;
static const int ELIFNCTX = 65;
static const int IDEFINE = 66;
static const int IASSIGN = 67;
static const int IMACRO = 68;

#include "ppDefine.h"
#include "ppError.h"
#include "ppFile.h"
#include "ppInclude.h"
#include "ppPragma.h"
#include "ppMacro.h"
#include "ppCtx.h"

class PreProcessor
{
public:
    PreProcessor(const std::string &FileName, const std::string &SrchPth, const std::string &SysSrchPth,
                 bool fullName, bool Trigraph, char PPStart, bool isunsignedchar, 
                 bool C89, bool extensions) 
        : ppStart(PPStart), preData(NULL), lineno(0),
        include(fullName, Trigraph, extensions, isunsignedchar, C89, SrchPth, SysSrchPth, PPStart == '%'), 
            define(extensions, &include, C89, PPStart == '%'), 
            macro(include, define), ctx(define), trigraphs(Trigraph)
        { InitHash(); Errors::SetInclude(&include); macro.SetPreProcessor(this); 
				include.SetParams(FileName, &define, &ctx); define.SetParams(&ctx, &macro); }
        
    void InitHash();
    bool GetLine(std::string &line);
    std::string GetFile() { return include.GetFile(); }
    int GetLineNo() { return include.GetLineNo(); }
    int GetMainLineNo() { return lineno; }
    void Define(const std::string &name, std::string &value, bool caseInsensitive) { define.Define(name, value, NULL, false, false, false, caseInsensitive); }
    void Undefine(std::string &name) { define.Undefine(name, true); }
    void IncludeFile(const std::string &name) { include.IncludeFile(name); }
    int GetCtxId() { return ctx.GetTopId(); }
    int GetMacroId() { return macro.GetMacroId(); }
    void Assign(std::string &name, int value, bool caseInsensitive) { define.Assign(name, value, caseInsensitive); }
    bool InMacro() { return macro.InMacro(); }
    void SetPreData(char *data) {preData = new std::string(data); }
    bool GetPreLine(std::string &line) ;
    std::string StripDigraphs(std::string line);
    std::string StripTrigraphs(std::string line);
private:
    bool trigraphs;
    char ppStart;
    ppCtx ctx;
    ppMacro macro;
    ppInclude include;
    ppDefine define;
    ppError ppErr;
    ppPragma pragma;
    KeywordHash hash;
    int lineno;
    std::string *preData;
} ;

#endif