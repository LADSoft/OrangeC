/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
#include "SharedMemory.h"
#include <string>
#include <array>
#include <random>
#include <algorithm>
#include <functional>

#ifdef _WIN32
#    include <Windows.h>
#endif

#define USE_PAGING_FILE

SharedMemory::SharedMemory(unsigned max, std::string name, unsigned window) :
    max_(max), windowSize_(window), current_(0), regionStart(0), regionHandle(nullptr), fileHandle_(nullptr), regionBase_(0)
{
    if (!name.empty())
        name_ = name;
    else
        SetName();
}

SharedMemory::~SharedMemory()
{
#ifdef _WIN32
    //    Flush();
    CloseMapping();
    CloseHandle(regionHandle);
    CloseHandle(fileHandle_);
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
#    ifdef USE_PAGING_FILE
    fileHandle_ = INVALID_HANDLE_VALUE;
#    else
    fileHandle_ = CreateFile(name_.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
#    endif
    regionHandle = CreateFileMapping(fileHandle_, NULL, PAGE_READWRITE | SEC_RESERVE, 0, max_, name_.c_str());
#endif
    return !!regionHandle && GetMapping();
}
void SharedMemory::Flush()
{
#ifdef _WIN32
    if (current_)
    {
        FlushViewOfFile(regionStart, current_);
        FlushFileBuffers(fileHandle_);
    }
#endif
}
unsigned char* SharedMemory::GetMapping(unsigned pos)
{
#ifdef _WIN32
    regionBase_ = pos & -4096;
    CloseMapping();
    regionStart = (unsigned char*)MapViewOfFile(regionHandle, FILE_MAP_ALL_ACCESS, 0, regionBase_, ViewWindowSize() * 2);
    if (regionStart)
        return regionStart - regionBase_;
#endif
    return 0;
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
    // remove next two lines???
//    current_ = end;
//    return true;
#ifdef _WIN32
    if (end > current_)
    {
        unsigned char* temp = (unsigned char*)MapViewOfFile(regionHandle, FILE_MAP_ALL_ACCESS, 0, current_, end - current_);
        if (!VirtualAlloc(temp, end - current_, MEM_COMMIT, PAGE_READWRITE))
        {
            UnmapViewOfFile(temp);
            return false;
        }
        UnmapViewOfFile(temp);
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

    std::generate(rnd.begin(), rnd.end(), [] { return Utils::Random(26) + 'a'; });

    // the lssm: is an attempt to prevent the RNG from choosing someone else's name accidentally...
    name_ = "lssm:" + std::string(rnd.begin(), rnd.end());
}