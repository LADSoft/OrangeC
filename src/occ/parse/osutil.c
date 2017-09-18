/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#ifdef MSIL
#include "..\version.h"
#else
#include "..\..\version.h"
#endif
#if defined(_MSC_VER) || defined(BORLAND) || defined(__ORANGEC__)
#include <io.h>
#endif

#ifdef __CCDL__
    int _stklen = 100 * 1024;
    #ifdef MSDOS
        int __dtabuflen = 32 * 1024;
    #endif 
#endif 

#if defined(WIN32) || defined(MICROSOFT)
char * __stdcall GetModuleFileNameA(void * handle, char *buf, int size);
#endif

extern COMPILER_PARAMS cparams ;
extern int total_errors;
#ifndef CPREPROCESSOR
extern ARCH_ASM *chosenAssembler;
extern int diagcount;
extern NAMESPACEVALUES *globalNameSpace;
extern char infile[];

#endif

FILE *outputFile;
FILE *listFile;
char outfile[256];
char *prm_searchpath = 0;
char *sys_searchpath = 0;
char version[256];
char copyright[256];
LIST *clist = 0;
int showBanner = TRUE;

static BOOLEAN has_output_file;
static LIST *deflist = 0, *undeflist = 0;
static jmp_buf ctrlcreturn;
static char **set_searchpath = &prm_searchpath;

void fatal(char *fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);
    printf("Fatal error: ");
    vprintf(fmt, argptr);
    va_end(argptr);
    exit(1);
}
void banner(char *fmt, ...)
{
    va_list argptr;


    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);

    putc('\n', stdout);
    putc('\n', stdout);
}

/* Print usage info */
void usage(char *prog_name)
{
    char *short_name;
    char *extension;

    short_name = strrchr(prog_name, '\\');
    if (short_name == NULL)
        short_name = strrchr(prog_name, '/');
    if (short_name == NULL)
        short_name = strrchr(prog_name, ':');
    if (short_name)
        short_name++;
    else
        short_name = prog_name;

    extension = strrchr(short_name, '.');
    if (extension != NULL)
        *extension = '\0';
    printf("Usage: %s %s", short_name, getUsageText());
#ifndef CPREPROCESSOR
    #ifndef USE_LONGLONG
        printf("   long long not supported");
    #endif 
#endif
    exit(1);
}
int strcasecmp(const char *left, const char *right)
{
    while (*left && *right)
    {
        if (toupper(*left) != toupper(*right))
            return TRUE;
        left++, right++;
    }
    return *left != *right;
}
/*
 * If no extension, add the one specified
 */
void AddExt(char *buffer, char *ext)
{
    char *pos = strrchr(buffer, '.');
    if (!pos || (*(pos - 1) == '.') || (*(pos+1) == '\\'))
        strcat(buffer, ext);
}

/*
 * Strip extension, if it has one
 */
void StripExt(char *buffer)
{
    char *pos = strrchr(buffer, '.');
    if (pos && (*(pos - 1) != '.'))
        *pos = 0;
}

/*
 * Return path of EXE file
 */
void EXEPath(char *buffer, char *filename)
{
    char *temp;
    strcpy(buffer, filename);
    if ((temp = strrchr(buffer, '\\')) != 0)
        *(temp + 1) = 0;
    else
        buffer[0] = 0;
}

/*-------------------------------------------------------------------------*/

int HasExt(char *buffer, char *ext)
{
    int l = strlen(buffer), l1 = strlen(ext);
    if (l1 < l)
    {
        return !strcasecmp(buffer + l - l1, ext);
    }
    return 0;
}
/*
 * Pull the next path off the path search list
 */
static char *parsepath(char *path, char *buffer)
{
    char *pos = path;

    /* Quit if hit a ';' */
    while (*pos)
    {
        if (*pos == ';')
        {
            pos++;
            break;
        }
        *buffer++ =  *pos++;
    }
    *buffer = 0;

    /* Return a null pointer if no more data */
    if (*pos)
        return (pos);

    return (0);
}
/*
 * For each library:
 * Search local directory and all directories in the search path
 *  until it is found or run out of directories
 */
