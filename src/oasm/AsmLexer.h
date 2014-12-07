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
#ifndef Lexer_h
#define Lexer_h

#include "Token.h"


class PreProcessor;
class AsmFile;

class Lexer
{
public:
    enum
    {
        openpa=1, closepa, plus, minus, lnot, bcompl, star, divide, mod,
        leftshift, rightshift, gt, lt, geq, leq, eq, ne,
        bor, bxor, band, land, lor, hook, colon, comma,
        openbr, closebr, assn,
        // must be after punctuation
        ABSOLUTE, ALIGN, CLASS, DB, DD, DQ, DT, DW, EXPORT, EXTERN, EQU, GROUP, 
        IMPORT, INCBIN, PUBLIC, RESB, RESD, RESQ, REST, RESW, SECTION, SEG, 
        STACK, WRT, TIMES, USE16, USE32, VIRTUAL
    };
    Lexer(PreProcessor &PP);
    ~Lexer() { }

    void SetAsmFile(AsmFile *th) { asmFile = th; }
    std::string GetRestOfLine();
    const Token *GetToken() { return token; }
    void NextToken();
    void Reset(const std::string &line) { tokenizer->Reset(line); NextToken(); }
    bool AtEol() { return atEol; }
    bool AtEof() { return atEof; }
    void SkipPastEol()
    {
        while (!AtEol() && !AtEof())
            NextToken();
    }
    void StopAtEol(bool stop) { stopAtEol = stop; }
protected:
    void CheckAssign(std::string &line, PreProcessor &pp);
    void InitHash();
    void InitTokenizer();
private:
    AsmFile *asmFile;
    PreProcessor &pp;
    bool atEol;
    bool atEof;
    bool stopAtEol;
    Tokenizer *tokenizer;
    const Token *token;
    bool parsingDirective;
    static KeywordHash hash;
    static bool hashInitted;
    static char *preData;
} ;
#endif // Lexer_h