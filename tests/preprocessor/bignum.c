#include <assert.h>
#include <stdio.h>

#define BIG64 18446744073709551615ull
#define BIG32 4294967295ul

#define XX BIG64/BIG32
#define YY BIG64%BIG32

#if XX == 4294967297ull
int divide = 1;
#else
int divide = 0;
#endif

#if YY == 0
int mod = 1;
#else
int mod = 0;
#endif

main()
{
	assert(divide == 1 && mod == 1);
}