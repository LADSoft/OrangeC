#include <stdio.h>
main()
{
    char buf[256];
    int i;
    FILE *fil = fopen("hi.txt","a+");
    printf("%p %d\n",fil->buffer,fil->bsize);
    printf("%d\n",setvbuf(fil,0,_IOFBF,1024));
    printf("%p %d\n\n",fil->buffer,fil->bsize);
    fclose(fil);
    fil = fopen("hi.txt","a+");
    printf("%p %d\n",fil->buffer,fil->bsize);
    printf("%d\n",setvbuf(fil,buf,_IOFBF,256));
    printf("%p %p %d\n\n",fil->buffer,buf,fil->bsize);
    fclose(fil);
    fil = fopen("hi.txt","a+");
    printf("%p %d\n",fil->buffer,fil->bsize);
    printf("%d\n",setvbuf(fil,buf,_IONBF,256));
    printf("%p %p %d\n\n",fil->buffer,buf,fil->bsize);
    printf("%d\n",setvbuf(fil,buf,_IONBF,256));
    fclose(fil);
}