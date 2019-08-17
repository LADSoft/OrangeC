/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "utype.h"	
#include "cmdline.h"	
#include "lists.h"
#include "expr.h"
#include "ccerr.h"
#include "c.h"
#include "diag.h"
#include "winmode.h"
#include "version.h"
#include <io.h>

#ifdef __CCDL__
    int _stklen = 100 * 1024;
    #ifdef MSDOS
        int __dtabuflen = 32 * 1024;
    #endif 
#endif 

// #define VERSION 206

extern char GLBDEFINE[], SOURCEXT[], OBJEXT[], ENVNAME[], PROGNAME[];
extern int packlevel;
extern ERRORS *errlist;
extern TABLE *tagtable;
extern int total_errors;
extern int diagcount;
extern char *errfile;
extern TABLE *gsyms;
int inputFile = 0;
FILE *listFile = 0,  *outputFile = 0,  *cppFile = 0,  *errFile = 0;
#ifdef ICODE
FILE *icdFile = 0;
#endif
int anyusedfloat;
int stoponerr = 0;
int has_output_file;
LIST *clist = 0;
LIST *deflist = 0;
int prm_linkreg = TRUE;
int prm_stackcheck = FALSE;
int prm_warning = TRUE, prm_extwarning = FALSE;
#ifdef ICODE
    int prm_asmfile = TRUE;
	int prm_icdfile = FALSE;
#else 
    #ifdef COLDFIRE
        int prm_asmfile = FALSE;
    #else 
        int prm_asmfile = FALSE;
    #endif 
#endif 

int prm_ansi = FALSE;
int prm_c99 = FALSE;
int prm_listfile = FALSE;
int prm_maxerr = 25;
int prm_diag = FALSE;
int prm_bss = TRUE;
int prm_cppfile = FALSE;
int prm_revbits = FALSE;
int prm_lines = TRUE;
int prm_cplusplus = FALSE;
int prm_cmangle = TRUE;
int prm_trigraph = FALSE;
int basear = 1, basedr = 1, basefr = 1;
int prm_debug = FALSE;
char version[256];
char copyright[256];
int prm_errfile = FALSE;
int prm_profiler = FALSE;
int prm_peepopt = TRUE;
int prm_optmult = TRUE;
int prm_intrinsic = FALSE;
int prm_asmopt = FALSE;
int prm_xcept = TRUE;
int prm_quieterrors = FALSE;
int prm_smartframes = TRUE;
int prm_oldfor = FALSE;
#ifdef i386
    int prm_compileonly = FALSE;
    #ifdef MSDOS
        int prm_targettype = DOS32A;
    #else 
        int prm_targettype = CONSOLE;
    #endif 
    int prm_crtdll;
    int prm_lscrtdll;
	int prm_msvcrt;
    extern int prm_nasm;
#endif 

char *prm_searchpath = 0;
char *sys_searchpath = 0;
char **set_searchpath = &prm_searchpath;

jmp_buf ctrlcreturn;
int file_count = 0;

char *infile, *srcfile, listfile[256], outfile[256], cppfile[256], errorfile[256];
#ifdef ICODE
char icdfile[256];
#endif
void bool_setup(char select, char *string);
void err_setup(char select, char *string);
void incl_setup(char select, char *string);
void def_setup(char select, char *string);
void codegen_setup(char select, char *string);
void optimize_setup(char select, char *string);
void warning_setup(char select, char *string);
void parsefile(char select, char *string);
void output_setup(char select, char *string);
#ifdef i386
    void winmode_setup(char select, char *string);
#endif 

