#include <variant>
#include <string>
#include <memory>
#include <vector>
#include <cassert>
typedef std::variant<std::shared_ptr<int>, int> aa;

struct visitor_struct
{
    std::string operator()(std::shared_ptr<int> token) { return std::to_string(*token); }
    std::string operator()(int token) { printf("xx\n") ;return std::to_string(token); }
};

int main()
{

	visitor_struct visitor;
	aa bb(5);
	assert( std::get<int>(bb) == 5);
        auto x = std::visit(visitor,bb);
	assert(x == "5");

	aa dd = std::make_shared<int>(0);
	dd = bb;
	assert( std::get<int>(dd) == 5);
        auto xd = std::visit(visitor,dd);
	assert(xd == "5");

	std::vector<aa> cc;
	cc.push_back(10);

	assert(std::get<int>(cc.back()) == 10);

	auto y = std::visit(visitor, cc[0]);
        assert(y == "10");

	cc.back() = 20;

	assert( std::get<int>(cc.back())== 20);
	y = std::visit(visitor, cc[0]);
	assert(y == "20");

}