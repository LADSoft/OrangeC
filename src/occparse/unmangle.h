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

extern const char* tn_void;
extern const char* tn_bool;
extern const char* tn_char;
extern const char* tn_int;
extern const char* tn_long;
extern const char* tn_longlong;
extern const char* tn_short;
extern const char* tn_unsigned;
extern const char* tn_signed;
extern const char* tn_ellipse;
extern const char* tn_float;
extern const char* tn_double;
extern const char* tn_longdouble;
extern const char* tn_volatile;
extern const char* tn_const;
extern const char* tn_class;
extern const char* tn_struct;
extern const char* tn_union;
extern const char* tn_floatcomplex;
extern const char* tn_doublecomplex;
extern const char* tn_longdoublecomplex;
extern const char* tn_floatimaginary;
extern const char* tn_doubleimaginary;
extern const char* tn_longdoubleimaginary;
extern const char* tn_wchar_t;
extern const char* tn_char8_t;
extern const char* tn_char16_t;
extern const char* tn_char32_t;

extern const char* cpp_funcname_tab[];
extern const char* xlate_tab[];

const char* unmang_intrins(char* buf, const char* name, const char* last);
char* unmangleExpression(char* dest, const char** name);
const char* unmang1(char* buf, const char* name, const char* last, bool tof);
char* unmangle(char* val, const char* name);
}  // namespace Parser