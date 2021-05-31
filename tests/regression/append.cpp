#include <string>

std::string rv = "hi dave ";
main(int slashcount)
{
	rv.append(slashcount * 2 + 1, L't');
	printf("%s\n", rv.c_str());
}
