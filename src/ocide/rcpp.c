/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
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

extern BOOL needingID;
extern char* rcIdFile;
extern WCHAR inputline[];
extern TABLE *gsyms, defsyms;
extern long ival;
extern char laststr[];
extern HASHREC** defhash;
extern char* infile;
extern int incconst;
extern char* rcSearchPath;
extern WCHAR* lptr;
extern int cantnewline;
extern char* infile;
extern int backupchar;
extern enum e_sym lastst;
extern char lastid[];
extern char laststr[];
extern int lastch;
extern int lineno;
extern char* inputBuffer;
extern int inputLength;
extern char* ibufPtr;
typedef struct _startups_
{
    struct _startups_* link;
    char* name;
    int prio;
} STARTUPS;
SYM *rcDefs, **rcDefTail;
int inSymFile;
char* errfile;
int errlineno = 0;
IFSTRUCT* ifshold[10];
char* inclfname[10];
FILE* inclfile[10];
int incldepth = 0;
int inclline[10];
int inclhfile[10];
int inclInputLen[10];
char* inclInputBuffer[10];
char* inclibufPtr[10];
int inhfile;
WCHAR* lptr;
LIST *incfiles = 0, *lastinc;

LIST* libincludes = 0;

IFSTRUCT* ifs = 0;
int ifskip = 0;
int elsetaken = 0;

void filemac(WCHAR* string);
void datemac(WCHAR* string);
void timemac(WCHAR* string);
void linemac(WCHAR* string);

static char* unmangid; /* In this module we have to ignore leading underscores
                        */
static WCHAR unmangidW[256];
static STARTUPS *startuplist, *rundownlist;
static WCHAR defkw[] = {'d', 'e', 'f', 'i', 'n', 'e', 'd', 0};
static int definelistcount;
static SYM* definelist[100]; /* Way deep but hey! */
static int skiplevel;
/* List of standard macros */
#define INGROWNMACROS 4

struct inmac
{
    WCHAR* s;
    void (*func)();
} ingrownmacros[INGROWNMACROS] = {{L"__FILE__", filemac},
                                  {
                                      L"__DATE__",
                                      datemac,
                                  },
                                  {L"__TIME__", timemac},
                                  {L"__LINE__", linemac}};

static void repdefines(WCHAR* lptr);

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
    needingID++;
    getsym(); /* get first word on line */
    needingID--;

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
    char* temp;
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

static int pragerror(int errornum)
{
    char buf[100], *p = buf, i = 99;
    WCHAR* s = lptr;
    while (i-- && *s && *s != '\n')
        *p++ = *s++;
    *p = 0;
    basicerror(errornum, buf);
    return (incldepth == 0);
}

//-------------------------------------------------------------------------

