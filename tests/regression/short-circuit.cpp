#include <stdio.h>
struct bb
{
	bool True = true;
	bool False = false;
	int x = 4;
	~bb() { printf("bbdest"); }
};
struct cc
{
	bool True = true;
	bool False = false;
	int x = 4;
	~cc() { printf("ccdest"); }
};
struct dd
{
	bool True = true;
	bool False = false;
	int x = 4;
	~dd() { printf("dddest"); }
};
struct ee
{
	bool True = true;
	bool False = false;
	int x = 4;
	~ee() { printf("eedest"); }
};
bb one;
bb bbfunc() 
{
	return one;
}
cc two;
cc ccfunc()
{
	return two;
}
dd three;
dd ddfunc()
{
	return three;
}
ee four;
ee eefunc()
{
	return four;
}
void func1()
{
        printf("false && false: ");
	if (one.False && two.False)
		printf("bad\n");
	else
		printf("good\n");
        printf("true && false: ");
	if (one.True && two.False)
		printf("bad\n");
	else
		printf("good\n");
        printf("false && true: ");
	if (one.False && two.True)
		printf("bad\n");
	else
		printf("good\n");
        printf("true && true: ");
	if (one.True && two.True)
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false && false): ");
	if (!(one.False && two.False))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true && false): ");
	if (!(one.True && two.False))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false && true): ");
	if (!(one.False && two.True))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true && true): ");
	if (!(one.True && two.True))
		printf("bad\n");
	else
		printf("good\n");


        printf("false || false: ");
	if (one.False || two.False)
		printf("bad\n");
	else
		printf("good\n");
        printf("true || false: ");
	if (one.True || two.False)
		printf("good\n");
	else
		printf("bad\n");
        printf("false || true: ");
	if (one.False || two.True)
		printf("good\n");
	else
		printf("bad\n");
        printf("true || true: ");
	if (one.True || two.True)
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false || false): ");
	if (!(one.False || two.False))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true || false): ");
	if (!(one.True || two.False))
		printf("bad\n");
	else
		printf("good\n");
        printf("!(false || true): ");
	if (!(one.False || two.True))
		printf("bad\n");
	else
		printf("good\n");
        printf("!(true || true): ");
	if (!(one.True || two.True))
		printf("bad\n");
	else
		printf("good\n");

}
void func2()
{
        printf("false && false: ");
	if (bbfunc().False && ccfunc().False)
		printf("bad\n");
	else
		printf("good\n");
        printf("true && false: ");
	if (bbfunc().True && ccfunc().False)
		printf("bad\n");
	else
		printf("good\n");
        printf("false && true: ");
	if (bbfunc().False && ccfunc().True)
		printf("bad\n");
	else
		printf("good\n");
        printf("true && true: ");
	if (bbfunc().True && ccfunc().True)
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false && false): ");
	if (!(bbfunc().False && ccfunc().False))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true && false): ");
	if (!(bbfunc().True && ccfunc().False))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false && true): ");
	if (!(bbfunc().False && ccfunc().True))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true && true): ");
	if (!(bbfunc().True && ccfunc().True))
		printf("bad\n");
	else
		printf("good\n");


        printf("false || false: ");
	if (bbfunc().False || ccfunc().False)
		printf("bad\n");
	else
		printf("good\n");
        printf("true || false: ");
	if (bbfunc().True || ccfunc().False)
		printf("good\n");
	else
		printf("bad\n");
        printf("false || true: ");
	if (bbfunc().False || ccfunc().True)
		printf("good\n");
	else
		printf("bad\n");
        printf("true || true: ");
	if (bbfunc().True || ccfunc().True)
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false || false): ");
	if (!(bbfunc().False || ccfunc().False))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true || false): ");
	if (!(bbfunc().True || ccfunc().False))
		printf("bad\n");
	else
		printf("good\n");
        printf("!(false || true): ");
	if (!(bbfunc().False || ccfunc().True))
		printf("bad\n");
	else
		printf("good\n");
        printf("!(true || true): ");
	if (!(bbfunc().True || ccfunc().True))
		printf("bad\n");
	else
		printf("good\n");

}

void func3()
{
        printf("false && false: ");
	if (bbfunc().x != ccfunc().x && ddfunc().x != eefunc().x)
		printf("bad\n");
	else
		printf("good\n");
        printf("true && false: ");
	if (bbfunc().x == ccfunc().x && ddfunc().x != eefunc().x)
		printf("bad\n");
	else
		printf("good\n");
        printf("false && true: ");
	if (bbfunc().x != ccfunc().x && ddfunc().x == eefunc().x)
		printf("bad\n");
	else
		printf("good\n");
        printf("true && true: ");
	if (bbfunc().x == ccfunc().x && ddfunc().x == eefunc().x)
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false && false): ");
	if (!(bbfunc().x != ccfunc().x && ddfunc().x != eefunc().x))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true && false): ");
	if (!(bbfunc().x == ccfunc().x && ddfunc().x != eefunc().x))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false && true): ");
	if (!(bbfunc().x != ccfunc().x && ddfunc().x == eefunc().x))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true && true): ");
	if (!(bbfunc().x == ccfunc().x && ddfunc().x == eefunc().x))
		printf("bad\n");
	else
		printf("good\n");


        printf("false || false: ");
	if (bbfunc().x != ccfunc().x || ddfunc().x != eefunc().x)
		printf("bad\n");
	else
		printf("good\n");
        printf("true || false: ");
	if (bbfunc().x == ccfunc().x || ddfunc().x != eefunc().x)
		printf("good\n");
	else
		printf("bad\n");
        printf("false || true: ");
	if (bbfunc().x != ccfunc().x || ddfunc().x == eefunc().x)
		printf("good\n");
	else
		printf("bad\n");
        printf("true || true: ");
	if (bbfunc().x == ccfunc().x || ddfunc().x == eefunc().x)
		printf("good\n");
	else
		printf("bad\n");
        printf("!(false || false): ");
	if (!(bbfunc().x != ccfunc().x || ddfunc().x != eefunc().x))
		printf("good\n");
	else
		printf("bad\n");
        printf("!(true || false): ");
	if (!(bbfunc().x == ccfunc().x || ddfunc().x != eefunc().x))
		printf("bad\n");
	else
		printf("good\n");
        printf("!(false || true): ");
	if (!(bbfunc().x != ccfunc().x || ddfunc().x == eefunc().x))
		printf("bad\n");
	else
		printf("good\n");
        printf("!(true || true): ");
	if (!(bbfunc().x == ccfunc().x || ddfunc().x == eefunc().x))
		printf("bad\n");
	else
		printf("good\n");

}
int main()
{
	printf("simple\n");
	func1();

	printf("\ncomplex\n");
	func2();

	printf("\nmixed\n");
	func3();
	printf("\ndone\n");
}