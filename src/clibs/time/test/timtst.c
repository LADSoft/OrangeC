#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
#include <stdio.h>

void query(char *string, int *val)
{
	char istring[100];
	printf("\n%s? ",string);
	gets(istring);
	*val = atoi(istring);
}
void main(void)
{
	time_t t,t1;
	struct tm a,b,c;
	char buf[100],buf2[100];
	printf("%d\n",0x7fffffff/86400/365+70);
	while (1) {
		query("sec",&a.tm_sec);
		query("min",&a.tm_min);
		query("hr",&a.tm_hour);
		query("month",&a.tm_mon);
		query("day",&a.tm_mday);
		query("year",&a.tm_year);
		t = mktime(&a);
		t1 = mktime(&a);
		printf("mktime: %d %d\n",t,t1);
        printf("%d\n",a.tm_yday);
		t1 = time(0);
		strcpy(buf,ctime(&t));
		strcpy(buf2,ctime(&t1));
		printf("ctime: %s %s\n",buf,buf2);
		b = *gmtime(&t);
		c = *gmtime(&t);
		strcpy(buf,asctime(&b));
		strcpy(buf2,asctime(&c));
		printf("gmtime: %s %s",buf,buf2);
		b = *localtime(&t);
		c = *localtime(&t);
		strcpy(buf,asctime(&b));
		strcpy(buf2,asctime(&c));
		printf("LOtime: %s %s",buf,buf2);
	}
}