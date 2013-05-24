char *str1 = "365/";
char *str2 = " 445";
char *str3 = "-45";
char *str4 = "+33";
char *str5 = "077";
char *str6 = "0x73";


void check(char *q, int radix)
{
	long l;
	unsigned long ul;
	char *endp;
	l = STRTOL(q,&endp,radix);
	printf("%s:%d %d\n",q,endp-q,l);
	ul=STRTOUL(q,&endp,radix);
	printf("%s:%d %d\n\n",q,endp-q,ul);
}

void main(void)
{
	check(str1,10);
	check(str1,8);
	check(str1,16);
	check(str2,10);
	check(str2,8);
	check(str2,16);
	check(str3,10);
	check(str3,8);
	check(str3,16);
	check(str4,10);
	check(str4,8);
	check(str4,16);
	check(str5,10);
	check(str5,8);
	check(str5,16);
	check(str6,10);
	check(str6,8);
	check(str6,16);
}