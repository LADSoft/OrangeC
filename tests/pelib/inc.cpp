#include "PELib.h"
#include <cstdarg>
#include <list>
#include <functional>
#include <cstdlib>

using namespace std;

static PE pe;

void vdie(const string &src, int line, int column, const char *format, va_list arg)
{
    if (line > 0)
        fprintf(stderr, "%s[%d:%d] ", src.c_str(), line, column);
    vfprintf(stderr, format, arg);
    fprintf(stderr, "\n");
    exit(1);
}

void die(const string &src, int line, int column, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    vdie(src, line, column, format, arg);
    va_end(arg);
}

struct Symbol
{
    string src;
    int line = 0, column = 0;
    Address addr;

    void clear()
	{
        *addr.addr = 0;
    }

    void die(const char *format, ...)
	{
        va_list arg;
        va_start(arg, format);
        vdie(src, line, column, format, arg);
        va_end(arg);
    }

    DWORD operator *() const { return *addr.addr; }
};

map<string, Symbol> funcs;

Address func(const string &name, const string &src = "", int line = 0, int column = 0)
{
    if (funcs.find(name) == funcs.end())
	{
        Symbol sym;
        sym.src = src;
        sym.addr = pe.sym(name, true);
        sym.line = line;
        sym.column = column;
        funcs[name] = sym;
    }
    return funcs[name].addr;
}

void link() {
    puts("linking...");
    for (auto p: funcs) p.second.clear();
    pe.link();
    list<pair<string, Symbol>> syms;
    for (auto p: funcs)
	{
        auto sym = p.second;
        if (!*sym) sym.die("undefined: %s", p.first.c_str());
        syms.push_back(p);
    }
    syms.sort([](const pair<string, Symbol> &p1, const pair<string, Symbol> &p2)
	{
        return *p1.second < *p2.second;
    });
    for (auto p: syms)
        printf("%x: %s\n", *p.second, p.first.c_str());
}

enum TokenKind { Word, Num, Str, Indent, Dedent, EndOfFile, Other };

class Token
{
public:
    TokenKind type = Other;
    string text;
    int line = 1;
    int column = 0;

    void emit()
    {
        printf("(%d:%d) type = %d, text = \"%s\"\n", line, column, type, text.c_str());
        fflush(stdout);
    }
};

class Lexer
{
private:
    FILE *file;
    int cur = 0;
    int curline = 1, curcol = 0;

    vector<Token> lookahead;            // a vector of lookahead tokens
    int lookidx = 0;                    // index of next token to return by read()

    int indent = 0;                     // the current indenting level (0 = none, ...)

public:
    string src;
    Token token;

    Lexer(const string &src): src(src)
	{
        file = fopen(src.c_str(), "rt");
        readc();

        // preprocess all tokens so as to handle synthetic indents
        while (scan())
            store(token);
    }

    ~Lexer()
	{
        if (file) fclose(file);
    }

    bool read()
	{
        if (lookidx >= lookahead.size())
            return false;

        token = lookahead[lookidx++];
        return true;
    }

private:
    void store(Token token)
    {
        // token.emit();
        lookahead.push_back(token);
    }

    int readc()
	{
        if (!file) cur = -1;
        if (cur < 0) return cur;
        cur = fgetc(file);
        ++curcol;
        if (cur == '\n') { ++curline; curcol = 0; }
        return cur;
    }

    static bool isalpha(char ch)
	{
        return ch == '_'
            || ('A' <= ch && ch <= 'Z')
            || ('a' <= ch && ch <= 'z');
    }

    static bool isnum(char ch)
	{
        return '0' <= ch && ch <= '9';
    }

    static bool isletter(char ch)
	{
        return ch == '\'' || isalpha(ch) || isnum(ch);
    }

    static bool istab(char ch)
	{
        return ch == '\t';
    }

