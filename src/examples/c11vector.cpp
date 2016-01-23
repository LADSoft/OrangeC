#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;
 
struct Sum {
    Sum() { sum = 0; }
    void operator()(int n) { sum += n; }
 
    int sum;
};
 
int main()
{
    std::vector<int> nums{3, 4, 2, 9, 15, 267};
    std::cout << "before: ";
    for (int n : nums) {
        std::cout << n << " ";
    }
    std::cout << '\n';
 
    std::for_each(nums.begin(), nums.end(), [](int &n){ n++; });
 
    // Calls Sum::operator() for each number
    Sum s = std::for_each(nums.begin(), nums.end(), Sum());
 
    std::cout << "after:  ";    
    for (int n : nums) {
        std::cout << n << " ";
    }
    std::cout << '\n';
    std::cout << "sum: " << s.sum << '\n';
}