/* setup for ARGS.C */
ARGLIST ArgList[] = 
{
    {
        '9', ARG_BOOL, bool_setup
    }
    , 
    {
        'c', ARG_BOOL, bool_setup
    }
    , 
    {
        'i', ARG_BOOL, bool_setup
    }
    , 
    {
        'e', ARG_BOOL, bool_setup
    }
    , 
    {
        'f', ARG_CONCATSTRING, parsefile
    }
    , 
    {
        'l', ARG_BOOL, bool_setup
    }
    , 
    {
        'o', ARG_CONCATSTRING, output_setup
    }
    , 
    {
        'v', ARG_BOOL, bool_setup
    }
    , 
    {
        'w', ARG_CONCATSTRING, warning_setup
    }
    , 
    {
        'A', ARG_BOOL, bool_setup
    }
    , 
    {
        'C', ARG_CONCATSTRING, codegen_setup
    }
    , 
    {
        'O', ARG_CONCATSTRING, optimize_setup
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
        'S', ARG_BOOL, bool_setup
    }
    , 
    {
        's', ARG_BOOL, bool_setup
    }
    , 
    {
        'R', ARG_BOOL, bool_setup
    }
    , 
    {
        'X', ARG_BOOL, bool_setup
    }
    , 
    {
        'Q', ARG_BOOL, bool_setup
    }
    , 
    #ifdef i386
        {
            'W', ARG_CONCATSTRING, winmode_setup
        }
        , 
    #endif 
    {
        'T', ARG_BOOL, bool_setup
    }
    ,
    {
        0, 0, 0
    }
};
#ifdef i386
    void winmode_setup(char select, char *string)
    {
		int canUseDLL = FALSE;
        switch (string[0])
        {
			case 'h':		/* hxdos, win32 stub */
				prm_targettype = HXDOS;
				canUseDLL = TRUE;
				break;
			case 'r': /* raw */
				prm_targettype = RAW;
				break;
            case 'a': /* dos, watcom/le, dos32a stub */
                prm_targettype = DOS32A;
                break;
            case 'd': /* win32 DLL */
                prm_targettype = DLL;
				canUseDLL = TRUE;
                break;
            case 'c': /* win32 CONSOLE */
                prm_targettype = CONSOLE;
				canUseDLL = TRUE;
                break;
            case 'g': /* win32 GUI */
                prm_targettype = GUI;
				canUseDLL = TRUE;
                break;
            case 'e': /* DOS, Tran's PMODE */
                prm_targettype = DOS;
                break;
			case 'x': /* DOS, HXDOS stubbed for DPMI */
				prm_targettype = WHXDOS;
				break;
            default:
                fatal("Invalid executable type");
        }
        if (string[1] == 'c')
        {
            if (!canUseDLL)
                fatal("Invalid use of CRTDLL");
            prm_crtdll = TRUE;
		} 
		else if (string[1] == 'm')
		{
            if (!canUseDLL)
                fatal("Invalid use of MSVCRT");
            prm_msvcrt = TRUE;
        } 
		else if (string[1] == 'l') {
            if (!canUseDLL)
                fatal("Invalid use of LSCRTDLL");
            prm_lscrtdll = TRUE;
        }
    }
#endif 


void bool_setup(char select, char *string)
/*
 * activation routine (callback) for boolean command line arguments
 */
{
    int bool = (int)string;
    if (select == '9')
        prm_c99 = bool;
    if (select == 'l')
        prm_listfile = bool;
    if (select == 'i')
        prm_cppfile = bool;
    if (select == 'e')
        prm_errfile = bool;
    if (select == 'v' && OBJEXT[0])
    {
        prm_debug = bool;
        prm_smartframes = !bool;
    }
    if (select == 'A')
        prm_ansi = bool;
    if (select == 'Q')
        prm_quieterrors = bool;
    #ifdef i386
        if (select == 'S')
        {
            prm_asmfile = bool;
            prm_nasm = bool;
            prm_compileonly = TRUE;
        }
        if (select == 's')
        {
            prm_asmfile = bool;
            prm_nasm = bool;
        }
        if (select == 'c')
        {
            prm_compileonly = bool;
            prm_asmfile = FALSE;
        }
    #else 
        if (select == 'S')
        {
            prm_asmfile = bool;
        }
    #endif 
    if (select == 'R' || select == 'X')
    {
        prm_xcept = bool;
    }
    if (select == 'T')
        prm_trigraph = bool;
}

//-------------------------------------------------------------------------

void codegen_setup(char select, char *string)
/*
 * activation for code-gen type command line arguments
 */
{
    int bool = TRUE;
    while (*string)
    {
        switch (*string)
        {
            //               case 'f':
            //                  prm_smartframes = bool ;
            //                  break ;
            case 'd':
                prm_diag = bool;
                break;
            case 'r':
                prm_revbits = bool;
                break;
            case 'b':
                prm_bss = bool;
                break;
            case 'l':
                prm_lines = bool;
                break;
            case 'm':
                prm_cmangle = bool;
                break;
                #ifndef i386
                case 'R':
                    prm_linkreg = bool;
                    break;
                #endif 
            case 'S':
                prm_stackcheck = bool;
                break;
                /*            case 'T': */
                /*                prm_trigraph = bool; */
                /*                break ; */
                /*            case 'X': This overrides something in confcodegen */
                /*                prm_xcept = bool; */
                /*                break; */
            case 'O':
                prm_oldfor = bool;
                break;
            case 'Z':
                prm_profiler = bool;
                break;
            case '-':
                bool = FALSE;
                break;
            case '+':
                bool = TRUE;
                break;
            default:
                if (!confcodegen(*string, bool))
                    fatal("Illegal codegen parameter ");
        }
        string++;
    }
}

