#include <stdio.h>
int main()
{
    char val =
#embed "single.txt"
        ;
    FILE* fil = fopen("single_out.txt", "w");
    fputc(val, fil);
    fclose(fil);
}