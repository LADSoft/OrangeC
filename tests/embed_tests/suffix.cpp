#include <stdio.h>
char vals[]
{
#embed "postfix_in.txt" __suffix__(, '4', '5', '6')
};
int main()
{
    FILE* fil = fopen("postfix_out.txt", "w");
    for (size_t i = 0; i < sizeof(vals); i++)
    {
        fputc(vals[i], fil);
    }
    fclose(fil);
}