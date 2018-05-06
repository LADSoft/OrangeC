#include "scanner.h"

bool ishexnum(char c)
{
    return isdigit(c) || ('a' <= tolower(c)  && tolower(c) <= 'f');
}

int hex_str_to_i(char* str)
{
    int res = 0;
    sscanf(str, "%x", &res);
    return res;
}

ostream& PrintSpaces(ostream& o, int offset)
{
    for (int i = 0; i < offset; ++i)
        o << "  ";
    return o;
}


const char* const TOKEN_DESCRIPTION[] =
{
    "IDENTIFIER",
    "RESERVED_WORD",
    "INT_CONST",
    "REAL_CONST",
    "STR_CONST",
    "OPERATION",
    "DELIMITER",
    "END_OF_FILE"
};

const string TOKEN_VALUE_DESCRIPTION[] =
{
    "TOK_AND",
    "TOK_ARRAY",
    "TOK_BEGIN",
    "TOK_BREAK",
    "TOK_CASE",
    "TOK_CONTINUE",
    "TOK_CONST",
    "TOK_DIV",
    "TOK_DO",
    "TOK_DOWNTO",
    "TOK_ELSE",
    "TOK_END",
    "TOK_EXIT",
    "TOK_FILE",
    "TOK_FOR",
    "TOK_FORWARD",
    "TOK_FUNCTION",
    "TOK_IF",
    "TOK_IN",
    "TOK_MOD",
    "TOK_NIL",
    "TOK_NOT",
    "TOK_OF",
    "TOK_OR",
    "TOK_PROCEDURE",
    "TOK_RECORD",
    "TOK_REPEAT",
    "TOK_SET",
    "TOK_SHL",
    "TOK_SHR",
    "TOK_STRING",
    "TOK_THEN",
    "TOK_TO",
    "TOK_TYPE",
    "TOK_UNTIL",
    "TOK_VAR",
    "TOK_WHILE",
    "TOK_WITH",
    "TOK_XOR",
    "TOK_DOUBLE_DOT",
    "TOK_ASSIGN",
    "TOK_MINUS",
    "TOK_PLUS",
    "TOK_MULT",
    "TOK_DIVISION",
    "TOK_BRACKETS_SQUARE_LEFT",
    "TOK_BRACKETS_SQUARE_RIGHT",
    "TOK_SEMICOLON",
    "TOK_COLON",
    "TOK_COMMA",
    "TOK_DOT",
    "TOK_CAP",
    "TOK_DOG",
    "TOK_BRACKETS_LEFT",
    "TOK_BRACKETS_RIGHT",
    "TOK_LESS",
    "TOK_GREATER",
    "TOK_EQUAL",
    "TOK_LESS_OR_EQUAL",
    "TOK_GREATER_OR_EQUAL",
    "TOK_NOT_EQUAL",
    "TOK_UNRESERVED",
    "TOK_INTEGER",
    "TOK_REAL",
    "TOK_WRITE",
    "TOK_WRITELN"
};

const string TOKEN_TO_STR[] = 
{
    "and",
    "array",
    "begin",
    "break",
    "case",
    "const",
    "continue",
    "div",
    "do",
    "downto",
    "else",
    "end",
    "exit",
    "file",
    "for",
    "forward",
    "function",
    "if",
    "in",
    "mod",
    "nil",
    "not",
    "of",
    "or",
    "procedure",
    "record",
    "repeat",
    "set",
    "shl",
    "shr",
    "string",
    "then",
    "to",
    "type",
    "until",
    "var",
    "while",
    "whith",
    "xor",
    "..",
    ":=",
    "-",
    "+",
    "*",
    "/",
    "[",
    "]",
    ";",
    ":",
    ".",
    ".",
    "^",
    "@",
    "(",
    ")",
    "<",
    ">",
    "=",
    "<=",
    ">=",
    "<>"
    "UNRESERVED",
    "integer",
    "real",
    "write",
    "writeln"
};

//---Reserved words--

