/* sz_mod4.c   (c) Michael Schindler, 1998
*
* This file is a probability model for blocksorted files.
* It uses the rangecoder (rangecod.c) as entropy coder and some
* other modules (qsmodel, bitmodel) to maintain statistics information.
* You have to call initmodel() to initialize the model and after
* encoding the first run you need to call fixafterfirst.
* closeszmodel finishes output.
* you may use the rangecoder model->rc for other purposes in between,
* provided that you do the same at decoding.
*
* This model always encodes <submodel><symbol><repeatcount> triples,
* where submodel is one of 3 possible models to encode a symbol.
*
* submodel 0 is a cache of the last 31 symbols seen (with repetition
* allowed), with the most recent symbol excluded (except after
* initialisation). The probabilities for symbols in cache are
* derived from their nuber of occurance and the runlengths.
*
* submodel 1 is a move-to-front (MTF) containing the MTFSIZE most
* recent symbols no longer on submodel 0. Probabilities are based
* on MTF rank here.
*
* submodel 2 contains symbols neither in submodel 0 or submodel 1.
*
* Symbols are moved between models lazily; an update is done only
* when needed.
*
* The probabilities for the submodel are derived from the most recent
* history; each submodel has a frequency of:
* 1 + #occurences in last 31 symbols + 5*#occurances in last 6 symbols,
* giving a total count of 64.
*
* The runlength code is an array of 5 quasistatic models, each
* containing 7 symbols. those symbols mean:
*  symbol   # extra bits     runlength
*    0         0             1
*    1         0             2
*    2         0             3
*    3         0             4
*    4         2             5+extra (5..8)
*    5         3             9+extra (9..16)
*    6         5             extra>16 ? extra : extra+5 bit follow,
*                               these bits preceded by 1 give the length
*
* the 5 runlength models are used for:
* 0:new symbols;  1:rl=1;  2:rl=2,3;  3: rl=4-8;  4: rl=9+
* model 1 is also used if the symbol was not seen in the last 6 symbols.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>   /* exit() */
#include "sz_mod4.h"

#define RLSHIFT 10
#define MTFSHIFT 10

#define FULLFLAG (MOD.cache-1)
#define MTFFLAG (MOD.cache-2)

#ifdef MODELGLOBAL
sz_model mod;
#define dumpcache(m) M_dumpcache()
#define decodewhat(m) M_decodewhat()
#define finishupdate(m,a) M_finishupdate(a)
#define addtomtf(m,a) M_addtomtf(a)
#define encodeother(m,a) M_encodeother(a)
#define readrunlength(m) M_readrun()
#define activatenext(m,a) M_activatenext(a)
#define MOD mod
#else
#define MOD (*m)
#endif

#if 0
static void dumpcache(sz_model *m)
{ uint i, last, bug=0;
  int sum;
  if (MOD.cachetotf>7*MAXCACHESIZE)
  { fprintf(stderr, "bug: sum too large\n");
    bug = 1;
  }
  last = MOD.cachefirst&(CACHESIZE-1);
  sum = MOD.cachetotf;
  for(i=0; i<MOD.cachesize; i++)
    sum -= MOD.cache[(CACHESIZE+last-i)&(CACHESIZE-1)].sy_f;
  if (sum!=0)
    { fprintf(stderr, "bug: sum=%4d !!!\n",sum);
      bug = 1;
    }
  for(i=0; i<MOD.cachesize; i++)
    if(MOD.lastseen[MOD.cache[(CACHESIZE+last-i)&(CACHESIZE-1)].symbol]>CACHESIZE)
    { fprintf(stderr, "bug: symbol flagged as not in cache!!!\n");
      bug = 1;
    }
 
  if (!bug)
    return;
  fprintf(stderr,"size:%2d last:%2d totf:%4d\nsymbol",MOD.cachesize, last, MOD.cachetotf);
  for(i=0; i<MOD.cachesize; i++)
    fprintf(stderr,"%4d",MOD.cache[(CACHESIZE+last-i)&(CACHESIZE-1)].symbol);
  fprintf(stderr,"\nrunlen");
  for(i=0; i<MOD.cachesize; i++)
    fprintf(stderr,"%4d",MOD.cache[(CACHESIZE+last-i)&(CACHESIZE-1)].rl);
  fprintf(stderr,"\nsy_f  ");
  for(i=0; i<MOD.cachesize; i++)
    fprintf(stderr,"%4d",MOD.cache[(CACHESIZE+last-i)&(CACHESIZE-1)].sy_f);
  fprintf(stderr,"\nindex ");
  for(i=0; i<MOD.cachesize; i++)
    fprintf(stderr,"%4d",(CACHESIZE+last-i)&(CACHESIZE-1));
  fprintf(stderr,"\nl_seen");
  for(i=0; i<MOD.cachesize; i++)
    fprintf(stderr,"%4d",MOD.lastseen[MOD.cache[(CACHESIZE+last-i)&(CACHESIZE-1)].symbol]);
  fprintf(stderr,"\n");
}


