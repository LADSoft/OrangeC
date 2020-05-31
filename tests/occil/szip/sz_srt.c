//#define CHECKINDIRECT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "port.h"
#include "sz_err.h"
#include "sz_srt.h"

#if defined SZ_UNSRT_O4
#include "sz_hash2.h"		// only used in sz_unsrt_o4
#endif

// the sorting is a little slow due to attempts to reuse memory as soon as possible.
// since the n-1 order sorted block is read sequentially a block can be freed (inserted
// in a freelist) as soon as it is processed. Since the new n-order sorted pointers
// grow as 256 different lists there is no need to have all memory available at once;
// new memory is needed at the same speed old is freed.

#define BITSSAMEBLOCK 10
#define BLOCKSIZE (1<<BITSSAMEBLOCK)
#define BLOCKMASK (BLOCKSIZE-1)

typedef struct p_block ptrblock;

struct p_block{
	uint2 msbytes[BLOCKSIZE];
	unsigned char lsbyte[BLOCKSIZE];
	ptrblock *nextfree;
};


typedef struct {
	ptrblock **index;		// index to blocks used in current sort
	ptrblock **oldindex;	// spare mem for alternate index
	ptrblock *freelist;
	ptrblock *block;
	ptrblock *spare[18];
	uint4 nrblocks;
} ptrstruct;


static ptrstruct globalptr;
static int globalinit = 0;

static void allocptrs(uint4 length, ptrstruct *p)
{	uint4 i;
	p->nrblocks = (length+BLOCKSIZE-1)/BLOCKSIZE;
	if (globalinit && (p->nrblocks>globalptr.nrblocks)) {
		free(globalptr.index);
		free(globalptr.oldindex);
		free(globalptr.block);
	    globalinit = 0;
	}
	if (!globalinit) {
		globalptr.nrblocks = p->nrblocks;
		globalptr.index = (ptrblock**) malloc(sizeof(ptrblock*)*globalptr.nrblocks);
		if (globalptr.index == NULL)
			sz_error(SZ_NOMEM_SORT);
		globalptr.oldindex = (ptrblock**) malloc(sizeof(ptrblock*)*globalptr.nrblocks);
		if (globalptr.oldindex == NULL)
			sz_error(SZ_NOMEM_SORT);
		globalptr.block = (ptrblock*) malloc(sizeof(ptrblock)*globalptr.nrblocks);
		if (globalptr.block == NULL)
			sz_error(SZ_NOMEM_SORT);
	    globalinit = 1;
	}
	p->index = globalptr.index;
	p->oldindex = globalptr.oldindex;
	p->block = globalptr.block;
	p->freelist = NULL;
	for(i=0; i<18; i++)
		p->spare[i] = NULL;
	for (i=0; i<p->nrblocks; i++)
		p->index[i] = p->block + i;
}

static void extraspare(ptrstruct *p, int blocks)
{	int i;
	for (i=0; p->spare[i]!= NULL; i++)
		/* void */;
	p->spare[i] = (ptrblock*) malloc(sizeof(ptrblock)*blocks);
	if (p->spare[i] == NULL)
		sz_error(SZ_NOMEM_SORT);
	p->spare[i]->nextfree = p->freelist;
	p->freelist = p->spare[i];
	for(i=1; i<blocks; i++)
		p->freelist[i-1].nextfree = p->freelist + i;
	p->freelist[blocks-1].nextfree = NULL;
}

static void allocspareptrs(uint4 length, ptrstruct *p)
{	length = (length>>BITSSAMEBLOCK) + 1;
	if (length>256) length = 256;
	extraspare(p,length);
}

static void freeptrs(ptrstruct *p)
{	int i;
//	free(p->index);
//	free(p->oldindex);
//	free(p->block);
	for (i=0; p->spare[i] != NULL; i++)
		free(p->spare[i]);
}

static Inline void setptr(ptrstruct *p, uint4 i, uint4 ptr)
{	ptrblock *tmp;
	tmp = p->index[i>>BITSSAMEBLOCK];
	if (tmp==NULL)
	{	if (p->freelist == NULL)
			extraspare(p,16);
		tmp = p->index[i>>BITSSAMEBLOCK] = p->freelist;
		p->freelist = p->freelist->nextfree;
	}
	i &= BLOCKMASK;
	tmp->msbytes[i] = ptr>>8;
	tmp->lsbyte[i] = ptr & 0xff;
}

