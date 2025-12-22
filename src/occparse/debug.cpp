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

#include <stack>

#include "compiler.h"
#include "lex.h"
namespace Parser
{
void displayLexeme(Lexeme* lex)
{
    const LCHAR* w;
    switch (currentLex->type)
    {
        case LexType::i_:
            printf("int constant: " LLONG_FORMAT_SPECIFIER "\n", currentLex->value.i);
            break;
        case LexType::ui_:
            printf("unsigned int constant: " LLONG_FORMAT_SPECIFIER "\n", currentLex->value.i);
            break;
        case LexType::l_:
            printf("long constant: " LLONG_FORMAT_SPECIFIER "\n", currentLex->value.i);
            break;
        case LexType::ul_:
            printf("unsigned long constant: " LLONG_FORMAT_SPECIFIER "\n", currentLex->value.i);
            break;
        case LexType::ll_:
            printf("long long constant: " LLONG_FORMAT_SPECIFIER "\n", currentLex->value.i);
            break;
        case LexType::ull_:
            printf("unsigned long long constant: " LLONG_FORMAT_SPECIFIER "\n", currentLex->value.i);
            break;
        case LexType::l_f_:
            printf("float constant: %s\n", ((std::string)*currentLex->value.f).c_str());
            break;
        case LexType::l_d_:
            printf("double constant: %s\n", ((std::string)*currentLex->value.f).c_str());
            break;
        case LexType::l_ld_:
            printf("long double constant: %s\n", ((std::string)*currentLex->value.f).c_str());
            break;
        case LexType::l_u8str_:
        case LexType::l_astr_:
        case LexType::l_msilstr_:
        case LexType::l_wstr_:
            if (currentLex->type == LexType::l_wstr_)
                printf("wide string: ");
            else if (currentLex->type == LexType::l_msilstr_)
                printf("msil string: ");
            else if (currentLex->type == LexType::l_astr_)
                printf("ascii string: ");
            else if (currentLex->type == LexType::l_u8str_)
                printf("utf8 string: ");
            w = currentLex->value.s.w;
            while (*w)
                fputc(*w++, stdout);
            fputc('\n', stdout);
            break;
        case LexType::l_achr_:
        case LexType::l_wchr_:
            if (currentLex->type == LexType::l_wchr_)
                printf("wide char: ");
            else
                printf("ascii char: ");
            fputc((int)currentLex->value.i, stdout);
            break;
        case LexType::l_id_:
            printf("id: %s\n", currentLex->value.s.a);
            break;
        case LexType::l_kw_:
            printf("kw: %s\n", currentLex->kw->name);
            break;
        default:
            printf("***** unknown token\n");
            break;
    }
}
}  // namespace Parser