//-------------------------------------------------------------------------

void optimize_setup(char select, char *string)
/*
 * activation for optimizer command line arguments
 */
{
    int bool = TRUE;
    while (*string)
    {
        switch (*string)
        {
            case 'a':
                prm_asmopt = bool;
                break;
            case 'i':
                prm_intrinsic = bool;
                break;
            case 'p':
                prm_peepopt = bool;
                break;
            case 'm':
                prm_optmult = bool;
                break;
            case 'R':
            case 'r':
                string++;
                while (*string &&  *string != '+' &&  *string != '-')
                {
                    switch (*string++)
                    {
                    case 'a':
                    case 'A':
                        basear = bool;
                        break;
                    case 'd':
                    case 'D':
                        basedr = bool;
                        break;
                    case 'f':
                    case 'F':
                        basefr = bool;
                        break;
                    default:
                        goto errorx;
                    }
                    if (! *string)
                        return ;
                }
                break;
#ifdef ICODE
			case 'Z':
				prm_icdfile = bool;
				break ;
#endif
            case '-':
                bool = FALSE;
                break;
            case '+':
                bool = TRUE;
                break;
            default:
                errorx: 
                #ifdef ICODE
                    if (!confoptgen(*string, bool))
                #endif 
                    fatal("Illegal optimizer parameter");
        }
        string++;
    }
}

//-------------------------------------------------------------------------

void err_setup(char select, char *string)
/*
 * activation for the max errs argument
 */
{
    if (*string == '+')
    {
        prm_extwarning = TRUE;
        string++;
    }
    else
    if (*string == '-')
    {
        prm_extwarning = FALSE;
        string++;
    }
    prm_maxerr = atoi(string);
}

//-------------------------------------------------------------------------

void incl_setup(char select, char *string)
/*
 * activation for include paths
 */
{
    if (*set_searchpath)
    {
        *set_searchpath = realloc(*set_searchpath, strlen(string) + strlen
            (*set_searchpath) + 2);
        strcat(*set_searchpath, ";");
    }
    else
    {
        *set_searchpath = malloc(strlen(string) + 1);
        *set_searchpath[0] = 0;
    }
    fflush(stdout);
    strcat(*set_searchpath, string);
}

//-------------------------------------------------------------------------

void def_setup(char select, char *string)
/*
 * activation for command line #defines
 */
{
    char *s = malloc(strlen(string) + 1);
    LIST *l = malloc(sizeof(LIST));
    strcpy(s, string);
    l->link = deflist;
    deflist = l;
    l->data = s;
}

//-------------------------------------------------------------------------

void output_setup(char select, char *string)
{
    strcpy(outfile, string);
    has_output_file = TRUE;
}

//-------------------------------------------------------------------------

void setglbdefs(void)
/*
 * function declares any global defines from the command line and also
 * declares a couple of other macros so we can tell what the compiler is
 * doing
 */
{
    LIST *l = deflist;
    char buf[256];
    int major, minor;
    while (l)
    {
        char *s = l->data;
        char *n = s;
        while (*s &&  *s != '=')
            s++;
        if (*s == '=')
            *s++ = 0;
        glbdefine(n, s);
        if (*s)
            *(s-1) = '=';
        l = l->link;
    }
    sscanf(CC_STRING_VERSION, "%d.%d", &major, &minor);
    sprintf(buf, "%d", major *100+minor);
    glbdefine("__CCDL__", buf);
	glbdefine("__CC386__", buf);
    #if !defined(__CCDL__)
        glbdefine("__NONNATIVE_BUILD",""); // the borland version of the compiler can't handle infinities
    #endif
    #ifdef i386
        glbdefine("__386__", "1");
        glbdefine("__i386__", "1");
		switch (prm_targettype)
		{
			case HXDOS:
		        glbdefine("__DOS__", "1");
	    	    glbdefine("__WIN32__", "1");
    		    glbdefine("_WIN32", "1");
				break;
			case DOS:
			case DOS32A:
			case WHXDOS:
		        glbdefine("__DOS__", "1");
				break;
			case CONSOLE:
			case GUI:
			case DLL:
	    	    glbdefine("__WIN32__", "1");
    		    glbdefine("_WIN32", "1");
				break;
			case RAW:
				glbdefine("__RAW_IMAGE__", "");
				break;
		}
    #endif 
		
        if (prm_cplusplus)
        {
            glbdefine("__cplusplus", "");
            if (prm_xcept)
                glbdefine("__RTTI__", "");
        }
#ifdef i386
    if (prm_lscrtdll)
        glbdefine("__LSCRTL_DLL", "");
    if (prm_msvcrt)
        glbdefine("__MSVCRT_DLL", "");
    if (prm_crtdll)
        glbdefine("__CRTDLL_DLL", "");
#endif
    // for the standard headers, we may fix this later
    if (prm_ansi || prm_c99)
    	glbdefine("__STDC__", "1");
    if (prm_c99)
        glbdefine("__STDC_VERSION__", "199901L");
    //   glbdefine("__STDC_IEC_599__","1");
    //   glbdefine("__STDC_IEC_599_COMPLEX__","1");
    //   glbdefine("__STDC_ISO_10646__","199712L");
    glbdefine(GLBDEFINE, "");
}