    bool scan()
	{
        while (cur >= 0)
        {
            if (cur == '\t' && curcol == 1)
            {
                // gather up indentation
                if (!read(Indent, istab))
                    return false;

                // handle indentation and produce synthetic dedent tokens if necessary

                // disallow indenting with spaces (assumed by all code in this project!)
                if (strchr(token.text.c_str(), ' ') != NULL)
                    die(src, token.line, token.column, "Space in indentation");

                // check if the indentation increase exceeds one level
                size_t level = token.text.size();
                if (level > indent + 1)
                    die(src, token.line, token.column, "Multiple indenting levels in a single line");
                else if (level == indent + 1)
                {
                    // indent a single level
                    indent = level;
                    token.type = Indent;
                    token.text = "(indent)";
                    return true;
                }
                else if (level == indent)
                {
                    // no change in indentation, so don't emit a token
                    return true;
                }
                else
                {
                    // produce synthetic dedent tokens as needed
                    int dedents = (int) indent - (int) level;
                    if (dedents < 1)
                    {
                        die(src, token.line, token.column, "Negative number of dedents detected");
                        return true;
                    }

                    indent = level;
                    token.text = "(dedent)";
                    token.type = Dedent;
                    // -1 because of the way the code works; one additional token is implicitly stored
                    for (int i = 0; i < dedents - 1; i++)
                    {
                        store(token);
                    }
                }

                return true;
            }
            else if (isalpha(cur))
                return read(Word, isletter);
            else if (isnum(cur))
                return read(Num, isnum);
            else if (cur == '"')
			{
                int dq = 0;
                bool esc = false;
                return read(Str, [&](int ch)
				{
                    if (dq == 2)
                        return false;
                    else if (esc)
                        esc = false;
                    else if (ch == '"')
                        ++dq;
                    else if (ch == '\\')
                        esc = true;
                    return true;
                });
            }
			else if (cur > ' ')
			{
                int c = 0;
                return read(Other, [&](int) { return c++ < 1; });
            }

            readc();
        }

        // create synthetic dedent tokens for the currently open scopes
        token.text = "(dedent)";
        token.type = Dedent;
        for (int i = 0; i < indent; i++)
            store(token);

        // output final token
        token.type = EndOfFile;
        token.text = "(eof)";

        return false;
    }

    bool read(TokenKind t, function<bool(int)> f)
	{
        token.line = curline;
        token.column = curcol;
        token.type = t;
        token.text.clear();
        while (f(cur))
		{
            token.text += cur;
            readc();
        }
        return true;
    }
};

string getstr(string s)
{
    if (s.size() >= 2 && s[0] == '"' && s[s.size() - 1] == '"')
        s = s.substr(1, s.size() - 2);
    string ret;
    bool esc = false;
    for (int i = 0; i < s.size(); ++i)
	{
        char ch = s[i];
        if (esc)
		{
            switch (ch)
			{
            case 'a': ret += '\a'; break;
            case 'b': ret += '\b'; break;
            case 'n': ret += '\n'; break;
            case 'f': ret += '\f'; break;
            case 't': ret += '\t'; break;
            case 'v': ret += '\v'; break;
            case '0': ret += '\0'; break;
            default : ret += ch  ; break;
            }
            esc = false;
        } 
		else if (ch == '\\')
            esc = true;
        else
            ret += ch;
    }
    return ret;
}

template <typename T> int index(const vector<T> &vec, const T &v)
{
    for (int i = 0; i < vec.size(); ++i)
        if (vec[i] == v) return i;
    return -1;
}

class Parser
{
private:
    Lexer lexer;
    TokenKind type;
    string token;

public:
    Parser(const string &src): lexer(src) {}

    void parse()
	{
        while (read())
		{
            if (type == EndOfFile)
                break;
            else if (token == "function")
                parseFunction();
            else if (token == "class")
                parseClass();
            else if (token == "import")
                parseImport();
            else
                die("error: %s", token.c_str());
        }
    }

    void die(const char *format, ...)
	{
        va_list arg;
        va_start(arg, format);
        vdie(lexer.src, lexer.token.line, lexer.token.column, format, arg);
        va_end(arg);
    }

private:
    bool read()
	{
        if (!lexer.read()) return false;
        type = lexer.token.type;
        token = lexer.token.text;
        return true;
    }

