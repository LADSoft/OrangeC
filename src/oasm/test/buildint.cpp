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
void bits(int n)
{
    fprintf(out,"cld\n");
    fprintf(out,"nop\n");
    fprintf(out,"nop\n");
    fprintf(out,"[bits %d]\n", n);
}
void instruction(char *n, char *s)
{
    fprintf(out, "\t%s\t%s\n", n,s);
    guard();
}
void branchinstruction(char *n, char *s)
{
    static int lbl;
    fprintf(out, "\t%s\t%s lbl%d\n",n, s, lbl);
    fprintf(out, "\tnop\n");
    fprintf(out, "\tnop\n");
    fprintf(out, "lbl%d: nop\n", lbl++);
}
void repinstruction(char *ins, char *rep, char *arg)
{
    char buf[256];
    sprintf(buf, "%s %s", rep, ins);
    instruction(buf, arg);
}

void single(bool with64)
{
	char *legacy[] = { "aaa", "aad", "aam" ,"aas", "daa", "das",
    "into", "popa","popaw","popad", "pusha" , "pushaw", "pushad", "popfw","popfd", "pushfw", "pushfd", };
    char *any[] = { "cbw", "cdq", "clc" , "cld" , "cli", "clts", "cmc", "cpuid",
			"cwd", "cwde", "hlt", "icebp", "int1", "int3", "invd" ,
			"iret", "iretw", "iretd", "lfence", "popf", 
            "pushf","rsm", "lahf", "sahf", "sfence",
            "stc","std","sti", "syscall", "sysenter", "sysexit", "sysret",
            "ud2","wait","wbinvd","wrmsr", "xsetbv" };
    char *sixtyfour[] = { "cdqe", "cqo", "iretq", "popfq", "pushfq" };

    IFNOT64 FOR(legacy)
            INS("");
    FOR(any)
        INS("");
    IF64 FOR(sixtyfour)
        INS("");
}
void math(bool with64)
{
	char *names[] = { "adc", "add", "and","cmp", "or", "sbb","sub","xor" };
    FOR(names)
    {
        INS("al,5");
        INS("ax,0x1234");
        INS("eax,0x12345678");
        IF64 INS("rax,0x12345678");

        INS("al,byte 5");
        INS("ax,word 0x1234");
        INS("eax,dword 0x12345678");
        IF64 INS("rax,qword 0x12345678");

        INS("bl,[ecx]");
        IF64 INS("dil,[ecx]");
        IF64 INS("r15b,[ecx]");
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
        INS("ebx,[ecx]");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");

        INS("bl,byte [ecx]");
        IF64 INS("dil,byte [ecx]");
        IF64 INS("r15b,byte [ecx]");
        INS("bx,word [ecx]");
        IF64 INS("r14w,word [ecx]");
        INS("ebx,dword [ecx]");
        IF64 INS("r14d,dword [ecx]");
        IF64 INS("rbx,qword [ecx]");
        IF64 INS("r14,qword [ecx]");

        INS("[ecx],bl");
        IF64 INS("[ecx],dil");
        IF64 INS("[ecx],r15b");
        INS("[ecx],bx");
        IF64 INS("[ecx],r14w");
        INS("[ecx],ebx");
        IF64 INS("[ecx],r14d");
        IF64 INS("[ecx],rbx");
        IF64 INS("[ecx],r14");

        INS("byte [ecx],bl");
        IF64 INS("byte [ecx],dil");
        IF64 INS("byte [ecx],r15b");
        INS("word [ecx],bx");
        IF64 INS("word [ecx],r14w");
        INS("dword [ecx],ebx");
        IF64 INS("dword [ecx],r14d");
        IF64 INS("qword [ecx],rbx");
        IF64 INS("qword [ecx],r14");

        INS("byte [ecx],44");
        INS("word [ecx],44");
        INS("dword [ecx],44");
        IF64 INS("qword [ecx],44");

        INS("byte [ecx],byte 44");
        INS("word [ecx],byte 44");
        INS("dword [ecx],byte 44");
        IF64 INS("qword [ecx],byte 44");

        INS("byte [ecx],byte 44");
        INS("word [ecx],word 44");
        INS("dword [ecx],dword 44");
        IF64 INS("qword [ecx],qword 44");

        INS("bl,44");
        IF64 INS("dil,44");
        IF64 INS("r15b,44");
        INS("bx,44");
        IF64 INS("r14w,44");
        INS("ebx,44");
        IF64 INS("r14d,44");
        IF64 INS("rbx,44");
        IF64 INS("r14,44");

        INS("bl,byte 44");
        IF64 INS("dil,byte 44");
        IF64 INS("r15b,byte 44");
        INS("bx,byte 44");
        IF64 INS("r14w,byte 44");
        INS("ebx,byte 44");
        IF64 INS("r14d,byte 44");
        IF64 INS("rbx,byte 44");
        IF64 INS("r14,byte 44");

        INS("bl,byte 44");
        IF64 INS("dil,byte 44");
        IF64 INS("r15b,byte 44");
        INS("bx,word 44");
        IF64 INS("r14w,word 44");
        INS("ebx,dword 44");
        IF64 INS("r14d,dword 44");
        IF64 INS("rbx,qword 44");
        IF64 INS("r14,qword 44");
    }
}
void bit(bool with64)
{
	char *names[] = {"bt", "btc", "btr", "bts" };
    FOR(names)
    {
        INS("word [ecx],44");
        INS("dword [ecx],44");
        IF64 INS("qword [ecx],44");

        INS("word [ecx],bx");
        IF64 INS("word [ecx],r14w");
        INS("dword [ecx],ebx");
        IF64 INS("dword [ecx],r14d");
        IF64 INS("qword [ecx],rbx");
        IF64 INS("qword [ecx],r14");
    }

}
void incdec(bool with64)
{
	char *names[] = {"inc", "dec" };
    FOR(names)
    {
        INS("bl");
        IF64 INS("dil");
        IF64 INS("r15b");
        INS("bx");
        IF64 INS("r14w");
        INS("ebx");
        IF64 INS("r14d");
        IF64 INS("rbx");
        IF64 INS("r14");

        INS("byte [ecx]");
        INS("word [ecx]");
        INS("dword [ecx]");
        IF64 INS("qword [ecx]");
    }
}
   
