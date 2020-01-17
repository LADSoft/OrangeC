/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include "SharedMemory.h"
#include <string>
#include <array>
#include <random>
#include <algorithm>
#include <functional>

#ifdef _WIN32
#include <Windows.h>
#endif

SharedMemory::SharedMemory(unsigned max, std::string name) : max_(max), current_(0), regionStart(0), regionHandle(nullptr)
{
    if (!name.empty())
        name_ = name;
    else
        SetName();
}

SharedMemory::~SharedMemory()
{
#ifdef _WIN32
    CloseMapping();
    CloseHandle(regionHandle);
#endif

}

bool SharedMemory::Open()
{
#ifdef _WIN32
    regionHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, name_.c_str());
#endif
    return !!regionHandle;
}

bool SharedMemory::Create()
{
#ifdef _WIN32
    regionHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
        PAGE_READWRITE | SEC_RESERVE,
        0, max_, name_.c_str()
        );
#endif
    return !!regionHandle;
}
unsigned char* SharedMemory::GetMapping()
{
#ifdef _WIN32
    if (!regionStart)
    {
        regionStart = (unsigned char*)MapViewOfFile(regionHandle, FILE_MAP_ALL_ACCESS, 0, 0, max_);
    }
#endif
    return regionStart;
}
void SharedMemory::CloseMapping()
{
#ifdef _WIN32
    if (regionStart)
    {
        UnmapViewOfFile(regionStart);
        regionStart = nullptr;
    }
#endif
}
bool SharedMemory::EnsureCommitted(int size)
{
    int end = size;
    end += 4095;
    end = (end / 4096) * 4096;
#ifdef _WIN32
    if (end > current_)
    {
        if (!VirtualAlloc(regionStart + current_, end - current_, MEM_COMMIT, PAGE_READWRITE))
            return false;
        current_ = end;
    }
    return true;
#else
    return false;
#endif
}
void SharedMemory::SetName()
{
    std::array<unsigned char, 21> rnd;

    std::uniform_int_distribution<int> distribution('a', 'z');
    std::random_device dev;
    std::mt19937 engine(dev());
    auto generator = std::bind(distribution, engine);

    std::generate(rnd.begin(), rnd.end(), generator);

    // the lssm: is an attempt to prevent the RNG from choosing someone else's name accidentally...
    name_ = "lssm:" + std::string(rnd.begin(), rnd.end());
}