void ReservedWords::Add(string name, TokenType type, TokenValue value)
{
    words.insert(pair<string, pair<TokenType, TokenValue> >(name, pair<TokenType, TokenValue>(type, value)));
}

ReservedWords::ReservedWords()
{
    Add("integer", IDENTIFIER, TOK_INTEGER);
    Add("real", IDENTIFIER, TOK_REAL);
    Add("and", OPERATION, TOK_AND);
    Add("array", RESERVED_WORD, TOK_ARRAY);
    Add("begin", RESERVED_WORD, TOK_BEGIN);
    Add("break", RESERVED_WORD, TOK_BREAK);
    Add("case", RESERVED_WORD, TOK_CASE);
    Add("continue", RESERVED_WORD, TOK_CONTINUE);
    Add("const", RESERVED_WORD, TOK_CONST);
    Add("div", OPERATION, TOK_DIV);
    Add("do", RESERVED_WORD, TOK_DO);
    Add("downto", RESERVED_WORD, TOK_DOWNTO);
    Add("else", RESERVED_WORD, TOK_ELSE);
    Add("end", RESERVED_WORD, TOK_END);
    Add("exit", RESERVED_WORD, TOK_EXIT);
    Add("file", RESERVED_WORD, TOK_FILE);
    Add("for", RESERVED_WORD, TOK_FOR);
    Add("forward", RESERVED_WORD, TOK_FORWARD);
    Add("function", RESERVED_WORD, TOK_FUNCTION);
    Add("if", RESERVED_WORD, TOK_IF);
    Add("in", RESERVED_WORD, TOK_IN);
    Add("mod", OPERATION, TOK_MOD);
    Add("nil", RESERVED_WORD, TOK_NIL);
    Add("not", OPERATION, TOK_NOT);
    Add("of", RESERVED_WORD, TOK_OF);
    Add("or", OPERATION, TOK_OR);
    Add("procedure", RESERVED_WORD, TOK_PROCEDURE);
    Add("record", RESERVED_WORD, TOK_RECORD);
    Add("repeat", RESERVED_WORD, TOK_REPEAT);
    Add("set", RESERVED_WORD, TOK_SET);
    Add("shl", OPERATION, TOK_SHL);
    Add("shr", OPERATION, TOK_SHR);
    Add("string", RESERVED_WORD, TOK_STRING);
    Add("then", RESERVED_WORD, TOK_THEN);
    Add("to", RESERVED_WORD, TOK_TO);
    Add("type", RESERVED_WORD, TOK_TYPE);
    Add("until", RESERVED_WORD, TOK_UNTIL);
    Add("var", RESERVED_WORD, TOK_VAR);
    Add("while", RESERVED_WORD, TOK_WHILE);
    Add("with", RESERVED_WORD, TOK_WITH);
    Add("xor", OPERATION, TOK_XOR);
    Add("..", RESERVED_WORD, TOK_DOUBLE_DOT);
    Add(":=", OPERATION, TOK_ASSIGN);
    Add("-", OPERATION, TOK_MINUS);
    Add("+", OPERATION, TOK_PLUS);
    Add("*", OPERATION, TOK_MULT);
    Add("/", OPERATION, TOK_DIVISION);
    Add("[", OPERATION, TOK_BRACKETS_SQUARE_LEFT);
    Add("]", OPERATION, TOK_BRACKETS_SQUARE_RIGHT);
    Add(";", DELIMITER, TOK_SEMICOLON);
    Add(":", DELIMITER, TOK_COLON);
    Add(",", DELIMITER, TOK_COMMA);
    Add(".", OPERATION, TOK_DOT);
    Add("^", OPERATION, TOK_CAP);
    Add("@", OPERATION, TOK_DOG);
    Add("(", OPERATION, TOK_BRACKETS_LEFT);
    Add(")", OPERATION, TOK_BRACKETS_RIGHT);
    Add(">", OPERATION, TOK_GREATER);
    Add("<", OPERATION, TOK_LESS);
    Add("=", OPERATION, TOK_EQUAL);
    Add(">=", OPERATION, TOK_GREATER_OR_EQUAL);
    Add("<=", OPERATION, TOK_LESS_OR_EQUAL);
    Add("<>", OPERATION, TOK_NOT_EQUAL);
    Add("write", IDENTIFIER, TOK_WRITE);
    Add("writeln", IDENTIFIER, TOK_WRITELN);    
}

