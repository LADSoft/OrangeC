/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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
#include <functional>
#include "compiler.h"
#include "lex.h"
#include "templatedecl.h"
#include "memory.h"
#include "stmt.h"
#include "ccerr.h"
#include "class.h"
#include "ListFactory.h"
#include "exprpacked.h"
#include "types.h"

namespace Parser
{
LexemeStreamFactory streamFactory;
LexemeTokenFactory lexemeFactory;

std::stack<LexemeStream*> contextStack;

Lexeme* currentLex;
LexemeStream* currentStream;

LexemeTokenFactory* LexemeTokenFactory::lexemeTokenFactory;
LexemeStreamFactory* LexemeStreamFactory::lexemeContextFactory;

void lextokeninit()
{
    while (!contextStack.empty())
        contextStack.pop();
    streamFactory.clear();
    lexemeFactory.clear();
    currentStream = LexemeStreamFactory::Instantiation().Create();
    contextStack.push(currentStream);
    currentLex = nullptr;
}
LexemeStreamPosition::LexemeStreamPosition(LexemeStream* Context) : tokenContext(Context), origPosition(Context->Index())
{
    tokenContext->Register(this, true);
}
LexemeStreamPosition::~LexemeStreamPosition() { tokenContext->Register(this, false); }
void LexemeStreamPosition::Replay(std::function<void()> callback)
{
    Backup();
    callback();
    Restore();
}
Lexeme* LexemeStreamPosition::get() { return tokenContext->get(origPosition); }
void LexemeStreamPosition::Backup()
{
    backupLex = currentLex;
    currentPosition = tokenContext->Index();
    tokenContext->Index(origPosition);
    currentLex = tokenContext->get(tokenContext->Index());
}
void LexemeStreamPosition::Restore()
{
    tokenContext->Index(currentPosition);
    if (backupLex)
        currentLex = backupLex;
    else
        currentLex = tokenContext->get(tokenContext->Index());
}
void LexemeStreamPosition::Bump() { origPosition = tokenContext->Index(); }
Lexeme* LexemeTokenFactory::Create()
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
void LexemeTokenFactory::Destroy(Lexeme* lex, int pruneSize)
{
    if (lex->refcount < 2)
    {
        if (cache.size() < pruneSize)
        {
            cache.push_back(lex);
            lex->refcount = 0;
        }
        else
        {
            delete lex;
        }
    }
}
void LexemeTokenFactory::clear()
{
    for (auto c : cache)
        delete c;
    cache.clear();
}
void LexemeTokenFactory::Initialize()
{
    if (!lexemeTokenFactory)
    {
        lexemeTokenFactory = new LexemeTokenFactory();
    }
}
void LexemeStream::Add(Lexeme* lex)
{
    assert(current == top);
    lex->refcount++;
    data.push_back(lex);
    ++current;
    if (current > top)
        ++top;
}
LexemeStream& LexemeStream::operator--()
{
    if (current > currentBase)
    {
        --current;
    }
    currentLex = currentStream->get(currentStream->Index());
    return *this;
}
void LexemeStream::Prune(unsigned maxDepth, unsigned pruneSize)
{
    if (data.size() > maxDepth && ValidPrune(pruneSize))
    {
        for (int i = 0; i < pruneSize; i++)
        {
            LexemeTokenFactory::Instantiation().Destroy(data[i], pruneSize);
        }
        std::rotate(data.begin(), data.begin() + pruneSize, data.end());
        data.resize(data.size() - pruneSize);
        currentBase += pruneSize;
    }
}
bool LexemeStream::ValidPrune(unsigned pruneSize)
{
    if (current < pruneSize + currentBase)
        return false;
    if (top < pruneSize + currentBase)
        return false;
    for (auto i : positions)
    {
        if (i && i->CheckMin(currentBase + pruneSize))
            return false;
    }
    return true;
}
void LexemeStream::Register(LexemeStreamPosition* position, bool enable)
{
    if (enable)
    {
        for (auto itr = positions.rbegin(); itr != positions.rend(); ++itr)
            if ((*itr) == nullptr)
            {
                *itr = position;
                return;
            }
        positions.push_back(position);
    }
    else
    {
        for (auto itr = positions.rbegin(); itr != positions.rend(); ++itr)
            if ((*itr) == position)
            {
                *itr = nullptr;
                break;
            }
    }
}
void LexemeStreamFactory::Initialize()
{
    if (!lexemeContextFactory)
    {
        lexemeContextFactory = new LexemeStreamFactory();
    }
}

void ParseOnStream(LexemeStream* newStream, std::function<void()> callback)
{
    auto lex1 = *newStream;
    lex1.reset();

    auto lex = currentLex;
    contextStack.push(&lex1);
    currentStream = &lex1;
    currentLex = lex1.get(lex1.Index());
    callback();
    contextStack.pop();
    currentStream = contextStack.top();
    currentLex = lex;
}
LexemeStream* GetFunctionTokenStream(LexemeStream* stream)
{
    LexemeStream* savePos = stream ? stream : streamFactory.Create();
    int paren = 0;
    bool viaTry = false;
    viaTry = MATCHKW(Keyword::try_);
    while (currentLex)
    {
        Keyword kw = KW();
        if (kw == Keyword::begin_)
        {
            paren++;
        }
        else if (kw == Keyword::end_ && !--paren)
        {
            savePos->Add(currentLex);
            getsym();
            if (!viaTry || !MATCHKW(Keyword::catch_))
            {
                break;
            }
        }
        savePos->Add(currentLex);
        currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
        lines = nullptr;
        getsym();
    }
    return savePos;
}

LexemeStream* GetDataTokenStream()
{
    LexemeStream* savePos = streamFactory.Create();
    int paren = 0;
    int begin = 0;
    int brack = 0;
    int ltgt = 0;
    bool viaTry = false;
    while (currentLex)
    {
        Keyword kw = KW();
        if (kw == Keyword::semicolon_)
        {
            break;
        }
        else if (kw == Keyword::openpa_)
        {
            paren++;
        }
        else if (kw == Keyword::closepa_)
        {
            if (paren-- == 0 && !brack && !begin)
            {
                break;
            }
        }
        else if (kw == Keyword::begin_)
        {
            begin++;
        }
        else if (kw == Keyword::end_)
        {
            if (begin-- == 0 && !brack && !paren)
            {
                break;
            }
        }
        else if (kw == Keyword::openbr_)
        {
            brack++;
        }
        else if (kw == Keyword::closebr_)
        {
            brack--;
        }
        else if (kw == Keyword::comma_ && !paren && !brack && !ltgt && !begin)
        {
            break;
        }
        // there is some ambiguity between templates and <
        else if (kw == Keyword::lt_)
        {
            ltgt++;
        }
        else if (kw == Keyword::gt_)
        {
            ltgt--;
        }
        savePos->Add(currentLex);
        currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
        lines = nullptr;
        getsym();
    }
    return savePos;
}
static void FindClass(SYMBOL* funcsp, SYMBOL** sym)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    Type* castType = nullptr;
    char buf[512];
    int ov = 0;
    bool namespaceOnly = false;

