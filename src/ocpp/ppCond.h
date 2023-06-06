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

#ifndef ppCond_h
#define ppCond_h

#include <string>
#include <deque>
#include <memory>

#include "ppExpr.h"
class ppDefine;
class ppCtx;

class ppCond
{
  public:
    ppCond(bool isunsignedchar, bool C89, bool Extensions, bool AsmPP) :
        define(nullptr), c89(C89), expr(isunsignedchar), extensions(Extensions), ctx(nullptr), asmpp(AsmPP){};
    ~ppCond();
    void SetParams(ppDefine* Define, ppCtx* Ctx)
    {
        define = Define;
        ctx = Ctx;
        expr.SetParams(Define);
    }
    bool Check(kw token, const std::string& line, int lineno);
    void CheckErrors();
    bool Skipping() { return current && current->skipping; }
    void Mark() { marks.push_front(skipList.size() + current.get() != nullptr); }
    void Drop()
    {
        if (marks.size())
            marks.pop_front();
    }
    void Release()
    {
        if (marks.size())
        {
            int n = marks.front();
            marks.pop_front();
            while ((skipList.size() + (current != nullptr)) > n)
            {
                std::string s("");
                HandleEndIf(s);
            }
        }
    }

  protected:
    ppCond(const ppCond&);
    void HandleIf(bool val, const std::string& line, int lineno);
    void HandleElif(bool val, const std::string& line);
    void HandleElse(std::string& line);
    void HandleEndIf(std::string& line);
    void HandleDef(std::string& line, bool Else, bool negate, int lineno);
    void HandleIdn(std::string& line, bool Else, bool negate, bool caseSensitive, int lineno);
    void HandleId(std::string& line, bool Else, bool negate, int lineno);
    void HandleNum(std::string& line, bool Else, bool negate, int lineno);
    void HandleStr(std::string& line, bool Else, bool negate, int lineno);
    void HandleCtx(std::string& line, bool Else, bool negate, int lineno);
    void ansieol(const std::string& args);

  private:
    class skip
    {
      public:
        skip() : skipping(false), takeElse(false), elseSeen(false), line(0) {}
        bool skipping;
        bool takeElse;
        bool elseSeen;
        int line;
    };

    std::deque<std::unique_ptr<skip>> skipList;
    std::deque<int> marks;
    std::unique_ptr<skip> current;

    ppDefine* define;
    ppExpr expr;
    bool c89;
    bool extensions;
    ppCtx* ctx;
    bool asmpp;
};

#endif