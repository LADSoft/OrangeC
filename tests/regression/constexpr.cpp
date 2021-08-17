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


template <class T>
constexpr std::pair<T, T> xx(T a) {
	return std::pair<T, T>(a, a);
}
int main1()
{
														
	constexpr std::pair<int, int> aa(4,3);
	static_assert(aa.first == 4);

	static_assert(aa == std::pair<int,int>(4,3));

        constexpr char il[] = { 1,2,3,4 };
	constexpr std::pair<char*, char*> aa1(il+1,il+3);
	static_assert(*(aa1.first) == 2);



        constexpr auto p = xx(5);
	static_assert(p.first == 5);
	return 0;
}
int main()
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

constexpr int il1[] = { 2, 4, 6, 8, 7, 5, 3, 1 };
struct less { constexpr bool operator ()( const int &x, const int &y) const { return x < y; }};

int main2()
{
    constexpr auto p = std::minmax_element(il1, il1+8, less());
    static_assert ( *(p.first)  == 1, "" );
    static_assert ( *(p.second) == 8, "" );
    constexpr auto p1 = std::minmax_element(il1, il1+8);
    static_assert ( *(p1.first)  == 1, "" );
    static_assert ( *(p1.second) == 8, "" );
    return 0;
}

int main3()
{
    static_assert(std::min({3, 2, 1}) == 1, "");
    static_assert(std::min({2, 1, 3}) == 1, "");
    static_assert(std::min({1, 3, 2}) == 1, "");
    return 0;
}

int main4()
{
    constexpr static int x = 1;
    constexpr static int y = 0;
    constexpr auto p1 = std::minmax (x, y);
    static_assert(p1.first  == y, "");
    static_assert(p1.second == x, "");
    constexpr auto p2 = std::minmax (y, x);
    static_assert(p2.first  == y, "");
    static_assert(p2.second == x, "");
    return 0;
}

int main5()
{
    constexpr static int x = 1;
    constexpr static int y = 0;
    constexpr auto p1 = std::minmax(x, y, std::greater<>());
    static_assert(p1.first  == x, "");
    static_assert(p1.second == y, "");
    constexpr auto p2 = std::minmax(y, x, std::greater<>());
    static_assert(p2.first  == x, "");
    static_assert(p2.second == y, "");
    return 0;
}
