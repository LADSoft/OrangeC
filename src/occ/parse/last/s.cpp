struct aa
{
    int aa1, aa2;
} ;
struct bb
{
    int bb1, bb2;
} ;
struct cc : private aa, bb
{
    int cc1, cc2;
} ;
main()
{
    cc rr;
    rr.aa1 = 4;
    rr.bb1 = 4;
    rr.cc1 = 4;
}
