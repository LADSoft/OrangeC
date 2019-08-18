#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define XOR(a,b) (a + b - 2*AND(a,b))
#define IOR(a,b) XOR(XOR(a,b),AND(a,b)) 

/*static const uint16_t andlookup[256] =
{
    0,0,0,0,0,0,0,0,0,0, 0, 0, 0, 0, 0, 0,0,1,0,1,0,1,0,1,0,1, 0, 1, 0, 1, 0, 1,
    0,0,2,2,0,0,2,2,0,0, 2, 2, 0, 0, 2, 2,0,1,2,3,0,1,2,3,0,1, 2, 3, 0, 1, 2, 3,
    0,0,0,0,4,4,4,4,0,0, 0, 0, 4, 4, 4, 4,0,1,0,1,4,5,4,5,0,1, 0, 1, 4, 5, 4, 5,
    0,0,2,2,4,4,6,6,0,0, 2, 2, 4, 4, 6, 6,0,1,2,3,4,5,6,7,0,1, 2, 3, 4, 5, 6, 7,
    0,0,0,0,0,0,0,0,8,8, 8, 8, 8, 8, 8, 8,0,1,0,1,0,1,0,1,8,9, 8, 9, 8, 9, 8, 9,
    0,0,2,2,0,0,2,2,8,8,10,10, 8, 8,10,10,0,1,2,3,0,1,2,3,8,9,10,11, 8, 9,10,11,
    0,0,0,0,4,4,4,4,8,8, 8, 8,12,12,12,12,0,1,0,1,4,5,4,5,8,9, 8, 9,12,13,12,13,
    0,0,2,2,4,4,6,6,8,8,10,10,12,12,14,14,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
};*/

static const uint16_t andlookup[256] = {
#define C4(a,b) ((a)&(b)), ((a)&(b+1)), ((a)&(b+2)), ((a)&(b+3))
#define L(a) C4(a,0), C4(a,4), C4(a,8), C4(a,12)
#define L4(a) L(a), L(a+1), L(a+2), L(a+3)
    L4(0), L4(4), L4(8), L4(12)
#undef C4
#undef L
#undef L4
};

uint16_t AND(uint16_t a, uint16_t b)
{
    uint16_t r=0, i;

    for ( i = 0; i < 16; i += 4 )
    {
            r = r/16 + andlookup[(a%16)*16+(b%16)]*4096;
            a /= 16;
            b /= 16;
    }
    return r;
}

int main() 
{
    uint16_t a = 0, b = 0;

    for(;a<1000;a++)
    {
      for(;b<1000;b++)
      {    
        if ( AND(a,b) != (a&b) ) return printf( "AND error\n" );
        if ( IOR(a,b) != (a|b) ) return printf( "IOR error\n" );
        if ( XOR(a,b) != (a^b) ) return printf( "XOR error\n" );
      }
    }
    return 0;
}