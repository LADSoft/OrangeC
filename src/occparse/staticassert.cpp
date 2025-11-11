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

#include "compiler.h"
#include "config.h"
#include <map>
#include "templateutil.h"
#include "declare.h"
#include "lex.h"
#include "ccerr.h"
#include "constopt.h"
#include "expr.h"
#include "templatedecl.h"
#include "help.h"
#include "memory.h"
#include "staticassert.h"

namespace Parser
{
    int inStaticAssert;
    std::map<SYMBOL*, std::list<LexToken*>> structureStaticAsserts;

    void EnterStructureStaticAssert(SYMBOL* sym, LexToken* tokenStream) { structureStaticAsserts[sym].push_back(tokenStream); }
    void ParseStructureStaticAssert(SYMBOL* sym)
    {
        auto it = structureStaticAsserts.find(sym);
        if (it != structureStaticAsserts.end())
        {
            int n = PushTemplateNamespace(sym);
            enclosingDeclarations.Add(sym);
            auto hold = it->second;
            for (auto deferred : hold)
            {
                SwitchTokenStream(deferred);
                handleStaticAssert();
                SwitchTokenStream(nullptr);
            }
            enclosingDeclarations.Drop();
            PopTemplateNamespace(n);
        }
    }
    void handleStaticAssert()
    {
        RequiresDialect::Keyword(Dialect::c11, "_Static_assert");
        if (!MATCHKW(Keyword::openpa_))
        {
            errskim(skim_closepa);
            skip(Keyword::closepa_);
        }
        else if (structLevel && enclosingDeclarations.GetFirst())
        {
            auto stream = GetTokenStream(false);
            EnterStructureStaticAssert(enclosingDeclarations.GetFirst(), stream);
        }
        else
        {
            getsym();  // past '('
            bool v = true;
            char buf[5000];
            Type* tp;
            EXPRESSION* expr = nullptr, * expr2 = nullptr;
            inConstantExpression++;
            anonymousNotAlloc++;
            inStaticAssert++;
            expression_assign(nullptr, nullptr, &tp, &expr, nullptr, 0);
            inStaticAssert--;
            anonymousNotAlloc--;
             expr2 = Allocate<EXPRESSION>();
            expr2->type = ExpressionNode::x_bool_;
            expr2->left = expr->type == ExpressionNode::select_ ? expr->left : expr;
            optimize_for_constants(&expr2);
            inConstantExpression--;
            if (!isarithmeticconst(expr2) && !definingTemplate)
                error(ERR_CONSTANT_VALUE_EXPECTED);
            v = expr2->v.i;

            if (MATCHKW(Keyword::comma_))
            {
                getsym();
                if (currentLex->type != LexType::l_astr_)
                {
                    error(ERR_NEEDSTRING);
                    buf[0] = 0;
                    v = true; // don't generate the static assert
                }
                else
                {
                    int i, pos = 0;
                    while (currentLex->type == LexType::l_astr_)
                    {
                        Optimizer::SLCHAR* ch = (Optimizer::SLCHAR*)currentLex->value.s.w;
                        getsym();
                        for (i = 0; i < ch->count && i + pos < sizeof(buf) - 1; i++)
                            buf[i + pos] = ch->str[i];
                        pos += i;
                    }
                    buf[pos] = 0;
                }
            }
            else
            {
                Utils::StrCpy(buf, "(unspecified)");
            }
            if (!needkw(Keyword::closepa_))
            {
                errskim(skim_closepa);
                skip(Keyword::closepa_);
            }
            else if (!v && (!definingTemplate))  // || instantiatingTemplate))
            {
                errorstr(ERR_STATIC_ASSERT, buf);
            }
        }
        return;
    }

}  // namespace Parser