int dopragma(void)
{
    char buf[40], *p = buf;
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
        return;
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
    int sysinc = FALSE;
    incconst = TRUE;
    getsym();       /* get file to include */
    if (!incconst)  // gets set false automatically if a a system style include is used
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
    FILE *inputFile = MySearchPath(laststr, rcSearchPath, "r");
    if (inputFile == 0)
    {
        gensymerror(ERR_CANTOPEN, laststr);
        rv = incldepth == 0;
    }
    else
    {
        fclose(inputFile);
        LIST* list;
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
        inclInputLen[incldepth] = inputLength;
        inclInputBuffer[incldepth] = inputBuffer;
        inclibufPtr[incldepth] = ibufPtr;
        inclfname[incldepth++] = infile;
        inputLength = 0;
        int fileFlags = 0;
        inputBuffer = ReadFileData(laststr, & fileFlags);
        if (!inputBuffer)
            fatal("Internal error");
        ibufPtr = inputBuffer;
        inputLength = strlen(inputBuffer);
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

WCHAR* prcStrdup(const WCHAR* string)
{
    WCHAR* temp = rcAlloc(pstrlen(string) * sizeof(WCHAR) + sizeof(WCHAR));
    pstrcpy(temp, string);
    return temp;
}

//-------------------------------------------------------------------------

void glbdefine(char* name, char* value)
{
    {
        SYM* sp;
        WCHAR* p;
        DEFSTRUCT* def;
        if ((sp = search(name, &defsyms)) != 0)
            return;
        sp = rcAlloc(sizeof(SYM));
        sp->name = rcStrdup(name);
        def = rcAlloc(sizeof(DEFSTRUCT));
        def->args = 0;
        def->argcount = 0;
        def->string = p = rcAlloc(strlen(value) * sizeof(WCHAR) + sizeof(WCHAR));
        while (*value)
            *p++ = *value++;
        *p++ = 0;
        sp->value.s = (char*)def;
        insert(sp, &defsyms);
        return;
    }
}

//-------------------------------------------------------------------------

void getdefsym(void)
{
    if (backupchar != -1)
    {
        lastst = backupchar;
        backupchar = -1;
        return;
    }
restart: /* we come back here after comments */
    while (iswhitespacechar(lastch))
        getch();
    if (lastch == -1)
        lastst = rceof;
    else if (isdigit(lastch))
        getnum();
    else if (isstartchar(lastch))
    {
        lptr--;
        defid(unmangidW, &lptr, unmangid);
        lastch = *lptr++;
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
    SYM* sp;
    DEFSTRUCT* def;
    WCHAR *args[40], count = 0;
    WCHAR* olptr;
    int p;
    LIST* prevLines = inSymFile ? GetCachedLines() : NULL;

    needingID++;
    getsym(); /* get past #define */
    needingID--;
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
        def->args = rcAlloc(count * sizeof(WCHAR*));
        memcpy(def->args, args, count * sizeof(WCHAR*));
        def->argcount = count + 1;
    }
    while (iswhitespacechar(*olptr))
        olptr++;
    p = pstrlen(olptr);
    if (olptr[p - 1] == '\n')
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
        SYM** p = (SYM**)LookupHash(unmangid, defhash, HASHTABLESIZE);
        if (p)
        {
            *p = (*p)->next;
        }
    }
}

//-------------------------------------------------------------------------

int defid(WCHAR* name, WCHAR** p, char* q)
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
int definsert(WCHAR* end, WCHAR* begin, WCHAR* text, int len, int replen)
{
    WCHAR* q;
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
        return (-8000);
    }
    if (val > 0)
        for (q = begin + r + 1; q >= end; q--)
            *(q + val) = *q;
    else if (val < 0)
    {
        r = pstrlen(end) + 1;
        for (q = end; q < end + r; q++)
            *(q + val) = *q;
    }
    for (i = 0; i < p; i++)
        begin[i] = text[i];
    return (p);
}

