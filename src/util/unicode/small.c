#include <stdio.h>
#include <stdlib.h>
struct binary
{
    int lower;
    int upper;
} *list;
int count;
int upper[0x110000];
int ToUpper(int val)
{
    int top = count;
    int bottom = -1;
    int v;
    while (top - bottom > 1)
    {
        int mid = (top + bottom) / 2;
        if (val < list[mid].lower)
        {
            top = mid;
        }
        else
        {
            bottom = mid;
        }
    }
    if (list[bottom].lower != val )
        return 0;
    return list[bottom].upper;
}
int main()
{
    int i,j;
    FILE *in = fopen("unicodedata.txt","r");
    int max;
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
        if ((l = strstr(buf,"LETTER")))
        {
            int n = strtoul(buf,0,16);
            char *p = strrchr(buf, ';');
            if (n >= 0x110000)
                break;
            if (isxdigit(p[1]))
            {
                int u = strtoul(p+1, 0, 16);
                upper[n] = u;
            }
        }
    }
    max = 0x110000;
    for ( ; max > 0; max--)
        if (upper[max-1] != 0)
            break;
    printf("%d\n", max);
    for (i=0; i < max; i++)
        if (upper[i])
            count++;
    list = calloc(count, sizeof(struct binary));
    count = 0;
    for (i=0; i < max; i++)
    {
        if (upper[i])
        {
            list[count].upper = upper[i];
            list[count++].lower = i;
        }
    }
    for (i=0; i < max; i++)
        if (ToUpper(i) != upper[i])
        {
            printf("failed");
            break;
        }
    if (i == max)
        printf("success");
    printf("%d\n", count);
    printf("unsigned char UTF8::upper[][2] = {\n");
    for (i=0; i < count; i+=4)
    {
        printf("\t");
        for (j=i; j < i+4 && j < count; j++)
            printf("{ 0x%05x, 0x%05x },", list[j].lower, list[j].upper);
        printf("\n");
    }
    printf("};\n");
}