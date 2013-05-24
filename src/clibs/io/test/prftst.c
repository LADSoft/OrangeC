#include <stdio.h>
#include <String.h>
void dorpl(char *out,char *rplstr, char ch, char *mod)
{
    while (*rplstr) {
        if (*rplstr == 'Z') {
            strcpy(out,mod);
            out = out + strlen(out);
            *out++ = ch;
        } else
            *out++ = *rplstr;
        rplstr++;
    }
    *out = 0;
}
void testi(int ch)
{
	char buf[256];
    char str[256];
	int count,i;
    static char rplstr[] = "'%Z'% Z'%-+Z'%+Z'%#Z'%0Z'%n";
    printf("\ntesting %c\n",ch);
    dorpl(str,"%Z%n",ch,"*.*");
    printf("%s\n",str);
    i = sprintf(buf,str,10,5,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"");
    i = sprintf(buf,str,-287,-287,-287,-287,-287,-287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"10");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,".5");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    dorpl(str,rplstr,ch,"2");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7h");
    i = sprintf(buf,str,65537,65537,65537,65537,65537,65537,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7hh");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7l");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7ll");
    i = sprintf(buf,str,287LL,287LL,287LL,287LL,287LL,287LL,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7j");
    i = sprintf(buf,str,287LL,287LL,287LL,287LL,287LL,287LL,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7z");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"9.7t");
    i = sprintf(buf,str,287,287,287,287,287,287,&count);
    printf("%s %d %d\n",buf,count,i);
}
void testf(char ch)
{
	char buf[256];
    char str[256];
	int count,i;
    static char rplstr[] = "'%Z'% Z'%-Z'%+Z'%#Z'%0Z'%n";
    printf("\ntesting %c\n",ch);
    dorpl(str,rplstr,ch,"");
    i = sprintf(buf,str,287.912,287.912,287.912,287.912,287.912,287.912,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"");
    i = sprintf(buf,str,-287.912,-287.912,-287.912,-287.912,-287.912,-287.912,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,".0");
    i = sprintf(buf,str,287.912,287.912,287.912,287.912,287.912,287.912,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"10.0");
    i = sprintf(buf,str,287.912,287.912,287.912,287.912,287.912,287.912,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"10.4");
    i = sprintf(buf,str,287.912,287.912,287.912,287.912,287.912,287.912,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"10.2");
    i = sprintf(buf,str,287.912,287.912,287.912,287.912,287.912,287.912,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"10.2");
    i = sprintf(buf,str,.00000056789,.00000056789,.00000056789,.00000056789,.00000056789,.00000056789,&count);
    printf("%s %d %d\n",buf,count,i);
    dorpl(str,rplstr,ch,"10.2L");
    i = sprintf(buf,str,287.912L,287.912L,287.912L,287.912L,287.912L,287.912L,&count);
    printf("%s %d %d\n",buf,count,i);
    
}
void main(void)
{
	char buf[100];
    int i,count=0xffffffff;
    wchar_t s[] = { 0x21, 0x7ff,0x800,0xffff, 0};
	i = sprintf(buf,"GO GO '%s' '%10s' '%-10s' '%10.4s' GO GO '%c' '%10c' '%-10c'%hn '%Z'","hi dave","hi dave","hi dave", "hi dave",'A','A','A', &count);
	printf("%s %d %d\n",buf,i, count);
    testi('d');
    testi('i');
    testi('u');
    testi('o');
    testi('x');
    testi('X');
    testi('p');
    testi('P');
    testf('a');
    testf('e');
    testf('f');
    testf('g');
    testf('A');
    testf('E');
    testf('F');
    testf('G');
    
    i = snprintf(buf,10,"hello there dave");
    printf("%d %s\n",i,buf);
    i = snprintf(buf,10,"%s","hello there dave");
    printf("%d %s\n",i,buf);
    sprintf(buf,"%5lc%-5lc%lc%lc%lc",s[0],s[1],s[2],s[3],s[4]);
    for (i=0 ; i < strlen(buf); i++)
        printf("%02x ",(unsigned char)buf[i]);
    printf("\n");
    sprintf(buf,"%20ls%-20ls",s,s);
    for (i=0 ; i < strlen(buf); i++)
        printf("%02x ",(unsigned char)buf[i]);
}