#include <stdio.h>
void caller(void (*func)(int))
{
	func(5);
}
main()
{
	auto aa = [](int x) { printf("%d\n", x); };
	aa(7);
        void (*xx)(int) = aa;
	xx(10);
	caller(aa);

}