void unary(bool with64)
{
    char *qualified[] = { "neg", "not","div", "idiv", "mul" };
    char *unsized[] =  { "invlpg", "ltr", "prefetchnta", "prefetcht0", "prefetcht1", "prefetcht2",
        "xrstor",  "xsave", };
    char *reg16[] = { "lmsw", "smsw", "verr", "verw" };
    char * reg1632[] = { "str" };
    char *reg32[] = { "bswap" };

    char *unsized64[] = { "xrstor64","xsave64"};
    FOR(qualified)
    {
	if (*p=="xrstor64" || *p == "xsave64")
	    if (!with64)
		continue;
        INS("bl");
        IF64 INS("dil");
        IF64 INS("r15b");
        INS("bx");
        IF64 INS("r14w");
        INS("ebx");
        IF64 INS("r14d");
        IF64 INS("rbx");
        IF64 INS("r14");

        INS("byte [ecx]");
        INS("word [ecx]");
        INS("dword [ecx]");
        IF64 INS("qword [ecx]");
    }
    FOR(unsized)
    {
        INS("[ecx]");
    }
    FOR(reg16)
    {
        INS("bx");
        IF64 INS("r14w");
    }
    FOR(reg1632)
    {
        INS("bx");
        IF64 INS("r14w");
        INS("ebx");
        IF64 INS("r14d");
        IF64 INS("rbx");
        IF64 INS("r14");
    }
    FOR(reg32)
    {
        INS("ebx");
        IF64 INS("r14d");
        IF64 INS("rbx");
        IF64 INS("r14");
    }
    FOR(unsized64)
    {
        IF64 INS("[ecx]");
    }
}
void pushpop(bool with64)
{
    char *names[] = { "pop", "push" };
    FOR(names)
    {
        IFNOT64 INS("bx");
        IFNOT64 INS("ebx");
        IF64 INS("rbx");
        IF64 INS("r14");

        IFNOT64 INS("word [ecx]");
        IFNOT64 INS("dword [ecx]");
        IF64 INS("qword [ecx]");
        INS("fs");
        INS("gs");
        IFNOT64 INS("ds");
        IFNOT64 INS("es");
        IFNOT64 INS("ss");
        IFNOT64 INS("cs");
        
    }
    char **q = &names[1];
    FOR (q)
    {
        INS("44");
        INS("-3");
        INS("0x1234");
        INS("byte 44");
        INS("word 1234");
        INS("dword 0x12345678");
        IF64 INS("qword 0x12345678");
    }
}
void calljmp(bool with64)
{
	char *names[] = { "call", "jmp" };

    FOR (names)
    {
        INS("[fs:0x4444]");
        BINS("");

        IFNOT64 INS("bx");
        IFNOT64 INS("ebx");
        IF64 INS("rbx");
        IF64 INS("r14");
        IFNOT64 INS("word [ecx]");
        IFNOT64 INS("dword [ecx]");
        IF64 INS("qword [ecx]");
        IFNOT64 INS("far word [0x4444]");
        IFNOT64 INS("far word [ecx]");
        IFNOT64 INS("far dword [0x4444]");
        IFNOT64 INS("far dword [ecx]");
    }

    char **p = &names[1];
    BINS("short");
    BINS("near");

}
void branch(bool with64)
{
	char *names[] = {
    "ja","jae","jb","jbe","jc","je","jg","jge","jl","jle","jna",
    "jnae","jnb","jnbe","jnc","jne","jng","jnge","jnl","jnle","jno",
    "jnp","jns","jnz","jo","jp","jpe","jpo","js","jz" };
    char *shortnames[] = { "jcxz", "jecxz", "loop", "loopne", "loope", "loopz", "loopnz" };

    FOR(names)
    {
        BINS("short");
        BINS("near");
        BINS("");
    }
    FOR (shortnames)
    {
	if (strcmp(*p, "jcxz") || !with64)
	        BINS("");
    }
}
void set(bool with64)
{
	char *names[] = {
    "seta","setae","setb","setbe","setc","sete","setg","setge","setl","setle","setna",
    "setnae","setnb","setnbe","setnc","setne","setng","setnge","setnl","setnle","setno",
    "setnp","setns","setnz","seto","setp","setpe","setpo","sets","setz" };
    FOR(names)
    {
        INS("[ecx]");
        INS("byte [ecx]");
    }
}
void cmov(bool with64)
{
	char *names[] = {
    "cmova","cmovae","cmovb","cmovbe","cmovc","cmove","cmovg","cmovge","cmovl","cmovle","cmovna",
    "cmovnae","cmovnb","cmovnbe","cmovnc","cmovne","cmovng","cmovnge","cmovnl","cmovnle","cmovno",
    "cmovnp","cmovns","cmovnz","cmovo","cmovp","cmovpe","cmovpo","cmovs","cmovz" };

    FOR(names)
    {
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
        INS("ebx,[ecx]");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");

        INS("bx,word [ecx]");
        IF64 INS("r14w,word [ecx]");
        INS("ebx,dword [ecx]");
        IF64 INS("r14d,dword [ecx]");
        IF64 INS("rbx,qword [ecx]");
        IF64 INS("r14,qword [ecx]");
    }
}
void movx(bool with64)
{
    char *names[] = {"movsx", "movzx" };
    FOR(names)
    {
        INS("bx,cl");
        IF64 INS("r14w,sil");
        INS("ebx,al");
        IF64 INS("r14d,r11b");
        IF64 INS("rbx,al");
        IF64 INS("r14,sil");
        INS("ebx,cx");
        IF64 INS("r14d,r11w");
        IF64 INS("rbx,ax");
	if (*p == "movsx")
	        IF64 INS ("r15,r12d");

        INS("bx,byte [ecx]");
        IF64 INS("r14w,byte [ecx]");
        INS("ebx,byte [ecx]");
        IF64 INS("r14d,byte [ecx]");
        IF64 INS("rbx,byte [ecx]");
	IF64 INS("r14,byte [ecx]");

        INS("ebx,word [ecx]");
        IF64 INS("r14d,word [ecx]");
        IF64 INS("rbx,word [ecx]");
	IF64 INS("r14,word [ecx]");

	if (*p == "movsx")
	        IF64 INS("rbx,dword [ecx]");
	if (*p == "movsx")
	        IF64 INS("r14,dword [ecx]");
    }
}
void shift(bool with64)
{
    char *names[] = {"rcl","rcr", "rol","ror","sal","sar","shl","shr" };
    FOR(names)
    {
        INS("bl,1");
        IF64 INS("dil,1");
        IF64 INS("r15b,1");
        INS("bx,1");
        IF64 INS("r14w,1");
        INS("ebx,1");
        IF64 INS("r14d,1");
        IF64 INS("rbx,1");
        IF64 INS("r14,1");

        INS("byte [ecx],1");
        INS("word [ecx],1");
        INS("dword [ecx],1");
        IF64 INS("qword [ecx],1");

        INS("bl,3");
        IF64 INS("dil,3");
        IF64 INS("r15b,3");
        INS("bx,3");
        IF64 INS("r14w,3");
        INS("ebx,3");
        IF64 INS("r14d,3");
        IF64 INS("rbx,3");
        IF64 INS("r14,3");

        INS("byte [ecx],3");
        INS("word [ecx],3");
        INS("dword [ecx],3");
        IF64 INS("qword [ecx],3");

        INS("bl,cl");
        IF64 INS("dil,cl");
        IF64 INS("r15b,cl");
        INS("bx,cl");
        IF64 INS("r14w,cl");
        INS("ebx,cl");
        IF64 INS("r14d,cl");
        IF64 INS("rbx,cl");
        IF64 INS("r14,cl");

        INS("byte [ecx],cl");
        INS("word [ecx],cl");
        INS("dword [ecx],cl");
        IF64 INS("qword [ecx],cl");

    }
}
void regshift(bool with64)
{
    char *names[] = {"shld", "shrd" };
    FOR(names)
    {
        INS("word [ecx],bx,1");
        IF64 INS("word [ecx],r14w,1");
        INS("dword [ecx],ebx,1");
        IF64 INS("dword [ecx],r14d,1");
        IF64 INS("qword [ecx],rbx,1");
        IF64 INS("qword [ecx],r14,1");

        INS("word [ecx],bx,cl");
        IF64 INS("word [ecx],r14w,cl");
        INS("dword [ecx],ebx,cl");
        IF64 INS("dword [ecx],r14d,cl");
        IF64 INS("qword [ecx],rbx,cl");
        IF64 INS("qword [ecx],r14,cl");

    }
}
void system(bool with64)
{
    char *legacy[] = {"bound", "lds","les", "lss"};
    char *names[]= { "lar", "lea","lfs", "lgs", "lsl","popcnt"};
    char *names2[] = { "bsf", "bsr" };
    char *nostrict[]={ "lgdt", "lidt", "lldt", "sidt", "sldt" };
    FOR (legacy)
    {
        IFNOT64 INS("bx,[ecx]");
        IFNOT64 INS("ebx,[ecx]");
    }
    FOR (names)
    {
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
        INS("ebx,[ecx]");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");

    }
    FOR (names2)
    {
	INS("bx,cx");
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
	IF64 INS("bx, r14w");
        INS("ebx,[ecx]");
	INS("ebx,ecx");
	IF64 INS("ebx,r14d");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");
	IF64 INS("r15, rcx");
	IF64 INS("rcx, r15");
	IF64 INS("rcx,rdx");

    }
    FOR (nostrict)
    {
        INS("[ecx]");
    }
    char *movbe = "movbe";
    {
        char **p=&movbe;
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
        INS("ebx,[ecx]");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");

        INS("bx,word [ecx]");
        IF64 INS("r14w,word [ecx]");
        INS("ebx,dword [ecx]");
        IF64 INS("r14d,dword [ecx]");
        IF64 INS("rbx,qword [ecx]");
        IF64 INS("r14,qword [ecx]");

        INS("[ecx],bx");
        IF64 INS("[ecx],r14w");
        INS("[ecx],ebx");
        IF64 INS("[ecx],r14d");
        IF64 INS("[ecx],rbx");
        IF64 INS("[ecx],r14");

        INS("word [ecx],bx");
        IF64 INS("word [ecx],r14w");
        INS("dword [ecx],ebx");
        IF64 INS("dword [ecx],r14d");
        IF64 INS("qword [ecx],rbx");
        IF64 INS("qword [ecx],r14");
    }
    SINS("ENTER", "4,4");
    //SINS("ESC","5,[ecx]");
    SINS("IN", "al,dx");
    SINS("IN", "ax,dx");
    SINS("IN", "eax,dx");
//    IF64 SINS("IN", "rax,dx");
    SINS("INT", "22");
    SINS("OUT", "dx,al");
    SINS("OUT", "dx,ax");
    SINS("OUT", "dx,eax");
//    IF64 SINS("OUT", "dx,rax");
}
void rep(char **p, char *q)
{
        RINS("",q);
        RINS("rep",q);
        RINS("repnz",q);
        RINS("repne",q);
        RINS("repz",q);
        RINS("repe",q);
}
void string(bool with64)
{
	char *names[] = { "cmpsb", "cmpsw", "cmpsd", "insb", "insw", "insd", 
    "lodsb","lodsw","lodsd", "lodsw", "movsb", "movsw", "movsd",
    "outsb","outsw","outsd","scasb","scasw","scasd", 
    "stosb", "stosw", "stosd" };
    char *names64[] = { "cmpsq", "lodsq", "movsq", "scasq", "stosq" };
    FOR(names)
    {
        rep(p,"");
    }
    FOR(names64)
    {
        IF64 rep(p,"");
    }
/*
    char *temp = "cmps";
    {
        char **p=&temp;
        IFNOT64 rep(p,"byte [ds:si], byte es:[di]");
        IFNOT64 rep(p,"byte [fs:si], byte es:[di]");
        rep(p,"byte [ds:esi], byte es:[edi]");
        rep(p,"byte [fs:esi], byte es:[edi]");
        IF64 rep(p,"byte [rsi], byte [rdi]");
        IF64 rep(p,"byte [fs:rsi], byte [rdi]");
        IFNOT64 rep(p,"word [ds:si], word es:[di]");
        IFNOT64 rep(p,"word [fs:si], word es:[di]");
        rep(p,"word [ds:esi], word es:[edi]");
        rep(p,"word [fs:esi], word es:[edi]");
        IF64 rep(p,"word [rsi], word [rdi]");
        IF64 rep(p,"word [fs:rsi], word [rdi]");
        IFNOT64 rep(p,"dword [ds:si], dword es:[di]");
        IFNOT64 rep(p,"dword [fs:si], dword es:[di]");
        rep(p,"dword [ds:esi], dword es:[edi]");
        rep(p,"dword [fs:esi], dword es:[edi]");
        IF64 rep(p,"dword [rsi], dword [rdi]");
        IF64 rep(p,"dword [fs:rsi], dword [rdi]");
        IF64 rep(p,"qword [ds:esi], qword es:[edi]");
        IF64 rep(p,"qword [fs:esi], qword es:[edi]");
        IF64 rep(p,"qword [rsi], qword [rdi]");
        IF64 rep(p,"qword [fs:rsi], qword [rdi]");
    }
    temp = "ins";
    {
        char **p=&temp;
        IFNOT64 rep(p,"byte es:[di],dx");
        IFNOT64 rep(p,"byte fs:[di],dx");
        rep(p,"byte es:[edi],dx");
        rep(p,"byte fs:[edi],dx");
        IF64 rep(p,"byte [rdi],dx");
        IF64 rep(p,"byte fs:[rdi],dx");
        IFNOT64 rep(p,"word es:[di],dx");
        IFNOT64 rep(p,"word fs:[di],dx");
        rep(p,"word es:[edi],dx");
        rep(p,"word fs:[edi],dx");
        IF64 rep(p,"word [rdi],dx");
        IF64 rep(p,"word fs:[rdi],dx");
        IFNOT64 rep(p,"dword es:[di],dx");
        IFNOT64 rep(p,"dword fs:[di],dx");
        rep(p,"dword es:[edi],dx");
        rep(p,"dword fs:[edi],dx");
        IF64 rep(p,"dword es:rdi],dx");
        IF64 rep(p,"dword fs:[rdi],dx");
        IF64 rep(p,"qword [ds:esi], qword es:[edi],dx");
        IF64 rep(p,"qword [fs:esi], qword fs:[edi],dx");
        IF64 rep(p,"qword [rsi], qword [rdi],dx");
        IF64 rep(p,"qword [fs:rsi], qword fs:[rdi],dx");
    }
    temp = "lods";
    {
        char **p=&temp;
        IFNOT64 rep(p,"byte [ds:si]");
        IFNOT64 rep(p,"byte [fs:si]");
        rep(p,"byte [ds:esi]");
        rep(p,"byte [fs:esi]");
        IF64 rep(p,"byte [rsi]");
        IF64 rep(p,"byte [fs:rsi]");
        IFNOT64 rep(p,"word [ds:si]");
        IFNOT64 rep(p,"word [fs:si]");
        rep(p,"word [ds:esi]");
        rep(p,"word [fs:esi]");
        IF64 rep(p,"word [rsi]");
        IF64 rep(p,"word [fs:rsi]");
        IFNOT64 rep(p,"dword [ds:si]");
        IFNOT64 rep(p,"dword [fs:si]");
        rep(p,"dword [ds:esi]");
        rep(p,"dword [fs:esi]");
        IF64 rep(p,"dword [rsi]");
        IF64 rep(p,"dword [fs:rsi]");
        IF64 rep(p,"qword [ds:esi]");
        IF64 rep(p,"qword [fs:esi]");
        IF64 rep(p,"qword [rsi]");
        IF64 rep(p,"qword [fs:rsi]");
    }
    temp = "movs";
    {
        char **p=&temp;
        IFNOT64 rep(p,"byte es:[di], byte [ds:si]");
        IFNOT64 rep(p,"byte es:[di], byte [fs:si]");
        rep(p,"byte es:[edi], byte [ds:esi]");
        rep(p,"byte es:[edi], byte [fs:esi]");
        IF64 rep(p,"byte [rdi], byte [rsi]");
        IF64 rep(p,"byte [rdi], byte [fs:rsi]");
        IFNOT64 rep(p,"word es:[di], word [ds:si]");
        IFNOT64 rep(p,"word es:[di], word [fs:si]");
        rep(p,"word es:[edi], word [ds:esi]");
        rep(p,"word es:[edi], word [fs:esi]");
        IF64 rep(p,"word [rdi], word [rsi]");
        IF64 rep(p,"word [rdi], word [fs:rsi]");
        IFNOT64 rep(p,"dword es:[di], dword [ds:si]");
        IFNOT64 rep(p,"dword es:[di], dword [fs:si]");
        rep(p,"dword es:[edi], dword [ds:esi]");
        rep(p,"dword es:[edi], dword [fs:esi]");
        IF64 rep(p,"dword [rdi], dword [rsi]");
        IF64 rep(p,"dword [rdi], dword [fs:rsi]");
        IF64 rep(p,"qword es:[edi], qword [ds:esi]");
        IF64 rep(p,"qword es:[edi], qword [fs:esi]");
        IF64 rep(p,"qword [rdi], qword [rsi]");
        IF64 rep(p,"qword [rdi], qword [fs:rsi]");
    }
    temp = "outs";
    {
        char **p=&temp;
        IFNOT64 rep(p,"dx,byte [ds:si]");
        IFNOT64 rep(p,"dx,byte [fs:si]");
        rep(p,"dx,byte [ds:esi]");
        rep(p,"dx,byte [fs:esi]");
        IF64 rep(p,"dx,byte [rsi]");
        IF64 rep(p,"dx,byte [fs:rsi]");
        IFNOT64 rep(p,"dx,word [ds:si]");
        IFNOT64 rep(p,"dx,word [fs:si]");
        rep(p,"dx,word [ds:esi]");
        rep(p,"dx,word [fs:esi]");
        IF64 rep(p,"dx,word [rsi]");
        IF64 rep(p,"dx,word [fs:rsi]");
        IFNOT64 rep(p,"dx,dword [ds:si]");
        IFNOT64 rep(p,"dx,dword [fs:si]");
        rep(p,"dx,dword [ds:esi]");
        rep(p,"dx,dword [fs:esi]");
        IF64 rep(p,"dx,dword [rsi]");
        IF64 rep(p,"dx,dword [fs:rsi]");
        IF64 rep(p,"dx,qword [ds:esi]");
        IF64 rep(p,"dx,qword [fs:esi]");
        IF64 rep(p,"dx,qword [rsi]");
        IF64 rep(p,"dx,qword [fs:rsi]");
    }
    temp = "scas";
    {
        char **p=&temp;
        IFNOT64 rep(p,"byte es:[di]");
        IFNOT64 rep(p,"byte fs:[di]");
        rep(p,"byte es:[edi]");
        rep(p,"byte fs:[edi]");
        IF64 rep(p,"byte [rdi]");
        IF64 rep(p,"byte fs:[rdi]");
        IFNOT64 rep(p,"word es:[di]");
        IFNOT64 rep(p,"word fs:[di]");
        rep(p,"word es:[edi]");
        rep(p,"word fs:[edi]");
        IF64 rep(p,"word [rdi]");
        IF64 rep(p,"word fs:[rdi]");
        IFNOT64 rep(p,"dword es:[di]");
        IFNOT64 rep(p,"dword fs:[di]");
        rep(p,"dword es:[edi]");
        rep(p,"dword fs:[edi]");
        IF64 rep(p,"dword es:rdi]");
        IF64 rep(p,"dword fs:[rdi]");
        IF64 rep(p,"qword [ds:esi], qword es:[edi]");
        IF64 rep(p,"qword [fs:esi], qword fs:[edi]");
        IF64 rep(p,"qword [rsi], qword [rdi]");
        IF64 rep(p,"qword [fs:rsi], qword fs:[rdi]");
    }
    temp = "stos";
    {
        char **p=&temp;
        IFNOT64 rep(p,"byte es:[di]");
        IFNOT64 rep(p,"byte fs:[di]");
        rep(p,"byte es:[edi]");
        rep(p,"byte fs:[edi]");
        IF64 rep(p,"byte [rdi]");
        IF64 rep(p,"byte fs:[rdi]");
        IFNOT64 rep(p,"word es:[di]");
        IFNOT64 rep(p,"word fs:[di]");
        rep(p,"word es:[edi]");
        rep(p,"word fs:[edi]");
        IF64 rep(p,"word [rdi]");
        IF64 rep(p,"word fs:[rdi]");
        IFNOT64 rep(p,"dword es:[di]");
        IFNOT64 rep(p,"dword fs:[di]");
        rep(p,"dword es:[edi]");
        rep(p,"dword fs:[edi]");
        IF64 rep(p,"dword es:rdi]");
        IF64 rep(p,"dword fs:[rdi]");
        IF64 rep(p,"qword [ds:esi], qword es:[edi]");
        IF64 rep(p,"qword [fs:esi], qword fs:[edi]");
        IF64 rep(p,"qword [rsi], qword [rdi]");
        IF64 rep(p,"qword [fs:rsi], qword fs:[rdi]");
    }
*/
}
void cmpxchg(bool with64)
{
    char *names[]={"cmpxchg","xadd" };
    FOR(names)
    {
        INS("[ecx],bl");
        IF64 INS("[ecx],sil");
        IF64 INS("[ecx],r14b");
        INS("[ecx],bx");
        IF64 INS("[ecx],r14w");
        INS("[ecx],ebx");
        IF64 INS("[ecx],r14d");
        IF64 INS("[ecx],rbx");
        IF64 INS("[ecx],r14");
    }
    char *temp="cmpxchg8b";
    {
        char **p = &temp;
        INS("[ecx]");
    }
    temp="cmpxchg16b";
    {
        char **p = &temp;
        IF64 INS("[ecx]");
    }
}
void imul(bool with64)
{
    char *names[]={"imul"};
    FOR(names)
    {
        INS("ax, 44");
        INS("ax, 0x1234");
        INS("ax, byte 44");
        INS("ax, word 0x1234");
        INS("eax, 44");
        INS("eax, 0x1234");
        INS("eax, byte 44");
        INS("eax, dword 0x1234");
        IF64 INS("rax, 44");
        IF64 INS("rax, 0x1234");
        IF64 INS("rax, byte 44");
        IF64 INS("rax, qword 0x1234");

        INS("ax, [ecx], 44");
        INS("ax, [ecx], byte 44");
        INS("ax, [ecx]");

        INS("eax, [ecx], 123444");
        INS("eax, [ecx], byte 44");
        INS("eax, [ecx]");

        IF64 INS("rax, [ecx], 123444");
        IF64 INS("rax, [ecx], byte 44");
        IF64 INS("rax, [ecx]");

        IF64 INS("r15w, 44");
        IF64 INS("r15w, 0x1234");
        IF64 INS("r15w, byte 44");
        IF64 INS("r15w, word 0x1234");
        IF64 INS("r15d, 44");
        IF64 INS("r15d, 0x1234");
        IF64 INS("r15d, byte 44");
        IF64 INS("r15d, dword 0x1234");
        IF64 INS("r15, 44");
        IF64 INS("r15, 0x1234");
        IF64 INS("r15, byte 44");
        IF64 INS("r15, qword 0x1234");

        IF64 INS("r15w, [ecx], 44");
        IF64 INS("r15w, [ecx], byte 44");
        IF64 INS("r15w, [ecx]");

        IF64 INS("r15d, [ecx], 123444");
        IF64 INS("r15d, [ecx], byte 44");
        IF64 INS("r15d, [ecx]");

        IF64 INS("r15, [ecx], 123444");
        IF64 INS("r15, [ecx], byte 44");
        IF64 INS("r15, [ecx]");

	INS("byte [ecx]");
	INS("word [ecx]");
	INS("dword [ecx]");
	IF64 INS("qword [ecx]");
	IF64 INS("byte [rcx]");
	IF64 INS("word [rcx]");
	IF64 INS("dword [rcx]");
	IF64 INS("qword [rcx]");
	IF64 INS("byte [r13]");
	IF64 INS("word [r13]");
	IF64 INS("dword [r13]");
	IF64 INS("qword [r13]");
    }
}
void mov(bool with64)
{
    char *names[]={"mov"};
    FOR(names)
    {
        IFNOT64 INS("cr4,ecx");
        IFNOT64 INS("dr5,ecx");
        IFNOT64 INS("tr3,ecx");
        IFNOT64 INS("ecx,cr4");
        IFNOT64 INS("ecx,dr5");
        IFNOT64 INS("ecx,tr3");
        IFNOT64 INS("al, [0x1234]");
        IFNOT64 INS("al, [fs:0x1234]");
        INS("al, [0x12345678]");
        INS("al, [fs:0x12345678]");
        IFNOT64 INS("al, byte [0x1234]");
        IFNOT64 INS("al, byte [fs:0x1234]");
        INS("al, byte [0x12345678]");
        INS("al, byte [fs:0x12345678]");
        IFNOT64 INS("ax, [0x1234]");
        IFNOT64 INS("ax, [fs:0x1234]");
        INS("ax, [0x12345678]");
        INS("ax, [fs:0x12345678]");
        IFNOT64 INS("ax, word [0x1234]");
        IFNOT64 INS("ax, word [fs:0x1234]");
        INS("ax, word [0x12345678]");
        INS("ax, word [fs:0x12345678]");
        IFNOT64 INS("eax, [0x1234]");
        IFNOT64 INS("eax, [fs:0x1234]");
        INS("eax, [0x12345678]");
        INS("eax, [fs:0x12345678]");
        IFNOT64 INS("eax, dword [0x1234]");
        IFNOT64 INS("eax, dword [fs:0x1234]");
        INS("eax, dword [0x12345678]");
        INS("eax, dword [fs:0x12345678]");
        IF64 INS("rax, [0x12345678]");
        IF64 INS("rax, [fs:0x12345678]");
        IF64 INS("rax, qword [0x12345678]");
        IF64 INS("rax, qword [fs:0x12345678]");

        IFNOT64 INS("[0x1234], al");
        IFNOT64 INS("[fs:0x1234], al");
        INS("[0x12345678], al");
        INS("[fs:0x12345678], al");
        IFNOT64 INS("byte [0x1234], al");
        IFNOT64 INS("byte [fs:0x1234], al");
        INS("byte [0x12345678], al");
        INS("byte [fs:0x12345678], al");
        IFNOT64 INS("[0x1234], ax");
        IFNOT64 INS("[fs:0x1234], ax");
        INS("[0x12345678], ax");
        INS("[fs:0x12345678], ax");
        IFNOT64 INS("word [0x1234], ax");
        IFNOT64 INS("word [fs:0x1234], ax");
        INS("word [0x12345678], ax");
        INS("word [fs:0x12345678], ax");
        IFNOT64 INS("[0x1234], eax");
        IFNOT64 INS("[fs:0x1234], eax");
        INS("[0x12345678], eax");
        INS("[fs:0x12345678], eax");
        IFNOT64 INS("dword [0x1234], eax");
        IFNOT64 INS("dword [fs:0x1234], eax");
        INS("dword [0x12345678], eax");
        INS("dword [fs:0x12345678], eax");
        IF64 INS("[0x12345678], rax");
        IF64 INS("[fs:0x12345678], rax");
        IF64 INS("qword [0x12345678], rax");
        IF64 INS("qword [fs:0x12345678], rax");

        INS("al,5");
        INS("ax,0x1234");
        INS("eax,0x12345678");
        IF64 INS("rax,0x12345678");

        INS("al,byte 5");
        INS("ax,word 0x1234");
        INS("eax,dword 0x12345678");
        IF64 INS("rax,qword 0x12345678");

        INS("bl,[ecx]");
        IF64 INS("dil,[ecx]");
        IF64 INS("r15b,[ecx]");
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
        INS("ebx,[ecx]");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");

        INS("bl,byte [ecx]");
        IF64 INS("dil,byte [ecx]");
        IF64 INS("r15b,byte [ecx]");
        INS("bx,word [ecx]");
        IF64 INS("r14w,word [ecx]");
        INS("ebx,dword [ecx]");
        IF64 INS("r14d,dword [ecx]");
        IF64 INS("rbx,qword [ecx]");
        IF64 INS("r14,qword [ecx]");

        INS("[ecx],bl");
        IF64 INS("[ecx],dil");
        IF64 INS("[ecx],r15b");
        INS("[ecx],bx");
        IF64 INS("[ecx],r14w");
        INS("[ecx],ebx");
        IF64 INS("[ecx],r14d");
        IF64 INS("[ecx],rbx");
        IF64 INS("[ecx],r14");

        INS("byte [ecx],bl");
        IF64 INS("byte [ecx],dil");
        IF64 INS("byte [ecx],r15b");
        INS("word [ecx],bx");
        IF64 INS("word [ecx],r14w");
        INS("dword [ecx],ebx");
        IF64 INS("dword [ecx],r14d");
        IF64 INS("qword [ecx],rbx");
        IF64 INS("qword [ecx],r14");

        INS("byte [ecx],44");
        INS("word [ecx],44");
        INS("dword [ecx],44");
        IF64 INS("qword [ecx],44");

        INS("byte [ecx],byte 44");
        INS("word [ecx],word 44");
        INS("dword [ecx],dword 44");
        IF64 INS("qword [ecx],qword 44");

        INS("bl,44");
        IF64 INS("dil,44");
        IF64 INS("r15b,44");
        INS("bx,44");
        IF64 INS("r14w,44");
        INS("ebx,44");
        IF64 INS("r14d,44");
        IF64 INS("rbx,44");
        IF64 INS("r14,44");

        INS("bl,byte 44");
        IF64 INS("dil,byte 44");
        IF64 INS("r15b,byte 44");
        INS("bx,word 44");
        IF64 INS("r14w,word 44");
        INS("ebx,dword 44");
        IF64 INS("r14d,dword 44");
        IF64 INS("rbx,qword 44");
        IF64 INS("r14,qword 44");

        INS("fs, bx");
        IF64 INS("fs, r15w");
        INS("fs, ebx");
        IF64 INS("fs, r15d");
        IF64 INS("fs, rbx");
        IF64 INS("fs, r15");
        INS("fs, [ecx]");
        INS("fs, word [ecx]");
        INS("bx, fs");
        IF64 INS("r15w, fs");
        INS("ebx, fs");
        IF64 INS("r15d, fs");
        INS("[ecx], fs");
        INS("word [ecx], fs");
    }

}
void ret(bool with64)
{
    char *names[] = { "ret","retf" };
    FOR (names)
    {
        INS("");
        INS("0x1234");
    }
}
void test(bool with64)
{
    char *names[] = { "test" };
    FOR (names)
    {
        INS("al,5");
        INS("ax,0x1234");
        INS("eax,0x12345678");

        IF64 INS("rax,0x12345678");
        INS("bl,[ecx]");
        IF64 INS("dil,[ecx]");
        IF64 INS("r15b,[ecx]");
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
        INS("ebx,[ecx]");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");

        INS("bl,byte [ecx]");
        IF64 INS("dil,byte [ecx]");
        IF64 INS("r15b,byte [ecx]");
        INS("bx,word [ecx]");
        IF64 INS("r14w,word [ecx]");
        INS("ebx,dword [ecx]");
        IF64 INS("r14d,dword [ecx]");
        IF64 INS("rbx,qword [ecx]");
        IF64 INS("r14,qword [ecx]");

        INS("[ecx],bl");
        IF64 INS("[ecx],dil");
        IF64 INS("[ecx],r15b");
        INS("[ecx],bx");
        IF64 INS("[ecx],r14w");
        INS("[ecx],ebx");
        IF64 INS("[ecx],r14d");
        IF64 INS("[ecx],rbx");
        IF64 INS("[ecx],r14");

        INS("byte [ecx],bl");
        IF64 INS("byte [ecx],dil");
        IF64 INS("byte [ecx],r15b");
        INS("word [ecx],bx");
        IF64 INS("word [ecx],r14w");
        INS("dword [ecx],ebx");
        IF64 INS("dword [ecx],r14d");
        IF64 INS("qword [ecx],rbx");
        IF64 INS("qword [ecx],r14");


        INS("bl,44");
        IF64 INS("dil,44");
        IF64 INS("r15b,44");
        INS("bx,44");
        IF64 INS("r14w,44");
        INS("ebx,44");
        IF64 INS("r14d,44");
        IF64 INS("rbx,44");
        IF64 INS("r14,44");

        INS("bl,byte 44");
        IF64 INS("dil,byte 44");
        IF64 INS("r15b,byte 44");
        INS("bx,word 44");
        IF64 INS("r14w,word 44");
        INS("ebx,dword 44");
        IF64 INS("r14d,dword 44");
        IF64 INS("rbx,qword 44");
        IF64 INS("r14,qword 44");

        INS("byte [ecx],44");
        INS("word [ecx],44");
        INS("dword [ecx],44");
        IF64 INS("qword [ecx],44");

        INS("byte [ecx],byte 44");
        INS("word [ecx],word 44");
        INS("dword [ecx],dword 44");
        IF64 INS("qword [ecx],qword 44");

	INS("bl, cl");
	INS("bl, al");
	IF64 INS("bl, sil");
        
	INS("bx, cx");
	IF64 INS("bx, r14w");
	IF64 INS("r14w, bx");
	INS("ebx, ecx");
	IF64 INS("ebx, r14d");
	IF64 INS("r14d, ebx");
	IF64 INS("rbx, rcx");
	IF64 INS("rbx, r14");
	IF64 INS("r14, rbx");

    }
}

