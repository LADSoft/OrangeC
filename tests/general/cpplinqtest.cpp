#include "cpplinq.hpp"
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace cpplinq;
auto is_prime = [](int i) -> bool 
{
    if (i < 2) { return false; }
    else if (i == 2) { return true; }
    else
    {
        auto r = (int)std::ceil (std::sqrt (i));
        for (auto iter = 2; iter <= r; ++iter)
        {
            if (i % iter == 0) { return false; }
        }
        return true;
    }
};

auto primes_as_strings(int n)
{
    auto primes = range(0, INT_MAX)
               >> where(is_prime)
               >> take(n)
               >> select([](int i) {std::stringstream s; s << i; return s.str();})
               >> to_vector();
 
   return primes;
}

int main()
{
    int numbers[] = {1,2,3,4,5,6,7,8,9};
    auto primes = from_array(numbers)
               >> where(is_prime)
               >> to_vector(); // yields {2,3,5,7}}
    for (auto a : primes)
        std::cout << a;
    std::cout << std::endl;

    auto primes2 = primes_as_strings(10);
    for (auto a : primes2)
        std::cout << a << " ";
    std::cout << std::endl;

    auto c = from_array(numbers)
             >> where([](int i) {return i%2==0;})
             >> count();
    std::cout << c << std::endl;
    
    auto m2 = from_array(numbers) >> max([](int i) {return i*i;});
    std::cout << m2 << std::endl;
    
}