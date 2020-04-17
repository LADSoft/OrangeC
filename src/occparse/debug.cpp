/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include "compiler.h"
void displayLexeme(LEXEME* lex)
{
    const LCHAR* w;
    switch (lex->type)
    {
        case l_i:
            printf("int constant: " LLONG_FORMAT_SPECIFIER "\n", lex->value.i);
            break;
        case l_ui:
            printf("unsigned int constant: " LLONG_FORMAT_SPECIFIER "\n", lex->value.i);
            break;
        case l_l:
            printf("long constant: " LLONG_FORMAT_SPECIFIER "\n", lex->value.i);
            break;
        case l_ul:
            printf("unsigned long constant: " LLONG_FORMAT_SPECIFIER "\n", lex->value.i);
            break;
        case l_ll:
            printf("long long constant: " LLONG_FORMAT_SPECIFIER "\n", lex->value.i);
            break;
        case l_ull:
            printf("unsigned long long constant: " LLONG_FORMAT_SPECIFIER "\n", lex->value.i);
            break;
        case l_f:
            printf("float constant: %s\n", ((std::string)*lex->value.f).c_str());
            break;
        case l_d:
            printf("double constant: %s\n", ((std::string)*lex->value.f).c_str());
            break;
        case l_ld:
            printf("long double constant: %s\n", ((std::string)*lex->value.f).c_str());
            break;
        case l_astr:
            printf("ascii string: ");
        case l_msilstr:

        case l_wstr:
            if (lex->type == l_wstr)
                printf("wide string: ");
            else if (lex->type == l_msilstr)
                printf("msil string: ");
            w = lex->value.s.w;
            while (*w)
                fputc(*w++, stdout);
            fputc('\n', stdout);
            break;
        case l_achr:
            printf("ascii char: ");
        case l_wchr:
            if (lex->type == l_wchr)
                printf("wide char: ");
            fputc((int)lex->value.i, stdout);
            break;
        case l_id:
            printf("id: %s\n", lex->value.s.a);
            break;
        case l_kw:
            printf("kw: %s\n", lex->kw->name);
            break;
        default:
            printf("***** unknown token\n");
            break;
    }
}