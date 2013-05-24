void aa(int a,int b)
{
}
void aa(char c)
{
}
void aa(int n)
{
}
#ifdef ERROR
int aa(int n)
{
}
#endif
namespace findme
{
    struct aa;
    int cc(struct aa *);
}
enum rrs { rrs1, rrs2, rrs3 } rrsx;
int bb(int n);
int bb(void *n);
int bb(void **n);
int bb(short &n);
int dd(int n);
int dd(void *n);
int (*xx)(int);
int (*xx1)(void *);
int (*xx2)(struct findme::aa *);
int f()
{
    void **p;
    void ** p1 = 0;
    int q;
    bb(p);
    bb(p1);
    bb(q);
    bb(rrsx);
#ifdef ERROR
    void * const* p2=0;
    bb(p2);
#endif
    dd(nullptr);
    dd(0);
    dd(*p);
    xx = bb;
    xx1 = bb;
    struct findme::aa *gg;
    cc(gg);
    xx2 = cc;
#ifdef ERROR
    char c;
    bb(c);
#endif
    short s;
    bb(s);
    
}
int gg(int a, char *b, int c);
int gg(int r, char s, int t);

int f1()
{
    gg(4,5,6);
   gg(4,nullptr,5);
}
#ifdef ERROR
int kk(int a);
int kk(char *a);
int f2()
{
    kk();
}
#endif

int hh(int , char *, ...);
int hh(int, int, int);
int f3()
{
    hh(5,nullptr, 5, 10, 15);
}
int ll();
int ll(int);
int ll(int, int);
int f4()
{
    ll();
    ll(4);
}
class aa1
{
    int r,s,t;
} ;
class cc1
{
    int u,v,w,z;
};
class bb1 : cc1, aa1
{
    int xx;
};
int vv(aa1&, int);
int vv(int , aa1&);

int tt(int &b);
int tt(aa1 b);
int tt1(int &&b);
int tt1(aa1 b);
int tt3(const int &b);
int tt3(aa1 b);

int qq(aa1*, int);
int qq(int, aa1*);
int rr(aa1*, int);

vmain()
{
    bb1 zz;
    vv(zz, 5);
#ifdef ERROR
    tt(4);  // error
#endif
    tt1(4);
    tt3(4);  
    int nn;
    tt(nn);
#ifdef ERROR
    tt1(nn); // error
#endif
    tt3(nn);
    qq(&zz, 5);
    rr(&zz, 5);
#ifdef ERROR
    cc1 xx;
    rr(&xx, 5);
    vv(xx, 5);
#endif
}
int xxz(int, ...);
int xxz(int, int);
int xxz(int,int,int);

xxzmain()
{
    xxz(4);
    xxz(4,5);
    xxz(4,5,6);
    xxz(4,5,6,7);
}