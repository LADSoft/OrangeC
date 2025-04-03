/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

// this is here because the threading stuff has to be up and running before the memory management stuff is
// but we have to allocate things in the threading stuff
#include <windows.h>

template <class T>
class LocalAllocAllocator
{
  public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    LocalAllocAllocator() {}
    LocalAllocAllocator(const LocalAllocAllocator&) {}

    pointer allocate(size_type n, const void* = 0) { return (T*)LocalAlloc(LMEM_FIXED, n * sizeof(T)); }

    void deallocate(void* p, size_type)
    {
        if (p)
        {
            LocalFree(p);
        }
    }

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    LocalAllocAllocator<T>& operator=(const LocalAllocAllocator&) { return *this; }
    template <class... Args>
    void construct(pointer p, Args&&... args)
    {
        new ((T*)p) T(std::forward<Args>(args)...);
    }
    void destroy(pointer p) { p->~T(); }

    size_type max_size() const { return size_t(-1); }

    template <class U>
    struct rebind
    {
        typedef LocalAllocAllocator<U> other;
    };

    template <class U>
    LocalAllocAllocator(const LocalAllocAllocator<U>&)
    {
    }

    template <class U>
    LocalAllocAllocator& operator=(const LocalAllocAllocator<U>&)
    {
        return *this;
    }
};
