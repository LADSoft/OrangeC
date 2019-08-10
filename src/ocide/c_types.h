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

#ifndef C_TYPES_H
#define C_TYPES_H

enum Basictype
{
    /* keep this ordering and dont insert anything before the end of the
     * basic types, type comparisons (LOSTCONV) depends on the ordering,
     * and the debug info has a table indexed by type
     */
    BT_BIT,
    BT_BOOL,
    BT_CHAR,
    BT_UNSIGNEDCHAR,
    BT_SHORT,
    BT_UNSIGNEDSHORT,
    BT_WCHAR_T,
    BT_ENUM,
    BT_INT,
    BT_UNSIGNEDINT,
    BT_LONG,
    BT_UNSIGNEDLONG,
    BT_LONGLONG,
    BT_UNSIGNEDLONGLONG,
    BT_FLOAT,
    BT_DOUBLE,
    BT_LONGDOUBLE,
    BT_FLOATIMAGINARY,
    BT_DOUBLEIMAGINARY,
    BT_LONGDOUBLEIMAGINARY,
    BT_FLOATCOMPLEX,
    BT_DOUBLECOMPLEX,
    BT_LONGDOUBLECOMPLEX,
    /* end of basic types */
    BT_VOID,
    /* end of debug needs */
    BT_UNTYPED,
    BT_TYPEDEF,
    BT_POINTER,
    BT_REF,
    BT_FARPOINTER,
    BT_STRUCT,
    BT_UNION,
    BT_FUNC,
    BT_CLASS,
    BT_ICLASS,
    BT_IFUNC,
    BT_MATCHALL,
    BT_MATCHNONE,
    BT_ELLIPSE,
    BT_BITFIELD,
    BT_MEMBERPTR,
    BT_DEFUNC,
    BT_COND,
    BT_CONSPLACEHOLDER,
    BT_TEMPLATEPLACEHOLDER,
    BT_SEGPOINTER,
    BT_STRING
};

typedef struct _type_t
{
    struct _type_t* link;
    struct _symbol_t* symbols;
    char* name;
    char* typedefname;
    enum Basictype btype;
    int elements;
    int t_const : 1;
    int t_volatile : 1;
    int t_restrict : 1;
    int byValue : 1;
} type_t;

#endif  // C_TYPES_H