bool ReservedWords::Identify(string& str, TokenType& returned_type, TokenValue& returned_value)
{
    map<string, pair<TokenType, TokenValue> >::iterator i  = words.find(str);
    if (i == words.end()) return false;
    returned_type = i->second.first;
    returned_value = i->second.second;
    return true;
}

//---Token---

bool Token::IsRelationalOp() const
{
    return value == TOK_GREATER || value == TOK_GREATER_OR_EQUAL ||
           value == TOK_LESS || value == TOK_LESS_OR_EQUAL ||
           value == TOK_NOT_EQUAL || value == TOK_EQUAL;
}

bool Token::IsAddingOp() const
{
    return value == TOK_PLUS || value == TOK_MINUS ||
           value == TOK_OR || value == TOK_XOR;
}

bool Token::IsMultOp() const
{
    return value == TOK_MULT || value == TOK_DIVISION ||
           value == TOK_DIV || value == TOK_MOD ||
           value == TOK_AND || value == TOK_SHL ||
           value == TOK_SHR;
}

bool Token::IsUnaryOp() const
{
    return value == TOK_NOT || value == TOK_PLUS || value == TOK_MINUS;
}

bool Token::IsFactorOp() const
{
    return value == TOK_BRACKETS_SQUARE_LEFT || value == TOK_DOT;
}

bool Token::IsBitwiseOp() const
{
    return value == TOK_DIV || value == TOK_MOD || value == TOK_SHL || value == TOK_SHR || value == TOK_NOT || value == TOK_OR || value == TOK_AND || value == TOK_XOR;
}

bool Token::IsConst() const
{
    return type == INT_CONST || type == REAL_CONST || type == STR_CONST;
}

bool Token::IsVar() const
{
    return type == IDENTIFIER;
}

bool Token::IsConstVar() const
{
    return IsConst() || IsVar();
}

ostream& operator<<(ostream& out, const Token & token)
{
    out << token.GetLine() << ':' << token.GetPos() << '\t'<< TOKEN_DESCRIPTION[token.GetType()]
        << '\t' << TOKEN_VALUE_DESCRIPTION[token.GetValue()] << '\t' << token.GetName() << endl;
    return out;
}

Token::Token():
    name(strcpy(new char[1], "")),
    type(UNDEFINED),
    value(TOK_UNRESERVED),
    line(0),
    pos(0)
{
}

Token::Token(const char* name_, TokenType type_, TokenValue value_, int line_, int pos_):
    name(strcpy(new char[strlen(name_)+1], name_)),
    type(type_),
    value(value_),
    line(line_),
    pos(pos_)
{
}

Token::Token(const Token& token):
    name(strcpy(new char[strlen(token.name) + 1], token.name)),
    type(token.type),
    value(token.value),
    line(token.line),
    pos(token.pos)
{
}

Token::Token(TokenValue val):
    name(strcpy(new char[1], "")),
    type(UNDEFINED),
    value(val),
    line(0),
    pos(0)
{
}

Token& Token::operator=(const Token& token)
{
    if (name != NULL) delete(name);
    name = strcpy(new char[strlen(token.name)+1], token.name);
    type = token.type;
    value = token.value;
    line = token.line;
    pos = token.pos;
    return *this;
}

Token::~Token()
{
    if (name != NULL) delete(name);
}

TokenType Token::GetType() const
{
    return type;
}

TokenValue Token::GetValue() const
{
    return value;
}

const char* Token::GetName() const
{
    return name;
}

int Token::GetPos() const
{
    return pos;
}

