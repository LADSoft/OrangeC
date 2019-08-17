void printf(char *, ...);
void exit(int);
int p=35;
int *x=&p;
int qq()
{
	int i;
    for (i=0; i < 10; i++)
    {
        p = i;
        printf("%d ", i);
    }
    return 27;
}
main()
{
    if (qq() > 10)
        printf("\nhello world %x %x",qq(), p);
    return 0;
}