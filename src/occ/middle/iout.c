/*
    Software License Agreement (BSD License)

    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/*
 * iout.c
 *
 * output routines for icode code gen->  Used only in optimizer tests.
 *
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"
/*      variable initialization         */
extern int tempCount;
extern BITINT bittab[BITINTBITS];
extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;
extern LIST *globalCache;
extern QUAD *intermed_head;
extern int cachedTempCount;
extern TEMP_INFO **tempInfo;
extern LIST *localfuncs, *localdata;
extern FILE *icdFile;
extern char outfile[];
extern LIST *libincludes;
extern int nextLabel;
extern LIST *externals;
extern SYMBOL *theCurrentFunc;
extern unsigned termCount;
extern QUAD *criticalThunks;
extern int cachedTempCount;

QUAD *currentQuad ;

int gentype = nogen; /* Current DC type */
int curseg = noseg; /* Current seg */
int outcol = 0; /* Curront col (roughly) */
char dataname[40]; /* Name of last label */

static int virtual_mode;
static STRING *strtab;
static int newlabel;

void putamode(QUAD *q, IMODE *ap);
void nl(void);
void xstringseg(void);

/* Init module */
void outcodeini(void)
{
    gentype = nogen;
    curseg = noseg;
    outcol = 0;
    virtual_mode = 0;
    newlabel = FALSE;
    strtab = NULL;
} 
static void iop_nop(QUAD *q)
{
    (void)q;
    oprintf(icdFile,"\tNOP");
}
static void iop_phi(QUAD *q)
{
    PHIDATA *phi = q->dc.v.phi;
    struct _phiblock *pb = phi->temps;
    EXPRESSION *enode = tempInfo[phi->T0]->enode;
    if (enode->right)
        oprintf(icdFile,"\tT%d[%s] = PHI(", phi->T0, ((SYMBOL *)(enode->right))->name);
    else
        oprintf(icdFile,"\tT%d = PHI(", phi->T0);
    while (pb)
    {
        oprintf(icdFile,"T%d(%d),", pb->Tn, pb->block->blocknum+1);
        pb = pb->next;
    }
    oprintf(icdFile,")\n");
}
/*
 * ICODE op display handlers
 */
static void iop_line(QUAD *q)
{
    if (chosenAssembler->gen->asm_line)
        chosenAssembler->gen->asm_line(q);
       if (cparams.prm_lines)
    {
        LINEDATA *ld = (LINEDATA *)q->dc.left;
        while (ld)
        {
            oprintf(icdFile, "; Line %d:\t%s\n", ld->lineno, ld->line);
            ld = ld->next;
        }
    }
}

/*-------------------------------------------------------------------------*/

static void iop_passthrough(QUAD *q)
{
    if (chosenAssembler->gen->asm_passthrough)
        chosenAssembler->gen->asm_passthrough(q);
    oprintf(icdFile, "%s\n", (char*)q->dc.left);
}
static void iop_datapassthrough(QUAD *q)
{
    if (chosenAssembler->gen->asm_datapassthrough)
        chosenAssembler->gen->asm_datapassthrough(q);
    oprintf(icdFile, "ASM DATA\n");
}
static void iop_skipcompare(QUAD *q)
{
    oprintf(icdFile, "\tskipcompare %d\n", q->dc.v.label);
}
/*-------------------------------------------------------------------------*/

static void iop_label(QUAD *q)
{
    if (chosenAssembler->gen->asm_label)
        chosenAssembler->gen->asm_label(q);
    nl();
    oprintf(icdFile, "L_%d:", q->dc.v.label);
    gentype = nogen;
}

/*-------------------------------------------------------------------------*/

static void putsingle(QUAD *q, IMODE *ap, char *string)
{
    if (!icdFile)
        return ;
    oprintf(icdFile, "\t%s", string);
    if (ap)
    {
        oputc('\t', icdFile);
        putamode(q, ap);
    }
}

/*-------------------------------------------------------------------------*/

static void iop_asmgoto(QUAD *q)
{
    oprintf(icdFile, "\tASMGOTO\tL_%d:PC", q->dc.v.label);
}
/*-------------------------------------------------------------------------*/

static void iop_goto(QUAD *q)
{
    if (chosenAssembler->gen->asm_goto)
        chosenAssembler->gen->asm_goto(q);
    oprintf(icdFile, "\tGOTO\tL_%d:PC", q->dc.v.label);
}
static void iop_directbranch(QUAD *q)
{
    if (chosenAssembler->gen->asm_goto)
        chosenAssembler->gen->asm_goto(q);
    putsingle(q, q->dc.left, "GOTO");
}

/*-------------------------------------------------------------------------*/

static void iop_gosub(QUAD *q)
{
    if (chosenAssembler->gen->asm_gosub)
        chosenAssembler->gen->asm_gosub(q);
    putsingle(q, q->dc.left, "GOSUB");
}

static void iop_fargosub(QUAD *q)
{
    if (chosenAssembler->gen->asm_fargosub)
        chosenAssembler->gen->asm_fargosub(q);
    putsingle(q, q->dc.left, "FARGOSUB");
}
/*-------------------------------------------------------------------------*/

static void iop_trap(QUAD *q)
{
    if (chosenAssembler->gen->asm_trap)
        chosenAssembler->gen->asm_trap(q);
    putsingle(q, q->dc.left, "TRAP");
}

/*-------------------------------------------------------------------------*/

static void iop_int(QUAD *q)
{
    if (chosenAssembler->gen->asm_int)
        chosenAssembler->gen->asm_int(q);
    putsingle(q, q->dc.left, "INT");
}

/*-------------------------------------------------------------------------*/

static void iop_ret(QUAD *q)
{
    if (chosenAssembler->gen->asm_ret)
        chosenAssembler->gen->asm_ret(q);
    oprintf(icdFile, "\tRET\t");
    putamode(q, q->dc.left);
}

/*-------------------------------------------------------------------------*/

static void iop_fret(QUAD *q)
{
    if (chosenAssembler->gen->asm_fret)
        chosenAssembler->gen->asm_fret(q);
    oprintf(icdFile, "\tFRET\t");
    putamode(q, q->dc.left);
}

/*-------------------------------------------------------------------------*/

static void iop_rett(QUAD *q)
{
    if (chosenAssembler->gen->asm_rett)
        chosenAssembler->gen->asm_rett(q);
    oprintf(icdFile, "\tRETT\n");
}

/*-------------------------------------------------------------------------*/

static void putbin(QUAD *q, char *str)
{
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " = ");
    putamode(q, q->dc.left);
    oprintf(icdFile, " %s ", str);
    putamode(q, q->dc.right);
}

/*-------------------------------------------------------------------------*/

static void iop_add(QUAD *q)
{
    if (chosenAssembler->gen->asm_add)
        chosenAssembler->gen->asm_add(q);
    putbin(q, "+");
}

/*-------------------------------------------------------------------------*/

static void iop_sub(QUAD *q)
{
    if (chosenAssembler->gen->asm_sub)
        chosenAssembler->gen->asm_sub(q);
    putbin(q, "-");
}

/*-------------------------------------------------------------------------*/

static void iop_udiv(QUAD *q)
{
    if (chosenAssembler->gen->asm_udiv)
        chosenAssembler->gen->asm_udiv(q);
    putbin(q, "U/");
}

/*-------------------------------------------------------------------------*/

static void iop_umod(QUAD *q)
{
    if (chosenAssembler->gen->asm_umod)
        chosenAssembler->gen->asm_umod(q);
    putbin(q, "U%");
}

/*-------------------------------------------------------------------------*/

static void iop_sdiv(QUAD *q)
{
    if (chosenAssembler->gen->asm_sdiv)
        chosenAssembler->gen->asm_sdiv(q);
    putbin(q, "S/");
}

/*-------------------------------------------------------------------------*/

static void iop_smod(QUAD *q)
{
    if (chosenAssembler->gen->asm_smod)
        chosenAssembler->gen->asm_smod(q);
    putbin(q, "S%");
}

/*-------------------------------------------------------------------------*/
static void iop_muluh(QUAD *q)
{
    if (chosenAssembler->gen->asm_muluh)
        chosenAssembler->gen->asm_muluh(q);
    putbin(q, "U*H");
}
static void iop_mulsh(QUAD *q)
{
    if (chosenAssembler->gen->asm_mulsh)
        chosenAssembler->gen->asm_mulsh(q);
    putbin(q, "S*H");
}

static void iop_mul(QUAD *q)
{
    if (chosenAssembler->gen->asm_mul)
        chosenAssembler->gen->asm_mul(q);
    putbin(q, "*");
}

