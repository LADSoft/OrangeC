#include <stdio.h>
template <class T>
struct aa1
{
	aa1(T) { printf("aa1 constructor\n"); }
};
template <class T>
struct bb1
{
	bb1(T, T) { printf("bb1 constructor\n"); }
};
template <class T, class ...bases>
class cc1 : bases...
{
	using bases::bases...;
};

int main2()
{
        cc1<int, aa1<int>,bb1<int>> rr3(5,5);
	return 0;
}

struct aa
{
	aa(int) { printf("aa constructor\n"); }
	int operator()(int) { printf("aa operator()\n"); return 0; }
};
struct bb
{
	bb(int, float) {printf("bb construct or\n"); }
	double operator()(double) { printf("bb operator()\n"); return 0; }
};

template <class ...bases>
struct cc : bases...
{
	using bases::operator()...;
	using bases::bases...;
};
int main()
{
	cc<aa,bb> rr(5);
	cc<aa,bb> ss(5, 5.0);
	rr(5);
	rr(5.0);
	main2();
}
