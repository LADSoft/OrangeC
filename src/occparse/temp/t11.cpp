#include <stdio.h>
#include <map>
std::map<char *, char *> aa;
int main()
{
	std::map<char *, char *> ss { {"r","s"}, {"a", "b"}};
	aa = ss;
	std::map<char *, char *> ss1 { {"r1","s1"}, {"a1", "b1"}};

	aa = ss1;


	for (auto t : aa)
		printf("%s\n", t.first);
}