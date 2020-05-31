/* b_i_t.h                                                                   */
/* this implements bit indexed trees for culmulative frequency storeage      */
/* described in                                                              */
/* Peter Fenwick: A New Data Structure for Cumulative Probability Tables     */
/* Technical Report 88, Dep. of Computer Science, University of Auckland, NZ */

#ifndef b_i_t_h
#define b_i_t_h

#include "port.h"
#include "rangecod.h"
//typedef uint4 freq;
typedef uint4 symb;

typedef struct {
   symb size, mask;
   freq *cf, *f, totfreq;
} cumtbl;

/* returns the culmulative frequency < the given symbol */
Inline freq getcf(symb s, cumtbl *tbl);

/* updates the given frequency */
#define updatefreq(_s,_tbl,_delta)      \
 { int upd_delta = (_delta);            \
   symb upd_s = (_s);                   \
   (_tbl)->f[upd_s] += upd_delta;       \
   updatecumonly(upd_s,_tbl,upd_delta); }

/* updates the given culmulative frequency */
Inline void updatecumonly(symb s, cumtbl *tbl, int delta);

/* get symbol for this culmulative frequency */
Inline symb getsym(freq f, cumtbl *tbl);

/* scales the culmulative frequency tables by 0.5 and keeps nonzero values */
void scalefreq(cumtbl *tbl);

/* scales the culmulative frequency tables by 0.5 and keeps nonzero values */
void scalefreqcond(cumtbl *tbl, uint *donotuse);

/* allocates memory for the frequency table and initializes it */
int initfreq(cumtbl *tbl, symb tblsize, freq initvalue);

/* does the obvious thing */
void freefreq(cumtbl *tbl);

#endif
