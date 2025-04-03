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

#ifndef REGEXP_H
#define REGEXP_H

typedef unsigned char UBYTE;

#include <deque>
#include <cctype>
#include <cstring>
#include <memory>
class RegExpContext;

class RegExpMatch
{
  public:
    enum
    {
        RE_M_WORD = 128,
        RE_M_IWORD = 129,
        RE_M_BWORD = 130,
        RE_M_EWORD = 131,
        RE_M_WORDCHAR = 132,
        RE_M_NONWORDCHAR = 133,
        RE_M_BBUFFER = 134,
        RE_M_EBUFFER = 135,
        RE_M_SOL = 136,
        RE_M_EOL = 137,
        RE_M_END = 138,

        M_MATCH = 144,
        M_START = 145,
        M_END = 146
    };
    RegExpMatch(UBYTE ch, bool caseSensitive = true) :
        matchStart(false), matchEnd(false), matchRange(-1), rl(-1), rh(-1), invalid(false)
    {
        memset(matches, 0, sizeof(matches));
        SetChar(ch, caseSensitive);
    }
    RegExpMatch(bool all = false) : matchStart(false), matchEnd(false), matchRange(-1), rl(-1), rh(-1), invalid(false)
    {
        memset(matches, 0, sizeof(matches));
        if (all)
            SetAll();
    }
    RegExpMatch(const char** name, bool caseSensitive) :
        matchStart(false), matchEnd(false), matchRange(-1), rl(-1), rh(-1), invalid(false)
    {
        memset(matches, 0, sizeof(matches));
        SetSet(name, caseSensitive);
    }
    RegExpMatch(int type, int range, bool caseSensitive) :
        matchStart(false), matchEnd(false), matchRange(range), rl(-1), rh(-1), invalid(false)
    {
        memset(matches, 0, sizeof(matches));
        SetChar(type, caseSensitive);
    }
    ~RegExpMatch() {}

    void SetChar(UBYTE ch, bool caseSensitive)
    {
        SetBit(ch);
        if (!caseSensitive)
        {
            if (isupper(ch))
                SetBit(tolower(ch));
            else
                SetBit(toupper(ch));
        }
    }
    void SetRange(UBYTE n, UBYTE m, bool caseSensitive)
    {
        for (int i = n; i < m; i++)
            SetChar(i, caseSensitive);
    }
    const char* SetClass(const char* name);
    void SetSet(const char** p, bool caseSensitive);
    void Complement()
    {
        for (int i = 32 / 8; i < 128 / 8; i++)
            matches[i] = ~matches[i];
    }
    void SetAll()
    {
        for (int i = 0; i < 256; i++)
            if (isgraph(i))
                SetBit(i);
    }
    bool IsValid() const { return !invalid; }
    void SetMatchStart(bool flag) { matchStart = flag; }
    bool GetMatchStart() const { return matchStart; }
    void SetMatchEnd(bool flag) { matchEnd = flag; }
    bool GetMatchEnd() const { return matchEnd; }
    void SetMatchRange(int val) { matchRange = val; }
    int GetMatchRange() const { return matchRange; }
    void SetInterval(int Rl, int Rh) { rl = Rl, rh = Rh; }
    int Matches(RegExpContext& context, const char* str);
    static void Init(bool caseSensitive);

    static void Reset(bool caseSensitive) { Init(caseSensitive); }

  protected:
    void SetBit(int n) { matches[n / 8] |= 1 << (n & 7); }
    void SetBits(int n, int m)
    {
        for (int i = n; i < m; i++)
            SetBit(i);
    }
    bool IsSet(const UBYTE* m, int n) const { return !!(m[n / 8] & (1 << (n & 7))); }
    bool IsSet(int n) const { return !!(matches[n / 8] & (1 << (n & 7))); }
    int MatchOne(RegExpContext& context, const char* str);
    bool MatchRange(RegExpContext& context, const char* str);

  private:
    bool matchStart;
    bool matchEnd;
    int matchRange;
    UBYTE matches[256 / 8];
    int rl, rh;
    bool invalid;
    static UBYTE wordChars[256 / 8];
    static bool initted;
};

class RegExpContext
{
    friend class RegExpMatch;

  public:
    RegExpContext(const char* exp, bool regular, bool caseSensitive, bool matchesWord) :
        caseSensitive(true), m_so(0), m_eo(0), invalid(false), beginning(nullptr)
    {
        matchStackTop = 0;
        Parse(exp, regular, caseSensitive, matchesWord);
    }
    ~RegExpContext() {}
    bool Match(int start, int len, const char* Beginning);

    bool IsValid() const { return !invalid; }

    int GetStart() const { return m_so; }
    int GetEnd() const { return m_eo; }

  protected:
    void Parse(const char* exp, bool regular, bool caseSensitive, bool matchesWord);
    int GetSpecial(char ch);
    int MatchOne(const char* str);
    void Clear();

  private:
    std::deque<std::unique_ptr<RegExpMatch>> matches;
    const char* beginning;
    int m_so;
    int m_eo;
    bool caseSensitive;
    bool invalid;
    int matchStack[10];
    int matchStackTop;
    int matchOffsets[10][2];
    int matchCount;
};

#endif
