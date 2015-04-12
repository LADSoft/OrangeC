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
// assumes tabs aren't going to get reset yet
#define STRICT 
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "header.h"
#include <richedit.h>
#include <limits.h>
#include "c_types.h"
#include "codecomp.h"
#include <stdlib.h>
#include <ctype.h>
#include <process.h>
#include "symtypes.h"
extern SCOPE *activeScope;
extern LOGFONT systemDialogFont;

#define TRANSPARENT_COLOR 0x872395

// This defines the maximum range for the horizontal scroll bar on the window
#define MAX_HSCROLL 256

// The KEYLIST structure holds information about text which should be
// colorized.  It gives the name of a keyword (e.g. 'int') and the color
// to display it in.

typedef struct
{
    char *text;
    unsigned char Color;
//    COLORREF *color;
} KEYLIST;

static COLORREF keywordColor = 0xff8000;
static COLORREF numberColor = 0x0000ff;
static COLORREF commentColor = 0x00c000;
static COLORREF stringColor = 0x8000ff;
static COLORREF directiveColor = 0xff0000;
static COLORREF backgroundColor = 0xffffff;
static COLORREF readonlyBackgroundColor = 0xc0c0c0;
static COLORREF textColor = 0;
static COLORREF highlightColor = 0x80ffff;
static COLORREF selectedTextColor = 0xffffff;
static COLORREF selectedBackgroundColor = 0xff0000;
static COLORREF columnbarColor = 0xccccff;

static COLORREF defineColor = 0x208080;
static COLORREF functionColor = 0xc00000;
static COLORREF parameterColor = 0x0080ff;
static COLORREF typedefColor = 0xc0c000;
static COLORREF tagColor = 0xc0c000;
static COLORREF autoColor = 0x0080ff;
static COLORREF localStaticColor = 0x00e0ff;
static COLORREF staticColor = 0x00e0ff;
static COLORREF globalColor = 0xffff00;
static COLORREF externColor = 0xe0e000;
static COLORREF labelColor = 0xc00000;
static COLORREF memberColor = 0xc0c000;

#define C_BACKGROUND 0
#define C_READONLYBACKGROUND 1
#define C_TEXT 2
#define C_HIGHLIGHT 3
#define C_KEYWORD 4
#define C_COMMENT 5
#define C_NUMBER 6
#define C_STRING 7
#define C_ESCAPE 8
#define C_DEFINE 9
#define C_FUNCTION 10
#define C_PARAMETER 11
#define C_TYPEDEF 12
#define C_TAG 13
#define C_AUTO 14
#define C_LOCALSTATIC 15
#define C_STATIC 16
#define C_GLOBAL 17
#define C_EXTERNAL 18
#define C_LABEL 19
#define C_MEMBER 20
#define C_SYS_WINDOWBACKGROUND 31
static COLORREF *colors[] = { &backgroundColor, &readonlyBackgroundColor, &textColor, &highlightColor, &keywordColor,
                        &commentColor, &numberColor, &stringColor, &directiveColor ,
                        &defineColor, &functionColor, &parameterColor, &typedefColor, &tagColor, &autoColor,
                        &localStaticColor, &staticColor, &globalColor, &externColor, &labelColor, &memberColor
                        };


LOGFONT EditFont = 
{
     - 13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH 
         | FF_DONTCARE,
        CONTROL_FONT
};

// COLORREF selcolor = 0 ;

// For drawing, put a little margin at the left
#define LEFTMARGIN 5

extern HWND hwndFind;
// well we aren't 'totally' independent of the rest of CCIDE...
// the following is used for the tooltip
extern enum DebugState uState;
extern HINSTANCE hInstance;
extern int progLanguage;

// and the next is highlight data filled in by the find
char highlightText[256] ;
int highlightCaseSensitive;
int highlightWholeWord;

static int page_size = -1;
static HWND codecompleteBox;
static BOOL inStructBox;
static HWND hwndShowFunc;

void SendUpdate(HWND hwnd);
static int LookupColorizeEntry(COLORIZE_HASH_ENTRY *entries[], char *name, int line);
// The C_keywordList is a list of all keywords, with colorization info
static KEYLIST C_keywordList[] = 
{
    #include "c_kw.h"
};
static KEYLIST ASM_keywordList[] = 
{
    #include "asm_kw.h"
};
static KEYLIST RC_keywordList[] =
{
#include "rc_kw.h"
};
void LoadColors(void)
{
    keywordColor = PropGetColor(NULL, "COLOR_KEYWORD");
    numberColor = PropGetColor(NULL, "COLOR_NUMBER");
    commentColor = PropGetColor(NULL, "COLOR_COMMENT");
    stringColor = PropGetColor(NULL, "COLOR_STRING");
    directiveColor = PropGetColor(NULL, "COLOR_DIRECTIVE");
    backgroundColor = PropGetColor(NULL, "COLOR_BACKGROUND");
    readonlyBackgroundColor = PropGetColor(NULL, "COLOR_READONLYBACKGROUND");
    textColor = PropGetColor(NULL, "COLOR_TEXT");
    selectedBackgroundColor = PropGetColor(NULL, "COLOR_SELECTEDBACKGROUND");
    selectedTextColor = PropGetColor(NULL, "COLOR_SELECTEDTEXT");
    highlightColor = PropGetColor(NULL, "COLOR_HIGHLIGHT");
    columnbarColor = PropGetColor(NULL, "COLOR_COLUMNBAR");

    defineColor = PropGetColor(NULL, "COLOR_DEFINE");
    functionColor = PropGetColor(NULL, "COLOR_FUNCTION");
    parameterColor = PropGetColor(NULL, "COLOR_PARAMETER");
    typedefColor = PropGetColor(NULL, "COLOR_TYPEDEF");
    tagColor = PropGetColor(NULL, "COLOR_TAG");
    autoColor = PropGetColor(NULL, "COLOR_AUTO");
    localStaticColor = PropGetColor(NULL, "COLOR_LOCALSTATIC");
    staticColor = PropGetColor(NULL, "COLOR_STATIC");
    globalColor = PropGetColor(NULL, "COLOR_GLOBAL");
    externColor = PropGetColor(NULL, "COLOR_EXTERN");
    labelColor = PropGetColor(NULL, "COLOR_LABEL");
    memberColor = PropGetColor(NULL, "COLOR_MEMBER");

}

/* using windows to allocate memory because the borland runtime gets confused
 * with large amounts of data
 */
#define SYSALLOC
void getPageSize()
{
    if (page_size == -1)
    {
        SYSTEM_INFO info ;
        GetSystemInfo(&info) ;
        page_size = info.dwPageSize ;                    
    }
}
int freemem(EDITDATA *p)
{
#ifdef SYSALLOC
    VirtualFree(p->cd->text, p->cd->textmaxlen, MEM_DECOMMIT);
    VirtualFree(p->cd->text, 0, MEM_RELEASE ) ;
#else
    free(p->cd->text);
#endif
    return 0;
}
int allocmem(EDITDATA *p, int size)
{
    size *= sizeof(INTERNAL_CHAR) ;
    size += 64 * 1024 ;
    size = size - size % (64 * 1024) ;
    if (!p->cd->text)
    {
#ifdef SYSALLOC
        p->cd->text = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
#else
        p->cd->text = calloc(1, size);
#endif
        if (!p->cd->text) 
        {
            ExtendedMessageBox("Error",0,"Out Of Memory");
            return 0 ;
        }
        p->cd->textmaxlen = size ;
        return 1 ;
    }
    else
    {
        INTERNAL_CHAR *x ;
#ifdef SYSALLOC
           x = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS) ;
        if (!x || !VirtualAlloc(x, (p->cd->textlen + 1) * sizeof(INTERNAL_CHAR), MEM_COMMIT, PAGE_READWRITE)) 
#else
        x = calloc(1, size);
        if (!x)
#endif
        {
            ExtendedMessageBox("Error",0,"Out Of Memory");
               return 0 ;
        }
        memcpy(x, p->cd->text, (p->cd->textlen + 1) * sizeof(INTERNAL_CHAR));
        freemem(p);
        p->cd->text = x;
        p->cd->textmaxlen = size;
        return 1 ;
    }
}
int commitmem(EDITDATA *p, int size)
{
    int size1 = size * sizeof(INTERNAL_CHAR);
    getPageSize() ;
    size1 += page_size ;
    size1 = size1 - size1 % page_size ;
    if (size1 > p->cd->textmaxlen)
    {
        if (!allocmem(p,size))
            return 0 ;
    }
#ifdef SYSALLOC
    if (!VirtualAlloc(p->cd->text, size1, MEM_COMMIT, PAGE_READWRITE)) 
    {
        ExtendedMessageBox("Error",0,"Cannot commit memory") ;
        return 0 ;
    }
#endif
    return 1 ;
}
void reparse(HWND hwnd, EDITDATA *p)
{
    if (p->cd->reparseTimerID)
        KillTimer(hwnd,p->cd->reparseTimerID);
    p->cd->reparseTimerID = SetTimer(hwnd, 2, 4000, 0);
}
/**********************************************************************
 * Colorize marks a range of text with a specific color and attributes
 **********************************************************************/
static void Colorize(INTERNAL_CHAR *buf, int start, int len, int color, int
    italic)
{
    int dwEffects = 0, i;
    INTERNAL_CHAR *p = buf + start;
    if (italic)
        dwEffects |= CFE_ITALIC;
    if (!color)
        dwEffects |= CFE_AUTOCOLOR;
    //   else 
    //   {
    //		if (!italic)
    //         dwEffects |= CFE_BOLD ;
    //	}
    for (i = 0; i < len; i++, p++)
    {
        p->Color = color;
        p->effect = dwEffects;
    }
}

/**********************************************************************
 * keysym returns true if it is a symbol that can be used in a keyword
 **********************************************************************/
int keysym(char x)
{
    return isalnum(x) || x == '_';
}

/**********************************************************************
 * strpstr  finds a text string within a string organized as internal
 * characters.  Returns 0 if it couldn't find the string
 **********************************************************************/
INTERNAL_CHAR *strpstr(INTERNAL_CHAR *t, char *text, int len)
{
    while (t->ch && len)
    {
        if (t->ch == text[0])
        {
            char *t1 = text;
            INTERNAL_CHAR *it1 = t;
            while (*t1 && it1->ch ==  *t1)
            {
                t1++;
                it1++;
            }
            if (! *t1)
                return t;
        }
        t++;
        len--;
    }
    return 0;

}

/**********************************************************************
 * strplen  finds the length of an internal_char array
 **********************************************************************/

int strplen(INTERNAL_CHAR *t)
{
    int rv = 0;
    while (t->ch)
        rv++, t++;
    return rv;
}

/**********************************************************************
 * backalpha goes backwards to find out if the current (numeric)
 * character is part of an identifier, or if it is a standalone number.
 * returns TRUE if part of an identifier.  Used in colorizing numbers
 **********************************************************************/

static int backalpha(INTERNAL_CHAR *buf, int i)
{
    while (i >= 0)
    {
        if (isalpha(buf[i].ch))
            return TRUE;
        if (buf[i].ch == '_')
            return TRUE;
        if (!isdigit(buf[i].ch))
            return FALSE;
        i--;
    }
    return FALSE;

}

//-------------------------------------------------------------------------

int pcmp(INTERNAL_CHAR *s, char *t, int preproc, int *retlen, int
    caseinsensitive, int bykey)
{
    *retlen = 0;
    while (*t && s->ch && (!bykey || keysym(s->ch)))
    {
        int val = s->ch;
        int ch = *t;
        if (caseinsensitive)
        {
            val = tolower(val);
            ch = tolower(ch);
        }
        if (val <  ch)
            return  - 1;
        else if (val >  ch)
            return 1;
        else
        {
            if (ch == preproc)
            {
                while (isspace(s[1].ch) && s[1].ch != '\n')
                    s++, (*retlen)++;
            }
            s++, t++, (*retlen)++;
        }
    }
    if (*t)
        return -1;
    return bykey && keysym(s->ch) ;
}

/**********************************************************************
 * See if a keyword matches the current text location
 **********************************************************************/
KEYLIST *matchkeyword(KEYLIST *table, int tabsize, int preproc, INTERNAL_CHAR
    *t, int *retlen, int insensitive)
{
    int top = tabsize;
    int bottom =  - 1;
    int v;
    while (top - bottom > 1)
    {
        int mid = (top + bottom) / 2;
        v = pcmp(t, table[mid].text, preproc, retlen, insensitive, t->ch != '#');
        if (v < 0)
        {
            top = mid;
        }
        else
        {
            bottom = mid;
        }
    }
    if (bottom ==  - 1)
        return 0;
    v = pcmp(t, table[bottom].text, preproc, retlen, insensitive, t->ch != '#');
    if (v)
        return 0;
    return  &table[bottom];
}

/**********************************************************************
 * SearchKeywords searches a range of INTERNAL_CHARs for keywords,
 * numbers, and strings, and colorizes them
 **********************************************************************/

static void SearchKeywords(COLORIZE_HASH_ENTRY *entries[], INTERNAL_CHAR *buf, 
                           int chars, int start, int type, int bkColor)
{
    int i;
    KEYLIST *sr = C_keywordList;
    int size = sizeof(C_keywordList) / sizeof(KEYLIST);
    int preproc = '#';
    int hexflg = FALSE, binflg = FALSE;
    int xchars = chars;
    INTERNAL_CHAR *t = buf;
    int lineno = 1;
    BOOLEAN hashed;
    while (t < buf + start)
    {
        if (t++->ch == '\n')
            lineno++;
    }
    t = buf + start;
    if (type == LANGUAGE_ASM)
    {
        sr = ASM_keywordList;
        size = sizeof(ASM_keywordList) / sizeof(KEYLIST);
        preproc = '%';
    }
    else if (type == LANGUAGE_RC)
    {
        sr = RC_keywordList;
        size = sizeof(RC_keywordList) / sizeof(KEYLIST);
        preproc = '#';
    }
    while (t->ch && xchars > 0)
    {
        while (t->ch && (t->Color & 0xf) == C_COMMENT && xchars > 0)
        {
            if (t->ch == '\n')
                lineno++;
            t++, xchars--;
        }
        if (xchars > 0 && (t == buf || (!keysym(t[ - 1].ch) && (keysym(t->ch) ||
            t->ch == preproc))))
        {
            int len;
            KEYLIST *p = matchkeyword(sr, size, preproc, t, &len, type ==
                LANGUAGE_ASM || type == LANGUAGE_RC);
            if (p)
            {
                Colorize(buf, t - buf, len,  (bkColor << 5) + p->Color, FALSE);
                t += len;
                xchars -= len;
            }
            else
            {
                int type;
                char name[256],*p=name;
                INTERNAL_CHAR *t1 = t;
                while (keysym(t1->ch) && p < name + 250)
                {
                    *p++ = t1++->ch;
                }
                *p = 0;
                if ((t != buf && t[-1].ch == '.') || (t > buf + 1 && t[-1].ch == '>' && t[-2].ch == '-'))
                {
                        len = p - name;
                        Colorize(buf, t - buf, len,  (bkColor << 5) + C_MEMBER, FALSE);
                        t += len;
                        xchars -= len;
                }
                else
                {
                    type = LookupColorizeEntry(entries, name, lineno);
                    if (type >= 0)
                    {
                        len = p - name;
                        Colorize(buf, t - buf, len,  (bkColor << 5) + (type - ST_DEFINE + C_DEFINE), FALSE);
                        t += len;
                        xchars -= len;
                    }
                    else
                    {
                        if (t->ch == '\n')
                            lineno++;
                        if (t->Color == (C_HIGHLIGHT << 5) + C_TEXT)
                            t->Color = (bkColor << 5) + C_TEXT;
                        t++, xchars--;
                    }
                }
        }
        }
        else
        {
            if (t->ch == '\n')
                lineno++;
            if (t->Color == (C_HIGHLIGHT << 5) + C_TEXT)
                t->Color = (bkColor << 5) + C_TEXT;
            t++, xchars--;
        }
    }
    for (i = 0; i < chars; i++)
	{
        if (i ==0 || buf[start+i].ch =='\n')
        {
            hashed = FALSE;
            while (i < chars && isspace(buf[start+i].ch)) i++;
            if (i >= chars)
                break;
            if (buf[start+i].ch == '#') 
            {
                i ++;
                while (i < chars && isspace(buf[start+i].ch)) i++;
                if (i >= chars)
                    break;
                hashed = buf[start+i].ch == 'i' && buf[start + i + 1].ch == 'n'; // check for #include
            }
        }
        if ((buf[start + i].Color & 0xf) != C_COMMENT)
        {
            int len;
            if (highlightText[0] && 
                !pcmp(buf + start + i, highlightText, preproc, &len, !highlightCaseSensitive, FALSE)
                && (!highlightWholeWord || (i == 0 || ((!isalnum(buf[i-1].ch)) && buf[i-1].ch != '_' 
				&& !isalnum(buf[i+len].ch) && buf[i+len].ch != '_'))))
            {
                Colorize(buf, start + i, len,  (C_HIGHLIGHT << 5) + C_TEXT, FALSE);
                i += len - 1;
            }
            else if (type == LANGUAGE_ASM && buf[start+i].ch == '$')
            {
                len = 1;
                while (isxdigit(buf[start + i + len].ch))
                    len++;
                Colorize(buf, start + i, len, (bkColor << 5) | C_NUMBER, FALSE);
                i += len - 1;
            }
            else if (isdigit(buf[start + i].ch))
            {
                if (!backalpha(buf, start + i - 1))
                {
                    int j = i;
                    char ch = buf[start + i++].ch;
                    if (type == LANGUAGE_C || type == LANGUAGE_RC)
                    {
                        if (isdigit(ch) || ch == '.')
                        {
                            char oc = ch;
                            ch = buf[start + i].ch;
                            hexflg = oc == '0' && (ch == 'x' || ch == 'X');
                            binflg = oc == '0' && (ch == 'b' || ch == 'B');
                            if (hexflg || binflg)
                                ch = buf [start + ++i].ch;
                            while (ch == '.' || (!binflg && isdigit(ch)) || (binflg && (ch == '0' || ch == '1'))
                                || (hexflg && (isxdigit(ch) || ch =='p' || ch == 'P')) ||
                                  (!hexflg && (ch == 'e' || ch== 'E')))
                            {
                                    i++;
                                    if ((!hexflg && ch >= 'A') || (hexflg && (ch =='p' || ch == 'P')))
                                    {
                                        ch = buf[start+i].ch;
                                        if (ch == '-' || ch == '+')
                                            i++;
                                    }
                                    ch = buf[start+i].ch ;
                            }
                            while (ch== 'L' || ch == 'l'
                                || ch == 'U' || ch == 'u'
                                || ch == 'F' || ch == 'f')
                                ch = buf[start + ++i].ch;
                        }
                    }
                    else
                    {
                        while (isxdigit(buf[start + i].ch))
                            i++;
                        if (buf[start + i].ch != 'H' && buf[start + i].ch != 'h')
                        {
                            i = j;
                            while (isdigit(buf[start + i].ch))
                                i++;
                        }
                        else
                            i++;
                    }
                    hexflg = FALSE;
                    Colorize(buf, start + j, i - j, (bkColor << 5) | C_NUMBER, FALSE);
                    i--;
                }
            }
            else if ((buf[start + i].ch == '"' 
                || buf[start + i].ch == '\'') && (start + i <2 || buf[start + i - 1].ch != '\\' || buf[start + i - 2].ch == '\\'))
            {
                int ch = buf[start + i].ch;
                int j = i++;
                while (buf[start + i].ch && ((buf[start + i].ch != ch && buf[start +
                    i].ch != '\n') || (buf[start + i - 1].ch == '\\' && buf[start + i -
                    2].ch != '\\')) && i < chars)
                    i++;
                Colorize(buf, start + j + 1, i - j - 1, (bkColor << 5) | C_STRING, FALSE);
            }
            else if (hashed && buf[start + i].ch == '<')
            {
                int j = i++;
                while (buf[start + i].ch && (buf[start + i].ch != '>' && buf[start +
                    i].ch != '\n'))
                    i++;
                Colorize(buf, start + j + 1, i - j - 1, (bkColor << 5) | C_STRING, FALSE);
            }
        }
    }

}

/**********************************************************************
 * FormatBuffer colorizes comments over a range of text, 
 * then calls SearchKeywords to colorize keywords
 **********************************************************************/
static int instring(INTERNAL_CHAR *buf, INTERNAL_CHAR *t1)
{
    INTERNAL_CHAR *t2 = t1;
    int quotechar = 0;
    while (t2 != buf && t2[ - 1].ch != '\n')
        t2--;
    while (t2 != t1)
    {
        if (quotechar)
        {
            if (t2->ch == quotechar && t2[ - 1].ch != '\\')
                quotechar = 0;
        }
        else
        {
            if (t2->ch == '\'' || t2->ch == '"')
                quotechar = t2->ch;
        }
        t2++;
    }
    return !!quotechar;
}

//-------------------------------------------------------------------------

static void FormatBuffer(COLORIZE_HASH_ENTRY *entries[], INTERNAL_CHAR *buf, int start, int end, int type, int bkColor)
{
    if (type != LANGUAGE_NONE && PropGetBool(NULL, "COLORIZE"))
    {
        if (type == LANGUAGE_C || type == LANGUAGE_RC)
        {
            INTERNAL_CHAR *t = buf + start;
            INTERNAL_CHAR *t1;
            while (TRUE)
            {
                t1 = strpstr(t, "/*", end - (t - buf));
    
                if (t1)
                {
                    if ((t1 == buf || t1[ - 1].ch != '/') && !instring(buf, t1))
                    {
    
                        t = strpstr(t1, "*/",  - 1);
                        if (!t)
                            t = t1 + strplen(t1);
                        else
                        {
                            t += 2;
                        }
                        Colorize(buf, t1 - buf, t - t1, (bkColor << 5) | C_COMMENT, TRUE);
                    }
                    else
                        t = t1 + 1;
                }
                else
                    break;
            }
            t = buf + start;
            t1 = strpstr(t, "//", end - (t - buf));
            while (t1)
            {
                if (!instring(buf, t1) && (t1->Color & 0xf) != C_COMMENT)
                {
    
                    t = strpstr(t1, "\n",  - 1);
                    while (1)
                    {
                        if (!t)
                        {
                            t = t1 + strplen(t1);
                            break;
                        }
                        else
                        {
                            INTERNAL_CHAR *t2 = t;
                            while (t2 > buf && isspace(t2[ - 1].ch))
                                t2--;
                            if (t2[ - 1].ch != '\\')
                                break;
                            t = strpstr(t + 1, "\n",  - 1);
                        }
                    }
                    Colorize(buf, t1 - buf, t - t1 + 1, (bkColor << 5) | C_COMMENT, TRUE);
                }
                else
                    t = t1 + 1;
                t1 = strpstr(t, "//", end - (t - buf));
            }
        }
        else if (type == LANGUAGE_ASM)
        {
            INTERNAL_CHAR *t = buf + start;
            int type;
            INTERNAL_CHAR *t1;
            t1 = strpstr(t, ";", end - (t - buf));
            while (t1)
            {
                t = strpstr(t1, "\n",  - 1);
                if (!t)
                {
                    t = t1 + strplen(t1);
                }
                Colorize(buf, t1 - buf, t - t1 + 1, (bkColor << 5) | C_COMMENT, TRUE);
                t1 = strpstr(t, ";", end - (t - buf));
            }
        }
        SearchKeywords(entries, buf, end - start, start, type, bkColor);
    }
    else
    {
        Colorize(buf, start, end, (bkColor << 5) | C_TEXT, FALSE);
    }
}

//-------------------------------------------------------------------------

static void FormatBufferFromScratch(COLORIZE_HASH_ENTRY *entries[], INTERNAL_CHAR *buf, int start, int end, int
    type, int bkColor)
{
    if (type != LANGUAGE_NONE && PropGetBool(NULL, "COLORIZE"))
    {
        int xend, xstart;
        xend = end;
        if (start < 0)
            start = 0;
        xstart = start;
        while (xstart && (buf[xstart - 1].ch != '\n' || (buf[xstart - 1].Color & 0xf) ==
            C_COMMENT))
            xstart--;
        while (buf[xend].ch && (buf[xend].ch != '\n' || (buf[xend].Color &0xf) ==
                    C_COMMENT))
                xend++;
    
        Colorize(buf, xstart, xend - xstart, (bkColor << 5) | C_TEXT, FALSE);
        FormatBuffer(entries, buf, xstart, xend, type, bkColor);
    }
}

/**********************************************************************
 * FormatLine is an optimized colorizer that just colorizes the current
 * line
 **********************************************************************/

static void FormatLine(HWND hwnd, INTERNAL_CHAR *buf, int type, int bkColor)
{
    if (PropGetBool(NULL, "COLORIZE") && type != LANGUAGE_NONE)
    {
    
        int start, end;
        EDITDATA *p = (EDITDATA*)GetWindowLong(hwnd, 0);
        SendMessage(hwnd, EM_GETSEL, (WPARAM) &start, (LPARAM) &end);
        FormatBufferFromScratch(p->colorizeEntries, buf, start, start, type, bkColor);
    }
}
static void UpdateSiblings(EDITDATA *p, int pos, int insert)
{
    EDLIST *list = p->cd->siblings;
    while (list)
    {
        if (list->data != p)
        {
            SendMessage(list->data->self, EM_UPDATESIBLING, insert, pos);
        }
        list = list->next ;
    }
}
/**********************************************************************
 * GetWordFromPos is a utility to find out what the word under the
 * cursor is, and various information about it
 **********************************************************************/