    *sym = nullptr;

    if (MATCHKW(Keyword::classsel_))
        namespaceOnly = true;
    nestedPath(&encloser, &ns, &throughClass, true, StorageClass::global_, false, 0);
    getIdName(funcsp, buf, sizeof(buf), &ov, &castType);
    if (buf[0])
    {
        *sym = finishSearch(buf, encloser, ns, false, throughClass, namespaceOnly);  // undefined in local context
    }
    return;
}
std::list<CONSTRUCTORINITIALIZER*>* GetConstructorInitializers(SYMBOL* funcsp, SYMBOL* sym, LexemeStream* fullTokenStream)
{
    (void)sym;
    std::list<CONSTRUCTORINITIALIZER*>* rv = constructorInitializerListFactory.CreateList();
    //    if (sym->name != overloadNameTab[CI_CONSTRUCTOR])
    //        error(ERR_Initializer_LIST_REQUIRES_CONSTRUCTOR);
    while (currentLex)
    {
        EnterPackedSequence();
        if (ISID() || MATCHKW(Keyword::classsel_))
        {
            SYMBOL* sym = nullptr;
            LexemeStreamPosition pos(currentStream);
            FindClass(funcsp, &sym);
            if (fullTokenStream)
            {
                CopyParsedLexemes(fullTokenStream, pos);
            }
            char name[1024];
            auto v = Allocate<CONSTRUCTORINITIALIZER>();
            v->line = currentLex->sourceLineNumber;
            v->file = currentLex->sourceFileName;
            v->initData = streamFactory.Create();
            name[0] = 0;
            if (ISID())
            {
                Utils::StrCpy(name, currentLex->value.s.a);
                if (fullTokenStream)
                {
                    fullTokenStream->Add(currentLex);
                    currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                    lines = nullptr;
                }
                getsym();
            }
            v->name = litlate(name);
            if (sym && istype(sym))
                v->basesym = sym;
            if (MATCHKW(Keyword::lt_))
            {
                int paren = 0, tmpl = 0;
                v->initData->Add(currentLex);
                if (fullTokenStream)
                {
                    fullTokenStream->Add(currentLex);
                    currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                    lines = nullptr;
                }
                getsym();
                while (currentLex && (!MATCHKW(Keyword::gt_) || paren || tmpl))
                {
                    if (MATCHKW(Keyword::openpa_))
                        paren++;
                    if (MATCHKW(Keyword::closepa_))
                        paren--;
                    if (!paren && MATCHKW(Keyword::lt_))
                        tmpl++;
                    if (!paren && (MATCHKW(Keyword::gt_) || MATCHKW(Keyword::rightshift_)))
                        tmpl--;
                    if (MATCHKW(Keyword::rightshift_))
                    {
                        SplitGreaterThanFromRightShift();
                        tmpl--;
                        v->initData->Add(currentLex);
                    }
                    else
                    {
                        v->initData->Add(currentLex);
                        if (fullTokenStream)
                        {
                            fullTokenStream->Add(currentLex);
                            currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                            lines = nullptr;
                        }
                        getsym();
                    }
                }
                if (MATCHKW(Keyword::gt_))
                {
                    v->initData->Add(currentLex);
                    if (fullTokenStream)
                    {
                        fullTokenStream->Add(currentLex);
                        currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                        lines = nullptr;
                    }
                    getsym();
                }
            }
            if (MATCHKW(Keyword::openpa_) || MATCHKW(Keyword::begin_))
            {
                Keyword open = KW(), close = open == Keyword::openpa_ ? Keyword::closepa_ : Keyword::end_;
                int paren = 0;
                v->initData->Add(currentLex);
                if (fullTokenStream)
                {
                    fullTokenStream->Add(currentLex);
                    currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                    lines = nullptr;
                }

                getsym();
                while (currentLex && (!MATCHKW(close) || paren))
                {
                    if (MATCHKW(open))
                        paren++;
                    if (MATCHKW(close))
                        paren--;
                    v->initData->Add(currentLex);
                    if (fullTokenStream)
                    {
                        fullTokenStream->Add(currentLex);
                        currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                        lines = nullptr;
                    }
                    getsym();
                }
                if (MATCHKW(close))
                {
                    v->initData->Add(currentLex);
                    if (fullTokenStream)
                    {
                        fullTokenStream->Add(currentLex);
                        currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                        lines = nullptr;
                    }
                    getsym();
                }
                if (MATCHKW(Keyword::ellipse_))
                {
                    ClearPackedSequence();
                    v->packed = true;
                    if (fullTokenStream)
                    {
                        fullTokenStream->Add(currentLex);
                        currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                        lines = nullptr;
                    }
                    getsym();
                }
            }
            else
            {
                error(ERR_MEMBER_INITIALIZATION_REQUIRED);
                skip(Keyword::closepa_);
                break;
            }
            rv->push_back(v);
        }
        else
        {
            error(ERR_MEMBER_NAME_REQUIRED);
        }
        LeavePackedSequence();
        if (!MATCHKW(Keyword::comma_))
            break;
        if (fullTokenStream)
        {
            fullTokenStream->Add(currentLex);
            currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
            lines = nullptr;
        }
        getsym();
    }
    return rv;
}
LexemeStream* GetStructTokenStream()
{
    auto fullTokenStream = LexemeStreamFactory::Instantiation().Create();
    if (KW() == Keyword::colon_)
    {
        fullTokenStream->Add(currentLex);
        currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
        lines = nullptr;
        getsym();
        while (currentLex)
        {
            while (currentLex && KW() == Keyword::public_ || KW() == Keyword::protected_ || KW() == Keyword::private_)
            {
                fullTokenStream->Add(currentLex);
                currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                lines = nullptr;
                getsym();
            }
            if (currentLex && (KW() == Keyword::classsel_ || ISID()))
            {
                LexemeStreamPosition pos(currentStream);
                SYMBOL* sym = nullptr;
                FindClass(theCurrentFunc, &sym);
                CopyParsedLexemes(fullTokenStream, pos);
                if (ISID())
                {
                    fullTokenStream->Add(currentLex);
                    currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
                    lines = nullptr;
                    getsym();
                }
            }
            if (KW() != Keyword::comma_)
            {
                break;
            }
            fullTokenStream->Add(currentLex);
            currentLex->linedata = lines && lines->size() ? lines->front() : &nullLineData;
            lines = nullptr;
            getsym();
        }
    }
    if (currentLex && KW() == Keyword::begin_)
    {
        int begin = 0;
        while (currentLex)
        {
            fullTokenStream->Add(currentLex);
            auto kw = KW();
            getsym();
            if (kw == Keyword::begin_)
            {
                begin++;
            }
            else if (kw == Keyword::end_)
            {
                if (--begin == 0)
                    break;
            }
        }
    }
    return fullTokenStream;
}

LexemeStream* CopyParsedLexemes(LexemeStream* fullTokenStream, LexemeStreamPosition& pos)
{
    if (Optimizer::cparams.prm_cplusplus)
    {
        LexemeStreamPosition end(currentStream);
        for (; pos != end; ++pos)
        {
            if (!fullTokenStream)
                fullTokenStream = LexemeStreamFactory::Instantiation().Create();
            fullTokenStream->Add(pos.get());
        }
    }
    return fullTokenStream;
}

}  // namespace Parser
