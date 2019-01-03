#include <stdio.h>
static constexpr unsigned int thing = -1;
inline constexpr unsigned int retThing()
{
    return thing;
}
int main()
{
    printf("%d", retThing());
}