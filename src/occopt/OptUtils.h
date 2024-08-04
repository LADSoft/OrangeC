# Software License Agreement
# 
#     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 
# 

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

#include <map>
#include "FNV_hash.h"
namespace Optimizer
{
extern std::map<IMODE*, IMODE*> loadHash;
extern std::unordered_map<CASTTEMP*, IMODE*, OrangeC::Utils::fnv1a32_binary<sizeof(CASTTEMP)>, OrangeC::Utils::bin_eql<sizeof(CASTTEMP)>>
    castHash;
extern int tempCount;

extern LIST* immed_list[4091];

void constoptinit(void);
int sizeFromISZ(int isz);
int alignFromISZ(int isz);
int needsAtomicLockFromISZ(int isz);
SimpleSymbol* varsp(SimpleExpression* node);
IMODE* make_immed(int size, long long i);
IMODE* make_fimmed(int size, FPF f);
IMODE* make_parmadj(long i);
SimpleExpression* tempenode(void);
IMODE* tempreg(int size, int mode);
IMODE* GetLoadTemp(IMODE* dest);
IMODE* LookupStoreTemp(IMODE* dest, IMODE* src);
IMODE* LookupLoadTemp(IMODE* dest, IMODE* source);
IMODE* LookupImmedTemp(IMODE* dest, IMODE* source);
IMODE* LookupCastTemp(IMODE* im, int size);
SimpleExpression* simpleExpressionNode(enum se_type type, SimpleExpression* left, SimpleExpression* right);
SimpleExpression* simpleIntNode(enum se_type type, unsigned long long i);
IMODE* indnode(IMODE* ap1, int size);
int pwrof2(long long i);
long long mod_mask(int i);
void my_sprintf(char* dest, const char* fmt, ...);
void cacheTempSymbol(SimpleSymbol* sym);
}  // namespace Optimizer