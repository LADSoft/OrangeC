/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#ifdef USE_LONGLONG
#    ifdef BORLAND
#        define ULLONG_TYPE unsigned __int64
#        define LLONG_TYPE __int64
#        define LLONG_MIN _I64_MIN
#        define LLONG_MAX _I64_MAX
#        define ULLONG_MAX ((_I64_MAX << 1) + 1)
#    else
#        define ULLONG_TYPE unsigned long long
#        define LLONG_TYPE long long
#    endif
#    define LLONG_FORMAT_SPECIFIER "%lld"
#else
#    define ULLONG_TYPE unsigned long
#    define LLONG_TYPE long
#    define ULLONG_MAX ULONG_MAX
#    define LLONG_FORMAT_SPECIFIER "%ld"
#endif

typedef unsigned BITINT;

typedef unsigned long ADDRESS;
typedef unsigned LCHAR;

typedef unsigned char UBYTE;

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include "ccerr.h"
#include "floating.h"
#include "osutil.h"
#include "pp.h"
#include "iexpr.h"
#include "c.h"
#include "iopt.h"
#include "beinterf.h"

#include "compiler.p"

#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
