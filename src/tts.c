#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *in, *out;
    int mode;
    int i;
    if (argc != 4)
        return;
    mode = argv[1][0];
        
    in = fopen(argv[2], "rb");
    out = fopen(argv[3], "wb");
    while (!feof(in))
    {
        char buf[1024], *p= buf;
        int n = 0;
        fgets(buf, 1024, in);
        while (isspace(*p))
        {
            if (*p == '\t')
                n += 4;
            else if (*p == ' ')
                n++;
            else break;
            p++;
        }
        if (mode == 't')
        {
            for (i=0; i < n/4; i++)
                fputc('\t', out);
            for (i=0; i < n %4; i++)
                fputc(' ', out);
        }
        else
        {
            for (i=0; i < n; i++)
                fputc(' ', out);
        }
        fprintf(out, "%s", p);
    }
}