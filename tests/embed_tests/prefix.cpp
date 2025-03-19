#include <stdio.h>
char vals[]{
#embed "prefix_in.txt" __prefix__('1', '2', '3', )
};
int main()
{
    FILE* fil = fopen("prefix_out.txt", "w");
    for (size_t i = 0; i < sizeof(vals); i++)
    {
        fputc(vals[i], fil);
    }
    fclose(fil);
}