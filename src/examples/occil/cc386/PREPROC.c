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
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "utype.h"
#include "cmdline.h"
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "diag.h"
#include <time.h>
#include <dir.h>
#include <stdlib.h>

extern int prm_nasm, prm_c99;
extern short inputline[];
extern int inputFile;
extern TABLE *gsyms, defsyms;
extern LLONG_TYPE ival;
extern char laststr[];
extern HASHREC **defhash;
extern char *infile;
extern int incconst;
extern char *prm_searchpath,  *sys_searchpath;
extern int prm_cplusplus, prm_ansi;
extern short *lptr;
extern int cantnewline;
extern int backupchar;
extern int floatregs, dataregs, addrregs, basefr, basedr, basear;
extern int prm_cmangle;
extern enum e_sym lastst;
extern char lastid[];
extern int lastch;
extern int lineno;
extern int global_flag;
extern char inputbuffer[];
extern int inputlen;
extern char *ibufPtr;
extern char *segnames[], *segclasses[];

typedef struct _startups_
{
    struct _startups_ *link;
    char *name;
    int prio;
} STARTUPS;

#define INCL_LEVEL_MAX 16

int noStackFrame;
char *errfile;
int sys_inc;
int errlineno = 0;
IFSTRUCT *ifshold[70];
char *inclfname[INCL_LEVEL_MAX];
int inclfile[INCL_LEVEL_MAX];
int incldepth = 0;
int inclline[64];
int inclcurrent[INCL_LEVEL_MAX];
char *incldata[INCL_LEVEL_MAX];
int inclpos[INCL_LEVEL_MAX];
int inclsysflags[INCL_LEVEL_MAX];
char *inclptr[INCL_LEVEL_MAX];
short *lptr;
int stdpragmas;

FILELIST *incfiles = 0,  *lastinc;

LIST *libincludes = 0;

IFSTRUCT *ifs = 0;
int ifskip = 0;
int elsetaken = 0;
int currentfile = 0;
AUXLIST *auxlist;
int packdata[100];
int packlevel;

void filemac(short *string);
void datemac(short *string);
void dateisomac(short *string);
void timemac(short *string);
void linemac(short *string);

static char *segaliases[] = {
        "???", "CODE" , "DATA", "BSS", "CONST", "STRING",
        "INITDATA", "EXITDATA", "CPPINIT", "CPPEXIT",
};

static char *unmangid; /* In this module we have to ignore leading underscores
    */
static STARTUPS *startuplist,  *rundownlist;
static short defkw[] =
{
    'd', 'e', 'f', 'i', 'n', 'e', 'd', 0
};
static int definelistcount;
static SYM *definelist[500]; /* Way deep but hey! */
static int skiplevel;

static short *macroReplaceBuffers[100];
static int macroReplaceAllocated = 0;
static int macroReplaceLevel = 0;
/* List of standard macros */
#define INGROWNMACROS 5

struct inmac
{
    char *s;
    void(*func)(char *);
} ingrownmacros[INGROWNMACROS] =
{
    {
        "__FILE__", filemac
    }
    ,
    {
        "__DATE__", datemac,
    }
    ,
    {
        "__DATEISO__", dateisomac,
    }
    ,
    {
        "__TIME__", timemac
    }
    ,
    {
        "__LINE__", linemac
    }
};

static void repdefines(short *lptr);

void pushif(void);

/* Moudle init */
void preprocini()
{
    stdpragmas = STD_PRAGMA_FCONTRACT ;
    skiplevel = 0;
    libincludes = 0;
    floatregs = basefr;
    dataregs = basedr;
    addrregs = basear;
    incldepth = 0;
    incfiles = 0;
    ifs = 0;
    ifskip = elsetaken = 0;
    currentfile = 0;
    unmangid = lastid;
    if (prm_cmangle)
        unmangid++;
    startuplist = rundownlist = 0;
    lastinc = 0;
        packlevel = 0;
        packdata[0] = 8; /* windows default */
}

short *allocateMacroReplaceBuffer(void)
{
        if (macroReplaceLevel < sizeof(macroReplaceBuffers)/sizeof(macroReplaceBuffers[0]))
        {
                if (macroReplaceLevel >= macroReplaceAllocated)
                {
                        macroReplaceBuffers[macroReplaceLevel] = malloc(sizeof(macroReplaceBuffers[0]) * MACRO_REPLACE_SIZE);
                        if (macroReplaceBuffers[macroReplaceLevel] == NULL)
                                fatal("out of memory");
                        macroReplaceAllocated++;
                }
                return macroReplaceBuffers[macroReplaceLevel++];
        }
        else
                fatal("out of memory");
}
void freeMacroReplaceBuffer(short *buf)
{
        if (macroReplaceLevel && buf == macroReplaceBuffers[macroReplaceLevel - 1])
                macroReplaceLevel--;
        else
                DIAG("Mismatch in macro level drop");
}
/* Preprocessor dispatch */
int preprocess(void)
{
    ++lptr;
    lastch = ' ';
    while (isspace (*lptr))
        lptr++ ;
    if (*lptr == '\n' || ! *lptr)
        return incldepth == 0;
    getsym(); /* get first word on line */

    if (lastst != id && lastst != kw_else && lastst != kw_if)
    {
        generror(ERR_IDEXPECT, 0, 0);
        return incldepth == 0;
    }
    if (strcmp(unmangid, "include") == 0)
        return doinclude();
    else if (strcmp(unmangid, "define") == 0)
        return dodefine();
    else if (strcmp(unmangid, "endif") == 0)
        return doendif();
    else if (lastst == kw_else)
        return doelse();
    else if (strcmp(unmangid, "ifdef") == 0)
        return doifdef(TRUE);
    else if (strcmp(unmangid, "ifndef") == 0)
        return doifdef(FALSE);
    else if (lastst == kw_if)
    {
        repdefines(lptr);
        defcheck(lptr);
        return doif(0);
    }
    else if (strcmp(unmangid, "elif") == 0)
    {
        repdefines(lptr);
        defcheck(lptr);
        return doelif();
    }
    else if (strcmp(unmangid, "undef") == 0)
        return (doundef());
    else if (strcmp(unmangid, "error") == 0)
        return (doerror(ERR_ERROR));
    else if (!prm_ansi && strcmp(unmangid, "warning") == 0)
        return (doerror(ERR_WARNING));
    else if (strcmp(unmangid, "pragma") == 0)
        return (dopragma());
    else if (strcmp(unmangid, "line") == 0)
        return (doline());
    else
    {
        if (!ifskip)
            gensymerror(ERR_PREPROCID, unmangid);
        return incldepth == 0;
    }
}

//-------------------------------------------------------------------------

