#include <string.h>
#include <stdio.h>

#define INS "not"

FILE *out;

char *reg8a[16] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
char *reg8b[16] = { "al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
char *regw[16] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
char *regd[16] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
char *regq[16] = { "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
char *segreg[6] = { "ds", "es", "ss", "cs", "fs", "gs" };
char *base16[8] = { "bx", "bp","si","di","bx+si","bx+di","bp+si","bp+di" };

void guard()
{
    fprintf(out,"nop\n");
    fprintf(out,"nop\n");
    fprintf(out,"nop\n");
}
void repreg8(int with64)
{
    for (int i=0; i < 8; i++)
    {
        fprintf(out,"\t%s\t%s\n", INS, reg8a[i]);
        guard();
    }
    if (with64)
    {
        for (int i=0; i < 8; i++)
        {
            fprintf(out,"\t%s\t%s\n", INS, reg8a[i+8]);
            guard();
        }
        for (int i=0; i < 4; i++)
        {
            fprintf(out,"\t%s\t%s\n", INS, reg8b[i+4]);
            guard();
        }
    }
}
void repreg16(int with64)
{
    for (int i=0; i < 8; i++)
    {
        fprintf(out,"\t%s\t%s\n", INS, regw[i]);
        guard();
    }
    if (with64)
    {
        for (int i=0; i < 8; i++)
        {
            fprintf(out,"\t%s\t%s\n", INS, regw[i+8]);
            guard();
        }
    }
}
void repreg32(int with64)
{
    for (int i=0; i < 8; i++)
    {
        fprintf(out,"\t%s\t%s\n", INS, regd[i]);
        guard();
    }
    if (with64)
    {
        for (int i=0; i < 8; i++)
        {
            fprintf(out,"\t%s\t%s\n", INS, regd[i+8]);
            guard();
        }
    }
}
void repreg64()
{
    for (int i=0; i < 16; i++)
    {
        fprintf(out,"\t%s\t%s\n", INS, regq[i]);
        guard();
    }
}
void repaddr16(char *seg, char *mod)
{
 
    for (int i=0; i < 8; i++)
    {
        if (seg)
        {
            fprintf(out,"\t%s\tbyte [%s:%s%s]\n", INS, seg, base16[i],mod);
            guard();
            fprintf(out,"\t%s\tword [%s:%s%s]\n", INS, seg, base16[i],mod);
            guard();
            fprintf(out,"\t%s\tdword [%s:%s%s]\n", INS, seg, base16[i],mod);
            guard();
        }
        else
        {
            fprintf(out,"\t%s\tbyte [%s%s]\n", INS, base16[i],mod);
            guard();
            fprintf(out,"\t%s\tword [%s%s]\n", INS, base16[i],mod);
            guard();
            fprintf(out,"\t%s\tdword [%s%s]\n", INS, base16[i],mod);
            guard();
        }
    }
}
void baseins(char *ins, int with64, char *seg, char* basein, char * index, char *qual)
{
    char base[256];
    if (basein)
        sprintf(base, "%s+", basein);
    else
        base[0] = 0;
    fprintf(out,"\t%s\tbyte [", INS);
    if (seg)
        fprintf(out,"%s:", seg);
    fprintf(out,"%s%s%s]\n", base, index, qual);
            guard();
    fprintf(out,"\t%s\tword [", INS);
    if (seg)
        fprintf(out,"%s:", seg);
    fprintf(out,"%s%s%s]\n", base, index, qual);
            guard();
    fprintf(out,"\t%s\tdword [", INS);
    if (seg)
        fprintf(out,"%s:", seg);
    fprintf(out,"%s%s%s]\n", base, index, qual);
            guard();
    if (with64)
    {
        fprintf(out,"\t%s\tqword [", INS);
        if (seg)
            fprintf(out,"%s:", seg);
        fprintf(out,"%s%s%s]\n", base, index, qual);
            guard();

    }
}
void repaddr32_64(int with64, int with64_2, char **regs, char *seg)
{
    for (int i=0; i < (with64+1)*8; i++)
    {
        if (seg)
        {
            fprintf(out,"\t%s\tbyte [%s:%s]\n", INS, seg, regs[i]);
            guard();
            fprintf(out,"\t%s\tword [%s:%s]\n", INS, seg, regs[i]);
            guard();
            fprintf(out,"\t%s\tdword [%s:%s]\n", INS, seg, regs[i]);
            guard();
            if (with64)
            {
                fprintf(out,"\t%s\tqword [%s:%s]\n", INS, seg, regs[i]);
                guard();
            }
        }
        else
        {
            fprintf(out,"\t%s\tbyte [%s]\n", INS, regs[i]);
            guard();
            fprintf(out,"\t%s\tword [%s]\n", INS, regs[i]);
            guard();
            fprintf(out,"\t%s\tdword [%s]\n", INS, regs[i]);
            guard();
            if (with64)
            {
                fprintf(out,"\t%s\tqword [%s]\n", INS, regs[i]);
                guard();
            }
        }
        for (int j=0; j < (with64+1)*8; j++)
        {
            if(regs[i] != regs[j] || i != 4) // esp no index
            {
                baseins(INS,with64_2,seg,regs[i],regs[j],"");
                baseins(INS,with64_2,seg,regs[i],regs[j],"+0");
                baseins(INS,with64_2,seg,regs[i],regs[j],"+5");
                baseins(INS,with64_2,seg,regs[i],regs[j],"+0x23456789");
            }
            if (j != 4) // ESP no index
            {
                baseins(INS,with64_2,seg,regs[i],regs[j],"*1");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*1+0");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*1+5");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*1+0x23456789");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*2");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*2+0");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*2+5");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*2+0x23456789");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*4");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*4+0");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*4+5");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*4+0x23456789");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*8");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*8+0");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*8+5");
                baseins(INS,with64_2,seg,regs[i],regs[j],"*8+0x23456789");
            }
        }
        if (i != 4) // ESP no index
        {
            baseins(INS,with64_2,seg,0, regs[i],"*1");
            baseins(INS,with64_2,seg,0, regs[i],"*1+0");
            baseins(INS,with64_2,seg,0, regs[i],"*1+5");
            baseins(INS,with64_2,seg,0, regs[i],"*1+0x23456789");
            baseins(INS,with64_2,seg,0, regs[i],"*2");
            baseins(INS,with64_2,seg,0, regs[i],"*2+0");
            baseins(INS,with64_2,seg,0, regs[i],"*2+5");
            baseins(INS,with64_2,seg,0, regs[i],"*2+0x23456789");
            baseins(INS,with64_2,seg,0, regs[i],"*4");
            baseins(INS,with64_2,seg,0, regs[i],"*4+0");
            baseins(INS,with64_2,seg,0, regs[i],"*4+5");
            baseins(INS,with64_2,seg,0, regs[i],"*4+0x23456789");
            baseins(INS,with64_2,seg,0, regs[i],"*8");
            baseins(INS,with64_2,seg,0, regs[i],"*8+0");
            baseins(INS,with64_2,seg,0, regs[i],"*8+5");
            baseins(INS,with64_2,seg,0, regs[i],"*8+0x23456789");
        }
    }
    fprintf(out,"\t%s\tbyte [0x1234567]\n", INS);
    guard();
    if (seg)
    {
        fprintf(out,"\t%s\tbyte [%s:0x1234567]\n", INS, seg);
        guard();
    }
    else 
    {
        fprintf(out,"\t%s\tbyte [0x1234567]\n", INS);
        guard();
    }

}

