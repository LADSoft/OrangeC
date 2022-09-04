#include <random>
#include <array>
#include <algorithm>
#include <functional>


extern "C" char *__cpp_tmpnam(char *buf)
{
    std::array<char, 8> rnd;

    std::uniform_int_distribution<int> distribution(0, 10 + 26 * 2-1);
    // note that this whole thing will fall apart if the C++ lib uses
    // a prng with constant seed for the random_device implementation.
    // that shouldn't be a problem on OS we are interested in.
    std::random_device dev;
    std::mt19937 engine(dev());
    auto generator = std::bind(distribution, engine);

    std::generate(rnd.begin(), rnd.end(), generator);

    for (int i=0; i < 8; i++)
    {
        char ch = rnd[i];
        ch += '0';
        if (ch > '9')
            ch += 7;
        if (ch > 'Z')
            ch += 6;
        buf[i] = ch;
    }
    strcpy(buf + 8, ".tmp");
}