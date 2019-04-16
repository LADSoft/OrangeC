#include <cstdint>
#include <string>

// Shamelessly stolen from https://stackoverflow.com/a/34597785 to help prevent the "cast to std::string" speed problem
// https://stackoverflow.com/users/1392132/5gon12eder here's the user 5gon12eder who posted this solution.
// 
std::size_t
hash_bytes(const void * data, std::size_t size) noexcept;
template <typename T>
struct myhash
{
  std::size_t
  operator()(const T& obj) const noexcept
  {
    // Fallback implementation.
    auto hashfn = std::hash<T> {};
    return hashfn(obj);
  }
};
template <>
struct myhash<std::string>
{
  std::size_t
  operator()(const std::string& s) const noexcept
  {
    return hash_bytes(s.data(), s.size());
  }
};

template <>
struct myhash<const char *>
{
  std::size_t
  operator()(const char *const s) const noexcept
  {
    return hash_bytes(s, std::strlen(s));
  }
};
template <typename ResultT, ResultT OffsetBasis, ResultT Prime>
class basic_fnv1a final
{

  static_assert(std::is_unsigned<ResultT>::value, "need unsigned integer");

public:

  using result_type = ResultT;

private:

  result_type state_ {};

public:

  constexpr
  basic_fnv1a() noexcept : state_ {OffsetBasis}
  {
  }

  constexpr void
  update(const void *const data, const std::size_t size) noexcept
  {
    const auto cdata = static_cast<const unsigned char *>(data);
    auto acc = this->state_;
    for (auto i = std::size_t {}; i < size; ++i)
      {
        const auto next = std::size_t {cdata[i]};
        acc = (acc ^ next) * Prime;
      }
    this->state_ = acc;
  }

  constexpr result_type
  digest() const noexcept
  {
    return this->state_;
  }

};
using fnv1a_32 = basic_fnv1a<std::uint32_t,
                             UINT32_C(2166136261),
                             UINT32_C(16777619)>;

using fnv1a_64 = basic_fnv1a<std::uint64_t,
                             UINT64_C(14695981039346656037),
                             UINT64_C(1099511628211)>;
template <std::size_t Bits>
struct fnv1a;

template <>
struct fnv1a<32>
{
  using type = fnv1a_32;
};

template <>
struct fnv1a<64>
{
  using type = fnv1a_64;
};
template <std::size_t Bits>
using fnv1a_t = typename fnv1a<Bits>::type;

constexpr std::size_t
hash_bytes(const void *const data, const std::size_t size) noexcept
{
  auto hashfn = fnv1a_t<CHAR_BIT * sizeof(std::size_t)> {};
  hashfn.update(data, size);
  return hashfn.digest();
}
