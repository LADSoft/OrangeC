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
 * iopt.h
 *
 * icode optimization structures
 */
#define DUMP_GCSE_INFO

#define isset(array,bit) ((array)[(bit)/8] & bittab[(bit) & 7])
#define setbit(array,bit)   (array)[(bit)/8] |= bittab[(bit) & 7]
#define clearbit(array,bit)   (array)[(bit)/8] &= ~bittab[(bit) & 7]


/*
 * basic blocks are kept in this type of structure
 * and marked with an i_block inn the icode
 */
#define BLOCKLIST_VISITED 1

typedef struct _blocklist
{
    struct _blocklist *link;
    struct _block
    {
        short blocknum;
        short dfstnum;
        short flags;
        short callcount;
        struct _blocklist *flowfwd,  *flowback;
        BIGINT *p_in,  *p_out,  *p_gen,  *p_kill;
        BIGINT *p_ain,  *p_aout,  *p_agen,  *p_akill;
        BIGINT *p_cin,  *p_cout,  *p_cgen,  *p_ckill;
        BIGINT *p_def,  *p_use,  *p_live_in,  *p_live_out;
        BIGINT *p_pin,  *p_pout;
        QUAD *head,  *tail;
    }  *block;
}

//-------------------------------------------------------------------------

BLOCKLIST;

typedef struct _block BLOCK;
/*
 * common code elimination uses this to track
 * all the gotos branching to a given label
 */
typedef struct _comgo
{
    char size;
    QUAD *head,  *tail;
} COMGOREC;
/*
 * DAG structures
 */

typedef struct _daglist
{
    struct _daglist *link;
    BYTE *key;
    BYTE *rv;
} DAGLIST;

typedef struct _list2
{
    struct _list2 *link;
    int id;
    struct _l2data
    {
        IMODE *ans;
        QUAD *val;
    } data;
}

//-------------------------------------------------------------------------

LIST2;

typedef struct _list3
{
    struct _list3 *link;
    IMODE *ans;
    LIST *decllist;
} LIST3;

