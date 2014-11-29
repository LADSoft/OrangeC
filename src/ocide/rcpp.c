/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>

#include "header.h"
#include <time.h>

#define MAX_STRLEN 257

extern char *rcIdFile;
extern short inputline[];
extern FILE *inputFile;
extern TABLE *gsyms, defsyms;
extern long ival;
extern char laststr[];
extern HASHREC **defhash;
extern char *infile;
extern int incconst;
extern char *rcSearchPath;
extern short *lptr;
extern int cantnewline;
extern char *infile;
extern int backupchar;
extern enum e_sym lastst;
extern char lastid[];
extern char laststr[];
extern int lastch;
extern int lineno;
extern char *inputBuffer;
extern int inputLen;
extern char *ibufPtr;
typedef struct _startups_
{
    struct _startups_ *link;
    char *name;
    int prio;
} STARTUPS;
SYM *rcDefs, **rcDefTail;
int inSymFile;
char *errfile;
int errlineno = 0;
IFSTRUCT *ifshold[10];
char *inclfname[10];
FILE *inclfile[10];
int incldepth = 0;
int inclline[10];
int inclhfile[10];
int inclInputLen[10];
char *inclInputBuffer[10];
char *inclibufPtr[10];
int inhfile;
short *lptr;
LIST *incfiles = 0,  *lastinc;

LIST *libincludes = 0;

IFSTRUCT *ifs = 0;
int ifskip = 0;
int elsetaken = 0;

void filemac(short *string);
void datemac(short *string);
void timemac(short *string);
void linemac(short *string);

static char *unmangid; /* In this module we have to ignore leading underscores
    */
static WCHAR unmangidW[256];
static STARTUPS *startuplist,  *rundownlist;
static short defkw[] = 
{
    'd', 'e', 'f', 'i', 'n', 'e', 'd', 0
};
static int definelistcount;
static SYM *definelist[100]; /* Way deep but hey! */
static int skiplevel;
/* List of standard macros */
#define INGROWNMACROS 4

struct inmac
{
    short *s;
    void(*func)();
} ingrownmacros[INGROWNMACROS] = 
{
    {
        L"__FILE__", filemac
    }
    , 
    {
        L"__DATE__", datemac, 
    }
    , 
    {
        L"__TIME__", timemac
    }
    , 
    {
        L"__LINE__", linemac
    }
};

static void repdefines(short *lptr);

void pushif(void);

/* Moudle init */
void preprocini()
{
    skiplevel = 0;
    libincludes = 0;
    incldepth = 0;
    incfiles = 0;
    ifs = 0;
    ifskip = elsetaken = 0;
    unmangid = lastid;
    startuplist = rundownlist = 0;
    rcDefs = NULL;
    rcDefTail = &rcDefs;
    inSymFile = FALSE;
}

/* Preprocessor dispatch */
int preprocess(void)
{
    ++lptr;
    lastch = ' ';
    getsym(); /* get first word on line */

    if (lastst != ident)
    {
        generror(ERR_IDEXPECT, 0);
        return incldepth == 0;
    }
    if (strcmp(unmangid, "include") == 0)
        return doinclude(FALSE);
    else if (strcmp(unmangid, "define") == 0)
        return dodefine();
    else if (strcmp(unmangid, "endif") == 0)
        return doendif();
    else if (strcmp(unmangid, "else") == 0)
        return doelse();
    else if (strcmp(unmangid, "ifdef") == 0)
        return doifdef(TRUE);
    else if (strcmp(unmangid, "ifndef") == 0)
        return doifdef(FALSE);
    else if (strcmp(unmangid, "if") == 0)
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
        return (doerror());
    else if (strcmp(unmangid, "pragma") == 0)
        return (dopragma());
    else if (strcmp(unmangid, "line") == 0)
        return (doline());
    else
    {
        gensymerror(ERR_PREPROCID, unmangid);
        return incldepth == 0;
    }
}

//-------------------------------------------------------------------------

int doerror(void)
{
    char *temp;
    int i = 0;
    if (ifskip)
        return incldepth == 0;
    temp = rcAlloc(pstrlen(lptr) + 1);
    while (*lptr)
    {
        temp[i++] = *lptr;
    }
    temp[i] = 0;
    basicerror(ERR_ERROR, temp);
    return incldepth == 0;
}

