/* Software License Agreement
 * 
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 * 
 */

// assumes tabs aren't going to get reset yet
#define STRICT 
#define Uses_TRect
#define Uses_MsgBox
#define Uses_TKeys
#define Uses_TEvent
#include "tv.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include <limits.h>
#include <stdlib.h>
#include "editview.h"
#include "ipconfig.h"

extern InfoPadCfg cfg;

// This defines the maximum range for the horizontal scroll bar on the window
#define MAX_HSCROLL 256

// The KEYLIST structure holds information about text which should be
// colorized.  It gives the name of a keyword (e.g. 'int') and the color
// to display it in.

typedef struct
{
    char *text;
    unsigned char Color;
} KEYLIST;

static char matchColor = 0xa;
static char keywordColor = 0x9;
static char numberColor = 0x4;
static char commentColor = 0x2;
static char stringColor = 0x4;
static char directiveColor = 0x1;
static char backgroundColor = 0xf;
static char readonlyBackgroundColor = 0x8;
static char textColor = 0;
static char highlightColor = 0xe;
static char selectedTextColor = 0xf;
static char selectedBackgroundColor = 0x1;
static ClipBoard clipBoard;

#define C_BACKGROUND 0
#define C_READONLYBACKGROUND 1
#define C_TEXT 2
#define C_HIGHLIGHT 3
#define C_KEYWORD 4
#define C_COMMENT 5
#define C_NUMBER 6
#define C_STRING 7
#define C_ESCAPE 8
#define C_SYS_WINDOWBACKGROUND 15
static char *colors[] = { &backgroundColor, &readonlyBackgroundColor, &textColor, &highlightColor, &keywordColor,
                        &commentColor, &numberColor, &stringColor, &directiveColor };


// For drawing, put a little margin at the left
#define LEFTMARGIN 5


// and the next is highlight data filled in by the find
char highlightText[256] ;
int highlightCaseSensitive;
int highlightWholeWord;

static int page_size = -1;

void SendUpdate(EDITDATA *p);
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

void LoadColors()
{
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
        p->cd->text = (INTERNAL_CHAR *)VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
#else
        p->cd->text = (INTERNAL_CHAR *)calloc(1, size);
#endif
        if (!p->cd->text) 
        {
            messageBox("Out Of Memory",  mfError);
            return 0 ;
        }
        p->cd->textmaxlen = size ;
        return 1 ;
    }
    else
    {
        char *x ;
#ifdef SYSALLOC
           x = (char *)VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS) ;
        if (!x || !VirtualAlloc(x, (p->cd->textlen + 1) * sizeof(INTERNAL_CHAR), MEM_COMMIT, PAGE_READWRITE)) 
#else
        x = (char *)calloc(1, size);
        if (!x)
#endif
        {
            messageBox("Out Of Memory",  mfError);
               return 0 ;
        }
        memcpy(x, p->cd->text, (p->cd->textlen + 1) * sizeof(INTERNAL_CHAR));
        freemem(p);
        p->cd->text = (INTERNAL_CHAR *)x;
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
            messageBox("Cannot commit memory",  mfError);
        return 0 ;
    }
#endif
    return 1 ;
}
/**********************************************************************
 * Colorize marks a range of text with a specific color and attributes
 **********************************************************************/
static void Colorize(INTERNAL_CHAR *p, int start, int len, int color, int
    italic)
{
    int i;
    p += start;
    for (i = 0; i < len; i++, p++)
    {
        p->Color = color;
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
    while (*t && s->ch)
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
        return 1;
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
        v = pcmp(t, table[mid].text, preproc, retlen, insensitive, TRUE);
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
    v = pcmp(t, table[bottom].text, preproc, retlen, insensitive, TRUE);
    if (v)
        return 0;
    return  &table[bottom];
}

/**********************************************************************
 * SearchKeywords searches a range of INTERNAL_CHARs for keywords,
 * numbers, and strings, and colorizes them
 **********************************************************************/

static void SearchKeywords(INTERNAL_CHAR *buf, int chars, int start, int type, int bkColor)
{
    int i;
    KEYLIST *sr = C_keywordList;
    int size = sizeof(C_keywordList) / sizeof(KEYLIST);
    int preproc = '#';
    int hexflg = FALSE, binflg = FALSE;
    int xchars = chars;
    INTERNAL_CHAR *t = buf + start;
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
        while (t->ch && (t->Color & 0xf) == colors[C_COMMENT][0] && xchars > 0)
            t++, xchars--;
        if (xchars > 0 && (t == buf || !keysym(t[ - 1].ch) && (keysym(t->ch) ||
            t->ch == preproc)))
        {
            int len;
            KEYLIST *p = matchkeyword(sr, size, preproc, t, &len, type ==
                LANGUAGE_ASM || type == LANGUAGE_RC);
            if (p)
            {
                Colorize(buf, t - buf, len,  (bkColor << 4) + colors[p->Color][0], FALSE);
                t += len;
                xchars -= len;
            }
            else
            {
                if (t->Color == (colors[C_HIGHLIGHT][0] << 4) + colors[C_TEXT][0])
                    t->Color = (bkColor << 4) + colors[C_TEXT][0];
                t++, xchars--;
            }
        }
        else
        {
            if (t->Color == (colors[C_HIGHLIGHT][0] << 4) + colors[C_TEXT][0])
                t->Color = (bkColor << 4) + colors[C_TEXT][0];
            t++, xchars--;
        }
    }

    for (i = 0; i < chars; i++)
        if ((buf[start + i].Color & 0xf) != colors[C_COMMENT][0])
        {
            int len;
            if (highlightText[0] && 
                !pcmp(buf + start + i, highlightText, preproc, &len, !highlightCaseSensitive, FALSE)
                && (!highlightWholeWord || (i == start || !isalnum(buf[i-1].ch)) && !isalnum(buf[i+len].ch)))
            {
                Colorize(buf, start + i, len,  (colors[C_HIGHLIGHT][0] << 4) + colors[C_TEXT][0], FALSE);
                i += len - 1;
            }
            else if (type == LANGUAGE_ASM && buf[start+i].ch == '$')
            {
                len = 1;
                while (isxdigit(buf[start + i + len].ch))
                    len++;
                Colorize(buf, start + i, len, (bkColor << 4) | colors[C_NUMBER][0], FALSE);
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
                            while (ch == '.' || !binflg && isdigit(ch) || binflg && (ch == '0' || ch == '1')
                                || hexflg && (isxdigit(ch) || ch =='p' || ch == 'P') ||
                                  !hexflg && (ch == 'e' || ch== 'E'))
                            {
                                    i++;
                                    if (!hexflg && ch >= 'A' || hexflg && (ch =='p' || ch == 'P'))
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
                    Colorize(buf, start + j, i - j, (bkColor << 4) | colors[C_NUMBER][0], FALSE);
                    i--;
                }
            }
            else if ((buf[start + i].ch == '"' 
                || buf[start + i].ch == '\'') && (start + i <2 || buf[start + i - 1].ch != '\\' || buf[start + i - 2].ch == '\\'))
            {
                int ch = buf[start + i].ch;
                int j = i++;
                while (buf[start + i].ch && (buf[start + i].ch != ch && buf[start +
                    i].ch != '\n' || buf[start + i - 1].ch == '\\' && buf[start + i -
                    2].ch != '\\') && i < chars)
                    i++;
                Colorize(buf, start + j + 1, i - j - 1, (bkColor << 4) | colors[C_STRING][0], FALSE);
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

static void FormatBuffer(INTERNAL_CHAR *buf, int start, int end, int type, int bkColor)
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
                        Colorize(buf, t1 - buf, t - t1, (bkColor << 4) | colors[C_COMMENT][0], TRUE);
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
                if (!instring(buf, t1) && (t1->Color & 0xf) != colors[C_COMMENT][0])
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
                    Colorize(buf, t1 - buf, t - t1 + 1, (bkColor << 4) | colors[C_COMMENT][0], TRUE);
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
                Colorize(buf, t1 - buf, t - t1 + 1, (bkColor << 4) | colors[C_COMMENT][0], TRUE);
                t1 = strpstr(t, ";", end - (t - buf));
            }
        }
        SearchKeywords(buf, end - start, start, type, bkColor);
    }
    else
    {
        Colorize(buf, start, end, (bkColor << 4) | colors[C_TEXT][0], FALSE);
    }
}

//-------------------------------------------------------------------------

static void FormatBufferFromScratch(INTERNAL_CHAR *buf, int start, int end, int
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
            colors[C_COMMENT][0]))
            xstart--;
        while (buf[xend].ch && (buf[xend].ch != '\n' || (buf[xend].Color &0xf) ==
                    colors[C_COMMENT][0]))
                xend++;
    
        Colorize(buf, xstart, xend - xstart, (bkColor << 4) | colors[C_TEXT][0], FALSE);
        FormatBuffer(buf, xstart, xend, type, bkColor);
    }
}

