#include <stdio.h>
#include "..\stdinc\string.h"

unsigned char str[] = "A\x84\x92ZABC\x90\xafZABC\xa0";
void main(void)
{
	long buf[100];
	int i = 0,t,j;
	char *q = str;
	MBLEN(0,0);
	while (*q) {
		printf("%d ",t = MBLEN(q,20));
		printf("%d\n",t = MBTOWC(&buf[i++],q,20));
		q += t;
	}
	printf("\n");
	for (j=0; j < i;j++) {
		printf("%lX\n",buf[j]);
	}
	printf("\n");
	MBLEN(0,0);
	i=MBSTOWCS(buf,str,100);		
	for (j=0; j < i;j++) {
		printf("%lX\n",buf[j]);
	}
}