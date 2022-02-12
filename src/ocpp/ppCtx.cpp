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

#include "PreProcessor.h"
#include "ppCtx.h"
#include "ppkw.h"

ppCtx::~ppCtx() {}
bool ppCtx::Check(kw token, std::string& line)
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
std::string ppCtx::GetId(std::string& line)
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
bool ppCtx::push(std::string& line)
{
    define.Process(line);
    stack.push_front(std::make_unique<CtxData>());
    CtxData* p = stack.front().get();
    p->id = nextId++;
    p->name = GetId(line);
    return true;
}
bool ppCtx::pop()
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
bool ppCtx::repl(std::string& line)
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
