#define printme(aa, bb...) { printf(aa, bb); }

main()
{
	printme("%d %d %d %d", 1,2,3,4);
}