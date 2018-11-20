/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
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
        openpa = 1,
        closepa,
        plus,
        minus,
        lnot,
        bcompl,
        star,
        divide,
        mod,
        leftshift,
        rightshift,
        gt,
        lt,
        geq,
        leq,
        eq,
        ne,
        bor,
        bxor,
        band,
        land,
        lor,
        hook,
        colon,
        comma,
        openbr,
        closebr,
        assn,
        // must be after punctuation
        ABSOLUTE,
        ALIGN,
        CLASS,
        DB,
        DD,
        DQ,
        DT,
        DW,
        EXPORT,
        EXTERN,
        EQU,
        GROUP,
        IMPORT,
        INCBIN,
        PUBLIC,
        RESB,
        RESD,
        RESQ,
        REST,
        RESW,
        SECTION,
        SEG,
        STACK,
        WRT,
        TIMES,
        USE16,
        USE32,
        USE64,
        VIRTUAL
    };
    Lexer(PreProcessor& PP);
    ~Lexer() {}

    void SetAsmFile(AsmFile* th) { asmFile = th; }
    std::string GetRestOfLine();
    const Token* GetToken() { return token; }
    void NextToken();
    void Reset(const std::string& line)
    {
        tokenizer->Reset(line);
        NextToken();
    }
    bool AtEol() { return atEol; }
    bool AtEof() { return atEof; }
    void SkipPastEol()
    {
        GetRestOfLine();
        NextToken();
    }
    void StopAtEol(bool stop) { stopAtEol = stop; }

  protected:
    void CheckAssign(std::string& line, PreProcessor& pp);
    void InitHash();
    void InitTokenizer();

  private:
    AsmFile* asmFile;
    PreProcessor& pp;
    bool atEol;
    bool atEof;
    bool stopAtEol;
    Tokenizer* tokenizer;
    const Token* token;
    bool parsingDirective;
    static KeywordHash hash;
    static bool hashInitted;
    static char* preData;
};
#endif  // Lexer_h