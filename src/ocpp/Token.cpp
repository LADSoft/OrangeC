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
bool NumericToken::ansi;
bool NumericToken::c99;

typedef unsigned long long L_UINT;

bool (*Tokenizer::IsSymbolChar)(const char*, bool) = Tokenizer::IsSymbolCharDefault;

bool Tokenizer::IsSymbolCharDefault(const char* data, bool startOnly)
{
    return *data == '_' || (startOnly ? UTF8::IsAlpha(data) : UTF8::IsAlnum(data));
}

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
long long NumericToken::GetInteger() const
{
    if (parsedAsFloat)
    {
        return (long long)floatValue;
    }
    return intValue;
}
const FPF* NumericToken::GetFloat() const
{
    if (!parsedAsFloat)
        switch (type)
        {
            case t_int:
            case t_longint:
            case t_longlongint:
                const_cast<FPF&>(floatValue) = (long long)intValue;
                break;
            default:
                const_cast<FPF&>(floatValue) = (unsigned long long)intValue;
                break;
        }
    return &floatValue;
}
bool NumericToken::Start(const std::string& line) { return isdigit(line[0]) || (line[0] == '.' && isdigit(line[1])); }
int NumericToken::Radix36(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return INT_MAX;
}
long long NumericToken::GetBase(int b, char** ptr)
{
    long long i;
    int j;
    int errd = 0;
    i = 0;
    while ((j = Radix36(**ptr)) < b)
    {
        (*ptr)++;
        if (i > (llminus1 - j) / b)
            if (!errd)
            {
                Errors::Error("Constant too large");
                errd++;
            }
        i = i * b + j;
    }
    return i;
}

void NumericToken::GetFraction(int radix, char** ptr, FPF& floatValue)
{
    int pow = -1;
    while (Radix36(**ptr) < radix)
    {
        FPF temp, temp1;
        temp = Radix36(*(*ptr)++);
        if (radix == 10)
            temp.MultiplyPowTen(pow--);
        else
        {
            temp.SetExp(temp.GetExp() + 4 * pow--);
        }
        floatValue += temp;
    }
}

int NumericToken::GetExponent(char** ptr)
{
    bool neg = false;
    int intValue;
    if (**ptr == '-')
    {
        neg = true;
        (*ptr)++;
    }
    else
    {
        if (**ptr == '+')
            (*ptr)++;
    }
    intValue = GetBase(10, ptr);
    if (neg)
        intValue = -intValue;
    return intValue;
}
void NumericToken::GetFloating(FPF& floatValue, int radix, char** ptr)
{
    FPF fradix;
    fradix = radix;
    while (**ptr && Radix36(**ptr) < radix)
    {
        floatValue *= fradix;
        FPF temp;
        temp = Radix36(*(*ptr)++);
        floatValue += temp;
    }
    if (**ptr == '.')
    {
        (*ptr)++;
        GetFraction(radix, ptr, floatValue);
        intValue = 0;
    }
    if (**ptr == 'e' || **ptr == 'E' || **ptr == 'p' || **ptr == 'P')
    {
        (*ptr)++;
        int intValue = GetExponent(ptr);
        /* floating point too large goes to infinity... */
        if (radix == 2)
        {
            floatValue.SetExp(floatValue.GetExp() + intValue);
        }
        else
        {
            floatValue.MultiplyPowTen(intValue);
        }
    }
}

