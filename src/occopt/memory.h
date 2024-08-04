# Software License Agreement
# 
#     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 
# 

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

#include "ctypes.h"
#if defined(BORLAND) || defined(__clang__) || defined(__GNUC__)
// hack for buggy embarcadero compiler
#    define beLocalAlloc(x) Alloc(x)
#    define beGlobalAlloc(x) globalAlloc(x)
#endif

typedef struct _memblk_
{
    struct _memblk_* next;
    long size;
    long left;
    char m[1]; /* memory area */
} MEMBLK;
void mem_summary(void);
void* globalAlloc(int size);
void globalFree(void);
void* localAlloc(int size);
void localFree(void);
void* Alloc(int size);
void* nzAlloc(int size);
void* oAlloc(int size);
void oFree(void);
void* aAlloc(int size);
void aFree(void);
void* tAlloc(int size);
void tFree(void);
void* cAlloc(int size);
void cFree(void);
void* sAlloc(int size);
void sFree(void);
void SetGlobalFlag(bool flag, bool& old);
void ReleaseGlobalFlag(bool old);
bool GetGlobalFlag(void);
char* litlate(const char* name);
LCHAR* wlitlate(const LCHAR* name);

template <class T>
T* Allocate(int sz = 1)
{
    auto rv = static_cast<T*>(Alloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* nzAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(nzAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* oAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(oAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* aAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(aAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* tAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(tAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* cAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(cAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* sAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(sAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* localAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(localAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* beLocalAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(beLocalAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
template <class T>
T* globalAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(globalAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}

template <class T>
T* beGlobalAllocate(int sz = 1)
{
    auto rv = static_cast<T*>(beGlobalAlloc(sz * sizeof(T)));
    new (rv) T;
    return rv;
}
