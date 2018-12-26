#include <string.h>
#include <stdio.h>

#define arrsizeof(x) sizeof(x)/sizeof(x[0])
FILE *out;


#define FOR(x) for (char **p=x; p < x + arrsizeof(x); p++)
#define INS(y) instruction(*p,y)
#define SINS(x,y) instruction(x,y);
#define BINS(y) branchinstruction(*p, y);
#define RINS(x,y) repinstruction(*p,x,y);
#define IF64  if (with64)","
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
    char *names[] = { "mfence","monitor"
    };
    FOR(names)
        INS("");
}
void regrm(bool with64)
{
    char *names[] = { 
        "addps", "andps", "divps",
        "cvtdq2ps",
        "comiss", "comisd", "ucomiss", "ucomisd",
        "maxps","minps","mulps","orps","sqrtps",
        "subps","unpckhps","unpcklps","xorps",
        "addpd", "addsd", "addss", "addsubpd", "addsubps",
	"andpd","divpd", "divsd", "divss",
        "cvtdq2pd","cvtpd2dq","cvtpd2ps","cvtps2dq", "cvtps2pd",
        "cvtsd2ss","cvtss2sd","cvttpd2dq","cvttps2dq",
        "hsubpd","hsubps","maxpd","maxsd","maxss","minpd","minsd","minss",
        "movddup","movshdup","movsldup","mulpd","mulsd","mulss","orpd","punpckhqdq","punpcklqdq","packusdw","rcpps", "rcpss",
        "rsqrtps","rsqrtss","sqrtpd","sqrtsd","sqrtss","subpd","subsd","subss","unpckhpd","unpcklpd","xorpd",
    };
    char *names1[] = {" lddqu","movhpd","movhps","movlpd","movlps",
 };
    FOR(names)
    {
        INS("xmm1, xmm10");
        INS("xmm1, [rcx]");
        INS("xmm10, xmm1");
        INS("xmm10, [rcx]");
        INS("xmm2, xmm1");
    }
    FOR(names1)
    {
        INS("xmm1, [rcx]");
        INS("xmm10, [rcx]");
    }
}
void regrmsized(bool with64)
{
    char *names[] = { "cvtsi2sd", "cvtsi2ss"
    };
    FOR(names)
    {
        INS("xmm1, dword [rcx]");
        INS("xmm1, qword [rcx]");
        INS("xmm10, dword [rcx]");
        INS("xmm10, qword [rcx]");
    }
}
void regrmimm(bool with64)
{
    char *names[] = { "blendpd", "blendps","cmpps", "cmppd",
        "mpsadbw","pblendw","pcmpestri","pcmpestrm","pcmpestri","pcmpestrm","roundpd","roundps",
        "roundsd","roundss","pshufd","pshufhw","pshuflw","shufpd", "shufps"

    };
    FOR(names)
    {
        INS("xmm1, xmm10,3");
        INS("xmm1, [rcx],5");
        INS("xmm10, xmm1,3");
        INS("xmm10, [rcx],5");
        INS("xmm1, xmm3, 2");
    }
}
void regimm(bool with64)
{
    char *names[] = {
        "pslld","psllq","psllw","psrad","psraw","psrld","psrlq","psrlw"
    };
    FOR(names)
    {
        INS("xmm1, xmm10");
        INS("xmm1, [rcx]");
        INS("xmm1, 3");
        INS("xmm10, xmm1");
        INS("xmm10, [rcx]");
        INS("xmm10, 3");
        INS("xmm1, xmm2");
        INS("mm1, mm5");
        INS("mm1, [rcx]");
        INS("mm1, 3");
    }
}
void regonlyimm(bool with64)
{
    char *names[] = {
        "pslldq","psrldq"
    };
    FOR(names)
    {
        INS("xmm1, 3");
        INS("xmm10, 3");
    }
}
void iregsse(bool with64)
{
    char *names[] = {"cvtsd2si","cvtss2si","cvttsd2si","cvttss2si","movmskpd","movmskps"
    };
    FOR(names)
    {
        INS("ebx,xmm1");
        INS("ebx,xmm12");
        INS("rbx,xmm1");
        INS("rbx,xmm12");
        INS("r15,xmm1");
        INS("r15,xmm12");
    }
}
void move(bool with64)
{
    char *names[] = {
        "movapd","movaps","movdqa","movdqu",
	"movss","movupd","movups"
    };
    FOR(names)
    {
        INS("xmm1, xmm10");
        INS("xmm1, [rcx]");
        INS("xmm10, xmm1");
        INS("xmm10, [rcx]");
        INS("xmm2, xmm1");
        INS("[rcx], xmm1");
        INS("[rcx], xmm10");
    }
}
void mmmm(bool with64)
{
    char *names[] = { "pshufw"
    };
    FOR(names)
    {
        INS("mm1, mm3,55");
        INS("mm2, [rcx],45");
    }
}
void mmsse(bool with64)
{
    char *names[] = {
        "cvtpd2pi","cvtps2pi","cvttpd2pi","cvttps2pi","movdq2q"

    };
    FOR(names)
    {
        INS("mm3,xmm4");
        INS("mm3,xmm10");
	if (strcmp(*p, "movdq2q"))
	        INS("mm3,[rcx]");
    }
}
void pack(bool with64)
{
    char *names[] = {
        "packssdw","packsswb","paddb","paddd","paddq","paddsw","paddusb","paddusw",
        "paddw","pand","pandn","pavgb","pavgw","pcmpeqb","pcmpeqd","pcmpeqw",
        "pcmpgtb","pcmpgtd","pcmpgtw","pmaddwd","pmaxsw","pmaxub","pminsw",
        "pminub","pmulhuw","pmulhw","pmullw","pmuludq","psadbw","psubb","psubd","psubq","psubsb",
        "psubsw","psubusb","psubusw","punpckhbw","punpckhdq","punpckhwd","punpcklbw",
        "punpckldq","punpcklwd","pxor"
        

    };
    FOR(names)
    {
        INS("xmm1, xmm10");
        INS("xmm1, [rcx]");
        INS("xmm10, xmm1");
        INS("xmm10, [rcx]");
        INS("xmm1, xmm2");
        INS("mm1, mm5");
        INS("mm1, [rcx]");
    }
}
void rmsse(bool with64)
{
    char *names[] = {"movntdq","movntpd"
    };
    FOR(names)
    {
        INS("[rcx], xmm2");
        INS("[rcx], xmm10");
    }

}
void regmm(bool with64)
{
    char *names[] = { "cvtpi2ps","cvtpi2pd","movq2dq"
    };
    FOR(names)
    {
        INS("xmm4, mm2");
        INS("xmm10, mm2");
        if (strcmp(*p, "movq2dq"))
        {
            INS("xmm4, [r15]");
            INS("xmm10, [rcx]");
        }
    }
}

