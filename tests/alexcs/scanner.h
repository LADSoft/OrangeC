#ifndef SCANNER
#define SCANNER

#include <istream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <string.h>
#include <sstream>
#include "exception.h"

using namespace std;

enum TokenType{
    IDENTIFIER,
    RESERVED_WORD,
    INT_CONST,
    REAL_CONST,
    STR_CONST,
    OPERATION,
    DELIMITER,
    END_OF_FILE,
    UNDEFINED
};

enum TokenValue{
    TOK_AND,
    TOK_ARRAY,
    TOK_BEGIN,
    TOK_BREAK,
    TOK_CASE,
    TOK_CONTINUE,
    TOK_CONST,
    TOK_DIV,
    TOK_DO,
    TOK_DOWNTO,
    TOK_ELSE,
    TOK_END,
    TOK_EXIT,
    TOK_FILE,
    TOK_FOR,
    TOK_FORWARD,
    TOK_FUNCTION,
    TOK_IF,
    TOK_IN,
    TOK_MOD,
    TOK_NIL,
    TOK_NOT,
    TOK_OF,
    TOK_OR,
    TOK_PROCEDURE,
    TOK_RECORD,
    TOK_REPEAT,
    TOK_SET,
    TOK_SHL,
    TOK_SHR,
    TOK_STRING,
    TOK_THEN,
    TOK_TO,
    TOK_TYPE,
    TOK_UNTIL,
    TOK_VAR,
    TOK_WHILE,
    TOK_WITH,
    TOK_XOR,
    TOK_DOUBLE_DOT,
    TOK_ASSIGN,
    TOK_MINUS,
    TOK_PLUS,
    TOK_MULT,
    TOK_DIVISION,
    TOK_BRACKETS_SQUARE_LEFT,
    TOK_BRACKETS_SQUARE_RIGHT,
    TOK_SEMICOLON,
    TOK_COLON,
    TOK_COMMA,
    TOK_DOT,
    TOK_CAP,
    TOK_DOG,
    TOK_BRACKETS_LEFT,
    TOK_BRACKETS_RIGHT,
    TOK_LESS,
    TOK_GREATER,
    TOK_EQUAL,
    TOK_LESS_OR_EQUAL,
    TOK_GREATER_OR_EQUAL,
    TOK_NOT_EQUAL,
    TOK_UNRESERVED,
    TOK_INTEGER, 
    TOK_REAL,
    TOK_WRITE,
    TOK_WRITELN
};

extern const string TOKEN_TO_STR[];
extern std::ostream& PrintSpaces(std::ostream& o, int offset);

class Token;

ostream& operator<<(ostream& out, const Token& token);

class ReservedWords{
private:
    map<string, pair<TokenType, TokenValue> > words;
    void Add(string name, TokenType type, TokenValue value);
public:
    ReservedWords();
    bool Identify(string& str, TokenType& returned_type, TokenValue& returned_value);
};

class Token{
protected:
    TokenType type;
    TokenValue value;
    int line;
    int pos;
    char* name;
public:
    bool IsRelationalOp() const;
    bool IsAddingOp() const;
    bool IsMultOp() const;
    bool IsUnaryOp() const;
    bool IsFactorOp() const;
    bool IsConst() const;
    bool IsVar() const;
    bool IsConstVar() const;
    bool IsBitwiseOp() const;
    Token();
    Token(const char* name_, TokenType type_, TokenValue value_, int line_ = -1, int pos_ = -1);
    Token(const Token& token);
    Token(TokenValue val);
    Token(int value_);
    Token(float value_);
    Token& operator=(const Token& token);
    ~Token();
    TokenType GetType() const;
    TokenValue GetValue() const;
    int GetPos() const;
    int GetLine() const;
    void NameToLowerCase();
    virtual const char* GetName() const;
    virtual int GetIntValue() const;
    virtual float GetRealValue() const;
    void ChangeSign();
};

class Scanner{
public:
    enum State {
        IDENTIFIER_ST,
        HEX_ST,
        INTEGER_ST,
        OPERATION_ST,
        EOF_ST,
        NONE_ST
    };
private:
    ReservedWords reserved_words;
    Token* currentToken;
    istream& in;
    string buffer;
    string buffer_low;
    int first_pos;
    int first_line;
    Token token;
    int line;
    int pos;
    char c;
    State state;
    void AddToBuffer(char c);
    void ReduceBuffer();
    void MakeToken(TokenType type, TokenValue value = TOK_UNRESERVED);
    void IdentifyAndMake();
    bool TryToIdentify();
    void Error(const char* msg) const;
    void ExtractChar();
    void EatLineComment();
    void EatBlockComment();
    void EatRealFractPart();
    void EatStrNum();
    void EatStrConst();
    void EatHex();
    void EatInteger();
    void EatIdentifier();
    void EatOperation();
public:
    Scanner(istream& input);
    Token GetToken();
    Token NextToken();
};

#endif
