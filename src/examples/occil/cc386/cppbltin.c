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
/* Handles name mangling
 */
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"

extern int prm_cplusplus;
extern short *interjectptr;
extern TABLE *gsyms;
extern TABLE *tagtable;
extern int sys_inc;

SYM *dynamic_cast_func;
SYM *catch_cleanup_func;
SYM *typeinfo_class;
SYM *get_typeinfo_func;
SYM *throw_func;
SYM *rethrow_func;
SYM *global_vararray_func;

static short *cppbuiltin = L"void * operator new(unsigned size); " 
    L"void * operator new[](unsigned size); " 
    L"void   operator delete  (void *); " 
	L"void   operator delete[](void *); "
    L"void * __dynamic_cast(void *, void *, void *); " 
    L"void   _CatchCleanup(); " 
    L"void * __GetTypeInfo(void *block, void *instance); " 
    L"void _ThrowException(void *,void *,void*); " 
	L"void _RethrowException(); ";

static short *cbuiltin = L"void * __vararray_alloc(int); ";

void installCPPBuiltins(void)
{
    sys_inc = TRUE; // don't put in list file
    if (prm_cplusplus)
    {
        interjectptr = cppbuiltin;
        getch();
        getsym();
        dodecl(sc_global);
        interjectptr = 0;
        dynamic_cast_func = search("___dynamic_cast", gsyms);
        if (dynamic_cast_func)
            dynamic_cast_func = dynamic_cast_func->tp->lst.head;

        catch_cleanup_func = search("__CatchCleanup", gsyms);
        if (catch_cleanup_func)
            catch_cleanup_func = catch_cleanup_func->tp->lst.head;
        throw_func = search("__ThrowException", gsyms);
        if (throw_func)
            throw_func = throw_func->tp->lst.head;
        rethrow_func = search("__RethrowException", gsyms);
        if (rethrow_func)
            rethrow_func = rethrow_func->tp->lst.head;
        get_typeinfo_func = search("___GetTypeInfo", gsyms);
        if (get_typeinfo_func)
            get_typeinfo_func = get_typeinfo_func->tp->lst.head;
    }
    // builtins for C language
    interjectptr = cbuiltin;
    getch();
    getsym();
    dodecl(sc_global);
    interjectptr = 0;
    global_vararray_func = search("___vararray_alloc", gsyms);
    if (prm_cplusplus && global_vararray_func)
        global_vararray_func = global_vararray_func->tp->lst.head;
    sys_inc = FALSE;
}