static void sortorder2(ptrstruct *p, unsigned char *in, uint4 length,
					   uint4 *counts, unsigned int offset, uint4 *indexlast)
{	uint4 i, *o2counts, sum;
	unsigned int context;
	memset(counts, 0, 256*sizeof(uint4));
	o2counts = (uint4*) calloc(0x10000, sizeof(uint4));
	if (o2counts == NULL)
		sz_error(SZ_NOMEM_SORT);
	context = (unsigned)in[length-1]<<8;
	for(i=0; i<length; i++)
	{	context = context>>8 | (unsigned)(in[i])<<8;
		counts[in[i]]++;
		o2counts[context]++;
	}
	sum = length;
	for (i=0x10000; i--; )
	{	sum -= o2counts[i];
		o2counts[i] = sum;
	}
	sum = length;
	for (i=0x100; i--; )
	{	sum -= counts[i];
		counts[i] = sum;
	}
	context = (unsigned)in[length-offset]<<8 | in[length-offset-1];
	if (context == 0xffff)
		*indexlast = length-1;
	else
		*indexlast = o2counts[context+1]-1;
	offset--;
	for(i=0; i<offset; i++)
	{	in[i+length] = in[i];
		context = context>>8 | (unsigned int)(in[i+length-offset])<<8;
		setptr(p,o2counts[context],i+length);
		o2counts[context]++;
	}
	for(i=offset; i<length; i++)
	{	context = context>>8 | (unsigned int)(in[i-offset])<<8;
		setptr(p,o2counts[context],i);
		o2counts[context]++;
	}
	free(o2counts);
}

static void incsortorder(ptrstruct *p, unsigned char *in, uint4 length,
						 uint4 *counts, int offset, uint4 *indexlast)
{	uint4 i, block, ct[256];
	ptrblock *curblock;
	unsigned char ch=0;
	{ptrblock **swap=p->index; p->index = p->oldindex; p->oldindex = swap;}
	memset(p->index,0,p->nrblocks*sizeof(ptrblock*));
	memcpy(ct,counts,256*sizeof(uint4));
	block = 0;
	curblock = p->oldindex[block];
	for (i=0; i<=*indexlast; i++)
	{	unsigned index = i & BLOCKMASK;
		uint4 tmp = (uint4)(curblock->msbytes[index])<<8 | curblock->lsbyte[index];
		ch = in[tmp-offset];
		setptr(p,ct[ch],tmp);
		ct[ch]++;
		if (index==BLOCKMASK && block!=p->nrblocks-1)		//last ptr in block
		{	curblock->nextfree = p->freelist;
			p->freelist = curblock;
			block++;
			curblock = p->oldindex[block];
		}
	}
	*indexlast = ct[ch]-1;
	for ( ; i<length; i++)
	{	unsigned index = i & BLOCKMASK;
		uint4 tmp = (uint4)(curblock->msbytes[index])<<8 | curblock->lsbyte[index];
		ch = in[tmp-offset];
		setptr(p,ct[ch],tmp);
		ct[ch]++;
		if (index==BLOCKMASK && block<p->nrblocks-1)		//last ptr in block
		{	curblock->nextfree = p->freelist;
			p->freelist = curblock;
			block++;
			curblock = p->oldindex[block];
		}
	}
	curblock->nextfree = p->freelist;
	p->freelist = curblock;
}


