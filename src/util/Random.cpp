/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "Utils.h"
#include <string>
#include <array>
#include <random>
#include <algorithm>
#include <functional>

#ifdef _MINGW
#    include <Windows.h>
#endif

class RandomNumber
{
  public:
    RandomNumber()
    {
#ifdef _MINGW
        if (!CryptAcquireContext(&ctx, nullptr, nullptr, PROV_RSA_FULL, 0))
            CryptAcquireContext(&ctx, nullptr, nullptr, PROV_RSA_FULL, CRYPT_NEWKEYSET);
#endif
    }
    ~RandomNumber()
    {
#ifdef _MINGW
        CryptReleaseContext(ctx, 0);
#endif
    }
    unsigned Get(unsigned size)
    {
#ifdef _MINGW
        unsigned rv;
        BYTE* buffer = reinterpret_cast<BYTE*>(&rv);
        CryptGenRandom(ctx, sizeof(rv), buffer);
        return rv % size;
#else
        return dev() % size;
#endif
    }
#ifdef _MINGW
    HCRYPTPROV ctx;
#else
    std::random_device dev;
#endif
};

unsigned Utils::Random(unsigned size)
{
    static RandomNumber rnd;
    return rnd.Get(size);
}