/*-------------------------------------------------------------------------*/

static void iop_lsl(QUAD *q)
{
    if (chosenAssembler->gen->asm_lsl)
        chosenAssembler->gen->asm_lsl(q);
    putbin(q, "<<");
}

/*-------------------------------------------------------------------------*/

static void iop_lsr(QUAD *q)
{
    if (chosenAssembler->gen->asm_lsr)
        chosenAssembler->gen->asm_lsr(q);
    putbin(q, "U>>");
}

/*-------------------------------------------------------------------------*/

static void iop_asr(QUAD *q)
{
    if (chosenAssembler->gen->asm_asr)
        chosenAssembler->gen->asm_asr(q);
    putbin(q, "S>>");
}

/*-------------------------------------------------------------------------*/

static void iop_and(QUAD *q)
{
    if (chosenAssembler->gen->asm_and)
        chosenAssembler->gen->asm_and(q);
    putbin(q, "&");
}

/*-------------------------------------------------------------------------*/

static void iop_or(QUAD *q)
{
    if (chosenAssembler->gen->asm_or)
        chosenAssembler->gen->asm_or(q);
    putbin(q, "|");
}

/*-------------------------------------------------------------------------*/

static void iop_eor(QUAD *q)
{
    if (chosenAssembler->gen->asm_eor)
        chosenAssembler->gen->asm_eor(q);
    putbin(q, "^");
}

/*-------------------------------------------------------------------------*/

static void putunary(QUAD *q, char *str)
{
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " = ");
    oprintf(icdFile, " %s ", str);
    putamode(q, q->dc.left);
}

/*-------------------------------------------------------------------------*/

static void putasunary(QUAD *q, char *str)
{
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    if (q->ans)
        putamode(q, q->ans);
    oprintf(icdFile, " %s ", str);
    putamode(q, q->dc.left);
}

/*-------------------------------------------------------------------------*/

static void iop_neg(QUAD *q)
{
    if (chosenAssembler->gen->asm_neg)
        chosenAssembler->gen->asm_neg(q);
    putunary(q, "-");
}

/*-------------------------------------------------------------------------*/

static void iop_not(QUAD *q)
{
    if (chosenAssembler->gen->asm_not)
        chosenAssembler->gen->asm_not(q);
    putunary(q, "~");
}

/*-------------------------------------------------------------------------*/

static void iop_assn(QUAD *q)
{
    if (chosenAssembler->gen->asm_assn)
        chosenAssembler->gen->asm_assn(q);
    putunary(q, "");
}

/*-------------------------------------------------------------------------*/

static void iop_genword(QUAD *q)
{
    if (chosenAssembler->gen->asm_genword)
        chosenAssembler->gen->asm_genword(q);
    putsingle(q, q->dc.left, "genword");
}

/*-------------------------------------------------------------------------*/

static void iop_coswitch(QUAD *q)
{
    if (chosenAssembler->gen->asm_coswitch)
        chosenAssembler->gen->asm_coswitch(q);
    if (!icdFile)
        return;
    oprintf(icdFile, "\tCOSWITCH(");
    putamode(q, q->dc.left);
    oputc(',', icdFile);
    putamode(q, q->ans);
    oputc(',', icdFile);
    putamode(q, q->dc.right);
    oputc(',', icdFile);
    oprintf(icdFile, "L_%d:PC)", q->dc.v.label);
}
static void iop_swbranch(QUAD *q)
{
    if (chosenAssembler->gen->asm_swbranch)
        chosenAssembler->gen->asm_swbranch(q);
    if (!icdFile)
        return;
    oprintf(icdFile, "\tSWBRANCH(");
    putamode(q, q->dc.left);
    oputc(',', icdFile);
    oprintf(icdFile, "L_%d:PC)", q->dc.v.label);
}

/*-------------------------------------------------------------------------*/

static void iop_array(QUAD *q)
{
    if (chosenAssembler->gen->asm_add)
        chosenAssembler->gen->asm_add(q);
    if (!icdFile)
        return;
    oputc('\t',icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " = ");
    oprintf(icdFile, " ARRAY ");
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}

/*-------------------------------------------------------------------------*/

static void iop_arrayindex(QUAD *q)
{
    if (chosenAssembler->gen->asm_mul)
        chosenAssembler->gen->asm_mul(q);
    if (!icdFile)
        return;
    oputc('\t',icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " = ");
    oprintf(icdFile, " ARRIND ");
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}
/*-------------------------------------------------------------------------*/

static void iop_arraylsh(QUAD *q)
{
    if (chosenAssembler->gen->asm_mul)
        chosenAssembler->gen->asm_mul(q);
    if (!icdFile)
        return;
    oputc('\t',icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " = ");
    oprintf(icdFile, " ARR<< ");
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}
static void iop_struct(QUAD *q)
{
    if (chosenAssembler->gen->asm_add)
        chosenAssembler->gen->asm_add(q);
    if (!icdFile)
        return;
    oputc('\t',icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " = ");
    oprintf(icdFile, " STRUCT ");
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}
/*-------------------------------------------------------------------------*/

static void iop_assnblock(QUAD *q)
{
    if (chosenAssembler->gen->asm_assnblock)
        chosenAssembler->gen->asm_assnblock(q);
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->dc.left);
    oprintf(icdFile, " BLOCK= ");
    putamode(q, q->dc.right);
    oprintf(icdFile, "(");
    putamode(q, q->ans);
    oprintf(icdFile, ")");
}

/*-------------------------------------------------------------------------*/

static void iop_clrblock(QUAD *q)
{
    if (chosenAssembler->gen->asm_clrblock)
        chosenAssembler->gen->asm_clrblock(q);
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->dc.left);
    oprintf(icdFile, " BLKCLR ");
    oprintf(icdFile, "(");
    putamode(q, q->dc.right);
    oprintf(icdFile, ")");
}
/*-------------------------------------------------------------------------*/

static void iop_asmcond(QUAD *q)
{
    oprintf(icdFile, "\tASMCOND\tL_%d:PC", q->dc.v.label);
}
/*-------------------------------------------------------------------------*/

static void putjmp(QUAD *q, char *str)
{
    if (!icdFile)
        return;
    oprintf(icdFile, "\tCONDGO\tL_%d:PC ; ", q->dc.v.label);
    if (q->dc.left)
        putamode(q, q->dc.left);
    oprintf(icdFile, " %s ", str);
    if (q->dc.right)
        putamode(q, q->dc.right);
}


/*-------------------------------------------------------------------------*/

static void putset(QUAD *q, char *str)
{
    if (!icdFile)
        return;
    oputc('\t',icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " = ");
    putamode(q, q->dc.left);
    oprintf(icdFile, " %s ", str);
    putamode(q, q->dc.right);
}

/*-------------------------------------------------------------------------*/

static void iop_jc(QUAD *q)
{
    if (chosenAssembler->gen->asm_jc)
        chosenAssembler->gen->asm_jc(q);
    putjmp(q, "U<");
}

/*-------------------------------------------------------------------------*/

static void iop_ja(QUAD *q)
{
    if (chosenAssembler->gen->asm_ja)
        chosenAssembler->gen->asm_ja(q);
    putjmp(q, "U>");
}

/*-------------------------------------------------------------------------*/

static void iop_je(QUAD *q)
{
    if (chosenAssembler->gen->asm_je)
        chosenAssembler->gen->asm_je(q);
    putjmp(q, "==");
}

/*-------------------------------------------------------------------------*/

static void iop_jnc(QUAD *q)
{
    if (chosenAssembler->gen->asm_jnc)
        chosenAssembler->gen->asm_jnc(q);
    putjmp(q, "U>=");
}

/*-------------------------------------------------------------------------*/

static void iop_jbe(QUAD *q)
{
    if (chosenAssembler->gen->asm_jbe)
        chosenAssembler->gen->asm_jbe(q);
    putjmp(q, "U<=");
}

/*-------------------------------------------------------------------------*/

static void iop_jne(QUAD *q)
{
    if (chosenAssembler->gen->asm_jne)
        chosenAssembler->gen->asm_jne(q);
    putjmp(q, "!=");
}

/*-------------------------------------------------------------------------*/

static void iop_jl(QUAD *q)
{
    if (chosenAssembler->gen->asm_jl)
        chosenAssembler->gen->asm_jl(q);
    putjmp(q, "S<");
}

/*-------------------------------------------------------------------------*/

static void iop_jg(QUAD *q)
{
    if (chosenAssembler->gen->asm_jg)
        chosenAssembler->gen->asm_jg(q);
    putjmp(q, "S>");
}

