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
#include "ASMFile.h"
#include "PreProcessor.h"
#include "UTF8.h"

#include <stdexcept>

KeywordHash Lexer::hash;	
bool Lexer::hashInitted;

Lexer::Lexer(PreProcessor &PP) : asmFile(NULL), 
    pp(PP), atEol(false), atEof(false), tokenizer(NULL), token(NULL), stopAtEol(false),
    parsingDirective(false)
{
    pp.SetPreData(preData);
    InitHash();
    InitTokenizer();
}
void Lexer::InitHash()
{
    if (!hashInitted)
    {
        hash["("] = openpa;
        hash[")"] = closepa;
        hash["+"] = plus;
        hash["-"] = minus;
        hash["!"] = lnot;
        hash["~"] = bcompl;
        hash["*"] = star;
        hash["/"] = divide;
        hash["%"] = mod;
        hash["<<"] = leftshift;
        hash[">>"] = rightshift;
        hash[">"] = gt;
        hash["<"] = lt;
        hash[">="] = geq;
        hash["<="] = leq;
        hash["=="] = eq;
        hash["!="] = ne;
        hash["="] = assn;
        hash["|"] = bor;
        hash["&"] = band;
        hash["^"] = bxor;
        hash["||"] = lor;
        hash["&&"] = land;
        hash["?"] = hook;
        hash[":"] = colon;
        hash[","] = comma;
        hash["["] = openbr;
        hash["]"] = closebr;
        hash["ABSOLUTE"] = ABSOLUTE;
        hash["ALIGN"] = ALIGN;
        hash["CLASS"] = CLASS;
        hash["DB"] = DB;
        hash["DD"] = DD;
        hash["DQ"] = DQ;
        hash["DT"] = DT;
        hash["DW"] = DW;
        hash["EXPORT"] = EXPORT;
        hash["EXTERN"] = EXTERN;
        hash["EQU"] = EQU;
        hash["GLOBAL"] = PUBLIC;
        hash["GROUP"] = GROUP;
        hash["IMPORT"] = IMPORT;
        hash["INCBIN"] = INCBIN;
        hash["RESB"] = RESB;
        hash["RESD"] = RESD;
        hash["RESQ"] = RESQ;
        hash["REST"] = REST;
        hash["RESW"] = RESW;
        hash["SECTION"] = SECTION;
        hash["STACK"] = STACK;
        hash["TIMES"] = TIMES;
        hash["USE16"] = USE16;
        hash["USE32"] = USE32;
        hash["VIRTUAL"] = VIRTUAL;
        hashInitted = true;
    }
}
void Lexer::InitTokenizer()
{
    tokenizer = new Tokenizer("",  &hash);
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
void Lexer::CheckAssign(std::string &line, PreProcessor &pp)
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
                if (line.size() - 7 > npos && line.substr(npos,6) == "assign" && isspace(line[npos+6]))
                {
                    assign = true;
                }
                else if (line.size() - 8 > npos && line.substr(npos, 7) == "iassign" && isspace(line[npos+7]))
                {
                    assign = true;
                    caseInsensitive = true;
                }
            }
            if (assign)
            {
                std::string name;
                int value = 0;
                npos = line.find_first_not_of(" \t\r\b\v", npos+6 + (caseInsensitive ? 1 : 0));
                if (npos == std::string::npos || !IsSymbolStartChar(line.c_str() + npos))
                {
                    Errors::Error("Expected identifier");
                }
                else
                {
                    int npos1 = npos;
                    
                    while (npos1 != line.size() && IsSymbolChar(line.c_str() + npos1))
                    {
                        int n = UTF8::CharSpan(line.c_str() + npos1);
                        while (n-- && npos1 < line.size())
                           npos1 ++;
                    }
                    name = line.substr(npos, npos1-npos);
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
                            catch (std::runtime_error *e)
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
