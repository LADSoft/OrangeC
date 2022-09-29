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
template <typename T, T FNV_prime, T FNV_offset, int N>
class fnv1a_binary
{
  public:
    T operator()(const void* arr) const
    {
        const unsigned char* arr2 = (const unsigned char *)arr;
        // Follows the Fowler-Noll-Vol hash function as described by wikipedia in the following article:
        // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
        T hash = FNV_offset;
        for (size_t i = 0; i < N; i++)
        {
            hash = (hash * FNV_prime) ^ arr2[i];
        }
        return hash;
    }
};
template <int N>
class bin_eql
{
  public:
    bool operator() (const void* a, const void* b) const { return !memcmp(a, b, N); }
};
using fnv1a64 = fnv1a_class<uint64_t, UINT64_C(1099511628211), UINT64_C(14695981039346656037)>;
using fnv1a32 = fnv1a_class<uint32_t, UINT32_C(16777619), UINT32_C(2166136261)>;
template <int N>
using fnv1a64_binary = fnv1a_binary<uint64_t, UINT64_C(1099511628211), UINT64_C(14695981039346656037), N>;
template <int N>
using fnv1a32_binary = fnv1a_binary<uint32_t, UINT32_C(16777619), UINT32_C(2166136261), N>;
}  // namespace Utils
}  // namespace OrangeC