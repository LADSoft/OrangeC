/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include <random>
#include <array>
#include <algorithm>
#include <functional>
#include "DotNetPELib.h"
#include "PEFile.h"

void DotNetPELib::PEWriter::CreateGuid(Byte* Guid)
{
    std::array<unsigned char, 128 / 8> rnd;

    std::uniform_int_distribution<int> distribution(0, 0xff);
    // note that this whole thing will fall apart if the C++ lib uses
    // a prng with constant seed for the random_device implementation.
    // that shouldn't be a problem on OS we are interested in.
    std::random_device dev;
    std::mt19937 engine(dev());
    auto generator = std::bind(distribution, engine);

    std::generate(rnd.begin(), rnd.end(), generator);

    // make it a valid version 4 (random) GUID
    // remember that on windows GUIDs are native endianness so this may need
    // work if you port it
    rnd[7 /*6*/] &= 0xf;
    rnd[7 /*6*/] |= 0x40;
    rnd[9 /*8*/] &= 0x3f;
    rnd[9 /*8*/] |= 0x80;

    memcpy(Guid, rnd.data(), rnd.size());
}
