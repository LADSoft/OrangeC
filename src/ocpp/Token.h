/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#ifndef Token_h
#define Token_h

#include <string>
#include <map>
#include "Floating.h"

typedef std::map<std::string, int> KeywordHash;

class Token
{
public:
    enum Type
    {
        t_int, t_unsignedint, t_longint, t_unsignedlongint, t_longlongint, t_unsignedlonglongint,
        t_float, t_double, t_longdouble
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
    virtual L_INT GetInteger() const { return 0; }
    virtual Type GetNumericType() const { return t_int; }
    virtual const FPF *GetFloat() const { return 0; }
    virtual int GetKeyword() const { return -1; }
    virtual const std::string &GetId() const { static std::string aa; return aa; }
    static bool Start(const std::string &line) { return false; }
    void SetChars(const std::string str) { chars = str; }
    std::string GetChars() const { return chars; }
	virtual ~Token() {}
protected:
    virtual void Parse(std::string &line) {}
private:
    std::string chars;
};
inline bool operator ==(const Token &tk, char *id) { return tk.GetId() == id; }
class StringToken : public Token
{
public:
    StringToken(std::string &line) : wide(false) { Parse(line); }
    virtual std::wstring GetString() const { return str; }
    virtual std::wstring GetRawString() const { return raw; }
    virtual bool IsString() const { return true; }
    virtual bool IsWide() const { return wide; }
    static bool Start(const std::string &line);
protected:
    virtual void Parse(std::string &line);
private:
    bool wide;
    bool doParse;
    std::wstring str;
    std::wstring raw;
} ;
class CharacterToken : public Token
{
    friend class StringToken;
public:
    CharacterToken(std::string &line) : value(0) { Parse(line); }
    virtual bool IsCharacter() const { return true; }
    virtual L_INT GetInteger() const { return value; }
    static bool Start(const std::string &line);	
    static void SetUnsigned(bool flag) { unsignedchar = flag; }
protected:
    virtual void Parse(std::string &line);
    static int QuotedChar(int len, const char **buf);
private:
    L_INT value;
    static bool unsignedchar;
} ;
class NumericToken : public Token
{
public:
    NumericToken(std::string &line) : intValue(0), type(t_int), parsedAsFloat(false) { Parse(line); }
    virtual bool IsNumeric() const { return true; }
    virtual bool IsFloating() const { return parsedAsFloat; }
    virtual L_INT GetInteger() const;
    virtual const FPF *GetFloat() const ;
    virtual Type GetNumericType() const { return type; }
    static bool Start(const std::string &line);
    static void SetAnsi(bool flag) { ansi = flag; }
    static void SetC99(bool flag) { c99 = flag; }
protected:
    virtual void Parse(std::string &line);
    int Radix36(char c);
    L_INT GetBase(int b, char **ptr);
    void GetFraction(int radix, char **ptr, FPF &rval);
    int GetExponent(char **ptr);
    void GetFloating(FPF &floatValue, int radix, char **ptr);
    int GetNumber(const char **ptr);
private:
    bool parsedAsFloat;
    L_INT intValue;
    FPF floatValue;
    Type type;
    static bool ansi;
    static bool c99;
} ;
class KeywordToken : public Token
{
public:
    KeywordToken(std::string &line, KeywordHash *table) : keyValue(-1), keywordTable(table) { Parse(line); }
    virtual bool IsKeyword() const { return keyValue != -1; }
    virtual int GetKeyword() const { return keyValue; }
    virtual bool IsError() const { return keyValue == -1; }
    static bool Start(const std::string &line);
protected:
    virtual void Parse(std::string &line);
private:
    int keyValue;
    KeywordHash *keywordTable;
} ;
class IdentifierToken : public Token
{
public:
    IdentifierToken(std::string &line, KeywordHash *Table, bool CaseInsensitive) 
    : keyValue(-1), keywordTable(Table), caseInsensitive(CaseInsensitive) { Parse(line); }
    virtual bool IsIdentifier() const { return keyValue == -1; }
    virtual bool IsKeyword() const { return keyValue != -1; }
    virtual int GetKeyword() const { return keyValue; }
    virtual const std::string &GetId() const { return id; }
    static bool Start(const std::string &line);
protected:
    virtual void Parse(std::string &line);
private:
    int keyValue;
    bool parseKeyword;
    KeywordHash *keywordTable;
    std::string id;
    bool caseInsensitive;
} ;
class EndToken : public Token
{
public:
    EndToken() { }
    virtual bool IsEnd() const { return true; }
} ;
class ErrorToken : public Token
{
public:
    ErrorToken(std::string &line) { Parse(line); }
    virtual bool IsError() const { return true; }
protected:
    virtual void Parse(std::string &line);
private:
    int ch;
} ;
class Tokenizer
{
public:
    Tokenizer(const std::string &Line, KeywordHash *Table) : line(Line), keywordTable(Table),
        currentToken(nullptr), caseInsensitive(false) { }
    virtual ~Tokenizer() {  delete currentToken; }
    void Reset(const std::string &Line) { line = Line; delete currentToken; currentToken = nullptr; }
    const Token *Next() ;
    std::string &GetString() { return line; }
    static void SetUnsigned(bool flag) { CharacterToken::SetUnsigned(flag); }
    static void SetAnsi(bool flag) { NumericToken::SetAnsi(flag); }
    static void SetC99(bool flag) { NumericToken::SetC99(flag); }
    void SetCaseInsensitive(bool flag) { caseInsensitive = flag; }
private:
    KeywordHash *keywordTable;
    std::string line;
    Token *currentToken;
    bool caseInsensitive;
};

bool IsSymbolStartChar(const char *data);
bool IsSymbolChar(const char *data);

#endif