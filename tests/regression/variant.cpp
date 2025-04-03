#include <variant>
#include <string>
#include <iostream>
#include <cassert>

int main()
{
    std::variant<int, float> v, w;
    v = 42; // v contains int
    int i = std::get<int>(v);
    assert(42 == i); // succeeds
    w = std::get<int>(v);
    w = std::get<0>(v); // same effect as the previous line
    w = v; // same effect as the previous line
    i = std::get<int>(w);
    assert (42 == i);
 
    assert(std::holds_alternative<int>(w)); // succeeds
    assert(!std::holds_alternative<float>(w)); // succeeds

	v = 15.75F;
float f = std::get<float>(v);
    assert(15.75 == f);
    assert(std::holds_alternative<float>(v)); // succeeds

//  std::get<double>(v); // error: no double in [int, float]
//  std::get<3>(v);      // error: valid index values are 0 and 1
 
    try
    {
        std::get<float>(w); // w contains int, not float: will throw
    }
    catch (const std::bad_variant_access& ex)
    {
        std::cout << ex.what() << '\n';
    }
 
    using namespace std::literals;

    std::variant<std::string> x("abc");
    // converting constructors work when unambiguous
    x = "def"; // converting assignment also works when unambiguous
 
    auto s = std::get<0>(x);
    assert(!strcmp(s.c_str(), "def"));
    std::variant<std::string, void const*> y("abc");
    // casts to void const* when passed a char const*
    assert(std::holds_alternative<void const*>(y)); // succeeds
printf("hi\n");
    y = "xyz"s;
    assert(std::holds_alternative<std::string>(y)); // succeeds
}