static void dumpmtf(sz_model *m)
{   int n, i, bug=0, inmtf[256];
    for(i=0; i<256; i++)
    {   if(((m->full.f[i]&0x8000)==0) != (m->lastseen[i]==FULLFLAG))
        {   fprintf(stderr,"full bug: flag and inactive don't match for %d Flag:%d  f:%d\n",
            i, m->lastseen[i], m->full.f[i]);
            bug=1;
        }
        inmtf[i] = 0;
    }
            
    if (m->mtfsizeact > m->mtfsize)
    {   fprintf(stderr,"mtf bug: actsize (%d) larger than mtfsize (%d)\n",
            m->mtfsizeact, m->mtfsize);
        bug=1;
    }
    if (m->full.totalfreq < 0)
    {   fprintf(stderr,"full bug: total frequency (%d) wrong\n", m->full.totalfreq);
        bug=1;
    }
    i = m->mtffirst;
    for (n=0; n<m->mtfsizeact; n++)
    {   int sym=m->mtfhist[i].sym;
        if (i>MTFHISTSIZE)
        {   fprintf(stderr,"mtf bug: invalid next (%d)\n", n);
            bug=1;
        }
        if (m->lastseen[sym]<CACHESIZE)
        {   fprintf(stderr,"mtf bug: active mtf symbol (%d,%d) in cache\n", sym, n);
            bug=1;
        }
        if (m->lastseen[sym] == FULLFLAG) 
        {   fprintf(stderr,"mtf bug: active mtf symbol (%d,%d) in full\n", sym, n);
            bug=1;
        }
        if (inmtf[m->mtfhist[i].sym] != 0) 
        {   fprintf(stderr,"mtf bug: active mtf symbol (%d,%d) doubled\n", sym, n);
            bug=1;
        }
        inmtf[m->mtfhist[i].sym]++;

        i=m->mtfhist[i].next;
    }
    for (; n<m->mtfsize; n++)
    {   int sym=m->mtfhist[i].sym;
        if (i>MTFHISTSIZE)
        {   fprintf(stderr,"mtf bug: invalid next (%d)\n", n);
            bug=1;
        }
        i=m->mtfhist[i].next;
    }
    i=m->mtffirst;
    if (bug)
    {   fprintf(stderr,"mtfsize: %3d  active: %3d\n", m->mtfsize, m->mtfsizeact);
        for (n=0; n<m->mtfsize; n++)
        {   int sym=m->mtfhist[i].sym;
            fprintf(stderr,"n: %3d  i: %3d  sym: %3d  next:%3d  active: %1d\n",
                n, i, m->mtfhist[i].sym, m->mtfhist[i].next, n<m->mtfsizeact);
            i=m->mtfhist[i].next;
        }
        n=i;
    }

}


/* decode what model was used */
static Inline int decodewhat(sz_model *m)
{   int sy_f, lt_f, mod;
    mod = qsgetsym( &(MOD.what), decode_culshift( &(MOD.ac), WHATSHIFT));
    qsgetfreq( &(MOD.what), mod, &sy_f, &lt_f );
    decode_update_shift(&(MOD.ac), sy_f, lt_f, WHATSHIFT);
    qsupdate( &(MOD.what), mod);
    return mod;
}
#endif


