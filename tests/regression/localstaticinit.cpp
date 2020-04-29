#include <map>
#include <string>

using namespace std;

int brokenfunc()
{
     static map<string, int> localvar = { { "one", 1}, { "two", 2} };
     for (auto v : localvar)
	printf("%s %d\n", v.first.c_str(), v.second);
}

main()
{
	for (int i=0; i < 10; i++)
		brokenfunc();
}