int NumericToken::GetNumber(const char** ptr)
{
    char buf[256];
    char* p = buf;
    int radix = 10;
    int floatradix = 0;
    bool hasdot = false;
    bool floating = false;
    if (!isdigit(**ptr) && **ptr != '.')
        return INT_MIN;
    if (**ptr == '.' && !isdigit(*(*ptr + 1)))
        return INT_MIN;
    if (**ptr == '0')
    {
        (*ptr)++;
        if (**ptr == 'x' || **ptr == 'X')
        {
            (*ptr)++;
            radix = 16;
        }
        else
            radix = 8;
    }
    else if (**ptr == '$')
    {
        radix = 16;
        (*ptr)++;
    }
    while (Radix36(**ptr) < radix || Radix36(**ptr) < 16)
    {
        *p++ = **ptr;
        (*ptr)++;
    }
    if (**ptr == '.')
    {
        hasdot = true;
        if (radix == 8)
            radix = 10;
        *p++ = **ptr;
        (*ptr)++;
        while (Radix36(**ptr) < radix)
        {
            *p++ = **ptr;
            (*ptr)++;
        }
    }
    if ((**ptr == 'e' || **ptr == 'E') && radix != 16)
        radix = floatradix = 10;
    else if ((**ptr == 'p' || **ptr == 'P') && radix == 16)
    {
        floatradix = 2;
    }
    else if (radix == 16 && hasdot)
    {
        Errors::Error("Invalid floating point constant");
    }

    if (floatradix)
    {
        *p++ = **ptr;
        (*ptr)++;
        if (**ptr == '-' || **ptr == '+')
        {
            *p++ = **ptr;
            (*ptr)++;
        }
        while (Radix36(**ptr) < 10)
        {
            *p++ = **ptr;
            (*ptr)++;
        }
    }

    *p = 0;
    if (!floatradix && radix != 16)
    {
        char* q = buf;
        if (**ptr == 'H' || **ptr == 'h')
        {
            radix = 16;
            (*ptr)++;
            while (*q)
                if (Radix36(*q++) >= 16)
                    return INT_MIN;
        }
    }
    p = buf;
    /* at this point the next char is any qualifier after the number*/

    if (hasdot || floatradix)
    {
        if (floatradix == 0)
            floatradix = radix;
        GetFloating(floatValue, floatradix, &p);
        floating = true;
    }
    else
    {
        if (Radix36(*p) < radix)
            intValue = GetBase(radix, &p);
        else
        {
            intValue = 0;
        }
    }
    if (!floating)
    {
        type = t_int;
        if (**ptr == 'i')
        {
            if (!ansi && (*ptr)[1] == '6' && (*ptr)[2] == '4')
            {
                (*ptr)++;
                (*ptr)++;
                (*ptr)++;
                type = t_longlongint;
            }
        }
        else if (**ptr == 'U' || **ptr == 'u')
        {
            (*ptr)++;
            type = t_unsignedint;
            if (**ptr == 'i')
            {
                if (!ansi && (*ptr)[1] == '6' && (*ptr)[2] == '4')
                {
                    (*ptr)++;
                    (*ptr)++;
                    (*ptr)++;
                    type = t_unsignedlonglongint;
                }
            }
            else if (**ptr == 'L' || **ptr == 'l')
            {
                (*ptr)++;
                type = t_unsignedlongint;
                if (c99 && (**ptr == 'L' || **ptr == 'l'))
                {
                    type = t_unsignedlonglongint;
                    (*ptr)++;
                }
            }
        }
        else if (**ptr == 'L' || **ptr == 'l')
        {
            type = t_longint;
            (*ptr)++;
            if (c99 && (**ptr == 'L' || **ptr == 'l'))
            {
                type = t_longlongint;
                (*ptr)++;
                if (**ptr == 'U' || **ptr == 'u')
                {
                    type = t_unsignedlonglongint;
                    (*ptr)++;
                }
            }

            else if (**ptr == 'U' || **ptr == 'u')
            {
                type = t_unsignedlongint;
                (*ptr)++;
            }
        }
        if (type == t_int) /* no qualifiers */
        {
            if (intValue > INT_MAX)
            {
                type = t_unsignedint;
                if (radix == 10 || (L_UINT)intValue > UINT_MAX)
                {
                    type = t_longint;
                    if (intValue > LONG_MAX)
                    {
                        type = t_unsignedlongint;
                        if (radix == 10 || (L_UINT)intValue > ULONG_MAX)
                        {
                            if (radix == 10)
                            {
                                type = t_longlongint;
                            }
                            else
                                type = t_unsignedlonglongint;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (**ptr == 'F' || **ptr == 'f')
        {
            type = t_float;
            (*ptr)++;
            floatValue.Truncate(FLT_MANT_DIG, FLT_MAX_EXP, FLT_MIN_EXP);
        }
        else if (**ptr == 'L' || **ptr == 'l')
        {
            type = t_longdouble;
            (*ptr)++;
            floatValue.Truncate(LDBL_MANT_DIG, LDBL_MAX_EXP, LDBL_MIN_EXP);
        }
        else
        {
            type = t_double;
            floatValue.Truncate(DBL_MANT_DIG, DBL_MAX_EXP, DBL_MIN_EXP);
        }
        parsedAsFloat = true;
    }
    if (Tokenizer::IsSymbolChar(*ptr, false))
    {
        Errors::Error("Invalid constant value");
        while (**ptr && Tokenizer::IsSymbolChar(*ptr, false))
        {
            int n = UTF8::CharSpan(*ptr);
            for (int i = 0; i < n && **ptr; i++)
                (*ptr)++;
        }
    }
    return type;
}
void NumericToken::Parse(std::string& line)
{
    const char* p = line.c_str();
    GetNumber(&p);
    SetChars(line.substr(0, p - line.c_str()));
    line.erase(0, p - line.c_str());
}
bool KeywordToken::Start(const std::string& line) { return ispunct(line[0]) != 0; }
void KeywordToken::Parse(std::string& line)
{
    char buf[256], *p = buf;
    int i;
    for (i = 0; i < line.size(); i++)
        if (ispunct(line[i]))
            *p++ = line[i];
        else
            break;
    *p = 0;
    if (keywordTable)
    {
        bool found = false;
        for (int j = i; j > 0; j--)
        {
            buf[j] = 0;
            auto it = keywordTable->find(std::string(buf));
            if (it != keywordTable->end())
            {
                SetChars(line.substr(0, j));
                line.erase(0, j);
                keyValue = (int)it->second;
                found = true;
                break;
            }
        }
        if (!found)
        {
            SetChars(line.substr(0, 1));
            line.erase(0, 1);
            keyValue = (int)kw::UNKNOWN;
        }
    }
}
bool IdentifierToken::Start(const std::string& line) { return Tokenizer::IsSymbolChar(line.c_str(), true); }
void IdentifierToken::Parse(std::string& line)
{
    char buf[256], *p = buf;
    int i, n;
    for (i = 0;
         (p == buf || p - buf - 1 < sizeof(buf)) && p - buf < line.size() && Tokenizer::IsSymbolChar(line.c_str() + i, false);)
    {
        n = UTF8::CharSpan(line.c_str() + i);
        for (int j = 0; j < n && i < line.size(); j++)
            *p++ = line[i++];
    }
    *p = 0;
    SetChars(line.substr(0, i));
    line.erase(0, i);
    id = buf;
    if (keywordTable)
    {
        KeywordHash::const_iterator it;
        if (caseInsensitive)
        {
            p = buf;
            while (*p)
                *p = toupper(*p), p++;
            std::string id1 = buf;
            it = keywordTable->find(id1);
        }
        else
        {
            it = keywordTable->find(id);
        }
        if (it != keywordTable->end())
            keyValue = (int)it->second;
    }
}
void ErrorToken::Parse(std::string& line)
{
    ch = line[0];
    SetChars(line.substr(0, 1));
    line.erase(0, 1);
}
const Token* Tokenizer::Next()
{
    size_t n = line.find_first_not_of("\t \v");
    line.erase(0, n);
    if (line.empty())
        currentToken = std::make_unique<EndToken>();
    else if (NumericToken::Start(line))
        currentToken = std::make_unique<NumericToken>(line);
    else if (CharacterToken::Start(line))
        currentToken = std::make_unique<CharacterToken>(line);
    else if (StringToken::Start(line))
        currentToken = std::make_unique<StringToken>(line);
    else if (IdentifierToken::Start(line))
        currentToken = std::make_unique<IdentifierToken>(line, keywordTable, caseInsensitive);
    else if (keywordTable && KeywordToken::Start(line))
        currentToken = std::make_unique<KeywordToken>(line, keywordTable);
    else
        currentToken = std::make_unique<ErrorToken>(line);
    return currentToken.get();
}
void Tokenizer::Reset(const std::string& Line)
{
    line = "";
    int last = 0;
    for (int p = 0; p < Line.size(); p++)
    {
        if (Line[p] == ppDefine::MACRO_PLACEHOLDER)
        {
            if (p != last)
                line += Line.substr(last, p - last);
            while (Line[p] == ppDefine::MACRO_PLACEHOLDER)
                p++;
            last = p;
        }
    }
    line += Line.substr(last);
    currentToken = nullptr;
}
