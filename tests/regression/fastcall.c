struct nn
{
	int q,v,a;
};

int __fastcall zz(int r, int s, struct nn z)
{
}
void kk()
{
}
int __fastcall vv(int r,int s, int t, int u, struct nn z)
{
	kk();
	return zz(s,r,z) + t + u;
}
int qq(int r,int s, int t, int u, struct nn z)
{
	return r + s + t + u;
}
zz1()
{
	struct nn r;
	printf("%d", vv(4,5,6,7,r) + qq(4,5,6,7, r));
}
int main()
{
	zz1();
	return 0;
}