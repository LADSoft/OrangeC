/*
  bitmodel.c     bit indexed trees probability model

  (c) Michael Schindler
  1997, 1998
  http://www.compressconsult.com or http://eiunix.tuwien.ac.at/~michael
  michael@compressconsult.com       michael@eiunix.tuwien.ac.at

  based on: Peter Fenwick: A New Data Structure for Cumulative Probability Tables
  Technical Report 88, Dep. of Computer Science, University of Auckland, NZ

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.  It may be that this
  program violates local patents in your country, however it is
  belived (NO WARRANTY!) to be patent-free here in Austria and I am
  not aware of a violation elsewhere.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA 02111-1307, USA.

  Bitmodel implements bit indexed trees for frequency storage described
  by Peter Fenwick: A New Data Structure for Cumulative Probability Tables
  Technical Report 88, Dep. of Computer Science, University of Auckland, NZ.
  It features a fast method for cumulative frequency storage and updating.
  The difference to the fenwick paper is the way the table is recalculated
  after rescaling; the method here is faster.

  There is a compiletime switch; if EXCLUDEONUPDATE is defined symbols
  are excluded on update; to be able to use them again you have to call
  the include function for that symbol.

  The module provides functions for creation, reset, deletion, query for
  probabilities, queries for symbols, reenabling symbols and model updating.
*/

#include "bitmodel.h"
#include <stdio.h>     /* NULL */
#include <stdlib.h>    /* malloc, free */


/* constructs the b_i_t structire */
static Inline void build_cf(bitmodel *m)
{   int i;
    uint2 *cf;
    cf = m->cf;
    m->totalfreq = 0;
    for (i=1; i<=m->n; i<<=1)
    {   int j;
        for (j=i; j<=m->n; j+= i<<1)
        {   int k;
#ifdef EXCLUDEONUPDATE
            if (m->f[j-1] & 0x8000)
                cf[j] = 0;
            else
#endif
                m->totalfreq += cf[j] = m->f[j-1];
            for (k=i>>1; k; k>>=1)
                cf[j] += cf[j-k];
        } /* end for j */
    } /* end for i */
}


/* scales the culmulative frequency tables by 0.5 and keeps nonzero values */
static void scalefreq(bitmodel *m)
{   uint2 *f, *endf;
    for (f=m->f, endf = f+m->n; f<endf; f++)
#ifdef EXCLUDEONUPDATE
        *f = ((1+(*f & 0x7fff))>>1) | (*f & 0x8000);
#else
        *f = (1 + *f)>>1;
#endif
    build_cf(m);
}


/* initialisation of bitmodel                          */
/* m   bitmodel to be initialized                      */
/* n   number of symbols in that model                 */
/* max_totf  maximum allowed total frequency count     */
/* rescale  desired rescaling interval, must be <max_totf/2 */
/* init  array of int's to be used for initialisation (NULL ok) */
void initbitmodel( bitmodel *m, int n, int max_totf, int rescale,
    int *init )
{   m->n = n;
    if (max_totf < n<<1) max_totf = n<<1;
    m->max_totf = max_totf;
    m->incr = max_totf/2/rescale;
    if (m->incr < 1) m->incr = 1;
    m->f = (uint2*) malloc(n*sizeof(uint2));
    m->cf = (uint2*) malloc((n+1)*sizeof(uint2));
    m->mask = 1;
    while (n>>=1)
        m->mask <<=1;
    resetbitmodel(m,init);
}



/* reinitialisation of bitmodel                        */
/* m   bitmodel to be initialized                      */
/* init  array of int's to be used for initialisation (NULL ok) */
void resetbitmodel( bitmodel *m, int *init)
{   int i;
    if (init == NULL)
    {   for(i=0; i<m->n; i++)
            m->f[i] = 1;
        m->totalfreq = m->n;
    } else
    {   m->totalfreq = 0;
        for(i=0; i<m->n; i++)
        {   m->f[i] = init[i];
            m->totalfreq += init[i];
        }
    }
    while (m->totalfreq > m->max_totf)
        scalefreq(m);
    build_cf(m);
}


/* deletion of bitmodel m                              */
void deletebitmodel( bitmodel *m )
{   free(m->f);
    free(m->cf);
    m->n = 0;
}


/* retrieval of estimated frequencies for a symbol     */
/* m   bitmodel to be questioned                       */
/* sym  symbol for which data is desired; must be <n   */
/* sy_f frequency of that symbol                       */
/* lt_f frequency of all smaller symbols together      */
/* the total frequency can be obtained with bit_totf   */
void bitgetfreq( bitmodel *m, int sym, int *sy_f, int *lt_f)
{   int cul;
    uint2 *cf;
    *sy_f = m->f[sym];
    sym++;
    cf = m->cf;
    cul = cf[sym];
    while (sym &= sym-1)
        cul += cf[sym];
    *lt_f = cul - *sy_f;
}


/* find out symbol for a given cumulative frequency    */
/* m   bitmodel to be questioned                       */
/* lt_f  cumulative frequency                          */
int bitgetsym( bitmodel *m, int lt_f )
{   int sym, mask, n;
    uint2 *cf;
    mask = m->mask;
    n = m->n;
    cf = m->cf;
    sym = 0;
    do
    {   int x;
        if ((x=sym|mask) <= n && lt_f >= cf[x])
        {   lt_f -= cf[x];
            sym = x;
        }
    } while (mask >>= 1);
    return sym;
}


/* update the cumulative frequency data by delta */
static Inline void bit_cfupd( bitmodel *m, int sym, int delta )
{   m->totalfreq += delta;
    if (m->totalfreq > m->max_totf)
        scalefreq(m);
    else
    {   uint2 *cf;
        sym++;
        cf = m->cf;
        while (sym<= m->n)
        {   cf[sym] += delta;
            sym = (sym | (sym-1)) + 1;
        }
    }
}


/* update model                                        */
/* m   bitmodel to be updated                          */
/* sym  symbol that occurred (must be <n from init)    */
void bitupdate( bitmodel *m, int sym )
{   m->f[sym] += m->incr;
    bit_cfupd(m, sym, m->incr);
}


#ifdef EXCLUDEONUPDATE
/* update model and exclude symbol                     */
/* m   bitmodel to be updated                          */
/* sym  symbol that occurred (must be <n from init)    */
void bitupdate_ex( bitmodel *m, int sym )
{   int delta;
    delta = -m->f[sym];
    m->f[sym] = (m->f[sym] + m->incr) | 0x8000;
    bit_cfupd(m, sym, delta);
}


/* deactivate symbol                                   */
/* m   bitmodel to be updated                          */
/* sym  symbol to be reactivated                       */
void bitdeactivate( bitmodel *m, int sym )
{   bit_cfupd(m, sym, -m->f[sym]);
    m->f[sym] |= 0x8000;
}


/* reactivate symbol                                   */
/* m   bitmodel to be updated                          */
/* sym  symbol to be reactivated                       */
void bitreactivate( bitmodel *m, int sym )
{   m->f[sym] &= 0x7fff;
    bit_cfupd(m, sym, m->f[sym]);
}
#endif