//-------------------------------------------------------------------------

static int pragerror(int errno)
{
    char buf[100],  *p = buf, i = 99;
    short *s = lptr;
    while (i-- &&  *s &&  *s != '\n')
        *p++ =  *s++;
    *p = 0;
    basicerror(errno, buf);
    return (incldepth == 0);
}

//-------------------------------------------------------------------------

int dopragma(void)
{
    char buf[40],  *p = buf;
    int val = 0;
    if (ifskip)
        return incldepth == 0;
        cantnewline = TRUE;
    getsym();
        cantnewline = FALSE;
    if (lastst != ident)
        return incldepth == 0;
    if (!strcmp(unmangid, "error"))
        return pragerror(ERR_USERERR);
    else if (!strcmp(unmangid, "warning"))
        return pragerror(ERR_USERWARN);
    else
        return incldepth == 0;

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
    getsym();
    if (lastst != iconst)
        gensymerror(ERR_PREPROCID, "#line");
    else
    {
        n = ival;
        getsym();
        if (lastst != sconst)
            gensymerror(ERR_PREPROCID, "#line");
        else
        {
            errfile = rcStrdup(laststr);
            errlineno = n - 1;
        }
    }
    return incldepth == 0;
}

void ReadFileName(void)
{
    int done = FALSE;
    if (lastst == sconst)
    {
        getsym();
        return ;
    }
    laststr[0] = 0;
    while (!done)
    {
        switch (lastst)
        {
            case divide:
            case backslash:
                strcat(laststr, "\\");
                break;
            case dot:
                strcat(laststr, ".");
                break;
            case ident:
                strcat(laststr, lastid);
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
            getsym();
    }
}
//-------------------------------------------------------------------------

int doinclude(int unquoted)
/*
 * HAndle include files
 */
{
    int rv;
    FILE *oldfile = inputFile;
    int sysinc = FALSE;
    incconst = TRUE;
    getsym(); /* get file to include */
    if (!incconst) // gets set false automatically if a a system style include is used
        sysinc = TRUE;
    incconst = FALSE;
    if (ifskip)
        return incldepth == 0;
    if (!unquoted)
    {
        if (lastst != sconst)
        {
            gensymerror(ERR_INCLFILE, "include");
            return incldepth == 0;
        }
    }
    else if (lastst == ident)
        ReadFileName();
    if (incldepth > 9)
    {
        generror(ERR_PREPROCID, 0);
        return incldepth == 0;
    }
    inputFile = MySearchPath(laststr, rcSearchPath, "r");
    if (inputFile == 0)
    {
        gensymerror(ERR_CANTOPEN, laststr);
        inputFile = oldfile;
        rv = incldepth == 0;
    }
    else
    {
        LIST *list;
        if (!sysinc && !rcIdFile && incldepth == 0 && !strstr(laststr, "windows.h"))
        {
            inSymFile = TRUE;
            rcIdFile = rcStrdup(laststr);
        }
        pushif();
        ifshold[incldepth] = ifs;
        elsetaken = 0;
        ifskip = 0;
        ifs = 0;
        inclhfile[incldepth] = inhfile;
        inclline[incldepth] = lineno;
        inclfile[incldepth] = oldfile; /* push current input file */
        inclInputLen[incldepth] = inputLen;
        inclInputBuffer[incldepth] = inputBuffer;
        inclibufPtr[incldepth] = ibufPtr;
        inclfname[incldepth++] = infile;
        inputLen = 0;
        inputBuffer = calloc(INPUT_BUFFER_LEN , 1);
        infile = rcStrdup(laststr);
        list = rcAlloc(sizeof(LIST));
        list->data = infile;
        list->next = 0;
        if (incfiles)
            lastinc = lastinc->next = list;
        else
            incfiles = lastinc = list;
        errfile = infile;
        errlineno = 0;
        rv = incldepth == 1;
        lineno = 0;
        inhfile = !stricmp(infile + strlen(infile) - 2, ".h");
    }
    return rv;
}

//-------------------------------------------------------------------------

short *prcStrdup(short *string)
{
    short *temp = rcAlloc(pstrlen(string) *sizeof(short) + sizeof(short))
        ;
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
        if ((sp = search(name, &defsyms)) != 0)
            return ;
        sp = rcAlloc(sizeof(SYM));
        sp->name = rcStrdup(name);
        def = rcAlloc(sizeof(DEFSTRUCT));
        def->args = 0;
        def->argcount = 0;
        def->string = p = rcAlloc(strlen(value) *sizeof(short) + 2);
        while (*value)
            *p++ =  *value++;
        *p++ = 0;
        sp->value.s = (char*)def;
        insert(sp, &defsyms);
        return ;
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
    while (iswhitespacechar(lastch))
        getch();
    if (lastch ==  - 1)
        lastst = eof;
    else if (isdigit(lastch))
        getnum();
    else if (isstartchar(lastch))
    {
        lptr--;
        defid(unmangidW, &lptr, unmangid);
        lastch =  *lptr++;
        lastst = ident;
    }
    else if (getsym2())
        goto restart;
}

/* Handle #defines
 * Doesn't check for redefine with different value
 * Does handle ANSI macros
 */
int dodefine(void)
{
    SYM *sp;
    DEFSTRUCT *def;
    short *args[40], count = 0;
    short *olptr;
    int p;
    LIST *prevLines = GetCachedLines();

    getsym(); /* get past #define */
    if (ifskip)
        return incldepth == 0;
    olptr = lptr;
    if (lastst != ident)
    {
        generror(ERR_IDEXPECT, 0);
        return incldepth == 0;
    }
    if ((sp = search(unmangid, &defsyms)) != 0)
        undef2();
    sp = rcAlloc(sizeof(SYM));
    sp->name = rcStrdup(unmangid);
    def = rcAlloc(sizeof(DEFSTRUCT));
    def->args = 0;
    def->argcount = 0;
    if (lastch == '(')
    {
        getdefsym();
        getdefsym();
        while (lastst == ident)
        {
            args[count++] = prcStrdup(unmangidW);
            getdefsym();
            if (lastst != comma)
                break;
            getdefsym();
        }
        if (lastst != closepa)
            generror(ERR_PUNCT, closepa);
        olptr = lptr - 1;
        def->args = rcAlloc(count *sizeof(short*));
        memcpy(def->args, args, count *sizeof(short*));
        def->argcount = count + 1;
    }
    while (iswhitespacechar(*olptr))
        olptr++;
    p = pstrlen(olptr);
    if (olptr[p - 1] == 0x0a)
        olptr[p - 1] = 0;
    def->string = prcStrdup(olptr);
    sp->value.s = (char*)def;
    sp->lines = prevLines;
    insert(sp, &defsyms);
    if (inSymFile)
    {
        *rcDefTail = sp;
        rcDefTail = &sp->xref;
    }
    return incldepth == 0;
}

/*
 * Undefine
 */
int doundef(void)
{
    getsym();
    if (!ifskip)
        undef2();
    return (incldepth == 0);
}

//-------------------------------------------------------------------------

void undef2(void)
{
    if (lastst != ident)
        generror(ERR_IDEXPECT, 0);
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

int defid(short *name, short **p, char *q)
/*
 * Get an identifier during macro replacement
 */
{
    int count = 0, i = 0;
    while (issymchar(**p))
    {
        if (count < 100)
        {
            name[count++] = *(*p);
            if (q)
                q[i++] = **p;
        }
        (*p)++;
    }
    if (q)
    {
        q[i] = '\0';
    }
    name[count] = 0;
    return (count);
}

/* 
 * Insert a replacement string
 */
int definsert(short *end, short *begin, short *text, int len, int replen)
{
    short *q;
    int i, p, r;
    int val;
    if (begin != inputline)
    if (*(begin - 1) == '#')
    {
        if (*(begin - 2) != '#')
        {
            begin--;
            replen++;
            r = pstrlen(text);

            text[r++] = '\"';
            text[r] = 0;
            for (i = r; i >= 0; i--)
                text[i + 1] = text[i];
            *text = '\"';
        }
    }
    p = pstrlen(text);
    val = p - replen;
    r = pstrlen(begin);
    if (val + (int)pstrlen(begin) + 1 >= len)
    {
        generror(ERR_MACROSUBS, 0);
        return ( - 8000);
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
    for (i = 0; i < p; i++)
        begin[i] = text[i];
    return (p);
}

/* replace macro args */
int defreplace(short *macro, int count, short **oldargs, short **newargs)
{
    int i, rv;
    int instring = 0;
    short narg[1024];
    short name[100];
    short *p = macro,  *q;
    while (*p)
    {
        if (*p == instring)
            instring = 0;
        else if (*p == '\'' ||  *p == '"')
            instring =  *p;
        else if (!instring && isstartchar(*p))
        {
            q = p;
            defid(name, &p, 0);
            for (i = 0; i < count; i++)
            if (!pstrcmp(name, oldargs[i]))
            {
                pstrcpy(narg, newargs[i]);
                if ((rv = definsert(p, q, narg, 1024-(q - macro), p - q)) ==  -
                    8000)
                    return (FALSE);
                else
                {
                    p = q + rv;
                    break;
                }
            }
        }
        p++;
    }
    return (TRUE);
}

/* Handlers for default macros */
void cnvt(short *out, char *in)
{
    while (*in)
        *out++ =  *in++;
    *out = 0;
}

//-------------------------------------------------------------------------

void filemac(short *string)
{
    char str1[40];
    sprintf(str1, "\"%s\"", infile);
    cnvt(string, str1);
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
    strftime(str1, 40, "\"%X\"", t1);
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
void defmacroreplace(short *macro, short *name)
{
    int i;
    macro[0] = 0;
    for (i = 0; i < INGROWNMACROS; i++)
    if (!pstrcmp(name, ingrownmacros[i].s))
    {
        (ingrownmacros[i].func)(macro);
        break;
    }
}

/* The next few functions support recursion blocking for macros.
 * Basicall a list of all active macros is kept and if a lookup would 
 * result in one of those macros, no replacement is done.
 */
void nodefines(void)
{
    definelistcount = 0;
}

//-------------------------------------------------------------------------

int indefine(SYM *sp)
{
    int i;
    for (i = 0; i < definelistcount; i++)
        if (sp == definelist[i])
            return TRUE;

    return FALSE;
}

//-------------------------------------------------------------------------

void enterdefine(SYM *sp)
{
    definelist[definelistcount++] = sp;
}

//-------------------------------------------------------------------------

void exitdefine(void)
{
    definelistcount--;
}

//-------------------------------------------------------------------------

int replacesegment(short *start, short *end, int *inbuffer, int totallen, int
    *changed)
{
    short macro[1024];
    short name[1024];
    short *args[40];
    char ascii[60];
    int waiting = FALSE, rv;
    short *p,  *q;
    SYM *sp;
    p = start;
    while (p < end)
    {
        q = p;
        if (*p == '"')
        {
            waiting = !waiting;
            p++;
        }
        else if (waiting)
            p++;
        else if (isstartchar(*p))
        {
            defid(name, &p, ascii);
            if ((sp = search(ascii, &defsyms)) != 0 && !indefine(sp))
            {
                DEFSTRUCT *def = (DEFSTRUCT *)sp->value.s;
                enterdefine(sp);
                pstrcpy(macro, def->string);
                if (def->argcount)
                {
                    int count = 0;
                    short *q = p;
                    while (iswhitespacechar(*q))
                        q++;
                    if (*q++ != '(')
                    {
                        exitdefine();
                        goto join;
                    }
                    p = q;
                    if (def->argcount > 1)
                    {
                        do
                        {
                            short *nm = name;
                            int nestedparen = 0;
                            int nestedstring = 0;
                            while (((*p != ',' &&  *p != ')') || nestedparen ||
                                nestedstring) &&  *p != '\n')
                            {
                                if (*p == '(')nestedparen++;
                                if (*p == ')' && nestedparen)
                                    nestedparen--;
                                if (nestedstring)
                                {
                                    if (*p == nestedstring)
                                        nestedstring = 0;
                                }
                                else if (*p == '\'' ||  *p == '"')
                                    nestedstring =  *p;
                                *nm++ =  *p++;
                            }
                            while (iswhitespacechar(*(nm - 1)))
                                nm--;
                            *nm = 0;
                            nm = name;
                            while (iswhitespacechar(*nm))
                                nm++;
                            args[count++] = prcStrdup(nm);
                        }
                        while (*p++ == ',')
                            ;
                    }
                    else
                        while (iswhitespacechar(*p++))
                            ;
                    if (*(p - 1) != ')' || count != def->argcount - 1)
                    {
                        if (*(p - 1) == '\n')
                            return  - 10;
                        gensymerror(ERR_WRONGMACROARGS, ascii);
                        return  - 1;
                    }
                    /* Can't replace if tokenizing next */
                    if (*p == '#' && *(p + 1) == '#')
                        continue;
                    if (count == 0)
                        goto insert;
                    if (!defreplace(macro, count, def->args, args))
                        return  - 1;
                }
                insert: if ((rv = definsert(p, q, macro, totallen -  *inbuffer,
                    p - q)) ==  - 8000)
                    return  - 1;
                *changed = TRUE;
                rv = replacesegment(q, q + rv, inbuffer, totallen, changed);
                if (rv < 0)
                    return rv;
                end += rv - (p - q);
                *inbuffer += rv - (p - q);
                p = q + rv;
                exitdefine();
                *changed = TRUE;
            }
            else
            {
                join: defmacroreplace(macro, name);
                if (macro[0])
                {
                    if ((rv = definsert(p, q, macro, totallen -  *inbuffer, p -
                        q)) ==  - 8000)
                        return  - 1;
                    *changed = TRUE;
                    end += rv - (p - q);
                    *inbuffer += rv - (p - q);
                    p = q + rv;
                }
            }
        }
        else
            p++;
    }
    return (int)(end - start);
}

/* Scan line for macros and do replacements */
int defcheck(short *line)
{
    int x;
    short *p = line,  *q;
    int inbuffer = pstrlen(line);
    int changed = FALSE;
    nodefines();
    if ((x = replacesegment(line, line + inbuffer, &inbuffer, 4096, &changed))
        < 0)
        return x;

    /* Token pasting */
    if (changed)
    {
        p = q = line;
        while (*p)
        {
            if (*p == '#' && *(p + 1) == '#')
                p += 2;
            else
                *q++ =  *p++;
        }
        *q = 0;
    }
    return 0;
}

//-------------------------------------------------------------------------

static void repdefines(short *lptr)
/*
 * replace 'defined' keyword in #IF and #ELIF statements
 */
{
    short *q = lptr;
    short name[40];
    char ascii[60];
    while (*lptr)
    {
        if (!pstrncmp(lptr, defkw, 7))
        {
            int needend = FALSE;
            lptr += 7;
            if (*lptr == '(')
            {
                lptr++;
                needend = TRUE;
            }
            while (iswhitespacechar(*lptr))
                lptr++;
            defid(name, &lptr, ascii);
            while (iswhitespacechar(*lptr))
                lptr++;
            if (needend)
                if (*lptr == ')')
                    lptr++;
                else
                    expecttoken(closepa);
            if (search(ascii, &defsyms) != 0)
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
    p = rcAlloc(sizeof(IFSTRUCT));
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
        generror(ERR_IDEXPECT, 0);
        return incldepth == 0;
    }
    else
        getid();
    sp = search(unmangid, &defsyms);
    pushif();
    if (sp && !flag || !sp && flag)
        ifskip = TRUE;
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
    getsym();
    pushif();
        cantnewline = TRUE;
    if (!intexpr())
        ifskip = TRUE;
    else
        elsetaken = TRUE;
        cantnewline = FALSE;
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

    getsym();
            cantnewline = TRUE;
    is = !intexpr();
            cantnewline = FALSE;
    if (!elsetaken)
    {
        if (ifs)
        {
            if (!ifs->iflevel)
                ifskip = !ifskip || is || elsetaken;
            if (!ifskip)
                elsetaken = TRUE;
        }
        else
            generror(ERR_PREPROCMATCH, 0);
    }
    else
        ifskip = TRUE;
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
        generror(ERR_PREPROCMATCH, 0);
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
        generror(ERR_PREPROCMATCH, 0);
    popif();
    return (incldepth == 0);
}