/**********************************************************************
 * FormatLine is an optimized colorizer that just colorizes the current
 * line
 **********************************************************************/

static void FormatLine(EDITDATA *p, INTERNAL_CHAR *buf, int type, int bkColor)
{
    if (PropGetBool(NULL, "COLORIZE") && type != LANGUAGE_NONE)
    {
    
        int start, end;
        p->WinMessage(EM_GETSEL, (WPARAM) &start, (LPARAM) &end);
        FormatBufferFromScratch(buf, start, start, type, bkColor);
    }
}
static void UpdateSiblings(EDITDATA *p, int pos, int insert)
{
    EDLIST *list = p->cd->siblings;
    while (list)
    {
        if (list->data != p)
        {
            list->data->WinMessage(EM_UPDATESIBLING, insert, pos);
        }
        list = list->next ;
    }
}
/**********************************************************************
 * GetWordFromPos is a utility to find out what the word under the
 * cursor is, and various information about it
 **********************************************************************/
int GetWordFromPos(EDITDATA *p, char *outputbuf, int charpos, int *linenum, int
    *startoffs, int *endoffs)
{
    int linepos;
    int linecharpos;
    int linecharindex;
    char buf[1000];
    CHARRANGE charrange;

    if (charpos ==  - 1)
    {
        p->WinMessage(EM_EXGETSEL, (WPARAM)0, (LPARAM) &charrange);
        charpos = charrange.cpMin;
    }
    linepos = p->WinMessage(EM_EXLINEFROMCHAR, 0, (LPARAM)charpos);
    linecharindex = p->WinMessage(EM_LINEINDEX, linepos, 0);
    linecharpos = charpos - linecharindex;
    *(short*)buf = 1000;
    outputbuf[0] = 0;
    if (linecharpos >= p->WinMessage(EM_GETLINE, linepos, (LPARAM)buf))
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
 * getundo creates an UNDO structure based on the user's last operation
 **********************************************************************/
UNDO *getundo(EDITDATA *p, int type)
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

UNDO *insertautoundo(EDITDATA *p, int type)
{
    UNDO *u = getundo(p, type);
    if (u)
    {
        u->postselstart = p->selstartcharpos;
        u->postselend = p->selendcharpos;
    }
    return u;
}

/**********************************************************************
 * undo_deletesel gets the undo structure for a CUT operation
 **********************************************************************/
UNDO *undo_deletesel(EDITDATA *p)
{
    UNDO *u;
    int start, end;
    int i = 0;
    if (p->selstartcharpos == p->selendcharpos)
        return 0;
    u = getundo(p, UNDO_DELETESELECTION);
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
        char *temp = (char *)realloc(u->data, end - start);
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

UNDO *undo_casechange(EDITDATA *p)
{
    UNDO *x = undo_deletesel(p);
    x->type = UNDO_CASECHANGE;
    return x;
}

/**********************************************************************
 * undo_insertsel gets the undo structure for an operation which pasts
 **********************************************************************/
UNDO *undo_insertsel(EDITDATA *p, char *s)
{
    UNDO *u = getundo(p, UNDO_INSERTSELECTION);
    if (!u)
        return u;
    u->len = strlen(s);
    if (u->max < u->len)
    {
        u->data = (unsigned char *)realloc(u->data, u->len);
    }
    memcpy(u->data, s, u->len);
    return u;
}

/**********************************************************************
 * undo_deletechar gets the undo structure for a character deletion
 **********************************************************************/
UNDO *undo_deletechar(EDITDATA *p, int ch, int type)
{
    UNDO *u = getundo(p, type);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        char *temp = (char *)realloc(u->data, u->max + 64);
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
UNDO *undo_modifychar(EDITDATA *p)
{
    UNDO *u = getundo(p, UNDO_MODIFY);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        char *temp = (char *)realloc(u->data, u->max + 64);
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
UNDO *undo_insertchar(EDITDATA *p, int ch)
{
    UNDO *u = getundo(p, UNDO_INSERT);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        char *temp = (char *)realloc(u->data, u->max + 64);
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
void ClientArea(EDITDATA *p, TRect *r)
{
    *r = p->getBounds();
    r->b = r->b - r->a;
    r->a.x = 0;
    r->a.y = 0;
}

/**********************************************************************
 * posfromchar determines the screen position of a given offset in
 * the buffer
 **********************************************************************/
int posfromchar(EDITDATA *p, TPoint *point, int pos)
{
    char buf[256],  *x = buf;
    SIZE size;
    TRect r;
    HDC dc;
    int spos = p->textshowncharpos, xcol;
    int i = 0, j;
    point->x = point->y = 0;
    if (spos > pos)
        return 0;
    while (spos + i < pos && spos + i < p->cd->textlen)
    {
        if (p->cd->text[spos + i].ch == '\n')
        {
            point->y ++;
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
        point->x = (xcol - p->leftshownindex);
    else
        return 0;
    ClientArea(p, &r);
    if (point->x >= r.b.x || point->y >= r.b.y)
        return 0;
    return 1;
}

/**********************************************************************
 * charfrompos determines the buffer offset from the screen position
 **********************************************************************/
int charfrompos(EDITDATA *p, TPoint *point)
{
    TRect r;
    int row, col, xcol = 0;
    int pos = p->textshowncharpos, i = 0;
    char buf[256],  *x = buf;
    ClientArea(p, &r);
    if (point->x > r.b.x || point->y > r.b.y)
        return 0;
    row = point->y;
    col = point->x;
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
void VScrollLen(EDITDATA *p, int count, int set)
{
    int count1 = count;
    int base = 0;
    if (!set)
    {
        count1 += p->limit.y;
    }
    p->setLimit( p->limit.x, count1);
}

/**********************************************************************
 *  vscrolllen sets the position for the vertical scroll bar
 **********************************************************************/
void VScrollPos(EDITDATA *p, int count, int set)
{
    int count1 = count;
    if (!set)
    {
        count1 += p->delta.y;
    }
    p->scrollTo( p->delta.x, count1);
    p->parent->WinMessage(EN_SETCURSOR, 0, 0);
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

void MoveCaret(EDITDATA *p)
{
    int x = 0, y = 0;
    TPoint pt;
    if (posfromchar(p, &pt, p->selstartcharpos != p->selendcharpos 
            ? p->selendcharpos: p->selstartcharpos))
    {
        if (p->hasfocus)
        {
            p->setCursor(pt.x, pt.y);
            p->showCursor();
        }
        p->hiddenCaret = FALSE;
    }
    else
    {
        if (!p->hiddenCaret && p->hasfocus)
            p->hideCursor();
        p->hiddenCaret = TRUE;
    }
}

/**********************************************************************
 *  Scroll Left scrolls left or right, depending on the sign of 'cols'
 **********************************************************************/
void scrollleft(EDITDATA *p, int cols)
{
    p->leftshownindex += cols;
    if (p->leftshownindex < 0)
        p->leftshownindex = 0;
    SendUpdate(p);
    p->drawView();
}

/**********************************************************************
 *  Scrollup scrolls up or down, depending on the sign of 'lines'
 **********************************************************************/

void scrollup(EDITDATA *p, int lines)
{
    static int inscrollup;
    if (!inscrollup++)
    {
        TRect r, update;
        int totallines, movelines = lines;
        int pos = p->textshowncharpos, len = 0;
        ClientArea(p, &r);
        totallines = r.b.y - r.a.y;
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
            SendUpdate(p);
            if (lines < totallines)
                p->delta.y -= movelines + lines;
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
            SendUpdate(p);
            if (lines < totallines)
            {
                p->delta.y += movelines - lines;
            }
            p->textshowncharpos = pos;
        }
        p->drawView();
        SendUpdate(p);
        VScrollPos(p, len, FALSE);
    }
    inscrollup--;
}

/**********************************************************************
 *  ScrollCaretIntoView moves the text in the window around in such a way
 *  that the caret is in the window.
 **********************************************************************/
void ScrollCaretIntoView(EDITDATA *p, BOOL middle)
{
    TPoint pt;
    int lines, cols, colpos = 0;
    TRect r;
    int pos1, pos;
    pos1 = p->selendcharpos;
    if (posfromchar(p, &pt, pos1))
    {
        MoveCaret(p);
        return ;
    }
    ClientArea(p, &r);
    lines = r.b.y - r.a.y;
    cols = r.b.x - r.a.x;
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
                scrollup(p, xlines - lines/2);
            else
                scrollup(p, xlines - lines + 1);
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
                    scrollup(p,  - xlines - lines / 2 );
                else
                    scrollup(p,  - xlines );
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
        scrollleft(p, colpos - p->leftshownindex - 10);
    else if (colpos >= p->leftshownindex + cols)
        scrollleft(p, colpos - p->leftshownindex - cols + 1);
    MoveCaret(p);


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
void Replace(EDITDATA *p, char *s, int lens)
{
    UNDO *u = 0;
    int i, temp, changed;
    char *buf,  *s1;
    int len = 0, linepos;

    i = p->selendcharpos - p->selstartcharpos;
    changed = lens - i;
    u = undo_deletesel(p);
    if (i > 0)
    {
        len -= lfchars(p->cd->text, p->selstartcharpos, p->selendcharpos);
        p->parent->WinMessage(EN_LINECHANGE, SelLine(p, p
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
        p->parent->WinMessage(EN_LINECHANGE, SelLine(p, p->selendcharpos)
            ,  - temp);
        VScrollPos(p, - temp, FALSE);
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
        SendUpdate(p);
        p->cd->sendchangeonpaint = TRUE;
        p->drawView();
        return ;
    }
    if (lens)
    {
        u = undo_insertsel(p, s);
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
    p->parent->WinMessage(EN_LINECHANGE, SelLine(p, p->selstartcharpos),
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
        p->cd->text[p->selstartcharpos + i].Color = (p->cd->defbackground << 4) + p->cd->defforeground;
        i++;
        p->cd->textlen++;
    }
    VScrollLen(p, len, FALSE);
    FormatBufferFromScratch(p->cd->text, p->selstartcharpos - 1, p
        ->selstartcharpos + lens + 1, p->cd->language, p->cd->defbackground);
    SendUpdate(p);
    p->cd->sendchangeonpaint = TRUE;
    if (p->selstartcharpos < p->textshowncharpos)
    {
        p->textshowncharpos += changed;
        if (p->textshowncharpos < 0)
            p->textshowncharpos = 0;         
        while (p->textshowncharpos && p->cd->text[p->textshowncharpos-1].ch != '\n')
            p->textshowncharpos--;
    }
    p->drawView();
}

/**********************************************************************
 * GetLineOffset returns the line number, this time as an offset from
 * the first line shown at the top of the window
 **********************************************************************/
int GetLineOffset(EDITDATA *p, int chpos)
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
void drawline(EDITDATA *p, int chpos)
{
    TRect r;
    int pos;
    pos = p->selendcharpos;
    ClientArea(p, &r);
    r.a.y = GetLineOffset(p, pos);
    SendUpdate(p);
    p->drawView();
    MoveCaret(p);
}

/**********************************************************************
 * insertchar handles the functionality of inserting a character
 * will also cut a previous selection
 **********************************************************************/
void insertchar(EDITDATA *p, int ch)
{
    int len = 0, temp;
    UNDO *u = 0;
    if (p->cd->inserting)
    {
        int i = p->selendcharpos - p->selstartcharpos;
        u = undo_deletesel(p);
        if (i > 0)
        {
            len -= lfchars(p->cd->text, p->selstartcharpos, p->selendcharpos);
            p->parent->WinMessage(EN_LINECHANGE, SelLine(p, p
                ->selstartcharpos), len);
            memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p->selendcharpos, (p
                ->cd->textlen - p->selendcharpos + 1) *sizeof(INTERNAL_CHAR));
            p->cd->textlen -= i;
            UpdateSiblings(p, p->selstartcharpos, -i);
        }
        else if (i < 0)
        {
            temp = lfchars(p->cd->text, p->selendcharpos, p->selstartcharpos);
            p->parent->WinMessage(EN_LINECHANGE, SelLine(p, p
                ->selendcharpos),  - temp);
            VScrollPos(p, - temp, FALSE);
            len -= temp;
            memcpy(p->cd->text + p->selendcharpos, p->cd->text + p->selstartcharpos, (p
                ->cd->textlen - p->selendcharpos + 1) *sizeof(INTERNAL_CHAR));
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
            p->parent->WinMessage(EN_LINECHANGE, SelLine(p, p
                ->selstartcharpos), 1);
        }
        if (!commitmem(p,p->cd->textlen + 2))
        {
            SendUpdate(p);
            p->cd->sendchangeonpaint = TRUE;
            p->drawView();
            return;
        }
        u = undo_insertchar(p, ch);
        memmove(p->cd->text + p->selstartcharpos + 1, p->cd->text + p->selstartcharpos,
            (p->cd->textlen - p->selstartcharpos + 1) *sizeof(INTERNAL_CHAR));
        p->cd->text[p->selstartcharpos].ch = ch;
        if (p->cd->text[p->selstartcharpos].Color == 0)
            p->cd->text[p->selstartcharpos].Color = (p->cd->defbackground << 4) + colors[C_TEXT][0];
        p->cd->textlen++;
        UpdateSiblings(p, p->selstartcharpos, 1);		
        VScrollLen(p, len, FALSE);
    }
    else
    {
        p->selendcharpos = p->selstartcharpos;
        if (p->cd->text[p->selstartcharpos].ch == '\n')
                p->selendcharpos = p->selstartcharpos++;
        u = undo_modifychar(p);
        p->cd->text[p->selstartcharpos].ch = ch;
    }
    p->selendcharpos = ++p->selstartcharpos;
    if (u)
    {
        u->postselstart = p->selstartcharpos;
        u->postselend = p->selendcharpos;
    }
    p->cd->sendchangeonpaint = TRUE;
    ScrollCaretIntoView(p, FALSE);
}

/**********************************************************************
 * insertcr inserts a cr/lf pair
 **********************************************************************/
void insertcrtabs(EDITDATA *p);
void insertcr(EDITDATA *p, BOOL tabs)
{
    TRect r, update;
    int totallines;
    int temp;
    drawline(p, p->selstartcharpos);
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
        insertautoundo(p, UNDO_AUTOEND);
        insertchar(p, '\n');
        if (temp < 0)
        {
            p->selstartcharpos =  - temp + 1;
        }
        else
        {
            p->selendcharpos = temp + 1;
        }
        insertcrtabs(p);
        insertautoundo(p, UNDO_AUTOBEGIN);
    }
    else
    {
        insertchar(p, '\n');
        if (temp < 0)
        {
            p->selstartcharpos =  - temp + 1;
        }
        else
        {
            p->selendcharpos = temp + 1;
        }
    }
    VScrollLen(p, 1, FALSE);
    VScrollPos(p, 1, FALSE);
}

/**********************************************************************
 * inserttab inserts a tab, or types in a bunch of spaces to take
 * us to the next tab position
 **********************************************************************/
void inserttab(EDITDATA *p)
{
    if (!PropGetBool(NULL, "TABS_AS_SPACES"))
        insertchar(p, '\t');
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
            insertchar(p, ' ');
            col++;
        }
    }
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
void insertcrtabs(EDITDATA *p)
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
                if ((p->cd->text[pos2].Color & 0xf) != colors[C_COMMENT][0])
                {
                    if (quotechar == p->cd->text[pos2].ch && (!pos2 || p->cd->text[pos2-1].ch != '\\' 
                                                           || pos2 < 2 || p->cd->text[pos2-2].ch == '\\'))
                    {
                        quotechar = 0;
                    }
                    else if (!quotechar)
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
//    insertautoundo(p, UNDO_AUTOCHAINEND);
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
        inserttab(p);
        n -= p->cd->tabs;
    }
    while (n--)
        insertchar(p, ' ');
    p->cd->inserting = oldinsert;
//    insertautoundo(p, UNDO_AUTOCHAINBEGIN);
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
void InsertBeginTabs(EDITDATA *p)
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
    insertautoundo(p, UNDO_AUTOCHAINEND);
    n = curcol(p, p->cd->text, pos);
    p->selstartcharpos = solpos;
    p->selendcharpos = eospos;
    Replace(p, "", 0);
    while (n >= p->cd->tabs)
    {
        inserttab(p);
        n -= p->cd->tabs;
    }
    while (n--)
        insertchar(p, ' ');
    p->selstartcharpos = ++p->selendcharpos; // skip past '}'
    insertautoundo(p, UNDO_AUTOCHAINBEGIN);
    p->cd->inserting = oldinsert;
}
/**********************************************************************
 * tab to the current line position
 **********************************************************************/
void InsertEndTabs(EDITDATA *p, int newend)
{
    int pos, n;
    int solpos, eospos;
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

    if (lsolpos = spacedend(p, pos))
    {
        int indentlevel = 0;
        eospos = 0;
        pos--;
        while (pos > 0)
        {
            int pos1 = preprocline(p, pos);
            if (pos1 !=  - 1)
                pos = pos1;
            else if ((p->cd->text[pos].Color & 0xf) != colors[C_COMMENT][0])
                if (p->cd->text[pos].ch == '{')
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
            else if (p->cd->text[pos].ch == '}')
                indentlevel++;
            pos--;
        }
        insertautoundo(p, UNDO_AUTOCHAINEND);
        n = curcol(p, p->cd->text, eospos);
        p->selstartcharpos = lsolpos;
        p->selendcharpos = leospos;
        Replace(p, "", 0);
        while (n >= p->cd->tabs)
        {
            inserttab(p);
            n -= p->cd->tabs;
        }
        while (n--)
            insertchar(p, ' ');
        p->selstartcharpos = ++p->selendcharpos; // skip past '}'
        insertautoundo(p, UNDO_AUTOCHAINBEGIN);
    }
    p->cd->inserting = oldinsert;
}

//-------------------------------------------------------------------------

void SelectIndent(EDITDATA *p, int insert)
{
    int oldSel;
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
            insertautoundo(p, UNDO_AUTOEND);
            p->selstartcharpos = p->selendcharpos = start;
            inserttab(p);
            p->cd->undolist[uVal].noChangeSel = TRUE;
            insertautoundo(p, UNDO_AUTOBEGIN);
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
                insertautoundo(p, UNDO_AUTOEND);
                p->selstartcharpos = start;
                p->selendcharpos = start + count;
                Replace(p, "", 0);
                insertautoundo(p, UNDO_AUTOBEGIN);
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
        insertautoundo(p, UNDO_AUTOEND);
        while (start < end)
        {
            int x = p->textshowncharpos;
            if (insert)
            {
                p->selstartcharpos = p->selendcharpos = start;
                inserttab(p);
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
                    Replace(p, "", 0);
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
        insertautoundo(p, UNDO_AUTOBEGIN);
    }
    p->cd->inserting = oldinsert;
    p->cd->selecting = TRUE;
    MoveCaret(p);
    p->cd->selecting = oldselect;
    p->drawView();
    p->parent->WinMessage(EN_SETCURSOR, 0, 0);
}
void SelectComment(EDITDATA *p, int insert)
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
    insertautoundo(p, UNDO_AUTOEND);
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
                Replace(p, ";", 1);
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
                Replace(p, "//", 2);
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
                    Replace(p, "", 0);
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
                Replace(p, "", 0);
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
                    Replace(p, ";", 1);
                    end += 1;
                    if (start < x)
                        p->textshowncharpos = x + 1;
                }
                else
                {
                    Replace(p, "//", 2);
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
                        Replace(p, "", 0);
                        end -= 1;
                        if (start < x)
                            p->textshowncharpos = x - 1;
                    }
                }
                else if (p->cd->text[start].ch == '/' && p->cd->text[start + 1].ch == '/')
                {
                    p->selendcharpos += 2;
                    Replace(p, "", 0);
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
    insertautoundo(p, UNDO_AUTOBEGIN);
    p->cd->inserting = oldinsert;
    p->cd->selecting = TRUE;
    MoveCaret(p);
    p->cd->selecting = oldselect;
    FormatBuffer(p->cd->text, olds, end + decd, p->cd->language, p->cd->defbackground);
    p->drawView();
    p->parent->WinMessage(EN_SETCURSOR, 0, 0);
}
//-------------------------------------------------------------------------

void DeletePound(EDITDATA *p)
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
    insertautoundo(p, UNDO_AUTOCHAINEND);
    p->selstartcharpos = n;
    p->selendcharpos = m;
    Replace(p, "", 0);
    p->selstartcharpos = p->selendcharpos = n + 1;
    insertautoundo(p, UNDO_AUTOCHAINBEGIN);
    ScrollCaretIntoView(p, FALSE);
}

//-------------------------------------------------------------------------

void DeletePercent(EDITDATA *p)
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
    insertautoundo(p, UNDO_AUTOCHAINEND);
    p->selstartcharpos = n;
    p->selendcharpos = m;
    Replace(p, "", 0);
    p->selstartcharpos = p->selendcharpos = n + 1;
    insertautoundo(p, UNDO_AUTOCHAINBEGIN);
    ScrollCaretIntoView(p, FALSE);
}

void CancelParenMatch(EDITDATA *p)
{
    if (p->matchingEnd != 0)
    {
        p->matchingEnd = p->matchingStart = 0;
        p->drawView();
    }
}
int FindParenMatchBackward(EDITDATA *p, int dec)
{
    int skip,match;
    int level = 1;
    int s = p->selstartcharpos;
    int quotechar = 0;
    if (s != p->selendcharpos || s == p->cd->textlen && !dec)
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
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"')
                && (!s || p->cd->text[s-1].ch != '\\'
                    || s < 2 || p->cd->text[s-2].ch == '\\'))
                quotechar = p->cd->text[s].ch;
            else if (p->cd->text[s].ch == skip)
                level++;
            else if (p->cd->text[s].ch == match)
                if (!--level)
                    break;
    }
    if (level)
        return FALSE;
    p->matchingStart = p->selstartcharpos - dec;
    p->matchingEnd = s;
    p->drawView();
    return TRUE;
}
int FindParenMatchForward(EDITDATA *p, int dec)
{
    int skip,match;
    int level = 1;
    int s = p->selstartcharpos;
    int quotechar = 0;
    if (s != p->selendcharpos || s == p->cd->textlen && !dec)
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
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"')
                && (p->cd->text[s-1].ch != '\\' || s < 2 || p->cd->text[s-2].ch == '\\'))
                quotechar = p->cd->text[s].ch;
            else if (p->cd->text[s].ch == skip)
                level++;
            else if (p->cd->text[s].ch == match)
                if (!--level)
                    break;
    }
    if (level)
        return FALSE;
    p->matchingStart = p->selstartcharpos - dec;
    p->matchingEnd = s;
    p->drawView();
    return TRUE;
}
void FindParenMatch(EDITDATA *p)
{
    if (PropGetBool(NULL, "MATCH_PARENTHESIS"))
        if (!FindParenMatchForward(p ,TRUE ))
            FindParenMatchBackward(p, TRUE);
}
/**********************************************************************
 * go backwards to the last tab position
 **********************************************************************/
void backtab(EDITDATA *p)
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
        Replace(p, "", 0);
    }

}


