#include <stdlib.h>
/* Combination of 3 tausworthe generators -- assumes 32-bit integers */
/* Source: Pierre L'Ecuyer, "Maximally Equidistributed Combined Tausworthe
   Generators". Mathematics of Computation, vol.65, no.213(1996), pp203--213.*/
/* Special thanks to Joushua Grass (of Microsoft) for pointing out an error
   in the initialization routine "rand_seed()" */

#pragma startup preinit 32

int rand();             /* returns a random 32 bit integer */
static void rand_seed( unsigned int, unsigned int, unsigned int );

/* return a random float >= 0 and < 1 */
#define rand_float          ((double)rand() / 4294967296.0)

static unsigned int s1=390451501, s2=613566701, s3=858993401;  /* The seeds */
static unsigned mask1, mask2, mask3;
static int shft1, shft2, shft3, k1=31, k2=29, k3=28;

/* use either of the following two sets of parameters*/
static int q1=13, q2=2, q3=3, p1=12, p2=4, p3=17;
/* static int q1=3, q2=2, q3=13, p1=20, p2=16, p3=7; */

int _RTL_FUNC rand()
   {
   unsigned int b;
  
   b  = ((s1 << q1)^s1) >> shft1;
   s1 = ((s1 & mask1) << p1) ^ b;
   b  = ((s2 << q2) ^ s2) >> shft2;
   s2 = ((s2 & mask2) << p2) ^ b;
   b  = ((s3 << q3) ^ s3) >> shft3;
   s3 = ((s3 & mask3) << p3) ^ b;
   return (s1 ^ s2 ^ s3) & 0x7fff; // we could go larger but I'm worried about the
                                    // affect on the random() macro.
   }


static void rand_seed( unsigned int a, unsigned int b, unsigned int c )
   {
   static unsigned int x = 4294967295U;

   shft1 = k1-p1;
   shft2 = k2-p2;
   shft3 = k3-p3;
   mask1 = x << (32-k1);
   mask2 = x << (32-k2);
   mask3 = x << (32-k3);
   if (a > (1 << (32-k1))) s1 = a;
   if (b > (1 << (32-k2))) s2 = b;
   if (c > (1 << (32-k3))) s3 = c;
   rand();
   }
#define ROTL(s,bits) (((s) << (bits)) | ((s) >> (32 - (bits)))) 

void _RTL_FUNC srand(unsigned seed)
{
    rand_seed(seed,ROTL(seed,8),ROTL(seed,16));
}
static void preinit()
{
    srand(1);
}