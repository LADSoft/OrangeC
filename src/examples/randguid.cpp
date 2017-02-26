#include <algorithm>
#include <functional>
#include <array>
#include <random>
#include <iostream>


typedef unsigned char Byte;

void CreateGuid(Byte *Guid)
{

    // note that this whole thing will fall apart if the C++ lib uses
    // a prng with constant seed for the random_device implementation.
    // that shouldn't be a problem on OS we are interested in.
    std::array<unsigned char, 128/8> rnd;
    std::uniform_int_distribution<int> distribution(0, 0xff);
    std::random_device dev;
    std::mt19937 engine(dev());
    auto generator = std::bind(distribution, engine);

    std::generate(rnd.begin(), rnd.end(), generator);

    // make it a valid version 4 (random) GUID
    // remember that on windows GUIDs are native endianness so this may need
    // work if you port it
    rnd[7/*6*/] &= 0xf;
    rnd[7/*6*/] |= 0x40;
    rnd[9/*8*/] &= 0x3f;
    rnd[9/*8*/] |= 0x80;

    memcpy(Guid, rnd.data(), rnd.size());
}
int main()
{
    Byte buf[200];
    CreateGuid(buf);
    for (int i=0; i < 16; i++)
        printf("%x ", buf[i]);
}