static void finishsort(ptrstruct *p, unsigned char *in, uint4 length,
						 uint4 *counts, uint4 *indexlast)
{	uint4 i, block, ct[256];
	ptrblock *curblock;
	unsigned char ch=0;
	{ptrblock **swap=p->index; p->index = p->oldindex; p->oldindex = swap;}
	memset(p->index,0,p->nrblocks*sizeof(ptrblock*));
	memcpy(ct,counts,256*sizeof(uint4));
	block = 0;
	curblock = p->oldindex[block];
	for (i=0; i<=*indexlast; i++)
	{	unsigned index = i & BLOCKMASK;
		uint4 tmp = (uint4)(curblock->msbytes[index])<<8 | curblock->lsbyte[index];
		ch = in[tmp-1];
		setptr(p,ct[ch],in[tmp]);
		ct[ch]++;
		if (index==BLOCKMASK && block!=p->nrblocks-1)		//last ptr in block
		{	curblock->nextfree = p->freelist;
			p->freelist = curblock;
			block++;
			curblock = p->oldindex[block];
		}
	}
	*indexlast = ct[ch]-1;
	for ( ; i<length; i++)
	{	unsigned index = i & BLOCKMASK;
		uint4 tmp = (uint4)(curblock->msbytes[index])<<8 | curblock->lsbyte[index];
		ch = in[tmp-1];
		setptr(p,ct[ch],in[tmp]);
		ct[ch]++;
		if (index==BLOCKMASK && block!=p->nrblocks-1)		//last ptr in block
		{	curblock->nextfree = p->freelist;
			p->freelist = curblock;
			block++;
			curblock = p->oldindex[block];
		}
	}
	curblock->nextfree = p->freelist;
	p->freelist = curblock;
	for (i=0; i<p->nrblocks-1; i++)
		memcpy(in+i*BLOCKSIZE, p->index[i]->lsbyte, BLOCKSIZE);
	i= p->nrblocks - 1;
	memcpy(in+BLOCKSIZE*i, p->index[i]->lsbyte, length-i*BLOCKSIZE);
}


// inout: bytes to be sorted; sorted bytes on return. must be length+order bytes long
// length: number of bytes in inout
// *indexlast: returns position of last context (needed for unsort)
// order: order of context used in sorting (must be >=3)
// the code assumes length>=order
// and inout is length+order bytes long (only the first length need to be filled)
void sz_srt(unsigned char *inout, uint4 length, uint4 *indexlast, unsigned int order)
{	uint4 i;
	ptrstruct p;
	uint4 counts[256];
	allocptrs(length, &p);
	sortorder2(&p, inout, length, counts, order, indexlast);
	allocspareptrs(length, &p);
	for (i=order-2; i>1; i--)
		incsortorder(&p, inout, length, counts, i, indexlast);
	finishsort(&p, inout, length, counts, indexlast);
	freeptrs(&p);
}


#define INDIRECT 0x800000

#define setbit(flags,bit) (flags[bit>>3] |= 1<<(bit & 7))
#define getbit(flags,bit) ((flags[bit>>3]>>(bit&7)) & 1)

static void makeorder2(unsigned char *flags, unsigned char *in, uint4 *counts,
					   uint4 length)
{	uint4 i, j, ct[256];
	memcpy(ct, counts, 256*sizeof(uint4));
	// set bits in flag1 at start of each order 2 context
	// for order 2 this is more efficient than the method used for higher orders
	for(i=0; i<256; i++)
	setbit(flags,ct[i]);
	j = 0;
	for (i=0; i<255; i++)
	{	uint4 k;
		for(k=counts[i+1]; j<k; j++)
			ct[in[j]]++;
		for(k=0; k<256; k++)
			setbit(flags,ct[k]);
	}
}


static void increaseorder(unsigned char *inflags, unsigned char *outflags,
						  unsigned char *in, uint4 *counts, uint4 length)
{	uint4 i, contextstart, lastseen[256], ct[256];
	memcpy(ct, counts, 256*sizeof(uint4));
	contextstart = 0;
	memset(lastseen, 0xff, 256*sizeof(uint4)); 
	for (i=0; i<length; i++)
	{	int ch;
		if (getbit(inflags, i))			// ch = -1 + getbit(inflags,i);
			contextstart = i;			// contextstart = (contextstart&ch) | (i&~ch);
		ch = in[i];
		if (lastseen[ch] != contextstart)	// use a bitfield instead of lastseen
		{	lastseen[ch] = contextstart;	// in hardware!
			setbit(outflags, ct[ch]);
		}
		ct[ch]++;
	}
}


static void maketable(unsigned char *inflags, uint4 *table, unsigned char *in,
					  uint4 *counts, uint4 length)
{	uint4 i, contextstart, firstseen[256], ct[256];
	memcpy(ct, counts, 256*sizeof(uint4));
	contextstart = 0;
	memset(firstseen, 0, 256*sizeof(uint4)); 
	for (i=0; i<length; i++)
	{	int ch;
		if (getbit(inflags, i))
			contextstart = i;
		ch = in[i];
		if (firstseen[ch] <= contextstart)
		{	table[i] = ct[ch];
			firstseen[ch] = i+1;
		}
		else
			table[i] = (firstseen[ch]-1) | INDIRECT;
		ct[ch]++;
	}
}

