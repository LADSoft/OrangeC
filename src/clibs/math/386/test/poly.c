long double xpoly[3] = {2.0,3.0,4.0};
long double xpolevl[3] = {4.0,3.0,2.0};
long double x1evl[2] = { 3.0, 2.0};
long double ypolevl[3] = { 1.0, 3.0, 2.0 };

long double polyl(long double, int, void *);
long double polevll(long double, void *, int);
long double p1evll(long double, void *, int);
main()
{
	printf("%Lf,",polyl(2.0, 2, xpoly));
	printf("%Lf\n",polevll(2.0, xpolevl, 2));
	printf("%Lf,",polevll(2.0, ypolevl, 2));
	printf("%Lf\n", p1evll(2.0, x1evl, 2));
}