#include <stdio.h>
char vals[]{
#embed "does_not_exist"
};
int main()
{
    FILE* fil = fopen("does_not_exist.txt", "w");
    for (size_t i = 0; i < sizeof(vals); i++)
    {
        fputc(vals[i], fil);
    }
    fclose(fil);
}