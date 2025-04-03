/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include "AsmFile.h"
#include "PreProcessor.h"
#include "UTF8.h"

#include <stdexcept>

KeywordHash Lexer::hash = {
    {"(", kw::openpa},
    {")", kw::closepa},
    {"+", kw::plus},
    {"-", kw::minus},
    {"!", kw::lnot},
    {"~", kw::bcompl},
    {"*", kw::star},
    {"/", kw::divide},
    {"%", kw::mod},
    {"<<", kw::leftshift},
    {">>", kw::rightshift},
    {">", kw::gt},
    {"<", kw::lt},
    {">=", kw::geq},
    {"<=", kw::leq},
    {"==", kw::eq},
    {"!=", kw::ne},
    {"=", kw::assn},
    {"|", kw::bor},
    {"&", kw::band},
    {"^", kw::bxor},
    {"||", kw::lor},
    {"&&", kw::land},
    {"?", kw::hook},
    {":", kw::colon},
    {",", kw::comma},
    {"[", kw::openbr},
    {"]", kw::closebr},
    {"{", kw::begin},
    {"}", kw::end},
    {"ABORT", kw::ABORT},
    {"ABSOLUTE", kw::ABSOLUTE},
    {"ALIGN", kw::ALIGN},
    {"BALIGN", kw::BALIGN},
    {"P2ALIGN", kw::P2ALIGN},
    {"BITS", kw::BITS},
    {"CLASS", kw::CLASS},
    {"DATA", kw::DATA},
    {"DB", kw::DB},
    {"DD", kw::DD},
    {"DOUBLE", kw::DOUBLE},
    {"DQ", kw::DQ},
    {"DT", kw::DT},
    {"DW", kw::DW},
    {"EQV", kw::EQV},
    {"EJECT", kw::EJECT},
    {"ERROR", kw::ERROR},
    {"EXPORT", kw::EXPORT},
    {"EXTERN", kw::EXTERN},
    {"EQU", kw::EQU},
    {"FAIL", kw::FAIL},
    {"FILL", kw::FILL},
    {"GALIGN", kw::GALIGN},
    {"GLOBAL", kw::PUBLIC},
    {"GSECTION", kw::GSECTION},
    {"GROUP", kw::GROUP},
    {"IMPORT", kw::IMPORT},
    {"INCBIN", kw::INCBIN},
    {"NOPS", kw::NOPS},
    {"P2ALIGN", kw::P2ALIGN},
    {"POPSECTION", kw::POPSECTION},
    {"PREVIOUS", kw::PREVIOUS},
    {"PRINT", kw::PRINT},
    {"PUSHSECTION", kw::PUSHSECTION},
    {"RESB", kw::RESB},
    {"RESD", kw::RESD},
    {"RESQ", kw::RESQ},
    {"REST", kw::REST},
    {"RESW", kw::RESW},
    {"SECTION", kw::SECTION},
    {"SET", kw::SET},
    {"SINGLE", kw::SINGLE},
    {"SPACE", kw::SPACE},
    {"STACK", kw::STACK},
    {"STRING", kw::STRING},
    {"SUBSECTION", kw::SUBSECTION},
    {"TEXT", kw::TEXT},
    {"TIMES", kw::TIMES},
    {"UNKNOWNDIRECTIVE", kw::UNKNOWNDIRECTIVE},
    {"USE16", kw::USE16},
    {"USE32", kw::USE32},
    {"USE64", kw::USE64},
    {"VIRTUAL", kw::VIRTUAL},
    {"WARNING", kw::WARNING},

};

