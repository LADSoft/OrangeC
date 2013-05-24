#include <stdio.h>
#include <stdlib.h>
unsigned char start[0x110000];
unsigned char sym[0x110000];

int main()
{
    int i,j;
    int count;
    FILE *in = fopen("unicodedata.txt","r");
    if (!in)
    {
        printf("error");
        exit(1);
    }
    while (!feof(in))
    {
        char buf[256];
        char *l, *d;
        fgets(buf, 256, in);
        if ((l = strstr(buf,"LETTER")) || (d = strstr(buf,"DIGIT")))
        {
            int n = strtoul(buf,0,16);
            if (n >= 0x110000)
                break;
            if (l)
                start[n] = 1;
            sym[n] = 1;
        }
    }
    count = 0;
    for (i=0; i < 0x110000; i++)
    {
        if (start[i])
            count++;
    }
    printf("%d\n", count);
    count = 0;
    for (i=0; i < 0x110000; i++)
    {
        if (sym[i])
            count++;
    }
    printf("%d\n", count);
    printf("int UTF8::alpha[] = {\n");
    printf("\t");
        count = 0;
    for (i=0; i < 0x110000; i++ )
    {
        if (start[i])
        {
            printf("0x%06x, ", i);
            if (++count %8==0)
                printf("\n\t");
        }
    }
    printf("\n};\n");
    printf("int UTF8::alnum[] = {\n");
    printf("\t");
    count = 0;
    for (i=0; i < 0x110000; i++ )
    {
        if (sym[i])
        {
            printf("0x%06x, ", i);
            if (++count %8==0)
                printf("\n\t");
        }
    }
    printf("\n};\n");
}