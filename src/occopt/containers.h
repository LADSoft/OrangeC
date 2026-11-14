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

#include <set>
#include <climits>
inline std::set<void*> aa;
#include <type_traits>

// sadly we have to have a bitset to speed up the live algorithm.  This should be a 20-25% speedup so we can't just ignore it....
// std::vector<std::bool> doesn't suffice because there are no guarantees on the implementation
class FastBitSet
{
  public:
    typedef unsigned StoreType;
    static const unsigned BitIntSize = sizeof(StoreType) * CHAR_BIT;

    void init()
    {
        data = nullptr;
        capacity = 0;
        elems = 0;
    }
    void destroy() { delete[] data; }
    FastBitSet& operator=(const FastBitSet& other)
    {
        resize(other.elems);

        std::copy(other.data, other.data + (other.elems + BitIntSize - 1) / BitIntSize, data);
        elems = other.elems;
        return *this;
    }
    FastBitSet& operator=(FastBitSet&& other)
    {
        delete[] data;
        elems = other.elems;
        capacity = other.capacity;
        data = other.data;
        other.data = nullptr;
        other.capacity = 8;
        other.elems = 0;
        return *this;
    }
    inline void setbit(unsigned n)
    {
        auto index = n / BitIntSize;
        auto mask = 1 << (n % BitIntSize);
        data[index] |= mask;
    }
    inline void clrbit(unsigned n)
    {
        auto index = n / BitIntSize;
        auto mask = 1 << (n % BitIntSize);
        data[index] &= ~mask;
    }
    inline bool isset(unsigned n)
    {
        if (!data)
            return false;
        auto index = n / BitIntSize;
        auto mask = 1 << (n % BitIntSize);
        return !!(data[index] & mask);
    }
    void union_from(const FastBitSet& added)
    {
        unsigned bitints = elems;
        if (added.elems < elems)
            bitints = added.elems;
        bitints += BitIntSize - 1;
        bitints /= BitIntSize;
        for (StoreType *dest = data, *src = added.data; bitints--; dest++, src++)
            *dest |= *src;
    }
    FastBitSet& dounion(const FastBitSet& added, FastBitSet& dest)
    {
        unsigned bitints = elems;
        if (added.elems < elems)
            bitints = added.elems;
        dest.resize(bitints);
        bitints += BitIntSize - 1;
        bitints /= BitIntSize;
        for (StoreType *d = dest.data, *s1 = added.data, *s2 = data; bitints--; d++, s1++, s2++)
            *d = *s1 | *s2;
        return dest;
    }
    FastBitSet& dointersect(const FastBitSet& added, FastBitSet& dest)
    {
        unsigned bitints = elems;
        if (added.elems < elems)
            bitints = added.elems;
        dest.resize(bitints);
        bitints += BitIntSize - 1;
        bitints /= BitIntSize;
        for (StoreType *d = dest.data, *s1 = added.data, *s2 = data; bitints--; d++, s1++, s2++)
            *d = *s1 & *s2;
        return dest;
    }
    FastBitSet& doinvert(FastBitSet& dest)
    {
        unsigned bitints = elems;
        dest.resize(bitints);
        bitints += BitIntSize - 1;
        bitints /= BitIntSize;
        for (StoreType *d = dest.data, *s1 = data; bitints--; d++, s1++)
            *d = ~*s1;
        return dest;
    }
    void clear() { clear(0, elems); }
    void resize(unsigned n)
    {
        int requested = n;
        n = (n + BitIntSize - 1) / BitIntSize * BitIntSize;
        if (n <= capacity)
        {
            clear(elems, requested);
            elems = requested;
        }
        else
        {
            int oldCapacity = capacity;
            capacity = n;
            elems = requested;
            StoreType* newData = new StoreType[n / BitIntSize];
            std::fill(newData, newData + capacity / BitIntSize, 0);
            if (data)
            {
                std::copy(data, data + oldCapacity / BitIntSize, newData);
                delete[] data;
            }
            data = newData;
        }
    }
    bool operator==(const FastBitSet& other)
    {
        if (elems != other.elems)
            return false;
        unsigned bitints = elems / BitIntSize;
        for (int i = 0; i < bitints; i++)
            if (data[i] != other.data[i])
                return false;
        int last = elems % BitIntSize;
        if (last)
        {
            StoreType mask = static_cast<StoreType>(-1ll) >> (BitIntSize - last);
            return (data[bitints] & mask) == (other.data[bitints] & mask);
        }
        return true;
    }
    bool operator!=(const FastBitSet& other) { return !operator==(other); }
    class BitIterator
    {
        FastBitSet* store;
        int pos;
        BitIterator(FastBitSet* data, int index) : store(data), pos(index)
        {
            if (store->data)
            {
                if (index == -1)
                    next();
            }
            else
            {
                pos = store->elems;
            }
        }