// in: bytes to be unsorted
// out: unsorted bytes; if out==NULL output is written to stdout
// length: number of bytes in in (and out)
// indexlast: position of last context (as returned bt sorttrans)
// counts: number of occurances of each byte in in (if NULL it will be calculated)
// order: order of context used in sorting (must be >=3)
// the code assumes length>=order
void sz_unsrt(unsigned char *in, unsigned char *out, uint4 length, uint4 indexlast,
			uint4 *counts, unsigned int order)
{	uint4 i, j;
    static uint4 *table;
	static unsigned char *flags1=NULL;
	static unsigned char *flags2=NULL;
	unsigned char nocounts;

	// get counts if not supplied
	nocounts = counts==NULL;
	if (nocounts)
	{	counts = (uint4*) calloc(256, sizeof(uint4));
		for (i=0; i<length; i++)
			counts[in[i]]++;
	}
	// sum counts
	j = length;
	for (i=256; i--; )
	{	j -= counts[i];
		counts[i] = j;
	}

	if (flags1==NULL){
		flags1 = (unsigned char*) calloc((length+8)>>3,1);
		if (flags1 == NULL)
			sz_error(SZ_NOMEM_SORT);
	} else 
		memset(flags1,0,(length+8)>>3);

	makeorder2(flags1, in, counts, length);
	
	// now incease the order to desired order-1
	if (flags2==NULL){
		flags2 = (unsigned char*) calloc((length+8)>>3,1);
		if (flags2 == NULL)
			sz_error(SZ_NOMEM_SORT);
	} else 
		memset(flags2,0,(length+8)>>3);
	for (i=2; i<order-1; i++)
	{	unsigned char *tmpflags;
		increaseorder(flags1, flags2, in, counts, length);
		tmpflags = flags1;
		flags1 = flags2;		// flags1 now contains the updated beginflags
		flags2 = tmpflags;		// no need to clear, the set bits will be set again
	}
//	free(flags2);

	// construct permutation table
	if (table==NULL) {
		table = (uint4*)malloc((length+1)*sizeof(uint4));
		if (table == NULL)
			sz_error(SZ_NOMEM_SORT);
	}
	maketable(flags1, table, in, counts, length);
	table[length] = INDIRECT;
//	free(flags1);
	if (nocounts)
		free(counts);

	// do the actual unsorting
	j = indexlast;
	if (out == NULL)
		for (i=0; i<length; i++)
		{	uint4 tmp = table[j];
			if (tmp & INDIRECT)
			{	j = table[tmp & ~INDIRECT]++;
#ifdef CHECKINDIRECT
				if (j&INDIRECT)
					sz_error(SZ_DOUBLEINDIRECT);
#endif
			}
			else
			{	table[j]++;
				j = tmp;
			}
			putc(in[j],stdout);
		}
	else
		for (i=0; i<length; i++)
		{	uint4 tmp = table[j];
			if (tmp & INDIRECT)
				j = table[tmp & ~INDIRECT]++;
			else
			{	table[j]++;
				j = tmp;
			}
			out[i] = in[j];
		}

	if (j != indexlast)
		sz_error(SZ_NOTCYCLIC);
//	free(table);
}


