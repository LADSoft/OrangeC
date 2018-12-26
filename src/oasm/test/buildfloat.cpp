#include <string.h>
#include <stdio.h>

#define arrsizeof(x) sizeof(x)/sizeof(x[0])
FILE *out;


#define FOR(x) for (char **p=x; p < x + arrsizeof(x); p++)
#define INS(y) instruction(*p,y)
#define SINS(x,y) instruction(x,y);
#define BINS(y) branchinstruction(*p, y);
#define RINS(x,y) repinstruction(*p,x,y);
#define IF64  if (with64)
#define IFNOT64 if (!with64)

void guard()
{
    fprintf(out,"nop\n");
    fprintf(out,"nop\n");
    fprintf(out,"nop\n");
}
void instruction(char *n, char *s)
{
    fprintf(out, "\t%s\t%s\n", n,s);
    guard();
}
void single(bool with64)
{
    char *names[] = {
    "f2xm1", "fabs", "fchs", "fclex", "fnclex","fcompp","fcos","fdecstp","fdisi", "feni",
    "fincstp", "finit", "fninit","fld1","fldl2e","fldl2t","fldlg2","fldln2",
    "fldpi","fldz","fnop","fpatan","fprem","fprem1","fptan","frndint", "fscale","fsetpm",
    "fsin","fsincos","fsqrt", "ftst", "fucompp","fwait","fxam", "fxtract","fyl2x","fyl2xp1"
    };
    FOR(names)
        INS("");
}
void math(bool with64)
{
    char *names[] = {    "fadd","fdiv","fdivr", "fmul", "fsub", "fsubr"};
    FOR(names)
    {
        INS("st0, st5");
        INS("st5, st0");
        INS("st5");
        INS("dword [ecx]");
        INS("qword [ecx]");
    }
}
void mathp(bool with64)
{
    char *names[] = {    "faddp","fdivp","fdivrp", "fmulp", "fsubp", "fsubrp", "fxch"};
    FOR(names)
    {
        INS("st5, st0");
        INS("st5");
        INS("");
    }

}
void mathi(bool with64)
{
    char *names[] = {    "fiadd","ficom","ficomp","fidiv","fidivr","fimul","fist","fisub","fisubr"};
    FOR(names)
    {
        INS("word [ecx]");
        INS("dword [ecx]");
    }
}
void qwordrm(bool with64)
{
    char *names[] = {    "fild","fistp"};
    FOR(names)
    {
	if (!strcmp(*p, "fild"))
		INS("word [ecx]");
	INS("dword [ecx]");
        INS("qword [ecx]");
    }
}
void fcom(bool with64)
{
    char *names[] = {  "fcom"  };
    FOR(names)
    {
        INS("st0, st5");
        INS("st5");
        INS("dword [ecx]");
        INS("qword [ecx]");
    }
}
void fcomp(bool with64)
{
    char *names[] = {  "fcomp"  };
    FOR(names)
    {
        INS("st5");
        INS("dword [ecx]");
        INS("qword [ecx]");
    }
}
void fld(bool with64)
{
    char *names[] = {  "fld"  };
    FOR(names)
    {
        INS("st5");
        INS("dword [ecx]");
        INS("qword [ecx]");
        INS("tword [ecx]");
    }
}
void fldcw(bool with64)
{
    char *names[] = {  "fldcw"  };
    FOR(names)
    {
        INS("[ecx]");
        INS("word [ecx]");
    }
}
void rm(bool with64)
{
    char *names[] = {"fldenv","fnsave","frstor","fsave","fstenv","fnstenv","fxrstor", "fxsave"};
    FOR(names)
    {
        INS("[ecx]");
    }
}

void srcreg(bool with64)
{
    char *names[] = {"" };
    FOR(names)
    {
//        INS("st4, st0");
    }
}
void dstreg(bool with64)
{
      char *names[] = { "ffree","ffreep", "fucom", "fucomp"};
    FOR(names)
    {
        INS("st4, st0");
    }
}
void cmov(bool with64)
{
      char *names[] = {"fcmovb","fcmovbe","fcmove","fcmovnb","fcmovnbe","fcmovne","fcmovnu", "fcmovu"};
    FOR(names)
    {
        INS("st0, st4");
    }
}
void fbld(bool with64)
{
        char *names[] = {"fbld","fbstp" };
    FOR(names)
    {
        INS("[ecx]");
        INS("tword [ecx]");
    }
}
void fst(bool with64)
{
    char *names[] = {  "fst"  };
    FOR(names)
    {
        INS("st5");
        INS("dword [ecx]");
        INS("qword [ecx]");
    }
}
void fstp(bool with64)
{
    char *names[] = {  "fstp"  };
    FOR(names)
    {
        INS("st5");
        INS("dword [ecx]");
        INS("qword [ecx]");
        INS("tword [ecx]");
    }
}
void wordrm(bool with64)
{
    char *names[] = {    "fldcw","fstcw","fnstcw",};
    FOR(names)
    {
        INS("[ecx]");
        INS("word [ecx]");
    }
}
void fstsw(bool with64)
{
    char *names[] = {    "fstsw","fnstsw"};
    FOR(names)
    {
        INS("[ecx]");
        INS("word [ecx]");
        INS("ax");
    }
}

void mainline(bool with64)
{
    single(with64);
    qwordrm(with64);
    wordrm(with64);
    math(with64);
    mathp(with64);
    mathi(with64);
    fld(with64);
    fbld(with64);
    fcom(with64);
    fcomp(with64);
    srcreg(with64);
    fst(with64);
    fstp(with64);
    fstsw(with64);
}
int main()
{
    out = fopen("float.asm","w");
    fprintf(out, "[bits 32]\n");
    mainline(false);
    fclose(out);
}