/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 */

#include "AsmFile.h"
#include "PreProcessor.h"
#include "UTF8.h"

#include <stdexcept>

KeywordHash Lexer::hash = {
    { "(", openpa},
    { ")", closepa},
    { "+", plus},
    { "-", minus},
    { "!", lnot},
    { "~", bcompl},
    { "*", star},
    { "/", divide},
    { "%", mod},
    { "<<", leftshift},
    { ">>", rightshift},
    { ">", gt},
    { "<", lt},
    { ">=", geq},
    { "<=", leq},
    { "==", eq},
    { "!=", ne},
    { "=", assn},
    { "|", bor},
    { "&", band},
    { "^", bxor},
    { "||", lor},
    { "&&", land},
    { "?", hook},
    { ":", colon},
    { ",", comma},
    { "[", openbr},
    { "]", closebr},
    { "ABSOLUTE", ABSOLUTE},
    { "ALIGN", ALIGN},
    { "BITS", BITS},
    { "CLASS", CLASS},
    { "DB", DB},
    { "DD", DD},
    { "DQ", DQ},
    { "DT", DT},
    { "DW", DW},
    { "EXPORT", EXPORT},
    { "EXTERN", EXTERN},
    { "EQU", EQU},
    { "GALIGN", GALIGN},
    { "GLOBAL", PUBLIC},
    { "GROUP", GROUP},
    { "IMPORT", IMPORT},
    { "INCBIN", INCBIN},
    { "RESB", RESB},
    { "RESD", RESD},
    { "RESQ", RESQ},
    { "REST", REST},
    { "RESW", RESW},
    { "SECTION", SECTION},
    { "STACK", STACK},
    { "TIMES", TIMES},
    { "USE16", USE16},
    { "USE32", USE32},
    { "USE64", USE64},
    { "VIRTUAL", VIRTUAL},

};

Lexer::Lexer(PreProcessor& PP) :
    asmFile(nullptr),
    pp(PP),
    atEol(false),
    atEof(false),
    tokenizer(nullptr),
    token(nullptr),
    stopAtEol(false),
    parsingDirective(false)
{
    pp.SetPreData(preData);
    InitTokenizer();
}
void Lexer::InitTokenizer()
{
    tokenizer = new Tokenizer("", &hash);
    tokenizer->SetCaseInsensitive(true);
}
std::string Lexer::GetRestOfLine()
{
    std::string rv;
    if (token)
        rv = token->GetChars();
    rv += tokenizer->GetString();
    tokenizer->Reset("");
    return rv;
}
void Lexer::CheckAssign(std::string& line, PreProcessor& pp)
{
    size_t npos = line.find_first_not_of(" \t\r\n\v");
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
                            catch (const std::runtime_error& e)
                            {
                                Errors::Error(e.what());
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
