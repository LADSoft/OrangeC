#include "port.h"

void reorder(unsigned char *in, unsigned char *out, uint4 length, uint recordsize)
{	uint4 i,j;
	for (i=0; i<recordsize; i++)
		for(j=i; j<length; j+=recordsize)
			*(out++) = in[j];
}

void unreorder(unsigned char *in, unsigned char *out, uint4 length, uint recordsize)
{	uint4 i,j;
	for (i=0; i<recordsize; i++)
		for(j=i; j<length; j+=recordsize)
			out[j] = *(in++);
}
