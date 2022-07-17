#pragma once
#include <cstdint>
#include <cstring>
namespace OrangeC
{
namespace Utils
{
// WARNING: unbounded strings and non-null-terminated strings *DO NOT WORK* here.
// AKA: Don't use multibyte strings, it's possible to do with _tcslen or _tcslen_l
template <typename T, T FNV_prime, T FNV_offset>
class fnv1a_class
{
  public:
    T operator()(const char* arr) const
    {
        // Follows the Fowler-Noll-Vol hash function as described by wikipedia in the following article:
        // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
        unsigned char* arr2 = (unsigned char*)arr;
        T hash = FNV_offset;
        for (size_t i = 0; arr[i] != '\0';
             i++)  // This optimizes away a call to strlen since our optimizer isn't smart enough to perform this IIRC
        {
            hash = (hash * FNV_prime) ^ arr2[i];
        }
        return hash;
    }
};
class str_eql
{
  public:
    bool operator()(const char* a, const char* b) const { return !strcmp(a, b); }
};
using fnv1a64 = fnv1a_class<uint64_t, UINT64_C(1099511628211), UINT64_C(14695981039346656037)>;
using fnv1a32 = fnv1a_class<uint32_t, UINT32_C(16777619), UINT32_C(2166136261)>;
}  // namespace Utils
}  // namespace OrangeC