Lexer::Lexer(PreProcessor& PP, bool GAS) :
    asmFile(nullptr),
    pp(PP),
    atEol(false),
    atEof(false),
    tokenizer(nullptr),
    token(nullptr),
    stopAtEol(false),
    parsingDirective(false)
{
    pp.SetPreData(GAS ? preDataGas : preDataIntel);
    InitTokenizer();
}
void Lexer::InitTokenizer()
{
    tokenizer = new Tokenizer("", &hash);
    tokenizer->SetCaseInsensitive(true);
}
std::string Lexer::GetRestOfLine(bool reset)
{
    std::string rv;
    if (reset)
    {
        if (token)
            rv = token->GetChars();
        rv += tokenizer->GetString();
        int npos = rv.find_first_of(
            ";");  // for GAS, semicolon is a statement seperator, in intel syntax these will have been elided as comments...
        if (npos == std::string::npos)
        {
            tokenizer->Reset("");
        }
        else
        {
            tokenizer->Reset(rv.substr(npos + 1));
            rv = rv.substr(0, npos);
        }
    }
    else
    {
        rv = tokenizer->GetString();
    }
    return rv;
}
void Lexer::CheckAssign(std::string& line, PreProcessor& pp)
{
    int npos = line.find_first_not_of(" \t\r\n\v");
    if (npos != std::string::npos)
    {
        if (line[npos] == '%')
        {
            npos = line.find_first_not_of(" \t\r\b\v", npos + 1);
            bool caseInsensitive = false;
            bool assign = false;
            if (npos != std::string::npos)
            {
                if (line.size() - 7 > npos && line.substr(npos, 6) == "assign" && isspace(line[npos + 6]))
                {
                    assign = true;
                }
                else if (line.size() - 8 > npos && line.substr(npos, 7) == "iassign" && isspace(line[npos + 7]))
                {
                    assign = true;
                    caseInsensitive = true;
                }
            }
            if (assign)
            {
                std::string name;
                int value = 0;
                npos = line.find_first_not_of(" \t\r\b\v", npos + 6 + (caseInsensitive ? 1 : 0));
                if (npos == std::string::npos || !Tokenizer::IsSymbolChar(line.c_str() + npos, true))
                {
                    Errors::Error("Expected identifier");
                }
                else
                {
                    int npos1 = npos;

                    while (npos1 != line.size() && Tokenizer::IsSymbolChar(line.c_str() + npos1, false))
                    {
                        int n = UTF8::CharSpan(line.c_str() + npos1);
                        while (n-- && npos1 < line.size())
                            npos1++;
                    }
                    name = line.substr(npos, npos1 - npos);
                    if (!isspace(line[npos1]))
                    {
                        Errors::Error("Invalid arguments to %assign");
                    }
                    else
                    {
                        npos = line.find_first_not_of(" \t\r\n\v", npos1);
                        if (npos == std::string::npos)
                        {
                            Errors::Error("Expected expression");
                        }
                        else
                        {
                            line = line.substr(npos);
                            Reset(line);
                            parsingDirective = true;
                            try
                            {
                                value = asmFile->GetValue();
                            }
                            catch (std::runtime_error* e)
                            {
                                Errors::Error(e->what());
                                delete e;
                            }
                            parsingDirective = false;
                            pp.Assign(name, value, caseInsensitive);
                        }
                    }
                }
                line = "";
            }
        }
    }
}
void Lexer::NextToken()
{
    atEol = false;
    atEof = false;
    if (parsingDirective)
    {
        tokenizer->Reset("");
    }
    else if (!token || !atEof)
    {
        if (tokenizer)
        {
            bool done = false;
            while (!done)
            {
                token = tokenizer->Next();
                // skip assembly language comments...
                if (token->IsEnd())
                {
                    atEol = true;
                    if (!stopAtEol)
                    {
                        std::string line;
                        if (!pp.GetLine(line))
                        {
                            atEof = true;
                            done = true;
                        }
                        else
                        {
                            CheckAssign(line, pp);
                            tokenizer->Reset(line);
                        }
                    }
                    else
                    {
                        atEof = true;
                        done = true;
                    }
                }
                else
                {
                    done = true;
                }
            }
        }
    }
}