/**********************************************************************
 * removechar cuts a character from the text (delete or back arrow)
 **********************************************************************/
void removechar(EDITDATA *p, int utype)
{
    if (p->cd->inserting && p->selstartcharpos != p->selendcharpos)
    {
        Replace(p, "", 0);
        ScrollCaretIntoView(p, FALSE);
    }
    else
    {
        int del;
        if (p->selstartcharpos == p->cd->textlen)
            return ;
        if (p->cd->text[p->selstartcharpos].ch == '\f')
        {
            del = 2;
            insertautoundo(p, UNDO_AUTOEND);
            if (utype == UNDO_BACKSPACE)
            {
                p->selstartcharpos++;
                undo_deletechar(p, '\n', utype);
                p->selstartcharpos--;
                undo_deletechar(p, '\f', utype);
            }
            else
            {
                undo_deletechar(p, '\f', utype);
                undo_deletechar(p, '\n', utype);
            }
            insertautoundo(p, UNDO_AUTOBEGIN);
        }
        else
        {
            del = 1;
            undo_deletechar(p, p->cd->text[p->selstartcharpos].ch, utype);
            if (p->cd->text[p->selstartcharpos].ch == '\n')                
            {
                p->parent->WinMessage(EN_LINECHANGE, SelLine(p, p
                    ->selstartcharpos)+1, -1);
            }
        }
        p->cd->sendchangeonpaint = TRUE;
        memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p
            ->selstartcharpos + del, (p->cd->textlen - p->selstartcharpos - del + 1)
            *sizeof(INTERNAL_CHAR));
        p->cd->textlen-=del;
        UpdateSiblings(p, p->selstartcharpos, -del);
        ScrollCaretIntoView(p, FALSE);
        drawline(p, p->selstartcharpos);
    }
    p->selendcharpos = p->selstartcharpos;
}
    /**********************************************************************
     * SelToClipboard copies the current selection to the clipboard
     **********************************************************************/
    void SelToClipboard(EDITDATA *p)
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
        {
            char *data = (char *)calloc(end - start + 1 + lf, sizeof(char));
            
            if (data != NULL)
            {
                char *q = data;
                for (i = start; i < end; i++)
                {
                    if (p->cd->text[i].ch == '\n')
                        *q++ = '\r';
                    *q++ = p->cd->text[i].ch;
                }
                *q++ = 0;
                free(clipBoard.text);
                clipBoard.text = data;
                clipBoard.size = end - start + 1 + lf;
            }
        }
    }
    /**********************************************************************
     * ClipboardToSel pastes the clipboard into the text
     **********************************************************************/
    void ClipboardToSel(EDITDATA *ptr)
    {
        if (!clipBoard.text)
            return ;
        {
            char *data = clipBoard.text;
            int l = clipBoard.size;
            if (l)
            {
                char *mem = (char *)calloc(1,l),  *p = mem,  *q = mem;
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
                        Replace(ptr, mem, q - mem);
                        ScrollCaretIntoView(ptr, FALSE);
                        FormatBufferFromScratch(ptr->cd->text, ptr
                            ->selstartcharpos - 1, ptr->selendcharpos + 1,
                            ptr->cd->language, ptr->cd->defbackground);
                        SendUpdate(ptr);
                        ptr->cd->sendchangeonpaint = TRUE;
                        ptr->selendcharpos = ptr->selstartcharpos = ptr
                            ->selstartcharpos + strlen(mem);
                        MoveCaret(ptr);
                        ptr->drawView();
                    }
                    free(mem);
                }
            }
        }
    }
    /**********************************************************************
     * upline scrolls the display down one line
     **********************************************************************/
    void upline(EDITDATA *p, int lines)
    {
        TRect r;
        int ilines = lines;
        int curline;
        int pos, oldsel;
        int col, index = 0;
        pos = p->selendcharpos;
        //   oldsel = pos ;
        col = p->cd->updowncol;
        if (lines > 0)
        {
            while (lines && pos < p->cd->textlen)
            {
                if (p->cd->text[pos].ch == '\n')
                {
                    lines--;
                    p->delta.y++;
                }
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
                {
                    lines++;
                    p->delta.y--;
                }
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
                p->drawView();
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        ScrollCaretIntoView(p, FALSE);
    }
    /**********************************************************************
     * eol indexes to the last character in a line
     **********************************************************************/
    void eol(EDITDATA *p)
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
                p->drawView();
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        ScrollCaretIntoView(p, FALSE);
        MoveCaret(p);
    }
    /**********************************************************************
     * sol indexes to the first character in a line
     **********************************************************************/
    void sol(EDITDATA *p)
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
                p->drawView();
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        ScrollCaretIntoView(p, FALSE);
        MoveCaret(p);


    }
    /**********************************************************************
     * left moves the cursor left one character
     **********************************************************************/
    void left(EDITDATA *p)
    {
        if (!p->cd->selecting && p->selstartcharpos != p->selendcharpos)
        {
            if (p->selstartcharpos > p->selendcharpos)
                p->selstartcharpos = p->selendcharpos;
            else
                p->selendcharpos = p->selstartcharpos;
            p->drawView();
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
                    p->drawView();
                }
                p->selendcharpos = p->selstartcharpos = pos;
            }
            else
            {
                p->selendcharpos = pos;
            }
        }
        MoveCaret(p);
        ScrollCaretIntoView(p, FALSE);
    }
    /**********************************************************************
     * right moves the cursor right one character
     **********************************************************************/
    void right(EDITDATA *p)
    {
        if (!p->cd->selecting && p->selstartcharpos != p->selendcharpos)
        {
            if (p->selstartcharpos < p->selendcharpos)
                p->selstartcharpos = p->selendcharpos;
            else
                p->selendcharpos = p->selstartcharpos;
            p->drawView();
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
                    p->drawView();
                }
                p->selendcharpos = p->selstartcharpos = pos;
            }
            else
            {
                p->selendcharpos = pos;
            }
        }
        MoveCaret(p);
        ScrollCaretIntoView(p, FALSE);
    }
    void leftword(EDITDATA *p)
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
                p->drawView();
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        MoveCaret(p);
        ScrollCaretIntoView(p, FALSE);
    }
    void rightword(EDITDATA *p)
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
                p->drawView();
            }
            p->selendcharpos = p->selstartcharpos = pos;
        }
        else
        {
            p->selendcharpos = pos;
        }
        MoveCaret(p);
        ScrollCaretIntoView(p, FALSE);
    }
    /**********************************************************************
     * AutoTimeProc fires When the cursor is moved out of the window during 
     * a select operation.  It is used to keep the text scrolling.
     **********************************************************************/
    void CALLBACK AutoTimeProc(UINT message, UINT event, DWORD timex)
    {
        /*
        EDITDATA *p = (EDITDATA*)GetWindowLong(0);
        TRect r;
        ClientArea(p, &r);
        scrollup(p, p->cd->autoscrolldir);
        if (p->cd->autoscrolldir < 0)
        {
            p->selendcharpos = p->textshowncharpos;
        }
        else
        {
            int lines, pos = p->textshowncharpos;
            lines = r.b.y;
            while (lines && pos < p->cd->textlen)
            {
                if (p->cd->text[pos].ch == '\n')
                    lines--;
                pos++;
            }
            p->selendcharpos = pos;
        }
        MoveCaret(p);
        */
    }
    /**********************************************************************
     * StartAutoScroll is called to start the timer which keeps the screen
     * scrolling while the cursor is out of the window during a select
     **********************************************************************/
    void StartAutoScroll(EDITDATA *p, int dir)
    {/*
        if (!p->cd->autoscrolldir)
        {
            p->cd->autoscrolldir = dir;
            SetTimer(1, 120, AutoTimeProc);
        }
        */
    }
    /**********************************************************************
     * EndAutoScroll stops the above timer
     **********************************************************************/
    void EndAutoScroll(EDITDATA *p)
    {
        /*
        if (p->cd->autoscrolldir)
        {
            KillTimer(1);
            p->cd->autoscrolldir = 0;
        }
        */
    }
    /**********************************************************************
     * HilightWord selects the word under the cursor
     **********************************************************************/
    void HilightWord(EDITDATA *p)
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
    void undo_pchar(EDITDATA *p, int ch)
    {
        insertchar(p, ch);
        FormatLine(p, p->cd->text, p->cd->language, p->cd->defbackground);
    }
    /**********************************************************************
     * doundo is the primary routine to traverse the undo list and perform
     * an undo operation
     **********************************************************************/

    int doundo(EDITDATA *p)
    {
        int rv = 1;
        if (p->WinMessage(EM_CANUNDO, 0, 0))
        {
            UNDO *u ;
            int oldinsert = p->cd->inserting;
            int start, end;
            char *s;
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
                    Replace(p, "", 0);
                    break;
                case UNDO_DELETE:
                    start = u->undotemp;
                    if (u->len > start && keysym(u->data[start]))
                        while (u->len > start && keysym(u->data[start]))
                        {
                            p->selstartcharpos = p->selendcharpos = u->preselstart;
                            undo_pchar(p, u->data[start++]);
                        }
                    else
                    {
                        p->selstartcharpos = p->selendcharpos = u->preselstart;
                        undo_pchar(p, u->data[start++]);
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
                            undo_pchar(p, u->data[start++]);
                        }
                    else
                    {
                        undo_pchar(p, u->data[start++]);
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
                            undo_pchar(p, u->data[start]);
                            u->data[start++] = n;
                        }
                    else
                    {
                        int n;
                        p->selstartcharpos = p->selendcharpos = u->postselstart - start-1;
                        n = p->cd->text[p->selstartcharpos].ch;
                        undo_pchar(p, u->data[start]);
                        u->data[start++] = n;
                        if (u->data[start] == '\n')
                        {
                            p->selstartcharpos = p->selendcharpos = u
                                ->postselstart - start;
                            n = p->cd->text[p->selstartcharpos].ch;
                            undo_pchar(p, u->data[start]);
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
                    Replace(p, "", 0);
                    p->cd->modified = u->modified;
                    p->cd->redopos = x;
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    break;
                case UNDO_DELETESELECTION:
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
                    Replace(p, u->data, u->len);
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
                    while (!doundo(p))
                        ;
                    break;
                case UNDO_AUTOCHAINBEGIN:
                    p->cd->redopos = x;
                    while (!doundo(p))
                        ;
                    doundo(p); 
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
            p->drawView();
            ScrollCaretIntoView(p, FALSE);
        }
        return rv;
    }
    /**********************************************************************
     * doundo is the primary routine to traverse the undo list and perform
     * an undo operation
     **********************************************************************/

    int doredo(EDITDATA *p)
    {
        int rv = 1;
        int start, end;
        char *s;
        char buf[512];
        if (p->WinMessage(EM_CANREDO, 0, 0))
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
                            undo_pchar(p, u->data[start++]);
                        }
                    else
                    {
                        undo_pchar(p, u->data[start++]);
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
                    Replace(p, "", 0);
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
                    Replace(p, "", 0);
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
                            undo_pchar(p, u->data[--start]);
                            u->data[start] = n;
                        }
                    else
                    {
                        int n = p->cd->text[p->selstartcharpos].ch;
                        undo_pchar(p, u->data[--start]);
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
                    Replace(p, u->data, u->len);
                    p->cd->modified = p->cd->undolist[x].modified;
                    p->cd->redopos = x;
                    p->selstartcharpos = u->postselstart;
                    p->selendcharpos = u->postselend;
    
                    break;
                case UNDO_DELETESELECTION:
                    p->selstartcharpos = u->preselstart;
                    p->selendcharpos = u->preselend;
                    Replace(p, "", 0);
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
                    while (!doredo(p))
                        ;
                    break;
                case UNDO_AUTOCHAINEND:
                    p->cd->redopos = x;
                    while (!doredo(p))
                        ;
                    doredo(p); 
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
            p->drawView();
            ScrollCaretIntoView(p, FALSE);
        }
        return rv;
    }
    void verticalCenter(EDITDATA *p)
    {
        TRect bounds;
        int rows, pos, lines=0;
        ClientArea(p, &bounds);
        rows = (bounds.b.y - bounds.a.y);
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
        p->drawView();
        MoveCaret(p);
    }
    void upperlowercase(EDITDATA *p, int ucase)
    {
        int oldsel = -1;
        int i;
        int s, e;
        UNDO *u;
        u = undo_casechange(p);
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
        p->drawView();
    }
    void SendUpdate(EDITDATA *p)
    {
        p->parent->WinMessage(WM_COMMAND, (WPARAM)(EN_UPDATE), (LPARAM)p);
    }
    void FullColorize(EDITDATA *p, int creation)
    {
        if (creation)
        {
            p->WinMessage(EM_GETLINECOUNT, 0, 0); // update scroll bar
        }
        FormatBufferFromScratch(p->cd->text, 0, p->cd->textlen, p->cd
            ->language, p->cd->defbackground);
        p->drawView();
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
     * EditPaint is the paint procedure for the window.
     **********************************************************************/
    int drawchar(TDrawBuffer &b, int j, int ch, int color)
    {
        int n = 1;
        if (ch == '\t')
        {
            n = cfg.TabIndent() - (j %cfg.TabIndent());
            ch = ' ';
        }
        b.moveChar(j, ch, color, n);
        return n;
    }
    void EDITDATA::draw()
    {
        TDrawBuffer b;
        int i, j, pos, lines;
        int y = 0;
        TRect r;
        ClientArea(this, &r);
        lines = r.b.y;
        pos = textshowncharpos;
        int start = selstartcharpos, end = selendcharpos;
        int matchColor = matchColor + (cd->defbackground << 4);
        if (end < start)
        {
            start = selendcharpos;
            end = selstartcharpos;
        }
        for (i = 0; i < lines && pos < cd->textlen; i++)
        {
#define PAGEBRK "+--------- Page Break ---------+"
            for (j=0; j < leftshownindex && cd->text[pos].ch != '\n' && pos < cd->textlen; j++, pos++);
            int indent = 0;
            if (cd->text[pos].ch != '\n')
            {
                if (cd->text[pos].ch == '\f')
                {
                    b.moveCStr(0, PAGEBRK, selstartcharpos <= pos && pos < selendcharpos ? selectedTextColor + (selectedBackgroundColor << 4) : cd->defbackground);
                    indent += sizeof(PAGEBRK)-1;
                    pos++;
                }
                for (j=indent; pos < cd->textlen && cd->text[pos].ch != '\n' && j < size.x && pos < start ; pos++)
                    j += drawchar(b, j,cd->text[pos].ch, pos && (pos ==matchingStart || pos == matchingEnd) ? matchColor : cd->text[pos].Color);
                for ( ;pos < cd->textlen && cd->text[pos].ch != '\n' && j < size.x && pos < end; pos++)
                    j += drawchar(b, j,cd->text[pos].ch, selectedTextColor + (selectedBackgroundColor << 4));
                for ( ;pos < cd->textlen && cd->text[pos].ch != '\n' && j < size.x; pos++)
                    j += drawchar(b,j,cd->text[pos].ch, pos && (pos ==matchingStart || pos == matchingEnd) ? matchColor : cd->text[pos].Color);
                if (j < size.x)
                    b.moveChar(j, ' ', cd->defbackground << 4, size.x - j);
                writeBuf( 0, y++, size.x, 1, b );
                for (j=0; pos < cd->textlen && cd->text[pos].ch != '\n'; j++, pos++);
                if (pos < cd->textlen)
                    pos++;
            }
            else
            {
                b.moveChar(0, ' ', cd->defbackground << 4, size.x);
                writeBuf( 0, y++, size.x, 1, b );
                pos++;
            }
        }
        for ( ; i < lines; i++)
        {
            b.moveChar(0, ' ', cd->defbackground << 4, size.x);
            writeBuf( 0, y++, size.x, 1, b );
        }
        if (cd->sendchangeonpaint)
        {
            cd->sendchangeonpaint = FALSE;
            parent->WinMessage(WM_COMMAND, (WPARAM)(EN_CHANGE), (LPARAM)this);
        }
    }
void mm()
{
}
    void EDITDATA::handleEvent( TEvent& event )
    {
        TScroller::handleEvent(event);
        EDITDATA *p = this;
        TRect r;
        int i;
        TPoint pt;
        switch(event.what)
        {
            case evKeyDown:
                mm();
                CancelParenMatch(p);
                if (event.keyDown.controlKeyState &kbShift)
                {
                    if (!p->cd->selecting)
                    {
                        p->cd->selecting = TRUE;
                        p->drawView();
                    }
                }
                else
                {
                    if (p->cd->selecting)
                    {
                        p->cd->selecting = FALSE;
                        p->drawView();
                    }
                }
                switch(event.keyDown.keyCode)
                {
                    case kbShiftIns:
                        if (!p->cd->readonly)
                        {
                            if (GetKeyState(VK_SHIFT) &0x80000000)
                            {
                                p->parent->WinMessage(WM_PASTE, 0, 0);
                            }
                        }
                        break;
                    case kbCtrlIns:
                        if (!p->cd->readonly)
                        {
                            if (GetKeyState(VK_CONTROL) &0x80000000)
                            {
                                p->parent->WinMessage(WM_COPY, 0, 0);
                            }
                        }
                        break;
                    case kbIns:
                        if (!p->cd->readonly)
                        {
                            p->cd->inserting = !p->cd->inserting;
                            if (p->hasfocus)
                            {
                                WinMessage(WM_KILLFOCUS, 0, 0);
                                WinMessage(WM_SETFOCUS, 0, 0);
                            }
                            p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        }
                        break;
                    case kbShiftDel:
                        if (!p->cd->readonly)
                        {
                            if (GetKeyState(VK_SHIFT) &0x80000000)
                            {
                                WinMessage(WM_CUT, 0, 0);
                            }
                        }
                        break;
                    case kbBack:
                        if (p->selstartcharpos == p->selendcharpos)
                        {
                            if (p->selstartcharpos == 0)
                                break;
                            left(p);
                        }
                        // fall through
                        
                    case kbDel:
                        if (!p->cd->readonly)
                        {
                            if (p->selstartcharpos != p->selendcharpos)
                            {
                                Replace(p, "", 0);
                                ScrollCaretIntoView(p, FALSE);
                            }
                            else
                            {
                                removechar(p, event.keyDown.keyCode == kbDel ?
                                    UNDO_DELETE : UNDO_BACKSPACE);
                                FormatBufferFromScratch(p->cd->text, p
                                    ->selstartcharpos, p->selstartcharpos, p
                                    ->cd->language, p->cd->defbackground);
                            }
                            setcurcol(p);
                        }
                        break;
                    case kbEnter:
                        if (!p->cd->readonly)
                        {
                            insertcr(p, TRUE);
                            FormatBufferFromScratch(p->cd->text, p->selstartcharpos
                                - 1, p->selstartcharpos, p->cd->language, p->cd->defbackground);
                            setcurcol(p);
                        }
                        
                        break;
                    case kbShiftTab:
                        if (!p->cd->readonly)
                        {
                            if (multilineSelect(p))
                                SelectIndent(p, FALSE);
                            else
                            {
                                backtab(p);
                                drawline(p, p->selstartcharpos);
                            }
                        }
                        else
                            left(p);
                        setcurcol(p);
                        break;
                    case kbTab:
                        if (!p->cd->readonly)
                        {
                            if (multilineSelect(p))
                                SelectIndent(p, TRUE);
                            else
                            {
                                inserttab(p);
                                InsertEndTabs(p, FALSE);
                                drawline(p, p->selstartcharpos);
                            }
                        }
                        else
                            right(p);
                        setcurcol(p);
                        break;
                    case kbDown:
                        upline(p, 1);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbUp:
                        upline(p,  - 1);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbCtrlPgUp:
                        VScrollPos(p, 0, TRUE);
                        p->textshowncharpos = p->selendcharpos = 0;
                        if (!p->cd->selecting)
                            p->selstartcharpos = p->selendcharpos;
                        MoveCaret(p);
                        p->drawView();
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbPgUp:
                        ClientArea(p, &r);
                        i = r.b.y - r.a.y;
                        upline(p, 2-i);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);    
                        break;
                    case kbCtrlPgDn:
                        ClientArea(p, &r);
                        i = lfchars(p->cd->text, p->textshowncharpos, p->cd->textlen);
                        upline(p, i);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbPgDn:
                        ClientArea(p, &r);
                        i = r.b.y - r.a.y;
                        upline(p, i - 2);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbCtrlEnd:
                        i = lfchars(p->cd->text, p->textshowncharpos, p->cd->textlen);
                        upline(p, i);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbEnd:
                        eol(p);
                        setcurcol(p);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbCtrlHome:
                        VScrollPos(p, 0, TRUE);
                        p->textshowncharpos = 0;
                        if (p->cd->selecting)
                            p->selstartcharpos = 0;
                        else
                            p->selstartcharpos = p->selendcharpos = 0;
                        MoveCaret(p);
                        SendUpdate(p);
                        p->drawView();
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbHome:
                        sol(p);
                        setcurcol(p);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbCtrlLeft:
                        leftword(p);
                        setcurcol(p);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbLeft:
                        left(p);
                        setcurcol(p);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbCtrlRight:
                        rightword(p);
                        setcurcol(p);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbRight:
                        right(p);
                        setcurcol(p);
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    case kbCtrlA:
                        p->selstartcharpos = 0;
                        p->selendcharpos = p->cd->textlen;
                        p->drawView();
                        break;
                    case kbCtrlX:                        
                        if (!p->cd->readonly)
                        {
                            WinMessage(WM_CUT, 0, 0);
                        }
                        break;
                    case kbCtrlC:
                        WinMessage(WM_COPY, 0, 0);
                        break;
                    case kbCtrlV:
                        if (!p->cd->readonly)
                        {
                            WinMessage(WM_PASTE, 0, 0);
                        }
                        break;
                    case kbCtrlY:
                        if (!p->cd->readonly)
                            WinMessage(WM_REDO, 0, 0);
                        break;
                    case kbCtrlZ:
                        if (!p->cd->readonly)
                            WinMessage(WM_UNDO, 0, 0);
                        break;
                    case kbCtrlS:
                        if (!p->cd->readonly)
                        {
                            if (p->cd->modified)
                                p->parent->WinMessage(WM_COMMAND, IDM_SAVE, 0);
                        }
                        break;
                    case kbCtrlT:
                        verticalCenter(p);
                        break;
                    case kbCtrlL:
                        if (!p->cd->readonly && p->cd->inserting)
                        {
                            insertautoundo(p, UNDO_AUTOEND);
                            insertchar(p , '\f');
                            insertcr(p,FALSE);	
                            insertautoundo(p, UNDO_AUTOBEGIN);
                        }
                        break;
                    case kbShift:
                        p->cd->selecting = TRUE;
                        break;
                    default:
                        if ((unsigned char)event.keyDown.keyCode >= ' ' && (unsigned char)event.keyDown.keyCode < 256)
                        {
                            CancelParenMatch(p);
                            if (!p->cd->readonly)
                            {
                                insertchar(p, event.keyDown.keyCode);
                                if (event.keyDown.keyCode == '{')
                                    InsertBeginTabs(p);
                                else if (event.keyDown.keyCode == ' ' || event.keyDown.keyCode == '}')
                                    InsertEndTabs(p, event.keyDown.keyCode == '}');
                                else if (event.keyDown.keyCode == '#')
                                    DeletePound(p);
                                if (!p->cd->inserting)
                                    WinMessage(WM_CLEAR, 0, 0);
                                FormatLine(p, p->cd->text, p->cd->language, p->cd->defbackground);
                                drawline(p, p->selstartcharpos);
                                p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                            }
                            else
                            {
                                right(p);
                            }
                            FindParenMatch(p);
                            setcurcol(p);
                            p->parent->drawView();
                            if (p->cd->selecting)
                            {
                                p->drawView();
                            }
                            return;
                        }
                }
                if (p->cd->selecting)
                {
                    p->drawView();
                }
                p->parent->drawView();
                FindParenMatch(p);
                
                break;
            case evBroadcast:
                switch (event.message.command)
                {
                    case cmScrollBarChanged:
                        if (event.message.infoPtr == hScrollBar)
                        {
                            int delta = hScrollBar->value - hScrollBar->lastValue;
                            scrollleft(this, delta);
                        }
                        else
                        {
                            int delta = vScrollBar->value - vScrollBar->lastValue;
                            scrollup(this, delta);
                        }                                                
                        break;
                    case cmReceivedFocus:
                        p->hasfocus = TRUE;
                        p->hiddenCaret = TRUE;
                        if (p->cd->inserting)
                            normalCursor();
                        else
                            blockCursor();
                        MoveCaret(p);
                        p->parent->WinMessage(WM_COMMAND, (WPARAM)(EN_SETFOCUS), (LPARAM)this);
                        break;
                    case cmReleasedFocus:
                        p->hasfocus = FALSE;
                        hideCursor();
                        p->parent->WinMessage(WM_COMMAND, (WPARAM)(EN_KILLFOCUS), (LPARAM)this);
                        break;
                }
                break;
            case evMouseMove:
                ClientArea(p, &r);
                pt = makeLocal(event.mouse.where);
                if (p->cd->buttondown)
                {
                    if ((signed)pt.y < 0)
                        StartAutoScroll(p,  - 1);
                    else if (pt.y > r.b.y)
                    {
                        StartAutoScroll(p, 1);
                    }
                    else
                    {
                        EndAutoScroll(p);
                        p->selendcharpos = charfrompos(p, &pt);
                        p->drawView();
                    }
                    MoveCaret(p);
                    p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                }
                return;
            case evMouseDown:
                if (event.mouse.buttons & 1)
                {
                    // left
                    if (event.mouse.eventFlags & meDoubleClick)
                    {
                        HilightWord(p);
                        drawline(p, 0);
                    }
                    else
                        
                    {
                        CancelParenMatch(p);
//                        if (GetFocus() != hwnd)
                            p->parent->WinMessage(WM_COMMAND, EN_NEEDFOCUS, 0);
        //                    SetFocus(hwnd);
                        p->cd->selecting = TRUE;
                        p->cd->buttondown = TRUE;
                        pt = makeLocal(event.mouse.where);
                        p->selstartcharpos = p->selendcharpos = charfrompos(p,
                            &pt);
                        MoveCaret(p);
                        setcurcol(p);
                        FindParenMatch(p);
                        p->drawView();
                        p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                        break;
                    }
                }
                else 
                {
//                    if (GetFocus() != hwnd)
                        p->parent->WinMessage(WM_COMMAND, EN_NEEDFOCUS, 0);
    //                    SetFocus(hwnd);
//                    nm.code = NM_RCLICK;
//                    p->parent->WinMessage(WM_NOTIFY, 0, (LPARAM) &nm);
                }
                
            case evMouseUp:
                // left
                if (p->cd->buttondown)
                {
                    p->cd->buttondown = FALSE;
                    EndAutoScroll(p);
                }
                p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                break;
        }
    }
    /**********************************************************************
     * exeditProc is the window proc for the edit control
     **********************************************************************/

    DWORD EDITDATA::WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam)
    {
        static char buffer[1024];
        char face[256];
        EDITDATA *p = this;
        DWINFO *x;
        int stop;
        char *data,  *buf;
        LRESULT rv;
        int i, linecount, start, end, line, chars;
        int offset, l;
        int charpos;
        TRect r;
        INTERNAL_CHAR *ic;
        TPoint pt;
        CHARRANGE *ci;
        switch (iMessage)
        {
            case WM_ERASEBKGND:
                return 0;
            case EM_GETINSERTSTATUS:
                return cd->inserting;
            case WM_SETTEXT:
                freemem(p);
                p->cd->text = 0;
                p->cd->textlen = 0;
                p->cd->textmaxlen = 0;
                if (p->cd->language)
                    WinMessage(EM_SETBKGNDCOLOR, 0, p->cd->readonly ? colors[C_READONLYBACKGROUND][0] : colors[C_BACKGROUND][0]);
                i = strlen((char*)lParam) ;
                
                if (allocmem(p, i * 5) && commitmem(p, i+1))
                {
                    p->cd->textlen = i;
                    for (i = 0; i < p->cd->textlen; i++)
                    {
                        p->cd->text[i].ch = ((char*)lParam)[i];
                        p->cd->text[i].Color = (p->cd->defbackground << 4) + p->cd->defforeground;
                    }
                    FullColorize(p, TRUE);
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
                return (DWORD)this;
            case EM_UPDATESIBLING:
                if (lParam <= p->textshowncharpos)
                    if (wParam > 0)
                        p->textshowncharpos += wParam;
                    else
                        if (lParam - wParam < p->textshowncharpos)
                            p->textshowncharpos += wParam;
                        else
                            p->textshowncharpos -= p->textshowncharpos - lParam;
                if (lParam <= p->selstartcharpos)
                    p->selstartcharpos += wParam;
                if (lParam <= p->selendcharpos)
                    p->selendcharpos += wParam;
                p->drawView();
                break ;
            case WM_CREATE:
    	        p->id = 0;
                p->selstartcharpos = 0;
                p->selstartcharpos = 0;
                p->selendcharpos = 0;
                p->textshowncharpos = 0;
                p->leftshownindex = 0;
    	        p->matchingStart = 0;
    	        p->matchingEnd = 0;
    	        p->wheelIncrement = 0;
                p->hasfocus = 0;
                p->hiddenCaret = 0;
                if (lParam)
                {
                    p->cd = (COREDATA *)((EDITDATA *)(lParam))->cd;
                }
                else 
                {
                    p->cd = (COREDATA*)calloc(1,sizeof(COREDATA));
                    p->cd->inserting = TRUE;
                    
                    p->cd->defbackground = p->cd->readonly ? colors[C_READONLYBACKGROUND][0] : colors[C_BACKGROUND][0];
                    p->cd->defforeground = colors[C_TEXT][0];
                    p->cd->tabs = PropGetInt(NULL, "TAB_INDENT");
                    p->cd->leftmargin = EC_LEFTMARGIN;
                    
                    getPageSize() ;
                    p->cd->textlen = 0;
                    allocmem(p, page_size * 20);
                    commitmem(p, page_size);
                }
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
                setLimit(MAX_HSCROLL, p->limit.y);
                break;
            case WM_SETEDITORSETTINGS:
                if (p->cd->language)
                    WinMessage(EM_SETBKGNDCOLOR, 0, p->cd->readonly ? colors[C_READONLYBACKGROUND][0] : colors[C_BACKGROUND][0]);
                p->cd->defbackground = p->cd->readonly ? colors[C_READONLYBACKGROUND][0] : colors[C_BACKGROUND][0];
                if (p->cd->language)
                {
                    stop = p->cd->tabs *4;
                    WinMessage(EM_SETTABSTOPS, 1, (LPARAM) &stop);
                    Colorize(p->cd->text, 0, p->cd->textlen, (p->cd->defbackground << 4) + p->cd->defforeground, FALSE);
                    FullColorize(p, FALSE);
                }
                drawView();
                return 0;
            case WM_DESTROY:
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
                        freemem(p);
                        for (i = 0; i < UNDO_MAX; i++)
                            free(p->cd->undolist[i].data);
                        free((void*)p->cd);
                    }
                }
                break;
            case WM_WORDUNDERCURSOR:
                   return GetWordFromPos(this, (char *)lParam,  - 1, 0, 0, 0);
            case WM_WORDUNDERPOINT:
            {
                int charpos, start, end, linepos;
                POINT pt = *(POINT *)wParam;
                charpos = WinMessage(EM_CHARFROMPOS, 0, (LPARAM) &pt);
                return GetWordFromPos(this, (char *)lParam, charpos, &linepos, &start, &end);
            }
                break;
            case EM_SETBKGNDCOLOR:
                if (wParam)
                {
                    p->cd->defbackground = colors[C_SYS_WINDOWBACKGROUND][0];
                }
                else
                {

                    p->cd->defbackground = lParam;
                }
                drawView();
                break;
            case WM_GETTEXT:
                if (!p->cd->textlen)
                    return 0;
                i = p->cd->textlen;
                if (wParam <= i)
                    i = wParam - 1;
                {
                    INTERNAL_CHAR *x = p->cd->text;
                    while (i--)
                        *((char*)lParam)++ = x++->ch;
                    *(char*)lParam = 0;
                }
                return i;
            case WM_GETTEXTLENGTH:
                return p->cd->textlen + 1;
            case EM_EXSETSEL:
                ci = (CHARRANGE*)lParam;
                wParam = ci->cpMin;
                lParam = ci->cpMax;
                // fall through
            case EM_SETSEL:
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
                    p->selstartcharpos = wParam;
                    p->selendcharpos = lParam;
                }
                if (!p->cd->nosel)
                {
                    //            VScrollPos(p, hwnd,LineFromChar(p,p->selstartcharpos),TRUE) ;
                    ScrollCaretIntoView(p, FALSE);
                    p->drawView();
                }
                return 0;
            case EM_GETSEL:
                if (wParam)
                    *(int*)wParam = p->selstartcharpos;
                if (lParam)
                    *(int*)lParam = p->selendcharpos;
                return  - 1;
            case EM_EXGETSEL:
                ((CHARRANGE*)lParam)->cpMin = p->selstartcharpos;
                ((CHARRANGE*)lParam)->cpMax = p->selendcharpos;
                return 0;
            case EM_HIDESELECTION:
                p->cd->nosel = wParam;
                if (p->selstartcharpos != p->selendcharpos)
                    p->drawView();
                return 0;
            case EM_REPLACESEL:
                Replace(p, (char*)lParam, strlen((char*)lParam));
                return 0;
            case EM_SCROLLCARET:
                ScrollCaretIntoView(p, lParam);
                return 0;
            case EM_LINEFROMCHAR:
                return LineFromChar(p, wParam);
            case EM_EXLINEFROMCHAR:
                return LineFromChar(p, lParam);
            case EM_LINEINDEX:
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
                return charfrompos(p, (TPoint*)lParam);
            case EM_POSFROMCHAR:
                {
                    int x, y;
                    posfromchar(p, (TPoint*)wParam, lParam);
                    return 0;
                }

            case EM_GETLINECOUNT:
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
                VScrollLen(p, rv - 1, TRUE);
                return rv;
            case EM_SETTABSTOPS:
                p->cd->tabs = (*(int*)lParam) / 4;
                p->drawView();
                return 0;
            case EM_SETMARGINS:
                p->cd->leftmargin = lParam &0xffff;
                return 0;
            case EM_GETSIZE:
                return p->cd->textlen;            
            case EM_SETEVENTMASK:
                return 0;
            case EM_SETLIMITTEXT:
                return 0;
            case EM_GETMODIFY:
                return p->cd->modified;
            case EM_SETMODIFY:
                p->cd->modified = wParam;
                for (i = 0; i < UNDO_MAX; i++)
                    p->cd->undolist[i].modified = TRUE;
                return 0;
            case EM_GETFIRSTVISIBLELINE:
                return WinMessage(EM_EXLINEFROMCHAR, 0, p
                    ->textshowncharpos);
            case EM_SETFIRSTVISIBLELINE:
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
                    p->drawView();
                }
                break;                
            case EM_GETRECT:
                ClientArea(p, (TRect *)lParam);
                ((TRect *)lParam)->a.x += p->cd->leftmargin;
                return 0;
            case EM_CANUNDO:
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
                    if (p->cd->undohead != p->cd->undotail )
                    {
                        return p->cd->redopos != p->cd->undohead;
                    }
                    return FALSE;
                }
            case WM_UNDO:
            case EM_UNDO:
                doundo(p);
                p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                return 0;
            case WM_REDO:
                doredo(p);
                p->parent->WinMessage(EN_SETCURSOR, 0, 0);
                return 0;
            case WM_CUT:
                SelToClipboard(p);
                Replace(p, "", 0);
                ScrollCaretIntoView(p, FALSE);
                setcurcol(p);
                p->cd->selecting = FALSE;
                break;
            case WM_COPY:
                SelToClipboard(p);
                break;
            case WM_PASTE:
                ClipboardToSel(p);
                p->cd->selecting = FALSE;
                setcurcol(p);
                break;
            case EM_GETTEXTHEIGHT:
                return 1;
            case EM_LANGUAGE:
                p->cd->language = lParam;
                if (lParam == LANGUAGE_NONE || !PropGetBool(NULL, "COLORIZE"))
                {
                    Colorize(p->cd->text, 0, p->cd->textlen, (p->cd->defbackground << 4) + colors[C_TEXT][0], FALSE);
                    p->drawView();
                }
                else
                {
                    FullColorize(p, FALSE);
                }
                break;
            case EM_GETCOLUMN:
                return curcol(p, p->cd->text, p->selstartcharpos);
            case EM_SETREADONLY:
                p->cd->readonly = !!wParam;
                WinMessage(WM_SETEDITORSETTINGS, 0, 0);
                return 1;
            case EM_GETREADONLY:
                return p->cd->readonly;
            case EM_TOUPPER:
                if (!p->cd->readonly)
                    upperlowercase(p, TRUE);
                break;
            case EM_TOLOWER:
                if (!p->cd->readonly)
                    upperlowercase(p, FALSE);
                break;
            case EM_SELECTINDENT:
                if (!p->cd->readonly)
                    SelectIndent(p, !!lParam);
                break;
            case EM_SELECTCOMMENT:
                if (!p->cd->readonly)
                    SelectComment(p, !!lParam);
                break;
            case WM_SIZE:
                p->drawView();
                break;
            case EM_GETSELTEXT:
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
    }