/* add a new symbol to MTF list */
static void addtomtf(sz_model *m, uint sym)
{   uint i = (MOD.mtffirst+1) & (MTFHISTSIZE-1);
    if (MOD.mtfhist[i].next == 0xffff)      /* an empty place */
    {   MOD.mtfsize++;
        MOD.mtfsizeact++;
    }
    else if (MOD.mtfsizeact==MOD.mtfsize)   /* occupied by active symbol */
    {   MOD.lastseen[MOD.mtfhist[i].sym] = FULLFLAG;
        bitreactivate(&(MOD.full),MOD.mtfhist[i].sym);
    }
    else                                    /* occupied by inactive symbol*/
        MOD.mtfsizeact++;
    MOD.mtfhist[i].next = MOD.mtffirst;
    MOD.mtfhist[i].sym = sym;
    MOD.mtffirst = i;
    MOD.lastseen[sym] = MTFFLAG;
}


/* finish updating the model
* this assumes that the following has been done properly:
* probability updating of RLE models/MTF models/full model
* m.newest points to the free entry
* m.newest->sy_f , weight and what are properly filled
* the symbol has been disabled in the old place (removed from MTF,
* set inactive in full or sy_f cleared for cache
* this will do the following: adjust lastseen.
* adjust m.cachetotf
* adjust m.whatmod
* advance m.lastnew
* modify lastseen
* free the next available element in cache */
static void finishupdate(sz_model *m, uint symbol)
{   cacheptr tmp;
    tmp = MOD.newest; /* make tmp point to new element */
    MOD.lastseen[symbol] = tmp;
    tmp->symbol = symbol;
    MOD.cachetotf += tmp->weight;
    tmp = tmp->next; /* make tmp point to the to be cleared element */
    MOD.whatmod[tmp->what] --;
    if (!tmp->sy_f)
    	(MOD.lastseen[tmp->symbol])->sy_f --;
	else /* last instance, move to MTF */
        addtomtf(m,tmp->symbol);
    tmp = MOD.lastnew; /* make tmp point to adjustment place */
    MOD.whatmod[tmp->what] -= 5;
    MOD.cachetotf -= tmp->weight;
    MOD.lastseen[tmp->symbol]->sy_f -= tmp->weight-1;
    tmp->weight = 1;
    MOD.lastnew = tmp->next;
}
        

/* encode non-cache symbols */
static unsigned char encodeother(sz_model *m, uint sym)
{   uint i, n, last;
    i = MOD.mtffirst;
    last = i;
    if (MOD.mtfsizeact >= MTFSIZE) /* we have enough active symbols */
    {   for (n=0; n<MTFSIZE; n++)
        {   if (MOD.mtfhist[i].sym == sym)
                goto found;
            last = i;
            i = MOD.mtfhist[i].next;
        }
        /* we didn't find it, so move all remaining active symbols to inactive */
        for (; n<MOD.mtfsizeact; n++)
        {   MOD.lastseen[MOD.mtfhist[i].sym] = FULLFLAG;
            bitreactivate(&(MOD.full),MOD.mtfhist[i].sym);
            i = MOD.mtfhist[i].next;
        }
        MOD.mtfsizeact = MTFSIZE;
    }
    else
    {   for (n=0; n<MOD.mtfsizeact; n++)
        {   if (MOD.mtfhist[i].sym == sym)
                goto found;
            last = i;
            i = MOD.mtfhist[i].next;
        }
        /* we didn't find it, so try to make more active */
        MOD.mtfsizeact = MOD.mtfsize>MTFSIZE ? MTFSIZE : MOD.mtfsize;
        while (n<MOD.mtfsizeact)
        {   if (MOD.lastseen[MOD.mtfhist[i].sym]==FULLFLAG)
            {   MOD.lastseen[MOD.mtfhist[i].sym] = MTFFLAG;
                bitdeactivate(&(MOD.full),MOD.mtfhist[i].sym);
                if (MOD.mtfhist[i].sym == sym)
                {   MOD.mtfsizeact = n+1;
                    goto found;
                }
                last = i;
                i = MOD.mtfhist[i].next;
                n++;
            }
            else /* symbol in cache or active MTF; remove from list */
            {   int next = MOD.mtfhist[i].next;
                if(n>0) MOD.mtfhist[last].next = next;
                else MOD.mtffirst = next;
                MOD.mtfhist[i].next = 0xffff;
                i = next;
                MOD.mtfsize--;
                if (MOD.mtfsizeact > MOD.mtfsize)
                    MOD.mtfsizeact = MOD.mtfsize;
            }
        }
    }
    /* we didn't find it, so use full model */
    encode_shift(&(MOD.ac), MOD.whatmod[2], MOD.whatmod[0]+MOD.whatmod[1], 6);
    MOD.whatmod[2]+=6;
  { int sy_f, lt_f;
    bitgetfreq(&(MOD.full),sym,&sy_f, &lt_f);
    encode_freq(&(MOD.ac),sy_f,lt_f,bittotf(&(MOD.full)));
    bitupdate_ex(&(MOD.full),sym);
    return 2;
  }
found: /* we found it in MTF, so encode it and remove it */
    encode_shift(&(MOD.ac), MOD.whatmod[1], MOD.whatmod[0], 6);
    MOD.whatmod[1]+=6;
  { int sy_f, lt_f;
    qsgetfreq(&(MOD.mtfmod), n, &sy_f, &lt_f);
    encode_shift(&(MOD.ac), sy_f, lt_f, MTFSHIFT);
    qsupdate(&(MOD.mtfmod), n);
  }
    if (n==0)
        MOD.mtffirst = MOD.mtfhist[i].next;
    else
        MOD.mtfhist[last].next= MOD.mtfhist[i].next;
    MOD.mtfhist[i].next = 0xffff;
    MOD.mtfsize--;
    MOD.mtfsizeact--;
    return 1;
}

