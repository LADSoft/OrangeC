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
#include "ppCommon.h"

#include "ppExpr.h"
#include "ForwardDecls.h"
template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppCond
{
  public:
    ppCond(bool isunsignedchar, Dialect dialect_, bool Extensions, bool AsmPP) :
        define(nullptr), dialect(dialect_), expr(isunsignedchar, dialect_), extensions(Extensions), ctx(nullptr), asmpp(AsmPP){};
    ~ppCond() {}
    void SetParams(ppDefine<T, isSymbolChar>* Define, ppCtx<T, isSymbolChar>* Ctx)
    {
        define = Define;
        ctx = Ctx;
        expr.SetParams(Define);
    }
    bool Check(kw token, const std::string& line, int lineno)
    {
        std::string line1 = line;
        switch (token)
        {
            case kw::ELSE:
                HandleElse(line1);
                break;
            case kw::ENDIF:
                HandleEndIf(line1);
                break;
            case kw::IF:
                define->replaceDefined(line1);
                define->Process(line1);
                HandleIf(current.get() && current.get()->skipping ? true : expr.Eval(line1, true), line1, lineno);
                break;
            case kw::ELIF:
                define->replaceDefined(line1);
                define->Process(line1);
                HandleElif(expr.Eval(line1, true), line1);
                break;
            case kw::IFDEF:
                HandleDef(line1, false, false, lineno);
                break;
            case kw::IFNDEF:
                HandleDef(line1, false, true, lineno);
                break;
            case kw::ELIFDEF:
                HandleDef(line1, true, false, lineno);
                break;
            case kw::ELIFNDEF:
                HandleDef(line1, true, true, lineno);
                break;
            case kw::IFIDN:
                HandleIdn(line1, false, false, true, lineno);
                break;
            case kw::ELIFIDN:
                HandleIdn(line1, true, false, true, lineno);
                break;
            case kw::IFNIDN:
                HandleIdn(line1, false, true, true, lineno);
                break;
            case kw::ELIFNIDN:
                HandleIdn(line1, true, true, true, lineno);
                break;
            case kw::IFIDNI:
                HandleIdn(line1, false, false, false, lineno);
                break;
            case kw::ELIFIDNI:
                HandleIdn(line1, true, false, false, lineno);
                break;
            case kw::IFNIDNI:
                HandleIdn(line1, false, true, false, lineno);
                break;
            case kw::ELIFNIDNI:
                HandleIdn(line1, true, true, false, lineno);
                break;
            case kw::IFID:
                HandleId(line1, false, false, lineno);
                break;
            case kw::ELIFID:
                HandleId(line1, true, false, lineno);
                break;
            case kw::IFNID:
                HandleId(line1, false, true, lineno);
                break;
            case kw::ELIFNID:
                HandleId(line1, true, true, lineno);
                break;
            case kw::IFNUM:
                HandleNum(line1, false, false, lineno);
                break;
            case kw::ELIFNUM:
                HandleNum(line1, true, false, lineno);
                break;
            case kw::IFNNUM:
                HandleNum(line1, false, true, lineno);
                break;
            case kw::ELIFNNUM:
                HandleNum(line1, true, true, lineno);
                break;
            case kw::IFSTR:
                HandleStr(line1, false, false, lineno);
                break;
            case kw::ELIFSTR:
                HandleStr(line1, true, false, lineno);
                break;
            case kw::IFNSTR:
                HandleStr(line1, false, true, lineno);
                break;
            case kw::ELIFNSTR:
                HandleStr(line1, true, true, lineno);
                break;
            case kw::IFCTX:
                HandleCtx(line1, false, false, lineno);
                break;
            case kw::ELIFCTX:
                HandleCtx(line1, true, false, lineno);
                break;
            case kw::IFNCTX:
                HandleCtx(line1, false, true, lineno);
                break;
            case kw::ELIFNCTX:
                HandleCtx(line1, true, true, lineno);
                break;
            default:
                return current && current->skipping;
        }
        return true;
    }
    void CheckErrors()
    {
        if (!skipList.empty() || current)
        {
            Errors::Error("Non-terminated preprocessor conditional started in line " + Errors::ToNum(current->line));
        }
    }
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
    void HandleIf(bool val, const std::string& line, int lineno)
    {
        skip* old = current.get();
        bool skipping = false;
        if (old)
            skipping = old->skipping;
        if (current)
            skipList.push_front(std::move(current));
        current = std::make_unique<skip>();
        current->line = lineno;
        if (skipping)
        {
            current->skipping = true;
        }
        else if (!val)
        {
            current->skipping = true;
            current->takeElse = true;
        }
    }
    void HandleElif(bool val, const std::string& line)
    {
        ansieol(line);
        if (!current || current->elseSeen)
        {
            Errors::Error("Misplaced elif directive");
        }
        else
        {
            skip* old = nullptr;
            if (!skipList.empty())
                old = skipList.front().get();
            if ((!old || !old->skipping) && current->takeElse && val)
            {
                current->skipping = false;
                current->takeElse = false;
            }
            else
            {
                current->skipping = true;
            }
        }
    }
    void HandleElse(std::string& line)
    {
        int npos = line.find_first_not_of(" \r\n\t\v");
        if (npos != std::string::npos)
        {
            std::string line1 = line;
            if (line1.substr(npos, 2) == "if")
            {
                line1 = line1.substr(npos + 2);
                define->replaceDefined(line1);
                define->Process(line1);
                HandleElif(expr.Eval(line1, true), line1);
                return;
            }
        }
        ansieol(line);
        if (!current)
        {
            Errors::Error("Misplaced else directive");
        }
        else
        {
            if (current->takeElse)
            {
                current->skipping = false;
            }
            else
            {
                current->skipping = true;
            }
            if (current->elseSeen)
            {
                Errors::Error("Misplaced else directive");
            }
            current->elseSeen = true;
        }
    }
    void HandleEndIf(std::string& line)
    {
        ansieol(line);
        if (!current)
        {
            Errors::Error("Misplaced endif directive");
        }
        else
        {
            if (!skipList.empty())
            {
                current = std::move(skipList.front());
                skipList.pop_front();
            }
            else
            {
                current = nullptr;
            }
        }
    }
    void HandleDef(std::string& line, bool Else, bool negate, int lineno)
    {
        Tokenizer<kw> tk(line, nullptr);
        const Token* t = tk.Next();
        if (!t->IsIdentifier())
        {
            Errors::Error("Identifier expected");
            if (Else)
                HandleElif(false, "");
            else
                HandleIf(false, "", lineno);
        }
        else
        {
            bool v = !!define->Lookup(t->GetId());
            if (asmpp && !v)
            {
                std::string one = UTF8::ToUpper(t->GetId());
                Symbol* t = define->Lookup(one);
                if (t)
                {
                    ppDefine<T, isSymbolChar>::Definition* p = static_cast<ppDefine<T, isSymbolChar>::Definition*>(t);
                    v = p->IsCaseInsensitive();
                }
            }
            if (negate)
                v = !v;
            if (Else)
                HandleElif(v, tk.GetString());
            else
                HandleIf(v, tk.GetString(), lineno);
        }
    }
    void HandleIdn(std::string& line, bool Else, bool negate, bool caseSensitive, int lineno)
    {
        define->Process(line);
        size_t n = line.find_first_of(',');
        if (n == std::string::npos)
        {
            Errors::Error("Syntax error in string comparison directive");
            if (Else)
                HandleElif(false, "");
            else
                HandleIf(false, "", lineno);
        }
        else
        {
            std::string line1 = line.substr(0, n);
            line.erase(0, n + 1);
            n = line.find_first_not_of(" \t\v\n");
            if (n && n != std::string::npos)
                line.erase(0, n);
            n = line.find_last_not_of(" \t\v\n");
            if (n != std::string::npos && n != line.size() - 1)
                line.erase(n + 1, line.size() - n - 1);
            n = line1.find_first_not_of(" \t\v\n");
            if (n && n != std::string::npos)
                line1.erase(0, n);
            n = line1.find_last_not_of(" \t\v\n");
            if (n != std::string::npos && n != line1.size() - 1)
                line1.erase(n + 1, line1.size() - n - 1);
            int pos = 0, pos1 = 0;
            int z = 0;
            if (!caseSensitive)
            {
                line = UTF8::ToUpper(line);
                line1 = UTF8::ToUpper(line);
            }
            while (true)
            {
                if (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\v' || line[pos] == '\n')
                {
                    z ^= 1;
                    pos = line.find_first_not_of(" \t\v\n", pos);
                    if (pos == std::string::npos)
                        pos = line.size();
                }
                if (line1[pos1] == ' ' || line1[pos1] == '\t' || line1[pos1] == '\v' || line1[pos1] == '\n')
                {
                    z ^= 1;
                    pos1 = line1.find_first_not_of(" \t\v\n", pos);
                    if (pos1 == std::string::npos)
                        pos1 = line1.size();
                }
                if (z || pos == line.size() || pos1 == line1.size())
                    break;
                if (line[pos] != line1[pos1])
                    break;
                pos++, pos1++;
                if (pos == line.size() || pos1 == line1.size())
                    break;
            }
            bool v = !z && pos == line.size() && pos1 == line1.size();
            if (negate)
                v = !v;
            if (Else)
                HandleElif(v, "");
            else
                HandleIf(v, "", lineno);
        }
    }
    void HandleId(std::string& line, bool Else, bool negate, int lineno)
    {
        define->Process(line);
        Tokenizer<kw> tk(line, nullptr);
        const Token* t = tk.Next();
        bool v = t->IsIdentifier();
        if (negate)
            v = !v;
        if (Else)
            HandleElif(v, tk.GetString());
        else
            HandleIf(v, tk.GetString(), lineno);
    }
    void HandleNum(std::string& line, bool Else, bool negate, int lineno)
    {
        define->Process(line);
        Tokenizer<kw> tk(line, nullptr);
        const Token* t = tk.Next();
        bool v = t->IsNumeric();
        if (negate)
            v = !v;
        if (Else)
            HandleElif(v, tk.GetString());
        else
            HandleIf(v, tk.GetString(), lineno);
    }
    void HandleStr(std::string& line, bool Else, bool negate, int lineno)
    {
        define->Process(line);
        Tokenizer<kw> tk(line, nullptr);
        const Token* t = tk.Next();
        bool v = t->IsString();
        if (negate)
            v = !v;
        if (Else)
            HandleElif(v, tk.GetString());
        else
            HandleIf(v, tk.GetString(), lineno);
    }
    void HandleCtx(std::string& line, bool Else, bool negate, int lineno)
    {
        define->Process(line);
        Tokenizer<kw> tk(line, nullptr);
        const Token* t = tk.Next();
        bool v = false;
        if (t->IsIdentifier())
        {
            v = ctx->Matches(t->GetId());
            if (negate)
                v = !v;
        }
        else
        {
            Errors::Error("Identifier expected");
        }
        if (Else)
            HandleElif(v, tk.GetString());
        else
            HandleIf(v, tk.GetString(), lineno);
    }
    void ansieol(const std::string& line)
    {
        if (!extensions)
        {
            const char* p = line.c_str();
            while (*p)
            {
                if (!isspace(*p))
                {
                    Errors::Error("Extra characters past end of preprocessor directive");
                    break;
                }
                p++;
            }
        }
    }

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

    ppDefine<T, isSymbolChar>* define;
    ppExpr<T, isSymbolChar> expr;
    Dialect dialect;
    bool extensions;
    ppCtx<T, isSymbolChar>* ctx;
    bool asmpp;
};

#endif