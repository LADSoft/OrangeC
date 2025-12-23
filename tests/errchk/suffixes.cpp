#include <string>
std::string operator"" _s(const char* s, size_t sz) { return std::string(s, sz); }
std::string operator"" _s3(unsigned long long int val) { return std::to_string(val); }
int main()
{
    std::string s = 12_s;
    std::string s2 = 12_s2;
    std::string s3 = 1.2_s3;

    return 0;
}