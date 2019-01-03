#include <stdio.h>
struct aa
{
	aa(char *b) :var(b) { }
	void printme() { printf("%s\n", var); }
	char *var;
};
void zz1()
{
	throw new aa("one");
}
void zz2()
{
	aa var("two");
	throw var;
}
void zz3()
{
	throw 548;
}
main()
{
	try
	{
		zz1();
	}
	catch (aa *b)
	{
		b->printme();
		delete b;
	}
	try
	{
		zz2();
	}
	catch (aa &b)
	{
		b.printme();
	}
	try
	{
		zz3();
	}
	catch (int c)
	{
		printf("%d\n", c);
	}

}