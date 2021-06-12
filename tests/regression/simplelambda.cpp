#include <functional>


struct cc
{
	int operator()(int*, int*);
};


typedef int*pint;
int dd(int*, int*);
int main(int argc)
{
	std::function<bool(int* , int*)> bb = [](int* a, int* b) { printf("hi%d %d", *a, *b); return true; };
	bb(&argc, &argc);
}