void sixteen32()
{
    repreg8(false);
    repreg16(false);
    repreg32(false);
    repaddr16(0,"");
    repaddr16(0,"+5");
    repaddr16(0,"+1234");
    fprintf(out,"\t%s\tbyte [0x1234567]\n", INS);
    guard();


    for (int i=0; i < 6; i++)
    {
        repaddr16(segreg[i],"");
        repaddr16(segreg[i],"+5");
        repaddr16(segreg[i],"+1234");
        fprintf(out,"\t%s\tbyte [%s:0x1234567]\n", INS, segreg[i]);
        guard();
    }

    repaddr32_64(false, false, regd, 0);
    for (int i=0; i < 6; i++)
    {
        repaddr32_64(false, false, regd, segreg[i]);
    }
}
int main()
{
    out = fopen("addr16.asm","w");
    fprintf(out, "[bits 16]\n");
    sixteen32();
    fclose(out);
    out = fopen("addr32.asm","w");
    fprintf(out, "[bits 32]\n");
    sixteen32();
    fclose(out);
    out = fopen("addr64.asm","w");
    fprintf(out, "[bits 64]\n");
    repreg8(true);
    repreg16(true);
    repreg32(true);
    repreg64();
    repaddr32_64(false, true, regd, 0);
    for (int i=4; i < 6; i++)
        repaddr32_64(false, true, regd, segreg[i]);
    repaddr32_64(true, true, regq, 0);
    for (int i=4; i < 6; i++)
        repaddr32_64(true, true, regq, segreg[i]);

    fclose(out);
}