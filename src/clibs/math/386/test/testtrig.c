
#include <math.h>

#define FUNCDECL(p,typ) typ FUNC##p(typ); typ q##FUNC##p(typ)
#define FUNCS { FUNCDECL(f,float); FUNCDECL(,double); FUNCDECL(l,long double);
#define FUNCE   }
#define ARG(p,typ,ARG1,FN) { double a = FN(ARG1); \
                          typ b = q##FN##p(ARG1); \
                          printf(#FN #p ": %f %f %f\n",(double)ARG1,a,(double)b); } 
#define TEST \
    { long double i; FUNCS; \
    for (i= - M_PI_2; i < M_PI_2 + .1; i+= 1.0/10) \
    { \
        ARG(f,float,i,FUNC); \
        ARG(,double,i,FUNC); \
        ARG(l,long double,i,FUNC); \
    } \
    FUNCE; \
    }

 
main()
{
#define FUNC acos
//TEST
#undef FUNC
#define FUNC asin
//TEST
//FUNCS;
//printf("%f\n",qasin(1.0));
//FUNCE;
#undef FUNC
#define FUNC atan
TEST
FUNCS;
printf("%f\n",qatan(M_PI_2)) ;
printf("%f\n",qatan(0));
FUNCE;
#undef FUNC
}