/*-------------------------------------------------------------------------*/

static void iop_jle(QUAD *q)
{
    if (chosenAssembler->gen->asm_jle)
        chosenAssembler->gen->asm_jle(q);
    putjmp(q, "S<=");
}

/*-------------------------------------------------------------------------*/

static void iop_jge(QUAD *q)
{
    if (chosenAssembler->gen->asm_jge)
        chosenAssembler->gen->asm_jge(q);
    putjmp(q, "S>=");
}

/*-------------------------------------------------------------------------*/

static void iop_setc(QUAD *q)
{
    if (chosenAssembler->gen->asm_setc)
        chosenAssembler->gen->asm_setc(q);
    putset(q, "U<");
}

/*-------------------------------------------------------------------------*/

static void iop_seta(QUAD *q)
{
    if (chosenAssembler->gen->asm_seta)
        chosenAssembler->gen->asm_seta(q);
    putset(q, "U>");
}

/*-------------------------------------------------------------------------*/

static void iop_sete(QUAD *q)
{
    if (chosenAssembler->gen->asm_sete)
        chosenAssembler->gen->asm_sete(q);
    putset(q, "==");
}

/*-------------------------------------------------------------------------*/

static void iop_setnc(QUAD *q)
{
    if (chosenAssembler->gen->asm_setnc)
        chosenAssembler->gen->asm_setnc(q);
    putset(q, "U>=");
}

/*-------------------------------------------------------------------------*/

static void iop_setbe(QUAD *q)
{
    if (chosenAssembler->gen->asm_setbe)
        chosenAssembler->gen->asm_setbe(q);
    putset(q, "U<=");
}

/*-------------------------------------------------------------------------*/

static void iop_setne(QUAD *q)
{
    if (chosenAssembler->gen->asm_setne)
        chosenAssembler->gen->asm_setne(q);
    putset(q, "!=");
}

/*-------------------------------------------------------------------------*/

static void iop_setl(QUAD *q)
{
    if (chosenAssembler->gen->asm_setl)
        chosenAssembler->gen->asm_setl(q);
    putset(q, "S<");
}

/*-------------------------------------------------------------------------*/

static void iop_setg(QUAD *q)
{
    if (chosenAssembler->gen->asm_setg)
        chosenAssembler->gen->asm_setg(q);
    putset(q, "S>");
}

/*-------------------------------------------------------------------------*/

static void iop_setle(QUAD *q)
{
    if (chosenAssembler->gen->asm_setle)
        chosenAssembler->gen->asm_setle(q);
    putset(q, "S<=");
}

/*-------------------------------------------------------------------------*/

static void iop_setge(QUAD *q)
{
    if (chosenAssembler->gen->asm_setge)
        chosenAssembler->gen->asm_setge(q);
    putset(q, "S>=");
}

/*-------------------------------------------------------------------------*/

static void iop_parm(QUAD *q)
{
    if (chosenAssembler->gen->asm_parm)
        chosenAssembler->gen->asm_parm(q);
    if (!icdFile)
        return;
    oprintf(icdFile, "\tPARM\t");
    putamode(q, q->dc.left);
}

/*-------------------------------------------------------------------------*/

static void iop_parmadj(QUAD *q)
{
    if (chosenAssembler->gen->asm_parmadj)
        chosenAssembler->gen->asm_parmadj(q);
    if (!icdFile)
        return;
    oprintf(icdFile, "\tPARMADJ\t");
    putamode(q, q->dc.left);
}

/*-------------------------------------------------------------------------*/

static void iop_parmblock(QUAD *q)
{
    if (chosenAssembler->gen->asm_parmblock)
        chosenAssembler->gen->asm_parmblock(q);
    if (!icdFile)
        return;
    oprintf(icdFile, "\tPARMBLOCK");
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}

/*-------------------------------------------------------------------------*/

static void iop_cppini(QUAD *q)
{
    if (chosenAssembler->gen->asm_cppini)
        chosenAssembler->gen->asm_cppini(q);
    if (!icdFile)
        return;
    oprintf(icdFile,"\tCPPINI");
}

/*-------------------------------------------------------------------------*/
static void iop_dbgblock(QUAD *q)
{
    if (chosenAssembler->gen->asm_blockstart)
        chosenAssembler->gen->asm_blockstart(q);
    oprintf(icdFile,"\tDBG BLOCK START");
}

/*-------------------------------------------------------------------------*/

static void iop_dbgblockend(QUAD *q)
{
    if (chosenAssembler->gen->asm_blockend)
        chosenAssembler->gen->asm_blockend(q);
    oprintf(icdFile,"\tDBG BLOCK END");
}
static void iop_block(QUAD *q)
{
    oprintf(icdFile, "\tBLOCK %d", q->dc.v.label + 1);
}

/*-------------------------------------------------------------------------*/

static void iop_blockend(QUAD *q)
{
    oprintf(icdFile, "\tBLOCK END");
    if (0 && q->dc.v.data)
    {
        int i,j;
        BITINT *p;
        oprintf(icdFile, "\n;\tLive: ");
        p = q->dc.v.data;
        for (i=0; i < (tempCount+BITINTBITS-1)/BITINTBITS; i++)
            if (*p)
                for (j=0; j < BITINTBITS; j++)
                    if ((*p) && (1 << j))
                        oprintf(icdFile,"TEMP%d, ", i * BITINTBITS + j);
    }
}

static void iop_varstart(QUAD *q)
{
    if (chosenAssembler->gen->asm_varstart)
        chosenAssembler->gen->asm_varstart(q);
    oprintf(icdFile, "\tVAR START\t%s",q->dc.left->offset->v.sp->name);
}
static void iop_func(QUAD *q)
{
    if (chosenAssembler->gen->asm_func)
        chosenAssembler->gen->asm_func(q);
}
/*-------------------------------------------------------------------------*/

static void iop_livein(QUAD *q)
{
    (void)q;
    diag("op_livein: propogated live-in node");
}

/*-------------------------------------------------------------------------*/

static void iop_icon(QUAD *q)
{
    if (chosenAssembler->gen->asm_assn)
        chosenAssembler->gen->asm_assn(q);
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " C= #%llX", q->dc.v.i);
}

/*-------------------------------------------------------------------------*/

