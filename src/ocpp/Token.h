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

#ifndef Token_h
#define Token_h
/**
 * Quick summary of what's going on with the templates here, in general we should only *EVER* specialize Tokenizer as that is
 */
#include <string>
#include <unordered_map>
#include <memory>
#include "Floating.h"
#include <UTF8.h>
enum class kw;
template <typename T>
using KeywordTable = std::unordered_map<std::string, T>;
using KeywordHash = KeywordTable<kw>;
constexpr bool IsSymbolCharOrDefault(const char* data, bool startOnly)
{
    switch (*data)
    {
        case '_':
        case '$':
            return true;
        default:
            return (startOnly ? UTF8::IsAlpha(data) : UTF8::IsAlnum(data));
    }
}
class Token
{
  public:
    enum Type
    {
        t_int,
        t_unsignedint,
        t_longint,
        t_unsignedlongint,
        t_longlongint,
        t_unsignedlonglongint,
        t_float,
        t_double,
        t_longdouble
    };
    virtual bool IsNumeric() const { return false; }
    virtual bool IsFloating() const { return false; }
    virtual bool IsKeyword() const { return false; }
    virtual bool IsCharacter() const { return false; }
    virtual bool IsString() const { return false; }
    virtual bool IsIdentifier() const { return false; }
    virtual bool IsEnd() const { return false; }
    virtual bool IsError() const { return false; }
    virtual bool IsWide() const { return false; }
    virtual std::wstring GetString() const { return L""; }
    virtual std::wstring GetRawString() const { return L""; }
    virtual long long GetInteger() const { return 0; }
    virtual Type GetNumericType() const { return t_int; }
    virtual const FPF* GetFloat() const { return 0; }
    virtual kw GetKeyword() const { return (kw)-1; }
    virtual const std::string& GetId() const
    {
        static std::string aa;
        return aa;
    }
    static bool Start(const std::string& line) { return false; }
    void SetChars(const std::string str) { chars = str; }
    std::string GetChars() const { return chars; }
    virtual ~Token() {}

  protected:
    virtual void Parse(std::string& line) {}

  private:
    std::string chars;
};
inline bool operator==(const Token& tk, char* id) { return tk.GetId() == id; }
class StringToken : public Token
{
  public:
    StringToken(std::string& line) : wide(false), doParse(true) { Parse(line); }
    virtual std::wstring GetString() const { return str; }
    virtual std::wstring GetRawString() const { return raw; }
    virtual bool IsString() const { return true; }
    virtual bool IsWide() const { return wide; }
    static bool Start(const std::string& line);

  protected:
    virtual void Parse(std::string& line);

  private:
    bool wide;
    bool doParse;
    std::wstring str;
    std::wstring raw;
};
class CharacterToken : public Token
{
    friend class StringToken;

  public:
    CharacterToken(std::string& line) : value(0) { Parse(line); }
    virtual bool IsCharacter() const { return true; }
    virtual long long GetInteger() const { return value; }
    static bool Start(const std::string& line);
    static void SetUnsigned(bool flag) { unsignedchar = flag; }

  protected:
    virtual void Parse(std::string& line);
    static int QuotedChar(int len, const char** buf);

  private:
    long long value;
    static bool unsignedchar;
};
template <bool(isSymbolChar)(const char*, bool) = IsSymbolCharOrDefault>
class NumericToken : public Token
{
  public:
    NumericToken(std::string& line) : intValue(0), type(t_int), parsedAsFloat(false) { Parse(line); }
    virtual bool IsNumeric() const { return true; }
    virtual bool IsFloating() const { return parsedAsFloat; }
    virtual long long GetInteger() const;
    virtual const FPF* GetFloat() const;
    virtual Type GetNumericType() const { return type; }
    static bool Start(const std::string& line);
    static void SetAnsi(bool flag) { ansi = flag; }
    static void SetC99(bool flag) { c99 = flag; }

  protected:
    virtual void Parse(std::string& line);
    int Radix36(char c);
    long long GetBase(int b, char** ptr);
    void GetFraction(int radix, char** ptr, FPF& rval);
    int GetExponent(char** ptr);
    void GetFloating(FPF& floatValue, int radix, char** ptr);
    int GetNumber(const char** ptr);

  private:
    bool parsedAsFloat;
    long long intValue;
    FPF floatValue;
    Type type;
    static bool ansi;
    static bool c99;
};
template <typename T>
class KeywordToken : public Token
{
  public:
    KeywordToken(std::string& line, KeywordTable<T>* table) : keyValue(-1), keywordTable(table) { Parse(line); }
    virtual bool IsKeyword() const { return keyValue != -1; }
    virtual kw GetKeyword() const { return (kw)keyValue; }
    virtual bool IsError() const { return keyValue == -1; }
    static bool Start(const std::string& line);

  protected:
    virtual void Parse(std::string& line);

  private:
    int keyValue;
    KeywordTable<T>* keywordTable;
};
template <typename T, bool(isSymbolChar)(const char*, bool) = IsSymbolCharOrDefault>
class IdentifierToken : public Token
{
  public:
    IdentifierToken(std::string& line, KeywordTable<T>* Table, bool CaseInsensitive) :
        keyValue(-1), keywordTable(Table), caseInsensitive(CaseInsensitive), parseKeyword(true)
    {
        Parse(line);
    }
    virtual bool IsIdentifier() const { return keyValue == -1; }
    virtual bool IsKeyword() const { return keyValue != -1; }
    virtual kw GetKeyword() const { return (kw)keyValue; }
    virtual const std::string& GetId() const { return id; }
    static bool Start(const std::string& line);

