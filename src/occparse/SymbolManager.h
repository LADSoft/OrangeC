/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#include <unordered_map>

bool equalnode(Optimizer::SimpleExpression* left, Optimizer::SimpleExpression* right);
inline bool isarithmeticconst(Optimizer::SimpleExpression* e)
{
    switch (e->type)
    case Optimizer::se_i:
    case Optimizer::se_ui:
    case Optimizer::se_f:
    case Optimizer::se_fc:
    case Optimizer::se_fi:
        return true;
    return false;
}
inline bool isintconst(Optimizer::SimpleExpression* e)
{
    switch (e->type)
    case Optimizer::se_i:
    case Optimizer::se_ui:
        return true;
    return false;
}
inline bool isfloatconst(Optimizer::SimpleExpression* e)
{
    switch (e->type)
    case Optimizer::se_f:
        return true;
    return false;
}
inline bool iscomplexconst(Optimizer::SimpleExpression* e)
{
    switch (e->type)
    case Optimizer::se_fc:
        return true;
    return false;
}
inline bool isimaginaryconst(Optimizer::SimpleExpression* e)
{
    switch (e->type)
    case Optimizer::se_fi:
        return true;
    return false;
}

namespace Parser
{
void refreshBackendParams(SYMBOL* funcsp);
}