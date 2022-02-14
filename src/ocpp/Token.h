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

#ifndef Token_h
#define Token_h

#include <string>
#include <unordered_map>
#include <memory>
#include "Floating.h"

enum class kw;

typedef std::unordered_map<std::string, kw> KeywordHash;

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
class KeywordToken : public Token
{
  public:
    KeywordToken(std::string& line, KeywordHash* table) : keyValue(-1), keywordTable(table) { Parse(line); }
    virtual bool IsKeyword() const { return keyValue != -1; }
    virtual kw GetKeyword() const { return (kw)keyValue; }
    virtual bool IsError() const { return keyValue == -1; }
    static bool Start(const std::string& line);

  protected:
    virtual void Parse(std::string& line);

  private:
    int keyValue;
    KeywordHash* keywordTable;
};
class IdentifierToken : public Token
{
  public:
    IdentifierToken(std::string& line, KeywordHash* Table, bool CaseInsensitive) :
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
    KeywordHash* keywordTable;
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
class Tokenizer
{
  public:
    Tokenizer(const std::string& Line, KeywordHash* Table) :
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

    // I so want to use std::function here but this call has to be fast...
    static bool (*IsSymbolChar)(const char*, bool);

  protected:
    static bool IsSymbolCharDefault(const char* data, bool startOnly);

  private:
    KeywordHash* keywordTable;
    std::string line;
    std::unique_ptr<Token> currentToken;
    bool caseInsensitive;
};

#endif