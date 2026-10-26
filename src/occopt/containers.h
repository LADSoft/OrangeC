/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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
 *
 */
#pragma once

#include <type_traits>
inline int count1, count2;
// this is meant for scalars right now...
// this whole thing is because instantiation a vector of lists with the stl is very slow
// and since the vector is usually sparse anyway a speedup is helpful...
template <class T>
class FastList
{
  private:
    T* data;
    size_t capacity;
    size_t elems;

  public:
    typedef T* iterator;
    // this class deliberately doesn't have constructors to make initial construction fast
    inline void init()
    {
        data = nullptr;
        capacity = 8;
        elems = 0;
    }
    inline void destroy() { delete[] data; }
    void push_back(T value)
    {
        if (data == nullptr)
        {
            data = new T[capacity];
        }
        else if (elems == capacity)
        {
            resize(capacity * 2);
        }
        data[elems++] = value;
    }
    void pop_back()
    {
        if (elems)
        {
            --elems;
        }
    }
    void push_front(T val)
    {
        count1++;
        if (data == nullptr)
        {
            data = new T[capacity];
        }
        else if (elems == capacity)
        {
            resize(capacity * 2);
        }
        std::copy_backward(begin(), end(), end() + 1);
        elems++;
        data[0] = val;
    }
    void pop_front()
    {
        count2++;
        if (elems)
        {
            std::copy(begin() + 1, end(), begin());
            --elems;
        }
    }
    void remove(T val)
    {
        for (auto it = begin(); it != end(); ++it)
        {
            if (val == *it)
            {
                std::copy(it + 1, end(), it);
                --elems;
                break;
            }
        }
    }
    void insert(iterator it, T val)
    {
        ptrdiff_t offset = it - data;
        if (data == nullptr)
        {
            data = new T[capacity];
        }
        else if (elems == capacity)
        {
            resize(capacity * 2);
        }
        it = data + offset;
        std::copy_backward(it, end(), end() + 1);
        elems++;
        *it = val;
    }
    T& front()
    {
        static T empty;
        return elems == 0 ? empty : data[0];
    }
    inline iterator begin() { return data; }
    inline iterator end() { return data + elems; }
    inline void clear() { destroy(); }
    inline size_t size() const { return elems; }
    inline bool empty() const { return elems == 0; }

  protected:
    void resize(size_t newSize)
    {
        // only growing...
        T* newData = new T[newSize];
        std::copy(begin(), end(), newData);
        delete[] data;
        data = newData;
        capacity = newSize;
    }
};
template <typename T>
class SimpleVector
{
  private:
    T* data;
    size_t elems;
    size_t capacity;

  public:
    SimpleVector() : elems(0), capacity(0), data(nullptr) {}

    ~SimpleVector() { clear(); }
    void resize(size_t newsize, bool setsize = true)
    {
        if (newsize == 0)
            newsize++;
        T* newData = new T[newsize];
        if (newsize > elems)
        {
            memmove(newData, data, elems * sizeof(T));
            if (setsize)
            {
                for (size_t i = elems; i < newsize; i++)
                    newData[i].init();
            }
        }
        else
        {
            auto c = std::min(newsize, elems);
            memmove(newData, data, c * sizeof(T));
            for (size_t i = c; i < elems; i++)
                data[i].destroy();
        }
        delete[] data;
        data = newData;
        capacity = newsize;
        if (setsize || elems > newsize)
            elems = newsize;
    }
    void clear()
    {
        if (capacity)
        {
            for (int i = 0; i < elems; i++)
                data[i].destroy();
            capacity = 0;
            elems = 0;
            delete[] data;
            data = nullptr;
        }
    }
    void push_back(T value)
    {
        if (data == nullptr)
            resize(capacity, false);
        else if (elems == capacity)
        {
            resize(nextCapacity(), false);
        }
        data[elems++] = value;
    }
    inline T& operator[](size_t index) { return data[index]; }

    inline size_t size() const { return elems; }
    auto begin() { return data; }
    auto end() { return data + elems; }

  private:
    size_t nextCapacity() { return capacity ? capacity * 2 : 1; }
};
#include <vector>
// template <class T>
// using FastList = std::list<T>;
// template <class T>
// using SimpleVector =  std::vector<T>;
