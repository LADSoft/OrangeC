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
#ifndef _MEM_H_
    #define _MEM_H_
    /*
     * EMS mem control type
     */
    #define MAX_EMS_READWRITE 16384

    typedef struct
    {
        BYTE *address;
        long size;
    } EMSMEM;

    /*
     * Linked list type.  Used for filenames, module number list, segment lists
     */
    typedef struct _list_
    {
        struct _list_ *link; /* Link to next element */
        void *data; /* Generic data */
    } LIST;

    /* Hash table record definition, all entries in a hash table must be
     * structures with the first two elements as given because hash table
     * entries are sometimes handled generically */

    typedef struct _hashrec_
    {
        struct _hashrec_ *link; /* Link to next element in list */
        char *key; /* Full key */
    } HASHREC;


#endif /* _MEM_H_ */
