/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
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
namespace std {
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

void    _RTL_FUNC _delay(unsigned int __milliseconds);
int  _RTL_FUNC _int386( int, union REGS *, union REGS * );

void    _RTL_FUNC _intr( int, union REGPACK * );
void    _RTL_FUNC _nosound( void );
void    _RTL_FUNC _sound( unsigned __frequency );
void    _RTL_FUNC _segread(struct SREGS *__segp);

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
    using std::_vectorType;
    using std::__disable;
    using std::__enable;
    using std::_delay;
    using std::_int386;
    using std::_intr;
    using std::_nosound;
    using std::_sound;
    using std::_segread;
    using std::setvect;
    using std::REGS;
    using std::SREGS;
    using std::REGPACK;
    using std::INTPACK;
    using std::INTR_CF	;	/* carry */
    using std::INTR_PF	;	/* parity */
    using std::INTR_AF	;	/* auxiliary carry */
    using std::INTR_ZF	;	/* zero */
    using std::INTR_SF	;	/* sign */
    using std::INTR_TF	;	/* trace */
    using std::INTR_IF	;	/* interrupt */
    using std::INTR_DF	;	/* direction */
    using std::INTR_OF	;	/* overflow */
#endif