/* replace macro args */
int defreplace(WCHAR* macro, int count, WCHAR** oldargs, WCHAR** newargs)
{
    int i, rv;
    int instring = 0;
    WCHAR narg[1024];
    WCHAR name[100];
    WCHAR *p = macro, *q;
    while (*p)
    {
        if (*p == instring)
            instring = 0;
        else if (*p == '\'' || *p == '"')
            instring = *p;
        else if (!instring && isstartchar(*p))
        {
            q = p;
            defid(name, &p, 0);
            for (i = 0; i < count; i++)
                if (!pstrcmp(name, oldargs[i]))
                {
                    pstrcpy(narg, newargs[i]);
                    if ((rv = definsert(p, q, narg, 1024 - (q - macro), p - q)) == -8000)
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
void cnvt(WCHAR* out, char* in)
{
    while (*in)
        *out++ = *in++;
    *out = 0;
}

//-------------------------------------------------------------------------

void filemac(WCHAR* string)
{
    char str1[40];
    sprintf(str1, "\"%s\"", infile);
    cnvt(string, str1);
}

//-------------------------------------------------------------------------

void datemac(WCHAR* string)
{
    char str1[40];
    struct tm* t1;
    time_t t2;
    time(&t2);
    t1 = localtime(&t2);
    strftime(str1, 40, "\"%b %d %Y\"", t1);
    cnvt(string, str1);
}

//-------------------------------------------------------------------------

void timemac(WCHAR* string)
{
    char str1[40];
    struct tm* t1;
    time_t t2;
    time(&t2);
    t1 = localtime(&t2);
    str1[0] = '"';
    strftime(str1, 40, "\"%X\"", t1);
    cnvt(string, str1);
}

//-------------------------------------------------------------------------

void linemac(WCHAR* string)
{
    char str1[40];
    sprintf(str1, "%d", lineno);
    cnvt(string, str1);
}
/* Scan for default macros and replace them */
void defmacroreplace(WCHAR* macro, WCHAR* name)
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
void nodefines(void) { definelistcount = 0; }

//-------------------------------------------------------------------------

int indefine(SYM* sp)
{
    int i;
    for (i = 0; i < definelistcount; i++)
        if (sp == definelist[i])
            return TRUE;

    return FALSE;
}

//-------------------------------------------------------------------------

void enterdefine(SYM* sp) { definelist[definelistcount++] = sp; }

//-------------------------------------------------------------------------

void exitdefine(void) { definelistcount--; }

//-------------------------------------------------------------------------

int replacesegment(WCHAR* start, WCHAR* end, int* inbuffer, int totallen, int* changed)
{
    WCHAR macro[1024];
    WCHAR name[1024];
    WCHAR* args[40];
    char ascii[60];
    int waiting = FALSE, rv;
    WCHAR *p, *q;
    SYM* sp;
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
                DEFSTRUCT* def = (DEFSTRUCT*)sp->value.s;
                enterdefine(sp);
                pstrcpy(macro, def->string);
                if (def->argcount)
                {
                    int count = 0;
                    WCHAR* q = p;
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
                            WCHAR* nm = name;
                            int nestedparen = 0;
                            int nestedstring = 0;
                            while (((*p != ',' && *p != ')') || nestedparen || nestedstring) && *p != '\n')
                            {
                                if (*p == '(')
                                    nestedparen++;
                                if (*p == ')' && nestedparen)
                                    nestedparen--;
                                if (nestedstring)
                                {
                                    if (*p == nestedstring)
                                        nestedstring = 0;
                                }
                                else if (*p == '\'' || *p == '"')
                                    nestedstring = *p;
                                *nm++ = *p++;
                            }
                            while (iswhitespacechar(*(nm - 1)))
                                nm--;
                            *nm = 0;
                            nm = name;
                            while (iswhitespacechar(*nm))
                                nm++;
                            args[count++] = prcStrdup(nm);
                        } while (*p++ == ',');
                    }
                    else
                        while (iswhitespacechar(*p++))
                            ;
                    if (*(p - 1) != ')' || count != def->argcount - 1)
                    {
                        if (*(p - 1) == '\n')
                            return -10;
                        gensymerror(ERR_WRONGMACROARGS, ascii);
                        return -1;
                    }
                    /* Can't replace if tokenizing next */
                    if (*p == '#' && *(p + 1) == '#')
                        continue;
                    if (count == 0)
                        goto insert;
                    if (!defreplace(macro, count, def->args, args))
                        return -1;
                }
            insert:
                if ((rv = definsert(p, q, macro, totallen - *inbuffer, p - q)) == -8000)
                    return -1;
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
            join:
                defmacroreplace(macro, name);
                if (macro[0])
                {
                    if ((rv = definsert(p, q, macro, totallen - *inbuffer, p - q)) == -8000)
                        return -1;
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
int defcheck(WCHAR* line)
{
    int x;
    WCHAR *p = line, *q;
    int inbuffer = pstrlen(line);
    int changed = FALSE;
    nodefines();
    if ((x = replacesegment(line, line + inbuffer, &inbuffer, 4096, &changed)) < 0)
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
                *q++ = *p++;
        }
        *q = 0;
    }
    return 0;
}

//-------------------------------------------------------------------------

static void repdefines(WCHAR* lptr)
/*
 * replace 'defined' keyword in #IF and #ELIF statements
 */
{
    WCHAR* q = lptr;
    WCHAR name[40];
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
            {
                if (*lptr == ')')
                    lptr++;
                else
                    expecttoken(closepa);
            }
            if (search(ascii, &defsyms) != 0)
                *q++ = '1';
            else
                *q++ = '0';
            *q++ = ' ';
        }
        else
        {
            *q++ = *lptr++;
        }
    }
    *q = 0;
}

//-------------------------------------------------------------------------

void pushif(void)
/* Push an if context */
{
    IFSTRUCT* p;
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
    SYM* sp;
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
    if ((sp && !flag) || (!sp && flag))
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
