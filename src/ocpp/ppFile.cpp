/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#include "ppFile.h"
bool ppFile::GetLine(std::string &line)
{
    line = std::string("");
    while (true)
    {
        char buf[LINE_WIDTH];
        errlineno = lineno + 1;
        if (!ReadLine(buf))
        {
            cond.CheckErrors();
            return false;
        }
        StripComment(buf);
        char *p = strrchr(buf, '\\');
        if (p)
        {
            char *q = p;
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
                line= line + buf;
                break;
            }
            *q = '\0';
            line = line + buf;
        }
        else 
        {
            line= line + buf;
            break;
        }
    }
    return true;
}
int ppFile::StripComment(char *line)
{
    char *s = line,  *e = s;
    char instr = 0;
    while (*e)
    {
        if (!instr)
        {
            if (!inComment)
            {
                if (*e == '/')
                {
                    if (*(e+1) == '*')
                    {
                        e += 2;
                        *s++ = ' ';
                        inComment = true;
                        commentLine = lineno;
                        continue;
                    }
                    else if (*(e+1) == '/' && extendedComment)
                    {
//                        *s++ = '\n';
                        *s = 0;
                        return strlen(line);
                    }
                }
                else
                    if (*e == '"' ||  *e == '\'')
                        instr =  *e;
            }
            else
            {
                if (*e == '*')
                {
                    if (*(e+1) == '/')
                    {
                        inComment = false;
                        e++;
                    }
                }
                e++;
                continue;
            }
        }
        else
        if (!inComment &&  *e == instr)
        {
            int count = 0;
            while (s - count > line && *(s - count - 1) == '\\')
                count++;
            if (!(count &1))
                instr = L'\0';
        }
        *s++ =  *e++;
    }
    *s = 0;
    return strlen(line);
}
void ppFile::StripTrigraphs(char *line)
{
    char *cp = line;
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
                    *line++ =  *cp++;
                    break;
            }
        }
        else
             *line++ =  *cp++;
    }
    *line = 0;
}
