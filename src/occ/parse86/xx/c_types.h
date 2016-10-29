/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef C_TYPES_H
#define C_TYPES_H

enum Basictype
{
    /* keep this ordering and dont insert anything before the end of the
     * basic types, type comparisons (LOSTCONV) depends on the ordering,
     * and the debug info has a table indexed by type
     */
    BT_BIT, BT_BOOL, BT_CHAR, BT_UNSIGNEDCHAR, BT_SHORT, BT_UNSIGNEDSHORT, BT_WCHAR_T, BT_ENUM,
        BT_INT, BT_UNSIGNEDINT, BT_LONG, BT_UNSIGNEDLONG, BT_LONGLONG,
        BT_UNSIGNEDLONGLONG, BT_FLOAT, BT_DOUBLE, BT_LONGDOUBLE, BT_FLOATIMAGINARY,
        BT_DOUBLEIMAGINARY, BT_LONGDOUBLEIMAGINARY,
        BT_FLOATCOMPLEX, BT_DOUBLECOMPLEX, BT_LONGDOUBLECOMPLEX, 
    /* end of basic types */
    BT_VOID, 
    /* end of debug needs */
    BT_UNTYPED, BT_TYPEDEF, BT_POINTER, BT_REF, BT_FARPOINTER, BT_STRUCT,
        BT_UNION, BT_FUNC, BT_CLASS, BT_ICLASS, BT_IFUNC, BT_MATCHALL,
        BT_MATCHNONE, BT_ELLIPSE, BT_BITFIELD, BT_MEMBERPTR, BT_DEFUNC, BT_COND,
        BT_CONSPLACEHOLDER, BT_TEMPLATEPLACEHOLDER, BT_SEGPOINTER, BT_STRING
};

typedef struct _type_t
{
    struct _type_t *link;
    struct _symbol_t *symbols;
    char *name;
    char *typedefname;
    enum Basictype btype;
    int elements;
    int t_const:1;
    int t_volatile:1;
    int t_restrict:1;
    int byValue:1;
} type_t;

#endif //C_TYPES_H