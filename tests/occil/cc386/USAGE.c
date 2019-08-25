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
/*
 * Display usage information and exit.
 */
#include <windows.h>
#undef SearchPath
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#define BOOL_DEFINED
#include "utype.h"	
#include "cmdline.h"	
#include "lists.h"
#include "expr.h"
#include "ccerr.h"
#include "c.h"
#include "diag.h"
#include "winmode.h"
/*
 * Main program must define the usage text
 */
extern char *usage_text;

/* Program banner */
void banner(char *fmt, ...)
{
    va_list argptr;

    #ifndef MSDOS
        if (GetStdHandle(STD_INPUT_HANDLE) == INVALID_HANDLE_VALUE)
            return ;
    #endif 
    putc('\n', stdout);

    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);

    putc('\n', stdout);
}

/* Print usage info */
void usage(char *prog_name)
{
    char *short_name;
    char *extension;

    short_name = strrchr(prog_name, '\\');
    if (short_name == NULL)
        short_name = strrchr(prog_name, '/');
    if (short_name == NULL)
        short_name = strrchr(prog_name, ':');
    if (short_name)
        short_name++;
    else
        short_name = prog_name;

    extension = strrchr(short_name, '.');
    if (extension != NULL)
        *extension = '\0';
    printf("\nUsage: %s %s", short_name, usage_text);
    #if sizeof(ULLONG_TYPE) == 4
        printf("   long long not supported");
    #endif 
    exit(1);
}
