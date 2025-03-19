#include <stdio.h>
char vals[]{
#embed "empty_file.txt" if_empty('H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!')
};
int main()
{
    FILE* fil = fopen("if_empty_out.txt", "w");
    for (size_t i = 0; i < sizeof(vals); i++)
    {
        fputc(vals[i], fil);
    }
    fclose(fil);
}