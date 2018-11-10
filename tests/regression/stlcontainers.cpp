#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <string>
#include <iostream>
main()
{
    std::vector<int> vec {5,4,3,2,1};
    std::deque<int> deq {5,4,3,2,1 };
    std::list<int> list {5,4,3,2,1 };
    std::set<int> set { 5,4,3,2,1};
    std::map<int, std::string> map { { 1, "hi"} , { 3, "try" }, { 2, "bye" }  };
    
    for (auto a : vec)
        std::cout << a << " ";
    std::cout << std::endl;
    for (auto a : deq)
        std::cout << a << " ";
    std::cout << std::endl;
    for (auto a : list)
        std::cout << a << " ";
    std::cout << std::endl;
    for (auto a : set)
        std::cout << a << " ";
    std::cout << std::endl;
    for (auto a : map)
    {
        std::cout << a.first << ":" << a.second << std::endl;
    }
    std::cout << std::endl;
}