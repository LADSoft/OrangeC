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

#include "PreProcessor.h"
#include "Errors.h"
#include "UTF8.h"
#include <fstream>
#include <limits.h>

void PreProcessor::InitHash()
{
    hash["define"] = DEFINE;
    hash["undef"] = UNDEF;
    hash["pragma"] = PRAGMA;
    hash["error"] = ERROR;
    hash["warning"] = WARNING;
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
bool PreProcessor::GetPreLine(std::string& line)
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
        if (last.size())
            line = last + " " + line;
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
                    line.erase(0, line.size());
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
                    if (define.Process(line) != INT_MIN + 1)
                        break;
                    last = line;
                }
            }
        }
    }
    return true;
}
