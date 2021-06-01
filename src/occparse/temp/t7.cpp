#include <functional>


struct cc
{
	int operator()(int*, int*);
};

#ifdef __ORANGEC__
#define invoke __invoke
#endif

typedef int*pint;
int dd(int*, int*);
int main(int argc)
{
	printf("%d\n", std::__invokable_r<int, cc, int*, int*>::value);
	printf("%d\n", std::__invokable_r<int, cc, int, int>::value);
	printf("%d\n", std::__invokable_r<void, cc, int*, int*>::value);
	printf("%d\n", std::__invokable_r<void, cc, int, int>::value);
	printf("%d\n", std::__invokable_r<int, decltype(dd), int*, int*>::value);
	printf("%d\n", std::__invokable_r<int, decltype(dd), int, int>::value);
	std::function<bool(int* , int*)> bb = [](int* a, int* b) { printf("hi"); return true; };
//	std::__invokable_r<int, decltype(dd), int, int>::_Result aa1;
//	bb(&argc, &argc);
//	cc aa;
//	invoke(aa, pint(), pint());
}