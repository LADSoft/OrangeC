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
#pragma once

#include <cassert>
#include <algorithm>

#define MAX_LOOKBACK 1024
namespace Parser
{
/* error list */
struct errl
{
    int errornumber;
    void* data;
};

/* used for error skimming */
#define BALANCE struct balance
#define BAL_PAREN 0
#define BAL_BRACKET 1
#define BAL_BEGIN 2
#define BAL_LT 3
#define ERRORS struct errl

struct balance
{
    struct balance* back;
    short type;
    short count;
};

// clang-format off
    enum _matchFlags : int
    {
        KW_NONE = 0, KW_CPLUSPLUS = 1, KW_INLINEASM = 2, KW_NONANSI = 4, KW_C99 = 8,
        KW_C1X = 16, KW_ASSEMBLER = 32, KW_MSIL = 64,
        KW_386 = 128, KW_68K = 256, KW_C2X = 512, KW_ALL = 0x40000000
    };
// clang-format on
// clang-format off

    enum _tokenTypes : unsigned long
    {
        TT_BASE = 1,
        TT_BOOL = 2,
        TT_INT = 4,
        TT_FLOAT = 8,
        TT_COMPLEX = 16,
        TT_TYPEQUAL = 32,
        TT_POINTERQUAL = 64,
        TT_UNARY = 128,
        TT_BINARY = 0x100,
        TT_OPERATOR = 0x200,
        TT_ASSIGN = 0x400,
        TT_RELATION = 0x800,
        TT_EQUALITY = 0x1000,
        TT_INEQUALITY = 0x2000,
        TT_POINTER = 0x4000,
        TT_STORAGE_CLASS = 0x8000,
        TT_CONTROL = 0x10000,
        TT_BLOCK = 0x20000,
        TT_PRIMARY = 0x40000,
        TT_SELECTOR = 0x80000,
        TT_VAR = 0x100000,
        TT_BASETYPE = 0x200000,
        TT_INCREMENT = 0x400000,
        TT_SWITCH = 0x800000,
        TT_ENUM = 0x1000000,
        TT_STRUCT = 0x2000000,
        TT_TYPENAME = 0x4000000,
        TT_TYPEDEF = 0x8000000,
        TT_VOID = 0x10000000,
        TT_CLASS = 0x20000000,
        TT_LINKAGE = 0x40000000,
        TT_DECLARE = 0x80000000,
        TT_UNKNOWN = 0
    };
// clang-format on

struct KeywordData
{
    const char* name;
    int len;
    Keyword key;
    unsigned matchFlags;
    unsigned tokenTypes;
};

// must match the definition in msilprocess.cpp
// clang-format off
    enum class LexType : unsigned
    {
        none_,
        i_, ui_, l_, ul_, ll_, ull_,
        bitint_, ubitint_,
        l_f_, l_d_, l_ld_, l_I_, 
        l_astr_, l_wstr_, l_ustr_, l_Ustr_, l_u8str_, l_msilstr_,
        l_achr_, l_wchr_, l_uchr_, l_Uchr_, l_u8chr_,
        l_id_, l_kw_, l_qualifiedName_,
        l_asmInstruction_, l_asmRegister_
    };
// clang-format on

struct StringData
{
    LexType strtype;
    int size;
    int label;
    int refCount;
    char* suffix;
    Optimizer::SLCHAR** pointers;
};

struct Lexeme
{
    Lexeme() = default;
    LexType type;
    union
    {
        struct u_val value;
        KeywordData* kw;
    };
    struct
    {
        int errline;
        const char* errfile;
        int charindex;
        int charindexend;
        int filenum;
    };
    struct
    {
        unsigned refcount;
        char* litaslit;
        char* suffix;
        Optimizer::LINEDATA* linedata;
        SYMBOL* typequal;
    };
};

struct LexToken
{
    typedef std::vector<Lexeme*>::iterator iterator;

