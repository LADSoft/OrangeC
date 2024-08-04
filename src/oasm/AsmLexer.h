/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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
 * 
 */

#ifndef Lexer_h
#define Lexer_h

#include "Token.h"

class PreProcessor;
class AsmFile;

enum class kw
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
    begin,
    end,
    assn,
    // must be after punctuation
    ABORT,
    ABSOLUTE,
    ALIGN,
    BALIGN,
    BITS,
    CLASS,
    DATA,
    DB,
    DD,
    DOUBLE,
    DQ,
    DT,
    DW,
    EJECT,
    EQV,
    ERROR,
    EXPORT,
    EXTERN,
    EQU,
    FAIL,
    FILL,
    GALIGN,
    GROUP,
    GSECTION,
    IMPORT,
    INCBIN,
    NOPS,
    P2ALIGN,
    POPSECTION,
    PREVIOUS,
    PRINT,
    PUBLIC,
    PUSHSECTION,
    RESB,
    RESD,
    RESQ,
    REST,
    RESW,
    SECTION,
    SEG,
    SET,
    SINGLE,
    SPACE,
    STACK,
    STRING,
    SUBSECTION,
    WRT,
    TEXT,
    TIMES,
    UNKNOWNDIRECTIVE,
    USE16,
    USE32,
    USE64,
    VIRTUAL,
    WARNING,

    dollars = 300,
    dollarsdollars,
    sdivide,
    smod,

};

class Lexer
{
  public:
    Lexer(PreProcessor& PP, bool GAS);
    ~Lexer() { delete tokenizer; }

    void SetAsmFile(AsmFile* th) { asmFile = th; }
    std::string GetRestOfLine(bool reset = true);
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
    static const char* preDataIntel;
    static const char* preDataGas;
};
#endif  // Lexer_h