int GetWordFromPos(HWND hwnd, char *outputbuf, int charpos, int *linenum, int
    *startoffs, int *endoffs)
{
    int linepos;
    int linecharpos;
    int linecharindex;
    char buf[1000];
    CHARRANGE charrange;

    EDITDATA *p = (EDITDATA*)GetWindowLong(hwnd, 0);

    if (charpos ==  - 1)
    {
        SendMessage(hwnd, EM_EXGETSEL, (WPARAM)0, (LPARAM) &charrange);
        charpos = charrange.cpMin;
    }
    linepos = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, (LPARAM)charpos);
    linecharindex = SendMessage(hwnd, EM_LINEINDEX, linepos, 0);
    linecharpos = charpos - linecharindex;
    *(short*)buf = 1000;
    outputbuf[0] = 0;
    if (linecharpos >= SendMessage(hwnd, EM_GETLINE, linepos, (LPARAM)buf))
    {
        return FALSE;
    }

    /* if there is a selection, attempt to use it */
    if (p->selstartcharpos != p->selendcharpos)
    {
        int start = p->selstartcharpos;
        int end = p->selendcharpos;
        if (start > end)
        {
            int temp = start;
            start = end;
            end = temp;
        }
        if (end - start < 511)
        {
            int i = 0;
            while (start < end)
                outputbuf[i++] = p->cd->text[start++].ch;
            outputbuf[i] = 0;
            if (linenum)
                *linenum = linepos;
            if (startoffs)
                *startoffs = start;
            if (endoffs)
                *endoffs = end;
            return TRUE;
        }
    }
    
        
    // otherwise get word under cursor
    while (linecharpos && (buf[linecharpos] == ' ' || buf[linecharpos] == '\n' 
        || buf[linecharpos] == '\t' || buf[linecharpos] == 0))
        linecharpos--;


    {
        char *start = buf + linecharpos,  *end = start;
        if (start > buf)
        {
            start--;

            if (start == buf)
            {
                if (!keysym(*start))
                    start++;
            }
            else
            while (start > buf)
            {
                if (!keysym(*start))
                {
                    start++;
                    break;
                }
                start--;
            }
            if (!keysym(*start))
                start++;
        }
        while (*end && keysym(*end))
            end++;
        *end = 0;
        if (start > end)
            start = end;
        if (linenum)
            *linenum = linepos;
        if (startoffs)
            *startoffs = start - buf + linecharindex;
        if (endoffs)
            *endoffs = end - buf + linecharindex;
        strcpy(outputbuf, start);
    }
        return TRUE;
}

/**********************************************************************
 * DoHelp handles the F1 key functionality - it gets the word under the
 * cursor then tries a lookup from the favorite help engine.  Again this
 * is kind of linked to the rest of CCIDE
 **********************************************************************/

static void DoHelp(HWND edwin, int speced)
{
    char buf[512];
    if (!GetWordFromPos(edwin, buf,  - 1, 0, 0, 0))
        return ;
    else
    {
        if (GetKeyState(VK_CONTROL) &0x80000000)
            WebHelp(buf);
        else if (speced)
            SpecifiedHelp(buf);
        else
            RTLHelp(buf);
    }
}


/**********************************************************************
 * getundo creates an UNDO structure based on the user's last operation
 **********************************************************************/
UNDO *getundo(HWND hwnd, EDITDATA *p, int type)
{
    int x;
    UNDO *u;
    if (p->cd->undoing)
        return 0;
    if (p->cd->undohead != p->cd->undotail)
    {
        x = p->cd->redopos + 1;
        if (x >= UNDO_MAX)
            x = 0;
        if (x != p->cd->undotail)
        {
            x = p->cd->redopos;
            if (x == p->cd->undohead)
            {
                if (--x < 0)
                    x += UNDO_MAX;
            }
            u = &p->cd->undolist[x];
            switch(u->type)
            {
                case UNDO_INSERT:
                    u->len = u->undotemp;
                    break;
                case UNDO_MODIFY:
                case UNDO_BACKSPACE:
                case UNDO_DELETE:
                    memcpy(u->data, u->data + u->undotemp, u->len - u->undotemp);
                    u->len -=u->undotemp;
                    u->undotemp = 0;
                    break;
                default:
                    u->undotemp = 0;
                    break;
            }
            if (type != UNDO_DELETESELECTION && type != UNDO_INSERTSELECTION && type !=
                UNDO_CASECHANGE && type != UNDO_AUTOBEGIN && type != UNDO_AUTOEND &&
                type != UNDO_AUTOCHAINBEGIN && p->cd->undohead != p->cd->undotail)
            {
                if (u->type == type)
                {
                    if (type != UNDO_BACKSPACE)
                    {
                        if (p->selstartcharpos == u->postselstart)
                        {
                            p->cd->undohead = x;
                            if (++p->cd->undohead >= UNDO_MAX)
                                p->cd->undohead -= UNDO_MAX;
                            p->cd->redopos = p->cd->undohead;
                            p->cd->modified = TRUE;
                            return u;
                        }
                    }
                    else
                    {
                        if (p->selstartcharpos + 1 == u->postselstart)
                        {
                            p->cd->undohead = x;
                            if (++p->cd->undohead >= UNDO_MAX)
                                p->cd->undohead -= UNDO_MAX;
                            p->cd->redopos = p->cd->undohead;
                            p->cd->modified = TRUE;
                            return  u;
                        }
                    }
                }
            }
        }
    }

    if (p->cd->redopos != p->cd->undohead)
    {
        p->cd->redopos++;
        if (p->cd->redopos >= UNDO_MAX)
            p->cd->redopos -= UNDO_MAX;
    }
    p->cd->undohead = p->cd->redopos;
    u = &p->cd->undolist[p->cd->redopos];
    if (++p->cd->undohead >= UNDO_MAX)
        p->cd->undohead = 0;
    p->cd->redopos = p->cd->undohead;
    if (p->cd->undohead == p->cd->undotail)
        if (++p->cd->undotail >= UNDO_MAX)
            p->cd->undotail = 0;
    u->len = u->undotemp = 0;
    u->preselstart = p->selstartcharpos;
    u->preselend = p->selendcharpos;
    u->charpos = p->textshowncharpos;
    u->modified = p->cd->modified;
    p->cd->modified = TRUE;
    u->type = type;
    return u;
}

//-------------------------------------------------------------------------

void insertautoundo(HWND hwnd, EDITDATA *p, int type)
{
    UNDO *u = getundo(hwnd, p, type);
    if (u)
    {
        u->postselstart = p->selstartcharpos;
        u->postselend = p->selendcharpos;
    }
}

/**********************************************************************
 * undo_deletesel gets the undo structure for a CUT operation
 **********************************************************************/
UNDO *undo_deletesel(HWND hwnd, EDITDATA *p)
{
    UNDO *u;
    int start, end;
    int i = 0;
    if (p->selstartcharpos == p->selendcharpos)
        return 0;
    u = getundo(hwnd, p, UNDO_DELETESELECTION);
    if (!u)
        return u;
    if (p->selstartcharpos < p->selendcharpos)
    {
        start = p->selstartcharpos;
        end = p->selendcharpos;
    }
    else
    {
        start = p->selendcharpos;
        end = p->selstartcharpos;
    }
    if (end - start > u->max)
    {
        BYTE *temp = (BYTE *)realloc(u->data, end - start);
        if (!temp)
            return 0;
        u->data = temp;
        u->max = end - start;
    }
    u->len = end - start;
    while (start < end)
    {
        u->data[i++] = p->cd->text[start++].ch;
    }
    return u;
}

//-------------------------------------------------------------------------

UNDO *undo_casechange(HWND hwnd, EDITDATA *p)
{
    UNDO *x = undo_deletesel(hwnd, p);
    x->type = UNDO_CASECHANGE;
    return x;
}

/**********************************************************************
 * undo_insertsel gets the undo structure for an operation which pasts
 **********************************************************************/
UNDO *undo_insertsel(HWND hwnd, EDITDATA *p, char *s)
{
    UNDO *u = getundo(hwnd, p, UNDO_INSERTSELECTION);
    if (!u)
        return u;
    u->len = strlen(s);
    if (u->max < u->len)
    {
        u->data = realloc(u->data, u->len);
    }
    memcpy(u->data, s, u->len);
    return u;
}

/**********************************************************************
 * undo_deletechar gets the undo structure for a character deletion
 **********************************************************************/
UNDO *undo_deletechar(HWND hwnd, EDITDATA *p, int ch, int type)
{
    UNDO *u = getundo(hwnd, p, type);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        BYTE *temp = realloc(u->data, u->max + 64);
        if (!temp)
            return 0;
        u->data = temp;
        u->max += 64;
    }
    memmove(u->data + 1, u->data, u->len++);
    u->data[0] = ch;
    u->postselstart = p->selstartcharpos;
    u->postselend = p->selendcharpos;
    return u;
}

/**********************************************************************
 * undo_deletechar gets the undo structure for typing over a character
 **********************************************************************/
UNDO *undo_modifychar(HWND hwnd, EDITDATA *p)
{
    UNDO *u = getundo(hwnd, p, UNDO_MODIFY);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        BYTE *temp = realloc(u->data, u->max + 64);
        if (!temp)
            return 0;
        u->data = temp;
        u->max += 64;
    }
    memmove(u->data + 1, u->data, u->len++);
    u->data[0] = p->cd->text[p->selstartcharpos].ch;
    return u;
}

/**********************************************************************
 * undo_deletechar gets the undo structure for inserting a character
 **********************************************************************/
UNDO *undo_insertchar(HWND hwnd, EDITDATA *p, int ch)
{
    UNDO *u = getundo(hwnd, p, UNDO_INSERT);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        BYTE *temp = realloc(u->data, u->max + 64);
        if (!temp)
            return 0;
        u->data = temp;
        u->max += 64;
    }
    u->data[u->len++] = ch;
    u->undotemp = u->len;
    return u;
}

/**********************************************************************
 * ClientArea gets the client area.  We are leaving space at the bottom
 * because it would be overlayed with the scroll bar
 **********************************************************************/
void ClientArea(HWND hwnd, EDITDATA *p, RECT *r)
{
    GetClientRect(hwnd, r);
    //   r->bottom -= GetSystemMetrics(SM_CYHSCROLL) ;
    r->bottom -= r->bottom % p->cd->txtFontHeight;
}

/**********************************************************************
 * posfromchar determines the screen position of a given offset in
 * the buffer
 **********************************************************************/
int posfromchar(HWND hwnd, EDITDATA *p, POINTL *point, int pos)
{
    char buf[256],  *x = buf;
    RECT r;
    int spos = p->textshowncharpos, xcol;
    int i = 0;
    point->x = point->y = 0;
    if (spos > pos)
        return 0;
    while (spos + i < pos && spos + i < p->cd->textlen)
    {
        if (p->cd->text[spos + i].ch == '\n')
        {
            point->y += p->cd->txtFontHeight;
            spos += i + 1;
            i = 0;
        }
        else
            i++;
    }
    i = 0;
    xcol = 0;
    while (spos + i < pos && spos + i < p->cd->textlen)
    {
        if (p->cd->text[spos + i].ch == '\n')
                break;
        if (p->cd->text[spos + i].ch == '\t')
        {
            xcol += p->cd->tabs;
            xcol /= p->cd->tabs;
            xcol *= p->cd->tabs;
        }
        else
            xcol++;
        i++;
    }
    if (xcol >= p->leftshownindex)
        point->x = (xcol - p->leftshownindex) *p->cd->txtFontWidth;
    else
        return 0;
    ClientArea(hwnd, p, &r);
    if (point->x >= r.right || point->y >= r.bottom)
        return 0;
    return 1;
}

/**********************************************************************
 * charfrompos determines the buffer offset from the screen position
 **********************************************************************/
int charfrompos(HWND hwnd, EDITDATA *p, POINTL *point)
{
    RECT r;
    int row, col, xcol = 0;
    int pos = p->textshowncharpos, i = 0;
    char buf[256],  *x = buf;
    ClientArea(hwnd, p, &r);
    if (point->x > r.right || point->y > r.bottom)
        return 0;
    row = point->y / p->cd->txtFontHeight;
    col = point->x / p->cd->txtFontWidth;
    while (row && pos < p->cd->textlen)
    {
        if (p->cd->text[pos].ch == '\n')
            row--;
        pos++;
    }

    if (pos == p->cd->textlen)
        return pos;
    while (pos + i < p->cd->textlen && xcol < p->leftshownindex)
    {
        if (p->cd->text[pos + i].ch == '\n')
                return pos + i - 1;
        if (p->cd->text[pos + i].ch == '\t')
        {
            xcol += p->cd->tabs;
            xcol /= p->cd->tabs;
            xcol *= p->cd->tabs;
        }
        else
            xcol++;
        i++;
    }
    pos += i;
    i = 0;
    xcol = 0;
    while (xcol < col && pos + i < p->cd->textlen)
    {
        if (p->cd->text[pos + i].ch == '\n')
                break;
        if (p->cd->text[pos + i].ch == '\t')
        {
            xcol += p->cd->tabs;
            xcol /= p->cd->tabs;
            xcol *= p->cd->tabs;
        }
        else
            xcol++;
        i++;
    }
    return pos + i /*-1*/;
}

/**********************************************************************
 *  vscrolllen sets the limits for the vertical scroll bar
 **********************************************************************/
void VScrollLen(HWND hwnd, int count, int set)
{
    int count1 = count;
    int base = 0;
    if (!set)
    {
        GetScrollRange(hwnd, SB_VERT, &base, &count1);
        count1 += count;
    }
    SetScrollRange(hwnd, SB_VERT, base, count1, TRUE);
}

/**********************************************************************
 *  vscrolllen sets the position for the vertical scroll bar
 **********************************************************************/
void VScrollPos(HWND hwnd, int count, int set)
{
    int count1 = count;
    if (!set)
    {
        count1 = GetScrollPos(hwnd, SB_VERT);
        count1 += count;
    }
    SetScrollPos(hwnd, SB_VERT, count1, TRUE);
}

/**********************************************************************
 * curcol finds the screen column number corresponding to a text position
 * (zero based)
 **********************************************************************/
int curcol(EDITDATA *p, INTERNAL_CHAR *text, int pos)
{
    int rv = 0;
    int opos = pos;
        while (pos && text[pos - 1].ch != '\n')
        {
            pos--;
        }
    while (pos < opos)
    {
        if (text[pos].ch == '\t')
        {
            rv += p->cd->tabs;
            rv = (rv / p->cd->tabs) *p->cd->tabs;
        }
        else
            rv++;
        pos++;
    }
    return rv;
}

//-------------------------------------------------------------------------

void setcurcol(EDITDATA *p)
{
    int pos;
    if (p->selstartcharpos != p->selendcharpos)
        pos = p->selendcharpos;
    else
        pos = p->selstartcharpos;
    p->cd->updowncol = curcol(p, p->cd->text, pos);
}

/**********************************************************************
 *  MoveCaret moves the caret to the position of the selection.  IF 
 *  the caret is offscreen, it gets hidden
 **********************************************************************/

void MoveCaret(HWND hwnd, EDITDATA *p)
{
    int x = 0, y = 0;
    POINTL pt;
    if (posfromchar(hwnd, p, &pt, p->selstartcharpos != p->selendcharpos 
            ? p->selendcharpos: p->selstartcharpos))
    {
        if (p->hasfocus)
        {
            SetCaretPos(pt.x, pt.y);
            ShowCaret(hwnd);
        }
        p->hiddenCaret = FALSE;
    }
    else
    {
        if (!p->hiddenCaret && p->hasfocus)
            HideCaret(hwnd);
        p->hiddenCaret = TRUE;
    }
}

/**********************************************************************
 *  Scroll Left scrolls left or right, depending on the sign of 'cols'
 **********************************************************************/
void scrollleft(HWND hwnd, EDITDATA *p, int cols)
{
    p->leftshownindex += cols;
    if (p->leftshownindex < 0)
        p->leftshownindex = 0;
    SendUpdate(hwnd);
    InvalidateRect(hwnd, 0, 0);
}

/**********************************************************************
 *  Scrollup scrolls up or down, depending on the sign of 'lines'
 **********************************************************************/

void scrollup(HWND hwnd, EDITDATA *p, int lines)
{
    RECT r, update;
    int totallines, movelines = lines;
    int pos = p->textshowncharpos, len = 0;
    ClientArea(hwnd, p, &r);
    totallines = r.bottom / p->cd->txtFontHeight;
    if (lines < 0)
    {
        lines =  - lines;
        while (lines && pos > 0)
        {
            --pos;
            if (p->cd->text[pos].ch == '\n')
            {
                lines--;
                len--;
            }
        }
        while (pos)
        {
            --pos;
            if (p->cd->text[pos].ch == '\n')
            {
                pos++;
                break;
            }
        }
        SendUpdate(hwnd);
        if (lines >= totallines)
        {
            InvalidateRect(hwnd, 0, 0);
        }
        else
        {
            if ( - movelines - lines)
                ScrollWindowEx(hwnd, 0, ( - movelines - lines) *p
                    ->cd->txtFontHeight, &r, &r, 0, &update, SW_INVALIDATE);
        }
        p->textshowncharpos = pos;
    }
    else
    {
        while (lines && pos < p->cd->textlen)
        {
            if (p->cd->text[pos].ch == '\n')
            {
                lines--;
                len++;
            }
            pos++;
        }
        SendUpdate(hwnd);
        if (lines >= totallines)
        {
            InvalidateRect(hwnd, 0, 0);
        }
        else
        {
            if (movelines - lines)
                ScrollWindowEx(hwnd, 0,  - (movelines - lines) *p
                    ->cd->txtFontHeight, &r, &r, 0, &update, SW_INVALIDATE);
        }
        p->textshowncharpos = pos;
    }
    SendUpdate(hwnd);
    VScrollPos(hwnd, len, FALSE);
}

/**********************************************************************
 *  ScrollCaretIntoView moves the text in the window around in such a way
 *  that the caret is in the window.
 **********************************************************************/
void ScrollCaretIntoView(HWND hwnd, EDITDATA *p, BOOL middle)
{
    POINTL pt;
    int lines, cols, colpos = 0;
    RECT r;
    int pos1, pos;
    pos1 = p->selendcharpos;
    if (posfromchar(hwnd, p, &pt, pos1))
    {
        MoveCaret(hwnd, p);
        return ;
    }
    ClientArea(hwnd, p, &r);
    lines = r.bottom / p->cd->txtFontHeight;
    cols = r.right / p->cd->txtFontWidth;
    if (cols == 0)
        cols = 80;
    if (pos1 > p->textshowncharpos)
    {
        int xlines = 0;
        pos = p->textshowncharpos;
        while (pos < pos1 && pos < p->cd->textlen)
        {
            if (p->cd->text[pos].ch == '\n')
                xlines++;
            pos++;
        }
        if (xlines >= lines)
        {
            if (middle)
                scrollup(hwnd, p, xlines - lines/2);
            else
                scrollup(hwnd, p, xlines - lines + 1);
        }

    }
    else
    {
        int xlines = 0;
        if (pos1 != p->textshowncharpos)
        {
            pos = p->textshowncharpos;
            while (pos > 0 && pos != pos1)
            {
                --pos;
                if (p->cd->text[pos].ch == '\n')
                    xlines++;
            }
            if (xlines > 0)
            {
                if (middle)
                    scrollup(hwnd, p,  - xlines - lines / 2 );
                else
                    scrollup(hwnd, p,  - xlines );
            }
        }
    }
    pos = pos1;
    while (pos > 0 && p->cd->text[pos - 1].ch != '\n')
        pos--;
    while (pos != pos1)
    {
        if (pos && p->cd->text[pos - 1].ch == '\t')
            colpos = ((colpos + p->cd->tabs) / p->cd->tabs) *p->cd->tabs;
        else
            colpos++;
        pos++;
    }
    if (colpos < p->leftshownindex)
        scrollleft(hwnd, p, colpos - p->leftshownindex - 10);
    else if (colpos >= p->leftshownindex + cols)
        scrollleft(hwnd, p, colpos - p->leftshownindex - cols + 1);
    MoveCaret(hwnd, p);


}

/**********************************************************************
 * TrackVScroll handles tracking messages and updating the display when
 * the user moves the vertical scroll bar
 **********************************************************************/
void TrackVScroll(HWND hwnd, EDITDATA *p, int end)
{
    SCROLLINFO si;
    int count;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_TRACKPOS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    count = SendMessage(hwnd, EM_LINEINDEX, si.nTrackPos, 0);
    p->textshowncharpos = count;
    //   if (end)
    SetScrollPos(hwnd, SB_VERT, si.nTrackPos, 0);
    SendUpdate(hwnd);
    InvalidateRect(hwnd, 0, 0);
    MoveCaret(hwnd, p);

}

/**********************************************************************
 * TrackHScroll handles tracking messages and updating the display when
 * the user moves the horizontal scroll bar
 **********************************************************************/
void TrackHScroll(HWND hwnd, EDITDATA *p, int end)
{
    SCROLLINFO si;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_TRACKPOS;
    GetScrollInfo(hwnd, SB_HORZ, &si);
    p->leftshownindex = si.nTrackPos;
    if (end)
        SetScrollPos(hwnd, SB_HORZ, si.nTrackPos, 0);
    SendUpdate(hwnd);

}

/**********************************************************************
 * lfchars counts the number of times we switch from one line to another
 * within a range of chars
 **********************************************************************/
int lfchars(INTERNAL_CHAR *c, int start, int end)
{
    int rv = 0;
    while (start < end)
    {
        if (c[start].ch == '\n')
            rv++;
        start++;
    }
    return rv;
}

/**********************************************************************
 * Line from char takes a character pos and turns it into a line number
 **********************************************************************/
int LineFromChar(EDITDATA *p, int pos)
{
    int rv = 0;
    INTERNAL_CHAR *ic = p->cd->text;
    while (ic < p->cd->text + p->cd->textlen && pos)
    {
        if (ic->ch == '\n')
            rv++;
        pos--;
        ic++;
    }
    return rv;
}

/**********************************************************************
 * SelLine is the Same as lineFromChar, but counts a partial (unterminated) 
 * line at the end of the buffer
 **********************************************************************/
static int SelLine(EDITDATA *p, int pos)
{
    int rv = LineFromChar(p, pos);
    if (pos && p->cd->text[pos - 1].ch != '\n')
            rv++;
    return rv;
}

/**********************************************************************
 * Replace handles pasting.  Will also cut a previous selection, if there
 * was one
 **********************************************************************/
void Replace(HWND hwnd, EDITDATA *p, char *s, int lens)
{
    UNDO *u = 0;
    int i, temp, changed;
    char  *s1;
    int len = 0;

    i = p->selendcharpos - p->selstartcharpos;
    changed = lens - i;
    u = undo_deletesel(hwnd, p);
    if (i > 0)
    {
        len -= lfchars(p->cd->text, p->selstartcharpos, p->selendcharpos);
        SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p
            ->selstartcharpos), len);
        memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p->selendcharpos, (p
            ->cd->textlen - p->selendcharpos + 1) *sizeof(INTERNAL_CHAR));
        p->selendcharpos = p->selstartcharpos;
        p->cd->textlen -= i;
        UpdateSiblings(p, p->selstartcharpos, -i);
    }
    else if (i < 0)
    {
        temp = lfchars(p->cd->text, p->selendcharpos, p->selstartcharpos);
        SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selendcharpos)
            ,  - temp);
        VScrollPos(hwnd,  - temp, FALSE);
        len -= temp;
        memcpy(p->cd->text + p->selendcharpos, p->cd->text + p->selstartcharpos, (p
            ->cd->textlen - p->selstartcharpos + 1) *sizeof(INTERNAL_CHAR));
        p->selstartcharpos = p->selendcharpos;
        p->cd->textlen += i;
        UpdateSiblings(p, p->selstartcharpos, i);
    }
    if (u)
    {
        u->postselstart = u->postselend = p->selstartcharpos;
    }
    if (!commitmem(p,p->cd->textlen + lens))
    {
        p->selendcharpos = p->selstartcharpos;
        SendUpdate(hwnd);
        p->cd->sendchangeonpaint = TRUE;
        InvalidateRect(hwnd, 0, 0);
        return ;
    }
    if (lens)
    {
        u = undo_insertsel(hwnd, p, s);
        if (u)
        {
            u->preselstart = u->preselend = p->selstartcharpos;
            u->postselstart = u->preselstart;
            u->postselend = u->preselstart + lens;
        }
    }
    temp = 0;
    s1 = s;
    i = 0;
    while (*s1 && i++ < lens)
        if (*s1++ == '\n')
            temp++;
    SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selstartcharpos),
        temp);
    memmove(p->selstartcharpos + lens + p->cd->text, p->cd->text + p->selstartcharpos, 
        (p->cd->textlen - p->selstartcharpos + 1) *sizeof(INTERNAL_CHAR));
    memset(p->selstartcharpos + p->cd->text, 0, (lens) *sizeof(INTERNAL_CHAR));
    UpdateSiblings(p, p->selstartcharpos, lens);
    i = 0;
    while (*s && i < lens)
    {
        if (*s == '\n')
            len++;
        p->cd->text[p->selstartcharpos + i].ch =  *s++;
        p->cd->text[p->selstartcharpos + i].Color = (p->cd->defbackground << 5) + p->cd->defforeground;
        i++;
        p->cd->textlen++;
    }
    VScrollLen(hwnd, len, FALSE);
    FormatBufferFromScratch(p->colorizeEntries, p->cd->text, p->selstartcharpos - 1, p
        ->selstartcharpos + lens + 1, p->cd->language, p->cd->defbackground);
    SendUpdate(hwnd);
    p->cd->sendchangeonpaint = TRUE;
    if (p->selstartcharpos < p->textshowncharpos)
    {
        p->textshowncharpos += changed;
        if (p->textshowncharpos < 0)
            p->textshowncharpos = 0;         
        while (p->textshowncharpos && p->cd->text[p->textshowncharpos-1].ch != '\n')
            p->textshowncharpos--;
    }
    InvalidateRect(hwnd, 0, 0);
    reparse(hwnd, p);
}