void rmmm(bool with64)
{
    char *names[] = { "movntq", "movntq"
    };
    FOR(names)
    {
        INS("[rcx], mm2");
    }
}
void rmireg(bool with64)
{
    char *names[] = {
        "movnti"
    };
    FOR(names)
    {
        INS("[rcx], ebx");
        INS("[rcx], rbx");
    }

}
void movd(bool with64)
{
    char *names[] = { "movd"
    };
    FOR(names)
    {
        INS("mm3, [rcx]");
        INS("xmm1, [rcx]");
        INS("xmm10, [rcx]");
        INS("[rcx], mm3");
        INS("[rcx],xmm2");
        INS("[rcx],xmm10");
        INS("mm3, ebx");
        INS("xmm1, ebx");
        INS("xmm10, ebx");
        INS("ebx, mm3");
        INS("ebx,xmm2");
        INS("ebx,xmm10");

    }
}
void movq(bool with64)
{
    char *names[] = { "movq"
    };
    FOR(names)
    {
        INS("mm3, [rcx]");
        INS("xmm1, [rcx]");
        INS("xmm10, [rcx]");
        INS("[rcx], mm3");
        INS("[rcx],xmm2");
        INS("[rcx],xmm10");
        INS("mm3, rbx");
        INS("xmm1, rbx");
        INS("xmm10, rbx");
        INS("rbx, mm3");
        INS("r15,xmm2");
        INS("rbx,xmm10");
    }
}

