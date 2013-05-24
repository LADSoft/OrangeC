#include <stdio.h>

void main(void)
{
	char buf[100],buf2[100];
    wchar_t s[] = { 0x21, 0x7ff,0x800,0xffff, 0};
    wchar_t st[10],*st1 = s;
	short a;
	int b,d,n,i;
	char c;
    float ff;
    long double fld;
    FILE *fil;
    i = sscanf("  20 -40 50a","%d %hd %n %d",&b,&a,&n,&d);
	printf("%d %d %d %d %d\n",i,n,b,a,d);
    i = sscanf("   a    bc"," %c %c",&buf[0],&buf[1]);
	printf("%d %d %d\n",i,buf[0],buf[1]);
	i = sscanf("0778a"," %o",&b);
	printf("%d %d\n",i,b);
	i = sscanf("778"," %u",&b);
	printf("%d %d\n",i,b);
	i = sscanf("-778"," %u",&b);
	printf("%d %d\n",i,b);
	i = sscanf("0xA2 7B","%x %x",&a,&b);
	printf("%d %x %x\n",i,a,b);
	i = sscanf(" xxx abc","%s",buf);
	printf("%d %s\n",i,buf);
	i = sscanf(" abcdefg","%[ abc]",buf);
	printf("%d %s\n",i,buf);
	i = sscanf(" abcdefghijk","%[^g]",buf);
	printf("%d %s\n",i,buf);
	i = sscanf(" abcdefghijk","%[ a-f]",buf);
	printf("%d %s\n",i,buf);
	i = sscanf(" abcdefghijk","%[^i-k]",buf);
	printf("%d %s\n",i,buf);
    i = sscanf(" 4.5 0x1.4P270","%f %Lf",&ff, &fld);
    printf("%d %f %La\n",i,ff, fld);
    i = sscanf(" 4.5e30 0x1.4FP270","%f %Lf",&ff, &fld);
    printf("%d %e %.1La\n",i,ff, fld);
    wcsrtombs(buf,&st1,100,0);
    i = sscanf(buf,"%lc%lc%lc%lc%lc",st,st+1,st+2,st+3,st+4);
    printf(":::%d\n",i);
    for (i=0; i < 5; i++)
        printf("%04x ",st[i]);
    printf("\n");
    memset(st,0,sizeof(st));
    i = sscanf(buf,"%ls",st);
    printf(":::%d\n",i);
    for (i=0; i < 5; i++)
        printf("%04x ",st[i]);
    printf("\n");
    fil = fopen("hi.txt","w");
    fputs(" 42 4.5hi dave",fil);
    freopen(0,"r",fil);
    fscanf(fil,"%d %f",&b, &ff);
    fgets(buf,100,fil);
    printf("%d %f %s\n",b,ff,buf);
    fclose(fil);
}