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
#ifndef _CMDLINE_H
    #define _CMDLINE_H

    #include "utype.h"
    #define VMSG(ms) ms" Version %d.%02d  Copyright (c) 1994-1997 LADsoft", (VERSION)/100,(VERSION)%100

    /* Directory buffer size */
    #define DIR_BUFFER 256

    /* Directory errors */
    #define DIR_OK 0
    #define DIR_BADFORMAT 2
    #define DIR_INVALID 3
    #define DRIVE_INVALID 4

    /* Mode values for ARGLIST */
    #define ARG_CONCATSTRING 1
    #define ARG_NOCONCATSTRING 2
    #define ARG_BOOL 3
    #define ARG_SWITCH 4
    #define ARG_SWITCHSTRING 5

    /* Valid arg separators */
    #define ARG_SEPSWITCH '/'
    #define ARG_SEPFALSE '-'
    #define ARG_SEPTRUE '+'

    /* Return values for dispatch routine */
    #define ARG_NEXTCHAR 1
    #define ARG_NEXTARG 2
    #define ARG_NEXTNOCAT 3
    #define ARG_NOMATCH 4
    #define ARG_NOARG 5

    typedef void(*FILEFUNC)(FILE *input, FILE *output, char *fname, char *dir,
        int drive);

    typedef struct
    {
        char id;
        BYTE mode;
        void(*routine)(char, char*);
    } ARGLIST;

    #ifdef __cplusplus
        extern "C"
        {
        #endif 
        #ifdef __cplusplus
        };
    #endif 

#endif /* _CMDLINE_H */