/**********************************************************************
 * GetLineOffset returns the line number, this time as an offset from
 * the first line shown at the top of the window
 **********************************************************************/
int GetLineOffset(HWND hwnd, EDITDATA *p, int chpos)
{
    int pos = p->textshowncharpos;
    int line = 0;
    while (pos != chpos && pos < p->cd->textlen)
    {
        if (p->cd->text[pos].ch == '\n')
            line++;
        pos++;
    }
    return line;
}

/**********************************************************************
 * drawline draws the current line and everything below it(by invalidating
 * the selection)
 **********************************************************************/
void drawline(HWND hwnd, EDITDATA *p, int chpos)
{
    RECT r;
    int pos;
    pos = p->selendcharpos;
    ClientArea(hwnd, p, &r);
    r.top = GetLineOffset(hwnd, p, pos) *p->cd->txtFontHeight;
    SendUpdate(hwnd);
    InvalidateRect(hwnd, &r, 1);
    MoveCaret(hwnd, p);
}

/**********************************************************************
 * insertchar handles the functionality of inserting a character
 * will also cut a previous selection
 **********************************************************************/
void insertchar(HWND hwnd, EDITDATA *p, int ch)
{
    int len = 0, temp;
    UNDO *u = 0;
    if (p->cd->inserting || p->cd->text[p->selstartcharpos].ch == '\n')
    {
        int i = p->selendcharpos - p->selstartcharpos;
        u = undo_deletesel(hwnd, p);
        if (i > 0)
        {
            len -= lfchars(p->cd->text, p->selstartcharpos, p->selendcharpos);
            SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p
                ->selstartcharpos), len);
            memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p->selendcharpos, (p
                ->cd->textlen - p->selendcharpos + 1) *sizeof(INTERNAL_CHAR));
            p->cd->textlen -= i;
            UpdateSiblings(p, p->selstartcharpos, -i);
        }
        else if (i < 0)
        {
            temp = lfchars(p->cd->text, p->selendcharpos, p->selstartcharpos);
            SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p
                ->selendcharpos),  - temp);
            VScrollPos(hwnd,  - temp, FALSE);
            len -= temp;
            memcpy(p->cd->text + p->selendcharpos, p->cd->text + p->selstartcharpos, (p
                ->cd->textlen - p->selstartcharpos + 1) *sizeof(INTERNAL_CHAR));
            p->selstartcharpos = p->selendcharpos;
            p->cd->textlen += i;
            UpdateSiblings(p, p->selstartcharpos, i);
        }
        p->selendcharpos = p->selstartcharpos;
        if (u)
        {
            u->postselstart = u->postselend = p->selstartcharpos;
        }
        if (ch == '\n')
        {
            len++;
            SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p
                ->selstartcharpos), 1);
        }
        if (!commitmem(p,p->cd->textlen + 2))
        {
            SendUpdate(hwnd);
            p->cd->sendchangeonpaint = TRUE;
            InvalidateRect(hwnd, 0, 0);
            return;
        }
        u = undo_insertchar(hwnd, p, ch);
        memmove(p->cd->text + p->selstartcharpos + 1, p->cd->text + p->selstartcharpos,
            (p->cd->textlen - p->selstartcharpos + 1) *sizeof(INTERNAL_CHAR));
        p->cd->text[p->selstartcharpos].ch = ch;
        if (p->cd->text[p->selstartcharpos].Color == 0)
            p->cd->text[p->selstartcharpos].Color = (p->cd->defbackground << 5) + C_TEXT;
        p->cd->textlen++;
        UpdateSiblings(p, p->selstartcharpos, 1);		
        VScrollLen(hwnd, len, FALSE);
    }
    else
    {
        p->selendcharpos = p->selstartcharpos;
        if (p->cd->text[p->selstartcharpos].ch == '\n')
                p->selendcharpos = p->selstartcharpos++;
        u = undo_modifychar(hwnd, p);
        p->cd->text[p->selstartcharpos].ch = ch;
    }
    p->selendcharpos = ++p->selstartcharpos;
    if (u)
    {
        u->postselstart = p->selstartcharpos;
        u->postselend = p->selendcharpos;
    }
    p->cd->sendchangeonpaint = TRUE;
    ScrollCaretIntoView(hwnd, p, FALSE);
    reparse(hwnd, p);
}

/**********************************************************************
 * insertcr inserts a cr/lf pair
 **********************************************************************/
void insertcrtabs(HWND hwnd, EDITDATA *p);
void insertcr(HWND hwnd, EDITDATA *p, BOOL tabs)
{
    int temp;
    drawline(hwnd, p, p->selstartcharpos);
    if (p->selstartcharpos > p->selendcharpos)
    {
        temp =  - p->selstartcharpos;
        p->selstartcharpos = p->selendcharpos;
    }
    else
    {
        temp = p->selendcharpos;
        p->selendcharpos = p->selstartcharpos;
    }
    if (PropGetBool(NULL, "AUTO_INDENT"))
    {
        insertautoundo(hwnd, p, UNDO_AUTOEND);
        insertchar(hwnd, p, '\n');
        if (temp < 0)
        {
            p->selstartcharpos =  - temp + 1;
        }
        else
        {
            p->selendcharpos = temp + 1;
        }
        insertcrtabs(hwnd, p);
        insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
    }
    else
    {
        insertchar(hwnd, p, '\n');
        if (temp < 0)
        {
            p->selstartcharpos =  - temp + 1;
        }
        else
        {
            p->selendcharpos = temp + 1;
        }
    }
    //   ClientArea(hwnd,p,&r) ;
    //   r.top = GetLineOffset(hwnd,p,p->selstartcharpos)*p->cd->txtFontHeight ;
    //   ScrollWindowEx(hwnd,0,1*p->cd->txtFontHeight,&r,0,0,&update,SW_ERASE) ;
    VScrollLen(hwnd, 1, FALSE);
    VScrollPos(hwnd, 1, FALSE);
}

/**********************************************************************
 * inserttab inserts a tab, or types in a bunch of spaces to take
 * us to the next tab position
 **********************************************************************/
void inserttab(HWND hwnd, EDITDATA *p)
{
    if (!PropGetBool(NULL, "TABS_AS_SPACES"))
        insertchar(hwnd, p, '\t');
    else
    {
        int pos = p->selstartcharpos, col = 0, col2;
        while (pos && --pos)
        {
            if (p->cd->text[pos].ch == '\n')
            {
                pos++;
                break;
            }
        }
        while (pos != p->selstartcharpos)
        {
            if (p->cd->text[pos].ch == '\t')
            {
                col = col + p->cd->tabs;
                col /= p->cd->tabs;
                col *= p->cd->tabs;
            }
            else
                col++;
            pos++;
        }
        col2 = col + p->cd->tabs;
        col2 /= p->cd->tabs;
        col2 *= p->cd->tabs;
        while (col < col2)
        {
            insertchar(hwnd, p, ' ');
            col++;
        }
    }
    reparse(hwnd, p);
}

int firstword(INTERNAL_CHAR *pos, char *name)
{
    int l = strlen(name);
    int i;
    for (i=0; i < l; i++)
        if (!pos[i].ch || pos[i].ch != name[i])
            return FALSE ;
    if (isalnum(pos[i].ch) || pos[i].ch == '_')
        return FALSE;
    return TRUE ;
}
/**********************************************************************
 * tab to the current line position
 **********************************************************************/
void insertcrtabs(HWND hwnd, EDITDATA *p)
{
    int pos, n;
    int oldinsert = p->cd->inserting;
    int storepos, parencount = 0;
    if (!p->cd->language)
        return ;
    if (!PropGetBool(NULL, "AUTO_INDENT"))
        return ;
    p->cd->inserting = TRUE;
    pos = p->selstartcharpos - 1;
    while (1)
    {
        int pos2 = pos ;
        while (pos && p->cd->text[pos - 1].ch != '\n')
            pos--;
        while (p->cd->text[pos].ch && isspace(p->cd->text[pos].ch) && p->cd->text[pos].ch 
            != '\n')
            pos++;
        if (p->cd->text[pos].ch != '#')
        {
            int quotechar = 0;
            while (pos2-- > pos &&
                   p->cd->text[pos2].ch && p->cd->text[pos2].ch != '\n')
            {
                if ((p->cd->text[pos2].Color & 0xf) != C_COMMENT)
                {
                    if (quotechar == p->cd->text[pos2].ch && (!pos2 || p->cd->text[pos2-1].ch != '\\' 
                                                           || pos2 < 2 || p->cd->text[pos2-2].ch == '\\'))
                    {
                        quotechar = 0;
                    }
                    else if (!quotechar)
                    {
                        if ((p->cd->text[pos2].ch == '\'' || p->cd->text[pos2].ch == '"')
                            && (!pos2 || p->cd->text[pos2-1].ch != '\\'
                                || pos2 < 2 || p->cd->text[pos2-2].ch == '\\'))
                        {
                            quotechar = p->cd->text[pos2].ch;
                        }
                        else if (p->cd->text[pos2].ch == '(')
                        {
                            storepos = pos2 ;
                            if (++parencount > 0)
                                break;
                        }
                        else
                            if (p->cd->text[pos2].ch == ')')
                                parencount--;
                    } 
                }
            }
            if (parencount > 0)
            {
                pos = storepos + 1 ;
                break ;
            }
            else if (parencount == 0)
                break;
        }
        while (pos && p->cd->text[pos - 1].ch != '\n')
            pos--;
        if (!pos)
            break;
        pos--;
    }
//    insertautoundo(hwnd, p, UNDO_AUTOCHAINEND);
    n = curcol(p, p->cd->text, pos);
    if (firstword(p->cd->text +pos, "if") || firstword(p->cd->text + pos, "else")
        ||firstword(p->cd->text + pos, "do") || firstword(p->cd->text + pos, "for")
        ||firstword(p->cd->text + pos, "while")||firstword(p->cd->text + pos, "case"))
    {
        pos = p->selstartcharpos-1;
        while (pos && isspace(p->cd->text[pos].ch))
        {
            pos--;
        }
        if (p->cd->text[pos].ch != ';')
            n+= p->cd->tabs;
    }
    else
    {
        pos = p->selstartcharpos-1;
        while (pos && isspace(p->cd->text[pos].ch))
        {
            pos--;
        }
        if (p->cd->text[pos].ch == '{')
        {
            while (pos && p->cd->text[pos-1].ch != '\n')
                pos--;
            while (isspace(p->cd->text[pos].ch))
                pos++;
            n = curcol(p, p->cd->text, pos);
            n += p->cd->tabs;
            n = (n / p->cd->tabs) * p->cd->tabs;
        }
    }
    while (n >= p->cd->tabs)
    {
        inserttab(hwnd, p);
        n -= p->cd->tabs;
    }
    while (n--)
        insertchar(hwnd, p, ' ');
    p->cd->inserting = oldinsert;
//    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
}

//-------------------------------------------------------------------------

int spacedend(EDITDATA *p, int pos)
{
    int rv = 0;
    while (pos && p->cd->text[pos - 1].ch != '\n')
        if (!isspace(p->cd->text[--pos].ch))
            return 0;
    rv = pos;
    while (p->cd->text[pos].ch && isspace(p->cd->text[pos].ch) && p->cd->text[pos].ch !=
        '\n')
        pos++;
    if (p->cd->text[pos].ch == '}')
        return rv;
    else
        return 0;
}

//-------------------------------------------------------------------------

int preprocline(EDITDATA *p, int pos)
{
    int rv;
    while (pos && p->cd->text[pos - 1].ch != '\n')
        pos--;
    rv = pos;
    while (isspace(p->cd->text[pos].ch))
        pos++;
    if (p->cd->text[pos].ch == '#')
        return rv;
    else
        return  - 1;
}

/**********************************************************************
 * tab to the current line position
 **********************************************************************/
void InsertBeginTabs(HWND hwnd, EDITDATA *p)
{
    int pos, n;
    int storepos, parencount = 0;
    int solpos, eospos;
    int oldinsert = p->cd->inserting;
    if (p->cd->language != LANGUAGE_C && p->cd->language != LANGUAGE_RC)
        return ;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return ;
    p->cd->inserting = TRUE;
    pos = p->selstartcharpos - 1;
    solpos = pos;
    while (solpos && p->cd->text[solpos - 1].ch != '\n')
    {
        if (!isspace(p->cd->text[solpos - 1].ch))
            return ;
        solpos--;
    }
    if (solpos)
        pos = solpos-1 ;
    eospos = solpos;
    while (eospos < p->cd->textlen && isspace(p->cd->text[eospos].ch) &&
               p->cd->text[eospos].ch != '\n')
        eospos++;
    while (1)
    {
        int pos2 = pos ;
        while (pos && p->cd->text[pos - 1].ch != '\n')
            pos--;
        while (p->cd->text[pos].ch && isspace(p->cd->text[pos].ch) && p->cd->text[pos].ch 
            != '\n')
            pos++;
        if (p->cd->text[pos].ch != '#')
        {
            while (pos2-- > pos &&
                   p->cd->text[pos2].ch && p->cd->text[pos2].ch != '\n')
            {
                if (p->cd->text[pos2].ch == '(')
                {
                    storepos = pos2 ;
                    if (++parencount == 0)
                        break;
                }
                else
                    if (p->cd->text[pos2].ch == ')')
                        if (--parencount == 0)
                            break;
            } 
            if (parencount >= 0)
                break;
        }
        while (pos && p->cd->text[pos - 1].ch != '\n')
            pos--;
        if (!pos)
            break;
        pos--;
    }
    while (pos && p->cd->text[pos-1].ch != '\n')
        pos--;
    while (isspace(p->cd->text[pos].ch) && p->cd->text[pos].ch && pos < p->cd->textlen)
        pos++;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINEND);
    n = curcol(p, p->cd->text, pos);
    p->selstartcharpos = solpos;
    p->selendcharpos = eospos;
    Replace(hwnd, p, "", 0);
    while (n >= p->cd->tabs)
    {
        inserttab(hwnd, p);
        n -= p->cd->tabs;
    }
    while (n--)
        insertchar(hwnd, p, ' ');
    p->selstartcharpos = ++p->selendcharpos; // skip past '}'
    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    p->cd->inserting = oldinsert;
}
/**********************************************************************
 * tab to the current line position
 **********************************************************************/
void InsertEndTabs(HWND hwnd, EDITDATA *p, int newend)
{
    int pos, n;
    int eospos;
    int lsolpos, leospos;
    int oldinsert = p->cd->inserting;
    if (p->cd->language != LANGUAGE_C && p->cd->language != LANGUAGE_RC)
        return ;
    if (!newend)
        return ;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return ;
    p->cd->inserting = TRUE;
    leospos = pos = p->selstartcharpos - 1;
    while (isspace(p->cd->text[leospos].ch) && p->cd->text[leospos].ch != '\n')
        leospos++;

    if ((lsolpos = spacedend(p, pos)))
    {
        int indentlevel = 0;
        eospos = 0;
        pos--;
        while (pos > 0)
        {
            int pos1 = preprocline(p, pos);
            if (pos1 !=  - 1)
                pos = pos1;
            else if ((p->cd->text[pos].Color & 0xf) != C_COMMENT)
            {
                if (p->cd->text[pos].ch == '{')
                {
                    if (!indentlevel)
                    {
                        while (pos && p->cd->text[pos - 1].ch != '\n')
                                pos--;
                        while (isspace(p->cd->text[pos].ch))
                            pos++;
                        eospos = pos;
                        break;
                    }
                    else
                        indentlevel--;
                }
                else if (p->cd->text[pos].ch == '}')
                    indentlevel++;
            }
            pos--;
        }
        insertautoundo(hwnd, p, UNDO_AUTOCHAINEND);
        n = curcol(p, p->cd->text, eospos);
        p->selstartcharpos = lsolpos;
        p->selendcharpos = leospos;
        Replace(hwnd, p, "", 0);
        while (n >= p->cd->tabs)
        {
            inserttab(hwnd, p);
            n -= p->cd->tabs;
        }
        while (n--)
            insertchar(hwnd, p, ' ');
        p->selstartcharpos = ++p->selendcharpos; // skip past '}'
        insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    }
    p->cd->inserting = oldinsert;
}

//-------------------------------------------------------------------------

void SelectIndent(HWND hwnd, EDITDATA *p, int insert)
{
    int olds = p->selstartcharpos;
    int olde = p->selendcharpos;
    int start = p->selstartcharpos;
    int end = p->selendcharpos;
    int oldinsert = p->cd->inserting;
    int oldselect = p->cd->selecting;
    int oldte = p->cd->textlen;
    int decd = FALSE;
    int inverted = FALSE ;
    p->cd->inserting = TRUE;
    if (start == end)
    {
        int x;
        int adjustPos = start;
        int uVal = p->cd->undohead;
        while (start && p->cd->text[start-1].ch != '\n')
            start--;
        while (p->cd->text[end].ch && p->cd->text[end].ch != '\n')
            end ++;
        x = p->textshowncharpos;
        if (insert)
        {
            insertautoundo(hwnd, p, UNDO_AUTOEND);
            p->selstartcharpos = p->selendcharpos = start;
            inserttab(hwnd, p);
            p->cd->undolist[uVal].noChangeSel = TRUE;
            insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
            end += p->cd->textlen - oldte;
            p->selstartcharpos = adjustPos;
            if (adjustPos > start)
                p->selstartcharpos += p->cd->textlen - oldte;
            p->selendcharpos = p->selstartcharpos;
            
            if (start < x)
                p->textshowncharpos = x + p->cd->textlen - oldte;
        }
        else
        {
            if (isspace(p->cd->text[start].ch) && p->cd->text[start].ch != '\n')
            {
                int count = 0;
                if (p->cd->text[start + count].ch == ' ')
                    while (count < p->cd->tabs)
                        if (p->cd->text[start + count].ch != ' ')
                            break;
                        else
                            count++;
                else
                    count++;
                insertautoundo(hwnd, p, UNDO_AUTOEND);
                p->selstartcharpos = start;
                p->selendcharpos = start + count;
                Replace(hwnd, p, "", 0);
                insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
                end -= count;
                p->selstartcharpos = adjustPos;
                if (adjustPos > start)
                    p->selstartcharpos += p->cd->textlen - oldte;
                p->selendcharpos = p->selstartcharpos;
                if (start < p->textshowncharpos)
                    p->textshowncharpos = x - count;
                p->cd->undolist[uVal].noChangeSel = TRUE;
            }
        }
    }
    else
    {
        if (end < start)
        {
            inverted = TRUE ;
            start = p->selendcharpos;
            end = p->selstartcharpos;
        }
        if (end && p->cd->text[end - 1].ch == '\n')
            end--, decd++;
        while (start && p->cd->text[start - 1].ch != '\n')
            start--;
        while (p->cd->text[end].ch && p->cd->text[end].ch != '\n')
            end++;
        olds = start;
        insertautoundo(hwnd, p, UNDO_AUTOEND);
        while (start < end)
        {
            int x = p->textshowncharpos;
            if (insert)
            {
                p->selstartcharpos = p->selendcharpos = start;
                inserttab(hwnd, p);
                end += p->cd->textlen - oldte;
                if (start < x)
                    p->textshowncharpos = x + p->cd->textlen - oldte;
            }
            else
            {
                if (isspace(p->cd->text[start].ch) && p->cd->text[start].ch != '\n')
                {
                    int count = 0;
                    if (p->cd->text[start + count].ch == ' ')
                        while (count < p->cd->tabs)
                            if (p->cd->text[start + count].ch != ' ')
                                break;
                            else
                                count++;
                    else
                        count++;
                    p->selstartcharpos = start;
                    p->selendcharpos = start + count;
                    Replace(hwnd, p, "", 0);
                    end -= count;
                    if (start < p->textshowncharpos)
                        p->textshowncharpos = x - count;
                }
            }
            oldte = p->cd->textlen;
            while (p->cd->text[start].ch && p->cd->text[start].ch != '\n')
                start++;
            if (p->cd->text[start].ch)
                start++;
        }
        if (inverted)
        {
            p->selstartcharpos = end + decd;
            p->selendcharpos = olds;
        }
        else
        {
            p->selendcharpos = end + decd;
            p->selstartcharpos = olds;
        }
        insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
    }
    p->cd->inserting = oldinsert;
    p->cd->selecting = TRUE;
    MoveCaret(hwnd, p);
    p->cd->selecting = oldselect;
    InvalidateRect(hwnd, 0, 0);
}
void SelectComment(HWND hwnd, EDITDATA *p, int insert)
{
    int olds = p->selstartcharpos;
    int olde = p->selendcharpos;
    int start = p->selstartcharpos;
    int end = p->selendcharpos;
    int oldinsert = p->cd->inserting;
    int oldselect = p->cd->selecting;
    int decd = FALSE;
    int inverted = FALSE ;
    int column = 10000;
    p->cd->inserting = TRUE;
    insertautoundo(hwnd, p, UNDO_AUTOEND);
    if (start == end)
    {
        int adjustPos = start;
           int x = p->textshowncharpos;
        while (start && p->cd->text[start - 1].ch != '\n')
            start--;
        while (p->cd->text[end].ch && p->cd->text[end].ch != '\n')
            end++;
        while (start && isspace(p->cd->text[start].ch) && p->cd->text[start].ch != '\n' )
            start++;
        p->selstartcharpos = p->selendcharpos = start;
        if (insert)
        {
            if (p->cd->language == LANGUAGE_ASM)
            {
                Replace(hwnd, p, ";", 1);
                end += 1;
                if (start < x)
                    p->textshowncharpos = x + 1;
                p->selstartcharpos = adjustPos;
                if (adjustPos > start)
                    p->selstartcharpos ++;
                p->selendcharpos = p->selstartcharpos;
            }
            else
            {
                Replace(hwnd, p, "//", 2);
                end += 2;
                if (start < x)
                    p->textshowncharpos = x + 2;
                p->selstartcharpos = adjustPos;
                if (adjustPos > start)
                    p->selstartcharpos += 2;
                p->selendcharpos = p->selstartcharpos;
            }
        }
        else
        {
            if (p->cd->language == LANGUAGE_ASM)
            {
                if (p->cd->text[start].ch == ';')
                {
                    p->selendcharpos += 1;
                    Replace(hwnd, p, "", 0);
                    end -= 1;
                    if (start < x)
                        p->textshowncharpos = x - 1;
                    p->selstartcharpos = adjustPos;
                    if (adjustPos > start)
                        p->selstartcharpos --;
                    p->selendcharpos = p->selstartcharpos;
                }
            }
            else if (p->cd->text[start].ch == '/' && p->cd->text[start + 1].ch == '/')
            {
                p->selendcharpos += 2;
                Replace(hwnd, p, "", 0);
                end -= 2;
                if (start < x)
                    p->textshowncharpos = x - 2;
                p->selstartcharpos = adjustPos;
                if (adjustPos > start)
                    p->selstartcharpos -= 2;
                p->selendcharpos = p->selstartcharpos;
            }
        }
    }
    else
    {
        if (end < start)
        {
            inverted = TRUE ;
            start = p->selendcharpos;
            end = p->selstartcharpos;
        }
        if (end && p->cd->text[end - 1].ch == '\n')
            end--, decd++;
        while (start && p->cd->text[start - 1].ch != '\n')
            start--;
        while (p->cd->text[end].ch && p->cd->text[end].ch != '\n')
            end++;
        olds = start;
        while (start < end)
        {
            int n = 0;
            while (isspace(p->cd->text[start].ch) && p->cd->text[start].ch != '\n')
            {
                if (p->cd->text[start].ch == '\t')
                    n += p->cd->tabs;
                else
                    n++;
                start++;
            }
            if (n < column)
                column = n;
            while (p->cd->text[start].ch && p->cd->text[start].ch != '\n')
                start++;
            if (p->cd->text[start].ch)
                start++;
        }
        start = olds;
        while (start < end)
        {
            int x = p->textshowncharpos;
            int n = 0;
            while (isspace(p->cd->text[start].ch) && p->cd->text[start].ch != '\n')
            {
                if (p->cd->text[start].ch == '\t')
                    n += p->cd->tabs;
                else
                    n++;
                start++;
                if (n >= column)
                    break;
            }
            p->selstartcharpos = p->selendcharpos = start;
            if (insert)
            {
                if (p->cd->language == LANGUAGE_ASM)
                {
                    Replace(hwnd, p, ";", 1);
                    end += 1;
                    if (start < x)
                        p->textshowncharpos = x + 1;
                }
                else
                {
                    Replace(hwnd, p, "//", 2);
                    end += 2;
                    if (start < x)
                        p->textshowncharpos = x + 2;
                }
            }
            else
            {
                if (p->cd->language == LANGUAGE_ASM)
                {
                    if (p->cd->text[start].ch == ';')
                    {
                        p->selendcharpos += 1;
                        Replace(hwnd, p, "", 0);
                        end -= 1;
                        if (start < x)
                            p->textshowncharpos = x - 1;
                    }
                }
                else if (p->cd->text[start].ch == '/' && p->cd->text[start + 1].ch == '/')
                {
                    p->selendcharpos += 2;
                    Replace(hwnd, p, "", 0);
                    end -= 2;
                    if (start < x)
                        p->textshowncharpos = x - 2;
                }
            }
            while (p->cd->text[start].ch && p->cd->text[start].ch != '\n')
                start++;
            if (p->cd->text[start].ch)
                start++;
        }
        if (inverted)
        {
            p->selstartcharpos = end + decd;
            p->selendcharpos = olds;
        }
        else
        {
            p->selendcharpos = end + decd;
            p->selstartcharpos = olds;
        }
    }
    insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
    p->cd->inserting = oldinsert;
    p->cd->selecting = TRUE;
    MoveCaret(hwnd, p);
    p->cd->selecting = oldselect;
    FormatBuffer(p->colorizeEntries, p->cd->text, olds, end + decd, p->cd->language, p->cd->defbackground);
    InvalidateRect(hwnd, 0, 0);
}
//-------------------------------------------------------------------------