#if defined SZ_SRT_O4
// a fast alternate sort, only for order 4. inout only length bytes is OK here.
void sz_srt_o4(unsigned char *inout, uint4 length, uint4 *indexlast)
{	static uint4 *counters=NULL;
	static uint2 *context=NULL;
	static unsigned char *symbols=NULL;
	register uint4 i;

	// count contexts
	if (counters==NULL) {
		counters = (uint4*)calloc(0x10000,sizeof(uint4));
		if (counters == NULL)
			sz_error(SZ_NOMEM_SORT);
	} else {
		memset(counters,0,0x10000*sizeof(uint4));
	}
	i = (uint)(inout[length-1])<<8;
  {	register unsigned char *tmp;
	for (tmp=inout; tmp<inout+length; tmp++)
	{	i = i>>8 | (uint)(*tmp)<<8;
		counters[i]++;
	}
  }

	// add context counts
  {	register uint4 sum = length;
	for (i=0x10000; i--; )
	{	sum -= counters[i];
		counters[i] = sum;
	}
  }

	// first sort pass
    if (context==NULL) {
		context = (uint2*)malloc(length*sizeof(uint2));
		if (context == NULL)
			sz_error(SZ_NOMEM_SORT);
	}
	if (symbols==NULL) {
		symbols = (unsigned char*)(malloc(length));
		if (symbols == NULL)
			sz_error(SZ_NOMEM_SORT);
	}

	// the following loop in assembler it would probably be a lot faster
  {	register unsigned char *tmp;
	register uint4 ctx = (uint4)inout[length-4]<<8 | inout[length-5];
	if (ctx == 0xffff)
		*indexlast = length-1;
	else
		*indexlast = counters[ctx+1]-1;
	ctx = (((uint4)inout[length-1] << 8 | inout[length-2]) << 8 |
		    inout[length-3]) << 8 | inout[length-4];
	for (tmp=inout; tmp<inout+length; tmp++)
	{	register uint4 x = counters[ctx&0xffff]++;
		context[x] = ctx >> 16;
		ctx = ctx>>8 | (uint4)(symbols[x] = *tmp)<<24;
	}
  }

	/* second sort pass */
  {	uint4 lastpos = *indexlast;
	for (i=length; i>lastpos; )		// lastpos is the last processed in this loop
	{	i-=1;
		inout[--counters[context[i]]] = symbols[i];
	}
  }
	*indexlast = counters[context[i]];
	while (i--)
		inout[--counters[context[i]]] = symbols[i];

//	free(counters);
//	free(context);
//	free(symbols);
}
#endif


#ifdef SZ_UNSRT_O4
// an alternate backtransform for order 4 using hash tables
void sz_unsrt_o4(unsigned char *in, unsigned char *out, uint4 length, uint4 indexlast,
			   uint4 *counts)
{	uint4 i, *contexts2, *contexts4, initcontext;
	uint2 *lastseen;
	unsigned char *loop, *endloop, nocounts;
	h2table htable;

	// get counts if not supplied
	nocounts = counts==NULL;
	if (nocounts)
	{	counts = (uint4*) calloc(256, sizeof(uint4));
		for (i=0; i<length; i++)
			counts[in[i]]++;
	}
	
	// allocate tables (could do on stack, but will cause problems with some compilers
	contexts2 = (uint4*)calloc(0x10000, sizeof(uint4));
	if (contexts2 == NULL)
		sz_error(SZ_NOMEM_SORT);

	// count contexts
	loop = in;
	for (i=0; i<0x100; i++)
		for (endloop = loop+counts[i]; loop<endloop; loop++)
			contexts2[(unsigned int)(*loop)<<8 | i]++;
		
	// put sum of order2 contexts in order 4 contexts
	contexts4 = (uint4*)malloc(0x10000*sizeof(uint4));
	if (contexts4 == NULL)
		sz_error(SZ_NOMEM_SORT);
  {	uint4 sum = length;
	for (i=0x10000; i--;)
	{	if (sum > indexlast)
			initcontext = i;
		sum -= contexts2[i];
		contexts4[i] = sum;
	}
	initcontext <<= 16;
	// sum counts
	sum = length;
	for (i=0x100; i--;)
	{	sum -= counts[i];
		counts[i] = sum;
	}
  }

	initHash2(htable);

	// in hardware: make lastseen a bitfield and zero them all when you hit a new
	// order 2 context. set them whenever you see a new order 4 context.
	// much like in the loop for the 0x????0000 contexts.

	// loop over all order 2 contexts
	// count contexts 0x????0000 first
	loop = in;
	lastseen = (uint2*)calloc(0x10000,sizeof(uint2));
	if (lastseen == NULL)
		sz_error(SZ_NOMEM_SORT);
	for (endloop = loop+contexts2[0]; loop<endloop; loop++)
	{	uint4 j, tmp = (uint4)(*loop);
		j = counts[tmp]++;
		tmp |= (uint4)(in[j])<<8;
		if (!lastseen[tmp])
		{	lastseen[tmp] = 1;
			h2_insert(htable, tmp<<16, contexts4[tmp]);
		}
		contexts4[tmp]++;
	}

	
	// and now all others (couldnt do it in one because of lastseens limited size)
	memset(lastseen, 0, 0x10000*sizeof(uint2));		// zero lastseen again
	for (i=1; i<0x10000; i++)
	{	if (indexlast >= contexts4[initcontext>>16])
			initcontext = (initcontext & 0xffff0000) | i;
		for (endloop = loop+contexts2[i]; loop<endloop; loop++)
		{	uint4 j, tmp = (uint4)(*loop);
			j = counts[tmp]++;
			tmp |= (uint4)(in[j])<<8;
			if (lastseen[tmp] != i)
			{	lastseen[tmp] = i;
				h2_insert(htable, tmp<<16 | i, contexts4[tmp]);
			}
			contexts4[tmp]++;
		}
	}

	free(contexts2);
	free(contexts4);
	free(lastseen);
	if (nocounts)
		free(counts);

	// do the actual unsorting
	initcontext = initcontext>>8 | (uint4)in[indexlast]<<24;
  {	uint4 context = initcontext;
	if (out == NULL)
		for ( i=0; i<length; i++ )
		{	unsigned char outchar;
			outchar = in[h2_get_inc(htable, context)];
			context = (context>>8) | ((uint4)outchar<<24);
			putc(outchar, stdout);
		}
	else
		for ( i=0; i<length; i++ )
		{	unsigned char outchar;
			out[i] = outchar = in[h2_get_inc(htable, context)];
			context = (context>>8) | ((uint4)outchar<<24);
		}

	if (context != initcontext)
		sz_error(SZ_NOTCYCLIC);
  }
	freeHash2(htable);
}
#endif


