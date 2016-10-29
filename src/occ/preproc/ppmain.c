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
#include "compiler.h"
extern LIST *clist;
extern char outfile[];
extern FILE *outputFile;
extern int total_errors;
char cppfile[256];
FILE *cppFile;
char infile[256];

static FILE *inputFile = 0;
static int stoponerr = 0;

COMPILER_PARAMS cparams = {
    25,    /* int  prm_maxerr;*/
    0,		/* prm_stackalign */
    FALSE, /* optimize */
    FALSE, /* char prm_quieterrors;*/
    TRUE,  /* char prm_warning;*/
    FALSE, /* char prm_extwarning;*/
    FALSE, /* char prm_diag;*/
    FALSE, /* char prm_ansi;*/
    TRUE,  /* char prm_cmangle;*/
    FALSE, /* char prm_c99;*/
    FALSE, /* char prm_c1x;*/
    FALSE, /* char prm_cplusplus;*/
    TRUE,  /* char prm_xcept;*/
    FALSE, /* char prm_icdfile;*/
    TRUE,  /* char prm_asmfile;*/
    FALSE, /* char prm_compileonly;*/
    FALSE, /* char prm_debug;*/
    FALSE, /* char prm_listfile;*/
    FALSE, /* char prm_cppfile;*/
    FALSE, /* char prm_errfile;*/
    FALSE, /* char prm_browse;*/
    TRUE,  /* char prm_trigraph;*/
    FALSE, /* char prm_oldfor;*/
    FALSE, /* char prm_stackcheck;*/
    FALSE, /* char prm_profiler;*/
    TRUE,  /* char prm_mergstrings;*/
    FALSE, /* char prm_revbits;*/
    TRUE,  /* char prm_lines;*/
    TRUE, /* char prm_bss;*/
    FALSE, /* char prm_intrinsic;*/
    FALSE, /* char prm_smartframes;*/
    FALSE,  /* char prm_farkeyword;*/
    FALSE, /* char prm_linkreg;*/
    FALSE, /* char prm_charisunsigned;*/
} ;

char *getUsageText(void)
{
    return "[options] files\n"
    "\n""/1     - C11 mode                     /9     - C99 mode\n"
        "+A     - disable extensions           /Dxxx  - define something\n"
        "/E[+]nn- max number of errors         /Ipath - specify include path\n"
        "/Uxxx  - undefine something\n"
        "\n"
        "Time: " __TIME__ "  Date: " __DATE__;
}
void bool_setup(char select, char *string);
void err_setup(char select, char *string);
void incl_setup(char select, char *string);
void def_setup(char select, char *string);
void codegen_setup(char select, char *string);
void optimize_setup(char select, char *string);
void warning_setup(char select, char *string);
void parsefile(char select, char *string);
void output_setup(char select, char *string);

/* setup for ARGS.C */
static CMDLIST Args[] = 
{
    {
        '9', ARG_BOOL, bool_setup
    }
    , 
    {
        'A', ARG_BOOL, bool_setup
    }
    ,
    {
        'E', ARG_CONCATSTRING, err_setup
    }
    , 
    {
        'I', ARG_CONCATSTRING, incl_setup
    }
    , 
    {
        'D', ARG_CONCATSTRING, def_setup
    }
    , 
    {
        'U', ARG_CONCATSTRING, undef_setup
    }
    , 
    {
        0, 0, 0
    }
};

CMDLIST *ArgList = &Args[0];
void bool_setup(char select, char *string)
/*
 * activation routine (callback) for boolean command line arguments
 */
{
    BOOLEAN v = (BOOLEAN)string;
    if (select == '9')
        cparams.prm_c99 = v;
    if (select == 'A')
        cparams.prm_ansi = v;
}

/*-------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    char buffer[256];
    char *p;
    int rv;

        /*   signal(SIGSEGV,internalError) ;*/
        /*   signal(SIGFPE, internalError) ;*/

    ccinit(argc, argv);
    /* loop through and preprocess all the files on the file list */
    while (clist)
    {
        cparams.prm_cplusplus = FALSE;
        strcpy(buffer, clist->data);
        outputfile(outfile, buffer, ".i");
        AddExt(buffer, ".C");
            p = strrchr(buffer, '.');
            if (*(p - 1) != '.')
            {
                if (p[1] == 'c' || p[1] == 'C')
                if (p[2] == 'p' || p[2] == 'P')
                {
                    if (p[3] == 'p' || p[3] == 'P')
                        cparams.prm_cplusplus = TRUE;
                }
                else
                {
                    if (p[2] == 'x' || p[2] == 'X')
                    {
                        if (p[3] == 'x' || p[3] == 'X')
                            cparams.prm_cplusplus = TRUE;
                    }
                }
                else
                {
                    if (p[2] == '+')
                    {
                        if (p[3] == '+')
                            cparams.prm_cplusplus = TRUE;
                    }
                }
            }
        inputFile = SrchPth2(buffer, "", "r");
        if (!inputFile)
            fatal("Cannot open input file %s", buffer);
        strcpy(infile, buffer);
        strcpy(cppfile, outfile);
        cppFile = fopen(outfile, "w");
        if (!cppFile)
        {
            fclose(inputFile);
            fatal("Cannot open output file %s", outfile);
        }
        preprocini(infile, inputFile);
        setglbdefs();
        while(!getline()) ;

        dumperrs(stdout);
        fclose(inputFile);
        fclose(cppFile);
        clist = clist->next;

        /* Flag to stop if there are any errors */
        stoponerr |= total_errors;
    }
    rv = !!stoponerr ;
    return rv;
}
