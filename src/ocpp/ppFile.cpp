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

#include "ppFile.h"
bool ppFile::GetLine(std::string& line)
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
        char* p = strrchr(buf, '\\');
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
                line = line + buf;
                break;
            }
            *q = '\0';
            line = line + buf;
        }
        else
        {
            line = line + buf;
            if (!inComment)
                break;
        }
    }
    line = line + " ";  // trailing spaced needed for function argument matching in replacesegment
    return true;
}
int ppFile::StripComment(char* line)
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
void ppFile::StripTrigraphs(char* line)
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
