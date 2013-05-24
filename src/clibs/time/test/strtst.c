#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

void main(void)
{
	struct tm a;
	int i;
	char buf[1000];
	a.tm_sec = 5;
	a.tm_min = 39;
	a.tm_hour = 13;
	a.tm_mday = 28;
	a.tm_mon = 1;
	a.tm_year = 107;
	a.tm_wday = 3;
	a.tm_yday = 59;
	a.tm_isdst = 0;
	printf("%s\n",asctime(&a));
	a.tm_sec = 5;
	a.tm_min = 39;
	a.tm_hour = 14;
	a.tm_mday = 1;
	a.tm_mon = 2;
	a.tm_year = 107;
	a.tm_wday = 3;
	a.tm_yday = 0;
	a.tm_isdst = 1;
	printf("%s\n",asctime(&a));
	i = strftime(buf,1000,"%a %b %c\n",&a);
	printf("%d %s\n",i,buf);
    memset(buf,0,sizeof(buf));
	i = strftime(buf,10,"%a %b %c\n",&a);
	printf("%d %s\n",i,buf);
	i = strftime(buf,1000,"\x22%a %A %b %B '%c' '%C'\n"
                "%d %D %e %F\n"
                "%g %G %h %H %i %I %j %m %M %n %r %R\n"
                "%P %S %t %T %u %U %V %w %W '%x' '%X' %y %Y %z %Z\n"
                "%%\042\n",&a);
	printf("%d %s\n",i,buf);
    
    for (i= 1970; i < 2020; i++) {
	    int j;
        struct tm s ;
        a.tm_mon = 0 ;
        a.tm_year = i - 1900; 
        a.tm_isdst = 0;
        printf(":%d/%d/%d/",a.tm_mon, a.tm_mday, a.tm_year + 1900);
        mktime(&a);
        printf("a%d/%d/%d/",a.tm_mon, a.tm_mday, a.tm_year + 1900);
        printf("%%");
        j = strftime(buf,1000,"%G %V/%U %W",&a);
    	printf("%d %s\n",j,buf);
    }
}