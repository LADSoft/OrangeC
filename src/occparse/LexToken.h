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
#include <functional>
#include <stack>
#include <cassert>

namespace Parser
{
struct Lexeme;
struct LexemeStream;

struct LexemeStreamPosition
{
    LexemeStreamPosition(LexemeStream* Context);
    ~LexemeStreamPosition();
    void Replay(std::function<void()> callback);
    void Backup();
    void Restore();
    void Bump();
    int Position() const { return origPosition; }
    void Position(int pos) { origPosition = pos;  }
    bool CheckMin(int pos) { return origPosition < pos || currentPosition < pos; }
    Lexeme* get();
    bool operator != (LexemeStreamPosition& right) { return this->origPosition != right.origPosition;  }
    LexemeStreamPosition& operator++() { origPosition++; return* this; }
private:
    int origPosition;
    int currentPosition = 0;
    LexemeStream* tokenContext;
    Lexeme* backupLex = nullptr;
};
struct LexemeTokenFactory
{
    static LexemeTokenFactory& Instantiation()
    {
        Initialize();
        return *lexemeTokenFactory;
    }
    Lexeme* Create();
    void Destroy(Lexeme* lex, int pruneSize);
    void clear();
private:
    static void Initialize();
    std::deque<Lexeme*> cache;

    static LexemeTokenFactory* lexemeTokenFactory;
};
struct LexemeStream
{
    typedef std::vector<Lexeme*>::iterator iterator;

    void Add(Lexeme* lex);
    LexemeStream& operator++()
    {
        if (current < currentBase + data.size())
        {
            if (current == top)
            {
                ++top;
            }
            ++current;
        }
        return *this;
    }
    LexemeStream& operator--();
    int Index()
    {
        return current;
    }
    void Index(int index)
    {
        current = index;
    }
    Lexeme* get(int index)
    {
        assert(index >= currentBase && index <= currentBase + size());
        if (index == currentBase + size())
            return nullptr;
        return data[index - currentBase];
    }
    void reset()
    {
        assert(currentBase == 0);
        replaying = true;
        current = top = 0;
    }
    bool RePlaying()
    {
        return replaying;
    }
    bool Reloaded()
    {
        return current < top;
    }
    void Prune(unsigned maxDepth, unsigned pruneSize);
    bool ValidPrune(unsigned pruneSize);
    void Register(LexemeStreamPosition* position, bool enable);
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    size_t size() const { return data.size(); }
    int Base() const { return currentBase; }
    int ReloadIndex() const { return reloadIndex; }
    void ReloadIndex(int val) { reloadIndex = val; }
private:
    bool replaying = false;
    int current = -1;
    int top = -1;
    int currentBase = 0;
    int reloadIndex = -1;
    std::vector<Lexeme*> data;
    std::set<LexemeStreamPosition*> positions;
};
struct LexemeStreamFactory
{
    static LexemeStreamFactory& Instantiation()
    {
        Initialize();
        return *lexemeContextFactory;
    }
    LexemeStream* Create()
    {
        auto tokens = new LexemeStream();
        lists.insert(tokens);
        return tokens;
    }
    void Destroy(LexemeStream* tokens)
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
private:
    static void Initialize();
    std::set<LexemeStream*> lists;

    static LexemeStreamFactory* lexemeContextFactory;
};
extern void lextokeninit();

extern LexemeStreamFactory streamFactory;
extern LexemeTokenFactory lexemeFactory;

extern std::stack<LexemeStream*> contextStack;

extern Lexeme* currentLex;
extern LexemeStream* currentStream;

void ParseOnStream(LexemeStream* newContext, std::function<void()> callback);

} // namespace Parser