//-------------------------------------------------------------------------

void InsertAnyFile(FILE *inf, FILE *outf, char *filename, char *path, int drive)
/*
 * Insert a file name onto the list of files to process
 */

{
    char *newbuffer, buffer[260],  *p = buffer;
    LIST *r = &clist;

    file_count++;

    if (drive !=  - 1)
    {
        *p++ = (char)(drive + 'A');
        *p++ = ':';
    }
    if (path)
    {
        strcpy(p, path);
        strcat(p, "\\");
    }
    else
        *p = 0;
    /* Allocate buffer and make .C if no extension */
    strcat(buffer, filename);
    #ifdef i386
        if (prm_compileonly || !InsertExternalFile(buffer))
    #endif 
    {
        AddExt(buffer, ".C");
        newbuffer = (char*)malloc(strlen(buffer) + 1);
        if (!newbuffer)
            return ;
        strcpy(newbuffer, buffer);

        /* Insert file */
        /* this is screwy, r is actually a pointer to a pointer.  It only works
         * because the link field is the first field
         */
        while (r->link)
            r = r->link;
        r->link = malloc(sizeof(LIST));
        r = r->link;
        if (!r)
            return ;
        r->link = 0;
        r->data = newbuffer;
    }
}

//-------------------------------------------------------------------------

void dumperrs(FILE *file);
void setfile(char *buf, char *orgbuf, char *ext)
/*
 * Get rid of a file path an add an extension to the file name
 */
{
    char *p = strrchr(orgbuf, '\\');
    if (!p)
        p = orgbuf;
    else
        p++;
    strcpy(buf, p);
    StripExt(buf);
    AddExt(buf, ext);
}

//-------------------------------------------------------------------------

void outputfile(char *buf, char *orgbuf, char *ext)
{
    if (has_output_file)
        AddExt(buf, ext);
    else
        setfile(buf, orgbuf, ext);
}

//-------------------------------------------------------------------------

void scan_env(char *output, char *string)
{
    char name[256],  *p = name;
    while (*string)
    {
        if (*string == '%')
        {
            p = name;
            string++;
            while (*string &&  *string != '%')
                *p++ =  *string++;
            if (*string)
                string++;
            *p = 0;
            p = getenv(name);
            if (p)
            {
                strcpy(output, p);
                output += strlen(output);
            }

        }
        else
            *output++ =  *string++;
    }
    *output = 0;
}

//-------------------------------------------------------------------------

int parse_arbitrary(char *string)
/*
 * take a C string and and convert it to ARGC, ARGV format and then run
 * it through the argument parser
 */
{
    char *argv[256];
    char output[10240];
    int rv, i;
    int argc = 1;
    if (!string || ! *string)
        return 1;
    scan_env(output, string);
    string = output;
    while (1)
    {
        int quoted = ' ';
        while (*string == ' ')
            string++;
        if (! *string)
            break;
        if (*string == '\"')
            quoted =  *string++;
        argv[argc++] = string;
        while (*string &&  *string != quoted)
            string++;
        if (! *string)
            break;
        *string = 0;
        string++;
    }
    rv = parse_args(&argc, argv, TRUE);
    for (i = 1; i < argc; i++)
        InsertAnyFile(0, 0, argv[i], 0,  - 1);
    return rv;
}

//-------------------------------------------------------------------------

