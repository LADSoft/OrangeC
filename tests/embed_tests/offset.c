#include <stdint.h>
#include <stdio.h>
char var_list[] = {
// Following should skip abcde and land us at fgh, a being offset 0 and so on.
#embed "alphabet.txt" offset(4) limit(3)
};
int main()
{
    FILE* fil = fopen("offset_out.txt", "w");
    for (size_t i = 0; i < sizeof(var_list); i++)
    {
        fputc(var_list[i], fil);
    }
    fclose(fil);
}