    Lexeme* Create()
    {
        if (cache.size())
        {
            auto rv = cache.front();
            *rv = {};
            cache.pop_front();
            return rv;
        }
        auto rv = new Lexeme();
        return rv;
    }
    void Add(Lexeme* lex)
    {
        data.push_back(lex);
        ++current;
        if (current > top) ++top;
    }
    void Next()
    {
        if (current < data.size())
        {
            if (current == top)
            {
                ++top;
            }
            ++current;
        }
    }
    void Prev()
    {
        if (current)
        {
            --current;
        }
    }
    Lexeme* operator*()
    {
        assert(current != data.size());
        return data[current];
    }
    iterator Index()
    {
        auto it = begin();
        std::advance(it, current);
        return it;
    }
    void Index(iterator it)
    {
        current = it - begin();
    }
    void reset()
    {
        replaying = true;
        current =top = 0;
    }
    bool RePlaying()
    {
        return replaying;
    }
    bool Reloaded()
    {
        return current < top;
    }
    void PlayAgain(iterator* index)
    {
        if (!index)
        {
            current = hold.back();
            hold.pop_back();
        }
        else
        {
            hold.push_back(current);
            current = *index - data.begin();
        }
    }
    void Prune(unsigned maxDepth, unsigned pruneSize)
    {
        if (hold.size() > maxDepth && ValidPrune(pruneSize) )
        {
            for (int i = 0; i < pruneSize; i++)
            {
                if (data[i]->refcount < 2)
                {
                    if (cache.size() < pruneSize)
                    {
                        cache.push_back(data[i]);
                    }
                    else
                    {
                        delete data[i];
                    }
                }
            }
            std::rotate(data.begin() + pruneSize, data.begin(), data.end());
            data.resize(maxDepth - pruneSize);
            for (auto&& it : hold)
            {
                it -= pruneSize;
            }
            current -= pruneSize;
            top -= pruneSize;
        }
    }
    bool ValidPrune(unsigned pruneSize)
    {
        if (current < pruneSize)
            return false;
        if (top < pruneSize)
            return false;
        for (auto val : hold)
        {
            if (val  < pruneSize)
                return false;
        }
        return true;
    }
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    size_t size() const { return data.size(); }
private:
    bool replaying = false;
    int current = 0;
    int top= 0;
    std::deque<Lexeme*> cache;
    std::deque<int> hold;
    std::vector<Lexeme*> data;
};
struct LexTokenFactory
{
    LexToken* Create()
    {
        auto tokens = new LexToken();
        lists.insert(tokens);
        return tokens;
    }
    void Destroy(LexToken* tokens)
    {
        delete tokens;
        lists.erase(tokens);
    }
    void clear()
    {
        for (auto s : lists)
        {
            delete s;
        }
        lists.clear();
    }
    ~LexTokenFactory()
    {
        clear();
    }
private:
    std::set<LexToken*> lists;
};

#define MATCHTYPE(tp) (currentLex->type == (tp))
#define ISID() (currentLex->type == LexType::l_id_)
#define ISKW() (currentLex->type == LexType::l_kw_)
#define MATCHKW(keyWord) (ISKW() && (currentLex->kw->key == keyWord))
bool KWTYPE(unsigned types);
#define KW() (ISKW() ? currentLex->kw->key : Keyword::none_)

extern Optimizer::LINEDATA nullLineData;
extern int eofLine;
extern const char* eofFile;
extern bool parsingPreprocessorConstant;
extern LexTokenFactory tokenFactory;
extern std::stack<LexToken*> context;
extern int charIndex;
extern SymbolTable<KeywordData>* kwSymbols;
extern Lexeme* currentLex;
extern LexToken* currentContext;

void lexini(void);
KeywordData* searchkw(const unsigned char** p);
void SkipToNextLine(void);
Lexeme* getGTSym();
void SkipToEol();
bool AtEol();
void CompilePragma(const unsigned char** linePointer);
void InsertLineData(int lineno, int fileindex, const char* fname, char* line);
void FlushLineData(const char* file, int lineno);
std::list<Statement*>* currentLineData(std::list<FunctionBlock*>& parent, Lexeme* lex, int offset);
void getsym(void);
void BackupTokenStream(LexToken::iterator to);
void BackupTokenStream(void);
void SwitchTokenStream(LexToken* lexList);
bool CompareLex(LexToken* left, LexToken* right);
void SetAlternateParse(bool set, const std::string& val);
long long ParseExpression(std::string& line);
}  // namespace Parser