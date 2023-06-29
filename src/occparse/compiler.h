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

/* the long long type is 8 bytes...  if neither long long nor the long type is 8 byte in the compiler
 * this is compiled with, things will still work, however long long constants will be limited in size
 * and then automatically sign-extended out to long-long at runtime if necessary when used.  Also
 * some floating point conversions will have a more limited integer portion.
 */

#include "ctypes.h"

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <ctime>
#include <cstring>
#include <cfloat>
#include "Floating.h"
#include "osutil.h"
#include "ioptimizer.h"
#include "c.h"
#include "beinterfdefs.h"

#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