void DeletePound(HWND hwnd, EDITDATA *p)
{
    int n, m;
    if (p->cd->language != LANGUAGE_C && p->cd->language != LANGUAGE_RC)
        return ;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return ;
    if (p->selstartcharpos && p->cd->text[p->selstartcharpos - 1].ch != '#')
        return ;
    n = p->selstartcharpos - 1;
    while (n && p->cd->text[n - 1].ch != '\n')
        n--;
    m = n;
    while (isspace(p->cd->text[m].ch))
        m++;
    if (p->cd->text[m].ch != '#' || m == n)
        return ;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINEND);
    p->selstartcharpos = n;
    p->selendcharpos = m;
    Replace(hwnd, p, "", 0);
    p->selstartcharpos = p->selendcharpos = n + 1;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    ScrollCaretIntoView(hwnd, p, FALSE);
}

//-------------------------------------------------------------------------

void DeletePercent(HWND hwnd, EDITDATA *p)
{
    int n, m;
    if (p->cd->language != LANGUAGE_ASM)
        return ;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return ;
    if (p->selstartcharpos && p->cd->text[p->selstartcharpos - 1].ch != '%')
        return ;
    n = p->selstartcharpos - 1;
    while (n && p->cd->text[n - 1].ch != '\n')
        n--;
    m = n;
    while (isspace(p->cd->text[m].ch))
        m++;
    if (p->cd->text[m].ch != '%' || m == n)
        return ;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINEND);
    p->selstartcharpos = n;
    p->selendcharpos = m;
    Replace(hwnd, p, "", 0);
    p->selstartcharpos = p->selendcharpos = n + 1;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    ScrollCaretIntoView(hwnd, p, FALSE);
}

void CancelParenMatch(HWND hwnd, EDITDATA *p)
{
    if (p->matchingEnd != 0)
    {
        p->matchingEnd = p->matchingStart = 0;
        InvalidateRect(hwnd,NULL,0);
    }
}
int FindParenMatchBackward(HWND hwnd, EDITDATA *p, int dec)
{
    int skip,match;
    int level = 1;
    int s = p->selstartcharpos;
    int quotechar = 0;
    if (s != p->selendcharpos || (s == p->cd->textlen && !dec))
        return FALSE;
    if (dec && s)
        s--;
    if (p->cd->text[s].ch == ')')
        skip = ')', match = '(';
    else if (p->cd->text[s].ch == '}')
        skip = '}', match = '{';
    else if (p->cd->text[s].ch == ']')
        skip = ']', match = '[';
    else
        return FALSE;
    while (--s)
    {
        if (quotechar == p->cd->text[s].ch && (!s || p->cd->text[s-1].ch != '\\' 
                                               || s < 2 || p->cd->text[s-2].ch == '\\'))
            quotechar = 0;
        else if (!quotechar)
        {
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"')
                && (!s || p->cd->text[s-1].ch != '\\'
                    || s < 2 || p->cd->text[s-2].ch == '\\'))
                quotechar = p->cd->text[s].ch;
            else if (p->cd->text[s].ch == skip)
                level++;
            else if (p->cd->text[s].ch == match)
            {
                if (!--level)
                    break;
            }
        }
    }
    if (level)
        return FALSE;
    p->matchingStart = p->selstartcharpos - dec;
    p->matchingEnd = s;
    InvalidateRect(hwnd, NULL, FALSE);
    return TRUE;
}
int FindParenMatchForward(HWND hwnd, EDITDATA *p, int dec)
{
    int skip,match;
    int level = 1;
    int s = p->selstartcharpos;
    int quotechar = 0;
    if (s != p->selendcharpos || (s == p->cd->textlen && !dec))
        return FALSE;
    if (dec && s)
        s--;
    if (p->cd->text[s].ch == '(')
        skip = '(', match = ')';
    else if (p->cd->text[s].ch == '{')
        skip = '{', match = '}';
    else if (p->cd->text[s].ch == '[')
        skip = '[', match = ']';
    else
        return FALSE;
    while (++s != p->cd->textlen)
    {
        if (quotechar == p->cd->text[s].ch && (p->cd->text[s-1].ch != '\\'
                                               || s < 2 || p->cd->text[s-2].ch == '\\'))
            quotechar = 0; 
        else if (!quotechar)
        {
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"')
                && (p->cd->text[s-1].ch != '\\' || s < 2 || p->cd->text[s-2].ch == '\\'))
                quotechar = p->cd->text[s].ch;
            else if (p->cd->text[s].ch == skip)
                level++;
            else if (p->cd->text[s].ch == match)
                if (!--level)
                    break;
        }
    }
    if (level)
        return FALSE;
    p->matchingStart = p->selstartcharpos - dec;
    p->matchingEnd = s;
    InvalidateRect(hwnd, NULL, FALSE);
    return TRUE;
}
void FindParenMatch(HWND hwnd, EDITDATA *p)
{
    if (PropGetBool(NULL, "MATCH_PARENTHESIS"))
        if (!FindParenMatchForward(hwnd, p ,TRUE ))
            FindParenMatchBackward(hwnd, p, TRUE);
}
/**********************************************************************
 * go backwards to the last tab position
 **********************************************************************/
void backtab(HWND hwnd, EDITDATA *p)
{
    int pos = p->selstartcharpos, col = 0, col2;
    if (pos)
    {
        if (pos && p->cd->text[pos - 1].ch == '\t')
            p->selstartcharpos--;
        else
        {
            int sol;
            if (p->cd->text[pos].ch == '\n')
                pos--;
            while (pos)
            {
                if (p->cd->text[pos].ch == '\n')
                {
                    pos++;
                    break;
                }
                pos--;
            }
            sol = pos;
            while (pos != p->selstartcharpos)
            {
                if (p->cd->text[pos].ch == '\t')
                {
                    col = col + p->cd->tabs;
                    col /= p->cd->tabs;
                    col *= p->cd->tabs;
                }
                else
                    col++;
                pos++;
            }
            col2 = col - 1;
            col2 /= p->cd->tabs;
            col2 *= p->cd->tabs;
            col = 0;
            while (col < col2)
            {
                if (p->cd->text[pos].ch == '\t')
                {
                    col = col + p->cd->tabs;
                    col /= p->cd->tabs;
                    col *= p->cd->tabs;
                }
                else
                    col++;
                sol++;
            }
            p->selstartcharpos = sol;
        }
        Replace(hwnd, p, "", 0);
    }

}


/**********************************************************************
 * removechar cuts a character from the text (delete or back arrow)
 **********************************************************************/