FILE *SrchPth3(char *string, char *searchpath, char *mode)
{
    FILE *in;
    char *newpath = searchpath;

    /* If no path specified we search along the search path */
    if (string[0] != '\\' && string[1] != ':')
    {
        char buffer[200];
        while (newpath)
        {
            int n;;
            /* Create a file name along this path */
            newpath = parsepath(newpath, buffer);
            n = strlen(buffer);
            if (n && buffer[n - 1] != '\\' && buffer[n - 1] != '/')
                strcat(buffer, "\\");
            strcat(buffer, (char*)string);

            /* Check this path */
            in = fopen(buffer, mode);
            if (in !=  NULL)
            {
                strcpy(string, buffer);
                return (in);
            }
        }
    }
    else
    {
        in = fopen((char*)string, mode);
        if (in !=  NULL)
        {
            return (in);
        }
    }
    return (NULL);
}
/* this ditty takes care of the fact that on DOS
 * (and on dos shells under NT/XP)
 * the filenames are limited to 8.3 notation
 * which is a problem because the C++ runtime has long file names
 * while we could add RTL support for long filenamse on DOS, that doesn't help on XP/NT
 *
 * so first we search for the full filename, if that fails for the ~1 version, and if that
 * fails for the truncated 8.3 version
 */
FILE *ccOpenFile(char *string, FILE *fil, char *mode);
FILE *SrchPth2(char *name, char *path, char *attrib)
{
    FILE *rv = SrchPth3(name, path, attrib);
#ifdef PARSER_ONLY
    rv = ccOpenFile(name, rv, attrib);
#endif
    #ifdef MSDOS
        char buf[256],  *p;
        if (rv !=  - 1)
            return rv;
        p = strrchr(name, '.');
        if (!p)
            p = name + strlen(name);
        if (p - name < 9)
            return rv;
        strcpy(buf, name);
        strcpy(buf + 6, "~1");
        strcpy(buf + 8, p);
        rv = SrchPth3(buf, path, attrib);
        if (rv !=  - 1)
        {
            strcpy(name, buf);
            return rv;
        }
        strcpy(buf, name);
        strcpy(buf + 8, p);
        rv = SrchPth3(name, path, attrib);
        if (rv !=  - 1)
        {
            strcpy(name, buf);
            return rv;
        }
        return  - 1; 
    #else
        return rv;
    #endif 
}

/*-------------------------------------------------------------------------*/

FILE *SrchPth(char *name, char *path, char *attrib, BOOLEAN sys)
{
    FILE *rv = SrchPth2(name, path, attrib);
    char buf[265],  *p;
    if (rv || !sys)
        return rv;
    strcpy(buf, name);
    p = strrchr(buf, '.');
    if (p && !strcasecmp(p, ".h"))
    {
        *p = 0;
        rv = SrchPth2(buf, path, attrib);
        if (rv)
            strcpy(name, buf);
    }
    return rv;
}

extern CMDLIST *ArgList;
/*int parseParam(int bool, char *string); */

static int use_case; /* Gets set for case sensitivity */

/* 
 * Function that unlinks the argument from che argv[] chain
 */
static void remove_arg(int pos, int *count, char *list[])
{
    int i;

    /* Decrement length of list */
    (*count)--;

    /* move items down */
    for (i = pos; i <  *count; i++)
        list[i] = list[i + 1];

}

/*
 * ompare two characters, ignoring case if necessary
 */
static int cmatch(char t1, char t2)
{
    if (use_case)
        return (t1 == t2);

    return (toupper(t1) == toupper(t2));
}

/* Routine scans a string to see if any of the characters match
 *  the arguments, then dispatches to the action routine if so.
 */
/* Callbacks of the form
 *   void boolcallback( char selectchar, int value)
 *   void switchcallback( char selectchar, int value)  ;; value always true
 *   void stringcallback( char selectchar, char *string)
 */
static int scan_args(char *string, int index, char *arg)
{
    int i =  - 1;
    while (ArgList[++i].id)
    {
        switch (ArgList[i].mode)
        {
            case ARG_SWITCHSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], &string[index]);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_SWITCH:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], (char*)TRUE);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_BOOL:
                if (cmatch(string[index], ArgList[i].id))
                {
                    if (string[0] == ARG_SEPTRUE || string[0] == '/')
                        (*ArgList[i].routine)(string[index], (char*)TRUE);
                    else
                        (*ArgList[i].routine)(string[index], (char*)FALSE);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_CONCATSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], string + index + 1);
                    return (ARG_NEXTARG);
                }
                break;
            case ARG_NOCONCATSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    if (!arg)
                        return (ARG_NOARG);
                    (*ArgList[i].routine)(string[index], arg);
                    return (ARG_NEXTNOCAT);
                }
                break;
        }
    }
    return (ARG_NOMATCH);
}

/*
 * Main parse routine.  Scans for '-', then scan for arguments and
 * delete from the argv[] array if so.
 */
