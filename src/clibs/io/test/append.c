#include <stdio.h>
main()
{
    char buf[256];
    int i;
    FILE *fil = fopen("hi.txt","a+");
    printf("%d ",fputs("hi dave",fil));
    rewind(fil);
    fputs("string",fil);
    rewind(fil);
    fputc('p',fil);
    rewind(fil);
    fwrite("geez",4,1,fil);
    rewind(fil);
    printf("%d\n",fclose(fil));
    printf("bye\n");
    fil = tmpfile();
//    printf("%s\n",fil->name);
//    strcpy(buf,fil->name);
    fprintf(fil,"hi dave");
    fflush(0);
    fclose(fil);
    fil = fopen("hidave","r");
    printf("%x\n",fil);
    fil = freopen("byedave","w",fil);
    fprintf(fil,"go home");
}