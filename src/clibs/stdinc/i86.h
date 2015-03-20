/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 *  i86.h	Defines the structs and unions used to handle the input and
 *    output registers for the Intel 386 interrupt interface
 *		routines.
 *
 */
#ifndef _I86_H_INCLUDED
#define _I86_H_INCLUDED

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#ifndef _REG_DEFS
#define _REG_DEFS

#undef __FILLER

/* dword registers */

struct DWORDREGS {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int esi;
    unsigned int edi;
    unsigned int cflag;
};
#define __FILLER(a) unsigned short a;

/* word registers */

struct WORDREGS {
    unsigned short ax;  __FILLER(_1)
    unsigned short bx;  __FILLER(_2)
    unsigned short cx;  __FILLER(_3)
    unsigned short dx;  __FILLER(_4)
    unsigned short si;  __FILLER(_5)
    unsigned short di;  __FILLER(_6)
    unsigned int cflag;
};

/* byte registers */

struct BYTEREGS {
    unsigned char al, ah;  __FILLER(_1)
    unsigned char bl, bh;  __FILLER(_2)
    unsigned char cl, ch;  __FILLER(_3)
    unsigned char dl, dh;  __FILLER(_4)
};

/* general purpose registers union - overlays the corresponding dword,
 * word, and byte registers.
 */

union REGS {
    struct DWORDREGS x;
    struct WORDREGS  w;
    struct BYTEREGS  h;
};
#define _REGS REGS

/* segment registers */

struct SREGS {
    unsigned short es, cs, ss, ds;
    unsigned short fs, gs;
};
#define _SREGS SREGS


/* intr structs */

struct REGPACKB {
    unsigned char al, ah;  __FILLER(_1)
    unsigned char bl, bh;  __FILLER(_2)
    unsigned char cl, ch;  __FILLER(_3)
    unsigned char dl, dh;  __FILLER(_4)
};

struct REGPACKW {
    unsigned short ax;  __FILLER(_1)
    unsigned short bx;  __FILLER(_2)
    unsigned short cx;  __FILLER(_3)
    unsigned short dx;  __FILLER(_4)
    unsigned short bp;  __FILLER(_5)
    unsigned short si;  __FILLER(_6)
    unsigned short di;  __FILLER(_7)
    unsigned short ds;
    unsigned short es;
    unsigned short fs;
    unsigned short gs;
    unsigned int flags;
};

struct REGPACKX {
    unsigned int   eax, ebx, ecx, edx, ebp, esi, edi;
    unsigned short ds, es, fs, gs;
    unsigned int   flags;
};

union REGPACK {
    struct REGPACKB h;
    struct REGPACKW w;
    struct REGPACKX x;
};

#endif    

/* input parm to an 'interrupt' function is union INTPACK */
/* e.g.  interrupt int10( union INTPACK r ) {}		  */

struct INTPACKX {
    unsigned gs,fs,es,ds,edi,esi,ebp,esp,ebx,edx,ecx,eax,eip,cs,flags;
};
struct INTPACKW {
    unsigned short gs;  __FILLER(_1)
    unsigned short fs;  __FILLER(_2)
    unsigned short es;  __FILLER(_3)
    unsigned short ds;  __FILLER(_4)
    unsigned short di;  __FILLER(_5)
    unsigned short si;  __FILLER(_6)
    unsigned short bp;  __FILLER(_7)
    unsigned short sp;  __FILLER(_8)
    unsigned short bx;  __FILLER(_9)
    unsigned short dx;  __FILLER(_a)
    unsigned short cx;  __FILLER(_b)
    unsigned short ax;  __FILLER(_c)
    unsigned short ip;  __FILLER(_d)
    unsigned short cs;  __FILLER(_e)
    unsigned flags;
};
struct INTPACKB {
    unsigned /*gs*/ :32,/*fs*/ :32,
         /*es*/ :32,/*ds*/ :32,
         /*edi*/:32,/*esi*/:32,
         /*ebp*/:32,/*esp*/:32;
    unsigned char bl, bh; __FILLER(_1)
    unsigned char dl, dh; __FILLER(_2)
    unsigned char cl, ch; __FILLER(_3)
    unsigned char al, ah; __FILLER(_4)
};
union  INTPACK {
    struct INTPACKB h;
    struct INTPACKW w;
    struct INTPACKX x;
};

