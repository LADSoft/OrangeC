#include <stdio.h>	
#include <memory>
int& z()
{
	static int a = 3;
	return a;
}

int main()
{
	auto t = std::forward_as_tuple(z());
	printf("%d %d\n", std::get<0>(t), z());
}