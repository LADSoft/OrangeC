#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define MX(p) (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[((p)&3)^e] ^ z)))

uint64_t xxtea_encrypt(const uint64_t inp, const uint32_t *key)
{
    static const uint32_t DELTA = 0x9e3779b9;
    static const unsigned int nrounds = 32;
    uint32_t y, z, sum = 0;
    union {
        uint32_t v[2];
        uint64_t x;
    } out;
    out.x = inp;
    z = out.v[1];
    for (unsigned int i = 0; i < nrounds; i++) {
        sum += DELTA;
        unsigned int e = (sum >> 2) & 3;
        y = out.v[1]; 
        z = out.v[0] += MX(0);
        y = out.v[0];
        z = out.v[1] += MX(1);
    }
    return out.x;
}

int xxtea_test()
{
    static const uint64_t OUT0 = 0x575d8c80053704ab;
    static const uint64_t OUT1 = 0xc4cc7f1cc007378c;
    uint32_t key[4] = {0, 0, 0, 0};
    // Test 1
    if (xxtea_encrypt(0, key) != OUT0) {
        fprintf(stderr, "0: %llX %llX", xxtea_encrypt(0, key), OUT0);
        return 0;
    }
    // Test 2
    key[0] = 0x08040201; key[1] = 0x80402010;
    key[2] = 0xf8fcfeff; key[3] = 0x80c0e0f0; 
    if (xxtea_encrypt(0x80c0e0f0f8fcfeff, key) != OUT1) {
        fprintf(stderr, "0: %llX %llX", xxtea_encrypt(0x80c0e0f0f8fcfeff, key), OUT1);
        return 0;
    }
    return 1;
}

int main()
{
    printf("Test result: %d\n", xxtea_test());
}