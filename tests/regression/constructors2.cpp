#include <stdio.h>
struct a1
{
	int rr = 4;
	int flag = 4;
	~a1() 
	{
		if (rr == flag)
		printf("a1 ok\n"); 
	}
};
struct a2
{
	int rr = 4;
	int flag = 4;
        int tt = 77;
	a2() { tt = flag; }
	a2(int) : rr(5) { flag = 5; tt = flag; }
	a2(int, int) { rr = 5, flag = 5; tt = flag; } 
	~a2() 
	{
		if (rr == flag && tt == flag)
		printf("a2 ok\n"); 
	}
};
struct a2a
{
	int rr = 4;
	int flag = 4;
        int tt = 77;
	a2a() { tt = flag; }
	a2a(int) : rr(5) { flag = 5; tt = flag; }
	a2a(int, int) { rr = 5, flag = 5; tt = flag; } 
	a2a(const a2a&) { }         
	~a2a() 
	{
		if (rr == flag && tt == flag)
		printf("a2a ok\n"); 
	}
};
struct a2b
{
	int rr = 4;
	int flag = 4;
        int tt = 77;
	a2b() { tt = flag; }
	a2b(int) : rr(5) { flag = 5; tt = flag; }
	a2b(int, int) { rr = 5, flag = 5; tt = flag; } 
	a2b(const a2a&) : rr(6), flag(6){ tt = flag; }         
	~a2b() 
	{
		if (rr == flag && tt == flag)
		printf("a2b ok\n"); 
	}
};
struct a2c
{
	int rr = 4;
	int flag = 4;
        int tt = 4;
	a2c() { }
	a2c(const a2c&) { }        
	~a2c() 
	{
		if (rr == flag && tt == flag)
		printf("a2c ok\n"); 
	}
};

struct a2d
{
	int rr = 4;
	int flag = 4;
        int tt;
	a2d() : tt(4) { }
	a2d(const a2c&) { }        
	~a2d() 
	{
		if (rr == flag && tt == flag)
		printf("a2d ok\n"); 
	}
};
struct a2e
{
	int rr = 4;
	int flag = 4;
        int tt;
	a2e() : tt(77) { }
	a2e(const a2c&) { }        
	~a2e() 
	{
		if (rr == flag && tt == flag)
		printf("a2e ok\n"); 
	}
};
struct b1
{
	b1() {printf("b1()\n"); }
	b1(const b1&) { printf("b1(const b1&)\n"); }
	b1(int) { printf("b1(int)\n"); }
};
struct b2
{
	b2() {printf("b2()\n"); }
	b2(const b2&) { printf("b2(const b2&)\n"); }
	b2(int) { printf("b2(int)\n"); }
};
struct c1
{
	c1() {printf("c1()\n"); }
	c1(const c1&) { printf("c1(const c1&)\n"); }
	c1(int) { printf("c1(int)\n"); }
};

struct d1 : b1, b2
{
	c1 aa;
	d1() { }
	d1(const d1 &) { }
	d1(int) { }
};
struct d2 : b1, b2
{
	c1 aa;
	d2() : b1(5) { }
	d2(const b1 &old) : b1(old) { }
	d2(int) : b1(5), aa(7) { }
};
struct d3 : b1, b2
{
	c1 aa;
	d3() : b1(5) { }
	d3(const d3 &old) : b1(old) { }
	d3(int) : b1(5) { }

};
struct d4 : b1, b2
{
	c1 aa;
	d4() = default;
	d4(const d4 &) = default;
	d4(int) { }
};
struct d5 : b1, b2
{
	c1 aa;
	d5() : b1(5) { }
	d5(const b1 &old) : b1(old) { }
	d5(const d5 &old) { }
	d5(int) : b1(5), aa(7) { }
};

int main()
{
	a1 x1;
	a2 x2a, x2a1(x2a);
	a2 x2ab(10), x2ab1(x2ab);
	a2 x3ab(10,10), x3ab1(x3ab);
	a2a x2aa, x2aa1(x2aa);
	a2a x2aab(10), x2aab1(x2aab);
	a2a x3aab(10,10), x3aab1(x3aab);
	a2b x2ba, x2ba1(x2ba);
	a2b x2bab(10), x2bab1(x2bab);
	a2b x3bab(10,10), x3bb1(x3bab);
	a2c x2ca, x2ca1(x2ca);
	a2d x2da, x2da1(x2da);
	a2 x2ea, x2ea1(x2ea);

	printf("d1\n");
	d1 d1a, d1a1(d1a), d1b(5);
	printf("d2\n");
	d2 d2a, d2a1(d2a), d2b(5);
	printf("d3\n");
	d3 d3a, d3a1(d3a), d3b(5);
	printf("d4\n");
	d4 d4a, d4a1(d4a), d4b(5);
	printf("d5\n");
	d5 d5a, d5a1(d5a), d5b(5);
}