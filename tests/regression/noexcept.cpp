#include <stdio.h>
#include <type_traits>

class bb1
{
public:
	bb1() = default;
};
class bb2
{
public:
	bb2() noexcept { };
};
class bb3
{
public:
	bb3() { };
};
class aa1 : bb1
{
public:
	aa1() = default;
	aa1(int) { printf("hi"); }
	int r, s, t;
};
class aa2 : bb2
{
public:
	aa2() = default;
	aa2(int) { printf("hi"); }
	int r, s, t;
};
class aa3 : bb3
{
public:
	aa3() = default;
	aa3(int) { printf("hi"); }
	int r, s, t;
};
class aa4 : bb3
{
public:
	aa4() noexcept { };
	aa4(int) { printf("hi"); }
	int r, s, t;
};
class aa5 : bb3
{
public:
	aa5() { };
	aa5(int) { printf("hi"); }
	int r, s, t;
};
main()
{
	printf("%d", std::is_nothrow_constructible<bb1>::value);
	printf("%d", std::is_nothrow_constructible<bb2>::value);
	printf("%d", std::is_nothrow_constructible<bb3>::value);
	printf("%d", std::is_nothrow_constructible<aa1>::value);
	printf("%d", std::is_nothrow_constructible<aa2>::value);
	printf("%d", std::is_nothrow_constructible<aa3>::value);
	printf("%d", std::is_nothrow_constructible<aa4>::value);
	printf("%d", std::is_nothrow_constructible<aa5>::value);
}