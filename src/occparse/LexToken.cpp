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
#include <functional>
#include "compiler.h"
#include "lex.h"
#include "templatedecl.h"

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
LexemeStreamPosition::~LexemeStreamPosition()
{
    tokenContext->Register(this, false);
}
void LexemeStreamPosition::Replay(std::function<void()> callback)
{
    Backup();
    callback();
    Restore();
}
Lexeme* LexemeStreamPosition::get()
{
    return tokenContext->get(origPosition);
}
void LexemeStreamPosition::Backup()
{
    currentPosition = tokenContext->Index();
    tokenContext->Index(origPosition);
    currentLex = tokenContext->get(tokenContext->Index());
}
void LexemeStreamPosition::Restore()
{
    tokenContext->Index(currentPosition);
    currentLex = tokenContext->get(tokenContext->Index());
}
void LexemeStreamPosition::Bump()
{
    origPosition = tokenContext->Index();
}
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
    if (current > top) ++top;
}
LexemeStream& LexemeStream::operator--()
{
    if (current > currentBase)
    {
        --current;
    }
    currentLex = data[current - currentBase];
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
        if (i->CheckMin(currentBase + pruneSize))
            return false;
    }
    return true;
}
void LexemeStream::Register(LexemeStreamPosition* position, bool enable)
{
    if (enable)
    {
        positions.insert(position);
    }
    else
    {
        positions.erase(position);
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
    auto lex = currentLex;
    newStream->reset();
    contextStack.push(newStream);
    currentStream = newStream;
    currentLex = newStream->get(newStream->Index());
    callback();
    contextStack.pop();
    currentStream = contextStack.top();
    currentLex = lex;
}

} // namespace Parser
