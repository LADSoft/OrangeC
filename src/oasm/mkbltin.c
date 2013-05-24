#include <stdio.h>
main()
{
    FILE *in = fopen("builtins.asm", "r");
    while (!feof(in))
    {
        char buf[256];
        if (fgets(buf, 256, in) > 0)
        {
            int l = strlen(buf);
            if (buf[l-1] == '\n')
                buf[l-1] = 0;
            if (strlen(buf))
                printf("\"%s\\n\"\n",buf);
        }
    }
}