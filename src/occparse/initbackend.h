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

extern Type stdobject;
extern Type stdvoid;
extern Type stdany;
extern Type stdauto;
extern Type stdfunc;
extern Type stdpointer;
extern Type std__string;
extern Type std__object;
extern Type stdnullpointer;
extern Type stdfloatcomplex;
extern Type stddoublecomplex;
extern Type stdlongdoublecomplex;
extern Type stdfloat;
extern Type stdfloatimaginary;
extern Type stddouble;
extern Type stddoubleimaginary;
extern Type stdlongdoubleimaginary;
extern Type stdlonglong;
extern Type stdunsigned;
extern Type stdunative;
extern Type stdunsignedlong;
extern Type stdunsignedlonglong;
extern Type stdconst;
extern Type stdchar;
extern Type stdchar8_t;
extern Type stdchar8_tptr;
extern Type stdchar16t;
extern Type stdchar16tptr;
extern Type stdchar32t;
extern Type stdchar32tptr;
extern Type stdsignedchar;
extern Type stdunsignedchar;
extern Type stdshort;
extern Type stdunsignedshort;
extern Type std__func__nc;
extern Type std__func__;
extern Type stdstring;
extern Type stdint;
extern Type stdinative;
extern Type stdlong;
extern Type stdlongdouble;
extern Type stdbool;
extern Type stdwidechar;
extern Type stdwcharptr;
extern Type stdcharptr;

int init_backend(void);
}  // namespace Parser