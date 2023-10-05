/* Software License Agreement
 *
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#ifndef ppFile_h
#define ppFile_h

#include <string>
#include <cstdio>

#include "InputFile.h"
#include "ppCond.h"
#include "ppCommon.h"

template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppFile : public InputFile
{

  public:
    ppFile(bool fullname, bool Trigraph, bool extended, const std::string& Name, ppDefine<T, isSymbolChar>* define,
           ppCtx<T, isSymbolChar>& Ctx, bool isunsignedchar, Dialect dialect, bool asmpp, PipeArbitrator& piper,
           int directories_travelled = 0) :
        InputFile(fullname, Name, piper),
        trigraphs(Trigraph),
        extendedComment(extended),
        cond(isunsignedchar, dialect, extended, asmpp),
        ctx(Ctx),
        directoriesTraversed(directories_travelled)
    {
        cond.SetParams(define, &ctx);
    }
    virtual ~ppFile() {}
    virtual bool GetLine(std::string& line)
    {
        instr = 0;
        line = "";
        while (true)
        {
            char buf[LINE_WIDTH];
            if (!ReadLine(buf))
            {
                cond.CheckErrors();
                CheckErrors();
                return false;
            }
            StripComment(buf);
            if (trigraphs)
                StripTrigraphs(buf);
            char* p = (char*)strrchr(buf, '\\');
            if (p)
            {
                char* q = p;
                p++;
                while (*p)
                {
                    if (!isspace(*p))
                    {
                        break;
                    }
                    p++;
                }
                if (*p)
                {
                    line += buf;
                    break;
                }
                *q = '\0';
                line += buf;
            }
            else
            {
                line += buf;
                if (!inComment)
                    break;
            }
        }
        line += " ";  // trailing spaced needed for function argument matching in replacesegment
        return true;
    }

    bool Check(kw token, const std::string& line, int lineno) { return cond.Check(token, line, lineno); }
    bool Skipping() { return cond.Skipping(); }
    void Mark() { cond.Mark(); }
    void Drop() { cond.Drop(); }
    void Release() { cond.Release(); }
    int getDirsTravelled() { return directoriesTraversed; }

  protected:
    virtual int StripComment(char* line)
    {
        char *s = line, *e = s;
        while (*e)
        {
            if (!instr)
            {
                if (!inComment)
                {
                    if (*e == '/')
                    {
                        if (*(e + 1) == '*')
                        {
                            e += 2;
                            *s++ = ' ';
                            inComment = true;
                            commentLine = lineno;
                            continue;
                        }
                        else if (*(e + 1) == '/' && extendedComment)
                        {
                            *s = 0;
                            return strlen(line);
                        }
                    }
                    else if (*e == '"' || *e == '\'')
                        instr = *e;
                }
                else
                {
                    if (*e == '*')
                    {
                        if (*(e + 1) == '/')
                        {
                            inComment = false;
                            e++;
                        }
                    }
                    e++;
                    continue;
                }
            }
            else if (!inComment && *e == instr)
            {
                int count = 0;
                while (s - count > line && *(s - count - 1) == '\\')
                    count++;
                if (!(count & 1))
                    instr = L'\0';
            }
            *s++ = *e++;
        }
        *s = 0;
        return strlen(line);
    }

    void StripTrigraphs(char* line)
    {
        char* cp = line;
        while (*cp)
        {
            if (*cp == '?' && *(cp + 1) == '?')
            {
                cp += 2;
                switch (*cp++)
                {
                    case '=':
                        *line++ = '#';
                        break;
                    case '(':
                        *line++ = '[';
                        break;
                    case '/':
                        *line++ = '\\';
                        break;
                    case ')':
                        *line++ = ']';
                        break;
                    case '\'':
                        *line++ = '^';
                        break;
                    case '<':
                        *line++ = '{';
                        break;
                    case '!':
                        *line++ = '|';
                        break;
                    case '>':
                        *line++ = '}';
                        break;
                    case '-':
                        *line++ = '~';
                        break;
                    default:
                        cp -= 3;
                        *line++ = *cp++;
                        break;
                }
            }
            else
                *line++ = *cp++;
        }
        *line = 0;
    }

  private:
    bool trigraphs;
    bool extendedComment;
    ppCond<T, isSymbolChar> cond;
    ppCtx<T, isSymbolChar>& ctx;
    int directoriesTraversed = 0;
};
#endif