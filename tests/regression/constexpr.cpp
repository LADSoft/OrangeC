#include <stdio.h>
#include <algorithm>

constexpr int il[] = { 2, 4, 6, 8, 7, 5, 3, 1 };

constexpr int bb(int a)
{
    return a + 1;
}
	constexpr int c = 4;
	constexpr int b = c + bb(c);

struct aa
{
	constexpr aa(int r) : _r(r), _s(0) { }
	constexpr int func(int s)
	{
		_s = s;
		return _r + _s + _t;
	}
	int _r;
	int _s;
	int _t = 6;
};

constexpr int aa1(int b)
{
	int rv = 0;
	while (b--)
		rv += b+1;
	return rv;
}
constexpr int bb1(bool b)
{
	if (b)
		return 5;
	else
		return 4;
}
constexpr int cc1(int z)
{
	int a = z + 4;
	return a;
}

constexpr int nn1()
{
   constexpr int aa[] = { 1,2,3,4,5 };
   int rv = 0;
   for (int i=0; i < sizeof(aa)/sizeof(aa[0]); i++)
       rv += aa[i];
   return rv;
}

main()
{
	static_assert(b == 9);
	aa nn(4);
        static_assert(nn.func(5) == 15);
        static_assert(aa1(4) == 10);
        static_assert(bb1(true) == 5);
        static_assert(bb1(false) == 4);
        static_assert(cc1(21) == 25);
        static_assert(nn1() == 15);

    constexpr int x = 1;
    constexpr int y = 0;
    static_assert(std::max(x, y) == x, "" );
    static_assert(std::max(y, x) == x, "" );

    constexpr auto p = std::max_element(il,il+8);
    static_assert ( *p == 8, "" );

}