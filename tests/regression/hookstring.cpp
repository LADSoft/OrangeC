#include <string>
#include <cstdio>

using namespace std;

main(int argc, char **argv)
{
	string aa = "hi";	
	aa = !argc ? "hello" : aa;
	printf("%s\n", aa.c_str());
}