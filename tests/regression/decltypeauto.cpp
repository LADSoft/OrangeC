#include <string>

template <class T>
decltype(auto) byVal(T a) { return a; }

template <class T>
decltype(auto) byRef(T& a) { return a; }
int main()
{
	std::string sval = "1";
	int ival = 10;

	decltype(auto) srval = byVal(sval);
	decltype(auto) irval = byVal(ival);
	sval += "1";
	ival *= 2;
	printf("%d:%s\n", irval, srval.c_str());

	decltype(auto) srval2 = byRef(sval);
	decltype(auto) irval2 = byRef(ival);
	sval += "3";
	ival *= 2;
	printf("%d:%s\n", irval2, srval2.c_str());

}