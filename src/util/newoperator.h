#include <cstdlib>
#include "Utils.h"
#pragma pack(4)


namespace
{
static const int MaxFixedPoolSize = 512;
static const int BucketSize = 128000;

class FixedPoolBase
{
public:
    static const int sig = 0xbeef0000;
    static const int sigMask = 0xffff0000;

    FixedPoolBase() {}
    virtual void Clear() = 0;
    virtual void* Alloc() = 0;
    virtual void Release(void* block) = 0;
};

template <int allocSize, int bucketSize>
class FixedPool : public FixedPoolBase
{
public:
    static const int elems = (bucketSize / allocSize);
    static const int blockSize = elems * allocSize;

    void Clear() override
    {
        while (currentData)
        {
            auto current = currentData;
            currentData = currentData->next;
            free(current);
        }
    }
    void* Alloc() override
    {
        void* rv = nullptr;
        if (freeList)
        {
            auto current = freeList;
            freeList = freeList->next;
            current->tag = allocSize | sig;
            rv = &current->data;
        }
        else
        {
            if (currentData == nullptr || nextElem >= elems)
            {
                auto block = (ListBlock*)malloc(sizeof(ListBlock));
                if (!block) return nullptr;
                block->next = currentData;
                currentData = block;
                nextElem = 0;
            }
            auto current = &currentData->data[nextElem++];
            current->tag = allocSize | sig;
            rv = &current->data;
        }
        return rv;
    }
    void Release(void* block) override
    {
        unsigned* data = reinterpret_cast<unsigned*>(block) - 1;
        if (*data == (allocSize | sig))
        {
            ListItem* item = reinterpret_cast<ListItem*>(data);
            item->next = freeList;
            freeList = item;
        }
    }
private:
    union ListItem
    {
        ListItem* next;
        struct
        {
            unsigned tag;
            unsigned char data[allocSize];
        };
    };
    struct ListBlock
    {
        ListBlock* next;
        ListItem data[elems];
    };
    int nextElem = 0;
    ListBlock* currentData = nullptr;
    ListItem* freeList = nullptr;
};
static bool initted;
class MemoryPools
{
    static const int MaxFixedPools = MaxFixedPoolSize / 8;
public:
    MemoryPools() { }
    ~MemoryPools() { Clear(); }
    void Clear()
    {
        for (int i = 0; i < MaxFixedPools; i++)
        {
            fixedPools[i]->Clear();
            delete fixedPools[i];
        }
    }
    void* Alloc(size_t size)
    {
        if (!initted) return malloc(size);

        if (size == 0)  return nullptr;
        unsigned pool = 0;
        if (size > 4)
            pool = 1 + (size - 4) / 8;
        if (pool < MaxFixedPools)
            return fixedPools[pool]->Alloc();
        else
            return malloc(size);
    }
    void Release(void* block)
    {
        if (!initted)
        {
            free(block);
            return;
        }
        if (block == nullptr) return;
        unsigned* data = reinterpret_cast<unsigned*>(block) - 1;
        if ((*data & FixedPoolBase::sigMask) == FixedPoolBase::sig)
        {
            auto size = (*data & ~FixedPoolBase::sigMask);
            auto pool = 0;
            if (size > 4)
                pool = 1 + (size - 5) / 8;
            if (pool < MaxFixedPools)
                fixedPools[pool]->Release(block);
            else
                Utils::Fatal("internal error");
        }
        else
        {
            free(block);
        }
    }
private:
FixedPoolBase* fixedPools[MaxFixedPools] =
{
    new FixedPool<4, BucketSize>(),
    new FixedPool<12, BucketSize>(),
    new FixedPool<20, BucketSize>(),
    new FixedPool<28, BucketSize>(),
    new FixedPool<36, BucketSize>(),
    new FixedPool<44, BucketSize>(),
    new FixedPool<52, BucketSize>(),
    new FixedPool<60, BucketSize>(),
    new FixedPool<68, BucketSize>(),
    new FixedPool<76, BucketSize>(),
    new FixedPool<84, BucketSize>(),
    new FixedPool<92, BucketSize>(),
    new FixedPool<100, BucketSize>(),
    new FixedPool<108, BucketSize>(),
    new FixedPool<116, BucketSize>(),
    new FixedPool<124, BucketSize>(),
    new FixedPool<132, BucketSize>(),
    new FixedPool<140, BucketSize>(),
    new FixedPool<148, BucketSize>(),
    new FixedPool<156, BucketSize>(),
    new FixedPool<164, BucketSize>(),
    new FixedPool<172, BucketSize>(),
    new FixedPool<180, BucketSize>(),
    new FixedPool<188, BucketSize>(),
    new FixedPool<196, BucketSize>(),
    new FixedPool<204, BucketSize>(),
    new FixedPool<212, BucketSize>(),
    new FixedPool<220, BucketSize>(),
    new FixedPool<228, BucketSize>(),
    new FixedPool<236, BucketSize>(),
    new FixedPool<244, BucketSize>(),
    new FixedPool<252, BucketSize>(),
    new FixedPool<260, BucketSize>(),
    new FixedPool<268, BucketSize>(),
    new FixedPool<276, BucketSize>(),
    new FixedPool<284, BucketSize>(),
    new FixedPool<292, BucketSize>(),
    new FixedPool<300, BucketSize>(),
    new FixedPool<308, BucketSize>(),
    new FixedPool<316, BucketSize>(),
    new FixedPool<324, BucketSize>(),
    new FixedPool<332, BucketSize>(),
    new FixedPool<340, BucketSize>(),
    new FixedPool<348, BucketSize>(),
    new FixedPool<356, BucketSize>(),
    new FixedPool<364, BucketSize>(),
    new FixedPool<372, BucketSize>(),
    new FixedPool<380, BucketSize>(),
    new FixedPool<388, BucketSize>(),
    new FixedPool<396, BucketSize>(),
    new FixedPool<404, BucketSize>(),
    new FixedPool<412, BucketSize>(),
    new FixedPool<420, BucketSize>(),
    new FixedPool<428, BucketSize>(),
    new FixedPool<436, BucketSize>(),
    new FixedPool<444, BucketSize>(),
    new FixedPool<452, BucketSize>(),
    new FixedPool<460, BucketSize>(),
    new FixedPool<468, BucketSize>(),
    new FixedPool<476, BucketSize>(),
    new FixedPool<484, BucketSize>(),
    new FixedPool<492, BucketSize>(),
    new FixedPool<500, BucketSize>(),
    new FixedPool<508, BucketSize>(),
};
};

static MemoryPools* memPool;
} // anonymous namespace

void* operator new(std::size_t __sz)
{
    static bool initting = false;
    if (!memPool) 
    {
        if (!initting)
        {
            initting = true;
            memPool = new MemoryPools;
            initted = true;
        }
        else
        {
            return malloc(__sz);
        }
    }
    void* rv = memPool->Alloc(__sz);
    if (!rv)
        Utils::Fatal("out of memory (operator new): %d",__sz);
    return rv;
}

void  operator delete(void* __p) noexcept
{
    if (initted)
        return memPool->Release(__p);
    else
        return free(__p);
}

void ReleaseMemoryPools()
{
    initted = false;
    delete memPool;
}