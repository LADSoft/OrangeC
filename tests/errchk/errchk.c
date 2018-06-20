int (*fptr)();

void vfunc(int);
int ifunc(int);
void suspicious_assign_to_func_s()
{
    fptr = ifunc;
}
void suspicious_assign_to_func_f()
{
    fptr = vfunc;
}
void expression_no_effect_s()
{
    int a = 5;
    (void) a;
    vfunc(a);
    ifunc(a);
    a = ifunc(a);
    a ? vfunc(a) : expression_no_effect();
    a++;
    a--;
    ++a;
    --a;    
}
void expression_no_effect_f()
{
    int a = 5;
    a;
    a ? a : a + 1;
    a ? a : vfunc(a);
    a ? vunc(a) : a;
    
}
#define finemacro 1 2
#define finemacro    1 2    
#define finemacro1 1 2
#define finemacro1     1    2
#define umacro hello
#define umacro dave

int switch_noreturn_s(int a)
{
     switch (a)
     {
          case 1:
		return 3;
	  case 2:
		return 3;
	  default:
		return 3;
     }
}
int switch_noreturn_f(int a)
{
     switch (a)
     {
          case 1:
		return 3;
	  case 2:
		return 3;
     }
}
int while_noreturn_s(int a)
{
     while (1) { }
}
int while_noreturn_f1(int a)
{
     while (a) { a++; }
}
int while_noreturn_f2(int a)
{
     while (1) { break; }
}
int do_noreturn_s1(int a)
{
     do
     {
     }while(1);
}
int do_noreturn_s2(int a)
{
     do
     {
         return 3;
     }while(0);
}
int do_noreturn_f(int a)
{
     do
     {
     } while(a);
}

int for_noreturn_s(int a)
{
     for (a = 4; ; a++) { }
}
int for_noreturn_s2(int a)
{
     for (;;) { }
}
int for_noreturn_f(int a)
{
    for(;a !=0; a++)
         return 3;
}
int if_noreturn_s(int a)
{
     if (a)
	return 3;
     else
	return 3;
}
int if_noreturn_f1(int a)
{
     if (a)
         return 3;
}
int if_noreturn_f2(int a)
{
     if (a)
         return 3;
     else ;
}
int if_noreturn_f3(int a)
{
     if (a) ;
     else
         return 3;
}

missing_type_f;