    void parseFunction(const string &prefix = "")
	{
        if (!read() || type != Word)
            die("function: name required");
        curtext->put(func(prefix + token));
        push(ebp);
        mov(ebp, esp);
        auto args = parseFunctionArgs();
        if (!read() || token != ":")
            die("function: colon required");
        bool epi = false;
        while (read())
		{
            if (type == Dedent)
			{
                if (!epi)
				{
                    leave();
                    ret();
                }
                return;
            } 
			else if (type == Indent)
			{
                if (!read() || type != Word)
                    die("function: indent required");
                epi = false;
                int l = lexer.token.line, c = lexer.token.column;
                auto t = token;
                if (t == "return")
				{
                    if (read() && type == Num)
					{
                        mov(eax, atoi(token.c_str()));
                        leave();
                        ret();
                        epi = true;
                        continue;
                    }
                } 
				else if (read())
				{
                    if (token == "(")
					{
                        int nargs = parseCallArgs(args);
                        call(func(t, lexer.src, l, c));
                        if (nargs > 0) add(esp, 4 * nargs);
                        continue;
                    }
                }
                ::die(lexer.src, l, c, "error: %s", t.c_str());
            } 
			else
                die("error: %s", token.c_str());
        }
        die("function: 'end function' required");
    }

    vector<string> parseFunctionArgs()
	{
        if (!read() || token != "(")
            die("function: '(' required");
        vector<string> args;
        while (read())
		{
            if (token == ")")
                break;
            else if (type == Word)
			{
                args.push_back(token);
                if (read())
				{
                    if (token == ")")
                        break;
                    else if (token == ",")
                        continue;
                }
                die("function: ',' or ')' required");
            } 
			else
                die("function: argument required");
        }
        return args;
    }

    int parseCallArgs(const vector<string> &fargs)
	{
        list<pair<TokenKind, string>> args;
        while (read())
		{
            if (token == ")")
                break;
            else if (type == Word)
			{
                int arg = index(fargs, token);
                if (arg == -1)
                    die("undefined variable: %s", token.c_str());
                args.push_front(make_pair(type, token));
            } 
			else if (type == Num || type == Str)
                args.push_front(make_pair(type, token));
            else
                die("function: argument required");
            if (read())
			{
                if (token == ")")
                    break;
                else if (token == ",")
                    continue;
            }
            die("function: ',' or ')' required");
        }
        for (auto p: args)
		{
            switch (p.first)
			{
            case Word:
                // TODO: push(ptr[ebp+X]);
                mov(eax, ebp);
                add(eax, (index(fargs, p.second) + 2) * 4);
                push(ptr[eax]);
                break;
            case Num:
                push(atoi(p.second.c_str()));
                break;
            case Str:
                push(pe.str(getstr(p.second)));
                break;
            }
        }
        return args.size();
    }

    void parseClass()
	{
        if (!read() || type != Word)
            die("class: name required");
        auto name = token;
        if (!read() || token != ":")
            die("class: colon required");
        if (!read() || type != Indent)
            die("class: indent required");
        while (read())
		{
            if (type == Dedent)
                break;
            else if (token == "function")
                parseFunction(name + "'");
            else
                die("error: %s", token.c_str());
        }
    }

    void parseImport()
	{
        if (!read() || type != Str)
            die("import: dll name required");
        auto dll = getstr(token);
        if (!read() || type != Word)
            die("import: calling convention required");
        if (token != "cdecl")
            die("import: not supported: %s", token.c_str());
        if (!read() || type != Word)
            die("import: function name required");
        curtext->put(func(token));
        jmp(ptr[pe.import(dll, token)]);
    }
};

int main(int argc, char *argv[])
{
    pe.select();
    curtext->put(func("_start"));
    call(func("main"));
    push(eax);
    call(ptr[pe.import("msvcrt.dll", "exit")]);
    jmp(curtext->addr());

    for (int i = 1; i < argc; ++i)
        Parser(argv[i]).parse();
    link();

    auto exe = "output.exe";
    auto f = fopen(exe, "wb");
    if (!f) die("", 0, 0, "can not open: %s", exe);
    pe.write(f);
    fclose(f);
    printf("output: %s\n", exe);
}