static unsigned char readrun(qsmodel *rlmod, uint4 *n)
{   int sy_f, lt_f, rl;
    rl = qsgetsym( rlmod, decode_culshift( &(MOD.ac), RLSHIFT));
    qsgetfreq( rlmod, rl, &sy_f, &lt_f );
    decode_update_shift(&(MOD.ac), sy_f, lt_f, RLSHIFT);
    qsupdate( rlmod, rl);
    if (rl<=3)   /* no extra bits */
    {   rl++;
        *n = rl;
        return (1 + (rl>>1));
    }
    if (rl==4)  /* two extra bits */
    {   rl = decode_culshift( &(MOD.ac), 2);
        decode_update_shift(&(MOD.ac), 1, rl, 2);
        *n = rl + 5;
        return 3;
    }
    if (rl==5)  /* three extra bits */
    {   rl = decode_culshift( &(MOD.ac), 3);
        decode_update_shift(&(MOD.ac), 1, rl, 3);
        *n = rl + 9;
        return 4;
    }
    /* five extra bits */
    rl = decode_culshift( &(MOD.ac), 5);
    decode_update_shift(&(MOD.ac), 1, rl, 5);
    if (rl>16)
        *n = rl;
    else
    {   uint4 bits;
        rl += 5;
        bits = decode_culshift( &(MOD.ac), rl);
        decode_update_shift(&(MOD.ac), 1, bits, rl);
        *n = bits + ((uint4)1 << rl);
    }
    return 4;
}


/* writes out the runlength */
static unsigned char writerun(qsmodel *rlmod, uint4 n)
{   int sy_f, lt_f;
	if (n<=4)       /* no extra bits */
    {   qsgetfreq( rlmod, n-1, &sy_f, &lt_f );
        encode_shift( &(MOD.ac), (freq)sy_f, (freq)lt_f, RLSHIFT);
        qsupdate( rlmod, n-1);
        return (1 + (n>>1));
    }
    if (n<=8)       /* two extra bits */
    {   qsgetfreq( rlmod, 4, &sy_f, &lt_f );
        encode_shift( &(MOD.ac), (freq)sy_f, (freq)lt_f, RLSHIFT);
        encode_shift( &(MOD.ac), (freq)1, (freq)(n-5), 2);
        qsupdate( rlmod, 4);
	    return 3;
    }
    if (n<=16)      /* three extra bits */
    {   qsgetfreq( rlmod, 5, &sy_f, &lt_f );
        encode_shift( &(MOD.ac), (freq)sy_f, (freq)lt_f, RLSHIFT);
        encode_shift( &(MOD.ac), (freq)1, (freq)(n-9), 3);
        qsupdate( rlmod, 5);
	    return 4;
    }
	qsgetfreq( rlmod, 6, &sy_f, &lt_f );
	encode_shift( &(MOD.ac), (freq)sy_f, (freq)lt_f, RLSHIFT);
	if (n < 32) /* five extra bits; n must be >16 here */
		encode_shift( &(MOD.ac), (freq)1, (freq)n, 5);
	else        /* #extra bits-5, 5 to 21 extra bits without leading 1 */
	{   uint i;
		for (i=5; n>>i > 1; i++)
            /* void */;
        encode_shift( &(MOD.ac), (freq)1, (freq)(i-5), 5);
	    encode_shift( &(MOD.ac), (freq)1, (freq)(n-((uint4)1<<i)), i);
	}
	qsupdate( rlmod, 6);
	return 4;
}

