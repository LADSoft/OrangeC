/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROTR(x,bits) (((x << (16 - bits)) | (x >> bits)) & 0xffff)

#define HASH_SIZE 4091

#pragma rundown show_profile 100
#pragma startup init_profile 254
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