#ifdef SZ_SRT_BW

#include "qsort_u4.c"

void sz_srt_BW(unsigned char *inout, uint4 length, uint4 *indexfirst)
{	uint4 i, counts[256], counts1[256], *contextp, start;

	for (i=0; i<256; i++)
		counts[i] = 0;
	for (i=0; i<length; i++)
		counts[inout[i]]++;
	counts1[0] = 0;
	for (i=0; i<255; i++) 
		counts1[i+1] = counts1[i] + counts[i];
	
	contextp = (uint4*) calloc(length, sizeof(uint4));
	if (contextp == NULL)
		sz_error(SZ_NOMEM_SORT);

	for (i=0; i<length; i++)
		contextp[counts1[inout[i]]++] = i;

	start = 0;
	for (i=0; i<256; i++)
    {   if (verbosity&1) fputc((char)('0'+i%10),stderr);
        if (counts[i])
        {	qsort_u4(contextp+start, counts[i], inout, i==inout[0]?0:1);
			if (i==inout[length-1]) // search for indexfirst
			{	uint4 j=start;
                while(contextp[j]!=(length-1))
                    j++;
				*indexfirst = j;
			}
            start += counts[i];
		}
    }

	contextp[*indexfirst] = 0;
	for(i=0; i<length; i++)
		contextp[i] = inout[contextp[i]+1];
	contextp[*indexfirst] = inout[0];
	for(i=0; i<length; i++)
		inout[i] = contextp[i];

	free(contextp);
}


void sz_unsrt_BW(unsigned char *in, unsigned char *out, uint4 length,
			   uint4 indexfirst, uint4 *counts)
{	uint4 i, *transvec;
	unsigned char nocounts;

	// get counts if not supplied
	nocounts = counts==NULL;
	if (nocounts)
	{	counts = (uint4*) calloc(256, sizeof(uint4));
		if (counts == NULL)
			sz_error(SZ_NOMEM_SORT);
		for (i=0; i<length; i++)
			counts[in[i]]++;
	}
    
  {	uint4 sum = length;
	for (i=0x100; i--; )
	{	sum -= counts[i];
		counts[i] = sum;
	}
  }

	// prepare transposition vector
	transvec = (uint4*)malloc((length)*sizeof(uint4));
	if (transvec == NULL)
		sz_error(SZ_NOMEM_SORT);

	transvec[indexfirst] = counts[in[indexfirst]]++;
	for (i=0; i<indexfirst; i++)
		transvec[i] = counts[in[i]]++;
	for (i=indexfirst+1; i<length; i++)
		transvec[i] = counts[in[i]]++;


	if (nocounts)
		free(counts);

	// undo the blocksort
  {	uint4 ic=indexfirst;
	if (out==NULL)
		for (i=0; i<length; i++)
		{	putc(in[ic], stdout);
			ic = transvec[ic];
		}
	else
		for (i=0; i<length; i++)
		{	out[i] = in[ic];
			ic = transvec[ic];
		}
	if (ic != indexfirst)
		sz_error(SZ_NOTCYCLIC);
  }
	free(transvec);
}
#endif
