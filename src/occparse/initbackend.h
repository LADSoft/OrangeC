#pragma once
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

namespace Parser
{

extern TYPE stdobject;
extern TYPE stdvoid;
extern TYPE stdany;
extern TYPE stdauto;
extern TYPE stdfunc;
extern TYPE stdpointer;
extern TYPE std__string;
extern TYPE std__object;
extern TYPE stdnullpointer;
extern TYPE stdfloatcomplex;
extern TYPE stddoublecomplex;
extern TYPE stdlongdoublecomplex;
extern TYPE stdfloat;
extern TYPE stdfloatimaginary;
extern TYPE stddouble;
extern TYPE stddoubleimaginary;
extern TYPE stdlongdoubleimaginary;
extern TYPE stdlonglong;
extern TYPE stdunsigned;
extern TYPE stdunative;
extern TYPE stdunsignedlong;
extern TYPE stdunsignedlonglong;
extern TYPE stdconst;
extern TYPE stdchar;
extern TYPE stdchar8_t;
extern TYPE stdchar8_tptr;
extern TYPE stdchar16t;
extern TYPE stdchar16tptr;
extern TYPE stdchar32t;
extern TYPE stdchar32tptr;
extern TYPE stdsignedchar;
extern TYPE stdunsignedchar;
extern TYPE stdshort;
extern TYPE stdunsignedshort;
extern TYPE std__func__nc;
extern TYPE std__func__;
extern TYPE stdstring;
extern TYPE stdint;
extern TYPE stdinative;
extern TYPE stdlong;
extern TYPE stdlongdouble;
extern TYPE stdbool;
extern TYPE stdwidechar;
extern TYPE stdwcharptr;
extern TYPE stdcharptr;

int init_backend(void);
}  // namespace Parser