int Token::GetLine() const
{
    return line;
}

void Token::NameToLowerCase()
{
    int i = -1;
    while (name[++i]) name[i] = tolower(name[i]);
}

int Token::GetIntValue() const
{    
    if (name[0] != '$') return atoi(name);
    return hex_str_to_i(name + 1);    
}

float Token::GetRealValue() const
{
    float res;
    stringstream s;
    s << name;
    s >> res;
    return res;
}

void Token::ChangeSign()
{
    if (name[0] == '+') name[0] = '-';
    else if (name[0] == '-') name[0] = '+';
    else
    {
        char* tmp = new char[strlen(name) + 2];
        strcpy(tmp + 1, name);
        tmp[0] = '-';        
        delete name;
        name = tmp;
    }
}

Token::Token(int value)
{
    type = INT_CONST;
    stringstream s;
    s << value;
    name = strcpy(new char[s.str().size() + 1], s.str().c_str());
}

Token::Token(float value)
{
    type = REAL_CONST;
    stringstream s;
    s << value;
    name = strcpy(new char[s.str().size() + 1], s.str().c_str());
}

//---Scanner---

void Scanner::AddToBuffer(char c)
{
    buffer.push_back(c);
    buffer_low.push_back(tolower(c));
}

void Scanner::ReduceBuffer()
{
    buffer.resize(buffer.size() - 1);
    buffer_low.resize(buffer.size());
}

void Scanner::MakeToken(TokenType type, TokenValue value)
{
    token = Token(buffer.c_str(), type, value, first_line, first_pos);
    buffer.clear();
    buffer_low.clear();
    state = NONE_ST;
}

void Scanner::IdentifyAndMake()
{
    TokenType t;
    TokenValue v;
    if (!reserved_words.Identify(buffer_low, t, v))
    {
        t = IDENTIFIER;
        v = TOK_UNRESERVED;
    }
    MakeToken(t, v);
}

bool Scanner::TryToIdentify()
{
    TokenType t;
    TokenValue v;
    if (reserved_words.Identify(buffer_low, t, v))
    {
        MakeToken(t, v);
        return true;
    }
    return false;
}

void Scanner::Error(const char* msg) const
{
    stringstream s;
    s << line << ':' << pos << " ERROR " << msg;
    throw( CompilerException( s.str().c_str() ) ) ;
}

Scanner::Scanner(istream& input):
    in(input),
    line(1),
    pos(0),
    state(NONE_ST),
    c(0)
{
}

Token Scanner::GetToken()
{
    return token;
}

void Scanner::EatLineComment()
{
    if (c =='/' && in.peek() == '/')
    {
        do {
            ExtractChar();
        } while (c != '\n' && !in.eof());
    }
}

void Scanner::EatBlockComment()
{
    if (c == '{')
    {
        do {
            ExtractChar();
            if (c == '\n')
            {
                ++line;
                pos = 0;
            }
            if (in.eof()) Error("end of file in comment");
        } while (c != '}');
        ExtractChar();
    }
}

void Scanner::EatRealFractPart()
{
    while (isdigit(c))
    {
        AddToBuffer(c);
        ExtractChar();
    }
    if (c == 'e' || c == 'E')
    {
        AddToBuffer(c);
        ExtractChar();
        if (c == '+' || c == '-')
        {
            AddToBuffer(c);
            ExtractChar();
        }
        if (!isdigit(c))
            Error("illegal character, should be number");
        do
        {
            AddToBuffer(c);
            ExtractChar();
        } while (isdigit(c));
    }
    MakeToken(REAL_CONST);
}

void Scanner::EatStrNum()
{
    int res = 0;
    if (!isdigit(c)) Error("illegal char constant");
    while (isdigit(c))
    {
        res = res*10 + c - '0';
        ExtractChar();
    }
    AddToBuffer(res);
    res = 0;
    while (c == '#')
    {
        ExtractChar();
        res = 0;
        while (isdigit(c))
        {
            res = res*10 + c - '0';
            ExtractChar();
        }
        AddToBuffer(res);
    }
}

