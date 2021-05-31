#include <stdio.h>
#include <setjmp.h>

jmp_buf inflate_sync;
unsigned char *inflatebuf;
int inflatepos;
unsigned char *outputQueue;
int outputPos;
unsigned accum;
unsigned accum_count;

unsigned masktab[32];

static unsigned get_n_bits_b(int count)
{
    return accum & masktab[count];

//    int rv = accum & masktab[count];
//    return rv;
}

static unsigned get_n_bits(int count)
{
    return get_n_bits_b(count);
}

static void GetInflateTables(void)
{
    int scount;

            scount = inflatebuf[inflatepos++];
            scount |= inflatebuf[inflatepos++] << 8;

		if (scount)
		{
		}

}


