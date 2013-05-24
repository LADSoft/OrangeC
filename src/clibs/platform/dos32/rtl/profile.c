/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROTR(x,bits) (((x << (16 - bits)) | (x >> bits)) & 0xffff)

#define HASH_SIZE 4091

#pragma rundown show_profile 100
#pragma startup init_profile 100
extern char **_argv;

unsigned _elapsed_time(void);

typedef struct _profile {
    struct _profile *hashlink;
    struct _profile *link;
    char *name;
   unsigned int time;
   unsigned int starttime;
    unsigned count;
} PROFILE;
    
#define PROFILESTR "$$$PROFILER"
#define PROFILESTR1 "$$$PROFILER1"

static PROFILE *proclink;
static int profiling = 0;
static int profileincomplete = 0;
static int profnames = 0;
static PROFILE *hashtab[HASH_SIZE];
static unsigned fnoverhead;

void _profile_in(char *name);
void _profile_out(char *name);
static PROFILE *HashIt(char *str);
static void validate(void);

static void init_profile(void)
{
    profiling = 1;
    validate();
}

static int comparison(const void *a, const void *b)
{
    PROFILE *x=*((PROFILE **)a),*y=*((PROFILE **)b);
    unsigned r,s;
    r = x->time;
    s = y->time;
    if (r & 0xf0000000)
        r = 0;
    if (s & 0xf0000000)
        s = 0;
    if (r < s)
        return 1;
    if (r == s)
        return 0;
    return -1;
}
static void validate(void)
{
    unsigned v1,v2,v3;
    PROFILE *t,*t1;
    _profile_in(PROFILESTR);
    _profile_out(PROFILESTR);
    t = HashIt(PROFILESTR);
    fnoverhead = t->time;
}
static void show_profile(void)
{
    FILE *out;
    PROFILE **p;
    int i,pos=0;
    char name[256],*s;
    unsigned totaltime = 0,totalcount=0;
    profiling = 0;

    if (profileincomplete) {
        printf(stderr,"Not enough memory to perform profiling");
        return;
    }
    p = malloc(profnames * sizeof(PROFILE *));
    if (!p) {
        printf(stderr,"Can't allocate profile sort list");
        return;
    }
   strcpy(name,_argv[0]);
    s = strrchr(name,'.');
    if (!s)
        s = name + strlen(name);
    strcpy(s,".prf");
    out = fopen(name,"w");
    if (!out) {
        printf(stderr,"Can't open profile output file");
        return;
    }
    for (i=0; i < HASH_SIZE; i++) {
        PROFILE *q = hashtab[i];
        while (q) {
            totaltime += q->time;
            totalcount +=q->count;
            p[pos++] = q;
            q = q->hashlink;
        }
    }
    qsort(p,profnames,sizeof(PROFILE *),comparison);
    fprintf(out,"Profile info: (time: %10d, calls: %10d)\n\n",totaltime,totalcount);
    for (i=0; i < profnames; i++) {
        unsigned v = p[i]->time;
        
        fprintf(out,"%32s: %03d%% (time:%10d) (count:%10d) (avg:%10d)\n",p[i]->name,totaltime ? v*100/totaltime: 0, v,p[i]->count,v/p[i]->count);
    }
    fclose(out);
    
}
static unsigned int ComputeHash(char *string)
{
  unsigned int len = strlen(string), rv;
  char *pe = len + string;
  unsigned char blank = ' ';

  rv = len | blank;
  while(len--) {
    unsigned char cback = (unsigned char)(*--pe) | blank;
    rv = ROTR(rv,2) ^ cback;
  }
  return(rv % HASH_SIZE);
}
static PROFILE *HashIt(char *str)
{
    int h = ComputeHash(str);
    PROFILE *p = hashtab[h];
    while (p) {
        if (!strcmp(p->name,str))
            return p;
        p = p->hashlink;
    }
    p = malloc(sizeof(PROFILE));
    if (!p)
        return p;
    profnames++;
    p->name = str;
    p->time = 0;
    p->count = 0;
    p->hashlink = hashtab[h];
    p->link = 0;
    hashtab[h] = p;
    return p;
}
void _profile_in(char *name)
{
    int t;
    PROFILE *p;
    if (!profiling)
        return;
    if (proclink) {
        t = _elapsed_time() - proclink->starttime;
        if (t < 0) t = - t;
        proclink->time += t - fnoverhead;
    }

    profiling = 0;
    p = HashIt(name);
    if (!p) {
        profileincomplete = 1;
        return;
    }
    profiling = 1;
    p->link = proclink;
    proclink = p;
    p->count++;
    p->starttime = _elapsed_time();
    
}
void _profile_out(char *name)
{
    int t;
    if (!profiling || !proclink)
        return;
    t = _elapsed_time() - proclink->starttime;
    if (t < 0) t = - t;
    proclink->time += t - fnoverhead;
    proclink = proclink->link;
    if (proclink)
        proclink->starttime = _elapsed_time();
}