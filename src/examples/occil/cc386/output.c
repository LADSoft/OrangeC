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
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "gen386.h"
#include "diag.h"
#include "lists.h"                       

void oinit(void)
{
}

//-------------------------------------------------------------------------

void oflush(FILE *file)
{
	fflush(file);
	oinit();
}

//-------------------------------------------------------------------------

void oputc(int ch, FILE *file)
{

	if (file)
		fputc(ch,file);
}


//-------------------------------------------------------------------------
void owrite(char *buf, size_t size, int n, FILE *fil)
{
	if (fil)
		fwrite(buf,size,n,fil);
}

//-------------------------------------------------------------------------
void oprintf(FILE *file, char *format, ...)
{
	if (file) {
	    va_list arg;
    	va_start(arg, format);
    	vfprintf(file, format, arg);
	    va_end(arg);
	}
}
