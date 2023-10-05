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

#include "Token.h"
#include "Errors.h"
#include "SymbolTable.h"
#include "UTF8.h"
#include "ppExpr.h"
#include "ppDefine.h"
#include "Errors.h"
#include "ppInclude.h"
#include "ppkw.h"

#include <cfloat>
#include <climits>

bool CharacterToken::unsignedchar;

typedef unsigned long long L_UINT;


unsigned long long llminus1 = (unsigned long long)-1LL;

bool StringToken::Start(const std::string& line)
{
    if (line[0] == '"')
        return true;
    if (line[0] == 'L')
    {
        size_t n = line.find_first_not_of(' ', 1);
        if (n != std::string::npos && line[n] == '"')
            return true;
    }
    return false;
}
void StringToken::Parse(std::string& line)
{
    wchar_t buf[4000], *p = buf;
    wchar_t Raw[4000], *q = Raw;
    const char* start = line.c_str();
    if (start[0] == 'L')
    {
        wide = true;
        start++;
        while (isspace(*start))
            start++;
    }
    start++;  // past the quote
    while (*start && *start != '\"')
    {
        int n;
        const char* cur = start;
        n = *p++ = (unsigned char)CharacterToken::QuotedChar(1, &start);
        while (cur < start)
            *q++ = *cur++;
        if (p - buf == sizeof(buf) / sizeof(wchar_t) - 1 || n == 0)
        {
            *p = 0;
            str += buf;
            p = buf;
            // since a null terminates a normal string
            // we have to go through extra effort to add a '\0' character
            // to our wstring.
            if (n == 0)
            {
                str += std::wstring(L" ");
                str[str.size() - 1] = 0;
            }
        }
    }
    if (p != buf)
    {
        *p = 0;
        str += buf;
        *q = 0;
        raw += Raw;
    }
    if (*start != '"')
        Errors::Error("Unterminated string constant");
    else
        start++;
    SetChars(line.substr(0, start - line.c_str()));
    line.erase(0, start - line.c_str());
}
bool CharacterToken::Start(const std::string& line)
{
    if (line[0] == '\'')
        return true;
    if (line[0] == 'L')
    {
        size_t n = line.find_first_not_of(' ', 1);
        if (n != std::string::npos && line[n] == '\'')
            return true;
    }
    return false;
}
void CharacterToken::Parse(std::string& line)
{
    const char* start = line.c_str();
    bool wide = false;
    if (start[0] == 'L')
    {
        wide = true;
        start++;
        while (isspace(*start))
            start++;
    }
    start++;  // past the quote
    value = QuotedChar(2, &start);
    if (*start != '\'')
        Errors::Error("Character constant must be one or two characters");
    else
        start++;
    SetChars(line.substr(0, start - line.c_str()));
    line.erase(0, start - line.c_str());
}
int CharacterToken::QuotedChar(int bytes, const char** source)
{
    int i = *(*source)++, j;
    if (**source == '\n')
        return INT_MIN;
    if (i != '\\')
    {
        return (unsigned char)i;
    }
    i = *(*source); /* get an escaped character */
    if (isdigit(i) && i < '8')
    {
        for (i = 0, j = 0; j < 3; ++j)
        {
            if (*(*source) <= '7' && *(*source) >= '0')
                i = (i << 3) + *(*source) - '0';
            else
                break;
            (*source)++;
        }
        return i;
    }
    (*source)++;
    switch (i)
    {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'v':
            return '\v';
        case 't':
            return '\t';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case '\\':
            return '\\';
        case '?':
            return '?';
        case 'U':
            bytes = 4;
            goto dox;
        case 'u':
            bytes = 2;
            goto dox;
        case 'x':
        dox : {
            int n = 0;
            while (isxdigit(*(*source)))
            {
                int v = **source;
                if (v >= 0x60)
                    v &= 0xdf;
                v -= 0x30;

                if (v > 10)
                    v -= 7;
                n *= 16;
                n += v;
                (*source)++;
            }
            /* hexadecimal escape sequences are only terminated by a non hex char */
            /* we sign extend or truncate */
            if (bytes == 1)
            {
                if (unsignedchar)
                    n = (int)(unsigned char)n;
                else
                    n = (int)(char)n;
            }
            if (bytes == 2 && i == 'x')
                n = (int)(wchar_t)n;
            return n;
        }
        default:
            return (char)i;
    }
}
void ErrorToken::Parse(std::string& line)
{
    ch = line[0];
    SetChars(line.substr(0, 1));
    line.erase(0, 1);
}