      public:
        BitIterator() : store(nullptr), pos(-1) {}
        BitIterator& operator++()
        {
            next();
            return *this;
        }
        bool operator!=(const BitIterator& right) const { return pos != right.pos || store != right.store; }
        bool operator==(const BitIterator& right) const { return !operator!=(right); }
        int& operator*() { return pos; }

      protected:
        void next(StoreType n, int index)
        {
            while (index < BitIntSize && pos < store->elems && !(n & 1))
            {
                ++pos;
                ++index;
                n >>= 1;
            }
        }
        void next()
        {
            if (pos == -1 || pos < store->elems)
            {
                ++pos;
                int shft = pos % BitIntSize;
                int index = pos / BitIntSize;
                StoreType n = store->data[index] >> shft;
                if (n)
                {
                    next(n, index);
                    return;
                }
                else
                {
                    pos += 32 - shft;
                }
                if (pos >= store->elems)
                {
                    pos = store->elems;
                    return;
                }
                for (int i = index + 1; i < store->elems / BitIntSize; i++, pos += 32)
                {
                    n = store->data[i];
                    if (n)
                    {
                        next(n, 0);
                        return;
                    }
                }
                n = store->data[store->elems / BitIntSize];
                if (n)
                {
                    next(n, 0);
                }
                else
                {
                    pos = store->elems;
                }
            }
        }
        friend class FastBitSet;
    };

    inline size_t size() const { return elems; }
    typedef BitIterator iterator;
    auto begin() { return BitIterator(this, -1); }
    auto end() { return BitIterator(this, elems); }

  protected:
    void clear(int first, int second)
    {
        if (first < second)
        {
            int startindex = first / BitIntSize;
            int startbit = first % BitIntSize;
            int stopindex = second / BitIntSize;
            int stopbit = second % BitIntSize;
            StoreType startmask = static_cast<StoreType>(-1ll) >> (BitIntSize - startbit);
            StoreType endmask = static_cast<StoreType>(-1ll) << stopbit;
            int fillstartindex = (first + BitIntSize - 1) / BitIntSize;
            int fillendindex = second / BitIntSize;
            if (startbit != 0)
                data[fillstartindex - 1] &= startmask;
            if (stopbit != 0)
                data[fillendindex] &= endmask;
            if (fillendindex > fillstartindex)
                std::fill(data + fillstartindex, data + fillendindex, 0);
        }
    }

  private:
    StoreType* data;
    size_t capacity;
    size_t elems;
};

class SimpleBitSet : public FastBitSet
{
  public:
    SimpleBitSet() { init(); }
    SimpleBitSet(unsigned n)
    {
        init();
        resize(n);
    }
    SimpleBitSet& operator=(const SimpleBitSet& other)
    {
        FastBitSet::operator=(other);
        return *this;
    }
    SimpleBitSet(SimpleBitSet&& other)
    {
        init();
        FastBitSet::operator=(other);
    }
    ~SimpleBitSet() { destroy(); }
};

// this is meant for scalars right now...
// this whole thing is because instantiation a vector of lists with the stl is very slow
// and since the vectors in question  are usually sparse anyway a speedup is helpful...
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
    inline void clear()
    {
        destroy();
        init();
    }
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
class SimpleList : public FastList<T>
{
  public:
    SimpleList() { FastList<T>::init(); }
    ~SimpleList() { FastList<T>::destroy(); }
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