/* bits defined for flags field defined in REGPACKW and INTPACKW */

enum {
    INTR_CF	= 0x0001,	/* carry */
    INTR_PF	= 0x0004,	/* parity */
    INTR_AF	= 0x0010,	/* auxiliary carry */
    INTR_ZF	= 0x0040,	/* zero */
    INTR_SF	= 0x0080,	/* sign */
    INTR_TF	= 0x0100,	/* trace */
    INTR_IF	= 0x0200,	/* interrupt */
    INTR_DF	= 0x0400,	/* direction */
    INTR_OF	= 0x0800	/* overflow */
};

#ifdef _cplusplus
extern "C" {
#endif

typedef void _interrupt (*_vectorType)(...) ;

void _RTL_INTRINS __disable( void );
void _RTL_INTRINS __enable( void );

void    _RTL_FUNC _IMPORT _delay(unsigned int __milliseconds);
int  _RTL_FUNC _IMPORT _int386( int, union REGS *, union REGS * );

void    _RTL_FUNC _IMPORT _intr( int, union REGPACK * );
void    _RTL_FUNC _IMPORT _nosound( void );
void    _RTL_FUNC _IMPORT _sound( unsigned __frequency );
void    _RTL_FUNC _IMPORT _segread(struct SREGS *__segp);

_vectorType setvect(int _num, _vectorType _vect) ;


#ifdef _cplusplus
} ;
};
#endif

/* macros to break 'far' pointers into segment and offset components */

#define MK_FP(__s,__o) ((void _far *)(((void _seg *)(__s)) + ((void _near *)(__o))))
#define _MK_FP(__s,__o)  MK_FP(__s,__o)

#define FP_SEG(__p)  ((unsigned)(void _seg *)(__p))
#define _FP_SEG(__p) ((unsigned)(void _seg *)(__p))

#define FP_OFF(__p)  ((unsigned)(__p))
#define _FP_OFF(__p) ((unsigned)(__p))

#ifdef __cplusplus
};
};
#endif

#pragma pack()

#endif /* __i86_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__I86_H_USING_LIST)
#define __I86_H_USING_LIST
    using __STD_NS_QUALIFIER _vectorType;
    using __STD_NS_QUALIFIER __disable;
    using __STD_NS_QUALIFIER __enable;
    using __STD_NS_QUALIFIER _delay;
    using __STD_NS_QUALIFIER _int386;
    using __STD_NS_QUALIFIER _intr;
    using __STD_NS_QUALIFIER _nosound;
    using __STD_NS_QUALIFIER _sound;
    using __STD_NS_QUALIFIER _segread;
    using __STD_NS_QUALIFIER setvect;
    using __STD_NS_QUALIFIER REGS;
    using __STD_NS_QUALIFIER SREGS;
    using __STD_NS_QUALIFIER REGPACK;
    using __STD_NS_QUALIFIER INTPACK;
    using __STD_NS_QUALIFIER INTR_CF	;	/* carry */
    using __STD_NS_QUALIFIER INTR_PF	;	/* parity */
    using __STD_NS_QUALIFIER INTR_AF	;	/* auxiliary carry */
    using __STD_NS_QUALIFIER INTR_ZF	;	/* zero */
    using __STD_NS_QUALIFIER INTR_SF	;	/* sign */
    using __STD_NS_QUALIFIER INTR_TF	;	/* trace */
    using __STD_NS_QUALIFIER INTR_IF	;	/* interrupt */
    using __STD_NS_QUALIFIER INTR_DF	;	/* direction */
    using __STD_NS_QUALIFIER INTR_OF	;	/* overflow */
#endif