int doerror(int n)
{
    char *temp;
    int i = 0;
    if (ifskip)
        return incldepth == 0;
    global_flag++;
    temp = xalloc(pstrlen(lptr) + 1);
    while (*lptr)
        temp[i++] = *lptr++;
    temp[i] = 0;
    global_flag--;
    basicerror(n, temp);
    return incldepth == 0;
}

//-------------------------------------------------------------------------

char *getauxname(short *ptr, char **bufp)
{
    char buf[200],  *bp = buf;
    while (isspace(*ptr))
        ptr++;
    if (*ptr != '"' &&  *ptr != '<')
    {
        generror(ERR_ILLCHAR,  *ptr, 0);
        return 0;
    }
    ptr++;
    if (prm_cmangle)
        *bp++ = '_';
    while (*ptr &&  *ptr != '>' &&  *ptr != '"')
        *bp++ =  *ptr++;
    *bp++ = 0;
    while (isspace(*ptr))
        ptr++;
    if (! *ptr ||  *ptr != '"' &&  *ptr != '>')
    {
        generror(ERR_ILLCHAR, *(ptr - 1), 0);
        return 0;
    }
    ptr++;
    while (isspace(*ptr))
        ptr++;
    *bufp = xalloc(strlen(buf) + 1);
    strcpy(*bufp, buf);
    return ptr;
}

//-------------------------------------------------------------------------

static int pragerror(int errnum)
{
    char buf[100],  *p = buf, i = 99;
    short *s = lptr;
        if (lastch == '(')
                return incldepth == 0;
    while (i-- &&  *s &&  *s != '\n')
        *p++ =  *s++;
    *p = 0;
    basicerror(errnum, buf);
    return (incldepth == 0);
}

//-------------------------------------------------------------------------

int dopragma(void)
{
    char buf[40],  *p = buf;
    STARTUPS *a;
    int val = 0, sflag;
    if (ifskip)
        return incldepth == 0;
    cantnewline = TRUE;
    if (lastst != kw__Pragma)  /* lineToCpp() in getline() too */
        lineToCpp();
    getsym();
    cantnewline = FALSE;
    if (lastst != id)
        return incldepth == 0;
    if (!strcmp(unmangid, "error"))
        return pragerror(ERR_USERERR);
    else if (!strcmp(unmangid, "warning"))
        return pragerror(ERR_USERWARN);
        else if (!strcmp(unmangid, "classname"))
        {
                int i;
                cantnewline = TRUE;
                getsym();
                if (lastst != id)
                {
                        cantnewline = FALSE;
                return incldepth == 0;
                }
                for (i=1; i < sizeof(segaliases)/sizeof(segaliases[0]); i++)
                        if (!strcmp(unmangid, segaliases[i]))
                                break;
                if (i >= sizeof(segaliases)/sizeof(segaliases[0]))
                {
                        cantnewline = FALSE;
                return incldepth == 0;
                }
                getsym();
                if (lastst != id)
                {
                        cantnewline = FALSE;
                return incldepth == 0;
                }
                segnames[i] = litlate(unmangid);
                getsym();

                if (lastst != id)
                {
                        cantnewline = FALSE;
                return incldepth == 0;
                }
                segclasses[i] = litlate(unmangid);
                cantnewline = FALSE;
        return incldepth == 0;

        }
    else if (!strcmp(unmangid, "startup"))
        sflag = 1;
    else if (!strcmp(unmangid, "rundown"))
        sflag = 0;
    else if (!strncmp(unmangid, "regopt", 6))
    {
        short *s = lptr;
        dataregs = floatregs = addrregs = 0;
        while (*s != '\n')
        {
            switch (*s)
            {
                case 'a':
                case 'A':
                    addrregs = 1;
                    break;
                case 'f':
                case 'F':
                    floatregs = 1;
                    break;
                case 'd':
                case 'D':
                    dataregs = 1;
                    break;
            }
            s++;
        }
        return incldepth == 0;
    }
    else if (!strncmp(unmangid, "STDC", 4))
    {
        int val = 0, on = 0;
        cantnewline = TRUE;
        getsym();
        cantnewline = FALSE;
        if (!strncmp(unmangid, "FENV_ACCESS", 11))
            val = STD_PRAGMA_FENV;
        else if (!strncmp(unmangid, "CX_LIMITED_RANGE", 16))
            val = STD_PRAGMA_CXLIMITED;
        else if (!strncmp(unmangid, "FP_CONTRACT", 11))
            val = STD_PRAGMA_FCONTRACT;
        else
            return incldepth == 0;
        cantnewline = TRUE;
        getsym();
        cantnewline = FALSE;
        if (!strncmp(unmangid, "ON", 2))
            on = 1;
        else if (strncmp(unmangid, "OFF", 3))
            return incldepth == 0;
        if (on)
            stdpragmas |= val;
        else
            stdpragmas &= ~val;
        return incldepth == 0;
    }
    else
    if (!strncmp(unmangid, "library", 7))
    {
        if (prm_nasm)
        {
            generror(ERR_PREPIG,  *lptr, 0);
        }
        else
        {
            short *s = lptr - 1;
            char buf[128], *p = buf;
            while (isspace(*s))
                s++;
            if (*s++ != '(')
            {
                generror(ERR_ILLCHAR,  *lptr, 0);
            }
            else
            {
                while (isspace(*s))
                    s++;
                while (isalnum(*s) ||  *s == '.' ||  *s == ':' ||  *s == '\\')
                    *p++ =  *s++;
                *p = 0;
                while (isspace(*s))
                    s++;
                if (*s++ != ')')
                {
                    generror(ERR_ILLCHAR,  *lptr, 0);
                }
                else
                {
                    LIST *l;
                    char *f;
                    global_flag++;
                    f = litlate(buf);
                    l = xalloc(sizeof(LIST));
                    l->data = f;
                    l->link = libincludes;
                    libincludes = l;
                    global_flag--;
                }
            }
        }
        return incldepth == 0;
    }
    else
    if (!strncmp(unmangid, "pack", 4))
    {
        lptr--;
        while (isspace(*lptr))
            lptr++;
        if (*lptr != '(')
        {
            generror(ERR_ILLCHAR,  *lptr, 0);
            return incldepth = 0;
        }
        lptr++;
        while (isspace(*lptr))
            lptr++;
        if (*lptr == ')')
        {
            if (packlevel)
                packlevel--;
        }
        else if (isdigit(*lptr))
        {
            if (packlevel < sizeof(packdata) - 1)
            {
                packdata[++packlevel] = 0;
                while (isdigit(*lptr))
                {
                    packdata[packlevel] *= 10;
                    packdata[packlevel] +=  *lptr++ - '0';
                }
                if (packdata[packlevel] < 1)
                    packdata[packlevel] = 1;
            }
            while (isdigit(*lptr))
                lptr++;
            while (isspace(*lptr))
                lptr++;
            if (*lptr != ')')
                generror(ERR_ILLCHAR,  *lptr, 0);
        }
        return incldepth == 0;
    }
    else
    if (!strcmp(unmangid, "aux"))
    {
        AUXLIST *newaux;
        SYM *sp;
        char *name;
        char *alias;
        ++global_flag;
        lptr = getauxname(lptr, &name);
        --global_flag;
        if (!lptr)
            return incldepth == 0;
        if (*lptr++ != '=')
        {
            generror(ERR_ILLCHAR,  *lptr, 0);
            return incldepth == 0;
        }
        ++global_flag;
        if (!getauxname(lptr, &alias))
        {
            --global_flag;
            return incldepth == 0;
        }


        if (alias[0] == '_' && prm_cmangle)
          alias = &alias[1];

        sp = gsearch(name);
        if (sp)
            sp->alias = alias;
        output_alias(name, alias);

        newaux = xalloc(sizeof(AUXLIST));
        newaux->link = auxlist;
        newaux->name = name;
        newaux->alias = alias;
        auxlist = newaux;

        --global_flag;
        return incldepth == 0;

    }
    #ifdef i386
        else if (!strcmp(unmangid, "farkeyword"))
        {
            extern int prm_farkeyword;
            while (isspace(*lptr))
                lptr++;
            prm_farkeyword =  *lptr == '1';
            return incldepth == 0;
        }
    #endif
    else if (!strcmp(unmangid,"NOSTACKFRAME"))
        {
                noStackFrame = TRUE;
                return incldepth == 0;
        }
        else
        return incldepth == 0;

    /* if we get here it was either a startup or rundown pragma */
    if (prm_cmangle)
        *p++ = '_';
    while (isalnum(*lptr) ||  *lptr == '_')
        *p++ =  *lptr++;
    *p = 0;
    while (*lptr && (*lptr == ' ' ||  *lptr == '\t' ||  *lptr == ','))
        lptr++;
    if (*lptr &&  *lptr != '\n' && !isdigit(*lptr))
    {
        generror(ERR_ILLCHAR,  *lptr, 0);
        while (*lptr)
            lptr++;
    }

    if (isdigit(*lptr))
    while (isdigit(*lptr))
    {
        val *= 10;
        val += (*lptr++) - '0';
    }
    else
        val = 64;
    ++global_flag;
    a = xalloc(sizeof(STARTUPS));
    a->name = litlate(buf);
    a->prio = val;
    if (sflag)
    {
        a->link = startuplist;
        startuplist = a;
    }
    else
    {
        a->link = rundownlist;
        rundownlist = a;
    }
    --global_flag;
    while (*lptr && (*lptr == ' ' ||  *lptr == '\t'))
        lptr++;
    if (*lptr &&  *lptr != '\n')
        generror(ERR_ILLCHAR,  *lptr, 0);
    return incldepth == 0;
}

