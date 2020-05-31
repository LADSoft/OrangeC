#ifndef SZ_SRT_H
#define SZ_SRT_H
#include "port.h"


// inout: bytes to be sorted; sorted bytes on return. must be length+order bytes long
// length: number of bytes in inout
// *indexlast: returns position of last context (needed for unsort)
// order: order of context used in sorting (must be >=3)
// the code assumes length>=order
// and inout is length+order bytes long (only the first length need to be filled)
void sz_srt(unsigned char *inout, uint4 length, uint4 *indexlast, unsigned int order);


// in: bytes to be unsorted
// out: unsorted bytes; if NULL output is written to stdout
// length: number of bytes in in (and out)
// indexlast: position of last context (as returned bt sorttrans)
// counts: number of occurances of each byte in in (if NULL it will be calculated)
// order: order of context used in sorting (must be >=3)
// the code assumes length>=order
void sz_unsrt(unsigned char *in, unsigned char *out, uint4 length, uint4 indexlast,
			   uint4 *counts, unsigned int order);


// comment the following #defines if you dont want them
#define SZ_SRT_O4
//#define SZ_UNSRT_O4
#define SZ_SRT_BW

// alternate sorter for order 4 (different method, same result)
#if defined SZ_SRT_O4
void sz_srt_o4(unsigned char *inout, uint4 length, uint4 *indexlast);
#endif


// alternate unsorter for order 4 (different method (hash), same result)
#if defined SZ_UNSRT_O4
void sz_unsrt_o4(unsigned char *in, unsigned char *out, uint4 length, uint4 indexlast,
				 uint4 *counts);
#endif


#if defined SZ_SRT_BW
// unlimited context sort (BWT but with context before symbol)
void sz_srt_BW(unsigned char *inout, uint4 length, uint4 *indexfirst);

// unsorter for unlimited context sort
void sz_unsrt_BW(unsigned char *in, unsigned char *out, uint4 length,
			   uint4 indexfirst, uint4 *counts);
#endif
#endif