BOOLEAN parse_args(int *argc, char *argv[], BOOLEAN case_sensitive)
{

    int pos = 0;
    BOOLEAN retval = TRUE;
    use_case = case_sensitive;

    while (++pos <  *argc)
    {
        if ((argv[pos][0] == ARG_SEPSWITCH) || (argv[pos][0] == ARG_SEPFALSE) 
            || (argv[pos][0] == ARG_SEPTRUE))
        {
            if (argv[pos][1] == '!')
            {
                // skip the silence arg
            }
            else
            {
                int argmode;
                int index = 1;
                BOOLEAN done = FALSE;
                do
                {
                    /* Scan the present arg */
                    if (pos <  *argc - 1)
                        argmode = scan_args(argv[pos], index, argv[pos + 1]);
                    else
                        argmode = scan_args(argv[pos], index, 0);
    
                    switch (argmode)
                    {
                        case ARG_NEXTCHAR:
                            /* If it was a char, go to the next one */
                            if (!argv[pos][++index])
                                done = TRUE;
                            break;
                        case ARG_NEXTNOCAT:
                            /* Otherwise if it was a nocat, remove the extra arg */
                            remove_arg(pos, argc, argv);
                            /* Fall through to NEXTARG */
                        case ARG_NEXTARG:
                            /* Just a next arg, go do it */
                            done = TRUE;
                            break;
                        case ARG_NOMATCH:
                            /* No such arg, spit an error  */
    #ifndef CPREPROCESSOR
    #ifdef XXXXX
                            switch( parseParam(argv[pos][index] != ARG_SEPFALSE, &argv[pos][index + 1])) {
                                case 0:
    #endif
    #endif
                                    fprintf(stderr, "Invalid Arg: %s\n", argv[pos]);
                                    retval = FALSE;
                                    done = TRUE;
    #ifndef CPREPROCESSORXX
    #ifdef XXXXX
                                    break ;
                                case 1:
                                    if (!argv[pos][++index])
                                        done = TRUE;
                                    break ;
                                case 2:
                                    done = TRUE;
                                    break;
                            }
    #endif
    #endif
                            break;
                        case ARG_NOARG:
                            /* Missing the arg for a CONCAT type, spit the error */
                            fprintf(stderr, "Missing string for Arg %s\n",
                                argv[pos]);
                            done = TRUE;
                            retval = FALSE;
                            break;
                    };

                }
                while (!done);
            }
            /* We'll always get rid of the present arg
             * And back up one
             */
            remove_arg(pos--, argc, argv);
        }
    }
    return (retval);
}
/*-------------------------------------------------------------------------*/

void err_setup(char select, char *string)
/*
 * activation for the max errs argument
 */
{
    int n;
    (void) select;
    if (*string == '+')
    {
        cparams.prm_extwarning = TRUE;
        string++;
    }
    else
    if (*string == '-')
    {
        cparams.prm_warning = FALSE;
        string++;
    }
    n = atoi(string);
    if (n > 0)
        cparams.prm_maxerr = n ;
}

/*-------------------------------------------------------------------------*/

