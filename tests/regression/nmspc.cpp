namespace aa
{
	enum bb { r,s,t};
	struct bb1 { int r,s,t; };
	int rr(bb ss)
 	{
		return 4;
	}
	int rr1(bb ss)
	{
		return 4;
	}
	int rr2(bb1 ss)
	{
		return 4;
	}
};


int main()
{
	struct aa::bb tt = aa::bb::s;
	rr(tt);
	rr1(aa::bb::r);
	struct aa::bb1 tt1;
	rr2(tt1);
}