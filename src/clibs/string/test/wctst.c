#include <stdio.h>
#include "..\stdinc\stdlib.h"

wchar_t val[] = {0x1734f0c1L, 0x1734f0c2L,10};
void main(void)
{
	unsigned char buf[100];
	int i,j;
	WCTOMB(0,0);
	i = WCTOMB(buf,val[0]);
	printf("%d ",i);
	i += WCTOMB(buf+i,val[1]);
	printf("%d ",i);
	i += WCTOMBFLUSH(buf+i);
	printf("%d ",i);
	for (j=0; j <i; j++)
		printf("%02x ",buf[j]);
	printf("\n\n");
	WCTOMB(0,0);
	i = WCSTOMBS(buf,&val[0],2);
	printf("%d ",i);
	for (j=0; j <i; j++)
		printf("%02x ",buf[j]);
	printf("\n\n");
	i = WCSTOMBS(buf,&val[0],3);
	printf("%d ",i);
	for (j=0; j <i; j++)
		printf("%02x ",buf[j]);
	printf("\n\n");
	for (i=0; i<280; i++) {
		int k = 0;
		WCTOMB(0,0);
		for (j=0; j < i; j++)
			k+=WCTOMB(buf+k,val[0]);
		for (j=0; j <k; j++)
			printf("%02x ",buf[j]);
		printf("\n\n");
	}
}
	