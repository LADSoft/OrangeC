/* sz_model4.h (c) Michael Schindler 1998 */
#ifndef SZ_MODEL4_H
#define SZ_MODEL4_H

#include "port.h"
#include "qsmodel.h"
#include "bitmodel.h"
#include "rangecod.h"

#define ALPHABETSIZE 256
#define CACHESIZE 32
#define MTFSIZE 20
#define MTFHISTSIZE 4096  /* must pe power of 2 */
#define MODELGLOBAL

typedef struct {
    uint sym, next;
} mtfentry;

typedef struct cacheS *cacheptr;

typedef struct cacheS {
    unsigned char symbol, sy_f, weight, what;
    cacheptr next, prev;
} cacheentry;

typedef struct {
    uint whatmod[3];  /* probabilities for the submodels */
    cacheptr newest,  /* points to newest element in cache */
             lastnew; /* points to last element with heigher weight */
    uint cachetotf;   /* total frequency count in cache */
    uint mtffirst;    /* where to find the newest entry in mtfhist */
    uint mtfsize;     /* size of mtflist */
    uint mtfsizeact;  /* size of active mtflist */
    cacheptr lastseen[ALPHABETSIZE]; /* tell if and where symbol is in cache */
    cacheentry cache[CACHESIZE]; /* cache */
    mtfentry mtfhist[MTFHISTSIZE];
    bitmodel full;    /* fallback model */
    qsmodel mtfmod;   /* probabilities for mtf ranks */
    qsmodel rlemod[5];
    rangecoder ac;
    uint compress;    /* 1 on compression, 0 on decompression */
} sz_model;

#ifdef MODELGLOBAL
#define initmodel(m,a,b) M_initmodel(a,b)
#define fixafterfirst(m) M_fixafterfirst()
#define deletemodel(m) M_deletemodel()
#define sz_finishrun(m) M_sz_finishrun()
#define sz_encode(m,a,b) M_sz_encode(a,b)
#define sz_decode(m,a,b) M_sz_decode(a,b)
#endif


/* initialisation if the model */
/* headersize -1 means decompression */
/* first is the first byte written by the arithcoder */
void initmodel(sz_model *m, int headersize, unsigned char *first);

/* call fixafterfirst after encoding/decoding the first run */
void fixafterfirst(sz_model *m);

/* deletion of the model */
void deletemodel(sz_model *m);

/* encode/decode a run of equal symbols */
void sz_encode(sz_model *m, uint symbol, uint4 runlength);
void sz_decode(sz_model *m, uint *symbol, uint4 *runlength);


#endif