void parsefile(char select, char *string)
/*
 * parse arguments from an input file
 */
{
    FILE *temp;
    if (!(temp = fopen(string, "r")))
        fatal("Argument file not found");
    while (!feof(temp))
    {
        char buf[10000];
        buf[0] = 0;
        fgets(buf, sizeof(buf), temp);
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
        if (!parse_arbitrary(buf))
            break;
    }
    fclose(temp);
}

//-------------------------------------------------------------------------

void addinclude(void)
/*
 * Look up the INCLUDE environment variable and append it to the
 * search path
 */
{
    #ifdef COLDFIRE
        char *string = getenv("cfccincl");
    #else 
        char *string = getenv("CCINCL");
    #endif 
    if (string && string[0])
    {
        char temp[1000];
        strcpy(temp, string);
        if (*set_searchpath)
        {
            strcat(temp, ";");
            strcat(temp,  *set_searchpath);
            free(*set_searchpath);
        }
        *set_searchpath = malloc(strlen(temp) + 1);
        strcpy(*set_searchpath, temp);
    }
}

//-------------------------------------------------------------------------

int parseenv(char *name)
/*
 * Parse the environment argument string
 */
{
    char *string = getenv(name);
    return parse_arbitrary(string);
}

//-------------------------------------------------------------------------

int parseconfigfile(char *name)
{
    char buf[256],  *p;
    strcpy(buf, name);
    p = strrchr(buf, '\\');
    if (p)
    {
        FILE *temp;
        strcpy(p + 1, PROGNAME);
        strcat(p, ".CFG");
        if (!(temp = fopen(buf, "r")))
            return 0;
        set_searchpath = &sys_searchpath;
        while (!feof(temp))
        {
            buf[0] = 0;
            fgets(buf, 256, temp);
            if (buf[strlen(buf) - 1] == '\n')
                buf[strlen(buf) - 1] = 0;
            if (!parse_arbitrary(buf))
                break;
        }
        set_searchpath = &prm_searchpath;
        fclose(temp);
    }
    return 0;

}

//-------------------------------------------------------------------------

void dumperrs(FILE *file)
{
    #ifdef DIAGNOSTICS
        if (diagcount && !total_errors)
            fprintf(file, "%d Diagnostics detected\n", diagcount);
    #endif 
    if (total_errors)
        fprintf(file, "%d Total errors\n", total_errors);
    if (prm_diag)
        mem_summary();
}

//-------------------------------------------------------------------------

void summary(void)
{
    if (prm_listfile)
    {
        fprintf(listFile, "\f\n *** global scope symbol table ***\n\n");
        list_table(gsyms, 0);
        fprintf(listFile, "\n *** structures and unions ***\n\n");
        list_table(tagtable, 0);
        dumperrs(listFile);
    }
}

//-------------------------------------------------------------------------

void ctrlchandler(int aa)
{
    longjmp(ctrlcreturn, 1);
}

//-------------------------------------------------------------------------

void internalError(int a)
{
    printf("Internal Error - Aborting compile");
    exit(1);
}

