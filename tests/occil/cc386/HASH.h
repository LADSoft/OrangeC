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
#ifndef _HASH_H
    #define _HASH_H

    /* Local hash table functions.  Librarian uses a different set of functions */
    /* Size is a prime number */
    #define HASHSIZE 1021

    /* Rotations in C */
    #define ROTR(x,bits) (((x << (16 - bits)) | (x >> bits)) & 0xffff)
    #define ROTL(x,bits) (((x << bits) | (x >> (16 - bits))) & 0xffff)

    /* Hash table record definition, all entries in a hash table must be
     * structures with the first two elements as given because hash table
     * entries are sometimes handled generically */
    typedef struct _hashrec_
    {
        struct _hashrec_ *link; /* Link to next element in list */
        char *key; /* Full key */
    } HASHREC;


#endif _HASH_H