static void iop_fcon(QUAD *q)
{
    char buf[256];
    if (chosenAssembler->gen->asm_assn)
        chosenAssembler->gen->asm_assn(q);
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " C= #%s", FPFToString(buf,&q->dc.v.f));
}
static void iop_imcon(QUAD *q)
{
    char buf[256];
    if (chosenAssembler->gen->asm_assn)
        chosenAssembler->gen->asm_assn(q);
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " C= #%s", FPFToString(buf,&q->dc.v.f));
}
static void iop_cxcon(QUAD *q)
{
    char buf[256], buf1[256];
    if (chosenAssembler->gen->asm_assn)
        chosenAssembler->gen->asm_assn(q);
    if (!icdFile)
        return;
    oputc('\t', icdFile);
    putamode(q, q->ans);
    oprintf(icdFile, " C= #%s + %s * I", FPFToString(buf,&q->dc.v.c.r), FPFToString(buf1,&q->dc.v.c.i));
}
static void iop_prologue(QUAD *q)
{
    if (chosenAssembler->gen->asm_prologue)
        chosenAssembler->gen->asm_prologue(q);
    oprintf(icdFile,"\tPROLOGUE");
    oputc('\t',icdFile);
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}
static void iop_epilogue(QUAD *q)
{
    if (chosenAssembler->gen->asm_epilogue)
        chosenAssembler->gen->asm_epilogue(q);
    oprintf(icdFile,"\tEPILOGUE");
    oputc('\t',icdFile);
    putamode(q, q->dc.left);
}
static void iop_pushcontext(QUAD *q)
{
    if (chosenAssembler->gen->asm_pushcontext)
        chosenAssembler->gen->asm_pushcontext(q);
    oprintf(icdFile,"\tPUSHCONTEXT",q->dc.v.label);
}
static void iop_popcontext(QUAD *q)
{
    if (chosenAssembler->gen->asm_popcontext)
        chosenAssembler->gen->asm_popcontext(q);
    oprintf(icdFile,"\tPOPCONTEXT",q->dc.v.label);
}
static void iop_loadcontext(QUAD *q)
{
    if (chosenAssembler->gen->asm_loadcontext)
        chosenAssembler->gen->asm_loadcontext(q);
    oprintf(icdFile,"\tLOADCONTEXT",q->dc.v.label);
}
static void iop_unloadcontext(QUAD *q)
{
    if (chosenAssembler->gen->asm_unloadcontext)
        chosenAssembler->gen->asm_unloadcontext(q);
    oprintf(icdFile,"\tUNLOADCONTEXT",q->dc.v.label);
}
static void iop_tryblock(QUAD *q)
{
    (void)q;
#ifdef XXXXX
    /* hardcoding offset in stack block */
    q->dc.v.label = -xceptoffs + 3 * getSize(bt_pointer);
    if (chosenAssembler->gen->asm_tryblock)
        chosenAssembler->gen->asm_tryblock(q);
    oprintf(icdFile,"\tTRYBLOCK",q->dc.v.label);
    switch(q->dc.left->offset->v.i)
    {
        case 0:
            oprintf(icdFile,"\tTry");
            break;
        case 1:
            oprintf(icdFile,"\tCatch");
            break;
        case 2:
            oprintf(icdFile,"\tEnd");
            break;
        default:
            oprintf(icdFile,"\t???");
            break;
    }
#endif
}
static void iop_substack(QUAD *q)
{
    if (chosenAssembler->gen->asm_stackalloc)
        chosenAssembler->gen->asm_stackalloc(q);
    putasunary(q,"STACKALLOC");
}
static void iop_loadstack(QUAD *q)
{
    if (chosenAssembler->gen->asm_loadstack)
        chosenAssembler->gen->asm_loadstack(q);
    oprintf(icdFile,"\tLOADSTACK");
    oputc('\t',icdFile);
    putamode(q, q->dc.left);
}
static void iop_savestack(QUAD *q)
{
    if (chosenAssembler->gen->asm_savestack)
        chosenAssembler->gen->asm_savestack(q);
    oprintf(icdFile,"\tSAVESTACK");
    oputc('\t',icdFile);
    putamode(q, q->dc.left);
}
static void iop_functailstart(QUAD *q)
{
    if (theCurrentFunc->tp->btp->type != bt_void && chosenAssembler->gen->asm_functail)
    {
        int r = getSize(theCurrentFunc->tp->btp->type);
        if (r < 0)
            r = - r;
        chosenAssembler->gen->asm_functail(q, TRUE, r);
    }
    oprintf(icdFile,"\tTAILBEGIN");
}
static void iop_functailend(QUAD *q)
{
    if (theCurrentFunc->tp->btp->type != bt_void && chosenAssembler->gen->asm_functail)
    {
        int r = getSize(theCurrentFunc->tp->btp->type);
        if (r < 0)
            r = - r;
        chosenAssembler->gen->asm_functail(q, FALSE, r);
    }
    oprintf(icdFile,"\tTAILEND");
}
static void iop_gcsestub(QUAD *q)
{
    oprintf(icdFile,"\tGCSE");
}
static void iop_expressiontag(QUAD *q)
{
    if (chosenAssembler->gen->asm_exprtag)
        chosenAssembler->gen->asm_exprtag(q);
    oprintf(icdFile, "\tEXPR TAG\t%d",q->dc.v.label);
}
static void iop_tag(QUAD *q)
{
    if (chosenAssembler->gen->asm_tag)
        chosenAssembler->gen->asm_tag(q);
    oprintf(icdFile, "TAG");
}
static void iop_atomic_fence(QUAD *q)
{
    if (chosenAssembler->gen->asm_atomic)
        chosenAssembler->gen->asm_atomic(q);
    oprintf(icdFile,"\tATOMIC FENCE");
    oputc(' ',icdFile);
    putamode(q, q->dc.left);
}
static void iop_atomic_flag_fence(QUAD *q)
{
    if (chosenAssembler->gen->asm_atomic)
        chosenAssembler->gen->asm_atomic(q);
    oputc('\t',icdFile);
    if (q->ans)
    {
        putamode(q, q->ans);
        oprintf(icdFile, " = ");
    }
    oprintf(icdFile,"ATOMIC FLAG FENCE");
    oputc(' ',icdFile);
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}
static void iop_atomic_flag_test_and_set(QUAD *q)
{
    if (chosenAssembler->gen->asm_atomic)
        chosenAssembler->gen->asm_atomic(q);
    oputc('\t',icdFile);
    putamode(q, q->ans);
    oprintf(icdFile,"\t=ATOMIC TEST AND SET");
    oputc(' ',icdFile);
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}
static void iop_atomic_flag_clear(QUAD *q)
{
    if (chosenAssembler->gen->asm_atomic)
        chosenAssembler->gen->asm_atomic(q);
    oprintf(icdFile,"\tATOMIC CLEAR");
    oputc(' ',icdFile);
    putamode(q, q->dc.left);
    oputc(',',icdFile);
    putamode(q, q->dc.right);
}
static void iop_cmpswp(QUAD *q)
{
    if (chosenAssembler->gen->asm_atomic)
        chosenAssembler->gen->asm_atomic(q);
    oprintf(icdFile,"\tCMPSWP\t");
    putamode(q, q->ans);
    oputc(',', icdFile);
    putamode(q, q->dc.left);
    oputc(',', icdFile);
    putamode(q, q->dc.right);
}
/* List of opcodes
 * This list MUST be in the same order as the op_ enums 
 */
static void(*oplst[])(QUAD *q) = 
{
     /* NOPROTO */
        iop_nop, iop_phi, iop_line, iop_passthrough, iop_datapassthrough, iop_skipcompare,
        iop_label, iop_asmgoto, iop_goto, iop_directbranch,
        iop_gosub, iop_fargosub, iop_trap, iop_int, iop_ret,
        iop_fret, iop_rett, iop_add, iop_sub, iop_udiv, iop_umod, iop_sdiv, iop_smod, iop_muluh, iop_mulsh, iop_mul,
        iop_lsl, iop_lsr, iop_asr, iop_neg, iop_not, iop_and, iop_or, iop_eor, 
        iop_setne, iop_sete, iop_setc, iop_seta, iop_setnc, iop_setbe, iop_setl, iop_setg, iop_setle, iop_setge,
        iop_asmcond, iop_jne, iop_je, iop_jc, iop_ja, iop_jnc, iop_jbe, iop_jl, iop_jg, iop_jle, iop_jge,  
        iop_assn, iop_genword, iop_coswitch, iop_swbranch, iop_assnblock, iop_clrblock, iop_parmadj, iop_parmblock, iop_parm,
        iop_array, iop_arrayindex, iop_arraylsh, iop_struct, iop_cppini, iop_block, iop_blockend, 
        iop_dbgblock, iop_dbgblockend, iop_varstart, iop_func, iop_livein, iop_icon, iop_fcon, iop_imcon, iop_cxcon, 
        iop_atomic_flag_test_and_set, iop_atomic_flag_clear, iop_atomic_fence, iop_atomic_flag_fence, iop_cmpswp,
        iop_prologue, iop_epilogue, iop_pushcontext, iop_popcontext, iop_loadcontext, iop_unloadcontext,
        iop_tryblock, iop_substack, iop_substack, iop_loadstack, iop_savestack, iop_functailstart, iop_functailend, 
        iop_gcsestub, iop_expressiontag, iop_tag,
};
/*-------------------------------------------------------------------------*/
void beDecorateSymName(char *buf, SYMBOL *sp)
{
    char *q;
    q = lookupAlias(sp->name);
    if (q)
        strcpy(buf, q);
    else if (sp->storage_class == sc_localstatic)
        sprintf(buf, "L_%d", sp->label);
    else
    {
        strcpy(buf, sp->decoratedName);
    }
}

/*-------------------------------------------------------------------------*/

