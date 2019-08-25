/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#define XD_X_MASK 0x3f           /* a count for extension bytes */
#define XD_ARRAY   0x40
#define XD_POINTER 0x80
#define XD_REF     0x100
#define XD_CL_PRIMARY 0x200
#define XD_CL_BASE    0x400
#define XD_CL_ENCLOSED 0x800
#define XD_CL_BASETYPE 0x1000
#define XD_CL_VTAB 0x2000
#define XD_CL_TRYBLOCK 0x4000
#define XD_CL_CONST    0x8000
#define XD_CL_BYREF	   0x10000
#define XD_CM_SYM (XD_CL_PRIMARY | XD_CL_BASE | XD_CL_ENCLOSED)
#define XD_VARARRAY	0x20000
#define XD_INUSE 0x40000
#define VTAB_XT_OFFS  12  /* VTAB offset - pointer to exception data offset */

// xcept block header
typedef struct
{
    void **xtrecord;
    int epboffs;
} FUNCXCEPTHDR;

// things in xcept block
typedef struct
{
    int flags;
    struct _classxcpt *xt;
    int ebpoffs; /* symbol stack offset, or tryblock label */
    int start;
    int end;
    // tryblock has an extra four bytes which are the label to jump to
} FUNCXCEPT;


typedef struct _classxcpt
{
    void(*destructor)(int); /* int = the this pointer, this var doubles as
     * the base type declarator for pointer types */
    int size; // total size of type
    int flags;
    char name[1]; // length, will be followed by enumerated name
    // if describing an array, following this will be number of elements
    // if describing a struct, following this will be list of CLASSEXCEPTREF structs
    // otherwise this is end of structure
} CLASSXCEPT;

typedef struct
{
    int flags;
    struct _classxcpt *xt;
    int offset;
    // array has an extra four bytes which is the size of the array
} CLASSXCEPTREF;

#define XC_SIG 0x4c41445a

// on stack for func
typedef struct _xceptdata
{
    struct _xceptdata *next; /* link to next exception higher function */
    void *_xceptfunc; /* windows exception handler */
    int sig; /* signature to show this is a valid block */
    int esp; /* esp at start of try block; code gen generates for this, don't
        move*/
    int ebp; /* ebp of this function */
    int xceptBaseEIP; /* EIP offsets in the table are based from,
     * e.g. right after the __InitExceptBlock call */
    FUNCXCEPTHDR *xceptBlock; /* pointer to the function's xception block */
    // things beyond this are used by throw()
    int flags; /* reserved */
    int eip; /* eip this function where the catch occurred */
    void *instance; /* instance pointer to thrown class */
    CLASSXCEPT *thrownxt; /* xt that was thrown */
    FUNCXCEPT *thisxt; /* pointer to this XT table list in case of throws
        through nested tries */
	void *copycons; /* copy constructor */
} XCEPTDATA;

// FS:[4] - 4
typedef struct _cppdata
{
    void(*term)();
    void(*unexpected)();
} CPPDATA;