void incl_setup(char select, char *string)
/*
 * activation for include paths
 */
{
    (void) select;
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

/*-------------------------------------------------------------------------*/

void def_setup(char select, char *string)
/*
 * activation for command line #defines
 */
{
    char *s = malloc(strlen(string) + 1);
    LIST *l = malloc(sizeof(LIST));
    (void) select;
    strcpy(s, string);
    l->next = deflist;
    deflist = l;
    l->data = s;
}

void undef_setup(char select, char *string)
{
    char *s = malloc(strlen(string) + 1);
    LIST *l = malloc(sizeof(LIST));
    (void) select;
    strcpy(s, string);
    l->next = undeflist;
    undeflist = l;
    l->data = s;
}

/*-------------------------------------------------------------------------*/

void output_setup(char select, char *string)
{
    (void) select;
    strcpy(outfile, string);
    has_output_file = TRUE;
}

/*-------------------------------------------------------------------------*/

void setglbdefs(void)
/*
 * function declares any global defines from the command line and also
 * declares a couple of other macros so we can tell what the compiler is
 * doing
 */
{
#ifndef CPREPROCESSOR
    ARCH_DEFINES *a = chosenAssembler->defines;
#endif
    LIST *l = deflist;
    char buf[256] ;
    int major, minor;
    while (l)
    {
        char *s = l->data;
        char *n = s;
        while (*s &&  *s != '=')
            s++;
        if (*s == '=')
            *s++ = 0;
        glbdefine(n, s,FALSE);
        if (*s)
            s[-1] = '=';
        l = l->next;
    }
    l = undeflist;
    while (l)
    {
        char *s = l->data;
        char *n = s;
        while (*s &&  *s != '=')
            s++;
        if (*s == '=')
            *s = 0;
        glbUndefine(n);
        l = l->next;
    }
    sscanf(STRING_VERSION, "%d.%d", &major, &minor);
    my_sprintf(buf, "%d", major *100+minor);
    glbdefine("__ORANGEC__", buf, TRUE);
    glbdefine("__CHAR_BIT__", "8", TRUE);
    if (cparams.prm_cplusplus)
    {
        glbdefine("__cplusplus", "201402",TRUE);
        if (cparams.prm_xcept)
            glbdefine("__RTTI__", "1",TRUE);
    }
    glbdefine("__STDC__", "1",TRUE);
    if (cparams.prm_c99)
    {
#ifndef CPREPROCESSOR
        glbdefine("__STDC_HOSTED__", chosenAssembler->hosted,TRUE); // hosted compiler, not embedded
#endif
        glbdefine("__STDC_VERSION__", "199901L",TRUE);
    }
    /*   glbdefine("__STDC_IEC_599__","1");*/
    /*   glbdefine("__STDC_IEC_599_COMPLEX__","1");*/
    /*   glbdefine("__STDC_ISO_10646__","199712L");*/
/*    glbdefine(GLBDEFINE, "");*/
#ifndef CPREPROCESSOR
    if (a) {
        while (a->define) {
            if (a->respect) {
                glbdefine(a->define, a->value, a->permanent);
            }
            a++;
        }
    }
#endif
}

/*-------------------------------------------------------------------------*/

void InsertOneFile(char *filename, char *path, int drive)
/*
 * Insert a file name onto the list of files to process
 */

{
    char *newbuffer, buffer[260],  *p = buffer;
    BOOLEAN inserted;
    LIST **r = &clist, *s;

    if (drive !=  - 1)
    {
        *p++ = (char)(drive + 'A');
        *p++ = ':';
    }
    if (path)
    {
        strcpy(p, path);
//        strcat(p, "\\");
    }
    else
        *p = 0;
    /* Allocate buffer and make .C if no extension */
    strcat(buffer, filename);
#ifndef CPREPROCESSOR
    inserted = chosenAssembler->insert_noncompile_file 
        && chosenAssembler->insert_noncompile_file(buffer);
    if (cparams.prm_compileonly || !inserted)
#endif
    {
        AddExt(buffer, ".C");
        newbuffer = (char*)malloc(strlen(buffer) + 1);
        if (!newbuffer)
            return ;
        strcpy(newbuffer, buffer);

        while ((*r))
            r = &(*r)->next;
        (*r) = malloc(sizeof(LIST));
        s = (*r);
        if (!s)
            return ;
        s->next = 0;
        s->data = newbuffer;
    }
}
void InsertAnyFile(char *filename, char *path, int drive)
{
    char drv[256],dir[256],name[256],ext[256];
#if defined(_MSC_VER) || defined(BORLAND) || defined(__ORANGEC__)
    struct _finddata_t findbuf;
    int n;
    _splitpath(filename, drv, dir, name, ext);
    n = _findfirst(filename, &findbuf);
    if (n != -1)
    {
        do {
            InsertOneFile(findbuf.name, dir[0] ? dir : 0, drv[0] ? tolower(drv[0])-'a' : -1);
        } while (_findnext(n, &findbuf) != -1);
        _findclose(n);
    }
    else
    {
        InsertOneFile(filename, path, drive);
    }
#else
    InsertOneFile(filename, path, drive);
#endif
}
/*-------------------------------------------------------------------------*/

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
    strcat(buf, ext);
}

/*-------------------------------------------------------------------------*/

void outputfile(char *buf, char *orgbuf, char *ext)
{
   
    if (buf[strlen(buf)-1] == '\\')
    {
        char *p = strrchr(orgbuf, '\\');
        if (p)
            p++;
        else
            p = orgbuf;
        strcat(buf, p);
        StripExt(buf);
        AddExt(buf, ext);
    }
    else if (has_output_file)
    {
        AddExt(buf, ext);
    }
    else
    {
        setfile(buf, orgbuf, ext);
    }
}

/*-------------------------------------------------------------------------*/

