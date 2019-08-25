#include <stdio.h>
#include <Stdlib.h>
#include <string.h>
struct aa1
{
    char q,u,v;
} ;
struct aa
{
    struct aa1 * tt1;
    int n;
    char r,s,t;
    int tt;
    void (*p)(int);
} bb = {0,0,1,2,3} , *c = &bb, **d = &c;

int vv(struct aa *bb1, int i)
{
    printf("%d\n", bb.s);
//    if (bb1->r)
//        bb1->tt[i].q = 0;
}
int main(int argc, char *argv)
{

    vv(c, 4);
}