// parses the _Pragma directive
void Compile_Pragma(void)
{
    short buf[MACRO_REPLACE_SIZE], *q = buf;
    int chhold;
    enum e_sym symold;
    short *lpold;
    getsym();
    needpunc(openpa, 0);
    if (lastst == sconst)
    {
        char *p = laststr;
        while (*p)
        {
            if (*p == '\\' && (*(p + 1) == '"' || *(p + 1) == '\\'))
                p++;
            *q++ =  *p++;
        }
        *q = 0;
    }
    else if (lastst == lsconst)
    {
        short *p = laststr;
        while (*p)
        {
            if (*p == '\\' && (*(p + 1) == '"' || *(p + 1) == '\\'))
                p++;
            *q++ =  *p++;
        }
        *q = 0;

    }
    else
    {
        getsym();
        generror(ERR_NEEDSTRING, 0, 0);
        needpunc(closepa, 0);
        return ;
    }
    chhold = lastch;
    symold = lastst;
    lastst = kw__Pragma;
    lpold = lptr;
    lptr = buf;
    getch();
    dopragma();
    lptr = lpold;
    lastst = symold;
    lastch = chhold;
    getsym();
    needpunc(closepa, 0);
}

//-------------------------------------------------------------------------

void setalias(SYM *sp)
{
    AUXLIST *s = auxlist;
    while (s)
    {
        if (!strcmp(s->name, sp->name))
        {
            sp->alias = s->alias;
            return ;
        }
        s = s->link;
    }
}

//-------------------------------------------------------------------------

void dumpstartups(void)
/*
 * Dump references to startup/rundown code
 */
{
    SYM *s;
    if (startuplist)
    {
        startupseg();
        while (startuplist)
        {
            s = search(startuplist->name, gsyms);
            if (!s || s->tp->type != bt_ifunc)
                gensymerror(ERR_UPDOWN, startuplist->name);
            else
            {
                gensrref(s, startuplist->prio);
                s->tp->uflags |= UF_USED;
            }
            startuplist = startuplist->link;
        }
    }
    if (rundownlist)
    {
        rundownseg();
        while (rundownlist)
        {
            s = search(rundownlist->name, gsyms);
            if (!s || s->tp->type != bt_ifunc)
                gensymerror(ERR_UPDOWN, rundownlist->name);
            else
            {
                gensrref(s, rundownlist->prio);
                s->tp->uflags |= UF_USED;
            }
            rundownlist = rundownlist->link;
        }
    }
}

//-------------------------------------------------------------------------

