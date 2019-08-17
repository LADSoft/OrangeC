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
 *memmgt.c
 *
 * memory management
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "utype.h"	
#include "cmdline.h"	
#include "lists.h"
#include "expr.h"
#include "ccerr.h"
#include "c.h"

void release_local(void);
void release_global(void);
void release_opt(void);
void release_oc(void);
void mem_summary(void);

#define BLOCK_SIZE 4096

int global_flag;
int glbsize = 0,  /* size left in current global block */
locsize = 0,  /* size left in current local block */
glbindx = 0,  /* global index */
locindx = 0; /* local index */

BLK *locblk = 0,  /* pointer to local block */
*glbblk = 0; /* pointer to global block */

static int optsize, optindx, ocsize, ocindx;
static BLK *optblk,  *ocblk;
static long maxopt, maxglb, maxloc, maxoc;
void memini(void)
{
    global_flag = 1;
    glbsize = 0;
    locsize = 0;
    glbindx = 0;
    locindx = 0;
    locblk = 0;
    glbblk = 0;
    optsize = 0;
    optblk = 0;
    optindx = 0;
    maxopt = maxglb = maxloc = 0;
}

//-------------------------------------------------------------------------

static char *palloc(int *sizepos, int size, int *indxpos, BLK **blk)
/*
 * main allocation routine
 */
{
    BLK *bp;
    char *rv;
    if (size &1)
     /* if odd size */
        size += 1;
     /* make it even */
    /* if anything left, try to allocate from it */
    if (*sizepos >= size)
    {
        rv = &((*blk)->m[ *indxpos]);
        *sizepos -= size;
        *indxpos += size;
        return rv;
    }
    else
    {
        long allocsize;
        /* else check for size > normal blcok size */
        if (size > BLOCK_SIZE)
        {
            /* this is going to fragment memory!!! I'd fix it except
             * the fragmentation is partially dependent on the calloc 
             * implementation 
             */
            allocsize = size - 1;
            *sizepos = 0;
        }
        else
        {
            /* as long as we stick to normal blocks, fragmentation
             * won't be an issue because as long as all blocks are the
             * same size calloc is guaranteed to find one if there are any
             */
            allocsize = BLOCK_SIZE - 1;
            *sizepos = BLOCK_SIZE - size;
        }
        /* allocate mem */
        bp = calloc(1, sizeof(BLK) + allocsize);
        if (bp == NULL)
        {
            release_global();
            release_local();
            release_opt();
            release_oc();
            mem_summary();
            fatal(" not enough memory.");
        }
        bp->blksize = allocsize + 1;
        /* link the block and return the base */
        bp->next =  *blk;
        *blk = bp;
        *indxpos = size;
        return (*blk)->m;
    }
}

//-------------------------------------------------------------------------

char *xalloc(int siz)
/*
 * user-level allocation.  Global symbols are never deallocated; local
 * symbols are deallocated all at once by deallocating the local symbol
 * blocks
 */
{
    if (global_flag)
        return palloc(&glbsize, siz, &glbindx, &glbblk);
    else
        return palloc(&locsize, siz, &locindx, &locblk);
}

//-------------------------------------------------------------------------

char *oalloc(int siz)
/*
 * allocation for optimizer temps
 */
{
    /*	return xalloc(siz); */
    return palloc(&optsize, siz, &optindx, &optblk);
}

//-------------------------------------------------------------------------

char *ocalloc(int siz)
/*
 * Allocation for binary code gen
 */
{
    return palloc(&ocsize, siz, &ocindx, &ocblk);
}

//-------------------------------------------------------------------------

static long release(int *sizepos, BLK **blk)
/*
 * msin memory free routine
 * frees all blocks from a list at once
 *
 * This memory management scheme reduces fragmentation, however
 * temps hang around for a while...
 *
 */
{
    BLK *bp1,  *bp2;
    long blkcnt = 0;
    bp1 =  *blk;
    while (bp1 != 0)
    {
        blkcnt += bp1->blksize;
        bp2 = bp1->next;
        free(bp1);
        bp1 = bp2;
    }
    *blk = 0;
    *sizepos = 0;
    return blkcnt;
}

//-------------------------------------------------------------------------

void release_local(void)
/*
 * release all local allocations
 */
{
    long temp = release(&locsize, &locblk);
    if (temp > maxloc)
        maxloc = temp;
}

//-------------------------------------------------------------------------

void release_global(void)
/*
 * release all global allocations
 */
{
    long temp = release(&glbsize, &glbblk);
    if (temp > maxglb)
        maxglb = temp;
}

//-------------------------------------------------------------------------

void release_opt(void)
/*
 * release optimizer temps
 */
{
    long temp = release(&optsize, &optblk);
    if (temp > maxopt)
        maxopt = temp;
}

//-------------------------------------------------------------------------

void release_oc(void)
/*
 * release all binary codegen allocations
 */
{
    long temp = release(&ocsize, &ocblk);
    if (temp > maxoc)
        maxoc = temp;
}

//-------------------------------------------------------------------------

void mem_summary(void)
{
    printf("Memory usage:\n");
    if (maxglb)
        printf("  Globals:        %ld\n", maxglb);
    if (maxloc)
        printf("  Local peak:     %ld\n", maxloc);
    if (maxopt)
        printf("  Optimizer peak: %ld\n", maxopt);
    if (maxoc)
        printf("  Binary code peak: %ld\n", maxopt);

}