void Scanner::EatStrConst()
{
    bool isNum = (buffer[0] =='#');
    buffer.clear();
    buffer_low.clear();
    if (isNum)
    {
        EatStrNum();
        if (c == '\'')
            ExtractChar();
        else
            {
                MakeToken(STR_CONST);
                return;
            }
    }
    bool end_of_str = false;
    while (!end_of_str)
    {
        while (!end_of_str)
        {
            if (in.eof())
                Error("end of file in string");
            if (c == '\n') Error("end of line in string");
            if (c != '\'')
            {
                AddToBuffer(c);
                ExtractChar();
            }
            while (c == '\'' && !end_of_str)
            {
                int count;
                for (count = 0; c == '\'' && !in.eof(); ++count, ExtractChar())
                    if (count % 2) AddToBuffer(c);
                if (count % 2) end_of_str = true;
            }
        }
        if (c == '#')
        {
            ExtractChar();
            EatStrNum();
            if (c == '\'')
            {
                ExtractChar();
                end_of_str = false;
            }

        }
    }
    MakeToken(STR_CONST);
}

void Scanner::EatHex()
{
    bool read = false;
    while (ishexnum(c))
    {
        read = true;
        AddToBuffer(c);
        ExtractChar();
    }
    if (!read)
        Error("invalid integer expression");
    else
        MakeToken(INT_CONST);
}

void Scanner::EatInteger()
{
    while (isdigit(c))
    {
        AddToBuffer(c);
        ExtractChar();
    }
    if (tolower(c) == 'e' || (c == '.' && in.peek() != '.'))
    {
        AddToBuffer(c);
        ExtractChar();
        EatRealFractPart();
    }
    else
        {
            MakeToken(INT_CONST);
        }
}

void Scanner::EatIdentifier()
{
    while (isalnum(c) || c == '_')
    {
        AddToBuffer(c);
        ExtractChar();
    }
    IdentifyAndMake();
}

void Scanner::EatOperation()
{
    bool matched = false;
    if (!in.eof() && !isalnum(c) && c != '_' && !isspace(c))
    {
        AddToBuffer(c);
        if (TryToIdentify())
        {
            matched = true;
            ExtractChar();
        }
        else
            {
                ReduceBuffer();
            }
    }
    if (!matched && !TryToIdentify())
        Error("illegal expression");
}

void Scanner::ExtractChar()
{
    c = in.get();
    ++pos;
}

Token Scanner::NextToken()
{
    bool matched = false;
    do
    {
        ExtractChar();
        if (state != NONE_ST) matched = true;
        switch (state)
        {
            case NONE_ST:
            break;
            case EOF_ST:
                MakeToken(END_OF_FILE);
            break;
            case INTEGER_ST:
                EatInteger();
            break;
            case HEX_ST:
                EatHex();
            break;
            case IDENTIFIER_ST:
                EatIdentifier();
            break;
            case OPERATION_ST:
                if (buffer[0] == '\'' || buffer[0] == '#')
                {
                    EatStrConst();
                }
                else
                    {
                        EatOperation();
                    };
        }
        if (state == NONE_ST)
        {
            EatLineComment();
            EatBlockComment();
            if (c == '\n')
            {
                ++line;
                pos = 0;
            }
            else
            {
                first_pos = pos;
                first_line = line;
                if (in.eof())
                {
                    state = EOF_ST;
                }
                else if (!isspace(c))
                {
                    if (isalpha(c) || c == '_')
                    {
                        state = IDENTIFIER_ST;
                    }
                    else if (isdigit(c))
                    {
                        state = INTEGER_ST;
                    }
                    else if (c == '$')
                    {
                        state = HEX_ST;
                    }
                    else
                        {
                            state = OPERATION_ST;
                        }
                    AddToBuffer(c);
                }
            }
        }
    } while (!matched);
    return token;
}

