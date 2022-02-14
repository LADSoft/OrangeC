/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include "PreProcessor.h"
#include "ppkw.h"
#include "Errors.h"
#include "UTF8.h"
#include <fstream>
#include <climits>

void PreProcessor::InitHash()
{
    hash["define"] = kw::DEFINE;
    hash["undef"] = kw::UNDEF;
    hash["pragma"] = kw::PRAGMA;
    hash["error"] = kw::ERROR;
    hash["warning"] = kw::WARNING;
    hash["line"] = kw::LINE;
    hash["include"] = kw::INCLUDE;
    hash["include_next"] = kw::INCLUDE_NEXT;
    hash["if"] = kw::IF;
    hash["elif"] = kw::ELIF;
    hash["ifdef"] = kw::IFDEF;
    hash["ifndef"] = kw::IFNDEF;
    hash["else"] = kw::ELSE;
    hash["endif"] = kw::ENDIF;
    if (ppStart == '%')
    {
        hash["idefine"] = kw::IDEFINE;
        hash["iassign"] = kw::IASSIGN;
        hash["ifctx"] = kw::IFCTX;
        hash["elifctx"] = kw::ELIFCTX;
        hash["ifnctx"] = kw::IFNCTX;
        hash["elifnctx"] = kw::ELIFNCTX;
        hash["ifidn"] = kw::IFIDN;
        hash["elifidn"] = kw::ELIFIDN;
        hash["ifidni"] = kw::IFIDNI;
        hash["elifidni"] = kw::ELIFIDNI;
        hash["ifid"] = kw::IFID;
        hash["elifid"] = kw::ELIFID;
        hash["ifnum"] = kw::IFNUM;
        hash["elifnum"] = kw::ELIFNUM;
        hash["ifstr"] = kw::IFSTR;
        hash["elifstr"] = kw::ELIFSTR;
        hash["ifnidn"] = kw::IFNIDN;
        hash["elifnidn"] = kw::ELIFNIDN;
        hash["ifnidni"] = kw::IFNIDNI;
        hash["elifnidni"] = kw::ELIFNIDNI;
        hash["ifnid"] = kw::IFNID;
        hash["elifnid"] = kw::ELIFNID;
        hash["ifnnum"] = kw::IFNNUM;
        hash["elifnnum"] = kw::ELIFNNUM;
        hash["ifnstr"] = kw::IFNSTR;
        hash["elifnstr"] = kw::ELIFNSTR;
        hash["elifdef"] = kw::ELIFDEF;
        hash["elifndef"] = kw::ELIFNDEF;
        hash["assign"] = kw::ASSIGN;
        hash["rep"] = kw::REP;
        hash["endrep"] = kw::ENDREP;
        hash["exitrep"] = kw::EXITREP;
        hash["macro"] = kw::MACRO;
        hash["imacro"] = kw::IMACRO;
        hash["endmacro"] = kw::ENDMACRO;
        hash["rotate"] = kw::ROTATE;
        hash["push"] = kw::PUSH;
        hash["pop"] = kw::POP;
        hash["repl"] = kw::REPL;
    }
}
bool PreProcessor::GetPreLine(std::string& line)
{
    if (!preData.empty())
    {
        int npos = preData.find_first_of("\n");
        if (npos == std::string::npos)
        {
            line = preData;
            preData = "";
        }
        else
        {
            line = preData.substr(0, npos);
            preData.erase(0, npos + 1);
        }
        return true;
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
    for (cp = 0; cp < line.size() - 1; cp++)
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
            static char msg[2] = {0, 0};
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

bool PreProcessor::GetLine(std::string& line)
{
    std::string last;
    while (1)
    {
        if (!GetPreLine(line))
            return false;
        if (!last.empty())
            line = last + " " + line;
        origLine = line;
    preprocess:
        size_t n = line.find_first_not_of(" \n\t\v\r");
        if (n != std::string::npos)
        {
            if ((line[n] == ppStart && (n + 1 < line.length() && line[n + 1] != ppStart)) ||
                (trigraphs && n < line.size() - 1 && line[n] == '%' && line[n + 1] == ':'))
            {
                int n1 = line.find_first_not_of(" \n\t\v\r", n + 1);
                if (n1 != std::string::npos)
                {
                    if (!isdigit(line[n1]))
                    {
                        if (trigraphs)
                            line = StripDigraphs(line);
                        Tokenizer tk(line.substr(n + 1), &hash);
                        const Token* t = tk.Next();
                        if (t->IsKeyword())
                        {
                            if (t->GetKeyword() == kw::ASSIGN || t->GetKeyword() == kw::IASSIGN)
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
                                kw token = t->GetKeyword();
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
                    line.erase(0, line.size());
                }
            }
            else
            {
            join:
                if (ppStart != '%' && include.Skipping())
                {
                    line.erase(0, line.size());
                    break;
                }
                if (define.Process(line, true) != INT_MIN + 1)
                {
                    if (ppStart == '%')
                    {
                        size_t n = line.find_first_not_of(" \n\t\v\r");
                        if (n != std::string::npos)
                        {
                            if ((line[n] == ppStart && (n + 1 < line.length() && line[n + 1] != ppStart)) ||
                                (trigraphs && n < line.size() - 1 && line[n] == '%' && line[n + 1] == ':'))
                                goto preprocess;
                        }
                        if (include.Skipping())
                        {
                            line.erase(0, line.size());
                        }
                    }
                    break;
                }
                last = line;
            }
        }
    }
    return true;
}
