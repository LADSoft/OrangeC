unsigned char buf1[] = { 10,9,8,7,6,5,4,3,2,1 };
unsigned char buf2[] = { 10,9,8,1,1,1,1,1,1,1 };
unsigned char buf3[] = { 10,9,8,8,1,1,1,1,1,1 };
main()
{
	char lbuf[20];
	int i;
	unsigned char *t = MEMCHR(buf1,7,3);
	printf("memchr: %d %d\n",buf1,t);
	t = MEMCHR(buf1,7,4);
	printf("memchr: %d %d\n",buf1,t);
	t = MEMCHR(buf1,7,5);
	printf("memchr: %d %d\n",buf1,t);
	
	printf("memcmp: %d\n", MEMCMP(buf1,buf2,3));
	printf("memcmp: %d\n", MEMCMP(buf1,buf2,4));
	printf("memcmp: %d\n", MEMCMP(buf1,buf2,5));
	printf("memcmp: %d\n", MEMCMP(buf1,buf3,5));

	printf("memset: %d,%d\n",lbuf,MEMSET(lbuf,0x34,10));
	for (i=0; i < 20; i++) 
		printf("%02x, ",lbuf[i]);
	printf("\n");
	printf("memcpy: %d\n",	MEMCPY(lbuf,buf1,10));
	for (i=0; i < 20; i++) 
		printf("%02x, ",lbuf[i]);
	printf("\n");

	MEMSET(lbuf,0x30,20);
	MEMCPY(lbuf,buf1,10);
	printf("memmove: %d\n",	MEMMOVE(lbuf+1,lbuf,10));
	for (i=0; i < 20; i++) 
		printf("%02x, ",lbuf[i]);
	printf("\nmemmove %d\n:",	MEMMOVE(lbuf,lbuf+1,10));
	for (i=0; i < 20; i++) 
		printf("%02x, ",lbuf[i]);
	printf("\n");


	
}