//-------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    char buffer[260];
    char *p;
    strcpy(copyright, PRODUCT_COPYRIGHT);
    strcpy(version, CC_STRING_VERSION);
    banner("%s Version %s %s", PROGNAME, version, copyright);

    #ifndef ICODE
        //   signal(SIGSEGV,internalError) ;
        //   signal(SIGFPE, internalError) ;
    #endif 
    if (OBJEXT[0])
        prm_asmfile = FALSE;

    outfile[0] = 0;

    /* parse the environment and command line */
    if (!parseenv(ENVNAME))
        usage(argv[0]);
    parseconfigfile(argv[0]);
    if (!parse_args(&argc, argv, TRUE) || (!file_count && argc == 1))
        usage(argv[0]);

    /* tack the environment includes in */
    addinclude();

    /* processor-dependent initialization */
    confsetup();

    #ifdef i386
		
        if (has_output_file && !prm_compileonly)
        {
            InsertOutputFile(outfile);
            has_output_file = FALSE;
        }
    #endif 
    /* Scan the command line for file names or response files */
    {
        int i;
        for (i = 1; i < argc; i++)
            InsertAnyFile(0, 0, argv[i], 0,  - 1);
    }

    if (file_count > 1 && has_output_file)
        fatal("Cannot specify output file for multiple input files\n");

    /* Set up a ctrl-C handler so we can exit the prog */
    signal(SIGINT, ctrlchandler);
    if (setjmp(ctrlcreturn))
        exit(1);
    /* loop through and compile all the files on the file list */
    while (clist)
    {
        prm_cplusplus = FALSE;
        anyusedfloat = 0;
        packlevel = 0;
        #ifndef ICODE
            regini();
            IntrinsIni();
            inasmini();
        #endif 
        constoptinit();
        memini();
        symini();
        stmtini();
        initini();
        preprocini();
        exprini();
        declini();
        template_init();
        defclassini();
        funcini();
        peepini();
        inlineinit();
        outcodeini();
        outcode_file_init();
		obj_init();
        dbginit();
        browse_init();
        xceptinit();
        oinit();

        //      if (!prm_quieterrors)
        //         printf("file: %s\n",clist->data);
        strcpy(buffer, clist->data);
        if (prm_asmfile)
            outputfile(outfile, buffer, SOURCEXT);
        else
            outputfile(outfile, buffer, OBJEXT);
        setfile(cppfile, buffer, ".I");
        setfile(listfile, buffer, ".LST");
        setfile(errorfile, buffer, ".ERR");
#ifdef ICODE
		setfile(icdfile, buffer, ".ICD");
#endif
        AddExt(buffer, ".C");
            p = strrchr(buffer, '.');
            if (*(p - 1) != '.')
            {
                if (p[1] == 'c' || p[1] == 'C')
                if (p[2] == 'p' || p[2] == 'P')
                {
                    if (p[3] == 'p' || p[3] == 'P')
                        prm_cplusplus = TRUE;
                }
                else
                {
                    if (p[2] == 'x' || p[2] == 'x')
                    {
                        if (p[3] == 'x' || p[3] == 'x')
                            prm_cplusplus = TRUE;
                    }
                }
                else
                {
                    if (p[2] == '+' || p[2] == '+')
                    {
                        if (p[3] == '+' || p[3] == '+')
                            prm_cplusplus = TRUE;
                    }
                }
            }
        kwini();

        if (prm_cppfile)
        {
            if (!(cppFile = fopen(cppfile, "w")))
                fatal("Cannot open preprocessor file %s", cppfile);
        }
        if ((inputFile = srchpth(buffer, "", O_RDONLY | O_BINARY, FALSE)) ==  - 1)
            fatal("Cannot open input file %s", buffer);
        infile = errfile = srcfile = litlate(buffer);
        browse_startfile(infile);
        if ((outputFile = fopen(outfile,prm_asmfile ? "w" : "wb")) == 0)
        {
            close(inputFile);
            fatal("Cannot open output file %s", outfile);
        }
		setvbuf(outputFile,0,_IOFBF,32768);
        if (prm_listfile)
        if (!(listFile = fopen(listfile, "w")))
        {
            close(inputFile);
            fclose(outputFile);
            fatal("Cannot open list file %s", listfile);
        } else {
            fprintf(listFile, "LADsoft C compiler Version %s - %s\n\n", version,
                clist->data);
        }
        if (prm_errfile)
        if (!(errFile = fopen(errorfile, "w")))
        {
            fclose(listFile);
            close(inputFile);
            fclose(outputFile);
            fatal("Cannot open error file %s", errorfile);
        }
#ifdef ICODE
        if (prm_icdfile) {
			if ((icdFile = fopen(icdfile,"w")) == 0)
    	    {
				fclose(errFile);
        	    fclose(listFile);
            	close(inputFile);
            	fclose(outputFile);
            	fatal("Cannot open output file %s", outfile);
        	}
			setvbuf(icdFile,0,_IOFBF,32768);
		}
#endif
        initerr();
        initsym();
        setglbdefs();
        installCPPBuiltins();
        getch();
        getsym();
        compile();
        summary();
        release_global();
        dumperrs(stdout);
        close(inputFile);
        //      if (prm_asmfile) {
        oflush(outputFile);
        fclose(outputFile);
        //      }
        if (prm_listfile)
            fclose(listFile);
        if (prm_errfile)
            fclose(errFile);
        if (prm_cppfile)
            fclose(cppFile);
#ifdef ICODE
		if (prm_icdfile)
			fclose(icdFile);
#endif
        clist = clist->link;

        /* Remove the ASM file if there are errors */
        if (total_errors)
            remove(outfile);

        /* Remove the ERR file if no warnings / errors */
        if (!errlist && prm_errfile)
            remove(errorfile);

        /* Flag to stop if there are any errors */
        stoponerr |= total_errors;
    }
    if (stoponerr)
        return (1);
    #ifdef i386
        else if (!prm_compileonly)
            return RunExternalFiles();
    #endif 
    else
        return (0);
}
