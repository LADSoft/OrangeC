#include <iostream>
#include <vector>
#include <map>
#include <Stdio.h>
#include <string>
template<typename T>
T adder(T v) {
  return v;
}

template<typename T, typename... Args>
T adder(T first, Args... args) {
  return first + adder(args...);
}

template <typename T, typename... Args>
void printer(T first,Args... args)
{
    printf(first, args...);
}
template<typename T>
bool pair_comparer(T a, T b) {
  return a == b;
}

template<typename T, typename... Args>
bool pair_comparer(T a, T b, Args... args) {
  return a == b && pair_comparer(args...);
}

template <template <typename, typename...> class ContainerType,
          typename ValueType, typename... Args>
void print_container(const ContainerType<ValueType, Args...>& c) {
  for (const auto& v : c) {
    std::cout << v << ' ';
  }
  std::cout << '\n';
}

// Implement << for pairs: this is needed to print out mappings where range
// iteration goes over (key, value) pairs.
template <typename T, typename U>
std::ostream& operator<<(std::ostream& out, const std::pair<T, U>& p) {
  out << "[" << p.first << ", " << p.second << "]";
  return out;
}

int main()
{
    printer("hi %d %d %d\n", 4,5,6);
    std::cout << pair_comparer(5,5,7,8) << std::endl;
    std::cout << pair_comparer(5,5,7,7) << std::endl;
    long sum = adder(1, 2, 3, 8, 7);
    std::cout << sum << std::endl;   
    std::string s1 = "x", s2 = "aa", s3 = "bb", s4 = "yy";
    std::string ssum = adder(s1, s2, s3, s4);
    std::cout << ssum << std::endl;   

    std::vector<int> t1 { 1,2,3,4,5 };
    print_container(t1);
    std::map<int, char *> t2 { {5, "hi"}, {4, "bye"}, {1, "try" }};
    print_container(t2);
}