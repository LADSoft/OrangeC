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

#ifndef ppCtx_h
#define ppCtx_h

#include <deque>
#include <string>
template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppDefine;

class CtxData
{
  public:
    int id;
    std::string name;
};
template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppCtx
{
  public:
    ppCtx(ppDefine& Define) : nextId(1), define(Define) {}
    ~ppCtx() {}
    int GetTopId()
    {
        if (!stack.empty())
            return stack.front()->id;
        else
            return -1;
    }
    bool Check(kw token, std::string& line)
    {
        bool rv = false;
        if (token == kw::PUSH)
            rv = push(line);
        else if (token == kw::POP)
            rv = pop();
        else if (token == kw::REPL)
            rv = repl(line);
        return rv;
    }
    bool Matches(const std::string& Name) const
    {
        if (!stack.empty())
            return stack.front()->name == Name;
        else
            return false;
    }

  protected:
    std::string GetId(std::string& line)
    {
        Tokenizer tk(line, nullptr);
        const Token* t = tk.Next();
        if (t->IsIdentifier())
        {
            return t->GetId();
        }
        else
        {
            Errors::Error("Identifier expected");
        }
        return "";
    }
    bool push(std::string& line)
    {
        define.Process(line);
        stack.push_front(std::make_unique<CtxData>());
        CtxData* p = stack.front().get();
        p->id = nextId++;
        p->name = GetId(line);
        return true;
    }
    bool pop()
    {
        if (!stack.empty())
        {
            stack.pop_front();
        }
        else
        {
            Errors::Error("%pop without %push");
        }
        return true;
    }
    bool repl(std::string& line)
    {
        if (!stack.empty())
        {
            define.Process(line);
            CtxData* p = stack.front().get();
            p->name = GetId(line);
        }
        else
        {
            Errors::Error("%repl without %push");
        }
        return true;
    }

  private:
    std::deque<std::unique_ptr<CtxData>> stack;
    ppDefine& define;
    int nextId;
};

#endif