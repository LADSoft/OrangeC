/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include "RegExp.h"
#include <climits>
#include <cstdlib>

UBYTE RegExpMatch::wordChars[256 / 8];
bool RegExpMatch::initted;

void RegExpMatch::Init(bool caseSensitive)
{
    if (!initted)
    {
        memset(wordChars, 0, sizeof(wordChars));
        for (int i = 'A'; i <= 'Z'; i++)
            wordChars[i / 8] |= 1 << (i & 7);
        if (caseSensitive)
            for (int i = 'a'; i <= 'z'; i++)
                wordChars[i / 8] |= 1 << (i & 7);
        for (int i = '0'; i <= '9'; i++)
            wordChars[i / 8] |= 1 << (i & 7);
        wordChars['_' / 8] |= 1 << ('_' & 7);
        initted = true;
    }
}
const char* RegExpMatch::SetClass(const char* name)
{
    int i;
    if (!strcmp(name, "alpha"))
    {
        for (i = 0; i < 128; i++)
            if (isalpha(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "upper"))
    {
        for (i = 0; i < 128; i++)
            if (isupper(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "lower"))
    {
        for (i = 0; i < 128; i++)
            if (islower(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "digit"))
    {
        for (i = 0; i < 128; i++)
            if (isdigit(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "alnum"))
    {
        for (i = 0; i < 128; i++)
            if (isalnum(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "xdigit"))
    {
        for (i = 0; i < 128; i++)
            if (isxdigit(i))
                SetBit(i);
        name += 6;
    }
    else if (!strcmp(name, "space"))
    {
        for (i = 0; i < 128; i++)
            if (isspace(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "print"))
    {
        for (i = 0; i < 128; i++)
            if (isprint(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "punct"))
    {
        for (i = 0; i < 128; i++)
            if (ispunct(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "graph"))
    {
        for (i = 0; i < 128; i++)
            if (isgraph(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "cntrl"))
    {
        for (i = 0; i < 128; i++)
            if (iscntrl(i))
                SetBit(i);
        name += 5;
    }
    else if (!strcmp(name, "blank"))
    {
        for (i = 0; i < 128; i++)
            if (i == ' ' || i == '\t')
                SetBit(i);
        name += 5;
    }
    else
        invalid = true;
    return name;
}
void RegExpMatch::SetSet(const char** p, bool caseSensitive)
{
    bool lnot = false;
    UBYTE last[256 / 8];
    const char* str = *p;
    memcpy(last, matches, sizeof(last));
    memset(matches, 0, sizeof(matches));
    if (*str == '^')
    {
        lnot = true;
        str++;
    }
    if (*str == '-')
    {
        SetBit('-');
        str++;
    }
    while (!invalid && *str && *str != ']')
    {
        if (*str == '[' && str[1] == ':')
        {
            str = SetClass(str + 2);
            if (*str != ':' || str[1] == ']')
                invalid = true;
            str += 2;
        }
        if (*str == '\\')
        {
            str++;
            if (!*str)
            {
                invalid = true;
            }
            else
                SetBit(*str);
            str++;
        }
        else if (str[1] == '-')
        {
            if (str[0] > str[2])
                invalid = true;
            else
            {
                SetRange(str[0], str[2], caseSensitive);
                str += 3;
            }
        }
        else
            SetChar(*str++, caseSensitive);
    }
    if (*str != ']')
        invalid = true;
    else
        str++;
    if (!invalid && lnot)
    {
        for (int i = 32 / 8; i < 128 / 8; i++)
            matches[i] = ~matches[i] & last[i];
    }
    *p = str;
}
bool RegExpMatch::MatchRange(RegExpContext& context, const char* str)
{
    if (IsSet(M_START))
    {
        if (context.matchCount < 10)
        {
            context.matchStack[context.matchStackTop++] = context.matchCount;
            context.matchOffsets[context.matchCount][0] = str - context.beginning;
            context.matchOffsets[context.matchCount++][1] = 0;
            return true;
        }
    }
    if (IsSet(M_END))
    {
        if (context.matchStackTop)
        {
            context.matchOffsets[context.matchStack[--context.matchStackTop]][1] = str - context.beginning;
            return true;
        }
    }
    return false;
}
int RegExpMatch::MatchOne(RegExpContext& context, const char* str)
{
    if (MatchRange(context, str))
        return 0;
    if (*str && IsSet(*str))
        return 1;
    if (IsSet(M_MATCH))
    {
        if (matchRange < context.matchCount)
        {
            int n = context.matchOffsets[matchRange][1] - context.matchOffsets[matchRange][0];
            if (context.caseSensitive)
            {
                if (!strncmp(str, context.beginning + context.matchOffsets[matchRange][0], n))
                    return n;
            }
            else
            {
                bool matches = true;
                const char* p = context.beginning + context.matchOffsets[matchRange][0];
                const char* q = str;

                for (int i = 0; i < n && matches; i++)
                    if (toupper(*p++) != toupper(*q++))
                        matches = false;
                if (matches)
                    return n;
            }
        }
    }
    if (IsSet(RE_M_WORD))
    {
        if (IsSet(wordChars, *str) && !IsSet(wordChars, str[-1]))
        {
            return 0;
        }
        if (!IsSet(wordChars, *str) && IsSet(wordChars, str[-1]))
        {
            return 0;
        }
    }
    if (IsSet(RE_M_IWORD))
    {
        if (IsSet(wordChars, *str))
            if (IsSet(wordChars, str[-1]))
                if (IsSet(wordChars, str[1]))
                    return 0;
    }
    if (IsSet(RE_M_BWORD))
    {
        if (IsSet(wordChars, *str))
            if (!IsSet(wordChars, str[-1]))
            {
                return 0;
            }
    }
    if (IsSet(RE_M_EWORD))
    {
        if (!IsSet(wordChars, *str))
            if (IsSet(wordChars, str[-1]))
            {
                return 0;
            }
    }
    if (IsSet(RE_M_WORDCHAR))
    {
        if (IsSet(wordChars, *str))
        {
            return 1;
        }
    }
    if (IsSet(RE_M_NONWORDCHAR))
    {
        if (!IsSet(wordChars, *str))
        {
            return 1;
        }
    }
    if (IsSet(RE_M_BBUFFER))
        if (str == context.beginning)
            return 0;
    if (IsSet(RE_M_EBUFFER))
        if (!*str)
            return 0;
    if (IsSet(RE_M_SOL))
    {
        if (str == context.beginning)
            return 0;
        if (str[-1] == '\n')
            return 0;
    }
    if (IsSet(RE_M_EOL))
    {
        if (*str == '\n' || !*str)
            return 0;
    }
    return -1;
}
int RegExpMatch::Matches(RegExpContext& context, const char* str)
{
    if (rl >= 0 && rh >= 0)
    {
        int n, m = 0;
        int count = 0;
        n = MatchOne(context, str);
        while (n > 0)
        {
            m += n;
            n = MatchOne(context, str + m);
            count++;
        }
        if (m < 0)
            return -m;
        if (count >= rl && count <= rh)
        {
            if (count != 0 || MatchOne(context, str - 1) == -1)
                return m;
        }
        if (m)
            return -m;
        return -1;
    }
    return MatchOne(context, str);
}
void RegExpContext::Clear()
{
    matches.clear();
    matchStackTop = 0;
    m_so = m_eo = 0;
    matchCount = 0;
}
int RegExpContext::GetSpecial(char ch)
{
    switch (ch)
    {
        case 'b':
            return RegExpMatch::RE_M_WORD;
        case 'B':
            return RegExpMatch::RE_M_IWORD;
        case 'w':
            return RegExpMatch::RE_M_BWORD;
        case 'W':
            return RegExpMatch::RE_M_EWORD;
        case '<':
            return RegExpMatch::RE_M_WORDCHAR;
        case '>':
            return RegExpMatch::RE_M_NONWORDCHAR;
        case '`':
            return RegExpMatch::RE_M_BBUFFER;
        case '\'':
            return RegExpMatch::RE_M_EBUFFER;
        default:
            return ch;
    }
}
void RegExpContext::Parse(const char* exp, bool regular, bool CaseSensitive, bool matchesWord)
{
    invalid = false;
    caseSensitive = CaseSensitive;
    Clear();
    if (matchesWord)
    {
        matches.push_back(std::make_shared<RegExpMatch>(RegExpMatch::RE_M_BWORD, caseSensitive));
    }
    if (regular)
    {
        std::shared_ptr<RegExpMatch>  lastMatch(nullptr);
        while (!invalid && *exp)
        {
            std::shared_ptr<RegExpMatch> currentMatch;
            switch (*exp)
            {
                case '.':
                    currentMatch = std::make_shared<RegExpMatch>(true);
                    exp++;
                    break;
                case '*':
                    if (lastMatch)
                    {
                        lastMatch->SetInterval(0, INT_MAX);
                        lastMatch.reset();
                    }
                    else
                        invalid = true;
                    exp++;
                    break;
                case '+':
                    if (lastMatch)
                    {
                        lastMatch->SetInterval(1, INT_MAX);
                        lastMatch.reset();
                    }
                    else
                        invalid = true;
                    exp++;
                    break;
                case '?':
                    if (lastMatch)
                    {
                        lastMatch->SetInterval(0, 1);
                        lastMatch.reset();
                    }
                    else
                        invalid = true;
                    exp++;
                    break;
                case '[':
                    currentMatch = std::make_shared<RegExpMatch>(&exp, caseSensitive);
                    invalid = !currentMatch->IsValid();
                    break;
                case '^':
                    currentMatch = std::make_shared<RegExpMatch>(RegExpMatch::RE_M_SOL, caseSensitive);
                    exp++;
                    break;
                case '$':
                    currentMatch = std::make_shared<RegExpMatch>(RegExpMatch::RE_M_EOL, caseSensitive);
                    exp++;
                    break;
                case '\\':
                    switch (*++exp)
                    {
                        case '{':
                            exp++;
                            if (!lastMatch || !isdigit(*exp))
                            {
                                invalid = true;
                            }
                            else
                            {
                                int n1, n2;
                                n1 = n2 = std::atoi(exp);
                                while (isdigit(*exp))
                                    exp++;
                                if (*exp == ',')
                                {
                                    exp++;
                                    if (!isdigit(*exp))
                                        invalid = true;
                                    else
                                    {
                                        n2 = std::atoi(exp);
                                        while (isdigit(*exp))
                                            exp++;
                                    }
                                }
                                if (*exp != '\\' || exp[1] != '}' || n2 < n1)
                                    invalid = true;
                                else
                                {
                                    exp += 2;
                                    lastMatch->SetInterval(n1, n2);
                                }
                            }
                            break;
                        case '(':
                            exp++;
                            if (matchCount >= 10)
                                invalid = true;
                            else
                            {
                                matchStack[matchStackTop++] = matchCount;
                                currentMatch = std::make_shared<RegExpMatch>(RegExpMatch::M_START, matchCount++);
                            }
                            break;
                        case ')':
                            exp++;
                            if (!matchStackTop)
                            {
                                invalid = true;
                            }
                            else
                            {
                                currentMatch = std::make_shared<RegExpMatch>(RegExpMatch::M_END, matchStack[-matchStackTop]);
                            }
                            break;
                        default:
                            if (isdigit(*exp))
                            {
                                currentMatch = std::make_shared<RegExpMatch>(RegExpMatch::M_MATCH, *exp++ - '0', caseSensitive);
                            }
                            else
                            {
                                currentMatch = std::make_shared<RegExpMatch>(GetSpecial(*exp++), caseSensitive);
                            }
                            break;
                    }
                    break;
                case '|':
                    if (!lastMatch)
                    {
                        invalid = true;
                    }
                    else
                    {
                        switch (*++exp)
                        {
                            case '\\':
                                exp++;
                                lastMatch->SetChar(GetSpecial(*exp++), caseSensitive);
                                lastMatch.reset();
                                break;
                            case '[':
                                exp++;
                                lastMatch->SetSet(&exp, caseSensitive);
                                if (!lastMatch->IsValid())
                                    invalid = true;
                                break;
                            default:
                                lastMatch->SetChar(*exp++, caseSensitive);
                                break;
                        }
                    }
                    lastMatch = nullptr;
                    break;
                default:
                    currentMatch = std::make_shared<RegExpMatch>(*exp++, caseSensitive);
                    break;
            }
            lastMatch = currentMatch;
            if (currentMatch)
                matches.push_back(std::move(currentMatch));
        }
    }
    else
    {
        while (*exp)
        {
            matches.push_back(std::make_shared<RegExpMatch>(*exp++, caseSensitive));
        }
    }
    if (!invalid && matchesWord)
    {
        matches.push_back(std::make_shared<RegExpMatch>(RegExpMatch::RE_M_EWORD, caseSensitive));
    }
}
int RegExpContext::MatchOne(const char* str)
{
    int m = 0;
    RegExpMatch::Reset(caseSensitive);
    matchCount = 0;
    matchStackTop = 0;
    for (auto& match : matches)
    {
        int n = match->Matches(*this, str + m);
        if (n < 0)
            return n;
        m += n;
    }
    return m;
}
bool RegExpContext::Match(int start, int len, const char* Beginning)
{
    beginning = Beginning;
    const char* str = Beginning + start;
    const char* end = str + len;
    matchStackTop = 0;
    while (*str && str < end)
    {
        int n = MatchOne(str);
        if (n >= 0)
        {
            m_so = str - beginning;
            m_eo = str - beginning + n;
            return true;
        }
        else
        {
            str += -n;
        }
    }
    return false;
}
