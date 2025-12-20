#include <string>
#include <array>
int main()
{
    using namespace std::string_literals;
    std::array arr = {"cmd"s, "cmd.exe"s, "command"s, "command.com"s};
    for (auto&& val : arr)
	printf("%s\n", val.c_str());
	printf("%s\n",arr[0].c_str());
}