void putconst(EXPRESSION *offset, int color)
/*
 *      put a constant to the icdFile file.
 */
{
    char buf[100], buf1[100];
    switch (offset->type)
    {
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            oprintf(icdFile, "%s + %s * I", FPFToString(buf, &offset->v.c.r),
                    FPFToString(buf1, &offset->v.c.i));
            break;
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u32:
        case en_c_u16:
            oprintf(icdFile, "%lX", offset->v.i);
            break;
        case en_c_ll:
        case en_c_ull:
        case en_nullptr:        
            oprintf(icdFile, "%llX", offset->v.i);
            break;
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            oprintf(icdFile, "%s", FPFToString(buf,&offset->v.f));
            break;
        case en_tempref:
            if (offset->v.sp)
            {
                SYMBOL *sp = (SYMBOL *)offset->v.sp;
                if (offset->right)
                    oprintf(icdFile, "T%d[%s]", (int)(sp->value.i), ((SYMBOL *)offset->right)->name);
                else
                    oprintf(icdFile, "T%d", (int)sp->value.i);
                   if (sp->regmode)
                       oprintf(icdFile,"(%s)",lookupRegName(color));
                else if (offset->right && sp->offset)
                    oprintf(icdFile, "(%d)", sp->offset) ; /* - chosenAssembler->arch->retblocksize)/chosenAssembler->arch->parmwidth);*/
            }
            else
            {
                oprintf(icdFile, "T%d", ((SYMBOL*)offset->v.sp)->value.i);
                   if (offset->v.sp->regmode)
                       oprintf(icdFile,"(%s)",lookupRegName(color));
            }
            break;
        case en_auto:
            oprintf(icdFile, "%s:LINK", ((SYMBOL*)offset->v.sp)->decoratedName);
/*            if (offset->v.sp->value.i >= chosenAssembler->arch->retblocksize)*/
            if (!offset->v.sp->regmode)
                oprintf(icdFile, "(%d)", offset->v.sp->offset) ; /* - chosenAssembler->arch->retblocksize)/chosenAssembler->arch->parmwidth);*/
            else
                oprintf(icdFile,"(%s)",lookupRegName(color));
            break;
        case en_structelem:
            oprintf(icdFile, "%s:STRUCTELEM(%d)", ((SYMBOL*)offset->v.sp)->decoratedName, offset->v.sp->offset);
            break;
        case en_label:
            oprintf(icdFile, "L_%ld:RAM", offset->v.sp->label);
            break;
        case en_labcon:
            oprintf(icdFile, "L_%ld:PC", offset->v.i);
            break;
        case en_pc:
            oprintf(icdFile, "%s:PC", ((SYMBOL*)offset->v.sp)->decoratedName);
            break;
        case en_threadlocal:
            oprintf(icdFile, "%s:TLS", ((SYMBOL*)offset->v.sp)->decoratedName);
            break;
        case en_global:
            oprintf(icdFile, "%s:RAM", ((SYMBOL*)offset->v.sp)->decoratedName);
            break;
        case en_absolute:
            oprintf(icdFile, "$%lX:ABS", ((SYMBOL*)offset->v.sp)->value.i);
            break;
        case en_add:
        case en_structadd:
        case en_arrayadd:
            putconst(offset->left, color);
            oprintf(icdFile, "+");
            putconst(offset->right, color);
            break;
        case en_sub:
            putconst(offset->left, color);
            oprintf(icdFile, "-");
            putconst(offset->right, color);
            break;
        case en_uminus:
            oputc('-', icdFile);
            putconst(offset->left, color);
            break;
        case en_not_lvalue:
        case en_lvalue:
            putconst(offset->left, color);
            break;
        default:
            diag("putconst: illegal constant node.");
            break;
    }
}

/*-------------------------------------------------------------------------*/

void putlen(int l)
/*
 *      append the length field to a value
 */
{
    switch (l)
    {
        case ISZ_REG:
            break;
        case ISZ_NONE:
            oprintf(icdFile, ".N");
            break;
        case ISZ_BOOLEAN:
            oprintf(icdFile, ".BOOL");
            break;
        case ISZ_ADDR:
            oprintf(icdFile, ".A");
            break;
        case ISZ_FARPTR:
            oprintf(icdFile, ".FA");
            break;
        case ISZ_SEG:
            oprintf(icdFile, ".SA");
            break;
        case ISZ_UCHAR:
            oprintf(icdFile, ".UC");
            break;
        case  - ISZ_UCHAR: 
            oprintf(icdFile, ".C");
            break;
        case ISZ_U16:
            oprintf(icdFile, ".U16");
            break;
        case ISZ_U32:
            oprintf(icdFile, ".U32");
            break;
        case ISZ_USHORT:
            oprintf(icdFile, ".US");
            break;
            case  - ISZ_USHORT: oprintf(icdFile, ".S");
            break;
        case ISZ_UINT:
            oprintf(icdFile, ".UI");
            break;
        case -ISZ_UINT:
            oprintf(icdFile, ".I");
            break;
        case ISZ_ULONG:
            oprintf(icdFile, ".UL");
            break;
            case  - ISZ_ULONG: oprintf(icdFile, ".L");
            break;
        case ISZ_ULONGLONG:
            oprintf(icdFile, ".ULL");
            break;
        case  - ISZ_ULONGLONG: oprintf(icdFile, ".LL");
            break;
        case ISZ_FLOAT:
            oprintf(icdFile, ".F");
            break;
        case ISZ_DOUBLE:
            oprintf(icdFile, ".D");
            break;
        case ISZ_LDOUBLE:
            oprintf(icdFile, ".LD");
            break;
        case ISZ_IFLOAT:
            oprintf(icdFile, ".IF");
            break;
        case ISZ_IDOUBLE:
            oprintf(icdFile, ".ID");
            break;
        case ISZ_ILDOUBLE:
            oprintf(icdFile, ".ILD");
            break;
        case ISZ_CFLOAT:
            oprintf(icdFile, ".CF");
            break;
        case ISZ_CDOUBLE:
            oprintf(icdFile, ".CD");
            break;
        case ISZ_CLDOUBLE:
            oprintf(icdFile, ".CLD");
            break;
        case ISZ_BIT:
            oprintf(icdFile, ".BIT");
            break;		
        default:
            diag("putlen: illegal length field.");
            break;
    }
}

/*-------------------------------------------------------------------------*/

void putamode(QUAD *q, IMODE *ap)
/*
 *      output a general addressing mode.
 */
{
    /* We want to see if the compiler does anything wrong with
     * volatiles
     */
    int color = 0;
    if (q)
    {
        if (q->ans == ap)
            color = q->ansColor;
        else if (q->dc.left == ap)
            color = q->leftColor;
        else
            color = q->rightColor;
    }
    if (ap->offset && ap->offset->isvolatile)
        oputc('%', icdFile);
    switch (ap->mode)
    {
        case i_immed:
            oputc('#', icdFile);
            putconst(ap->offset, color);
            break;
        case i_ind:
            oputc('*', icdFile);
            oputc('(', icdFile);
            if (ap->offset)
                putconst(ap->offset, color);
            else
                oputc('#', icdFile);
            if (ap->offset2)
            {
                oprintf(icdFile, " + ");
                putconst(ap->offset2, q->scaleColor);
                if (ap->scale)
                    oprintf(icdFile, " * %d", 1 << ap->scale);
            }
            if (ap->offset3)
            {
                oprintf(icdFile, " + ");
                putconst(ap->offset3, color);
            }
            oputc(')', icdFile);
            break;
        case i_direct:
            if (ap->retval)
            {
                oprintf(icdFile, "RV.");
            }
            putconst(ap->offset, color);
            break;
        default:
            diag("putamode: illegal address mode.");
            break;
    }
    if (ap->bits)
        oprintf(icdFile, "{%d:%d}", ap->startbit, ap->bits);
    if (ap->mode == i_ind)
        putlen(ap->ptrsize);
    putlen(ap->size);
//    if (q)
//        oprintf(icdFile,"[0x%x]", q->liveRegs);
}

/*-------------------------------------------------------------------------*/
extern BLOCK **blockArray;
extern int blockCount;
void put_code(QUAD *q)
/*
 *      output a generic instruction.
 */
{
    int i;
    if (q->block && q->block->head == q)
    {
        oprintf(icdFile, "block %d\n", q->block->blocknum);
    }
    (*oplst[q->dc.opcode])(q);
    if (q->genConflict)
        oputc('^', icdFile);
    oputc('\n', icdFile);
        /*
    if (!icdFile)
        return;
    oprintf(icdFile,"\t$%d$", q->temps);
    oprintf(icdFile,"\t#%d%d#", q->invarInserted, q->OCPInserted);
    if (q->ans)
        oprintf(icdFile,"\t$%d$ #%d#", q->ans->vol,q->OCP != 0);
    oputc('\n', icdFile);
    if (q->OCP && icdFile)
    {
        int i;
        oprintf(icdFile, "Uses :");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->uses[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "Trans:");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->transparent[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "DSafe:");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->dsafe[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "Early:");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->earliest[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "Delay:");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->delay[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "Lates:");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->latest[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "Isola:");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->isolated[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "OCP  :");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->OCP[i]);
        oputc('\n', icdFile);
        oprintf(icdFile, "RO   :");
        for (i=0; i < (termCount+BITINTBITS-1)/BITINTBITS; i++)
            oprintf(icdFile,"%02x ", q->RO[i]);
        oputc('\n', icdFile);
    }
    */
}

/*
 * Very simple live variable analysis.  Only to be used with
 * temp regs at the moment...
 */
int beVariableLive(IMODE *m)
{
    QUAD *q = currentQuad ;
    while (q && q->dc.opcode != i_blockend)
    {
        if (q->ans == m || q->dc.left == m || q->dc.right == m)
            return TRUE ;
        q = q->fwd ;
    }	
    return FALSE;
}

