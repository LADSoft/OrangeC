
#include <__config>
int aaa = __has_builtin(__type_pack_element);
#include <stdio.h>

template <int I, class ...T>
using tryme = __type_pack_element<I, T...>;

main()
{
	tryme<0, int, double, float> a1 = 44;
	printf("%x %x\n", a1, 0);
	tryme<1, int, double, float> a2 = 44.3;
	printf("%x %x\n", a2, 0);
	tryme<2, int, double, float> a3 = 44.3;
	printf("%x %x\n", a3, 0);
}