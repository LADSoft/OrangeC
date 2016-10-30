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
#include "PreProcessor.h"
#include "Errors.h"
#include "UTF8.h"
#include <fstream>
#include <limits.h>

bool asmMode = false;
bool IsSymbolStartChar(const char *data)
{
    if (asmMode)
    {
        return *data == '@' || *data == '_' || *data == '?' || *data == '.' || UTF8::IsAlpha(data);
    }
    else
    {
        return *data == '_' || UTF8::IsAlpha(data);	
    }
}
bool IsSymbolChar(const char *data)
{
    if (asmMode)
    {
        return *data == '_' || *data == '$' || *data == '#' || *data == '@' || *data == '~' ||
            *data == '?' || *data == '.' || *data == '&' || UTF8::IsAlnum(data);
    }
    else
    {
        return *data == '_' || UTF8::IsAlnum(data);
    }
}
void PreProcessor::InitHash()
{
    hash["define"] = DEFINE;
    hash["undef"] = UNDEF;
    hash["pragma"] = PRAGMA;
    hash["error"] = ERROR;
    hash["line"] = LINE;
    hash["include"] = INCLUDE;
    hash["if"] = IF;
    hash["elif"] = ELIF;
    hash["ifdef"] = IFDEF;
    hash["ifndef"] = IFNDEF;
    hash["else"] = ELSE;
    hash["endif"] = ENDIF;
    if (ppStart == '%')
    {
        asmMode = true;
        hash["idefine"] = IDEFINE;
        hash["iassign"] = IASSIGN;
        hash["ifctx"] = IFCTX;
        hash["elifctx"] = ELIFCTX;
        hash["ifnctx"] = IFNCTX;
        hash["elifnctx"] = ELIFNCTX;
        hash["ifidn"] = IFIDN;
        hash["elifidn"] = ELIFIDN;
        hash["ifidni"] = IFIDNI;
        hash["elifidni"] = ELIFIDNI;
        hash["ifid"] = IFID;
        hash["elifid"] = ELIFID;
        hash["ifnum"] = IFNUM;
        hash["elifnum"] = ELIFNUM;
        hash["ifstr"] = IFSTR;
        hash["elifstr"] = ELIFSTR;
        hash["ifnidn"] = IFNIDN;
        hash["elifnidn"] = ELIFNIDN;
        hash["ifnidni"] = IFNIDNI;
        hash["elifnidni"] = ELIFNIDNI;
        hash["ifnid"] = IFNID;
        hash["elifnid"] = ELIFNID;
        hash["ifnnum"] = IFNNUM;
        hash["elifnnum"] = ELIFNNUM;
        hash["ifnstr"] = IFNSTR;
        hash["elifnstr"] = ELIFNSTR;
        hash["elifdef"] = ELIFDEF;
        hash["elifndef"] = ELIFNDEF;
        hash["assign"] = ASSIGN;
        hash["rep"] = REP;
        hash["endrep"] = ENDREP;
        hash["exitrep"] = EXITREP;
        hash["macro"] = MACRO;
        hash["imacro"] = IMACRO;
        hash["endmacro"] = ENDMACRO;
        hash["rotate"] = ROTATE;
        hash["push"] = PUSH;
        hash["pop"] = POP;
        hash["repl"] = REPL;
    }
}
bool PreProcessor::GetPreLine(std::string &line)
{
    if (preData)
    {
        if (preData->size())
        {
            int npos = preData->find_first_of("\n");
            if (npos == std::string::npos)
            {
                line = *preData;
                preData->erase(0, preData->size());
            }
            else
            {
                line = preData->substr(0, npos);
                preData->erase(0, npos + 1);
            }
            return true;
        }
        else
        {
            delete preData;
            preData = nullptr;
        }
    }
    if (ppStart == '%' && macro.GetLine(line, lineno))
    {
        return true;
    }
    if (include.GetLine(line, lineno))
    {
        return true;
    }
    return false;
}
/* strip digraphs */
std::string PreProcessor::StripDigraphs(std::string line)
{
    if (line.size() < 2)
        return line;
    std::string rv;
    int last = 0, cp;
    for (cp=0; cp < line.size()-1; cp++)
    {
        int ch = 0;
        if (line[cp] == '<' && line[cp + 1] == ':')
        {
            ch = '[';
        }
        else if (line[cp] == ':' && line[cp + 1] == '>')
        {
            ch = ']';
        }
        else if (line[cp] == '<' && line[cp + 1] == '%')
        {
            ch = '{';
        }
        else if (line[cp] == '%' && line[cp + 1] == '>')
        {
            ch = '}';
        }
        else if (line[cp] == '%' && line[cp + 1] == ':')
        {
            ch = '#';
        }
        if (ch)
        {
            static char msg[2] = { 0, 0 };
            msg[0] = ch;
            if (last != cp)
            {
                rv += line.substr(last, cp - last) + msg;
            }
            else
            {
                rv += msg;
            }
            cp++;
            last = cp + 1;
        }
    }
    if (last == 0)
        return line;
    rv += line.substr(last, line.size() - last);
    return rv;
}

bool PreProcessor::GetLine(std::string &line)
{
    std::string last;
    while (1)
    {
        if (!GetPreLine(line))
            return false;
        if (last.size())
            line = last + " " + line;
        size_t n = line.find_first_not_of(" \n\t\v\r");
        if (n != std::string::npos)
        {
            if ((line[n] == ppStart && (n +1 < line.length() && line[n+1] != ppStart))
                 || (trigraphs && n < line.size()-1 && line[n] == '%' && line[n+1] == ':'))
            {
                int n1 = line.find_first_not_of(" \n\t\v\r", n+1);
                if (!isdigit(line[n1]))
                {
                    if (trigraphs)
                        line = StripDigraphs(line);
                    Tokenizer tk(line.substr(n+1), &hash);
                    const Token *t = tk.Next();
                    if (t->IsKeyword())
                    {
                        if (t->GetKeyword() == ASSIGN || t->GetKeyword() == IASSIGN)
                        {
                            if (include.Skipping())
                                line.erase(0, line.size());
                            else
                            {
                                int npos = line.find("assign");
                                if (npos != std::string::npos)
                                {
                                    npos = line.find_first_not_of(" \t\r\n\v", npos + 6);
                                    if (npos != std::string::npos)
                                    {
                                        npos = line.find_first_of(" \t\r\n\v", npos);
                                        if (npos != std::string::npos)
                                        {
                                            std::string left = line.substr(0, npos);
                                            std::string right = line.substr(npos);
                                            define.Process(right);
                                            line = left + right;
                                        }
                                    }
                                }
                            }
                            return true;
                        }
                        else
                        {
                            line = tk.GetString();
                            int token = t->GetKeyword();
                            // must come first
                            if (!include.Check(token, line))
                            {
                                if (!define.Check(token, line))
                                {
                                    if (!ppErr.Check(token, line))
                                    {
                                        if (!pragma.Check(token, line))
                                        {
                                            if (ppStart != '%' || (!macro.Check(token, line) && !ctx.Check(token, line)))
                                                Errors::Error("Unknown preprocessor directive");
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (ppStart == '%')
                        {
                            goto join;
                        }
                        Errors::Error("Invalid preprocessor directive");
                    }
                    line.erase(0, line.size());
                }
                else
                {
                    goto join;
                }
            }
            else
            {
    join:
                if (include.Skipping())
                {
                    line.erase(0, line.size());
                    break;
                }
                else
                {
                    if (define.Process(line) != INT_MIN+1)
                        break;
                    last = line;
                }
            }
        }
    }
    return true;
}
