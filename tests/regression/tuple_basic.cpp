#include <stdio.h>
#include <tuple>
int main()
{
    auto t = std::make_tuple(1, 123, 142312);
	printf("%d\n", std::get<1>(t));
    auto t1 = std::make_tuple(1.1, 123.123, 142312.142312);
	printf("%f\n", std::get<1>(t1));

    std::tuple<int,double,int> t2 = { 1,123.124, 5};
	printf("%f\n", std::get<1>(t2));
}