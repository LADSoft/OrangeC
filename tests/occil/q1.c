void printf(char *, ...);
void exit(int);

struct aa
{
    int r, s, t;
} bb = {3,4,5};
main()
{
    int i = 7;
    struct aa rr;
    rr.t = 245;
    rr = bb;
    printf("%d %d %d\n", bb.s, rr.t, i);
    return 0;
}