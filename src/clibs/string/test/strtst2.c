int main()
{
	char buf1[200], buf2[200];
	int i;
	for (i=0; i < 200; i++)
		buf1[i] = buf2[i] = (rand() & 0x7f) | 32;
	buf1[199] =77;
	buf2[199] = 78;
	printf("%d\n",strcmp("abc","abc"));
	printf("%d\n",strcmp("abc","abd"));
	printf("%d\n",strcmp("abc","abb"));
	printf("%d\n",strcmp("abc","abcd"));
	printf("%d\n",strcmp("abcd","abc"));
	printf("\n");
	printf("%d\n",memcmp("abc","abc",3));
	printf("%d\n",memcmp("abc","abd",3));
	printf("%d\n",memcmp("abc","abb",3));
	printf("\n");
	printf("%d\n",strncmp(buf1, buf2, 199));
	buf2[198]++;
	printf("%d\n",strncmp(buf1, buf2, 199));
	buf2[197]++;
	buf2[198]--;
	printf("%d\n",strncmp(buf1, buf2, 199));
	buf1[100] = 0;	
	printf("%d\n",strncmp(buf1, buf2, 199));
	buf2[50] = 0;
	printf("%d\n",strncmp(buf1, buf2, 199));
	buf1[50] = 0;
	printf("%d\n",strncmp(buf1, buf2, 199));
	memset(buf1,0,sizeof(buf1));
	strcpy(buf1,"hello dave");
	buf1[3] = 0;
	strncat(buf1,"hi", 5);
	printf("%s\n", buf1);
	buf1[5]='$';
	printf("%s\n", buf1);
	strcpy(buf1,"hello dave");
	buf1[3] = 0;
	strncat(buf1,"hi dave", 4);
	printf("%s\n", buf1);
	strcpy(buf1,"hello dave");
	strncat(buf1,"peter rabbit", 5);
	printf("%s\n", buf1);
	
	strcpy(buf1,"hi");
	strcat(buf1,"dave");
	printf("%s\n", buf1);
	printf("%x %x\n",strchr(buf1,'d'), strchr(buf1,'z'));
	printf("%d\n",strlen(buf1));
	printf("%x %x\n",memchr(buf1,'d',6), memchr(buf1,'z',6));
}