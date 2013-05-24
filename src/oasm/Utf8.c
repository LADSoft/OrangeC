#include <stdio.h>
#include <stdlib.h>
unsigned char start[0x110000/8];
unsigned char sym[0x110000/8];
int main()
{
    int i,j;
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
                start[n/8] |= (1 << (n & 7));
            sym[n/8] |= (1 << (n & 7));
        }
    }
    printf("unsigned char UTF8::alpha[] = {\n");
    for (i=0; i < 0x110000/8; i+= 8)
    {
        printf("\t");
        for (j=i; j < i+8 && j < 0x110000/8; j++)
            printf("0x%02x, ", start[j]);
        printf("\n");
    }
    printf("};\n");
    printf("unsigned char UTF8::alnum[] = {\n");
    for (i=0; i < 0x110000/8; i+= 8)
    {
        printf("\t");
        for (j=i; j < i+8 && j < 0x110000/8; j++)
            printf("0x%02x, ", sym[j]);
        printf("\n");
    }
    printf("};\n");
}