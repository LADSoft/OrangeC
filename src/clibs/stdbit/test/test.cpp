#define _USE_32BIT_TIME_T
#include <stdbit.h>
#include <limits.h>
#include <Stdlib.h>
#include <time.h>

template <class T>
void tceil()
{
        printf("%Lx\n%Lx\n\n", 0ULL, (unsigned long long)stdc_bit_ceil((T)0));
        printf("%Lx\n%Lx\n\n", 1ULL, (unsigned long long)stdc_bit_ceil((T)1));
	for (unsigned long long i =2; i ; i <<= 1)
		printf("%Lx\n%Lx\n\n", i+1, (unsigned long long)stdc_bit_ceil((T)(i+1)));
}

template <class T>
void tfloor()
{
        printf("%Lx\n%Lx\n\n", 0ULL, (unsigned long long)stdc_bit_floor((T)0));
        printf("%Lx\n%Lx\n\n", 1ULL, (unsigned long long)stdc_bit_floor((T)1));
	for (unsigned long long i =2; i ; i <<= 1)
		printf("%Lx\n%Lx\n\n", i+1, (unsigned long long)stdc_bit_floor((T)(i+1)));
}

template <class T>
void twidth()
{
	printf("%d\n%d\n\n", stdc_bit_width((T)0), stdc_bit_width((T)1));
}
template <class T>
void thassingle(T val)
{
	printf("%Lx,%d\n", (long long)val, stdc_has_single_bit((T)(val)));
	printf("%Lx,%d\n", (long long)val + 1, stdc_has_single_bit((T)(val+1)));
	printf("%Lx,%d\n", (long long)val + 0x80, stdc_has_single_bit((T)(val+0x80)));
	printf("%Lx,%d\n", (long long)val + 0x8000, stdc_has_single_bit((T)(val+0x8000)));
	printf("%Lx,%d\n", (long long)val + 0x80000000, stdc_has_single_bit((T)(val+0x80000000)));
	printf("%Lx,%d\n", (long long)val + 0x8000000000000000ull, stdc_has_single_bit((T)(val+0x8000000000000000ull)));
        printf("\n");
}
template <class T>
void thassingle()
{
       thassingle((T)1);
       thassingle((T)0x80);
       thassingle((T)0x100);
       thassingle((T)0x8000);
       thassingle((T)0x1000000);
       thassingle((T)0x80000000);
       thassingle((T)0x100000000000000ull);
       thassingle((T)0x8000000000000000ull);
       thassingle((T)4);
       printf("\n");
}
template <class T>
void tcount(T val)
{
    printf("%Lx, %u %u\n", (long long)val, stdc_count_zeros((T)val), stdc_count_ones((T)val)); 

}
template <class T>
void tcount()
{
	tcount((T)1);
	tcount((T)4);
	tcount((T)0x80);
	tcount((T)0x100);
	tcount((T)0x8000);
	tcount((T)0x1000000);
	tcount((T)0x80000000);
	tcount((T)0x100000000000000);
	tcount((T)0x8000000000000000);
	tcount((T)0x55);
	tcount((T)0x5500);
	tcount((T)0x550000);
	tcount((T)0x55000000);
	tcount((T)0x5500000000000000ull);
	tcount((T)0x5555555555555555ull);
	tcount((T)0xaaaaaaaaaaaaaaaaull);
	printf("\n");
}
template <class T>
void tleadingtrailing(T val)
{
    printf("%Lx, %u %u %u %u\n", (long long)val, stdc_first_leading_zero((T)val), stdc_first_trailing_zero((T)val), stdc_first_leading_one((T)val), stdc_first_trailing_one((T)val)); 

}
template <class T>
void tleadingtrailing()
{
    tleadingtrailing((T)0x0);
    tleadingtrailing((T)0xff);
    tleadingtrailing((T)0xffff);
    tleadingtrailing((T)0xff00);
    tleadingtrailing((T)0xffffffff);
    tleadingtrailing((T)0xffff0000);
    tleadingtrailing((T)0xffffffffffffffffll);
    tleadingtrailing((T)0xffffffff00000000ll);
    tleadingtrailing((T)0x35);
    tleadingtrailing((T)0xac);
    tleadingtrailing((T)0xff35);
    tleadingtrailing((T)0xffac);
    tleadingtrailing((T)0xffffff35);
    tleadingtrailing((T)0xffffffac);
    tleadingtrailing((T)0xffffffffffffff35ll);
    tleadingtrailing((T)0xffffffffffffffacll);
    tleadingtrailing((T)0x15);
    tleadingtrailing((T)0xa8);
    tleadingtrailing((T)0x15f5);
    tleadingtrailing((T)0xa8f8);
    tleadingtrailing((T)0xffff15f5);
    tleadingtrailing((T)0xffffaff8);
    tleadingtrailing((T)0xffffffffffff1ff8ll);
    tleadingtrailing((T)0xffffffffffffaff8ll);
    tleadingtrailing((T)0x75);
    tleadingtrailing((T)0xae);
    tleadingtrailing((T)0x7ff5);
    tleadingtrailing((T)0xaffe);
    tleadingtrailing((T)0x7ffffff5);
    tleadingtrailing((T)0xaffffffe);
    tleadingtrailing((T)0xffffffff75fffff5ll);
    tleadingtrailing((T)0xffffffffaffffffell);
    tleadingtrailing((T)0x05);
    tleadingtrailing((T)0xa0);
    tleadingtrailing((T)0x0ff5);
    tleadingtrailing((T)0xaff0);
    tleadingtrailing((T)0xffff0ff5);
    tleadingtrailing((T)0xffffaffe);
    tleadingtrailing((T)0x0ffffffffffffff5ll);
    tleadingtrailing((T)0xa0ffffffffffff0ll);
}

