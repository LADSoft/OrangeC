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
#ifndef _UTYPE_H_
    #define _UTYPE_H_

    #define TRUE 1
    #define FALSE 0
    #define ABS(x)  ((x) < 0 ? -(x) : (x))
    typedef unsigned int uint;
    typedef unsigned char BYTE;
//    typedef BYTE uchar;
#ifndef BOOL_DEFINED
    typedef uint BOOL;
#endif
    typedef unsigned short ushort;
    typedef uint(*FUNC)();

#endif /* _UTYPE_H_ */
