#include <stdio.h>
#include <inttypes.h>
// We're reusing stuff over here
#define ONE_PLUS_ONE 1 + 1
char vals[]{
#embed <postfix_in.txt> limit(ONE_PLUS_ONE)
};
int main()
{
    static_assert(sizeof(vals) == ONE_PLUS_ONE, "Wtf, vals size should be 2");
    if (vals[0] != '1' && vals[1] != '2')
    {
        int a0 = vals[0];
        int a1 = vals[1];
        printf("Did not compare properly, size: %zd, vals[0] = %d, vals[1] = %d\r\n", sizeof(vals), a0, a1);
        return 1;
    }
    return 0;
}