void sz_encode(sz_model *m, uint symbol, uint4 runlength)
{   cacheptr tmp;

    /* now encode what is the next symbol, first what model to use */
    /* then within the model */
    if ((tmp=MOD.lastseen[symbol]) >= MOD.cache) /* symbol in cache */
    {   uint lt_f;
        cacheptr old;
        encode_shift(&(MOD.ac), MOD.whatmod[0], 0, 6);
        MOD.whatmod[0]+=6;
        old = tmp;
        lt_f = 0;
        tmp = tmp->next;
        while (tmp != MOD.newest)
        {   lt_f += tmp->sy_f;
            tmp = tmp->next;
        }
        encode_freq(&(MOD.ac), old->sy_f, lt_f, MOD.cachetotf - tmp->sy_f);
        tmp = tmp->next;
        tmp->what = 0;
        tmp->weight = writerun(MOD.rlemod + old->weight, runlength);
        tmp->sy_f = tmp->weight + old->sy_f;
        old->sy_f = 0;
        MOD.newest = tmp;
    }
    else
    {   tmp = MOD.newest->next;
        tmp->what = encodeother(m,symbol);
        tmp->weight = writerun(MOD.rlemod, runlength);
        tmp->sy_f = tmp->weight;
        MOD.newest = tmp;
    }
    finishupdate(M,symbol);
}


static int activatenext(sz_model *m, uint *next)
{   while (MOD.mtfsize>MOD.mtfsizeact)
    {   mtfentry *tmp;
        tmp = MOD.mtfhist + *next;
        if (MOD.lastseen[tmp->sym]==FULLFLAG)
        {   bitdeactivate(&(MOD.full),tmp->sym);
            MOD.lastseen[tmp->sym] = MTFFLAG;
            MOD.mtfsizeact++;
            return 1;
        }
        *next = tmp->next;
        tmp->next = 0xffff;
        MOD.mtfsize--;
    }
    return 0;
}


