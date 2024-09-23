namespace tt
{
struct aa
{
	using q = int;
	typedef int r;
	void nn()  { printf("tt::aa::nn\n"); }
};
}
struct bb
{
	bb(int) { printf("bb constructor\n"); }
};

struct cc
{
	int operator()(int) { printf("cc operator()\n"); return 0;}
};

struct dd : tt::aa, bb, cc
{
	using tt::aa::nn;
	using bb::bb;
	using cc::operator();
	using tt::aa::q;
	using tt::aa::r;
};


int main()
{
	dd rr(6);
	rr.nn();
	rr(5);
	dd::q aa;
	dd::r bb;
}