void xchg(bool with64)
{
    char *names[] = { "xchg" };
    FOR (names)
    {
        INS("ax,cx");
        INS("eax,ecx");
        IF64 INS("rax,rcx");
        IF64 INS("ax,r11w");
        IF64 INS("eax,r10d");
        IF64 INS("rax,r15");
        INS("cx,ax");
        INS("ecx,eax");
        IF64 INS("rcx,rax");
        IF64 INS("r11w,ax");
        IF64 INS("r10d,eax");
        IF64 INS("r15,rax");
        INS("bl,[ecx]");
        IF64 INS("dil,[ecx]");
        IF64 INS("r15b,[ecx]");
        INS("bx,[ecx]");
        IF64 INS("r14w,[ecx]");
        INS("ebx,[ecx]");
        IF64 INS("r14d,[ecx]");
        IF64 INS("rbx,[ecx]");
        IF64 INS("r14,[ecx]");

        IF64 INS("r15b, al");
        IF64 INS("sil,al");
        INS("bl, al");
        IF64 INS("al, r15b");
        IF64 INS("al, sil");
        INS("al, bl");

         INS("bl,byte [ecx]");
        IF64 INS("dil,byte [ecx]");
        IF64 INS("r15b,byte [ecx]");
        INS("bx,word [ecx]");
        IF64 INS("r14w,word [ecx]");
        INS("ebx,dword [ecx]");
        IF64 INS("r14d,dword [ecx]");
        IF64 INS("rbx,qword [ecx]");
        IF64 INS("r14,qword [ecx]");

        INS("[ecx],bl");
        IF64 INS("[ecx],dil");
        IF64 INS("[ecx],r15b");
        INS("[ecx],bx");
        IF64 INS("[ecx],r14w");
        INS("[ecx],ebx");
        IF64 INS("[ecx],r14d");
        IF64 INS("[ecx],rbx");
        IF64 INS("[ecx],r14");

    }
}
void xlat(bool with64)
{
    char *names[]={"xlat"};
    FOR(names)
    {
        INS("");
/*
        IFNOT64 INS("[ds:bx]");
        IFNOT64 INS("[fs:bx]");
        IFNOT64 INS("byte [ds:bx]");
        IFNOT64 INS("byte [fs:bx]");
        INS("[ds:ebx]");
        INS("[fs:ebx]");
        INS("byte [ds:ebx]");
        INS("byte [fs:ebx]");
        IF64 INS("[ds:rbx]");
        IF64 INS("[fs:rbx]");
        IF64 INS("byte [ds:rbx]");
        IF64 INS("byte [fs:rbx]");
*/
    }
}
void arpl(bool with64)
{
    char *names[] = { "arpl" };
    FOR(names)
    {
	INS("[ecx], bx");
	IF64 INS("[ecx],r13d");
    }
}
void mainline(bool with64)
{
    single(with64);
    math(with64);
    bit(with64);
    incdec(with64);
    unary(with64);
    pushpop(with64);
    calljmp(with64);
    branch(with64);
    set(with64);
    cmov(with64);
    movx(with64);
    shift(with64);
    regshift(with64);
    system(with64);
    string(with64);
    cmpxchg(with64);
    imul(with64);
    mov(with64);
    ret(with64);
    test(with64);
    xchg(with64);
    xlat(with64);
}
int main()
{
    out = fopen("int.asm","w");
    fprintf(out, "[bits 16]\n");
    mainline(false);
    bits(32);
    mainline(false);
    bits(64);
    mainline(true);
    fclose(out);
}