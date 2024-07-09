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

#pragma once

#include "forwarddecls.h"
#include "ppMacroStates.h"
#include <string>
#include <unordered_map>
#include <memory>
#include "Floating.h"
#include <UTF8.h>
#include "TokenSettings.h"
#include "Errors.h"
constexpr unsigned long long llminus1 = (unsigned long long)-1LL;

typedef unsigned long long L_UINT;

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
template <typename T>
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
template <typename T>
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
    std::shared_ptr<Token> NextShared();
    std::string GetString() { return line; }
    void SetString(const std::string& Line) { line = Line; }
    static void SetUnsigned(bool flag) { CharacterToken::SetUnsigned(flag); }
    static void SetAnsi(bool flag) { NumericToken::SetAnsi(flag); }
    static void SetC99(bool flag) { NumericToken::SetC99(flag); }
    void SetCaseInsensitive(bool flag) { caseInsensitive = flag; }

  private:
    KeywordTable<T>* keywordTable;
    std::string line;
    std::shared_ptr<Token> currentToken;
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
            keyValue = (int)-1;
        }
    }
}
template <typename T>
bool IdentifierToken<T>::Start(const std::string& line)
{
    return TokenizerSettings::Instance()->GetSymbolCheckFunction()(line.c_str(), true);
}
template <typename T>
void IdentifierToken<T>::Parse(std::string& line)
{
    char buf[256], *p = buf;
    int i, n;
    for (i = 0; (p == buf || p - buf - 1 < sizeof(buf)) && p - buf < line.size() &&
                TokenizerSettings::Instance()->GetSymbolCheckFunction()(line.c_str() + i, false);)
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
        typename KeywordTable<T>::const_iterator it;
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

template <typename T>
const Token* Tokenizer<T>::Next()
{
    size_t n = line.find_first_not_of("\t \v");
    line.erase(0, n);
    if (line.empty())
        currentToken = std::make_shared<EndToken>();
    else if (NumericToken::Start(line))
        currentToken = std::make_shared<NumericToken>(line);
    else if (CharacterToken::Start(line))
        currentToken = std::make_shared<CharacterToken>(line);
    else if (StringToken::Start(line))
        currentToken = std::make_shared<StringToken>(line);
    else if (IdentifierToken<T>::Start(line))
        currentToken = std::make_shared<IdentifierToken<T>>(line, keywordTable, caseInsensitive);
    else if (keywordTable && KeywordToken<T>::Start(line))
        currentToken = std::make_shared<KeywordToken<T>>(line, keywordTable);
    else
        currentToken = std::make_shared<ErrorToken>(line);
    return currentToken.get();
}
template<typename T>
std::shared_ptr<Token> Tokenizer<T>::NextShared()
{
    Next();
    //
    return currentToken;
}
template <typename T>
void Tokenizer<T>::Reset(const std::string& Line)
{
    line = "";
    int last = 0;
    for (int p = 0; p < Line.size(); p++)
    {
        if (Line[p] == MacroStates::MACRO_PLACEHOLDER)
        {
            if (p != last)
                line += Line.substr(last, p - last);
            while (Line[p] == MacroStates::MACRO_PLACEHOLDER)
                p++;
            last = p;
        }
    }
    line += Line.substr(last);
    currentToken = nullptr;
}
