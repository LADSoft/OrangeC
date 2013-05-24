#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

void main(void)
{
	struct tm a;
	int i;
    int oldtime = 0,newtime;
	char buf[1000];
    
    for (i= 1970; i < 2020; i++) {
	    int j,newtime2;
        struct tm s ;
	a.tm_sec = 5;
	a.tm_min = 39;
	a.tm_hour = 14;
	a.tm_mday = 1;
	a.tm_mon = 2;
	a.tm_year = 107;
	a.tm_wday = 3;
	a.tm_yday = 0;
	a.tm_isdst = 1;
        a.tm_mon = 0 ;
        a.tm_year = i - 1900; 
        a.tm_isdst = 1;
        newtime = mktime(&a);
        s = *localtime(&newtime);
        newtime2 = mktime(&s);
    	printf("%d %d %d\n",i,newtime - oldtime, newtime2 == newtime);
        oldtime = newtime;
    }
    newtime = time(0) ;
    a = *localtime(&newtime);
    printf("%shi",asctime(&a));
}