template <class T>
void tfirst(T val)
{
    printf("%Lx, %u %u %u %u\n", (long long)val, stdc_first_leading_zero((T)val), stdc_first_trailing_zero((T)val), stdc_first_leading_one((T)val), stdc_first_trailing_one((T)val)); 

}
template <class T>
void tfirst()
{
    tfirst((T)0x0);
    tfirst((T)0xff);
    tfirst((T)0xffff);
    tfirst((T)0xff00);
    tfirst((T)0xffffffff);
    tfirst((T)0xffff0000);
    tfirst((T)0xffffffffffffffffll);
    tfirst((T)0xffffffff00000000ll);
    tfirst((T)0x35);
    tfirst((T)0xac);
    tfirst((T)0xff35);
    tfirst((T)0xffac);
    tfirst((T)0xffffff35);
    tfirst((T)0xffffffac);
    tfirst((T)0xffffffffffffff35ll);
    tfirst((T)0xffffffffffffffacll);
    tfirst((T)0x15);
    tfirst((T)0xa8);
    tfirst((T)0x15f5);
    tfirst((T)0xa8f8);
    tfirst((T)0xffff15f5);
    tfirst((T)0xffffaff8);
    tfirst((T)0xffffffffffff1ff8ll);
    tfirst((T)0xffffffffffffaff8ll);
    tfirst((T)0x75);
    tfirst((T)0xae);
    tfirst((T)0x7ff5);
    tfirst((T)0xaffe);
    tfirst((T)0x7ffffff5);
    tfirst((T)0xaffffffe);
    tfirst((T)0xffffffff75fffff5ll);
    tfirst((T)0xffffffffaffffffell);
    tfirst((T)0x05);
    tfirst((T)0xa0);
    tfirst((T)0x0ff5);
    tfirst((T)0xaff0);
    tfirst((T)0xffff0ff5);
    tfirst((T)0xffffaffe);
    tfirst((T)0x0ffffffffffffff5ll);
    tfirst((T)0xa0ffffffffffff0ll);
}

int main()
{
	printf("Ceil\n");
	tceil<unsigned char>();
	tceil<unsigned short>();
	tceil<unsigned int>();
	tceil<unsigned long>();
	tceil<unsigned long long>();

	printf("floor\n");
	tfloor<unsigned char>();
	tfloor<unsigned short>();
	tfloor<unsigned int>();
	tfloor<unsigned long>();
	tfloor<unsigned long long>();

        printf("width\n");
	twidth<unsigned char>();
	twidth<unsigned short>();
	twidth<unsigned int>();
	twidth<unsigned long>();
	twidth<unsigned long long>();

        printf("hassingle\n");
	thassingle<unsigned char>();
	thassingle<unsigned short>();
	thassingle<unsigned int>();
	thassingle<unsigned long>();
	thassingle<unsigned long long>();

        printf("count\n");
	tcount<unsigned char>();
	tcount<unsigned short>();
	tcount<unsigned int>();
	tcount<unsigned long>();
	tcount<unsigned long long>();

        printf("leading trailing\n");
	tleadingtrailing<unsigned char>();
	tleadingtrailing<unsigned short>();
	tleadingtrailing<unsigned int>();
	tleadingtrailing<unsigned long>();
	tleadingtrailing<unsigned long long>();

        printf("first\n");
	tfirst<unsigned char>();
	tfirst<unsigned short>();
	tfirst<unsigned int>();
	tfirst<unsigned long>();
	tfirst<unsigned long long>();

}
}