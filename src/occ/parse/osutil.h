/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef OSUTIL_H
#define OSUTIL_H

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

    typedef struct
    {
        char id;
        UBYTE mode;
        void(*routine)(char, char*);
    } CMDLIST;

typedef struct _list_
{
    struct _list_ *next;
    void *data;
} LIST;

typedef struct _memblk_
{
    struct _memblk_ *next;
    long size;
    long left;
    char m[1]; /* memory area */
} MEMBLK;

/* Global HASHRECbol table is a hash table */
#define GLOBALHASHSIZE 8192 /*9973 */
#define LOCALHASHSIZE 29

typedef struct _hashrec_
{
    struct _hashrec_ *next; /* next to next element in list */
    struct _hrintern_{
        char *name;
    } *p;
} HASHREC;

typedef struct _hashtable_
{
    struct _hashtable_ *next, *chain;
    int size;
    int blockLevel;
    int blocknum; /* debugger block number */
    HASHREC **table;
    
} HASHTABLE;

#ifdef __cplusplus
extern "C" {
#endif
extern char *getUsageText(void);
#ifdef __cplusplus
}
#endif
#endif
