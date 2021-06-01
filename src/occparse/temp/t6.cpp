#include <deque>
#include <memory>

struct xx
{
	xx(int, int) { }
};

struct yy : xx
{
	yy (int a, int b) : xx(a,b) { }
};
std::deque<std::unique_ptr<xx>> aa;
main()
{
//	aa.push_back(std::make_unique<yy>(4,5));
	printf("%d\n", std::is_convertible<typename std::unique_ptr<yy, std::default_delete<yy>>::pointer, std::unique_ptr<xx>::pointer>::value);
	printf("%d\n", std::is_convertible<std::default_delete<yy>, std::default_delete<xx>>::value);
        printf("%d\n", std::is_array<yy>::value);
}