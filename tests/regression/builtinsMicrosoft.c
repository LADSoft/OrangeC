#include <stdio.h>
#ifdef _MSC_VER
#include <intrin.h>
#include <stdlib.h>
#endif
int main()
{
    unsigned long index;
    printf("%d\n", _BitScanReverse(&index, 12));
    printf("Index: %d\n", index);
    printf("%d\n", _BitScanForward(&index, 12));
    printf("Index: %d\n", index);
    printf("%d\n", _BitScanReverse(&index, 0));
    printf("%d\n", _BitScanForward(&index, 0));
    printf("End bitscan checks\n");

    printf("%d\n", _rotr8(12, 2));
    printf("%d\n", _rotr16(12, 2));
    printf("%d\n", _rotr(12, 2));
    printf("%lld\n", _rotr64(12, 2));

    printf("%d\n", _rotl8(12, 2));
    printf("%d\n", _rotl16(12, 2));
    printf("%d\n", _rotl(12, 2));
    printf("%lld\n", _rotl64(12, 2));
    printf("End rotate checks\n");
    return 0;
}