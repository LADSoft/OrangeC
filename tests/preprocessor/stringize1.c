#define hello(x,y) printf("this is a long string to get the stringizing later in the macro %s %s", # x, # y);

main()
{
	hello("hi", "dave");
	return 0;
}