int doline(void)
/*
 * Handle #line directive
 */
{
    int n;
    if (ifskip)
        return incldepth == 0;
    defcheck(lptr);
    getsym();
    if (lastst != iconst && lastst != lconst || ival == 0)  /* C99 mode returns lconst */
        gensymerror(ERR_PREPROCID, "#line");
    else
    {
        global_flag++;
        n = ival;
        while (lastch != '\n' && isspace(lastch))
            getch();
        if (lastch != '\n' && lastch != '"')
            gensymerror(ERR_PREPROCID, "#line");
        else if (lastch == '\n')
            lineno = errlineno = n - 1;
        else
        {
            getsym();
            if (lastst != sconst)
                gensymerror(ERR_PREPROCID, "#line");
            else
            {
                infile = errfile = litlate(laststr);
                lineno = errlineno = n - 1;
            }
        }
        global_flag--;
    }
    return incldepth == 0;
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
int srchpth2(char *name, char *path, int attrib)
{
    char buf[256],  *p;
    int rv = SearchPath(name, path, attrib);
    #ifdef WIN32
        return rv;
    #else
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
        rv = SearchPath(buf, path, attrib);
        if (rv !=  - 1)
        {
            strcpy(name, buf);
            return rv;
        }
        strcpy(buf, name);
        strcpy(buf + 8, p);
        rv = SearchPath(name, path, attrib);
        if (rv !=  - 1)
        {
            strcpy(name, buf);
            return rv;
        }
        return  - 1;
    #endif
}

//-------------------------------------------------------------------------

int srchpth(char *name, char *path, int attrib, int sys)
{
    int rv = srchpth2(name, path, attrib);
    char buf[265],  *p;
    if (rv !=  - 1 || !sys)
        return rv;
    strcpy(buf, name);
    p = strrchr(buf, '.');
    if (p && !stricmp(p, ".h"))
    {
        *p = 0;
        rv = srchpth2(buf, path, attrib);
        if (rv !=  - 1)
            strcpy(name, buf);
    }
    return rv;
}

//-------------------------------------------------------------------------

int doinclude(void)
/*
 * HAndle include files
 */
{
    int rv;
    int oldsysinc = sys_inc;
    int oldfile = inputFile;
    short *olptr = lptr;
    incconst = TRUE;
    sys_inc = FALSE;
    getsym(); /* get file to include */
    if (lastst != sconst)
    {
        lptr = olptr;
        defcheck(lptr);
        getch();
        getsym();
    }
    incconst = FALSE;
    if (ifskip)
        return incldepth == 0;
    if (lastst != sconst)
    {
        gensymerror(ERR_INCLFILE, "include");
        return incldepth == 0;
    }
    if (incldepth >= INCL_LEVEL_MAX)
    {
        generror(ERR_PREPROCID, 0, 0);
        return incldepth == 0;
    }
    inputFile = -1;
    if (sys_inc)
        inputFile = srchpth(laststr, sys_searchpath, O_RDONLY | O_BINARY, TRUE);
    if (inputFile == -1 && inclfname[incldepth]) {
      char *inclpath = strdup(inclfname[incldepth]);
      if (inclpath) {
        char *p = strrchr(inclpath, '\\'), *p1 = strrchr(inclpath, '/');
        if (p < p1) p = p1;
        if (p) {
          *p = 0;
          inputFile = srchpth(laststr, inclpath, O_RDONLY | O_BINARY, FALSE);
        }
        free(inclpath);
      }
    }
    if (inputFile == -1)
        inputFile = srchpth(laststr, ".", O_RDONLY | O_BINARY, FALSE);
    if (inputFile == -1)
        inputFile = srchpth(laststr, prm_searchpath, O_RDONLY | O_BINARY, FALSE);
    if (!sys_inc && inputFile == -1)
        inputFile = srchpth(laststr, sys_searchpath, O_RDONLY | O_BINARY, TRUE);
    if (inputFile ==  - 1)
    {
        gensymerror(ERR_CANTOPEN, laststr);
        inputFile = oldfile;
        rv = incldepth == 0;
    }
    else
    {
        LIST *list;
        int i;
        ansieol();
        pushif();
        ifshold[incldepth] = ifs;
        elsetaken = 0;
        ifskip = 0;
        ifs = 0;
        if (!incldata[incldepth])
            incldata[incldepth] = malloc(INPUT_BUFFER_SIZE);
        if (!incldata[incldepth])
            fatal("out of memory");
        memcpy(incldata[incldepth], inputbuffer, INPUT_BUFFER_SIZE);
        inclpos[incldepth] = inputlen;
        inclptr[incldepth] = ibufPtr;
        inputlen = 0;
        inclline[incldepth] = lineno;
        inclfile[incldepth] = oldfile; /* push current input file */
        inclcurrent[incldepth] = currentfile;
        inclsysflags[incldepth] = oldsysinc;
        inclfname[incldepth++] = infile;
        global_flag++;
        infile = litlate(laststr);
        for (i = 1, list = incfiles; list; i++, list = list->link)
            if (!strcmp(list->data, laststr))
                break;
        if (list)
            currentfile = i;
        else
        {
            list = xalloc(sizeof(FILELIST));
            list->data = infile;
            list->link = 0;
            if (incfiles)
                lastinc = lastinc->link = list;
            else
                incfiles = lastinc = list;
            currentfile = i;
        }
        browse_startfile(infile);
        errfile = infile;
        errlineno = 0;
        global_flag--;
        rv = incldepth == 1;
        lineno = 0;
    }
    return rv;
}

//-------------------------------------------------------------------------

short *plitlate(short *string)
{
    short *temp = xalloc(pstrlen(string) *sizeof(short) + sizeof(short));
    pstrcpy(temp, string);
    return temp;
}

//-------------------------------------------------------------------------

void glbdefine(char *name, char *value)
{
    {
        SYM *sp;
        short *p;
        DEFSTRUCT *def;
        if ((sp = basesearch(name, &defsyms, 0)) != 0)
            return ;
        ++global_flag; /* always do #define as globals */
        sp = makesym(sc_static);
        sp->name = litlate(name);
        def = xalloc(sizeof(DEFSTRUCT));
        def->args = 0;
        def->argcount = 0;
        def->string = p = xalloc((strlen(value) + 1) *sizeof(short));
        while (*value)
            *p++ =  *value++;
        *p++ = 0;
        sp->value.s = (char*)def;
        insert(sp, &defsyms);
        --global_flag;
        return ;
    }
}

/* Handle #defines
 * Doesn't check for redefine with different value
 * Does handle ANSI macros
 */
#define MAX_MACRO_ARGS 128
int dodefine(void)
{
    SYM *sp,  *spo = 0;
    DEFSTRUCT *def;
    short *args[MAX_MACRO_ARGS], count = 0;
    short *olptr;
    int p,i,j;
    int defline = lineno;
    if (ifskip)
        return incldepth == 0;
    getch();
    while (isspace(lastch))
        getch();
    if (!isstartchar(lastch))
    {
        generror(ERR_IDEXPECT, 0, 0);
        return incldepth == 0;
    }
    else
        getid();
    olptr = lptr;
    if (lastst != id)
    {
        generror(ERR_IDEXPECT, 0, 0);
        return incldepth == 0;
    }
    if ((spo = basesearch(unmangid, &defsyms, 0)) != 0)
        undef2();

    ++global_flag; /* always do #define as globals */
    sp = makesym(sc_static);
    sp->name = litlate(unmangid);
    def = xalloc(sizeof(DEFSTRUCT));
    def->args = 0;
    def->argcount = 0;
    if (lastch == '(')
    {
        int gotcomma=FALSE,nullargs=TRUE;
        getdefsym();
        getdefsym();
        while (lastst == id)
        {
            int j;
            char buf[256];
            gotcomma = FALSE;
            nullargs = FALSE;
            args[count++] = plitlate(unmangid);
            if (count > MAX_MACRO_ARGS)
                fatal("number of macro args exceeded (more than %d)", MAX_MACRO_ARGS);
            for (j=0; j < count-1; j++)
                if (!pstrcmp(args[count-1],args[j])) {
                    for (j=0; j < pstrlen(unmangid); j++)
                        buf[j] = ((short *)unmangid)[j] ;
                    buf[j] = 0 ;
                    gensymerror(ERR_DUPSYM,buf) ;
                    break ;
                }
            getdefsym();
            if (lastst != comma)
                break;
            gotcomma = TRUE;
            getdefsym();
        }
        if (lastst == ellipse && (gotcomma || nullargs))
        {
                        if (!prm_c99)
                                generror(ERR_C99_VARIABLE_MACRO_ARGS, 0, 0);
            def->flags |= DS_VARARGS;
            gotcomma = FALSE;
            getsym();
        }
        if (lastst != closepa || gotcomma)
            generror(ERR_PUNCT, closepa, 0);
        olptr = lptr - 1;
        def->args = xalloc(count *sizeof(short*));
        memcpy(def->args, args, count *sizeof(short*));
        def->argcount = count + 1;
    }
    while (isspace(*olptr))
        olptr++;
    *(--olptr) = MACRO_PLACEHOLDER;
    p = pstrlen(olptr);
    while (isspace(olptr[p - 1]))
        p--;
    olptr[p++] = MACRO_PLACEHOLDER;

    olptr[p] = 0;
    for (i=0,j=0; i < p+1; i++,j++)
        if (olptr[i]=='#' && olptr[i+1] == '#') {
            olptr[j] = REPLACED_TOKENIZING;
            i++;
        } else
            olptr[j] = olptr[i];
    def->string = plitlate(olptr);
    sp->value.s = (char*)def;
    if (spo)
    {
        int same = TRUE;
        DEFSTRUCT *odef = (DEFSTRUCT*)spo->value.s;
        if (def->argcount != odef->argcount)
            same = FALSE;
        else
        {
            int i;
            short *p, *q;
            for (i = 0; i < def->argcount - 1 && same; i++)
                if (pstrcmp(def->args[i], odef->args[i]))
                    same = FALSE;
            p = def->string;
            q = odef->string;
            while (*p && *q) {
                if (isspace(*p))
                    if (isspace(*q)) {
                        while (isspace(*p)) p++;
                        while (isspace(*q)) q++;
                    } else {
                        break ;
                    }
                else
                    if (isspace(*q)) {
                        break ;
                    }
                    else if (*p++ != *q++)
                        break ;
            }
                        if (*p)
                                while (isspace(*p))
                                        p++;
                        if (*q)
                                while (isspace(*q))
                                        q++;
            if (*p || *q)
                same = FALSE;
        }
        if (!same) {
            char buf[256],*p=buf;
            if (prm_cmangle)
                *p++ = '_';
            strcpy(p,sp->name);
            gensymerror(ERR_REDEFINE, buf);
        }
    }
    insert(sp, &defsyms);
    --global_flag;
    browse_define(unmangid, defline);
    return incldepth == 0;
}

/*
 * Undefine
 */
int doundef(void)
{
    getch();
    while (isspace(lastch))
        getch();
    if (!isstartchar(lastch))
    {
        generror(ERR_IDEXPECT, 0, 0);
        return incldepth == 0;
    }
    else
        getid();
    if (!ifskip)
        undef2();
    return (incldepth == 0);
}

//-------------------------------------------------------------------------

int undef2(void)
{
    if (lastst != id)
        generror(ERR_IDEXPECT, 0, 0);
    else
    {
        SYM **p = (SYM **)LookupHash(unmangid, defhash, HASHTABLESIZE);
        if (p)
        {
            *p = (*p)->next;
        }
    }
}

//-------------------------------------------------------------------------

void getdefsym(void)
{
    if (backupchar !=  - 1)
    {
        lastst = backupchar;
        backupchar =  - 1;
        return ;
    }
    restart:  /* we come back here after comments */
    while (isspace(lastch))
        getch();
    if (lastch ==  - 1)
        lastst = eof;
    else if (isdigit(lastch))
        getnum();
    else if (isstartchar(lastch))
    {
        lptr--;
        defid(unmangid, &lptr, 0);
        lastch =  *lptr++;
        lastst = id;
    }
    else if (getsym2())
        goto restart;
}


//-------------------------------------------------------------------------


int defid(short *name, short **p, char *q)
/*
 * Get an identifier during macro replacement
 */
{
    int count = 0, i = 0;
    while (issymchar(**p))
    {
            if (count < (prm_cmangle ? 99 : 100))
            {
                name[count++] = *(*p);
                if (q)
                    q[i++] = *(*p);
            }
        (*p)++;
    }
    if (q)
        q[count] = 0;
    name[count] = 0;
    return (count);
}

/*
 * Insert a replacement string
 */
int definsert(short *macro, short *end, short *begin, short *text, short *etext, int len, int replen)
{
    short *q;
        static short NULLTOKEN[] = { TOKENIZING_PLACEHOLDER, 0 };
    int i, p, r;
    int val;
        int stringizing = FALSE;
        q = end;
        while (isspace(*q)) q++;
        if (*q == REPLACED_TOKENIZING)
        {
                if (text[0] == 0)
                {
                        text = NULLTOKEN;
                }
        }
        else
        {
                q = begin-1;
                while (q > macro && isspace(*q)) q--;
                if (*q == REPLACED_TOKENIZING)
                {
                        if (text[0] == 0)
                        {
                                text = NULLTOKEN;
                        }
                }
                else if (*q == '#')
                {
                        stringizing = TRUE;
                }
                else
                {
                        text = etext;
                }
        }
    p = pstrlen(text);
        if (stringizing)
                p += 1;
    val = p - replen;
    r = pstrlen(begin);
    if (val + r + 1 >= len)
    {
        generror(ERR_MACROSUBS, 0, 0);
        return (INT_MIN);
    }
    if (val > 0)
        for (q = begin + r + 1; q >= end; q--)
            *(q + val) =  *q;
    else
        if (val < 0)
        {
            r = pstrlen(end) + 1;
            for (q = end; q < end + r; q++)
                    *(q + val) =  *q;
        }
        while (*text)
        *begin++ = *text++;
        if (stringizing)
                *begin++ = STRINGIZING_PLACEHOLDER;
    return (p);
}
static int oddslash(short *start, short *p)
{
    int v = 0;
    while (p >= start && *p-- == '\\')
    {
        v++;
    }
    return v & 1;
}
void defstringizing(short *macro)
{
        short *replmac = allocateMacroReplaceBuffer();
        short *p = replmac, *q = macro;
        int waiting = 0;
        while (*q)
        {
        if (!waiting && (*q == '"'
            ||  *q == '\'') && !oddslash(macro, q-1))
        {
            waiting =  *q;
                        *p++ = *q++;
        }
        else if (waiting)
        {
            if (*q == waiting && !oddslash(macro, q-1))
                waiting = 0;
                        *p++ = *q++;
        }
                else if (*q == '#' && *(q-1) != '#' && *(q+1) != '#')  /* # ## # */
                {
                    short *s, *r;
                        q++;
                        while (isspace(*q))
                                q++;
                        *p++ = '"';
                        s = r = p;
                        while (*q && *q != STRINGIZING_PLACEHOLDER)
                        {
                            while (*q == MACRO_PLACEHOLDER) q++;
                            if (*q == '\\' || *q == '"')
                            {
                                    *p++ = '\\';
                            }
                            if (*q != STRINGIZING_PLACEHOLDER)
                                *p++ = *q++;
                        }
                        *p++ = '"';
                        if (*q)
                                q++;
                        while (s < p)
                        {
                            while (isspace(*s) && isspace(s[1])) s++;
                            *r++ = *s++;
                        }
                        p = r;
                }
                else
                {
                        *p++ = *q++;
                }
        }
        *p = 0;
        pstrcpy(macro, replmac);
        freeMacroReplaceBuffer(replmac);
}
/* replace macro args */
int defreplaceargs(short *macro, int count, short **oldargs, short **newargs,
                                   short **expandedargs, short *varargs)
{
    int i, rv;
    short name[256];
    short *p = macro,  *q;
        int waiting = 0;
    while (*p)
    {
        if (!waiting && (*p == '"'
            ||  *p == '\'') && !oddslash(macro, p-1))
        {
            waiting =  *p;
        }
        else if (waiting)
        {
            if (*p == waiting && !oddslash(macro, p-1))
                waiting = 0;
        }
        else if (issymchar(*p))
        {
            q = p;
            defid(name, &p, 0);
            if (!pstrcmp(name,L"__VA_ARGS__"))
            {
				if (!prm_c99)
                                        generror(ERR_C99_VARIABLE_MACRO_ARGS, 0, 0);
                if (!varargs[0]) {
                    gensymerror(ERR_WRONGMACROARGS,"___VA_ARGS__");
                }
                else
                {
                    if ((rv = definsert(macro, p, q, varargs, varargs, MACRO_REPLACE_SIZE-(q - macro), p - q))
                            < -MACRO_REPLACE_SIZE)
                        return (FALSE);
                    else
                        p = q + rv-1;
                }
            }
            else for (i = 0; i < count; i++)
            {
                if (!pstrcmp(name, oldargs[i]))
                {
                    if ((rv = definsert(macro, p, q, newargs[i], expandedargs[i], MACRO_REPLACE_SIZE-(q - macro), p - q))
                        < - MACRO_REPLACE_SIZE)
                        return (FALSE);
                    else
                    {
                        p = q + rv-1;
                        break;
                    }
                }
            }
        }
        if (*p)
            p++;
    }
    return (TRUE);
}
void deftokenizing(short *macro)
{
    short *b = macro,  *e = macro;
        int waiting = 0;
    while (*e)
    {
        if (!waiting && (*e == '"'
            ||  *e == '\'') && !oddslash(macro, e-1))
        {
            waiting =  *e;
            *b++ = *e++;
        }
        else if (waiting)
        {
            if (*e == waiting && !oddslash(macro, e-1))
                waiting = 0;
            *b++ = *e++;
        }
        else if (*e == REPLACED_TOKENIZING)
        {
            while (b != macro && (isspace(*(b-1))|| b[-1] == MACRO_PLACEHOLDER))
                b-- ;
            while (*++e != 0 && (isspace(*e) || *e == MACRO_PLACEHOLDER)) ;
            if (b != macro && b[-1] == TOKENIZING_PLACEHOLDER && *e != TOKENIZING_PLACEHOLDER)
                b--;
            if (*e == TOKENIZING_PLACEHOLDER)
            {
                    e++;
            }
        }
        else
            *b++ = *e++;
    }
        *b = 0;
}

/* Handlers for default macros */
void cnvt(short *out, char *in)
{
    while (*in)
        *out++ =  *in++;
    *out = 0;
}

//-------------------------------------------------------------------------

char *fullqualify(char *string)
{
    static char buf[265];
    if (string[1] != ':')
    {
        char *p,  *q = string;
        getcwd(buf, 265);
        p = buf + strlen(buf);
                if (!strncmp(q, ".\\", 2))
                        q += 2;
        while (!strncmp(q, "..\\", 3))
        {
            q += 3;
            while (p > buf &&  *p != '\\')
                p--;
        }
        *p++ = '\\';
        strcpy(p, q);
        return buf;
    }
    return string;
}

//-------------------------------------------------------------------------

void filemac(short *string)
{
    short *p = string;
    char *q = infile; // fullqualify(infile);
    *p++ = '"' ;
    while(*q)
    {
        if (*q == '\\')
            *p++ = *q ;
        *p++ = *q++;
    }
    *p++ = '"';
    *p++ = 0;
}

//-------------------------------------------------------------------------

void datemac(short *string)
{
    char str1[40];
    struct tm *t1;
    time_t t2;
    time(&t2);
    t1 = localtime(&t2);
    strftime(str1, 40, "\"%b %d %Y\"", t1);
    if (str1[5] == '0')
        str1[5] = ' '; /* as asctime() */

    cnvt(string, str1);
}

void dateisomac(short *string)
{
    char str1[40];
    struct tm *t1;
    time_t t2;
    time(&t2);
    t1 = localtime(&t2);
    strftime(str1, 40, "\"%Y-%m-%d\"", t1);
    cnvt(string, str1);
}

//-------------------------------------------------------------------------

void timemac(short *string)
{
    char str1[40];
    struct tm *t1;
    time_t t2;
    time(&t2);
    t1 = localtime(&t2);
    str1[0] = '"';
    strftime(str1, 40, "\"%H:%M:%S\"", t1);
    cnvt(string, str1);
}

//-------------------------------------------------------------------------

void linemac(short *string)
{
    char str1[40];
    sprintf(str1, "%d", lineno);
    cnvt(string, str1);
}
/* Scan for default macros and replace them */
void defmacroreplace(short *macro, char *name)
{
    int i;
    macro[0] = 0;
    for (i = 0; i < INGROWNMACROS; i++)
    if (!strcmp(name, ingrownmacros[i].s))
    {
        (ingrownmacros[i].func)(macro);
        break;
    }
}

void SetupAlreadyReplaced(short *macro)
{
        short nn[MACRO_REPLACE_SIZE], *src = nn;
        int instr = FALSE;
        pstrcpy(nn, macro);
        while (*src)
        {
                if (*src == '"' && !oddslash(macro, src-1))
                        instr =  !instr;
                if (isstartchar(*src) && !instr)
                {
                        short name[256];
                        char ascii[256];
                        SYM *sp;
                        defid(name, &src, ascii);
                        if ((sp = basesearch(ascii, &defsyms, 0)) != 0 && sp->preprocessing)
                        {
                                *macro++ = REPLACED_ALREADY;
                        }
                        pstrcpy(macro, name);
                        macro += pstrlen(macro);
                }
                else
                {
                        *macro ++ = *src ++;
                }
        }
        *macro = 0;
}
//
// a preprocessing number starts with a digit or '.' followed by a digit, then
// has any number of alphanumeric charactors or any of the sequences
// E+ e+ P+ p+
int ppNumber(short *start, short *pos)
{
    int sixteensexp = FALSE;
    short *x = pos;
    if (*pos == '+' || *pos == '-' || isdigit(*pos)) // we would get here with the first alpha char following the number
    {
        // backtrack through all characters that could possibly be part of the number
        while (pos >= start &&
               issymchar(*pos) || *pos == '.' ||
               (*pos == '-' || *pos == '+') && (pos[-1] == 'e' || pos[-1] == 'E' || pos[-1] == 'p' || pos[-1] == 'P'))
        {
            if (*pos == '-' || *pos == '+') 
            {
                pos--;
                if (*pos == 'p' || *pos == 'P')
                    sixteensexp = TRUE;
            }
            pos--;
        }
        // go forward, skipping sequences that couldn't actually start a number
        pos++;
        if (!isdigit(*pos))
        {
            while (pos < x && (* pos != '.' || isdigit(pos[-1]) || !isdigit(pos[1]))) pos++;
        }
        // if we didn't get back where we started we have a number
        return pos < x && (sixteensexp || pos[0] != '0' || pos[1] != 'x' && pos[1] != 'X') ;
    }
    return FALSE;
}
//-------------------------------------------------------------------------

int replacesegment(short *start, short *end, int *inbuffer, int totallen, short **pptr)
{
    short *args[MAX_MACRO_ARGS], *expandedargs[MAX_MACRO_ARGS];
    short varargs[4096];
        short *macro = allocateMacroReplaceBuffer();
    char ascii[256];
    short name[256];
    int waiting = FALSE, rv;
    int size = 0;
    short *p,  *q;
    SYM *sp;
    int insize,rv1;
    short * orig_end = end ;
    p = start;
    while (p < end)
    {
        q = p;
        if (!waiting && (*p == '"'
            ||  *p == '\'') && !oddslash(start, p-1))
        {
            waiting =  *p;
            p++;
        }
        else if (waiting)
        {
            if (*p == waiting && !oddslash(start, p-1))
                waiting = 0;
            p++;
        }
        else if (isstartchar(*p) && !issymchar(p[-1]))
        {
                        short *q1 = p;
            defid(name, &p, ascii);
            if ((sp = basesearch(ascii, &defsyms, 0)) != 0 && q[-1] != REPLACED_ALREADY && !ppNumber(start,q-1))
            {
                DEFSTRUCT *def = sp->value.s;
                short *r, *s;
                if (def->argcount)
                {
                    int count = 0;
                    short *q2 = p;

                    varargs[0] = 0;

                    while (isspace(*q2) || *q2 == MACRO_PLACEHOLDER) q2++ ;
                    if (*(q2 - 1) == '\n')
                    {
                                                freeMacroReplaceBuffer(macro);
                        return INT_MIN + 1;
                    }
                    if (*q2++ != '(')
                    {
                        goto join;
                    }
                    p = q2;
                    if (def->argcount > 1)
                    {
                                                int lm, ln = pstrlen(name);
                        do
                        {
                            short *nm = macro;
                            int nestedparen = 0, nestedstring = 0;
                            insize = 0;
                            while (isspace(*p)) p++;
                            while (*p && (((*p != ',' &&  *p != ')') ||
                                nestedparen || nestedstring) &&  *p != '\n'))
                            {
                                if (nestedstring)
                                {
                                    if (*p == nestedstring && !oddslash(start, p-1))
                                        nestedstring = 0;
                                }
                                else if ((*p == '\'' ||  *p == '"')
                                    && !oddslash(start, p-1))
                                    nestedstring =  *p;
                                else if (*p == '(')
                                                                                 nestedparen++;
                                else if (*p == ')' && nestedparen)
                                    nestedparen--;
                                *nm++ =  *p++;
                            }
                                                        while (nm > macro && isspace(nm[-1]))
                                                                nm--;
                            *nm = 0;

                            args[count] = plitlate(macro);
                                                        insize = 0;
                                        size = pstrlen(macro) ;
                                        rv = replacesegment(macro, macro + size, &insize, totallen,0);
                                        if (rv <-MACRO_REPLACE_SIZE) {
                                                                freeMacroReplaceBuffer(macro);
                                                return rv;
                                        }
                                                        macro[rv+size] = 0;
                                                        expandedargs[count++] = plitlate(macro);
                        }
                        while (*p && *p++ == ',' && count != def->argcount-1)
                            ;
                    }
                    else
                    {
                        count = def->argcount - 1;
                        while (isspace(*p)) p++;
                        if (*p == ')')
                            p++;
                    }
                    if (*(p - 1) != ')' || count != def->argcount - 1)
                    {
                        if (count == def->argcount-1 && (def->flags & DS_VARARGS))
                        {
                            short *q3 = varargs ;
                            int nestedparen=0;
                            if (!(def->flags & DS_VARARGS))
                                                        {
                                                                freeMacroReplaceBuffer(macro);
                                return INT_MIN;
                                                        }
                            while (*p != '\n' && *p && (*p != ')' || nestedparen)) {
                                if (*p == '(')nestedparen++;
                                if (*p == ')' && nestedparen)
                                    nestedparen--;
                                *q3++ = *p++;
                            }
                            *q3 = 0 ;
                            p++ ;
//                            count = def->argcount - 1;
                        }
                        if (*(p - 1) != ')' || count != def->argcount - 1)
                        {
                            freeMacroReplaceBuffer(macro);
                            if (!*(p))
                                return  INT_MIN + 1;
                            gensymerror(ERR_WRONGMACROARGS, ascii);
                            return  INT_MIN;
                        }
                    }
                    else if (def->flags & DS_VARARGS)
                    {
                        gensymerror(ERR_WRONGMACROARGS, ascii);
                    }
                    pstrcpy(macro,def->string);
                    if (count != 0 || varargs[0])
                        if (!defreplaceargs(macro, count, def->args, args, expandedargs, varargs)) {
                                                        freeMacroReplaceBuffer(macro);
                            return  INT_MIN;
                        }
                        deftokenizing(macro);
                        defstringizing(macro);
                        r = macro, s = macro;
                        while (*r)
                        {
                                if (*r != TOKENIZING_PLACEHOLDER)
                                        *s++ = *r++;
                                else
                                        r++;
                        }
                        *s = 0;
                } else {
                        pstrcpy(macro,def->string);
                }
                                sp->preprocessing = TRUE;
                                SetupAlreadyReplaced(macro);
                if ((rv1 = definsert(start, p, q1, macro, macro, totallen -  *inbuffer,
                    p - q1)) < -MACRO_REPLACE_SIZE)
                                {
                                        freeMacroReplaceBuffer(macro);
                                        sp->preprocessing = FALSE;
                    return  rv1;
                                }
                                rv1 = (rv1 - (p - q1));
                *inbuffer += rv1;
                end += rv1;
                                p += rv1;
                                insize = 0;
                rv1 = replacesegment(q1, p, &insize, totallen, &p);
                                sp->preprocessing = FALSE;
                if (rv1 <-MACRO_REPLACE_SIZE) {
                                        freeMacroReplaceBuffer(macro);
                        return rv1;
                }
                end += rv1;
                *inbuffer += rv1;
                                insize = 0;
            }
            else
            {
                join: defmacroreplace(macro, ascii);
                if (macro[0])
                {
                    if ((rv = definsert(start, p, q, macro, macro, totallen -  *inbuffer, p -
                        q)) < - MACRO_REPLACE_SIZE)
                                        {
                                                freeMacroReplaceBuffer(macro);
                        return  rv;
                                        }
                    end += rv - (p - q);
                    *inbuffer += rv - (p - q);
                    p += rv - (p-q);
                }
            }
        }
        else
            p++;
    }
    if (pptr)
        *pptr = p ;
        freeMacroReplaceBuffer(macro);
        if (end < orig_end)
                return - (orig_end - end);
        else
            return end - orig_end ;
}

/* Scan line for macros and do replacements */
int defcheck(short *line)
{
    int inbuffer = pstrlen(line);
	int rv;
	short *p;
    macroReplaceLevel = 0;

    rv = replacesegment(line, line + inbuffer, &inbuffer, MACRO_REPLACE_SIZE,0);
	p = line;
	while (*p)
	{
		if (*p != REPLACED_ALREADY)
		{
			*line++ = *p++;
		}
		else
		{
			p++;
			if (rv >= -MACRO_REPLACE_SIZE)
				rv--;
		}
	}
	*line = 0;
	return rv;
}

//-------------------------------------------------------------------------

static void repdefines(short *lptr)
/*
 * replace 'defined' keyword in #IF and #ELIF statements
 */
{
    short *q = lptr;
    short name[100];
    char ascii[200];
    while (*lptr)
    {
        if (!pstrncmp(lptr, defkw, 7))
        {
            int needend = FALSE;
            lptr += 7;
            while (isspace(*lptr))
                lptr++;
            if (*lptr == '(')
            {
                lptr++;
                needend = TRUE;
            }
            while (isspace(*lptr))
                lptr++;
            defid(name, &lptr, ascii);
            while (isspace(*lptr))
                lptr++;
            if (needend)
                if (*lptr == ')')
                    lptr++;
                else
                    expecttoken(closepa, 0);
            if (basesearch(ascii, &defsyms, 0) != 0)
                *q++ = '1';
            else
                *q++ = '0';
            *q++ = ' ';

        }
        else
        {
            *q++ =  *lptr++;
        }
    }
    *q = 0;
}

//-------------------------------------------------------------------------

void pushif(void)
/* Push an if context */
{
    IFSTRUCT *p;
    global_flag++;
    p = xalloc(sizeof(IFSTRUCT));
    global_flag--;
    p->link = ifs;
    p->iflevel = ifskip;
    p->elsetaken = elsetaken;
    elsetaken = FALSE;
    ifs = p;
}

//-------------------------------------------------------------------------

void popif(void)
/* Pop an if context */
{
    if (ifs)
    {
        ifskip = ifs->iflevel;
        elsetaken = ifs->elsetaken;
        ifs = ifs->link;
    }
    else
    {
        ifskip = 0;
        elsetaken = 0;
    }
}

//-------------------------------------------------------------------------

void ansieol(void)
{
    if (prm_ansi)
    {
        while (isspace(*lptr))
            lptr++;
        if (*lptr)
        {
            lastch =  *lptr;
            lastst = kw_if;
            generror(ERR_UNEXPECT, 0, 0);
        }
    }
}

//-------------------------------------------------------------------------

int doifdef(int flag)
/* Handle IFDEF */
{
    SYM *sp;
    if (ifskip)
    {
        skiplevel++;
        return (incldepth == 0);
    }
    getch();
    while (isspace(lastch))
        getch();
    if (!isstartchar(lastch))
    {
        generror(ERR_IDEXPECT, 0, 0);
        return incldepth == 0;
    }
    else
        getid();
    sp = basesearch(unmangid, &defsyms, 0);
    pushif();
    if (sp && !flag || !sp && flag)
        ifskip = TRUE;
    ansieol();
    return (incldepth == 0);
}

//-------------------------------------------------------------------------

int doif(int flag)
/* Handle #if */
{
    if (ifskip)
    {
        skiplevel++;
        return (incldepth == 0);
    }
    cantnewline = TRUE;
    getsym();
    pushif();
    if (!intexpr(0))
        ifskip = TRUE;
    else
        elsetaken = TRUE;
    cantnewline = FALSE;
    ansieol();
    return (incldepth == 0);
}

//-------------------------------------------------------------------------

int doelif(void)
/* Handle #elif */
{
    int is;
    if (skiplevel)
    {
        return (incldepth == 0);
    }

    cantnewline = TRUE;
    getsym();
    is = !intexpr(0);
    cantnewline = FALSE;
    if (!elsetaken)
    {
        if (ifs)
        {
            if (!ifs->iflevel)
                        {
                                int oldifskip = ifskip;
                ifskip = !ifskip || is || elsetaken;
                                if (!oldifskip || !ifskip)
                                        elsetaken = TRUE;
                        }
        }
        else
            generror(ERR_PREPROCMATCH, 0, 0);
    }
    else
        {
        ifskip = TRUE;
                elsetaken = TRUE;
        }
    ansieol();
    return (incldepth == 0);
}

/* handle else */
int doelse(void)
{
    if (skiplevel)
    {
        return (incldepth == 0);
    }
    if (ifs)
    {
        if (!ifs->iflevel)
            ifskip = !ifskip || elsetaken;
    }
    else
        generror(ERR_PREPROCMATCH, 0, 0);
    ansieol();
    return (incldepth == 0);
}

/* HAndle endif */
int doendif(void)
{
    if (skiplevel)
    {
        skiplevel--;
        return (incldepth == 0);
    }
    if (!ifs)
        generror(ERR_PREPROCMATCH, 0, 0);
    popif();
    ansieol();
    return (incldepth == 0);
}