/*
 * Low level routinne to rewrite code for processor and dump it.
 * Here we only need dump
 */
void rewrite_icode(void)
{
    QUAD *q = intermed_head;
    while (q)
    {
        currentQuad = q ;
        put_code(q);
        q = q->fwd;
    }
#if 0
    q = criticalThunks;
    while (q)
    {
        currentQuad = q ;
        put_code(q);
        q = q->fwd;
    }
#endif
    intermed_head = 0;
}

void gen_vtt(VTABENTRY *entry, SYMBOL *func, SYMBOL *name)
{
    if (chosenAssembler->gen->gen_vtt)
        chosenAssembler->gen->gen_vtt(entry, func);
    if (!icdFile)
        return;
    oprintf(icdFile, "\t[this] = [this] - %d\n", entry->dataOffset);
    oprintf(icdFile, "\tGOTO\t%s:PC", func->decoratedName);
}
void gen_importThunk(SYMBOL *func)
{
    if (chosenAssembler->gen->gen_importThunk)
        chosenAssembler->gen->gen_importThunk(func);
    if (!icdFile)
        return;
    oprintf(icdFile, "\tGOTO [%s]\n", func->name);
}
void gen_vc1(SYMBOL *func)
{
    gen_strlab(func);
    if (chosenAssembler->gen->gen_vc1)
        chosenAssembler->gen->gen_vc1(func);
    if (!icdFile)
        return;
    oprintf(icdFile, "\tGOTO [[this] + %d]\n", func->offset);
}
/*-------------------------------------------------------------------------*/

void gen_strlab(SYMBOL *sp)
/*
 *      generate a named label.
 */
{
    if (chosenAssembler->gen->gen_strlab)
        chosenAssembler->gen->gen_strlab(sp);
    if (!icdFile)
        return;

    newlabel = TRUE;
    gentype = nogen;
    oprintf(icdFile, "%s:\n", sp->decoratedName);
}

/*-------------------------------------------------------------------------*/

void put_label(int lab)
/*
 *      icdFile a compiler generated label.
 */
{
    if (chosenAssembler->gen->gen_label)
        chosenAssembler->gen->gen_label(lab);
    if (!icdFile)
        return;
    nl();
    sprintf(dataname, "L_%d", lab);
    oprintf(icdFile, "%s:\n", dataname);
}
void put_string_label(int lab, int type)
/*
 *      icdFile a compiler generated label.
 */
{
    if (chosenAssembler->gen->gen_string_label)
        chosenAssembler->gen->gen_string_label(lab, type);
    if (!icdFile)
        return;
    nl();
    sprintf(dataname, "L_%d", lab);
    oprintf(icdFile, "%s:\n", dataname);
}

/*-------------------------------------------------------------------------*/

void put_staticlabel(long label)
{
    put_label(label);
}

/*-------------------------------------------------------------------------*/

void genfloat(FPF *val)
/*
 * Output a float value
 */
{
    char buf[100];
    CastToFloat(ISZ_FLOAT, val);
    if (chosenAssembler->gen->gen_float)
        chosenAssembler->gen->gen_float(floatgen,val);
    if (!icdFile)
        return;
    if (gentype == floatgen && outcol < 60)
    {
        oprintf(icdFile, ",%s", FPFToString(buf,val));
        outcol += 8;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.S\t%s", FPFToString(buf,val));
        gentype = floatgen;
        outcol = 19;
    }
}

/*-------------------------------------------------------------------------*/

void gendouble(FPF *val)
/*
 * Output a double value
 */
{
    char buf[100];
    CastToFloat(ISZ_DOUBLE, val);
    if (chosenAssembler->gen->gen_float)
        chosenAssembler->gen->gen_float(doublegen,val);
    if (!icdFile)
        return;
    if (gentype == doublegen && outcol < 60)
    {
        oprintf(icdFile, ",%s", FPFToString(buf,val));
        outcol += 8;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.D\t%s", FPFToString(buf,val));
        gentype = doublegen;
        outcol = 19;
    }
}

/*-------------------------------------------------------------------------*/

void genlongdouble(FPF *val)
/*
 * Output a double value
 */
{
    char buf[100];
    CastToFloat(ISZ_LDOUBLE, val);
    if (chosenAssembler->gen->gen_float)
        chosenAssembler->gen->gen_float(longdoublegen,val);
    if (!icdFile)
        return;
    if (gentype == longdoublegen && outcol < 60)
    {
        oprintf(icdFile, ",%s", FPFToString(buf,val));
        outcol += 8;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.X\t%s", FPFToString(buf,val));
        gentype = longdoublegen;
        outcol = 19;
    }
}


/*-------------------------------------------------------------------------*/

void genbyte(long val)
/*
 * Output a byte value
 */
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(chargen, val);
    if (!icdFile)
        return;
    if (gentype == chargen && outcol < 60)
    {
        oprintf(icdFile, ",$%X", val &0x00ff);
        outcol += 4;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.B\t$%X", val &0x00ff);
        gentype = chargen;
        outcol = 19;
    }
}
void genbool(int val)
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(chargen, val);
    if (!icdFile)
        return;
    if (gentype == chargen && outcol < 60)
    {
        oprintf(icdFile, ",$%X", val &0x00ff);
        outcol += 4;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.B\t$%X", val &0x00ff);
        gentype = chargen;
        outcol = 19;
    }
}
/* val not really used and will always be zero*/
void genbit(SYMBOL *sp, int val)
{
    if (chosenAssembler->gen->gen_bit)
        chosenAssembler->gen->gen_bit(sp,val);
    if (!icdFile)
        return;
    nl();
    oprintf(icdFile, "%s\tBIT\t$%X", sp->decoratedName, val &0x00ff);
    gentype = nogen;
}
/*-------------------------------------------------------------------------*/

void genshort(long val)
/*
 * Output a word value
 */
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(shortgen, val);
    if (!icdFile)
        return;
    if (gentype == shortgen && outcol < 58)
    {
        oprintf(icdFile, ",$%X", val &0x0ffff);
        outcol += 6;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.W\t$%X", val &0x0ffff);
        gentype = shortgen;
        outcol = 21;
    }
}

void genwchar_t(long val)
/*
 * Output a word value
 */
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(wchar_tgen, val);
    if (!icdFile)
        return;
    if (gentype == wchar_tgen && outcol < 58)
    {
        oprintf(icdFile, ",$%X", val);
        outcol += 6;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.$W\t$%X", val);
        gentype = wchar_tgen;
        outcol = 21;
    }
}
/*-------------------------------------------------------------------------*/

void genlong(long val)
/*
 * Output a long value
 */
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(longgen, val);
    if (!icdFile)
        return;
    if (gentype == longgen && outcol < 56)
    {
        oprintf(icdFile, ",$%lX", val);
        outcol += 10;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.L\t$%lX", val);
        gentype = longgen;
        outcol = 25;
    }
}

/*-------------------------------------------------------------------------*/

