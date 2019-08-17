#include <memory>
class Val
{
    public:
    Val(int lin) : line(lin) {printf("%d\n", lin);}
    int line;
};
int main()
{
static int a = 66;
    std::shared_ptr<Val> t = std::make_shared<Val>(a);
	printf("%d\n", t->line);
	printf("%d",std::make_shared<Val>(66)->line);
}