void pextrb(bool with64)
{
    char *names[] = { "pextrb",

    };
    FOR(names)
    {
        INS("[rcx],xmm1,55");
        INS("[rcx],xmm10,55");
        INS("ebx,xmm1,55");
        INS("ebx,xmm10,55");

    }
}


void pextrd(bool with64)
{
    char *names[] = { "pextrd"

    };
    FOR(names)
    {
        INS("[rcx],xmm1,55");
        INS("[rcx],xmm10,55");
        INS("ebx,xmm1,55");
        INS("ebx,xmm10,55");

    }
}
void pextrw(bool with64)
{
    char *names[] = { "pextrw"
    };
    FOR(names)
    {
        INS("ebx, xmm1,55");
        INS("[ecx],xmm8,55");
        INS("ebx, xmm10,55");
        INS("ebx,mm3,55");
        INS("rbx, xmm10,55");
    }
}
void pextrq(bool with64)
{
    char *names[] = { "pextrq"
    };
    FOR(names)
    {
        INS("[rcx],xmm1,55");
        INS("[rcx],xmm10,55");
        INS("rbx,xmm1,55");
        INS("r15,xmm10,55");
    }
}
void pinsrb(bool with64)
{
    char *names[] = { "pinsrb"
    };
    FOR(names)
    {
        INS("xmm1,[rcx],55");
        INS("xmm10,[rcx],55");
        INS("xmm1,ebx,55");
        INS("xmm6,r12d,55");
    }
}

void pinsrd(bool with64)
{
    char *names[] = { "pinsrd"
    };
    FOR(names)
    {
        INS("xmm1,[rcx],55");
        INS("xmm10,[rcx],55");
        INS("xmm1,ebx,55");
        INS("xmm10,r12d,55");
    }
}
void pinsrw(bool with64)
{
    char *names[] = { "pinsrw"
    };
    FOR(names)
    {
        INS("xmm1,ebx,55");
        INS("xmm8,[ecx],55");
        INS("xmm10,ebx,55");
        INS("mm3,ebx,55");
        INS("mm3, [ecx],55");
        INS("xmm10,rbx,55");

    }
}
void pinsrq(bool with64)
{
    char *names[] = { "pinsrq"
    };
    FOR(names)
    {
        INS("xmm1,[rcx],55");
        INS("xmm10,[rcx],55");
        INS("xmm1,rbx,55");
        INS("xmm10,r12,55");
    }
}
void regreg(bool with64)
{
    char *names[] = { "movhlps","movlhps"
    };
    FOR(names)
    {
        INS("xmm1,xmm10");
        INS("xmm10,xmm1");
        INS("xmm3, xmm6");
    }

}
void palignr(bool with64)
{
    char *names[] = { "palignr"
    };
    FOR(names)
    {
        INS("xmm1,xmm10,55");
        INS("xmm8,[ecx],55");
        INS("xmm10,xmm3,55");
        INS("mm3,mm4,55");
        INS("mm3, [ecx],55");
    }
}
void maskmov(bool with64)
{
    char *name1[] = { "maskmovq", 
    };
    char *name2[] = { "maskmovdqu", 
    };
    FOR(name1)
    {
        INS("mm1,mm7");
    }

    FOR(name2)
    {
        INS("xmm1,xmm10");
        INS("xmm10,xmm1");
        INS("xmm3, xmm6");
    }

}
void mainline(bool with64)
{
    single(with64);
    regrm(with64);
    regrmsized(with64);
    regrmimm(with64);

    regimm(with64);
    regonlyimm(with64);
    iregsse(with64);
    move(with64);
    mmmm(with64);
    mmsse(with64);
    pack(with64);
    rmsse(with64);
    rmireg(with64);
    rmmm(with64);
    regmm(with64);
    movd(with64);
    movq(with64);

    pextrb(with64);
    pextrd(with64);
    pextrw(with64);
    pextrq(with64);
    pinsrb(with64);
    pinsrd(with64);
    pinsrw(with64);
    pinsrq(with64);
    regreg(with64);
    palignr(with64);
    maskmov(with64);
}
int main()
{
    out = fopen("sse.asm","w");
    fprintf(out, "[bits 64]\n");
    mainline(false);
    fclose(out);
}