void scan_env(char *output, char *string)
{
    char name[256], *p ;
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

/*-------------------------------------------------------------------------*/

int parse_arbitrary(char *string)
/*
 * take a C string and and convert it to ARGC, ARGV format and then run
 * it through the argument parser
 */
{
    char *argv[40];
    char output[1024];
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
        InsertAnyFile(argv[i], 0,  - 1);
    return rv;
}

/*-------------------------------------------------------------------------*/

void parsefile(char select, char *string)
/*
 * parse arguments from an input file
 */
{
    FILE *temp = fopen(string, "r");
    (void) select;
    if (!temp)
        fatal("Response file not found");
    while (!feof(temp))
    {
        char buf[256];
        buf[0] = 0;
        fgets(buf, 256, temp);
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
        if (!parse_arbitrary(buf))
            break;
    }
    fclose(temp);
}

/*-------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------*/

int parseenv(char *name)
/*
 * Parse the environment argument string
 */
{
    char *string = getenv(name);
    return parse_arbitrary(string);
}

/*-------------------------------------------------------------------------*/

int parseconfigfile(char *name)
{
    char buf[256],  *p;
#ifndef CPREPROCESSOR
    if (!chosenAssembler->cfgname)
        return 0;
#endif
    strcpy(buf, name);
    p = strrchr(buf, '\\');
    if (p)
    {
        FILE *temp;
#ifdef CPREPROCESSOR
        strcpy(p + 1, "CPP");
#else
        strcpy(p + 1, "CC");
        strcpy(p + 1, chosenAssembler->cfgname);
#endif
        strcat(p, ".CFG");
        temp = fopen(buf, "r");
        if (!temp)
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

/*-------------------------------------------------------------------------*/

void dumperrs(FILE *file)
{
#ifndef CPREPROCESSOR
    if (cparams.prm_listfile)
    {
        fprintf(listFile,"******** Global Symbols ********\n");
        list_table(globalNameSpace->syms,0);
        fprintf(listFile,"******** Global Tags ********\n");
        list_table(globalNameSpace->tags,0);
    }
        if (diagcount && !total_errors)
            fprintf(file, "%d Diagnostics\n", diagcount);
#endif
    if (total_errors)
        fprintf(file, "%d Errors\n", total_errors);
}

/*-------------------------------------------------------------------------*/

void ctrlchandler(int aa)
{
    (void) aa;
    longjmp(ctrlcreturn, 1);
}

/*-------------------------------------------------------------------------*/

void internalError(int a)
{
    (void) a;
    printf("Internal Error - Aborting compile");
    exit(1);
}

/*-------------------------------------------------------------------------*/
void ccinit(int argc, char *argv[])
{
    char buffer[260];
    char *p;
    int rv;
    int i;
    
    strcpy(copyright, COPYRIGHT);
    strcpy(version, STRING_VERSION);

    outfile[0] = 0;
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-' || argv[i][0] == '/')
            if (argv[i][1] == '!')
            {
                showBanner = FALSE;
            }
    if (showBanner)
    {
#ifdef CPREPROCESSOR
        banner("CPP Version %s %s", version, copyright);
#else
        banner("%s Version %s %s", chosenAssembler->progname, version, copyright);
#endif
    }
    /* parse the environment and command line */
#ifndef CPREPROCESSOR
    if (chosenAssembler->envname && !parseenv(chosenAssembler->envname))
        usage(argv[0]);
#endif
        
#if defined(WIN32) || defined(MICROSOFT)
    GetModuleFileNameA(NULL, buffer, sizeof(buffer));    
#else
    strcpy(buffer, argv[0]);
#endif

    parseconfigfile(buffer);
    if (!parse_args(&argc, argv, TRUE) || (!clist && argc == 1))
        usage(argv[0]);

    /* tack the environment includes in */
    addinclude();

    /* Scan the command line for file names or response files */
    {
        int i;
        for (i = 1; i < argc; i++)
            if (argv[i][0] == '@')
                parsefile(0, argv[i] + 1);
            else
                InsertAnyFile(argv[i], 0,  - 1);
    }

#ifndef PARSER_ONLY

    if (has_output_file)
    {
    #ifndef CPREPROCESSOR
            if (chosenAssembler->insert_output_file)
                chosenAssembler->insert_output_file(outfile);
    #endif
        if (!cparams.prm_compileonly)
        {
            has_output_file = FALSE;
        }
        else
        {
            if (clist && clist->next && outfile[strlen(outfile)-1] != '\\')
                fatal("Cannot specify output file for multiple input files\n");
        }
    }
    #else
    {
        LIST *t = clist;
        while (t)
        {
            t->data = litlate(fullqualify(t->data));
            t = t->next;
        }
    }
    #endif
            
    /* Set up a ctrl-C handler so we can exit the prog */
    signal(SIGINT, ctrlchandler);
    if (setjmp(ctrlcreturn))
        exit(1);
}