void genlonglong(LLONG_TYPE val)
/*
 * Output a long value
 */
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(longlonggen, val);
    if (!icdFile)
        return;
    if (gentype == longlonggen && outcol < 56)
    {
        #ifdef USE_LONGLONG
            oprintf(icdFile, "\tDC.L\t0%lXH,0%lXH", val, val >> 32);
        #else 
            oprintf(icdFile, "\tDC.L\t0%lXH,0%lXH", val, val < 0 ?  - 1: 0);
        #endif 
        outcol += 10;
    }
    else
    {
        nl();
        #ifdef USE_LONGLONG
            oprintf(icdFile, "\tDC.L\t0%lXH,0%lXH", val, val >> 32);
        #else 
            oprintf(icdFile, "\tDC.L\t0%lXH,0%lXH", val, val < 0 ?  - 1: 0);
        #endif 
        gentype = longlonggen;
        outcol = 25;
    }
}
void genint(int val)
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(intgen, val);
    if (!icdFile)
        return;
    if (gentype == intgen && outcol < 56)
    {
        oprintf(icdFile, ",$%lX", val);
        outcol += 10;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.I\t$%lX", val);
        gentype = intgen;
        outcol = 25;
    }
}
void genuint16(int val)
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(u16gen, val);
    if (!icdFile)
        return;
    if (gentype == intgen && outcol < 56)
    {
        oprintf(icdFile, ",$%lX", val);
        outcol += 10;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.U16\t$%lX", val);
        gentype = intgen;
        outcol = 25;
    }
}
void genuint32(int val)
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(u32gen, val);
    if (!icdFile)
        return;
    if (gentype == intgen && outcol < 56)
    {
        oprintf(icdFile, ",$%lX", val);
        outcol += 10;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.U32\t$%lX", val);
        gentype = intgen;
        outcol = 25;
    }
}
void genenum(int val)
{
    if (chosenAssembler->gen->gen_int)
        chosenAssembler->gen->gen_int(enumgen, val);
    if (!icdFile)
        return;
    if (gentype == enumgen && outcol < 56)
    {
        oprintf(icdFile, ",$%lX", val);
        outcol += 10;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.ENUM\t$%lX", val);
        gentype = enumgen;
        outcol = 25;
    }
}
/*-------------------------------------------------------------------------*/
int genstring(STRING *str)
/*
 * Generate a string literal
 */
{
    int size = 1;
    int i;
    BOOLEAN instring = FALSE;
    gentype = nogen;
    for (i=0; i < str->size; i++)
    {
        LCHAR *p = str->pointers[i]->str;
        int n = str->pointers[i]->count;
        size += n;
        if (str->strtype == l_astr && chosenAssembler->gen->gen_string)
            chosenAssembler->gen->gen_string(p, n);
        while (n--)
        {
            switch (str->strtype)
            {
                case l_wstr:
                    genwchar_t(*p++);
                    break;
                case l_ustr:
                    genuint16(*p++);
                    break;
                case l_Ustr:
                    genuint32(*p++);
                    break;
                default:
                    if (p >= 0x20 && p < 0x7f || *p == ' ')
                    {
                        if (!instring)
                        {
                            gentype = nogen;
                            nl();
                            oprintf(icdFile,"\tDC.B\t\"");
                            instring = TRUE;
                        }
                        oputc(*p++, icdFile);
                    }
                    else {
                        if (instring)
                        {
                            oprintf(icdFile, "\"\n");
                            instring = FALSE;
                        }
                        oprintf(icdFile, "\tDB\t$%02X\n", *p++);
                    }
                    break;
            }
        }
    }
    if (instring)
        oprintf(icdFile,"\"\n");
    switch (str->strtype)
    {
        case l_wstr:
            genwchar_t(0);
            size *= getSize(bt_wchar_t);
            break;
        case l_ustr:
            genuint16(0);
            size *= 2;
            break;
        case l_Ustr:
            genuint32(0);
            size *= 4;
            break;
        default:
            genbyte(0);
            break;
    }
    return size;
}
/*-------------------------------------------------------------------------*/

void genaddress(ULLONG_TYPE address)
/*
 * Output a long value
 */
{
    if (chosenAssembler->gen->gen_address)
        chosenAssembler->gen->gen_address(address);
    if (!icdFile)
        return;
    if (gentype == longgen && outcol < 56)
    {
        oprintf(icdFile, ",%p", address);
        outcol += 10;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.A\t%p", address);
        gentype = longgen;
        outcol = 25;
    }
}

/*-------------------------------------------------------------------------*/

void gensrref(SYMBOL *sp, int val)
/*
 * Output a startup/rundown reference
 */
{
    if (chosenAssembler->gen->gen_srref)
        chosenAssembler->gen->gen_srref(sp,val);
    if (!icdFile)
        return;
    if (gentype == srrefgen && outcol < 56)
    {
        oprintf(icdFile, ",%s,%d", sp->decoratedName, val);
        outcol += strlen(sp->decoratedName) + 1;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.A\t%s,%d", sp->decoratedName, val);
        gentype = srrefgen;
        outcol = 25;
    }
}

/*-------------------------------------------------------------------------*/
void genref(SYMBOL *sp, int offset)
/*
 * Output a reference to the data area (also gens fixups )
 */
{
    char sign;
    char buf[2048];
    if (chosenAssembler->gen->gen_ref)
        chosenAssembler->gen->gen_ref(sp,offset);
    if (!icdFile)
        return;
    if (offset < 0)
    {
        sign = '-';
        offset =  - offset;
    }
    else
        sign = '+';
    sprintf(buf, "%s%c%d", sp->decoratedName, sign, offset);
    {
        if (gentype == longgen && outcol < 55-strlen(sp->decoratedName))
        {
            oprintf(icdFile, ",%s", buf);
            outcol += (11+strlen(sp->decoratedName));
        }
        else
        {
            nl();
            oprintf(icdFile, "\tDC.A\t%s", buf);
            outcol = 26+strlen(sp->decoratedName);
            gentype = longgen;
        }
    }
}

/*-------------------------------------------------------------------------*/

void genpcref(SYMBOL *sp, int offset)
/*
 * Output a reference to the code area (also gens fixups )
 */
{
    char sign;
    char buf[40];
    if (chosenAssembler->gen->gen_pcref)
        chosenAssembler->gen->gen_pcref(sp,offset);
    if (!icdFile)
        return;
    if (offset < 0)
    {
        sign = '-';
        offset =  - offset;
    }
    else
        sign = '+';
    sprintf(buf, "%s%c%d", sp->decoratedName, sign, offset);
    {
        if (gentype == longgen && outcol < 55-strlen(sp->decoratedName))
        {
            oprintf(icdFile, ",%s", buf);
            outcol += (11+strlen(sp->decoratedName));
        }
        else
        {
            nl();
            oprintf(icdFile, "\tDC.A\t%s", buf);
            outcol = 26+strlen(sp->decoratedName);
            gentype = longgen;
        }
    }
}

/*-------------------------------------------------------------------------*/

void genstorage(int nbytes)
/*
 * Output bytes of storage
 */
{
    if (nbytes < 0)
        diag("genstorage: negative storage");
        
    else if (chosenAssembler->gen->gen_storage)
        chosenAssembler->gen->gen_storage(nbytes);
    if (!icdFile)
        return;
    {
        nl();
        oprintf(icdFile, "\tDS.B\t$%X\n", nbytes);
    }
}

/*-------------------------------------------------------------------------*/

void gen_labref(int n)
/*
 * Generate a reference to a label
 */
{
    if (n < 0)
        diag("gen_labref: uncompensatedlabel");
    if (chosenAssembler->gen->gen_labref)
        chosenAssembler->gen->gen_labref(n);
    if (!icdFile)
        return;
    if (gentype == longgen && outcol < 58)
    {
        oprintf(icdFile, ",L_%d", n);
        outcol += 6;
    }
    else
    {
        nl();
        oprintf(icdFile, "\tDC.A\tL_%d", n);
        outcol = 22;
        gentype = longgen;
    }
}

void gen_labdifref(int n1, int n2)
{
    if (chosenAssembler->gen->gen_labdifref)
        chosenAssembler->gen->gen_labdifref(n1,n2);
    if (!icdFile)
        return;
    {
        if (gentype == longgen && outcol < 58)
        {
            oprintf(icdFile, ",L_%d-L_%d", n1, n2);
            outcol += 6;
        }
        else
        {
            if (!newlabel)
                nl();
            else
                newlabel = FALSE;
            oprintf(icdFile, "\tDC.A\tL_%d-L_%d", n1, n2);
            outcol = 22;
            gentype = longgen;
        }
    }
}

int wchart_cmp(LCHAR *left, LCHAR *right, int len)
{
    while (len--)
    {
        if (*left != *right)
        {
            if (*left < *right)
                return -1;
            else
                return 1;
        }
        left++, right++;
    }
    return 0;
}
/*-------------------------------------------------------------------------*/

EXPRESSION *stringlit(STRING *s)
/*
 *      make s a string literal and return it's label number.
 */
{
    STRING *lp = strtab;
    EXPRESSION *rv;
    if (cparams.prm_mergestrings)
    {
        while (lp)
        {
             int i;
            if (s->size == lp->size)
            {
                /* but it won't get in here if s and lp are the same, but
                 * resulted from different concatenation sequences
                 * this whole behavior of using strings over is undefined
                 * in the standard...
                 */
                for (i =0; i < s->size && i < lp->size; i++)
                {
                    if (lp->pointers[i]->count != s->pointers[i]->count || 
                        wchart_cmp(lp->pointers[i]->str, s->pointers[i]->str, s->pointers[i]->count))
                            break;
                }
                if (i >= s->size)
                {
                    rv = intNode(en_labcon, lp->label);
                    rv->size = s->size;
                    rv->altdata = s->strtype;
                    return rv;
                }
            }
            lp = lp->next;
        } 
    }
    s->label = nextLabel++;
    s->next = strtab;
    strtab = s;
    rv = intNode(en_labcon, s->label);
    rv->size = s->size;
    rv->altdata = s->strtype;
    return rv;
}