void sz_decode(sz_model *m, uint *symbol, uint4 *runlength)
{   uint sym;

    /* first decode what model was used in encoding */
    sym = decode_culshift( &(MOD.ac), 6);
    if (sym < MOD.whatmod[0])  /* cache */
    {   uint lt_f, tot_f;
        cacheptr tmp;
        decode_update_shift(&(MOD.ac), MOD.whatmod[0], 0, 6);
        MOD.whatmod[0] += 6;
        tmp = MOD.newest;
        tot_f = MOD.cachetotf - tmp->sy_f;
        sym = decode_culfreq( &(MOD.ac), tot_f);
        tmp = tmp->prev;
        lt_f = tmp->sy_f;
        while (lt_f <= sym)
        {   tmp = tmp->prev;
            lt_f += tmp->sy_f;
        }
        decode_update(&(MOD.ac), tmp->sy_f, lt_f-tmp->sy_f, tot_f);
      { cacheptr free = MOD.newest->next;
        MOD.newest = free;
        free->what = 0;
        free->weight = readrun(MOD.rlemod + tmp->weight, runlength);
        free->sy_f = free->weight + tmp->sy_f;
      }
        tmp->sy_f = 0;
        *symbol = tmp->symbol;
    }
    else if (sym < MOD.whatmod[0]+MOD.whatmod[1])  /* MTF */
    {   mtfentry *pred;
        uint sy_f, lt_f;
        decode_update_shift(&(MOD.ac), MOD.whatmod[1], MOD.whatmod[0], 6);
        MOD.whatmod[1] += 6;
        sym = qsgetsym( &(MOD.mtfmod), decode_culshift( &(MOD.ac), MTFSHIFT));
        qsgetfreq( &(MOD.mtfmod), sym, &sy_f, &lt_f );
        decode_update_shift(&(MOD.ac), sy_f, lt_f, MTFSHIFT);
        qsupdate( &(MOD.mtfmod), sym);
        if (MOD.mtfsizeact == 0)
            activatenext(&MOD,&(MOD.mtffirst));

        pred = MOD.mtfhist + MOD.mtffirst;
        if (sym==0)    /* the first entry */
        {   if(MOD.mtfsizeact==0)
                activatenext(&MOD,&(MOD.mtffirst));
            pred = MOD.mtfhist + MOD.mtffirst;
            sym = pred->sym;
            MOD.mtffirst = pred->next;
            pred->next = 0xffff;
        }
        else
        {   uint n;
            mtfentry *target;
            if (sym < MOD.mtfsizeact) /* active list is large enough */
                for (n=sym-1; n; n--) /* skip unneeded part of MTF */
                    pred = MOD.mtfhist + pred->next;
            else
            {   for (n=MOD.mtfsizeact-1; n; n--) /* skip active part of MTF */
                    pred = MOD.mtfhist + pred->next;
                while (MOD.mtfsizeact<sym)
                {   activatenext(&MOD,&(pred->next));
                    pred = MOD.mtfhist + pred->next;
                }
                activatenext(&MOD,&(pred->next));
            }
            target = MOD.mtfhist + pred->next;
            sym = target->sym;
            pred->next = target->next;
            target->next = 0xffff;
        }
        MOD.mtfsizeact--;
        MOD.mtfsize--;
      { cacheptr free = MOD.newest->next;
        MOD.newest = free;
        free->what = 1;
        free->weight = readrun(MOD.rlemod, runlength);
        free->sy_f = free->weight;
      }
        *symbol = sym;
    }
    else /* full model */
    {   uint sy_f, lt_f;
        decode_update_shift(&(MOD.ac), MOD.whatmod[2], MOD.whatmod[0]+MOD.whatmod[1], 6);
        MOD.whatmod[2] += 6;
        /* first adjust the size of the MTF */
        if (MOD.mtfsizeact>MTFSIZE) /* active MTF too big */
        {   uint n, i;
            i = MOD.mtffirst;
            for (n=0; n<MTFSIZE; n++) /* skip active part of MTF */
                i = MOD.mtfhist[i].next;
            while (n<MOD.mtfsizeact)
            {   bitreactivate(&(MOD.full),MOD.mtfhist[i].sym);
                MOD.lastseen[MOD.mtfhist[i].sym] = FULLFLAG;
                i = MOD.mtfhist[i].next;
                n++;
            }
            MOD.mtfsizeact = MTFSIZE;
        }
        else if (MOD.mtfsizeact < MTFSIZE) /* active MTF too small */
        {   uint n;
            mtfentry *pred;
            if (MOD.mtfsizeact==0)
            {   activatenext(&MOD, &(MOD.mtffirst));
                pred = MOD.mtfhist + MOD.mtffirst;
            }
            else
            {   pred = MOD.mtfhist + MOD.mtffirst;
                for(n=MOD.mtfsizeact-1; n; n--)
                    pred = MOD.mtfhist + pred->next;
            }
            while (MOD.mtfsizeact<MTFSIZE && activatenext(&(MOD), &(pred->next)))
                pred = MOD.mtfhist + pred->next;
        }
        sym = bitgetsym( &(MOD.full), decode_culfreq( &(MOD.ac), bittotf(&(MOD.full))));
        bitgetfreq( &(MOD.full), sym, &sy_f, &lt_f );
        decode_update(&(MOD.ac), sy_f, lt_f, bittotf(&(MOD.full)));
        bitupdate_ex(&(MOD.full), sym);
      { cacheptr free = MOD.newest->next;
        MOD.newest = free;
        free->what = 2;
        free->weight = readrun(MOD.rlemod, runlength);
        free->sy_f = free->weight;
      }
        *symbol = sym;
    }
    finishupdate(m, *symbol);
};


