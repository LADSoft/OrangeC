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
#ifndef RCFile_h
#define RCFile_h

#include "Lexer.h"
#include "Expression.h"
#include <string>

class PreProcessor;
class Resource;
class ResFile;

class RCFile
{
public:
    RCFile(PreProcessor &PP, ResFile &ResFile, std::string &includes, int Language) 
        : pp(PP), resFile(ResFile), lexer(PP), expr(lexer), language(Language), includePath(includes) { }
    virtual ~RCFile() { }

    void NextToken() { lexer.NextToken(); }	
    const Token *GetToken() { return lexer.GetToken(); }
    bool IsKeyword();
    unsigned GetTokenId();
    bool IsNumber();
    unsigned GetNumber();
    bool IsString();
    std::wstring GetString();
    bool IsIdentifier();
    std::wstring GetId();
    void NeedEol();
    void SkipComma();
    void NeedBegin();
    void NeedEnd();
    int GetLanguage() { return language; }
    std::string GetFileName();
    bool AtEol() { return lexer.AtEol(); }
    bool AtEof() { return lexer.AtEof(); }
    std::string GetRestOfLine() { return lexer.GetRestOfLine(); }
    bool Read();
    ResFile &GetResFile() { return resFile; }
    std::string CvtString(const std::wstring &str);
    std::wstring CvtString(const std::string &str);
protected:
    Resource *GetRes();
    
private:
    PreProcessor &pp;
    Lexer lexer;
    Expression expr;
    int language;
    ResFile &resFile;
    std::string includePath;
} ;
#endif