/*-------------------------------------------------------------------------*/

void dumpLits(void)
/*
 *      dump the string literal pool.
 */
{
    {
        while (strtab != 0)
        {
            xstringseg();
            nl();
            put_string_label(strtab->label, strtab->strtype);
            genstring(strtab);
            strtab = strtab->next;
        } 
        nl();
    }
}

/*-------------------------------------------------------------------------*/

void nl(void)
/*
 * New line
 */
{
    {
        if (outcol > 0)
        {
            oputc('\n', icdFile);
            outcol = 0;
            gentype = nogen;
        }
    }
}

/*-------------------------------------------------------------------------*/


static void exitseg(void)
{
        if (curseg != noseg && chosenAssembler->gen->exitseg)
            chosenAssembler->gen->exitseg(curseg);
        curseg = noseg ;
}
static void enterseg(enum e_sg seg)
{
    exitseg();
    if (chosenAssembler->gen->enterseg)
        chosenAssembler->gen->enterseg(seg);
    curseg = seg ;
}
/*
 * Switch to cseg 
 */
void cseg(void)
{
    if (curseg != codeseg)
    {
        enterseg(codeseg);
        nl();
        oprintf(icdFile, "\tSECTION\tcode\n");
    }
}

/*
 * Switch to dseg
 */
void dseg(void)
{
    if (curseg != dataseg)
    {
        enterseg(dataseg);
        nl();
        oprintf(icdFile, "\tSECTION\tdata\n");
    }
}

void tseg(void)
{
    if (curseg != tlsseg)
    {
        enterseg(tlsseg);
        nl();
        oprintf(icdFile, "\tSECTION\tls\n");
    }
}
/*
 * Switch to bssseg
 */
void bssseg(void)
{
    if (curseg != bssxseg)
    {
        enterseg(bssxseg);
        nl();
        oprintf(icdFile, "\tSECTION\tbss\n");
    }
}
/*
 * Switch to const seg
 */
void xconstseg(void)
{
    if (curseg != constseg)
    {
        enterseg(constseg);
        nl();
        oprintf(icdFile, "\tSECTION\tconst\n");
    }
}
/*
 * Switch to string seg
 */
void xstringseg(void)
{
    if (curseg != stringseg)
    {
        enterseg(stringseg);
        nl();
        oprintf(icdFile, "\tSECTION\tstring\n");
    }
}

/*
 * Switch to startupseg
 */
void startupseg(void)
{
    if (curseg != startupxseg)
    {
        enterseg(startupxseg);
        nl();
        oprintf(icdFile, "\tSECTION\tcstartup\n");
    }
}

/*
 * Switch to rundownseg
 */
void rundownseg(void)
{
    if (curseg != rundownxseg)
    {
        enterseg(rundownxseg);
        nl();
        oprintf(icdFile, "\tSECTION\tcrundown\n");
    }
}
void tlsstartupseg(void)
{
    if (curseg != tlssuseg)
    {
        enterseg(tlssuseg);
        nl();
        oprintf(icdFile, "\tSECTION\tcpptlsstartup\n");
    }
}
void tlsrundownseg(void)
{
    if (curseg != tlsrdseg)
    {
        enterseg(tlsrdseg);
        nl();
        oprintf(icdFile, "\tSECTION\tcpptlsrundown\n");
    }
}
void gen_virtual(SYMBOL *sp, int data)
{
    {
        if (chosenAssembler->gen->gen_virtual)
            chosenAssembler->gen->gen_virtual(sp, data);
        if (!icdFile)
            return;
        virtual_mode = data;
        curseg = virtseg;

        nl();
        oprintf(icdFile, "@%s\tVIRTUAL\n", sp->decoratedName);
        oprintf(icdFile, "%s:\n", sp->decoratedName);
   }
}

/*-------------------------------------------------------------------------*/

void gen_endvirtual(SYMBOL *sp)
{
    {
        if (chosenAssembler->gen->gen_endvirtual)
            chosenAssembler->gen->gen_endvirtual(sp);
        if (!icdFile)
            return;
        nl();
        oprintf(icdFile, "@%s\tENDVIRTUAL\n", sp->decoratedName);
        if (virtual_mode)
            dseg();
        else
            cseg();
    }
}
void align(int size)
{
    if (curseg == codeseg)
        return ;
    nl();
    oprintf(icdFile,"align %d\n",size);
}
/*-------------------------------------------------------------------------*/

void asm_header(char *name, char *version)
{
    if (cparams.prm_asmfile)
    {
        if (chosenAssembler->gen->gen_header)
           chosenAssembler->gen->gen_header(name, version);
           
        oprintf(icdFile, ";Icode test - %s\n\n", outfile);
    }
}
void asm_trailer(void)
{
    if (cparams.prm_asmfile)
    {
        if (chosenAssembler->gen->gen_trailer)
            chosenAssembler->gen->gen_trailer();
        oprintf(icdFile, "\tEND\n");
    }
}
/*-------------------------------------------------------------------------*/
void localdef(SYMBOL *sp)
{
    IncGlobalFlag();
    if (chosenAssembler->gen->local_define)
        chosenAssembler->gen->local_define(sp);
    DecGlobalFlag();
}
void localstaticdef(SYMBOL *sp)
{
    IncGlobalFlag();
    if (chosenAssembler->gen->local_static_define)
        chosenAssembler->gen->local_static_define(sp);
    DecGlobalFlag();
}
void globaldef(SYMBOL *sp)
{
    if (sp->linkage2 == lk_export && sp->linkage != lk_virtual)
        put_expfunc(sp);
    IncGlobalFlag();
    if (chosenAssembler->gen->global_define)
        chosenAssembler->gen->global_define(sp);
    DecGlobalFlag();
    if (!icdFile)
        return;
    oprintf(icdFile, "\n\tPUBLIC\t%s\n", sp->decoratedName);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

int put_exfunc(SYMBOL *sp, int notyet)
{
    if (notyet)
    {
        notyet = FALSE;
        cseg();
    }
    IncGlobalFlag();
    if (chosenAssembler->gen->extern_define)
        chosenAssembler->gen->extern_define(sp, sp->tp->type == bt_func || sp->tp->type == bt_ifunc);
    if (sp->linkage2 == lk_import && chosenAssembler->gen->import_define && sp->importfile)
        chosenAssembler->gen->import_define(sp, sp->importfile);
    DecGlobalFlag();
    if (!icdFile)
        return notyet;
    if (isfunction(sp->tp))
    {
        oprintf(icdFile, "\tEXTRN\t%s:PROC\n", sp->decoratedName);
    }
    else
    {
        oprintf(icdFile, "\tEXTRN\t%s:DATA\n", sp->decoratedName);
    }
    if (sp->linkage2 == lk_import)
        oprintf(icdFile, "\timport %s %s\n", sp->decoratedName, 
                sp->importfile ? sp->importfile : "");
    return notyet;
}

/*-------------------------------------------------------------------------*/

void put_expfunc(SYMBOL *sp)
{
    IncGlobalFlag();
    if (chosenAssembler->gen->export_define)
        chosenAssembler->gen->export_define(sp);
    DecGlobalFlag();
    if (!icdFile)
        return ;
    oprintf(icdFile, "\n\texport %s\n", sp->decoratedName);
}
/*-------------------------------------------------------------------------*/

void putexterns(void)
/*
 * Output the fixup tables and the global/external list
 */
{
    SYMBOL *sp;
    int i;
    {
        int notyet = TRUE;
        LIST *externList = externals;
        nl();
        exitseg();
        while (globalCache)
        {
            SYMBOL *sp = globalCache->data;
            globaldef(sp);
            globalCache = globalCache->next;
        }
        while (externList)
        {
            SYMBOL *sp = externList->data;
            if (!sp->ispure && (sp->dontinstantiate && sp->genreffed || !sp->inlineFunc.stmt && !sp->init && 
                                (sp->parentClass || sp->genreffed && sp->storage_class == sc_external)) & !sp->noextern)
            {
                notyet = put_exfunc(sp, notyet);
                sp->genreffed = FALSE;
/*            if (sp->mainsym->exportable && !(sp->value.classdata.cppflags & PF_INLINE))
                notyet = put_expfunc(sp, notyet);
*/
            }
            externList = externList->next;
        }
        exitseg();
        while (libincludes)
        {
            if (chosenAssembler->gen->output_includelib)
                chosenAssembler->gen->output_includelib(libincludes->data);
            
            oprintf(icdFile, "\tINCLUDELIB\t%s\n", libincludes->data);
            libincludes = libincludes->next;
        }
        oputc('\n', icdFile);
    }
}