void removechar(HWND hwnd, EDITDATA *p, int utype)
{
    if (p->cd->inserting && p->selstartcharpos != p->selendcharpos)
    {
        Replace(hwnd, p, "", 0);
        ScrollCaretIntoView(hwnd, p, FALSE);
    }
    else
    {
        int del;
        if (p->selstartcharpos == p->cd->textlen)
            return ;
        if (p->cd->text[p->selstartcharpos].ch == '\f')
        {
            del = 2;
            insertautoundo(hwnd, p, UNDO_AUTOEND);
            if (utype == UNDO_BACKSPACE)
            {
                p->selstartcharpos++;
                undo_deletechar(hwnd, p, '\n', utype);
                p->selstartcharpos--;
                undo_deletechar(hwnd, p, '\f', utype);
            }
            else
            {
                undo_deletechar(hwnd, p, '\f', utype);
                undo_deletechar(hwnd, p, '\n', utype);
            }
            insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
        }
        else
        {
            del = 1;
            undo_deletechar(hwnd, p, p->cd->text[p->selstartcharpos].ch, utype);
            if (p->cd->text[p->selstartcharpos].ch == '\n')                
            {
                SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p
                    ->selstartcharpos)+1, -1);
            }
        }
        p->cd->sendchangeonpaint = TRUE;
        memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p
            ->selstartcharpos + del, (p->cd->textlen - p->selstartcharpos - del + 1)
            *sizeof(INTERNAL_CHAR));
        p->cd->textlen-=del;
        UpdateSiblings(p, p->selstartcharpos, -del);
        ScrollCaretIntoView(hwnd, p, FALSE);
        drawline(hwnd, p, p->selstartcharpos);
        reparse(hwnd, p);
    }
    p->selendcharpos = p->selstartcharpos;
}
    /**********************************************************************
     * SelToClipboard copies the current selection to the clipboard
     **********************************************************************/
    void SelToClipboard(HWND hwnd, EDITDATA *p)
    {
        int start, end, lf = 0, i;
        if (p->selendcharpos - p->selstartcharpos < 0)
        {
            end = p->selstartcharpos;
            start = p->selendcharpos;
        }
        else
        {
            end = p->selendcharpos;
            start = p->selstartcharpos;
        }
        if (end == start)
            return ;
        for (i = start; i < end; i++)
            if (p->cd->text[i].ch == '\n')
                lf++;
        if (OpenClipboard(hwnd))
        {
            HGLOBAL glmem = GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, end -
                start + 1+lf);
            if (glmem != NULL)
            {
                char *data = GlobalLock(glmem),  *q = data;
                for (i = start; i < end; i++)
                {
                    if (p->cd->text[i].ch == '\n')
                        *q++ = '\r';
                    *q++ = p->cd->text[i].ch;
                }
                *q++ = 0;
                GlobalUnlock(data);
                EmptyClipboard();
                SetClipboardData(CF_TEXT, glmem);
            }
            CloseClipboard();
        }
    }
    /**********************************************************************
     * ClipboardToSel pastes the clipboard into the text
     **********************************************************************/
    void ClipboardToSel(HWND hwnd, EDITDATA *ptr)
    {
        if (!IsClipboardFormatAvailable(CF_TEXT))
            return ;
        if (OpenClipboard(hwnd))
        {
            HANDLE clh = GetClipboardData(CF_TEXT);
            char *data = GlobalLock(clh);
            int l = GlobalSize(clh);
            if (l)
            {
                char *mem = calloc(1,l),  *p = mem,  *q = mem;
                if (mem)
                {
                    memcpy(mem, data, l);
                    mem[l - 1] = 0;
                    while (*p)
                        if (*p == '\r')
                            p++;
                        else
                            *q++ =  *p++;
                    *q = 0;
                    if (mem[0])
                    {
                        Replace(hwnd, ptr, mem, q - mem);
                        ScrollCaretIntoView(hwnd, ptr, FALSE);
                        FormatBufferFromScratch(ptr->colorizeEntries, ptr->cd->text, ptr
                            ->selstartcharpos - 1, ptr->selendcharpos + 1,
                            ptr->cd->language, ptr->cd->defbackground);
                        SendUpdate(hwnd);
                        ptr->cd->sendchangeonpaint = TRUE;
                        ptr->selendcharpos = ptr->selstartcharpos = ptr
                            ->selstartcharpos + strlen(mem);
                        MoveCaret(hwnd, ptr);
                        InvalidateRect(hwnd, 0, 0);
                    }
                    free(mem);
                }
            }
            GlobalUnlock(data);
            CloseClipboard();
        }
    }
    /**********************************************************************
     * upline scrolls the display down one line
     **********************************************************************/
    void upline(HWND hwnd, EDITDATA *p, int lines)
    {
        int ilines = lines;
        int pos;
        int col, index = 0;
        pos = p->selendcharpos;
        //   oldsel = pos ;
        col = p->cd->updowncol;
        if (lines > 0)
        {
            while (lines && pos < p->cd->textlen)
            {
                if (p->cd->text[pos].ch == '\n')
                    lines--;
                pos++;
            }
        }
        else
        {
                if (pos)
                    pos--;
            while (lines && pos)
            {
                if (p->cd->text[pos].ch == '\n')
                    lines++;
                pos--;
            }
            while (pos)
            {
                if (p->cd->text[pos].ch == '\n')
                {
                    pos++;
                    break;
                }
                pos--;
            }
        }
        while (index < col && pos < p->cd->textlen && p->cd->text[pos].ch != '\n')
        {
            if (p->cd->text[pos].ch == '\t')
            {
                index += p->cd->tabs;
                index = (index / p->cd->tabs) *p->cd->tabs;
            }
            else
                index++;
            pos++;
        }
        if (pos && p->cd->text[pos-1].ch == '\f')
            pos--;
        if (!p->cd->selecting)
        {
            if (p->selendcharpos != p->selstartcharpos)
            {
                p->selendcharpos = p->selstartcharpos = pos;
                InvalidateRect(hwnd, 0, 0);
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        ScrollCaretIntoView(hwnd, p, FALSE);
    }
    int lineParsed(EDITDATA *p, int line)
    {
        if (PropGetInt(NULL, "CODE_COMPLETION") != 0)
        {
            if (p->cd->lineData && !IsBadReadPtr(p->cd->lineData, (p->cd->lineDataMax+7)/8))
            {
                if (p->cd->lineDataMax < line)
                    line = p->cd->lineDataMax;
                return !!(p->cd->lineData[line/8] & (1 << (line & 7)));
            }
        }
        return TRUE;
    }
    int getfragment(EDITDATA *p, int pos, int autoDecoration, int colorizing, char *buf, COLORREF *fcolor, COLORREF *bcolor,
        HFONT *font, int *col, int line)
    {
        int count = 0;
        int found = FALSE;
        int attribs = 0;
        int color = (p->cd->defbackground << 5) + C_TEXT;
        int selecting;
        int start, end;
        int matched = FALSE;
        int taboffs = p->leftshownindex % p->cd->tabs;
        int parsed = !colorizing || lineParsed(p, line);
        if (p->cd->nosel)
            start = end = 0;
        else if (p->selstartcharpos <= p->selendcharpos)
        {
            start = p->selstartcharpos;
            end = p->selendcharpos;
        }
        else
        {
            start = p->selendcharpos;
            end = p->selstartcharpos;
        }
        selecting = pos >= start && pos < end;
        while (TRUE)
        {
            if (pos >= p->cd->textlen || p->cd->text[pos].ch == '\n')
                break;
            if (p->cd->text[pos].ch == '\f')
            {
                buf[count++] = '\f';
                pos++;
                break;
            }
            if (selecting)
            {
                if (pos >= end)
                    break;
            }
            else
            {
                if (p->matchingEnd && parsed)
                    if (pos == p->matchingStart || pos == p->matchingEnd)
                    {	
                        buf[count++] = p->cd->text[pos++].ch;
                        (*col)++;
                        matched = TRUE;
                        break;
                    }
                if (pos >= start && pos < end)
                    break;
            }
            if (found)
            {
                if (!selecting && parsed)
                {
                    if (p->cd->text[pos].effect != attribs)
                        break;
                    if (!(attribs &CFE_AUTOCOLOR) && p->cd->text[pos].Color
                        != color)
                        break;
                }
            }
            if (p->cd->text[pos].ch == '\t')
            {
                int newpos = ((*col + p->cd->tabs) / p->cd->tabs) *p->cd->tabs;
                int i;
                for (i =  *col; i < newpos; i++)
                    buf[count++] = ' ';
                pos++;
                *col = newpos;
                found = TRUE;
            }
            else
            {
                if (p->cd->text[pos].ch == ' ')
                {
                    buf[count++] = ' ';
                    pos++;
                    (*col)++;
                    found = TRUE;
                }
                else
                {
                    found = TRUE;
                    attribs = p->cd->text[pos].effect;
                    color = p->cd->text[pos].Color;
                    buf[count++] = p->cd->text[pos++].ch;
                    (*col)++;
                }
            }
            if (p->matchingEnd && parsed)
                if (pos == p->matchingStart || pos == p->matchingEnd)
                {
                    break;
                }
        }
        if (!parsed)
        {
            *fcolor = 0x80808080;
            if (p->cd->defbackground == C_SYS_WINDOWBACKGROUND)
            {
                *bcolor = RetrieveSysColor(COLOR_WINDOW);
            }
            else
                *bcolor = *(colors[(color >> 5) & 0x1f]);
        }
        else if (selecting)
        {
            *fcolor = selectedTextColor;
            *bcolor = selectedBackgroundColor;
        }
        else
        {
            if (attribs &CFE_AUTOCOLOR)
                *fcolor = *(colors[p->cd->defforeground]);
            else
                *fcolor = *(colors[color & 0x1f]);
            if (p->cd->defbackground == C_SYS_WINDOWBACKGROUND)
            {
                *bcolor = RetrieveSysColor(COLOR_WINDOW);
            }
            else
                *bcolor = *(colors[(color >> 5) & 0x1f]);
        }
        switch (attribs &~CFE_AUTOCOLOR)
        {
            case CFE_BOLD:
                *font = p->cd->hBoldFont;
                break;
            case CFE_ITALIC:
                *font = p->cd->hItalicFont;
                if (pos >= p->cd->textlen || p->cd->text[pos].ch == '\n')
                    buf[count++] = ' ';
                break;
            case CFE_BOLD | CFE_ITALIC: default:
                if (color == C_AUTO)
                {
                    switch( autoDecoration)
                    {
                        case 0: default:
                            *font = p->cd->hFont;
                            break;
                        case 1:
                            *font = p->cd->hItalicFont;
                            if (pos >= p->cd->textlen || p->cd->text[pos].ch == '\n')
                                buf[count++] = ' ';
                            break;
                        case 2:
                            *font = p->cd->hBoldFont;
                            break;
                    }
                }
                else
                {
                    *font = p->cd->hFont;
                }
                break;
        }
        if (matched)
        {
            *font = p->cd->hBoldFont;
            *fcolor = keywordColor;
        }
        buf[count] = 0;
        return pos;
    }
    /**********************************************************************
     * EditPaint is the paint procedure for the window.  It selectively
     * paints only those lines that have been marked as changed
     **********************************************************************/
    void EditPaint(HWND hwnd, EDITDATA *p)
    {
        char buf[4000];
        PAINTSTRUCT ps;
        HDC dc;
        RECT r, b1, client;
        int lines, i, pos;
        int baseline;
        HBRUSH selBrush = CreateSolidBrush(selectedBackgroundColor);
        HPEN columnbarPen ;
        int colMark = PropGetInt(NULL, "COLUMN_MARK");
        int autoDecoration = PropGetInt(NULL, "DECORATE_AUTO");
		int colorizing = PropGetBool(NULL, "COLORIZE");
        if (colMark != 0)
        {
            columnbarPen =	CreatePen(PS_SOLID, 0, columnbarColor);
        }
        else
        {
            columnbarPen = CreatePen(PS_NULL, 0, 0);
        }
        ClientArea(hwnd, p, &r);
        GetClientRect(hwnd, &client);
        lines = r.bottom / p->cd->txtFontHeight;
        dc = BeginPaint(hwnd, &ps);
        SetBkMode(dc, OPAQUE);
        pos = p->textshowncharpos;
        baseline = LineFromChar(p, pos)+1;
        for (i = 0; i < lines; i++)
        {
            int col = 0, leftcol = 0;
            r.top = i * p->cd->txtFontHeight;
            r.bottom = r.top + p->cd->txtFontHeight;
            if (IntersectRect(&b1, &r, &ps.rcPaint))
            {
                while (leftcol < p->leftshownindex && p->cd->text[pos].ch != '\n'
                    && p->cd->text[pos].ch != '\f'
                    && pos < p->cd->textlen)
                {
                    if (p->cd->text[pos].ch == '\t')
                    {
                        int v = leftcol;
                        v = v + p->cd->tabs;
                        v = (v / p->cd->tabs) *p->cd->tabs;
                        leftcol = v;
                        if (v > p->leftshownindex)
                            break;
                        pos++;
                    }
                    else
                    {
                        pos++;
                        leftcol++;
                    }
                }
                if (leftcol > p->leftshownindex)
                    leftcol = p->leftshownindex;
                while (p->cd->text[pos].ch != '\n' && pos < p->cd->textlen)
                {
                    int selection = FALSE;
                    COLORREF fcolor, bcolor;
                    HFONT font;
                    int n, j, z=0;
                    int dx[500];
                    int ofs = 0;
                    pos = getfragment(p, pos, autoDecoration, colorizing, buf, &fcolor, &bcolor, &font,
                        &leftcol, baseline + i);
                    if (buf[0] == '\f')
                    {
                        strcpy(buf,"+--------- Page Break ---------+");
                    }
                    SetTextColor(dc, fcolor);
                    SetBkColor(dc, bcolor);
                    SelectObject(dc, font);
                    n = strlen(buf);
                    for (j=0; j < n; j++)
                    {
                        dx[j] = p->cd->txtFontWidth; 
                    }
                    if (col > 1 && font == p->cd->hItalicFont)
                        z = -1;
                    ExtTextOut(dc, col+z, r.top, ETO_OPAQUE, NULL, buf, n,dx);
                    col += (p->cd->txtFontWidth) * n;
                }
            }
            while (pos < p->cd->textlen)
            {
                if (p->cd->text[pos].ch == '\n')
                    break;
                pos++;
            }
            r.left = col;
            if ((pos < p->selendcharpos && pos > p->selstartcharpos)
                    || (pos >= p->selendcharpos && pos < p->selstartcharpos)) {
                FillRect(dc, &r, selBrush);
            }
            else
                FillRect(dc, &r, p->cd->hbrBackground);
            if (colMark && p->leftshownindex < colMark)
            {
                HPEN oldpen = SelectObject(dc, columnbarPen);
                int x = (colMark - p->leftshownindex) * (p->cd->txtFontWidth);
                MoveToEx(dc, x, r.top, NULL);
                         
                LineTo(dc, x, r.bottom);
                SelectObject(dc, oldpen);
            }
            r.left = 0;
            if (pos >= p->cd->textlen)
                break;
            pos++;
        }
        if (r.bottom < client.bottom)
        {
            r.top = r.bottom;
            r.bottom = client.bottom;
            FillRect(dc, &r, p->cd->hbrBackground);
            if (colMark && p->leftshownindex < colMark)
            {
                HPEN oldpen = SelectObject(dc, columnbarPen);
                int x = (colMark - p->leftshownindex) * (p->cd->txtFontWidth);
                MoveToEx(dc, x, r.top, NULL);
                         
                LineTo(dc, x, r.bottom);
                SelectObject(dc, oldpen);
            }
        }
        EndPaint(hwnd, &ps);
        if (p->cd->sendchangeonpaint)
        {
            p->cd->sendchangeonpaint = FALSE;
            SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_CHANGE | 
                (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
        }
        DeleteObject(selBrush);
        DeleteObject(columnbarPen);
    }
    /**********************************************************************
     * eol indexes to the last character in a line
     **********************************************************************/
    void endofline(HWND hwnd, EDITDATA *p)
    {
        int pos;
        pos = p->selendcharpos;
        while (pos < p->cd->textlen && p->cd->text[pos].ch != '\n')
            pos++;
        if (!p->cd->selecting)
        {
            if (p->selendcharpos != p->selstartcharpos)
            {
                p->selendcharpos = p->selstartcharpos = pos;
                InvalidateRect(hwnd, 0, 0);
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        ScrollCaretIntoView(hwnd, p, FALSE);
        MoveCaret(hwnd, p);
    }
    /**********************************************************************
     * sol indexes to the first character in a line
     **********************************************************************/
    void sol(HWND hwnd, EDITDATA *p)
    {
        int pos;
        int encns = FALSE;
        pos = p->selendcharpos;
        if (pos && p->cd->text[pos - 1].ch == '\n')
        {
            pastspace: 
            while (p->cd->text[pos].ch == ' ' || p->cd->text[pos].ch == '\t')
                pos++;
        }
        else
        {
            while (pos && p->cd->text[pos - 1].ch != '\n')
            {
                if (pos && p->cd->text[pos - 1].ch != ' ' && p->cd->text[pos - 1].ch != '\t')
                    encns = TRUE;
                pos--;
            }
            if (encns)
                goto pastspace;
        }
        if (!p->cd->selecting)
        {
            if (p->selendcharpos != p->selstartcharpos)
            {
                p->selendcharpos = p->selstartcharpos = pos;
                InvalidateRect(hwnd, 0, 0);
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        ScrollCaretIntoView(hwnd, p, FALSE);
        MoveCaret(hwnd, p);


    }
    /**********************************************************************
     * left moves the cursor left one character
     **********************************************************************/
    void left(HWND hwnd, EDITDATA *p)
    {
        if (!p->cd->selecting && p->selstartcharpos != p->selendcharpos)
        {
            if (p->selstartcharpos > p->selendcharpos)
                p->selstartcharpos = p->selendcharpos;
            else
                p->selendcharpos = p->selstartcharpos;
            InvalidateRect(hwnd, 0, 0);
        }
        else
        {
            int pos;
            pos = p->selendcharpos;
            if (pos)
            {
                pos--;
                if (pos && p->cd->text[pos-1].ch == '\f')
                    pos--;
            }
            if (!p->cd->selecting)
            {
                if (p->selendcharpos != p->selstartcharpos)
                {
                    p->selendcharpos = p->selstartcharpos = pos;
                    InvalidateRect(hwnd, 0, 0);
                }
                p->selendcharpos = p->selstartcharpos = pos;
            }
            else
            {
                p->selendcharpos = pos;
            }
        }
        MoveCaret(hwnd, p);
        ScrollCaretIntoView(hwnd, p, FALSE);
    }
    /**********************************************************************
     * right moves the cursor right one character
     **********************************************************************/
    void right(HWND hwnd, EDITDATA *p)
    {
        if (!p->cd->selecting && p->selstartcharpos != p->selendcharpos)
        {
            if (p->selstartcharpos < p->selendcharpos)
                p->selstartcharpos = p->selendcharpos;
            else
                p->selendcharpos = p->selstartcharpos;
            InvalidateRect(hwnd, 0, 0);
        }
        else
        {
            int pos;
            pos = p->selendcharpos;
            if (pos < p->cd->textlen)
            {
                if (p->cd->text[pos].ch == '\f')
                    pos++;
                if (pos < p->cd->textlen)
                    pos++;
            }
            if (!p->cd->selecting)
            {
                if (p->selendcharpos != p->selstartcharpos)
                {
                    p->selendcharpos = p->selstartcharpos = pos;
                    InvalidateRect(hwnd, 0, 0);
                }
                p->selendcharpos = p->selstartcharpos = pos;
            }
            else
            {
                p->selendcharpos = pos;
            }
        }
        MoveCaret(hwnd, p);
        ScrollCaretIntoView(hwnd, p, FALSE);
    }
    void leftword(HWND hwnd, EDITDATA *p)
    {
        int pos;
        int flag = 0;
        pos = p->selendcharpos;
        if (pos)
            pos--;
        while (pos && isspace(p->cd->text[pos].ch))
            pos--;
        while (pos && (isalnum(p->cd->text[pos].ch) || p->cd->text[pos].ch == '_'))
        {
            flag = 1;
            pos--;
        }
        if (pos && flag)
            pos++;
        if (!p->cd->selecting)
        {
            if (p->selendcharpos != p->selstartcharpos)
            {
                p->selendcharpos = p->selstartcharpos = pos;
                InvalidateRect(hwnd, 0, 0);
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        MoveCaret(hwnd, p);
        ScrollCaretIntoView(hwnd, p, FALSE);
    }
    void rightword(HWND hwnd, EDITDATA *p)
    {
        int pos;
        int flag = 0;
        pos = p->selendcharpos;
        while (pos < p->cd->textlen && (isalnum(p->cd->text[pos].ch) || p->cd->text[pos].ch
            == '_'))
        {
            flag = TRUE;
            pos++;
        }
        while (pos < p->cd->textlen && isspace(p->cd->text[pos].ch))
        {
            flag = TRUE;
            pos++;
        }
        if (!flag && pos < p->cd->textlen)
        {
            pos++;
            while (pos < p->cd->textlen && isspace(p->cd->text[pos].ch))
            {
                pos++;
            }
        }
        if (!p->cd->selecting)
        {
            if (p->selendcharpos != p->selstartcharpos)
            {
                p->selendcharpos = p->selstartcharpos = pos;
                InvalidateRect(hwnd, 0, 0);
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        MoveCaret(hwnd, p);
        ScrollCaretIntoView(hwnd, p, FALSE);
    }
    /**********************************************************************
     * AutoTimeProc fires When the cursor is moved out of the window during 
     * a select operation.  It is used to keep the text scrolling.
     **********************************************************************/
    void CALLBACK AutoTimeProc(HWND hwnd, UINT message, UINT event, DWORD timex)
    {
        EDITDATA *p = (EDITDATA*)GetWindowLong(hwnd, 0);
        RECT r;
        ClientArea(hwnd, p, &r);
        scrollup(hwnd, p, p->cd->autoscrolldir);
        if (p->cd->autoscrolldir < 0)
        {
            p->selendcharpos = p->textshowncharpos;
        }
        else
        {
            int lines, pos = p->textshowncharpos;
            lines = r.bottom / p->cd->txtFontHeight;
            while (lines && pos < p->cd->textlen)
            {
                if (p->cd->text[pos].ch == '\n')
                    lines--;
                pos++;
            }
            p->selendcharpos = pos;
        }
        MoveCaret(hwnd, p);
    }
    /**********************************************************************
     * StartAutoScroll is called to start the timer which keeps the screen
     * scrolling while the cursor is out of the window during a select
     **********************************************************************/
    void StartAutoScroll(HWND hwnd, EDITDATA *p, int dir)
    {
        if (!p->cd->autoscrolldir)
        {
            p->cd->autoscrolldir = dir;
            SetTimer(hwnd, 1, 120, AutoTimeProc);
        }
    }
    /**********************************************************************
     * EndAutoScroll stops the above timer
     **********************************************************************/
    void EndAutoScroll(HWND hwnd, EDITDATA *p)
    {
        if (p->cd->autoscrolldir)
        {
            KillTimer(hwnd, 1);
            p->cd->autoscrolldir = 0;
        }
    }
    /**********************************************************************
     * HilightWord selects the word under the cursor
     **********************************************************************/
    void HilightWord(HWND hwnd, EDITDATA *p)
    {
        if (keysym(p->cd->text[p->selstartcharpos].ch))
        {
            while (p->selstartcharpos && keysym(p->cd->text[p->selstartcharpos -
                1].ch))
                p->selstartcharpos--;
            while (p->selendcharpos < p->cd->textlen && keysym(p->cd->text[p
                ->selendcharpos].ch))
                p->selendcharpos++;
        }
    }
    /**********************************************************************
     * undo_pchar undoes a delete character operation
     **********************************************************************/
    void undo_pchar(HWND hwnd, EDITDATA *p, int ch)
    {
        insertchar(hwnd, p, ch);
        FormatLine(hwnd, p->cd->text, p->cd->language, p->cd->defbackground);
    }
    /**********************************************************************
     * doundo is the primary routine to traverse the undo list and perform
     * an undo operation
     **********************************************************************/

    int doundo(HWND hwnd, EDITDATA *p)
    {
        int rv = 1;
        if (SendMessage(hwnd, EM_CANUNDO, 0, 0))
        {
            UNDO *u ;
            int oldinsert = p->cd->inserting;
            int start;
            int x = p->cd->redopos;
            if (x == p->cd->undohead)
            {
                if (--x < 0)
                    x += UNDO_MAX;
            }
            u = &p->cd->undolist[x];
            if (--x < 0)
                x += UNDO_MAX;
            p->cd->undoing++;
            p->cd->inserting = TRUE;
            rv = 0;
            switch (u->type)
            {
                case UNDO_INSERT:
                    start = u->undotemp;
                    start--;
                    if (keysym(u->data[start]))
                        while (start > 0 && keysym(u->data[start-1]))
                            start--;
                    p->selstartcharpos = u->preselstart + start;
                    p->selendcharpos = p->selstartcharpos + u->undotemp -start;
                    u->postselstart = u->postselend = p->selstartcharpos;
                    u->undotemp = start;
                    if (u->undotemp <= 0)
                    {
                        p->cd->modified = u->modified;
                        p->cd->redopos = x;
                    }
                    Replace(hwnd, p, "", 0);
                    break;
                case UNDO_DELETE:
                    start = u->undotemp;
                    if (u->len > start && keysym(u->data[start]))
                        while (u->len > start && keysym(u->data[start]))
                        {
                            p->selstartcharpos = p->selendcharpos = u->preselstart;
                            undo_pchar(hwnd, p, u->data[start++]);
                        }
                    else
                    {
                        p->selstartcharpos = p->selendcharpos = u->preselstart;
                        undo_pchar(hwnd, p, u->data[start++]);
                    }
                    u->undotemp = start;
                    if (u->len <= start)
                    {
                        p->cd->modified = u->modified;
                        p->cd->redopos = x;
                    }
                    p->selstartcharpos = p->selendcharpos = u->preselstart;
                    break;
                case UNDO_BACKSPACE:
                    start = u->undotemp;
                    p->selstartcharpos = p->selendcharpos = u->postselstart;
                    if (u->len > start && keysym(u->data[start]))
                        while (u->len > start && keysym(u->data[start]))
                        {
                            undo_pchar(hwnd, p, u->data[start++]);
                        }
                    else
                    {
                        undo_pchar(hwnd, p, u->data[start++]);
                    }
                    u->postselstart = u->postselend += start - u->undotemp ;
                    u->undotemp = start;
                    if (u->len <= start)
                    {
                        p->cd->modified = u->modified;
                        p->cd->redopos = x;
                    }
                    break;
                case UNDO_MODIFY:
                    p->cd->inserting = FALSE;
                    start = u->undotemp;
                    if (keysym(u->data[start]))
                        while (keysym(u->data[start]) && u->len > start)
                        {
                            int n;
                            p->selstartcharpos = p->selendcharpos = u->postselstart - start-1;
                            n = p->cd->text[p->selstartcharpos].ch;
                            undo_pchar(hwnd, p, u->data[start]);
                            u->data[start++] = n;
                        }
                    else
                    {
                        int n;
                        p->selstartcharpos = p->selendcharpos = u->postselstart - start-1;
                        n = p->cd->text[p->selstartcharpos].ch;
                        undo_pchar(hwnd, p, u->data[start]);
                        u->data[start++] = n;
                        if (u->data[start] == '\n')
                        {
                            p->selstartcharpos = p->selendcharpos = u
                                ->postselstart - start;
                            n = p->cd->text[p->selstartcharpos].ch;
                            undo_pchar(hwnd, p, u->data[start]);
                            u->data[start++] = n;
                        }
                    }
                    u->undotemp = start;
                    if (u->len <= start)
                    {
                        p->cd->modified = u->modified;
                        p->cd->redopos = x;
                    }
                    p->selstartcharpos = p->selendcharpos = u->postselstart - start;
                    break;
                case UNDO_INSERTSELECTION:
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
                    Replace(hwnd, p, "", 0);
                    p->cd->modified = u->modified;
                    p->cd->redopos = x;
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    break;
                case UNDO_DELETESELECTION:
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
                    Replace(hwnd, p, (char *)u->data, u->len);
                    p->cd->modified = u->modified;
                    p->cd->redopos = x;
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    break;
                case UNDO_CASECHANGE:
                {
                    int s = p->selstartcharpos;
                    int e = p->selendcharpos;
                    
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
                    if (p->selstartcharpos > p->selendcharpos)
                    {
                        int i = p->selstartcharpos;
                        p->selstartcharpos = p->selendcharpos;
                        p->selendcharpos = i;
                    }
                    p->cd->modified = u->modified;
                    p->cd->redopos = x;
                    for (start = 0; start < u->len; start++)
                    {
                        int n = p->cd->text[p->selstartcharpos + start].ch;
                        p->cd->text[p->selstartcharpos + start].ch = u->data[start];
                        u->data[start] = n;
                    }
                    if (u->noChangeSel)
                    {
                        p->selstartcharpos = s;
                        p->selendcharpos = e;
                    }
                    else
                    {
                        p->selstartcharpos = u->preselstart;
                        p->selendcharpos = u->preselend;
                    }
                    break;
                }
                case UNDO_AUTOBEGIN:
                    p->cd->redopos = x;
                    while (!doundo(hwnd, p))
                        ;
                    break;
                case UNDO_AUTOCHAINBEGIN:
                    p->cd->redopos = x;
                    while (!doundo(hwnd, p))
                        ;
                    doundo(hwnd, p); 
                        // will undo things recursively if there are more auto-begins
                    break;
                case UNDO_AUTOEND:
                case UNDO_AUTOCHAINEND:
                    p->cd->redopos = x;
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    p->textshowncharpos = u->charpos;
                    p->cd->modified = u->modified;
                    rv = 1;
                    break;
            }
            p->cd->inserting = oldinsert;
            p->cd->undoing--;
            p->cd->sendchangeonpaint = TRUE;
            InvalidateRect(hwnd, 0, 0);
            ScrollCaretIntoView(hwnd, p, FALSE);
            reparse(hwnd, p);
        }
        return rv;
    }
    /**********************************************************************
     * doundo is the primary routine to traverse the undo list and perform
     * an undo operation
     **********************************************************************/

    int doredo(HWND hwnd, EDITDATA *p)
    {
        int rv = 1;
        int start;
        if (SendMessage(hwnd, EM_CANREDO, 0, 0))
        {
            int oldinsert = p->cd->inserting;
            UNDO *u;
            int x = p->cd->redopos+1;
            if (x >= UNDO_MAX)
                x -= UNDO_MAX;
            if (x != p->cd->undotail)
                x = p->cd->redopos;
            u = &p->cd->undolist[x];
            if (++x >= UNDO_MAX)
                x -= UNDO_MAX;
            p->cd->undoing++;
            p->cd->inserting = TRUE;
            rv = 0;
            switch (u->type)
            {
                case UNDO_INSERT:
                    p->selstartcharpos = p->selendcharpos = u->preselstart + u->undotemp;
                    start = u->undotemp;
                    if (keysym(u->data[start]))
                        while (u->len > start && keysym(u->data[start]))
                        {
                            undo_pchar(hwnd, p, u->data[start++]);
                        }
                    else
                    {
                        undo_pchar(hwnd, p, u->data[start++]);
                    }
                    u->undotemp = start;
                    u->postselstart = u->postselend = u->preselstart + start;
                    if (u->len <= u->undotemp)
                    {
                        p->cd->modified = p->cd->undolist[x].modified;
                        p->cd->redopos = x;
                    }
                    break;
                case UNDO_DELETE:
                    start = u->undotemp;
                    if (keysym(u->data[start-1]))
                    {
                        while (start >0 && keysym(u->data[start-1]))
                        {
                            start--;
                        }
                    }
                    else
                    {
                        start--;
                    }
                    p->selstartcharpos = u->preselstart ;
                    p->selendcharpos = p->selstartcharpos + u->undotemp - start;
                    Replace(hwnd, p, "", 0);
                    u->undotemp = start;
                    if (u->undotemp == 0)
                    {
                        p->cd->modified = p->cd->undolist[x].modified;
                        p->cd->redopos = x;
                    }
                    break;
                case UNDO_BACKSPACE:
                    start = u->undotemp;
                    if (keysym(u->data[start-1]))
                    {
                        while (start >0 && keysym(u->data[start-1]))
                        {
                            start--;
                        }
                    }
                    else
                    {
                        start--;
                    }
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = p->selstartcharpos - u->undotemp + start;
                    Replace(hwnd, p, "", 0);
                    u->postselstart = u->postselend -= u->undotemp - start;
                    u->undotemp = start;
                    if (u->undotemp == 0)
                    {
                        p->cd->modified = p->cd->undolist[x].modified;
                        p->cd->redopos = x;
                    }
                    break;
                case UNDO_MODIFY:
                    p->cd->inserting = FALSE;
                    start = u->undotemp;
                    p->selendcharpos = p->selstartcharpos = u->preselstart + u->len - start;
                    
                    if (keysym(u->data[start-1]))
                        while (start > 0 && keysym(u->data[start-1]))
                        {
                            int n = p->cd->text[p->selstartcharpos].ch;
                            undo_pchar(hwnd, p, u->data[--start]);
                            u->data[start] = n;
                        }
                    else
                    {
                        int n = p->cd->text[p->selstartcharpos].ch;
                        undo_pchar(hwnd, p, u->data[--start]);
                        u->data[start] = n;
                    }
                    u->undotemp = start;
                    if (start == 0)
                    {
                        p->cd->modified = p->cd->undolist[x].modified;
                        p->cd->redopos = x;
                    }
                    break;
                case UNDO_INSERTSELECTION:
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    Replace(hwnd, p, (char *)u->data, u->len);
                    p->cd->modified = p->cd->undolist[x].modified;
                    p->cd->redopos = x;
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
    
                    break;
                case UNDO_DELETESELECTION:
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    Replace(hwnd, p, "", 0);
                    p->cd->modified = p->cd->undolist[x].modified;
                    p->cd->redopos = x;
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
                    break;
                case UNDO_CASECHANGE:
                {
                    int s = p->selstartcharpos;
                    int e = p->selendcharpos;
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    if (p->selstartcharpos > p->selendcharpos)
                    {
                        int i = p->selstartcharpos;
                        p->selstartcharpos = p->selendcharpos;
                        p->selendcharpos = i;
                    }
                    p->cd->modified = p->cd->undolist[x].modified;
                    p->cd->redopos = x;
                    for (start = 0; start < u->len; start++)
                    {
                        int n = p->cd->text[p->selstartcharpos + start].ch;
                        p->cd->text[p->selstartcharpos + start].ch = u->data[start];
                        u->data[start] = n;
                    }
                    if (u->noChangeSel)
                    {
                        p->selstartcharpos = s;
                        p->selendcharpos = e;
                    }
                    else
                    {
                        p->selstartcharpos = u->postselstart;
                        p->selendcharpos = u->postselend;
                    }
                    break;
                }
                case UNDO_AUTOEND:
                    p->cd->redopos = x;
                    while (!doredo(hwnd, p))
                        ;
                    break;
                case UNDO_AUTOCHAINEND:
                    p->cd->redopos = x;
                    while (!doredo(hwnd, p))
                        ;
                    doredo(hwnd, p); 
                        // will undo things recursively if there are more auto-ends
                    break;
                case UNDO_AUTOBEGIN:
                case UNDO_AUTOCHAINBEGIN:
                    p->cd->redopos = x;
                    p->cd->modified = p->cd->undolist[x].modified;
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
                    p->textshowncharpos = u->charpos;
                    rv = 1;
                    break;
            }
            p->cd->inserting = oldinsert;
            p->cd->undoing--;
            p->cd->sendchangeonpaint = TRUE;
            InvalidateRect(hwnd, 0, 0);
            ScrollCaretIntoView(hwnd, p, FALSE);
            reparse(hwnd, p);
        }
        return rv;
    }
    void verticalCenter(HWND hwnd, EDITDATA *p)
    {
        RECT bounds;
        int rows, pos, lines=0;
        ClientArea(hwnd, p, &bounds);
        rows = (bounds.bottom - bounds.top) / p->cd->txtFontHeight;
        rows = rows ; /* find center */
        if (p->selstartcharpos <= p->selendcharpos)
            pos = p->selendcharpos;
        else
            pos = p->selstartcharpos;
        while(--pos > p->textshowncharpos)
        {
            if (p->cd->text[pos].ch == '\n')
                lines++;
        }

        pos = p->textshowncharpos;
        
        lines = rows/2 - lines ;
        
        if (lines > 0)
        {
            while (lines > 0 && pos > 0)
            {
                pos--;
                if (p->cd->text[pos].ch == '\n')
                    lines--;
            }
            if (pos)
                pos++;
        }
        else
        {
            while (lines < 0 && p->cd->text[pos].ch)
            {
                if (p->cd->text[pos].ch == '\n')
                    lines++;
                pos++;
            }
        }
        p->textshowncharpos = pos;
        InvalidateRect(hwnd, 0, 0);
        MoveCaret(hwnd, p);
    }
    void upperlowercase(HWND hwnd, EDITDATA *p, int ucase)
    {
        int oldsel = -1;
        int i;
        int s, e;
        UNDO *u;
        u = undo_casechange(hwnd, p);
        if (p->selstartcharpos == p->selendcharpos)
        {
            e = s = p->selstartcharpos;
            if (!isalnum(p->cd->text[e].ch) && p->cd->text[e].ch != '_')
                return;
            e++;
            while (p->cd->text[e].ch && (isalnum(p->cd->text[e].ch) || p->cd->text[e].ch == '_'))
                   e++;
            while (s && (isalnum(p->cd->text[s-1].ch) || p->cd->text[s-1].ch == '_'))
                s--;
            oldsel = p->selstartcharpos;
            p->selstartcharpos = s;
            p->selendcharpos = e;
        }
        else
        {
            s = p->selstartcharpos;
            e = p->selendcharpos;
        }
        u->postselstart = s;
        u->postselend = e;
        if (e < s)
        {
            int v = s;
            s = e;
            e = v;
        }
        if (oldsel != -1)
        {
            u->noChangeSel = TRUE;
            p->selstartcharpos = p->selendcharpos = oldsel;
        }
        for (i = s; i < e; i++)
        {
            p->cd->text[i].ch = ucase ? toupper(p->cd->text[i].ch): tolower(p
                ->cd->text[i].ch);
        }
        InvalidateRect(hwnd, 0, 0);
    }
    void SendUpdate(HWND hwnd)
    {
        SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_UPDATE | 
            (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
    }
    typedef struct qq
    {
        EDITDATA *ed;
        HWND wnd;
        int creation;
    } CINFO;
    static void colorizeThread(CINFO *info)
    {
        if (info->creation)
        {
            ShowWindow(info->wnd, SW_SHOW);
            SendMessage(info->wnd, EM_GETLINECOUNT, 0, 0); // update scroll bar
            info->ed->cd->sendchangeonpaint = TRUE;
            SendUpdate(info->wnd);
        }
        FormatBufferFromScratch(info->ed->colorizeEntries, info->ed->cd->text, 0, info->ed->cd->textlen, info->ed->cd
            ->language, info->ed->cd->defbackground);
        InvalidateRect(info->wnd, 0, 1);
        info->ed->cd->colorizing--;
        free(info);
    }
    void FullColorize(HWND hwnd, EDITDATA *p, int creation)
    {
        CINFO *info = calloc(1,sizeof(CINFO));
        if (info)
        {
            DWORD hand;
            info->ed = p;
            info->wnd = hwnd;
            info->creation = creation;
            SetLastError(0);
            p->cd->colorizing++;
            _beginthread((BEGINTHREAD_FUNC)colorizeThread, 0, (LPVOID)info);
        }
    }
    struct llist 
    {
        struct llist *next;
        char name[256];
    } ;
    LRESULT CALLBACK codecompProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
        lParam)
    {
        static HWND parent;
        static HWND hwndLB;
        static char buf[256];
        static int len;
        static int begin_inserted;
        static int selected;
        PAINTSTRUCT ps;
        HDC dc;
        char buf2[256];
        EDITDATA *p ;
        DRAWITEMSTRUCT *lpdis;
        MEASUREITEMSTRUCT *lpmis;
        TEXTMETRIC tm;
        HFONT font;
        RECT r;
        int fg, bg;
        int i = selected;
        HPEN pen;
        HBRUSH brush;
        static struct llist *stringdata;
        static int initialLen;
        char *temp;
        switch(iMessage)
        {
            case WM_MEASUREITEM: 
                lpmis = (LPMEASUREITEMSTRUCT) lParam; 
                dc = GetDC(hwnd);
                GetTextMetrics(dc, &tm);
                ReleaseDC(hwnd, dc);
                /* Set the height of the list box items. */ 
     
                lpmis->itemHeight = tm.tmHeight; 
                return TRUE; 
     
//	 		case WM_COMPAREITEM:
//				lpcis = (LPCOMPAREITEMSTRUCT)lParam;
//				return strcmp((char *)lpcis->itemData1, (char *)lpcis->itemData2);
            case WM_DRAWITEM: 
     
                lpdis = (LPDRAWITEMSTRUCT) lParam; 
                /* If there are no list box items, skip this message. */ 
     
                /* 
                 * Draw the bitmap and text for the list box item. Draw a 
    
                 * rectangle around the bitmap if it is selected. 
                 */ 
                switch (lpdis->itemAction) { 
     
                    case ODA_SELECT: 
                    case ODA_DRAWENTIRE: 
                        if (lpdis->itemState == ODS_SELECTED) 
                        {
                              FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)(COLOR_HIGHLIGHT + 1));
                            fg = SetTextColor(lpdis->hDC, RetrieveSysColor(COLOR_HIGHLIGHTTEXT));
                            bg = SetBkColor(lpdis->hDC, RetrieveSysColor(COLOR_HIGHLIGHT));
                        }
                        else
                        {
                              FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)(COLOR_INFOBK+1));
                            bg = SetBkColor(lpdis->hDC, RetrieveSysColor(COLOR_INFOBK));
                        }
                        
                        temp = (char *)SendMessage(lpdis->hwndItem, LB_GETITEMDATA, lpdis->itemID, 0);
                        strcpy(buf2, temp);
                        GetTextMetrics(lpdis->hDC, &tm); 
     
                        TextOut(lpdis->hDC, 
                            6, 
                            (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2,
                            buf2, 
                            strlen(buf2)); 

                        if (lpdis->itemState == ODS_SELECTED) 
                        { 
                            SetTextColor(lpdis->hDC, fg);
//		 					font = SelectObject(lpdis->hDC, font);
//							DeleteObject(font); 
                        }
                        SetBkColor(lpdis->hDC, bg);
                        break; 
     
                    case ODA_FOCUS: 
                        /* 
                         * Do not process focus changes. The focus caret 
    
                         * (outline rectangle) indicates the selection. 
                         * The Which one? (IDOK) button indicates the final 
                         * selection. 
                         */ 
                        break; 
                }
                return TRUE;
            case WM_CREATE:
                hwndLB = CreateWindow("LISTBOX","", 
                                       WS_CHILD | WS_VISIBLE | LBS_NOTIFY |
                                       LBS_OWNERDRAWFIXED | WS_VSCROLL | LBS_SORT |
                                       LBS_WANTKEYBOARDINPUT | WS_CLIPSIBLINGS,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       hwnd, (HMENU)100, hInstance,0);
                ApplyDialogFont(hwndLB);
                SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
                break;
            case WM_DESTROY:
                SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
                DestroyWindow(hwndLB);
                hwndLB = NULL;
                codecompleteBox = NULL;
                inStructBox = FALSE;
                break;
            case WM_PAINT:
                dc = BeginPaint(hwnd, &ps);
                GetClientRect(hwnd, &r);
                brush = CreateSolidBrush(TRANSPARENT_COLOR);
                FillRect(dc, &r, brush);
                DeleteObject(brush);

                pen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_INFOTEXT));
                pen = SelectObject(dc, pen); 
                RoundRect(dc, r.left, r.top, r.right, r.bottom, 5, 5);
                pen = SelectObject(dc, pen);
                DeleteObject(pen);
                
                EndPaint(hwnd, &ps);
                return 0;
            case WM_USER:
                parent = (HWND)lParam;
                p= (EDITDATA *)GetWindowLong(parent, 0);
                insertautoundo(parent, p, UNDO_AUTOEND);
                len = 0;
                selected = 0;
                initialLen = 0;
                begin_inserted = FALSE;
                break;
            case WM_USER+1:
            {
                len = 0;
                selected = SendMessage(hwndLB, LB_GETCURSEL, 0, 0);
                if (selected != LB_ERR)
                {
                    temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                    buf[len++] = temp[0];
                    buf[len] = 0;
                }
            }
                break;
            case WM_USER+2:
            {
                strcpy(buf, (char *)lParam);
                len = initialLen = strlen(buf);
                selected = SendMessage(hwnd, LB_SELECTSTRING, -1, (LPARAM)buf);
                if (selected != LB_ERR)
                {
                    temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                    strcpy(buf2, temp);
                    if (!stricmp(buf, buf2))
                    {
                        temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, selected+1, 0);
                        if ((int)temp != LB_ERR)
                        {
                            if (strnicmp(temp, buf2, strlen(buf2)) != 0)
                                PostMessage(hwnd, WM_CLOSE, 0, 0);
                        }
                        else
                        {
                            PostMessage(hwnd, WM_CLOSE, 0, 0);
                        }
                    }
                }
                return -1;
            }
                break;
            case WM_SETFOCUS:
                SetFocus(hwndLB);
                break;
            case WM_CLOSE:
                if (!begin_inserted)
                {
                    p= (EDITDATA *)GetWindowLong(parent, 0);
                    begin_inserted = TRUE;
                    insertautoundo(parent, p, UNDO_AUTOBEGIN);
                    if (len == 0)
                        doundo(parent, p);
                }
                break ;
            case WM_COMMAND:
                if (HIWORD(wParam) == LBN_KILLFOCUS)
                {
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                } 
                else if (HIWORD(wParam) == LBN_DBLCLK)
                {
                    PostMessage(hwndLB, WM_KEYDOWN, VK_RETURN, 0);
                }
                else if (HIWORD(wParam) == LBN_SELCHANGE)
                {
                    selected = SendMessage(hwndLB, LB_GETCURSEL, 0, 0);
                }
                break;
            case WM_SIZE:
                MoveWindow(hwndLB, 2, 2, (lParam &65535)-4, (lParam >> 16)-4, 1);
                break;
            case WM_VKEYTOITEM:
                switch(LOWORD(wParam))
                {				
                    case VK_RETURN:
                    case VK_RIGHT:
                        if (len && selected != LB_ERR)
                        {
                            EDITDATA *p = (EDITDATA *)GetWindowLong(parent, 0);
                            int count;
                            begin_inserted = TRUE;
                            insertautoundo(parent, p, UNDO_AUTOBEGIN);
                            doundo(parent, p);

                            temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                            strcpy(buf, temp);
                            count = strlen(buf);
                            p->selstartcharpos -= initialLen;
                            p->selendcharpos = p->selstartcharpos + initialLen;
                            Replace(parent, p, buf, count);
                            p->selstartcharpos = p->selendcharpos = p->selstartcharpos + count;
                            MoveCaret(parent, p);
                        }
                        // fallthrough
                        
                    case VK_SPACE:
                    case VK_ESCAPE:
                    case VK_LEFT:
                        PostMessage(hwnd, WM_CLOSE, 0, 0);
                        return -2;
                    case VK_UP:
                    case VK_DOWN:
                    case VK_PRIOR:
                    case VK_NEXT:
                        PostMessage(hwnd, WM_USER + 1, 0, 0);
                        return -1;
                    case VK_BACK:
                        if (len > initialLen)
                        {
                            len--;
                            buf[len] = 0;
                            if (len == 0)
                            {
                                selected = 0;
                                PostMessage(hwndLB, LB_SETCURSEL, -1, 0);
                            }
                            else
                                selected = SendMessage(hwnd, LB_SELECTSTRING, -1, (LPARAM)buf);
                        }
                        else
                        {
                            p= (EDITDATA *)GetWindowLong(parent, 0);
                            begin_inserted = TRUE;
                            insertautoundo(parent, p, UNDO_AUTOBEGIN);
                            doundo(parent, p);
                            PostMessage(hwnd, WM_CLOSE, 0, 0);
                        }
                        SendMessage(parent, WM_KEYDOWN, VK_BACK, lParam);
                        return -2;
                    case VK_DELETE:
                        len = 0;
                        selected = 0;
                        buf[len] = 0;
                        SendMessage(hwndLB, LB_SETCURSEL, -1, 0);
                        p= (EDITDATA *)GetWindowLong(parent, 0);
                        insertautoundo(parent, p, UNDO_AUTOBEGIN);
                        doundo(parent, p);
                        insertautoundo(parent, p, UNDO_AUTOEND);
                        return -2;
                    case VK_SHIFT:
                    case VK_CONTROL:
                        break;
                    default:
                        if (GetKeyState(VK_CONTROL) &0x80000000)
                        {
                    case VK_HOME:
                    case VK_END:
                            p= (EDITDATA *)GetWindowLong(parent, 0);
                            begin_inserted = TRUE;
                            insertautoundo(parent, p, UNDO_AUTOBEGIN);
//                            doundo(parent, p);
                            PostMessage(hwnd, WM_CLOSE, 0, 0);
                            SendMessage(parent, WM_KEYDOWN, wParam, lParam);
                            return -2;
                        }
                        break;
                }
                return -1;
            case WM_CHARTOITEM:
                if (wParam == '.' || wParam == '-')
                {
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    SendMessage(parent, WM_CHAR, wParam, 0);
                }
                else
                {
                    wParam = LOWORD(wParam);
                    buf[len++] = wParam;
                    buf[len] = 0;
                    SendMessage(parent, WM_CHAR, wParam, 0);
                    selected = SendMessage(hwnd, LB_SELECTSTRING, -1, (LPARAM)buf);
                    if (selected != LB_ERR)
                    {
                        temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                        strcpy(buf2, temp);
                        if (!stricmp(buf, buf2))
                        {
                            // if it is the prefix of another item don't close out the box
                            temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, selected+1, 0);
                            if (temp == (char *)-1 || strnicmp(buf, temp, len))
                                PostMessage(hwnd, WM_CLOSE, 0, 0);
                        }
                    }
                }
                return -1;
            case LB_RESETCONTENT:
                SendMessage(hwndLB, LB_RESETCONTENT, 0, 0);
                while (stringdata)
                {
                    struct llist *next = stringdata->next;
                    free(stringdata);
                    stringdata = next;
                }
                return 0;
            case LB_ADDSTRING:
                {
                    struct llist *ll = malloc(sizeof(struct llist)), **find = &stringdata;
                    if (ll)
                    {
                        int i = 0, rv;
                        strncpy(ll->name, (char *)lParam, 256);
                        ll->name[255] = 0;
                        while ((*find) && strcmp(ll->name, (*find)->name) > 0)
                        {
                            i++;
                            find = &(*find)->next;
                        }
                        ll->next = *find;
                        *find = ll;
                        rv =  SendMessage(hwndLB, LB_INSERTSTRING, i, (LPARAM)ll->name);
                        SendMessage(hwndLB, LB_SETCURSEL, selected = 0, 0);
                        return rv;
                    }
                }
                return 0;
            case LB_SELECTSTRING:
            {
                int i = wParam +1;
                do
                {
                    temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, i, 0);
                    if ((int)temp != LB_ERR)
                        if (!strnicmp(temp, (char *)lParam, strlen((char *)lParam)))
                        {
                            int n = SendMessage(hwndLB, LB_GETCOUNT, 0, 0);
                            if (n > 8)
                                n = 8;
                            n = i - n/2;
                            if (n < 0)
                                n = 0;
                            SendMessage(hwndLB, LB_SETTOPINDEX, n, 0);
                            SendMessage(hwndLB, LB_SETCURSEL, i, 0);
                            return i;
                        }
                    i++;
                } while ((int)temp != LB_ERR);
                if (wParam != -1)
                {
                    for (i=0; i <= wParam; i++)
                    {
                        buf2[0] = 0;
                        temp = (char *)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                        if ((int)temp != LB_ERR)
                            if (!strnicmp(temp, (char *)lParam, strlen((char *)lParam)))
                            {
                                int n = SendMessage(hwndLB, LB_GETCOUNT, 0, 0);
                                if (n > 8)
                                    n = 8;
                                n = i - n/2;
                                if (n < 0)
                                    n = 0;
                                SendMessage(hwndLB, LB_SETTOPINDEX, n, 0);
                                SendMessage(hwndLB, LB_SETCURSEL, i, 0);
                                return i;
                            }
                    }
                }
                SendMessage(hwndLB, LB_SETCURSEL, -1, 0);
                return LB_ERR;
            }
            default:
                if (iMessage >= LB_ADDSTRING && iMessage <= LB_SETCOUNT)
                    return SendMessage(hwndLB, iMessage, wParam, lParam);
                break;
                
        }
        return DefWindowProc(hwnd, iMessage, wParam, lParam);
    }
    void GetCompletionPos(HWND hwnd, EDITDATA *p, LPPOINT pt, int width, int height)
    
    {
        if (posfromchar(hwnd, p, pt, p->selstartcharpos))
        {
            RECT rect, frame;
            GetWindowRect(hwnd, &rect);
            GetFrameWindowRect(&frame);
            pt->x -= width/2;
            pt->x += rect.left;
//			pt->x -= frame.left;
            pt->y += p->cd->txtFontHeight ; //+ GetSystemMetrics(SM_CYCAPTION) * 2;
            pt->y += rect.top ;
//			pt->y -= frame.top;
            if (pt->y + height > frame.bottom - frame.top)
            {
                pt->y -= p->cd->txtFontHeight;
                pt->y -= height;
            }			
        }
    }
    CCSTRUCTDATA * find_type(HWND hwnd, EDITDATA *p, int ch, sqlite3_int64 *id)
    {
        char buf[4096], *q = buf+4095, *p1, *r1;
        DWINFO *info = (DWINFO *)GetWindowLong(GetParent(hwnd), 0);
        int lineno = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, p->selstartcharpos);
        int pos = p->selstartcharpos-1;
        int done = FALSE;
        int wading = FALSE;
        int parenLevel = 0;
        CCSTRUCTDATA *structData = NULL;
        if (!info || PropGetInt(NULL, "CODE_COMPLETION") == 0)
            return NULL;
        *q = 0;
        if (pos <= 0)
            return NULL;
        while (pos && isspace (p->cd->text[pos].ch))
            pos--;
        while (pos && !done && q != buf)
        {
            switch(p->cd->text[pos].ch)
            {
                case ';':
                case '}':
                case '{':
                    done = TRUE;
                    break;
                case ',':
                    if (!wading)
                        done = TRUE;
                    break;
                case ')':
                    parenLevel++;
                    *--q = ')';
                    break;
                case '(':
                    if (parenLevel)
                    {
                        parenLevel--;
                        *(--q) = '(';
                    }
                    else
                    {
                        done = TRUE;
                    }
                    break;
                case ']':
                    if (!wading)
                    {
                        *--q = ']';
                    }
                      wading++ ;
                    break;
                case '[':
                    if (wading)
                    {
                        wading --;
                        if (!wading)
                            *--q = '[';
                    }
                    else
                        done = TRUE;
                    break;
                case ' ':
                case '\t':
                case '\f':
                case '\n':
                    while (pos && isspace(p->cd->text[pos].ch))
                    {
                        pos--;
                    }
                    pos++;
                    break;
                case '>':
                    if (!wading)
                    {
                        if (pos && p->cd->text[pos-1].ch == '-')
                        {
                            *--q = '>';
                            *--q = '-';
                            pos--;
                        }
                        else
                            done = TRUE;
                    }
                    break;
                case '.':
                    if (!wading)
                        *--q = '.';
                    break;
                case '*':
                {
                    int lvl = 0;
                    p1 = q;
                    q-=2;
                    r1 = q;
                    while (*p1 && (*p1 != ')' || lvl))
                    {
                        if (*p1 == '(')
                            lvl++;
                        else if (*p1 == ')')
                            lvl--;
                        *r1++ = *p1++;
                    }
                    *r1++ = '[';
                    *r1++ = ']';
                }
                    break;
                default:
                    if (!wading)
                    {
                        if (!isalnum(p->cd->text[pos].ch) && 
                            p->cd->text[pos].ch != '_')
                            done = TRUE;
                        else
                            *--q = p->cd->text[pos].ch;
                    }
                    break ;
            }
            pos--;
        }
        p1 = r1 = q;
        while (*p1)
        {
            if (*p1 != '(' && *p1 != ')')
                *r1++ = *p1++;
            else
                p1++;
        }
        *r1 = 0;
        *id = 0;
        while (*q)
        {
            int goon = FALSE;
            int indir = 0;
            char name[256], *s = name;
            while (*q && (isalnum(*q) || *q == '_'))
                *s++ = *q++;
            *s = 0;
            if (*id == 0)
            {
                ccFreeStructData(structData);
                structData = NULL;
                if (ccLookupStructType(name, info->dwName, lineno, id, &indir))
                {
                    structData = ccLookupStructElems(info->dwName, *id, indir);
                }
            }
            else
            {
                int i;
                *id = 0;
                for (i= 0; i < structData->fieldCount; i++)
                {
                    if (!stricmp(name, structData->data[i].fieldName))
                    {
                        *id = structData->data[i].subStructId;
                        indir = structData->data[i].indirectCount;
                        break;
                    }
                }
                ccFreeStructData(structData);
                structData = NULL;
                if (*id)
                {
                    structData = ccLookupStructElems(info->dwName, *id, indir);
                }
            }
            if (!structData)
            {
                ccFreeStructData(structData);                
                return NULL;
            }
            while (*q == '[' && *(q+1) == ']')
            {
                q += 2;
                (indir)--;
            }
            if (*q)
                if (*q == '.')
                    q++;
                else if (*q == '-' && *(q+1) == '>')
                {
                    q += 2;
                    (indir)--;
                }
                else
                {
                    ccFreeStructData(structData);                
                    return NULL;
                }
            else
                if (ch == '>')
                    (indir)--;

            if (indir != 0)
            {
                ccFreeStructData(structData);                
                return NULL;
            }
        }
        return structData;
    }
    int showStruct(HWND hwnd, EDITDATA *p, int ch)
    {
        if (instring(p->cd->text, &p->cd->text[p->selstartcharpos]))
            return 0;
        if (ch == '>')
        {
            if (p->selstartcharpos < 2 || p->cd->text[p->selstartcharpos-2].ch != '-')
            {
                return 0;
            }
        }

        if (!codecompleteBox)
        {
            sqlite3_int64 id;
            CCSTRUCTDATA *structData = find_type(hwnd, p, ch, &id);
            if (structData)
            {
                codecompleteBox = CreateWindowEx(WS_EX_TOPMOST| WS_EX_LAYERED | WS_EX_NOACTIVATE, "xcccodeclass","", 
                                       (WS_POPUP) | WS_CLIPCHILDREN,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       hwnd,0, hInstance,0);
                inStructBox = TRUE;
                if (codecompleteBox)
                {
                    int count = 0;
                    int max = 0;
                    SIZE siz1;
                    int width, height;
                    POINT cpos;
                    TEXTMETRIC t;
                    HDC dc = GetDC(codecompleteBox);
                    drawline(hwnd, p, p->selstartcharpos);
                    GetTextMetrics(dc, &t);
                    SendMessage(codecompleteBox, LB_RESETCONTENT, 0, 0); 
                    p->cd->selecting = FALSE;
                    width = 0;
                    for (count =0; count < structData->fieldCount; count++)
                    {
                        int n;
                        char *name = structData->data[count].fieldName;
                        GetTextExtentPoint32(dc, name, strlen(name), &siz1);
                        if (siz1.cx > width)
                            width = siz1.cx;
                        n = SendMessage(codecompleteBox, LB_ADDSTRING, 0, (LPARAM)name);
                    }
                    ccFreeStructData(structData);
                    ReleaseDC(codecompleteBox, dc);
                    if (count > 8)
                        count = 8;
                    if (width < 70)
                        width = 70;
                    if (count)
                    {
                        height = t.tmHeight * count;
                        width += 10 + GetSystemMetrics(SM_CXVSCROLL) + 2 * GetSystemMetrics(SM_CXFRAME);
                        GetCompletionPos(hwnd, p, &cpos, width, height);
                        SendMessage(codecompleteBox, WM_USER, 0, (LPARAM)hwnd);
                        MoveWindow(codecompleteBox, cpos.x, cpos.y, width+4, height+4, TRUE);
                        ShowWindow(codecompleteBox, SW_SHOW);
                        if (IsWindowVisible(hwndShowFunc))
                            ShowWindow(hwndShowFunc, SW_HIDE);
                        return TRUE;
                    }
                    else
                        DestroyWindow(codecompleteBox);
                }
            }
        }
        return FALSE;
    }
    KEYLIST *partialmatchkeyword(KEYLIST *table, int tabsize, char *name)
    {
        int len = strlen(name);
        int top = tabsize;
        int bottom =  - 1;
        int v;
        char *p = name;
        while (*p)
        {
            *p = tolower(*p);
            p++;
        }
        while (top - bottom > 1)
        {
            int mid = (top + bottom) / 2;
            char trans[256];
            p = &trans[0];
            strcpy(trans, table[mid].text);
            while (*p)
            {
                *p = tolower(*p);
                p++;
            }
            v = strncmp(name, trans, len);
            if (v < 0)
            {
                top = mid;
            }
            else
            {
                bottom = mid;
            }
        }
        if (bottom ==  - 1)
            return 0;
        return  &table[bottom];
    }
    char **GetCodeCompKeywords(char *name, char **names, int *size, int *max, HWND hwnd, EDITDATA *p)
    {
        KEYLIST *table = partialmatchkeyword(C_keywordList, sizeof(C_keywordList)/sizeof(C_keywordList[0]), name);
        if (table)
        {
            int len = strlen(name);
            while (C_keywordList != table && !strnicmp(table[-1].text, name, len))
                table--;
            while (!strnicmp(name, table->text, len))
            {
                if (*size >= *max)
                {
                    if (!*max) *max = 10; else *max *= 2;
                    names = realloc(names, *max * sizeof(void *));
                }
                names[(*size)++] = strdup(table->text);
                table++;
            }
        }
        return names;
    }
    char **GetCodeCompVariables(char *name, char **names, int *size, int *max, HWND hwnd, EDITDATA *p)
    {
        names = ccGetMatchingNames(name, names, size, max);
        return names;
    }
    void showVariableOrKeyword(HWND hwnd, EDITDATA *p)
    {
        if (instring(p->cd->text, &p->cd->text[p->selstartcharpos]))
            return;
        if (inStructBox || PropGetInt(NULL, "CODE_COMPLETION") < 2)
            return;
        {
            char buf[4096], *q = buf + sizeof(buf);
            int pos = p->selstartcharpos-1;
            char **names = NULL;
            int size = 0, max = 0;
            *--q = 0;
            if (pos <= 0)
                return;
            while (pos && (isalnum(p->cd->text[pos].ch) || p->cd->text[pos].ch == '_' || p->cd->text[pos].ch == '#'))
                   *--q = p->cd->text[pos--].ch;
            if (codecompleteBox)
            {
                if (strlen(q) == PropGetInt(NULL, "CODE_COMPLETION_VARIABLE_THRESHOLD")
                    || strlen(q) == PropGetInt(NULL, "CODE_COMPLETION_KEYWORD_THRESHOLD"))
                {
                    SendMessage(codecompleteBox, WM_CLOSE, 0, 0);
                    codecompleteBox = NULL;
                }
            }
            if (!codecompleteBox)
            {
                if (strlen(q) >= PropGetInt(NULL, "CODE_COMPLETION_VARIABLE_THRESHOLD"))
                    names = GetCodeCompVariables(q, names, &size, &max, hwnd, p);
                if (strlen(q) >= PropGetInt(NULL, "CODE_COMPLETION_KEYWORD_THRESHOLD"))
                    names = GetCodeCompKeywords(q, names, &size, &max, hwnd, p);
                if (size)
                {
                    codecompleteBox = CreateWindowEx(WS_EX_TOPMOST| WS_EX_LAYERED | WS_EX_NOACTIVATE, "xcccodeclass","", 
                                       (WS_POPUP) | WS_CLIPCHILDREN,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       hwnd,0, hInstance,0);
                    if (codecompleteBox)
                    {
                        int count = 0;
                        int max = 0;
                        SIZE siz1;
                        int width, height;
                        POINT cpos;
                        TEXTMETRIC t;
                        HDC dc = GetDC(codecompleteBox);
                        drawline(hwnd, p, p->selstartcharpos);
                        GetTextMetrics(dc, &t);
                        SendMessage(codecompleteBox, LB_RESETCONTENT, 0, 0); 
                        p->cd->selecting = FALSE;
                        width = 0;
                        for (count =0; count < size; count++)
                        {
                            int n;
                            char *name = names[count];
                            GetTextExtentPoint32(dc, name, strlen(name), &siz1);
                            if (siz1.cx > width)
                                width = siz1.cx;
                            n = SendMessage(codecompleteBox, LB_ADDSTRING, 0, (LPARAM)name);
                            free(name);
                        }
                        free(names);
                        ReleaseDC(codecompleteBox, dc);
                        if (count > 8)
                            count = 8;
                        if (width < 70)
                            width = 70;
                        if (count)
                        {
                            height = t.tmHeight * count;
                            width += 10 + GetSystemMetrics(SM_CXVSCROLL) + 2 * GetSystemMetrics(SM_CXFRAME);
                            GetCompletionPos(hwnd, p, &cpos, width, height);
                            SendMessage(codecompleteBox, WM_USER, 0, (LPARAM)hwnd);
                            SendMessage(codecompleteBox, WM_USER + 2, 0, (LPARAM)q);
                            MoveWindow(codecompleteBox, cpos.x, cpos.y, width+4, height+4, TRUE);
                            ShowWindow(codecompleteBox, SW_SHOW);
                            if (IsWindowVisible(hwndShowFunc))
                                ShowWindow(hwndShowFunc, SW_HIDE);
                            return;
                        }
                        else
                            DestroyWindow(codecompleteBox);
                    }
                }
            }
        }
    }
    void SetFuncWindowSize(HWND hwnd, HFONT normal, HFONT bold, char *proto, 
                           int arg, int max, int *offsets)
    {
        HDC dc = GetDC(hwnd);
        SIZE size1, size2, size3;
        HFONT oldfont;
        RECT r;
        oldfont = SelectObject(dc, normal);
        GetTextExtentPoint32(dc, proto, offsets[arg], &size1);
        SelectObject(dc, bold);
        GetTextExtentPoint32(dc, proto+ offsets[arg], offsets[arg+1]-offsets[arg], 
                             &size2);
        SelectObject(dc, normal);
        GetTextExtentPoint32(dc, proto+ offsets[arg+1], 
                             strlen(proto)- offsets[arg+1], &size3);
        SelectObject(dc, oldfont);
        ReleaseDC(hwnd, dc);
        GetWindowRect(hwnd, &r);
        MoveWindow(hwnd, r.left, r.top, size1.cx + size2.cx + size3.cx 
                   + 4 + 4 * GetSystemMetrics(SM_CXBORDER), 
                   size1.cy + 4 + 2 * GetSystemMetrics(SM_CYBORDER), TRUE);
    }  
    int getFuncPos(EDITDATA *p, char *funcname)
    {
        int l = p->selstartcharpos;
        int slen = strlen(funcname);
        int count = 0;
        if (p->selstartcharpos != p->selendcharpos)
        {
            return INT_MAX;
        }
        while (l >= 0)
        {
            if (funcname[0] == p->cd->text[l].ch)
            {
                int i;
                for (i=0; i < slen; i++)
                {
                    if (funcname[i] != p->cd->text[i+l].ch)
                        break;
                }
                if (i >= slen)
                    break;
            }
            l--;
        }
        if (l < 0)
        {
            return INT_MAX;
        }
        l += slen;
        while (isspace(p->cd->text[l].ch) && l < p->selstartcharpos)
            l++;
        if (p->cd->text[l].ch != '(')
        {
            return INT_MAX;
        }
        l++;
        while (l < p->selstartcharpos)
        {
            if (p->cd->text[l].ch == ',')
                count++, l++;
            else if (p->cd->text[l].ch == '[')
            {
                slen = 0;
                do
                {
                    if (p->cd->text[l].ch == '[')
                        slen++;
                    else if (p->cd->text[l].ch == ']')
                        slen--;
                    l++;
                } while (slen && l < p->selstartcharpos);
            }
            else if (p->cd->text[l].ch == '(')
            {
                slen = 0;
                do
                {
                    if (p->cd->text[l].ch == '(')
                        slen++;
                    else if (p->cd->text[l].ch == ')')
                        slen--;
                    l++;
                } while (slen && l < p->selstartcharpos);				
            }
            else if (p->cd->text[l].ch == ')')
            {
                return INT_MAX;
            }
            else
                l++;
                
        }
        return count;
    }
    static int getColorizeHashKey(char *name)
    {
        unsigned n = 0;
        while (*name)
        {
            n = (n << 7) + (n << 1) + n + *name++;
        }
        return n % COLORIZE_HASH_SIZE;
    }
    static COLORIZE_HASH_ENTRY *lookupColorizeName(COLORIZE_HASH_ENTRY *entries[], char *name)
    {
        int key = getColorizeHashKey(name);
        COLORIZE_HASH_ENTRY *entry = entries[key];
        while (entry)
        {
            if (!strcmp(entry->name, name))
                return entry;
            entry = entry->next;
        }
        return entry;
    }
    static int LookupColorizeEntry(COLORIZE_HASH_ENTRY *entries[], char *name, int line)
    {
        int rv = -1;
        COLORIZE_HASH_ENTRY *entry = lookupColorizeName(entries, name);
        if (entry)
        {
            int lastlargest = 1;
            struct _colorize_lines_entry *lines = entry->lines;
            while (lines)
            {
                if (lines->start >= lastlargest && lines->start <= line && (lines->end == 0 || lines->end >= line))
                {
                    lastlargest = lines->start;
                    rv = lines->type;
                }
                lines = lines->next;
            }
        }
        return rv;
    }
    void InsertColorizeEntry(COLORIZE_HASH_ENTRY *entries[], char *name, int start, int end, int type)
    {
        COLORIZE_HASH_ENTRY *entry = lookupColorizeName(entries, name);
        struct _colorize_lines_entry *lines;
        if (!entry)
        {
            int key = getColorizeHashKey(name);
            entry = (COLORIZE_HASH_ENTRY *)calloc(sizeof(COLORIZE_HASH_ENTRY),1);
            entry->name = strdup(name);
            entry->next = entries[key];
            entries[key] = entry;
        }
        lines = entry->lines;
        while (lines)
        {
            if (lines->start == start && lines->end == end)
                return;
            lines = lines->next;
        }
        lines = calloc(sizeof(struct _colorize_lines_entry),1);
        lines->start = start;
        lines->end = end;
        lines->type = type;
        lines->next = entry->lines;
        entry->lines = lines;
    }
    void FreeColorizeEntries(COLORIZE_HASH_ENTRY *entries[])
    {
        int i;
        for (i=0; i < COLORIZE_HASH_SIZE; i++)
        {
            COLORIZE_HASH_ENTRY *he = entries[i];
            entries[i] = NULL;
            while (he)
            {
                COLORIZE_HASH_ENTRY *next = he->next;
                struct _colorize_lines_entry *le = he->lines;
                while (le)
                {
                    struct _colorize_lines_entry *lnext = le->next;
                    free(le);
                    le = lnext;
                }
                free(he->name);
                free(he);
                he = next;
            }
        }
    }
    LRESULT CALLBACK funcProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
        lParam)
    {
        static char name[256];
        static char proto[4096];
        static int offsets[400];
        static int argcount, currentArg;
        static HFONT normal, bold, oldfont;
        int offset;
        int oldbk, oldfg;
        LOGFONT lf;
        PAINTSTRUCT ps;
        HPEN pen;
        HBRUSH brush;
        HDC dc;
        SIZE size1, size2;
        RECT r;
        switch(iMessage)
        {
            case WM_CREATE:
                lf = systemDialogFont;
                normal = (HFONT)CreateFontIndirect(&lf);
                lf.lfWeight = FW_BOLD;
                bold = CreateFontIndirect(&lf);
                SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
                break;
            case WM_USER:
                strcpy(name, (char *)wParam);
                strcpy(proto, (char *)lParam);
                currentArg = 0;
                SetFuncWindowSize(hwnd, normal, bold, proto, 
                                  currentArg, argcount, offsets);
                break;
            case WM_USER + 1:
                memcpy(offsets, (int *)lParam, (wParam +1 )* sizeof(int));
                argcount = wParam;
                currentArg = 0;
                SetFuncWindowSize(hwnd, normal, bold, proto, 
                                  currentArg, argcount, offsets);
                break;
            case WM_USER + 2:
                currentArg = wParam;
                if (currentArg >= argcount)
                    currentArg = argcount - 1;
                SetFuncWindowSize(hwnd, normal, bold, proto, 
                                  currentArg, argcount, offsets);
                InvalidateRect(hwnd, 0, 0);
                break;
            case WM_USER + 3:
                if (IsWindowVisible(hwnd))
                {
                    switch(wParam)
                    {
                        case VK_UP:
                        case VK_DOWN:
                        case VK_HOME:
                        case VK_END:
                        case VK_PRIOR:
                        case VK_NEXT:
                            ShowWindow(hwnd, SW_HIDE);
                            break;
                        default:
                            currentArg = getFuncPos((EDITDATA *)lParam, name);
                            if (currentArg == INT_MAX)
                            {
                                currentArg = 0;
                                ShowWindow(hwnd, SW_HIDE);
                            }
                            else
                            {
                                if (currentArg >= argcount)
                                    currentArg = argcount-1;
                                InvalidateRect(hwnd, 0, 0);
                            }
                            break;
                    }
                }
                break;
            case WM_PAINT:
                dc = BeginPaint(hwnd, &ps);
                GetClientRect(hwnd, &r);
                brush = CreateSolidBrush(TRANSPARENT_COLOR);
                FillRect(dc, &r, brush);
                DeleteObject(brush);
                
                brush = CreateSolidBrush(RetrieveSysColor(COLOR_INFOBK));
                brush = SelectObject(dc, brush);
                pen = CreatePen(PS_SOLID, 0, 0);
                pen = SelectObject(dc, pen); 
                RoundRect(dc, r.left, r.top, r.right, r.bottom, 5, 5);
                brush = SelectObject(dc, brush);
                DeleteObject(brush);
                
                oldbk = SetBkColor(dc, RetrieveSysColor(COLOR_INFOBK));
                oldfg = SetTextColor(dc, RetrieveSysColor(COLOR_INFOTEXT));
                offset = 2 + 2 * GetSystemMetrics(SM_CXBORDER);
                oldfont = SelectObject(dc, normal);
                GetTextExtentPoint32(dc, proto, offsets[currentArg], &size1);
                TextOut(dc, offset, 3, proto, offsets[currentArg]);
                SelectObject(dc, oldfont);

                oldfont = SelectObject(dc, bold);
                GetTextExtentPoint32(dc, proto+ offsets[currentArg], 
                                     offsets[currentArg+1]-offsets[currentArg], 
                                     &size2);
                TextOut(dc, offset + size1.cx, 3, proto + offsets[currentArg], 
                        offsets[currentArg + 1 ] - offsets[currentArg]);
                SelectObject(dc, oldfont);


                oldfont = SelectObject(dc, normal);
                TextOut(dc, offset + size1.cx + size2.cx, 3, 
                        proto + offsets[currentArg+1], 
                        strlen(proto) - offsets[currentArg + 1]);
                SelectObject(dc, oldfont);
                pen = SelectObject(dc, pen);
                SetTextColor(dc, oldfg);
                SetBkColor(dc, oldbk);
                DeleteObject(pen);
                EndPaint(hwnd, &ps);
                return 0;
        }
        return DefWindowProc(hwnd, iMessage, wParam, lParam);
    }
    void showFunction(HWND hwnd, EDITDATA *p, int ch)
    {
        sqlite3_int64 id, baseid;
        CCPROTODATA *args;
        DWINFO *info = (DWINFO *)GetWindowLong(GetParent(hwnd), 0);
        char name[2046], *q = name;
        int end = p->selstartcharpos;
        int pos = p->selstartcharpos - 1;
        POINT cpos;
        SIZE size;
        int curArg = 0;
        if (instring(p->cd->text, &p->cd->text[p->selstartcharpos]))
            return;
        p->cd->selecting = FALSE;
        if (pos <= 0 || PropGetInt(NULL, "CODE_COMPLETION") == 0)
            return ;
        if (ch == ',' || ch == '(')
        {
            int parenLevel = 0;
            if (ch == ',')
                curArg ++;
            while (((pos && p->cd->text[pos].ch != '(') || parenLevel) && p->cd->text[pos].ch != ';')
            {
                if (p->cd->text[pos].ch == ',')
                    curArg++;
                if (p->cd->text[pos].ch == ')')
                    parenLevel++;
                if (p->cd->text[pos].ch == '(')
                    parenLevel--;
                pos--;
            }
            if (!pos || p->cd->text[pos].ch != '(')
                return;
            end = pos;
            pos--;
            
        }
        while (pos && isspace(p->cd->text[pos].ch))
        {
            pos--;
            end--;
        }
        while (pos && (isalnum(p->cd->text[pos].ch)
                || p->cd->text[pos].ch == '_'))
            pos--;
        pos++;
        while (pos < end)
            *q++ = p->cd->text[pos++].ch;
        *q = 0;
        if (ccLookupFunctionType(name, info->dwName, &id, &baseid) && (args = ccLookupArgList(id, baseid)))
        {
            HDC dc;
            int offsets[400];
            char proto[4096];
            int count =0;
            sprintf(proto, "%s %s(", args->baseType, name);
            for (count = 0; count < args->argCount; count++)
            {
                offsets[count] = strlen(proto);
                if (args->data[count].fieldName[0] == '{') // unnamed...
                    sprintf(proto + strlen(proto), "%s, ", args->data[count].fieldType);
                else                    
                    sprintf(proto + strlen(proto), "%s %s, ", args->data[count].fieldType, args->data[count].fieldName);
            }
            if (count)
                proto[strlen(proto)-2] = 0;
            offsets[count] = strlen(proto);
            strcat(proto, ")");
            ccFreeArgList(args);            
            if (!hwndShowFunc)
            {
                hwndShowFunc = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE, "xccfuncclass", "",
                                            (WS_POPUP),
                                            CW_USEDEFAULT, CW_USEDEFAULT,
                                            CW_USEDEFAULT, CW_USEDEFAULT,
                                            hwnd, 0, GetModuleHandle(0), 0);
            }
            SendMessage(hwndShowFunc, WM_USER, (WPARAM)name, (LPARAM)proto);
            SendMessage(hwndShowFunc, WM_USER + 1, count, (LPARAM)offsets);
            dc = GetDC(hwndShowFunc);
            GetTextExtentPoint32(dc, proto, strlen(proto), &size);
            ReleaseDC(hwndShowFunc, dc);
            GetCompletionPos(hwnd, p, &cpos, size.cx, size.cy);
//						SetParent(codecompleteBox, hwnd);
            MoveWindow(hwndShowFunc, cpos.x, cpos.y, size.cx, size.cy, TRUE);
            SendMessage(hwndShowFunc, WM_USER + 2, curArg, 0);
            ShowWindow(hwndShowFunc, SW_SHOW);
            SetFocus(hwnd);
        }
    }
    static BOOL multilineSelect(EDITDATA *p)
    {
        int st = p->selstartcharpos;
        int en = p->selendcharpos;
        if (st == en)
            return FALSE;
        if (en < st)
        {
            int temp = st;
            st = en;
            en = temp;
        }
        while (st < en)
            if (p->cd->text[st++].ch == '\n')
                return TRUE;
        return FALSE;
    }
    /**********************************************************************
     * exeditProc is the window proc for the edit control
     **********************************************************************/

    LRESULT CALLBACK exeditProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
        lParam)
    {
        CREATESTRUCT *lpCreate;
        static char buffer[1024];
        EDITDATA *p;
        DWINFO *x;
        int stop;
        LRESULT rv;
        int i, start, end;
        TOOLTIPTEXT *lpnhead;
        int l;
        DEBUG_INFO *dbg;
        VARINFO *var;
        NMHDR nm;
        int charpos;
        RECT r;
        HDC dc;
        HFONT xfont;
        TEXTMETRIC t;
        INTERNAL_CHAR *ic;
        POINTL pt;
        LOGFONT lf;
        CHARRANGE *ci;
        switch (iMessage)
        {
            case WM_TIMER:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                KillTimer(hwnd, p->cd->reparseTimerID);
                p->cd->reparseTimerID = 0;
                InstallForParse(GetParent(hwnd));
                break;
            case WM_ERASEBKGND:
                return 0;
            case WM_PAINT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->colorizing)
                    EditPaint(hwnd, p);
                return 0;
            case EM_GETINSERTSTATUS:
                return ((EDITDATA*)GetWindowLong(hwnd, 0))->cd->inserting;
            case WM_CODECOMPLETE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (wParam == '.' || wParam == '>')
                    showStruct(hwnd, p, wParam);
                else
                    showVariableOrKeyword(hwnd, p);
                break;
            case WM_SHOWFUNCTION:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                showFunction(hwnd, p, wParam);
                break;
            case WM_NOTIFY:
                lpnhead = (TOOLTIPTEXT*)lParam;
                if (lpnhead->hdr.code == NM_CUSTOMDRAW)
                {
                    LPNMTTCUSTOMDRAW lpnmCustomDraw = (LPNMTTCUSTOMDRAW)lParam;
                }
                else switch (lpnhead->hdr.code)
                {
                case TTN_SHOW:
                case TTN_POP:
                    break;
                case TTN_NEEDTEXT:
                    // tooltip
                    if ((uState == notDebugging && PropGetBool(NULL, "EDITOR_HINTS"))
                        || ((uState == atException || uState == atBreakpoint) && PropGetBool(NULL, "DEBUGGER_HINTS")))
                    {
                        char name[1000];
                        DWINFO *info = (DWINFO *)GetWindowLong(GetParent(hwnd), 0);
                        int lineno;
                        POINT pt;
                        name[0] = 0;
                        p = (EDITDATA*)GetWindowLong(hwnd, 0);				
                        GetCursorPos(&pt);
                        ScreenToClient(hwnd, &pt);
                        charpos = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM) &pt);
                           GetWordFromPos(hwnd, name,  charpos, &lineno, &start, &end);
                        lineno++;
                        if (name[0])
                        {
                            x = (DWINFO*)GetWindowLong(GetParent(hwnd), 0);
                            if (uState == notDebugging)
                            {
                                int flags;
                                if (ccLookupType(buffer, name, info->dwName, lineno, & flags))
                                {
                                    if (strncmp(buffer, "(*)", 3) != 0)
                                    {
                                        strcat(buffer, " ");
                                        strcat(buffer, name);
                                        if (flags & 1)
                                            strcat(buffer, "()");
                                        strcpy(lpnhead->lpszText, buffer);
                                    }
                                }
                            }
                            else
                            {
                                var = EvalExpr(&dbg, activeScope, name, FALSE);
                                if (var)
                                {
                                    sprintf(buffer, "%s: ", name);
                                    HintValue(dbg, var, buffer + strlen(buffer));
                                    strcpy(lpnhead->lpszText, buffer);
                                    FreeVarInfo(var);
                                }
                            }
                        }
                    }
                    break;
                }
                break;
            case WM_COMMAND:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->cd->selecting = FALSE;
                if (LOWORD(wParam) == IDM_RTLHELP)
                    DoHelp(hwnd, 0);
                if (LOWORD(wParam) == IDM_SPECIFIEDHELP)
                    DoHelp(hwnd, 1);
                if (LOWORD(wParam) == IDM_CLOSEFIND)
                {
                    if (IsWindowVisible(hwndShowFunc))
                        ShowWindow(hwndShowFunc, SW_HIDE);
                    if (IsWindow(codecompleteBox))
                        SendMessage(codecompleteBox, WM_CLOSE, 0, 0);
                }
                break;
            case WM_KEYDOWN:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                CancelParenMatch(hwnd, p);
                switch (wParam)
                {
                case VK_ESCAPE:
                    SendMessage(hwnd, WM_COMMAND, IDM_CLOSEFIND, 0);
                    break;
                case VK_INSERT:
                    if (!p->cd->readonly)
                    {
                        if (GetKeyState(VK_SHIFT) &0x80000000)
                        {
                            SendMessage(hwnd, WM_PASTE, 0, 0);
                        }
                        else if (GetKeyState(VK_CONTROL) &0x80000000)
                        {
                            SendMessage(hwnd, WM_COPY, 0, 0);
                        }
                        else
                        {
                            p->cd->inserting = !p->cd->inserting;
                            if (p->hasfocus)
                            {
                                SendMessage(hwnd, WM_KILLFOCUS, 0, 0);
                                SendMessage(hwnd, WM_SETFOCUS, 0, 0);
                            }
                            SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                        }
                    }
                    break;
                case VK_BACK:
                    if (p->selstartcharpos == p->selendcharpos)
                    {
                        if (p->selstartcharpos == 0)
                            break;
                        left(hwnd, p);
                    }
                case VK_DELETE:
                    if (!p->cd->readonly)
                    {
                        if (GetKeyState(VK_SHIFT) &0x80000000)
                        {
                            SendMessage(hwnd, WM_CUT, 0, 0);
                        }
                        else if (p->selstartcharpos != p->selendcharpos)
                        {
                            Replace(hwnd, p, "", 0);
                            ScrollCaretIntoView(hwnd, p, FALSE);
                        }
                        else
                        {
                            removechar(hwnd, p, wParam == VK_DELETE ?
                                UNDO_DELETE : UNDO_BACKSPACE);
                            FormatBufferFromScratch(p->colorizeEntries, p->cd->text, p
                                ->selstartcharpos, p->selstartcharpos, p
                                ->cd->language, p->cd->defbackground);
                        }
                        setcurcol(p);
                    }
                    break;
                case VK_RETURN:
                    if (!p->cd->readonly)
                    {
                        insertcr(hwnd, p, TRUE);
                        FormatBufferFromScratch(p->colorizeEntries, p->cd->text, p->selstartcharpos
                            - 1, p->selstartcharpos, p->cd->language, p->cd->defbackground);
                        setcurcol(p);
                        if (IsWindowVisible(hwndShowFunc))
                            scrollup(hwnd, p, 1);
                        }
                    
                    break;
                case VK_TAB:
                    if (GetKeyState(VK_SHIFT) &0x80000000)
                    {
                        if (!p->cd->readonly)
                        {
                            if (multilineSelect(p))
                                SelectIndent(hwnd, p, FALSE);
                            else
                            {
                                backtab(hwnd, p);
                                drawline(hwnd, p, p->selstartcharpos);
                            }
                        }
                        else
                            left(hwnd, p);
                        setcurcol(p);
                    }
                    else
                    {
                        if (!p->cd->readonly)
                        {
                            if (multilineSelect(p))
                                SelectIndent(hwnd, p, TRUE);
                            else
                            {
                                inserttab(hwnd, p);
                                InsertEndTabs(hwnd, p, FALSE);
                                drawline(hwnd, p, p->selstartcharpos);
                            }
                        }
                        else
                            right(hwnd, p);
                        setcurcol(p);
                    }
                    break;
                case VK_DOWN:
                    upline(hwnd, p, 1);
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                    break;
                case VK_UP:
                    upline(hwnd, p,  - 1);
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                    break;
                case VK_PRIOR:
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        VScrollPos(hwnd, 0, TRUE);
                        p->textshowncharpos = p->selendcharpos = 0;
                        if (!p->cd->selecting)
                            p->selstartcharpos = p->selendcharpos;
                        MoveCaret(hwnd, p);
                        InvalidateRect(hwnd, 0, 0);
                    }
                    else
                    {
                        ClientArea(hwnd, p, &r);
                        i = r.bottom / p->cd->txtFontHeight;
                        upline(hwnd, p, 2-i);
                    }
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);

                    break;
                case VK_NEXT:
                    ClientArea(hwnd, p, &r);
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        i = lfchars(p->cd->text, p->textshowncharpos, p->cd->textlen);
                        upline(hwnd, p, i);
                    }
                    else
                    {
                        i = r.bottom / p->cd->txtFontHeight;
                        upline(hwnd, p, i - 2);
                    }
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                    break;
                case VK_END:
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        i = lfchars(p->cd->text, p->textshowncharpos, p->cd->textlen);
                        upline(hwnd, p, i);
                    }
                    else
                    {
                        endofline(hwnd, p);
                        setcurcol(p);
                    }
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                    break;
                case VK_HOME:
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        VScrollPos(hwnd, 0, TRUE);
                        p->textshowncharpos = 0;
                        if (p->cd->selecting)
                            p->selstartcharpos = 0;
                        else
                            p->selstartcharpos = p->selendcharpos = 0;
                        MoveCaret(hwnd, p);
                        SendUpdate(hwnd);
                        InvalidateRect(hwnd, 0, 0);
                    }
                    else
                    {
                        sol(hwnd, p);
                        setcurcol(p);
                    }
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                    break;
                case VK_LEFT:
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                        leftword(hwnd, p);
                    else
                        left(hwnd, p);
                    setcurcol(p);
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                    break;
                case VK_RIGHT:
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                        rightword(hwnd, p);
                    else
                        right(hwnd, p);
                    setcurcol(p);
                    SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                    break;
                case 'A':
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        p->selstartcharpos = 0;
                        p->selendcharpos = p->cd->textlen;
                        InvalidateRect(hwnd, 0, 0);
                        }
                    break;
                case 'X':
                    if (!p->cd->readonly && GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        SendMessage(hwnd, WM_CUT, 0, 0);
                    }
                    break;
                case 'C':
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        SendMessage(hwnd, WM_COPY, 0, 0);
                    }
                    break;
                case 'V':
                    if (!p->cd->readonly && GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        SendMessage(hwnd, WM_PASTE, 0, 0);
                    }
                    break;
                case 'Y':
                    if (!p->cd->readonly)
                        if (GetKeyState(VK_CONTROL) &0x80000000)
                        {
                            SendMessage(hwnd, WM_REDO, 0, 0);
                        }
                        break;
                case 'Z':
                    if (!p->cd->readonly)
                        if (GetKeyState(VK_CONTROL) &0x80000000)
                        {
                            SendMessage(hwnd, WM_UNDO, 0, 0);
                        }
                    break;
                case 'S':
                    if (!p->cd->readonly && GetKeyState(VK_CONTROL) &0x80000000)
                    {
                        if (p->cd->modified)
                            SendMessage(GetParent(hwnd), WM_COMMAND, IDM_SAVE,
                                0);
                    }
                    break;
                case 'T':
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                        verticalCenter(hwnd, p);
                    break;
                case 'L':
                    if (!(GetKeyState(VK_CONTROL) &0x80000000))
                        break;
                    if (!p->cd->readonly && p->cd->inserting)
                    {
                        insertautoundo(hwnd, p, UNDO_AUTOEND);
                        insertchar(hwnd,p , '\f');
                        insertcr(hwnd,p,FALSE);	
                        insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
                    }
                    break;
                case 'W':
                    if (!(GetKeyState(VK_CONTROL) &0x80000000))
                        break;
                    InstallForParse(GetParent(hwnd));
                    break ;
                case VK_SHIFT:
                    p->cd->selecting = TRUE;
                    break;
                }
                if (p->cd->selecting)
                    InvalidateRect(hwnd, 0, 0);
                PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0);
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                SendMessage(hwndShowFunc, WM_USER+3, wParam, (LPARAM)p);
                FindParenMatch(hwnd, p);
                break;
            case WM_KEYUP:
                if (wParam == VK_SHIFT)
                {
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    p->cd->selecting = FALSE;
                    break;
                }
                break;
            case WM_CHAR:
                if (wParam >= ' ' && wParam < 256)
                {
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    CancelParenMatch(hwnd, p);
                    if (!p->cd->readonly)
                    {
                        insertchar(hwnd, p, wParam);
                        if (wParam == '.' || wParam == '>' || isalnum(wParam) || wParam == '_')
                        {
                            PostMessage(hwnd, WM_CODECOMPLETE, wParam, p->selstartcharpos);
                        }
                        else if (wParam == '(' || wParam == ',')
                        {
                            PostMessage(hwnd, WM_SHOWFUNCTION, wParam, 0);
                        }
                        if (wParam == '{')
                            InsertBeginTabs(hwnd, p);
                        else if (wParam == ' ' || wParam == '}')
                            InsertEndTabs(hwnd, p, wParam == '}');
                        else if (wParam == '#')
                            DeletePound(hwnd, p);
                        if (!p->cd->inserting)
                            SendMessage(hwnd, WM_CLEAR, 0, 0);
                        FormatLine(hwnd, p->cd->text, p->cd->language, p->cd->defbackground);
                        drawline(hwnd, p, p->selstartcharpos);
                    }
                    else
                        right(hwnd, p);
                    FindParenMatch(hwnd, p);
                    setcurcol(p);
                    PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0)
                        ;
                    break;
                }
                break;
            case WM_SETFOCUS:			
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->hasfocus = TRUE;
                p->hiddenCaret = TRUE;
                CreateCaret(hwnd, 0, p->cd->inserting ? 2 : p->cd->txtFontWidth, p
                    ->cd->txtFontHeight);
                MoveCaret(hwnd, p);
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_SETFOCUS |
                    (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
                break;
            case WM_KILLFOCUS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->hasfocus = FALSE;
                DestroyCaret();
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_KILLFOCUS 
                    | (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
                break;
            case WM_SETTEXT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                freemem(p);
                p->cd->text = 0;
                p->cd->textlen = 0;
                p->cd->textmaxlen = 0;
                if (p->cd->language)
                    SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, p->cd->readonly ? C_READONLYBACKGROUND : C_BACKGROUND);
                i = strlen((char*)lParam) ;
                
                if (allocmem(p, i * 5) && commitmem(p, i+1))
                {
                    p->cd->textlen = i;
                    for (i = 0; i < p->cd->textlen; i++)
                    {
                        p->cd->text[i].ch = ((char*)lParam)[i];
                        p->cd->text[i].Color = (p->cd->defbackground << 5) + p->cd->defforeground;
                    }
                    FullColorize(hwnd, p, TRUE);
                }
                if (wParam)
                {
                    if (p->textshowncharpos >= p->cd->textlen)
                        p->textshowncharpos = p->selstartcharpos = p->selendcharpos = 0;
                    if (p->selstartcharpos > p->cd->textlen)
                        p->selstartcharpos = p->selendcharpos = 0;
                    if (p->selendcharpos > p->cd->textlen)
                        p->selstartcharpos = p->selendcharpos = 0;
                }
                else
                {
                    p->selstartcharpos = p->selendcharpos = p->textshowncharpos = 0;
                }
                break;
            case EM_GETEDITDATA:
                return (LRESULT) (EDITDATA*)GetWindowLong(hwnd, 0);
            case EM_UPDATESIBLING:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (lParam <= p->textshowncharpos)
                {
                    if (wParam > 0)
                        p->textshowncharpos += wParam;
                    else
                        if (lParam - wParam < p->textshowncharpos)
                            p->textshowncharpos += wParam;
                        else
                            p->textshowncharpos -= p->textshowncharpos - lParam;
                }
                if (lParam <= p->selstartcharpos)
                    p->selstartcharpos += wParam;
                if (lParam <= p->selendcharpos)
                    p->selendcharpos += wParam;
                InvalidateRect(hwnd, 0, 0);
                break ;
            case EM_LOADLINEDATA:
                if (PropGetInt(NULL, "CODE_COMPLETION") != 0)
                {
                   p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    free(p->cd->lineData);
                    p->cd->lineData = NULL;
                    p->cd->lineDataMax = 0;
                    if (!p->cd->lineData)
                    {
                        HWND t = GetParent(p->self);
                        DWINFO *x = (DWINFO *)GetWindowLong(t, 0);
                        p->cd->lineData = ccGetLineData(x->dwName, &p->cd->lineDataMax);
                    }
                    FreeColorizeEntries(p->colorizeEntries);
                    {
                        HWND t = GetParent(p->self);
                        DWINFO *x = (DWINFO *)GetWindowLong(t, 0);
                        ccGetColorizeData(x->dwName, p->colorizeEntries);
                        FullColorize(hwnd, p, FALSE);
                    }
                }
                break;
            case WM_CREATE:
                p = (EDITDATA*)calloc(1,sizeof(EDITDATA));
                SetWindowLong(hwnd, 0, (int)p);
                p->self = hwnd;
                lpCreate = (LPCREATESTRUCT)lParam;
                if (lpCreate->lpCreateParams)
                {
                    p->cd = (COREDATA *)((EDITDATA *)(lpCreate->lpCreateParams))->cd;
                }
                else 
                {
                    LOGFONT editFont;
                    OSVERSIONINFO osvi;
                    p->cd = (COREDATA*)calloc(1,sizeof(COREDATA));
                    p->cd->inserting = TRUE;
                    
                    memcpy(&editFont, &EditFont, sizeof(editFont));
                    memset(&osvi,0,sizeof(osvi));
                    osvi.dwOSVersionInfoSize = sizeof(osvi);
                    GetVersionEx(&osvi);
                    if (osvi.dwMajorVersion >= 6)
                        strcpy(editFont.lfFaceName, "Consolas");
                    PropGetFont(NULL, "FONT", &editFont);

                    xfont = CreateFontIndirect(&editFont);
                    SendMessage(hwnd, WM_SETFONT, (WPARAM)xfont, 0);
                    p->cd->defbackground = p->cd->readonly ? C_READONLYBACKGROUND : C_BACKGROUND;
                    p->cd->defforeground = C_TEXT;
                    p->cd->hbrBackground = CreateSolidBrush(backgroundColor);
                    p->cd->tabs = PropGetInt(NULL, "TAB_INDENT");
                    p->cd->leftmargin = EC_LEFTMARGIN;
                    
                    getPageSize() ;
                    if (allocmem(p, page_size * 20) && commitmem(p, page_size))
                    {
                        p->cd->textlen = 0;
                        for (i = 0; i < p->cd->textlen; i++)
                        {
                            p->cd->text[i].Color = (p->cd->defbackground << 5) + p->cd->defforeground;
                        }
                    }
                }
                p->tooltip = CreateTTIPWindow(hwnd, 0);
                {
                    EDLIST *l = (EDLIST*)calloc(1, sizeof(EDLIST));
                    if (l)
                    {
                        EDLIST **l1 = &p->cd->siblings;
                        while (*l1) 
                        {
                            if (p->id <= (*l1)->data->id)
                                p->id = (*l1)->data->id + 1;
                            l1 = &(*l1)->next ;
                        }
                        *l1 = l ;
                        l->data = p;
                    }
                }
                PostMessage(hwnd, EM_LOADLINEDATA, 0, 0);
                SetScrollRange(hwnd, SB_HORZ, 0, MAX_HSCROLL, TRUE);
                break;
            case WM_SETEDITORSETTINGS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (p->cd->language)
                    SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, p->cd->readonly ? C_READONLYBACKGROUND : C_BACKGROUND);
                p->cd->defbackground = p->cd->readonly ? C_READONLYBACKGROUND : C_BACKGROUND;
                if (p->cd->language)
                {
                    stop = p->cd->tabs *4;
                    SendMessage(hwnd, EM_SETTABSTOPS, 1, (LPARAM) &stop);
                    Colorize(p->cd->text, 0, p->cd->textlen, (p->cd->defbackground << 5) + p->cd->defforeground, FALSE);
                    FullColorize(hwnd, p, FALSE);
                }
                InvalidateRect(hwnd, 0, 1);
                return 0;
            case WM_DESTROY:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                while (p->cd->colorizing) Sleep(10);
                DestroyWindow(p->tooltip);
                {
                    EDLIST **l = &p->cd->siblings;
                    while (*l)
                    {
                        if ((*l)->data == p)
                        {
                            EDLIST *l2 = (*l) ;
                            (*l) = (*l)->next;
                            free((void *)l2);
                            break ;
                        }
                        l = &(*l)->next;
                    }
                    if (!(p->cd->siblings))
                    {	
                        for (i = 0; i < UNDO_MAX; i++)
                            free(p->cd->undolist[i].data);
                        DeleteObject(p->cd->hFont);
                        DeleteObject(p->cd->hItalicFont);
                        DeleteObject(p->cd->hBoldFont);
                        DeleteObject(p->cd->hbrBackground);
                        freemem(p);
                        free(p->cd->lineData);
                        free((void*)p->cd);
                    }
                }
                FreeColorizeEntries(&p->colorizeEntries);
                free((void*)p);
                break;
            case WM_VSCROLL:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_VSCROLL | 
                    (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
                switch (LOWORD(wParam))
                {
                case SB_BOTTOM:
                    SendMessage(hwnd, WM_KEYDOWN, VK_NEXT, 0);
                    break;
                case SB_TOP:
                    SendMessage(hwnd, WM_KEYDOWN, VK_PRIOR, 0);
                    break;
                case SB_LINEDOWN:
                    scrollup(hwnd, p, 1);
                    InvalidateRect(hwnd, 0, 0);
                    MoveCaret(hwnd, p);
                    break;
                case SB_LINEUP:
                    scrollup(hwnd, p,  - 1);
                    InvalidateRect(hwnd, 0, 0);
                    MoveCaret(hwnd, p);
                    break;
                case SB_PAGEDOWN:
                    ClientArea(hwnd, p, &r);
                    i = r.bottom / p->cd->txtFontHeight;
                    scrollup(hwnd, p, i - 1);
                    InvalidateRect(hwnd, 0, 0);
                    MoveCaret(hwnd, p);
                    break;
                case SB_PAGEUP:
                    ClientArea(hwnd, p, &r);
                    i = r.bottom / p->cd->txtFontHeight;
                    scrollup(hwnd, p, 1-i);
                    InvalidateRect(hwnd, 0, 0);
                    MoveCaret(hwnd, p);
                    break;
                case SB_ENDSCROLL:
                    return 0;
                case SB_THUMBPOSITION:
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    TrackVScroll(hwnd, p, TRUE);
                    break;
                case SB_THUMBTRACK:
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    TrackVScroll(hwnd, p, FALSE);
                    break;
                default:
                    return 0;
                }
                InvalidateRect(GetParent(hwnd), 0, 0);
                return 0;
            case WM_HSCROLL:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_HSCROLL | 
                    (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
                switch (LOWORD(wParam))
                {
                case SB_LEFT:
                    p->leftshownindex = 0;
                    break;
                case SB_RIGHT:
                    ClientArea(hwnd, p, &r);
                    i = r.right / p->cd->txtFontWidth;
                    p->leftshownindex = MAX_HSCROLL - i;
                    break;
                case SB_LINELEFT:
                    scrollleft(hwnd, p,  - 1);
                    break;
                case SB_LINERIGHT:
                    scrollleft(hwnd, p, 1);
                    break;
                case SB_PAGERIGHT:
                    ClientArea(hwnd, p, &r);
                    i = r.right / p->cd->txtFontWidth;
                    scrollleft(hwnd, p, i - 1);
                    break;
                case SB_PAGELEFT:
                    ClientArea(hwnd, p, &r);
                    i = r.right / p->cd->txtFontWidth;
                    scrollleft(hwnd, p, 1-i);
                    break;
                case SB_ENDSCROLL:
                    return 0;
                case SB_THUMBPOSITION:
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    TrackHScroll(hwnd, p, TRUE);
                    InvalidateRect(hwnd, 0, 0);
                    MoveCaret(hwnd, p);
                    return 0;
                case SB_THUMBTRACK:
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    TrackHScroll(hwnd, p, FALSE);
                    InvalidateRect(hwnd, 0, 0);
                    MoveCaret(hwnd, p);
                    return 0;
                }
                SetScrollPos(hwnd, SB_HORZ, p->leftshownindex, TRUE);
                SendUpdate(hwnd);
                InvalidateRect(hwnd, 0, 0);
                MoveCaret(hwnd, p);
                return 0;
            case WM_MOUSEWHEEL:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                i = PropGetInt(NULL, "MOUSEWHEEL_SCROLL");
                if (i <=0 )
                    i = 1;
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_VSCROLL | 
                    (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
                p->wheelIncrement += GET_WHEEL_DELTA_WPARAM(wParam);
                if (p->wheelIncrement < 0)
                {
                    while (p->wheelIncrement <= - WHEEL_DELTA)
                    {
                        scrollup(hwnd, p,  1);
                        p->wheelIncrement += WHEEL_DELTA/i;
                    }
                }
                else
                {
                    while (p->wheelIncrement >= WHEEL_DELTA)
                    {
                        scrollup(hwnd, p,  -1);
                        p->wheelIncrement -= WHEEL_DELTA/i;
                    }
                }
                InvalidateRect(hwnd, 0, 0);
                SendUpdate(hwnd);
                MoveCaret(hwnd, p);
                InvalidateRect(GetParent(hwnd), 0, 0);
                break;
            case WM_MOUSEMOVE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                ClientArea(hwnd, p, &r);
                pt.x = LOWORD(lParam);
                pt.y = (int)(short)HIWORD(lParam);
                if (p->cd->buttondown)
                {
                    if ((signed)pt.y < 0)
                        StartAutoScroll(hwnd, p,  - 1);
                    else if (pt.y > r.bottom)
                    {
                        StartAutoScroll(hwnd, p, 1);
                    }
                    else
                    {
                        EndAutoScroll(hwnd, p);
                        p->selendcharpos = charfrompos(hwnd, p, &pt);
                        InvalidateRect(hwnd, 0, 0);
                    }
                    MoveCaret(hwnd, p);
               }
                SendMessage(hwndShowFunc, WM_USER+3, 0, (LPARAM)p);
                return 0;
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (GetFocus() != hwnd)
                    SendMessage(GetParent(hwnd), WM_COMMAND, EN_NEEDFOCUS, 0);
//                    SetFocus(hwnd);
                nm.code = NM_RCLICK;
                SendMessage(GetParent(hwnd), WM_NOTIFY, 0, (LPARAM) &nm);
                return 0;
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return 0;
            case WM_LBUTTONDOWN:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                CancelParenMatch(hwnd, p);
                if (GetFocus() != hwnd)
                    SendMessage(GetParent(hwnd), WM_COMMAND, EN_NEEDFOCUS, 0);
//                    SetFocus(hwnd);
                p->cd->selecting = TRUE;
                if (p->selstartcharpos != p->selendcharpos)
                {
                    InvalidateRect(hwnd, 0, 0);
                }
                p->cd->buttondown = TRUE;
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);
                if (GetKeyState(VK_SHIFT) &0x80000000)
                {
                    int n = charfrompos(hwnd, p, &pt);
                    if (n != p->selstartcharpos)
                    {
                        if (n < p->selstartcharpos)
                        {
                            p->selstartcharpos = n;
                        }
                        else
                        {
                            p->selendcharpos = n;
                        }
                        while (p->selstartcharpos && p->cd->text[p->selstartcharpos-1].ch != '\n')
                            p->selstartcharpos--;
                        while (p->cd->text[p->selendcharpos].ch && p->cd->text[p->selendcharpos+1].ch != '\n')
                            p->selendcharpos++;
                        if (p->cd->text[p->selendcharpos].ch)
                            p->selendcharpos++;
                    }
                }
                else
                {

                    p->selstartcharpos = p->selendcharpos = charfrompos(hwnd, p,
                        &pt);
                }
                MoveCaret(hwnd, p);
                setcurcol(p);
                SetCapture(hwnd);
                FindParenMatch(hwnd, p);
                break;
            case WM_LBUTTONUP:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (p->cd->buttondown)
                {
                    p->cd->selecting = !!(GetKeyState(VK_SHIFT) &0x80000000);
                    p->cd->buttondown = FALSE;
                    ReleaseCapture();
                    EndAutoScroll(hwnd, p);
                }
                SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                break;
            case WM_LBUTTONDBLCLK:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                /* have the pos from the lbutton down */
                HilightWord(hwnd, p);
                drawline(hwnd, p, 0);
                break;
            case WM_WORDUNDERCURSOR:
                   return GetWordFromPos(hwnd, (char*)lParam,  - 1, 0, 0, 0);
            case WM_WORDUNDERPOINT:
            {
                int charpos, start, end, linepos;
                POINT pt = *(POINT *)wParam;
                charpos = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM) &pt);
                return GetWordFromPos(hwnd, (char *)lParam, charpos, &linepos, &start, &end);
            }
                break;
            case EM_SETBKGNDCOLOR:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (wParam)
                {
                    p->cd->defbackground = C_SYS_WINDOWBACKGROUND;
                }
                else
                {

                    p->cd->defbackground = lParam;
                }
                DeleteObject(p->cd->hbrBackground);
                p->cd->hbrBackground = CreateSolidBrush(colors[lParam][0]);
                InvalidateRect(hwnd, 0, 1);
                break;
            case WM_GETTEXT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->textlen)
                    return 0;
                i = p->cd->textlen;
                if (wParam <= i)
                    i = wParam - 1;
                {
                    INTERNAL_CHAR *x = p->cd->text;
                    while (i--)
                    {
                        *((char*)lParam) = x++->ch;
                        lParam += sizeof(char);
                    }
                    *(char*)lParam = 0;
                }
                return i;
            case WM_GETTEXTLENGTH:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return p->cd->textlen + 1;
            case EM_EXSETSEL:
                ci = (CHARRANGE*)lParam;
                wParam = ci->cpMin;
                lParam = ci->cpMax;
                // fall through
            case EM_SETSEL:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (wParam == 0 && lParam ==  - 1)
                {
                    p->selstartcharpos = 0;
                    p->selendcharpos = p->cd->textlen;
                }
                else if (wParam ==  - 1)
                {
                    p->selendcharpos = p->selstartcharpos;
                }
                else
                {
                    if ((int)wParam < 0)
                        wParam = 0;
                    if (wParam > p->cd->textlen)
                        wParam = p->cd->textlen;
                    if ((int)lParam < 0)
                        lParam = 0;
                    if (lParam > p->cd->textlen)
                        lParam = p->cd->textlen;
                    p->selstartcharpos = wParam;
                    p->selendcharpos = lParam;
                }
                if (!p->cd->nosel)
                {
                    //            VScrollPos(hwnd,LineFromChar(p,p->selstartcharpos),TRUE) ;
                    ScrollCaretIntoView(hwnd, p, FALSE);
                    InvalidateRect(hwnd, 0, 0);
                }
                return 0;
            case EM_GETSEL:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (wParam)
                    *(int*)wParam = p->selstartcharpos;
                if (lParam)
                    *(int*)lParam = p->selendcharpos;
                return  - 1;
            case EM_EXGETSEL:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                ((CHARRANGE*)lParam)->cpMin = p->selstartcharpos;
                ((CHARRANGE*)lParam)->cpMax = p->selendcharpos;
                return 0;
            case EM_HIDESELECTION:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->cd->nosel = wParam;
                if (p->selstartcharpos != p->selendcharpos)
                    InvalidateRect(hwnd, 0, 0);
                return 0;
            case EM_REPLACESEL:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                Replace(hwnd, p, (char*)lParam, strlen((char*)lParam));
                return 0;
            case EM_SCROLLCARET:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                ScrollCaretIntoView(hwnd, p, lParam);
                return 0;
            case EM_LINEFROMCHAR:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return LineFromChar(p, wParam);
            case EM_EXLINEFROMCHAR:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return LineFromChar(p, lParam);
            case EM_LINEINDEX:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                rv = 0;
                ic = p->cd->text;
                while (ic < p->cd->text + p->cd->textlen && wParam)
                {
                    if (ic->ch == '\n')
                        wParam--;
                    ic++;
                    rv++;
                }
                return rv;
            case EM_GETLINE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                rv = 0;
                ic = p->cd->text;
                while (ic < p->cd->text + p->cd->textlen && wParam)
                {
                    if (ic->ch == '\n')
                        wParam--;
                    ic++;
                    rv++;
                }
                if (ic >= p->cd->text + p->cd->textlen)
                    return 0;
                l = *(short*)lParam;
                for (i = 0; i < l && ic < p->cd->text + p->cd->textlen; i++, ic++)
                {
                    *(char*)(lParam + i) = ic->ch;
                    if (ic->ch == '\n')
                    {
                        break;
                    }
                }
                *(char*)(lParam + i) = 0;

                return i;
            case EM_CHARFROMPOS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return charfrompos(hwnd, p, (POINTL*)lParam);
            case EM_POSFROMCHAR:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                {
                    posfromchar(hwnd, p, (POINTL*)wParam, lParam);
                    return 0;
                }

            case EM_GETLINECOUNT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                rv = 0;
                if (!p->cd->text)
                        return 0;
                ic = p->cd->text + 1;
                while (ic->ch)
                {
                    if (ic->ch == '\n')
                        rv++;
                    ic++;
                }
                if (ic[ - 1].ch != '\n')
                    rv++;
                VScrollLen(hwnd, rv - 1, TRUE);
                return rv;
            case EM_SETTABSTOPS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->cd->tabs = (*(int*)lParam) / 4;
                InvalidateRect(hwnd, 0, 0);
                return 0;
            case EM_SETMARGINS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->cd->leftmargin = lParam &0xffff;
                return 0;
            case EM_GETSIZE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return p->cd->textlen;            
            case EM_SETEVENTMASK:
                return 0;
            case EM_SETLIMITTEXT:
                return 0;
            case EM_GETMODIFY:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return p->cd->modified;
            case EM_SETMODIFY:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->cd->modified = wParam;
                for (i = 0; i < UNDO_MAX; i++)
                    p->cd->undolist[i].modified = TRUE;
                return 0;
            case EM_GETFIRSTVISIBLELINE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, p
                    ->textshowncharpos);
            case EM_SETFIRSTVISIBLELINE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                {
                        int x = 0;
                        while (wParam-- && p->cd->text[x].ch)
                        {
                            while (p->cd->text[x].ch && p->cd->text[x].ch != '\n')
                                x++;
                            if (p->cd->text[x].ch)
                                x++;
                        }
                        p->textshowncharpos = x;
                        InvalidateRect(hwnd, 0, 0);
                }
                break;                
            case EM_GETRECT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                ClientArea(hwnd, p, (LPRECT)lParam);
                ((LPRECT)lParam)->left += p->cd->leftmargin;
                return 0;
            case EM_CANUNDO:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                {
                    if (p->cd->undohead != p->cd->undotail )
                    {
                        int x = p->cd->redopos + 1;
                        if (x >= UNDO_MAX)
                            x -= UNDO_MAX;
                        return x != p->cd->undotail;
                    }
                }
                return FALSE;
            case EM_CANREDO:
                {
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    if (p->cd->undohead != p->cd->undotail )
                    {
                        return p->cd->redopos != p->cd->undohead;
                    }
                    return FALSE;
                }
            case WM_UNDO:
            case EM_UNDO:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                doundo(hwnd, p);
                return 0;
            case WM_REDO:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                doredo(hwnd, p);
                return 0;
            case WM_CUT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                SelToClipboard(hwnd, p);
                Replace(hwnd, p, "", 0);
                ScrollCaretIntoView(hwnd, p, FALSE);
                setcurcol(p);
                p->cd->selecting = FALSE;
                break;
            case WM_COPY:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                SelToClipboard(hwnd, p);
                break;
            case WM_PASTE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                ClipboardToSel(hwnd, p);
                p->cd->selecting = FALSE;
                setcurcol(p);
                break;
            case WM_GETFONT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return (LRESULT)p->cd->hFont;
            case WM_SETFONT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                DeleteObject(p->cd->hFont);
                DeleteObject(p->cd->hBoldFont);
                DeleteObject(p->cd->hItalicFont);
                p->cd->hFont = (HFONT)wParam;
                dc = GetDC(hwnd);
                xfont = SelectObject(dc, p->cd->hFont);
                GetTextMetrics(dc, &t);
                SelectObject(dc, xfont);

                ReleaseDC(hwnd, dc);
                p->cd->txtFontHeight = t.tmHeight;
                p->cd->txtFontWidth = t.tmAveCharWidth;
                GetObject(p->cd->hFont, sizeof(LOGFONT), &lf);
                lf.lfWidth = t.tmAveCharWidth-1;
                lf.lfItalic = TRUE;
                p->cd->hItalicFont = CreateFontIndirect(&lf);
                lf.lfItalic = FALSE;
                lf.lfWeight = FW_BOLD;
                p->cd->hBoldFont = CreateFontIndirect(&lf);
                if (lParam)
                {
                    SendUpdate(hwnd);
                    InvalidateRect(hwnd, 0, 0);
                }
                break;
            case EM_GETTEXTHEIGHT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return p->cd->txtFontHeight;
            case EM_LANGUAGE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->cd->language = lParam;
                if (lParam == LANGUAGE_NONE || !PropGetBool(NULL, "COLORIZE"))
                {
                    Colorize(p->cd->text, 0, p->cd->textlen, (p->cd->defbackground << 5) + C_TEXT, FALSE);
                    InvalidateRect(hwnd, 0, 0);
                }
                else
                {
                    FullColorize(hwnd, p, FALSE);
                }
                break;
            case EM_GETCOLUMN:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return curcol(p, p->cd->text, p->selstartcharpos);
            case EM_SETREADONLY:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->cd->readonly = !!wParam;
                SendMessage(hwnd, WM_SETEDITORSETTINGS, 0, 0);
                return 1;
            case EM_GETREADONLY:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                return p->cd->readonly;
            case EM_TOUPPER:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->readonly)
                    upperlowercase(hwnd, p, TRUE);
                break;
            case EM_TOLOWER:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->readonly)
                    upperlowercase(hwnd, p, FALSE);
                break;
            case EM_SELECTINDENT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->readonly)
                    SelectIndent(hwnd, p, !!lParam);
                break;
            case EM_SELECTCOMMENT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->readonly)
                    SelectComment(hwnd, p, !!lParam);
                break;
            case WM_SIZE:
                InvalidateRect(hwnd, 0, 0);
                break;
            case EM_GETSELTEXT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (p->selstartcharpos != p->selendcharpos)
                {
                    int start, end ;
                    char *pt = (char *)lParam;
                    INTERNAL_CHAR *q ;
                    if (p->selstartcharpos < p->selendcharpos)
                    {
                        start = p->selstartcharpos;
                        end = p->selendcharpos;
                    }
                    else
                    {
                        start = p->selendcharpos;
                        end = p->selstartcharpos;
                    }
                    q = p->cd->text + start ;
                    while (q < p->cd->text + end)
                        *pt++ = q++->ch;
                    *pt = 0;
                }				
                break ;
            default:
                break;

        }
        return DefWindowProc(hwnd, iMessage, wParam, lParam);
    }
    /**********************************************************************
     * RegisterXeditWindow registers the edit window
     **********************************************************************/
    void RegisterXeditWindow(void)
    {
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));
        wc.lpfnWndProc = &exeditProc;
        wc.lpszClassName = "XEDIT";
        wc.hInstance = hInstance;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 4;
        wc.style = CS_DBLCLKS;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_IBEAM);
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        RegisterClass(&wc);

        wc.lpfnWndProc = &funcProc;
        wc.lpszClassName = "xccfuncclass";
        wc.hInstance = hInstance;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.style = 0;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = 0; // LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        RegisterClass(&wc);

        wc.lpfnWndProc = &codecompProc;
        wc.lpszClassName = "xcccodeclass";
        wc.hInstance = hInstance;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.style = 0;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        RegisterClass(&wc);
        LoadColors();
    }