/* initialisation if the model */
/* headersize -1 means decompression */
/* first is the first byte written by the arithcoder */
void initmodel(sz_model *m, int headersize, unsigned char *first)
{   int i;

    /* init the arithcoder */
    if((MOD.compress = (headersize>=0)))
        start_encoding(&(MOD.ac),*first,headersize);
    else
        *first = start_decoding(&(MOD.ac));

    /* init the full model */
    initbitmodel(&(MOD.full), ALPHABETSIZE, 40*ALPHABETSIZE, 10*ALPHABETSIZE, NULL);
    for(i=0; i<ALPHABETSIZE; i++)
        MOD.lastseen[i] = FULLFLAG;

    /* init the cache with symbols CACHESIZE-1 to 0 */
  { cacheptr tmp = MOD.cache;
    for(i=0; i<CACHESIZE-1; i++)
    {   tmp->next = tmp+1;
        tmp->prev = tmp-1;
        tmp->symbol = CACHESIZE - 2 - i;
        MOD.lastseen[tmp->symbol] = tmp;
        bitdeactivate(&(MOD.full),tmp->symbol);
	tmp->sy_f = 1;
        tmp->weight = 1;
        tmp->what = 0;
        tmp++;
    }
    MOD.cache[0].prev = MOD.cache + (CACHESIZE-1);
    tmp->next = MOD.cache;
    tmp->prev = tmp-1;
    tmp->sy_f = 0;
  }
    MOD.newest = MOD.cache + (CACHESIZE-2);
    MOD.lastnew = MOD.cache + (CACHESIZE-7);
    MOD.cachetotf = CACHESIZE; // for starup only, decremented by 1 later
    /* initialize the whatmodel */
    MOD.whatmod[0] = 41; // 1 + 22*1 + 3*6
    MOD.whatmod[1] = 8;  // 1 + 1*1 + 1*6
    MOD.whatmod[2] = 15; // 1 + 2*1 + 2*6
    /* make 2 old and 2 new full hits for what */
    for(i=0; i<2; i++)
    {   MOD.cache[i].what = 2;
        MOD.lastnew[i].what = 2;
    }
    /* make 1 old and 1 new hit for MTF */
    MOD.cache[2].what = 1;
    MOD.lastnew[2].what = 1;

    
    /* init the mtf models with symbols CACHESIZE .. (CACHESIZE+MTFSIZE<<1)*/
    MOD.mtfhist[0].next = MTFHISTSIZE-1;
    MOD.mtfhist[0].sym = CACHESIZE;
    for(i=1; i<MTFSIZE<<1; i++)
    {   MOD.mtfhist[i].next = i-1;
        MOD.mtfhist[i].sym = CACHESIZE+i;
    }
    for(; i<MTFHISTSIZE; i++)
        MOD.mtfhist[i].next = 0xffff;
    MOD.mtfsize = MTFSIZE<<1;
    MOD.mtfsizeact = 0;
    MOD.mtffirst = (MTFSIZE<<1) - 1;
    initqsmodel(&(MOD.mtfmod),MTFSIZE,MTFSHIFT,400,NULL,MOD.compress);

    /* init the runlengthmodels */
    for(i=0; i<5; i++)
        initqsmodel(MOD.rlemod+i,7,RLSHIFT,150,NULL,MOD.compress);
}


/* call fixafterfirst after encoding/decoding the first run */
void fixafterfirst(sz_model *m)
{   MOD.cachetotf--;
}


/* deletion of the model */
void deletemodel(sz_model *m)
{   int i;
    if (MOD.compress)
        MOD.ac.bytecount = done_encoding(&(MOD.ac));
    else
        done_decoding(&(MOD.ac));

//fprintf(stderr,"%d %d %d ",MOD.ac.bytecount,MAXCACHESIZE,MTFSIZE);
//for(i=0; i<MTFSIZE; i++) fprintf(stderr,"%d ",modelused[i]);

    /* delete the fullmodel */
    deletebitmodel(&(MOD.full));

    /* delete the mtfmodel */
    deleteqsmodel(&(MOD.mtfmod));

    /* init the runlengthmodels */
    for(i=0; i<5; i++)
        deleteqsmodel(MOD.rlemod+i);
}