  protected:
    virtual void Parse(std::string& line);

  private:
    int keyValue;
    bool parseKeyword;
    KeywordTable<T>* keywordTable;
    std::string id;
    bool caseInsensitive;
};
class EndToken : public Token
{
  public:
    EndToken() {}
    virtual bool IsEnd() const { return true; }
};
class ErrorToken : public Token
{
  public:
    ErrorToken(std::string& line) { Parse(line); }
    virtual bool IsError() const { return true; }

  protected:
    virtual void Parse(std::string& line);

  private:
    int ch;
};

// default out to our normal schema for now...
template <typename T, bool(isSymbolChar)(const char*, bool) = IsSymbolCharOrDefault>
class Tokenizer
{
  public:
    Tokenizer(const std::string& Line, KeywordTable<T>* Table) :
        line(Line), keywordTable(Table), currentToken(nullptr), caseInsensitive(false)
    {
    }
    virtual ~Tokenizer() {}
    void Reset(const std::string& Line);
    const Token* Next();
    std::string GetString() { return line; }
    void SetString(const std::string& Line) { line = Line; }
    static void SetUnsigned(bool flag) { CharacterToken::SetUnsigned(flag); }
    static void SetAnsi(bool flag) { NumericToken::SetAnsi(flag); }
    static void SetC99(bool flag) { NumericToken::SetC99(flag); }
    void SetCaseInsensitive(bool flag) { caseInsensitive = flag; }

  private:
    KeywordTable<T>* keywordTable;
    std::string line;
    std::unique_ptr<Token> currentToken;
    bool caseInsensitive;
};
template <typename T>
bool KeywordToken<T>::Start(const std::string& line)
{
    return ispunct(line[0]) != 0;
}
template <typename T>
void KeywordToken<T>::Parse(std::string& line)
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
template <typename T, bool(isSymbolChar)(const char*, bool)>
bool IdentifierToken<T, isSymbolChar>::Start(const std::string& line)
{
    return isSymbolChar(line.c_str(), true);
}
template <typename T, bool(isSymbolChar)(const char*, bool)>
void IdentifierToken<T, isSymbolChar>::Parse(std::string& line)
{
    char buf[256], *p = buf;
    int i, n;
    for (i = 0; (p == buf || p - buf - 1 < sizeof(buf)) && p - buf < line.size() && isSymbolChar(line.c_str() + i, false);)
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

template <typename T, bool(isSymbolChar)(const char*, bool)>
const Token* Tokenizer<T, isSymbolChar>::Next()
{
    size_t n = line.find_first_not_of("\t \v");
    line.erase(0, n);
    if (line.empty())
        currentToken = std::make_unique<EndToken>();
    else if (NumericToken::Start(line))
        currentToken = std::make_unique<NumericToken<isSymbolChar>>(line);
    else if (CharacterToken::Start(line))
        currentToken = std::make_unique<CharacterToken>(line);
    else if (StringToken::Start(line))
        currentToken = std::make_unique<StringToken>(line);
    else if (IdentifierToken::Start(line))
        currentToken = std::make_unique<IdentifierToken<T, isSymbolChar>>(line, keywordTable, caseInsensitive);
    else if (keywordTable && KeywordToken::Start(line))
        currentToken = std::make_unique<KeywordToken<T>>(line, keywordTable);
    else
        currentToken = std::make_unique<ErrorToken>(line);
    return currentToken.get();
}
template <typename T, bool(isSymbolChar)(const char*, bool)>
void Tokenizer<T, isSymbolChar>::Reset(const std::string& Line)
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
template <bool(isSymbolChar)(const char*, bool)>
long long NumericToken<isSymbolChar>::GetInteger() const
{
    if (parsedAsFloat)
    {
        return (long long)floatValue;
    }
    return intValue;
}
template <bool(isSymbolChar)(const char*, bool)>
const FPF* NumericToken<isSymbolChar>::GetFloat() const
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
template <bool(isSymbolChar)(const char*, bool)>
bool NumericToken<isSymbolChar>::Start(const std::string& line)
{
    return isdigit(line[0]) || (line[0] == '.' && isdigit(line[1]));
}
template <bool(isSymbolChar)(const char*, bool)>
int NumericToken<isSymbolChar>::Radix36(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return INT_MAX;
}
template <bool(isSymbolChar)(const char*, bool)>
long long NumericToken<isSymbolChar>::GetBase(int b, char** ptr)
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
template <bool(isSymbolChar)(const char*, bool)>
void NumericToken<isSymbolChar>::GetFraction(int radix, char** ptr, FPF& floatValue)
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
template <bool(isSymbolChar)(const char*, bool)>
int NumericToken<isSymbolChar>::GetExponent(char** ptr)
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
template <bool(isSymbolChar)(const char*, bool)>
void NumericToken<isSymbolChar>::GetFloating(FPF& floatValue, int radix, char** ptr)
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
template <bool(isSymbolChar)(const char*, bool)>

int NumericToken<isSymbolChar>::GetNumber(const char** ptr)
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
    if (isSymbolChar(*ptr, false))
    {
        Errors::Error("Invalid constant value");
        while (**ptr && isSymbolChar(*ptr, false))
        {
            int n = UTF8::CharSpan(*ptr);
            for (int i = 0; i < n && **ptr; i++)
                (*ptr)++;
        }
    }
    return type;
}
template <bool(isSymbolChar)(const char*, bool)>
void NumericToken<isSymbolChar>::Parse(std::string& line)
{
    const char* p = line.c_str();
    GetNumber(&p);
    SetChars(line.substr(0, p - line.c_str()));
    line.erase(0, p - line.c_str());
}
#endif