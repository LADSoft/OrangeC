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

// assumes tabs aren't going to get reset yet
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "header.h"
#include <limits.h>
#include "c_types.h"
#include <stdlib.h>
#include <ctype.h>
#include <process.h>
#include "symtypes.h"

// and the next is highlight data filled in by the find
char highlightText[256];
int highlightCaseSensitive;
int highlightWholeWord;

COLORREF keywordColor = 0xff8000;
COLORREF numberColor = 0x0000ff;
COLORREF commentColor = 0x00c000;
COLORREF stringColor = 0x8000ff;
COLORREF directiveColor = 0xff0000;
COLORREF backgroundColor = 0xffffff;
COLORREF readonlyBackgroundColor = 0xc0c0c0;
COLORREF textColor = 0;
COLORREF highlightColor = 0x80ffff;
COLORREF selectedTextColor = 0xffffff;
COLORREF selectedBackgroundColor = 0xff0000;
COLORREF columnbarColor = 0xccccff;

COLORREF defineColor = 0x208080;
COLORREF functionColor = 0xc00000;
COLORREF parameterColor = 0x0080ff;
COLORREF typedefColor = 0xc0c000;
COLORREF tagColor = 0xc0c000;
COLORREF autoColor = 0x0080ff;
COLORREF localStaticColor = 0x00e0ff;
COLORREF staticColor = 0x00e0ff;
COLORREF globalColor = 0xffff00;
COLORREF externColor = 0xe0e000;
COLORREF labelColor = 0xc00000;
COLORREF memberColor = 0xc0c000;

COLORREF* colors[] = {
    &backgroundColor, &readonlyBackgroundColor, &textColor,   &highlightColor, &keywordColor,   &commentColor, &numberColor,
    &stringColor,     &directiveColor,          &defineColor, &functionColor,  &parameterColor, &typedefColor, &tagColor,
    &autoColor,       &localStaticColor,        &staticColor, &globalColor,    &externColor,    &labelColor,   &memberColor};

static int page_size = -1;
static int matchCount = 0;

void SendUpdate(HWND hwnd);
static int LookupColorizeEntry(COLORIZE_HASH_ENTRY* entries[], char* name, int line);
// The C_keywordList is a list of all keywords, with colorization info
KEYLIST C_keywordList[] = {
#include "c_kw.h"
};
KEYLIST Ctype_keywordList[] = {
#include "ctype_kw.h"
};
KEYLIST ASM_keywordList[] = {
#include "asm_kw.h"
};
KEYLIST RC_keywordList[] = {
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

/**********************************************************************
 * Colorize marks a range of text with a specific color and attributes
 **********************************************************************/
void Colorize(INTERNAL_CHAR* buf, int start, int len, int color, int italic)
{
    int dwEffects = 0, i;
    INTERNAL_CHAR* p = buf + start;
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
    return (x >= '0' && x <= '9') || (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || (x <= 0xff && x >= 0xc0) || x == '_';
}

/**********************************************************************
 * strpstr  finds a text string within a string organized as internal
 * characters.  Returns 0 if it couldn't find the string
 **********************************************************************/
INTERNAL_CHAR* strpstr(INTERNAL_CHAR* t, char* text, int len)
{
    while (t->ch && len)
    {
        if (t->ch == text[0])
        {
            char* t1 = text;
            INTERNAL_CHAR* it1 = t;
            while (*t1 && it1->ch == *t1)
            {
                t1++;
                it1++;
            }
            if (!*t1)
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

int strplen(INTERNAL_CHAR* t)
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

static int backalpha(INTERNAL_CHAR* buf, int i)
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

int pcmp(INTERNAL_CHAR* s, char* t, int preproc, int* retlen, int caseinsensitive, int bykey)
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
        if (val < ch)
            return -1;
        else if (val > ch)
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
    return bykey && keysym(s->ch);
}

/**********************************************************************
 * See if a keyword matches the current text location
 **********************************************************************/
KEYLIST* matchkeyword(KEYLIST* table, int tabsize, int preproc, INTERNAL_CHAR* t, int* retlen, int insensitive)
{
    int top = tabsize;
    int bottom = -1;
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
    if (bottom == -1)
        return 0;
    v = pcmp(t, table[bottom].text, preproc, retlen, insensitive, t->ch != '#');
    if (v)
        return 0;
    return &table[bottom];
}

/**********************************************************************
 * SearchKeywords searches a range of INTERNAL_CHARs for keywords,
 * numbers, and strings, and colorizes them
 **********************************************************************/

static void SearchKeywords(COLORIZE_HASH_ENTRY* entries[], INTERNAL_CHAR* buf, int chars, int start, int type, int bkColor)
{
    int i;
    KEYLIST* sr = C_keywordList;
    int size = sizeof(C_keywordList) / sizeof(KEYLIST);
    int preproc = '#';
    int hexflg = FALSE, binflg = FALSE;
    int xchars = chars;
    INTERNAL_CHAR* t = buf;
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
        if (xchars > 0 && ((t == buf || !keysym(t[-1].ch)) && (keysym(t->ch) || t->ch == preproc)))
        {
            int len;
            KEYLIST* p = matchkeyword(sr, size, preproc, t, &len, type == LANGUAGE_ASM || type == LANGUAGE_RC);
            if (p)
            {
                Colorize(buf, t - buf, len, (bkColor << 5) + p->Color, FALSE);
                t += len;
                xchars -= len;
            }
            else
            {
                int type;
                char name[256], *p = name;
                INTERNAL_CHAR* t1 = t;
                while (keysym(t1->ch) && p < name + 250)
                {
                    *p++ = t1++->ch;
                }
                *p = 0;
                if ((t != buf && t[-1].ch == '.') || (t > buf + 1 && t[-1].ch == '>' && t[-2].ch == '-'))
                {
                    if (p != name)
                    {
                        len = p - name;
                        Colorize(buf, t - buf, len, (bkColor << 5) + C_MEMBER, FALSE);
                    }
                    t += len;
                    xchars -= len;
                }
                else
                {
                    type = LookupColorizeEntry(entries, name, lineno);
                    if (type >= 0)
                    {
                        if (p != name)
                        {
                            len = p - name;
                            Colorize(buf, t - buf, len, (bkColor << 5) + (type - ST_DEFINE + C_DEFINE), FALSE);
                        }
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
        if (i == 0 || buf[start + i].ch == '\n')
        {
            hashed = FALSE;
            while (i < chars && isspace(buf[start + i].ch))
                i++;
            if (i >= chars)
                break;
            if (buf[start + i].ch == '#')
            {
                i++;
                while (i < chars && isspace(buf[start + i].ch))
                    i++;
                if (i >= chars)
                    break;
                hashed = buf[start + i].ch == 'i' && buf[start + i + 1].ch == 'n';  // check for #include
            }
        }
        if ((buf[start + i].Color & 0xf) != C_COMMENT)
        {
            int len;
            if (highlightText[0] && !pcmp(buf + start + i, highlightText, preproc, &len, !highlightCaseSensitive, FALSE) &&
                (!highlightWholeWord || (i == 0 || ((!isalnum(buf[i - 1].ch)) && buf[i - 1].ch != '_' &&
                                                    !isalnum(buf[i + len].ch) && buf[i + len].ch != '_'))))
            {
                Colorize(buf, start + i, len, (C_HIGHLIGHT << 5) + C_TEXT, FALSE);
                i += len - 1;
            }
            else if (type == LANGUAGE_ASM && buf[start + i].ch == '$')
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
                    if (type == LANGUAGE_C || type == LANGUAGE_CPP || type == LANGUAGE_RC)
                    {
                        if (isdigit(ch) || ch == '.')
                        {
                            char oc = ch;
                            ch = buf[start + i].ch;
                            hexflg = oc == '0' && (ch == 'x' || ch == 'X');
                            binflg = oc == '0' && (ch == 'b' || ch == 'B');
                            if (hexflg || binflg)
                                ch = buf[start + ++i].ch;
                            while (ch == '.' || (!binflg && ch >= '0' && ch <= '9') || (binflg && (ch == '0' || ch == '1')) ||
                                   (hexflg && ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') ||
                                               ch == 'p' || ch == 'P')) ||
                                   (!hexflg && (ch == 'e' || ch == 'E')))
                            {
                                i++;
                                if ((!hexflg && ch >= 'A') || (hexflg && (ch == 'p' || ch == 'P')))
                                {
                                    ch = buf[start + i].ch;
                                    if (ch == '-' || ch == '+')
                                        i++;
                                }
                                ch = buf[start + i].ch;
                            }
                            while (ch == 'L' || ch == 'l' || ch == 'U' || ch == 'u' || ch == 'F' || ch == 'f')
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
            else if ((buf[start + i].ch == '"' || buf[start + i].ch == '\'') &&
                     (start + i < 2 || buf[start + i - 1].ch != '\\' || buf[start + i - 2].ch == '\\'))
            {
                int ch = buf[start + i].ch;
                int j = i++;
                while (buf[start + i].ch &&
                       ((buf[start + i].ch != ch && buf[start + i].ch != '\n') ||
                        (buf[start + i - 1].ch == '\\' && buf[start + i - 2].ch != '\\')) &&
                       i < chars)
                    i++;
                Colorize(buf, start + j + 1, i - j - 1, (bkColor << 5) | C_STRING, FALSE);
            }
            else if (hashed && buf[start + i].ch == '<')
            {
                int j = i++;
                while (buf[start + i].ch && (buf[start + i].ch != '>' && buf[start + i].ch != '\n'))
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
int instring(INTERNAL_CHAR* buf, INTERNAL_CHAR* t1)
{
    INTERNAL_CHAR* t2 = t1;
    int quotechar = 0;
    while (t2 != buf && t2[-1].ch != '\n')
        t2--;
    while (t2 != t1)
    {
        if (quotechar)
        {
            if (t2->ch == quotechar && t2[-1].ch != '\\')
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

void FormatBuffer(COLORIZE_HASH_ENTRY* entries[], INTERNAL_CHAR* buf, int start, int end, int type, int bkColor)
{
    if (type != LANGUAGE_NONE && PropGetBool(NULL, "COLORIZE"))
    {
        if (type == LANGUAGE_C || type == LANGUAGE_CPP || type == LANGUAGE_RC)
        {
            INTERNAL_CHAR* t = buf + start;
            INTERNAL_CHAR* t1;
            while (TRUE)
            {
                t1 = strpstr(t, "/*", end - (t - buf));

                if (t1)
                {
                    if ((t1 == buf || t1[-1].ch != '/') && !instring(buf, t1))
                    {

                        t = strpstr(t1, "*/", -1);
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

                    t = strpstr(t1, "\n", -1);
                    while (1)
                    {
                        if (!t)
                        {
                            t = t1 + strplen(t1);
                            break;
                        }
                        else
                        {
                            INTERNAL_CHAR* t2 = t;
                            while (t2 > buf && isspace(t2[-1].ch))
                                t2--;
                            if (t2[-1].ch != '\\')
                                break;
                            t = strpstr(t + 1, "\n", -1);
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
            INTERNAL_CHAR* t = buf + start;
            INTERNAL_CHAR* t1;
            t1 = strpstr(t, ";", end - (t - buf));
            while (t1)
            {
                t = strpstr(t1, "\n", -1);
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

void FormatBufferFromScratch(COLORIZE_HASH_ENTRY* entries[], INTERNAL_CHAR* buf, int start, int end, int type, int bkColor)
{
    if (type != LANGUAGE_NONE && PropGetBool(NULL, "COLORIZE"))
    {
        int xend, xstart;
        xend = end;
        if (start < 0)
            start = 0;
        xstart = start;
        while (xstart && (buf[xstart - 1].ch != '\n' || (buf[xstart - 1].Color & 0xf) == C_COMMENT))
            xstart--;
        while (buf[xend].ch && (buf[xend].ch != '\n' || (buf[xend].Color & 0xf) == C_COMMENT))
            xend++;

        Colorize(buf, xstart, xend - xstart, (bkColor << 5) | C_TEXT, FALSE);
        FormatBuffer(entries, buf, xstart, xend, type, bkColor);
    }
}

/**********************************************************************
 * FormatLine is an optimized colorizer that just colorizes the current
 * line
 **********************************************************************/

void FormatLine(HWND hwnd, INTERNAL_CHAR* buf, int type, int bkColor)
{
    if (PropGetBool(NULL, "COLORIZE") && type != LANGUAGE_NONE)
    {

        int start, end;
        EDITDATA* p = (EDITDATA*)GetWindowLong(hwnd, 0);
        SendMessage(hwnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
        FormatBufferFromScratch(p->colorizeEntries, buf, start, start, type, bkColor);
    }
}
static void colorizeThread(CINFO* info)
{
    if (info->creation)
    {
        ShowWindow(info->wnd, SW_SHOW);
        SendMessage(info->wnd, EM_GETLINECOUNT, 0, 0);  // update scroll bar
        info->ed->cd->sendchangeonpaint = TRUE;
        SendUpdate(info->wnd);
    }
    FormatBufferFromScratch(info->ed->colorizeEntries, info->ed->cd->text, 0, info->ed->cd->textlen, info->ed->cd->language,
                            info->ed->cd->defbackground);
    SyntaxCheck(info->wnd, info->ed);
    InvalidateRect(info->wnd, 0, 0);
    info->ed->cd->colorizing--;
    free(info);
}
void FullColorize(HWND hwnd, EDITDATA* p, int creation)
{
    CINFO* info = calloc(1, sizeof(CINFO));
    if (info)
    {
        info->ed = p;
        info->wnd = hwnd;
        info->creation = creation;
        SetLastError(0);
        p->cd->colorizing++;
        _beginthread((BEGINTHREAD_FUNC)colorizeThread, 0, (LPVOID)info);
    }
}
static int getColorizeHashKey(char* name)
{
    unsigned n = 0;
    while (*name)
    {
        n = (n << 7) + (n << 1) + n + *name++;
    }
    return n % COLORIZE_HASH_SIZE;
}
static COLORIZE_HASH_ENTRY* lookupColorizeName(COLORIZE_HASH_ENTRY* entries[], char* name)
{
    int key = getColorizeHashKey(name);
    COLORIZE_HASH_ENTRY* entry = entries[key];
    while (entry)
    {
        if (!strcmp(entry->name, name))
            return entry;
        entry = entry->next;
    }
    return entry;
}
static int LookupColorizeEntry(COLORIZE_HASH_ENTRY* entries[], char* name, int line)
{
    int rv = -1;
    COLORIZE_HASH_ENTRY* entry = lookupColorizeName(entries, name);
    if (entry)
    {
        int lastlargest = 0;
        struct _colorize_lines_entry* lines = entry->lines;
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
void InsertColorizeEntry(COLORIZE_HASH_ENTRY* entries[], char* name, int start, int end, int type)
{
    COLORIZE_HASH_ENTRY* entry;
    struct _colorize_lines_entry* lines;
    if (name[0] == '_')
    {
        name++;
    }
    else if (name[0] == '@')
    {
        int nesting = 0;
        char* last = name;
        while (*name)
        {
            if (*name == '#')
                nesting++;
            else if (*name == '~' && nesting)
                nesting--;
            else if (*name == '@')
                last = name;
            name++;
        }
        nesting = 0;
        name = last + 1;
        while (*last)
        {
            if (*last == '#')
                nesting++;
            else if (*last == '~' && nesting)
                nesting--;
            else if (*last == '$')
                break;
            last++;
        }
        *last = '\0';
    }
    if (name[0] == '#')
    {
        char* last = ++name;
        while (*last && *last != '$')
            last++;
        *last = '\0';
    }
    entry = lookupColorizeName(entries, name);
    if (!entry)
    {
        int key = getColorizeHashKey(name);
        entry = (COLORIZE_HASH_ENTRY*)calloc(sizeof(COLORIZE_HASH_ENTRY), 1);
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
    lines = calloc(sizeof(struct _colorize_lines_entry), 1);
    lines->start = start;
    lines->end = end;
    lines->type = type;
    lines->next = entry->lines;
    entry->lines = lines;
}
void FreeColorizeEntries(COLORIZE_HASH_ENTRY* entries[])
{
    int i;
    for (i = 0; i < COLORIZE_HASH_SIZE; i++)
    {
        COLORIZE_HASH_ENTRY* he = entries[i];
        entries[i] = NULL;
        while (he)
        {
            COLORIZE_HASH_ENTRY* next = he->next;
            struct _colorize_lines_entry* le = he->lines;
            while (le)
            {
                struct _colorize_lines_entry* lnext = le->next;
                free(le);
                le = lnext;
            }
            free(he->name);
            free(he);
            he = next;
        }
    }
}
static BOOL IsOperator(INTERNAL_CHAR* p)
{
    switch (p->ch)
    {
        case '+':
        case '-':
        case '=':
        case '/':
        case '*':
        case '&':
        case '|':
        case '^':
        case '%':
        case '~':
        case '!':
        case '<':
        case '>':
        case '.':
        case '?':
            return TRUE;
        case ',':
            return matchCount ? TRUE : FALSE;
        default:
            return FALSE;
    }
}
static BOOL IsReturn(INTERNAL_CHAR* p)
{
    DWORD len;
    if (p->ch == 'c' && p[1].ch == 'a' && !pcmp(p, "case", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 'n' && p[1].ch == 'e' && !pcmp(p, "new", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 'd' && p[1].ch == 'e' && !pcmp(p, "delete", FALSE, &len, TRUE, FALSE))
        return TRUE;
    return p->ch == 'r' && p[1].ch == 'e' && !pcmp(p, "return", FALSE, &len, TRUE, FALSE);
}
static BOOL IsElse(INTERNAL_CHAR* p)
{
    DWORD len;
    return p->ch == 'e' && p[1].ch == 'l' && !pcmp(p, "else", FALSE, &len, TRUE, FALSE);
}
static BOOL IsFreeControl(INTERNAL_CHAR* p)
{
    DWORD len;
    //        if (p->ch == 'e' && p[1].ch == 'l' && !pcmp(p, "else", FALSE, &len, TRUE, FALSE))
    //            return TRUE;
    if (p->ch == 'b' && p[1].ch == 'r' && !pcmp(p, "break", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 'c' && p[1].ch == 'o' && !pcmp(p, "continue", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 'd' && p[1].ch == 'e' && !pcmp(p, "default", FALSE, &len, TRUE, FALSE))
        return TRUE;
    //        if (p->ch == 't' && p[1].ch == 'r' && !pcmp(p, "try", FALSE, &len, TRUE, FALSE))
    //            return TRUE;
    if (p->ch == 'd' && p[1].ch == 'o')
        return TRUE;
    return FALSE;
}
static BOOL IsBoundControl(INTERNAL_CHAR* p)
{
    int len;
    if (p->ch == 'w' && p[1].ch == 'h' && !pcmp(p, "while", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 'f' && p[1].ch == 'o' && !pcmp(p, "for", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 'c' && p[1].ch == 'a' && !pcmp(p, "catch", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 's' && p[1].ch == 'w' && !pcmp(p, "switch", FALSE, &len, TRUE, FALSE))
        return TRUE;
    if (p->ch == 'i' && p[1].ch == 'f')
        return TRUE;
    return FALSE;
}
static BOOL IsType(INTERNAL_CHAR* p)
{
    return FALSE;  // can't do the below without knowing typedefs...
                   //        DWORD retlen = 0;
    //        return !!matchkeyword(Ctype_keywordList, sizeof(Ctype_keywordList)/sizeof(Ctype_keywordList[0]), FALSE, p, &retlen,
    //        FALSE);
}
#define EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL 0
#define FOUND_FREE_CONTROL 1
#define FOUND_FREE_CONTROL_END 2
#define FOUND_BOUND_CONTROL 3
#define FOUND_BOUND_CONTROL_END 4
#define FOUND_RETURN 5
#define FOUND_RETURN_END 6
#define FOUND_ID_START 7
#define FOUND_ID_CHAR 8
#define FOUND_ID_END 9
#define FOUND_TYPE_START 10
#define FOUND_TYPE_CHAR 11
#define FOUND_TYPE_END 12
#define FOUND_DIGIT 13
#define FOUND_DIGIT_END 14
#define IN_MATCH 15
#define FOUND_OPERATOR 16
#define FOUND_OPERATOR_END 17
#define FOUND_STRING 18
#define FOUND_STRING_END 19
#define FOUND_CONTROL_BINDING 20
#define FOUND_ELSE 21
#define FOUND_ELSE_END 22
#define SKIM_TO_EOL 23
#define NEED_SEMI 100

void SyntaxCheck(HWND hWnd, EDITDATA* p)
{
    if (p->cd->language != LANGUAGE_C && p->cd->language != LANGUAGE_CPP)
        return;
    BOOL inFor = FALSE;
    BOOL changed = FALSE;
    char matching[1024];
    int matchIndex[1024];
    int matchCount = 0;
    int braceIndex[1024];
    int braceCount = 0;

    char instring = 0;

    int semiPos = 0;
    int semiState = 0, oldSemiState = 0, oldStringSemiState = 0;
    BOOL erred = FALSE;
    BOOL functionMatch = FALSE;
    INTERNAL_CHAR* ptr = p->cd->text;
    matchCount = 0;
    for (int i = 0; i < p->cd->textlen; i++, ptr++)
    {
        if (ptr->squiggle)
            changed = TRUE;

        ptr->squiggle = FALSE;
        if ((ptr->Color & 0xf) != C_COMMENT)
        {
            if (instring && ptr->ch == '\n')
            {
                instring = 0;
                ptr[-1].squiggle = TRUE;
                changed = TRUE;
            }
            if (semiState == SKIM_TO_EOL)
            {
                if (ptr->ch == '\n')
                    semiState = oldSemiState;
            }
            else if (ptr->ch == '"' || ptr->ch == '\'')
            {
                int n = i;
                while (n > 0 && p->cd->text[n - 1].ch == '\\')
                    n--;
                if (!((i - n) & 1))
                {
                    if (instring == ptr->ch)
                    {
                        instring = 0;
                        semiState = oldStringSemiState;
                        if (semiState == FOUND_OPERATOR_END)
                            semiState = FOUND_DIGIT_END;
                    }
                    else if (!instring)
                    {
                        instring = ptr->ch;
                        switch (semiState)
                        {
                            case FOUND_FREE_CONTROL:
                            case FOUND_BOUND_CONTROL:
                            case FOUND_DIGIT:
                                semiState = NEED_SEMI;
                                break;
                            default:
                                oldStringSemiState = semiState;
                                semiState = FOUND_STRING;
                                break;
                        }
                    }
                }
            }
            else if (!instring)
            {
                if (ptr->ch == '#')
                {
                    if (i == 0)
                    {
                        oldSemiState = semiState;
                        semiState = SKIM_TO_EOL;
                    }
                    else
                    {
                        INTERNAL_CHAR* q = ptr - 1;
                        while (q > p->cd->text && isspace(q->ch))
                        {
                            if (q->ch == '\n')
                            {
                                oldSemiState = semiState;
                                semiState = SKIM_TO_EOL;
                                break;
                            }
                            q--;
                        }
                    }
                    if (ptr->squiggle)
                        changed = TRUE;

                    ptr->squiggle = FALSE;
                }
                if (!matchCount)
                    switch (semiState)
                    {
                        case FOUND_CONTROL_BINDING:
                        case EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL:
                            if (IsType(ptr))
                                semiState = FOUND_TYPE_START;
                            else if (ptr->ch == ',')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else if (ptr->ch == '*' || ptr->ch == '&')
                                semiState = FOUND_OPERATOR;
                            else if (ptr->ch == '_')
                                semiState = FOUND_TYPE_START;
                            else if (isalpha(ptr->ch))
                            {
                                if (IsElse(ptr))
                                    semiState = FOUND_ELSE;
                                else if (IsReturn(ptr))
                                    semiState = FOUND_RETURN;
                                else if (IsFreeControl(ptr))
                                    semiState = FOUND_FREE_CONTROL;
                                else if (IsBoundControl(ptr))
                                    semiState = FOUND_BOUND_CONTROL;
                                else
                                    semiState = FOUND_TYPE_START;
                            }
                            else if (isdigit(ptr->ch))
                                semiState = FOUND_DIGIT;
                            else if (ptr->ch == '(' || ptr->ch == '[')
                                semiState = IN_MATCH;
                            else if (isspace(ptr->ch))
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else if (ptr->ch != '{' && ptr->ch != '}' && ptr->ch != ';' && ptr->ch != ',' && ptr->ch != ':')
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_TYPE_START:
                        case FOUND_TYPE_CHAR:
                            if (isalnum(ptr->ch) || isdigit(ptr->ch) || ptr->ch == '_')
                                semiState = FOUND_TYPE_CHAR;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_TYPE_END;
                            else if (ptr->ch == '(' || ptr->ch == '[')
                                semiState = IN_MATCH;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else if (ptr->ch == ';' || (ptr->ch == ',' && !matchCount) || ptr->ch == ':' || ptr->ch == '}' ||
                                     ptr->ch == '{')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_TYPE_END:
                            if (IsElse(ptr))
                                semiState = NEED_SEMI;
                            else if (IsReturn(ptr))
                                semiState = NEED_SEMI;
                            else if (IsFreeControl(ptr))
                                semiState = NEED_SEMI;
                            else if (IsBoundControl(ptr))
                                semiState = NEED_SEMI;
                            else if (IsType(ptr))
                                semiState = FOUND_TYPE_START;
                            else if (isalpha(ptr->ch) || ptr->ch == '_')
                                semiState = FOUND_TYPE_START;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_TYPE_END;
                            else if (ptr->ch == '(' || ptr->ch == '[')
                                semiState = IN_MATCH;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else if (ptr->ch == ';' || (ptr->ch == ',' && !matchCount) || ptr->ch == ':' || ptr->ch == '}' ||
                                     ptr->ch == '{')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else if (!isspace(ptr->ch))
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_ID_START:
                        case FOUND_ID_CHAR:
                            if (isalnum(ptr->ch) || isdigit(ptr->ch) || ptr->ch == '_')
                                semiState = FOUND_ID_CHAR;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_ID_END;
                            else if (ptr->ch == '(' || ptr->ch == '[')
                                semiState = IN_MATCH;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else if (ptr->ch == ';' || (ptr->ch == ',' && !matchCount) || ptr->ch == ':' || ptr->ch == '}' ||
                                     ptr->ch == '{')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            break;
                        case FOUND_ID_END:
                            if (IsElse(ptr))
                                semiState = NEED_SEMI;
                            else if (IsReturn(ptr))
                                semiState = NEED_SEMI;
                            else if (IsFreeControl(ptr))
                                semiState = NEED_SEMI;
                            else if (IsBoundControl(ptr))
                                semiState = NEED_SEMI;
                            else if (ptr->ch == '(' || ptr->ch == '[')
                                semiState = IN_MATCH;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_ID_END;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else if (ptr->ch == ';' || (ptr->ch == ',' && !matchCount) || ptr->ch == ':' || ptr->ch == '}' ||
                                     ptr->ch == '{')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else if (!isspace(ptr->ch))
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_FREE_CONTROL:
                            if (!isalpha(ptr->ch))
                            {
                                if (ptr->ch == ';' || (ptr->ch == ',' && !matchCount) || ptr->ch == ':' || ptr->ch == '}')
                                    semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                                else if (isspace(ptr->ch))
                                    semiState = FOUND_FREE_CONTROL_END;
                                else
                                    semiState = NEED_SEMI;
                            }
                            break;
                        case FOUND_FREE_CONTROL_END:
                            if (!isspace(ptr->ch))
                            {
                                if (ptr->ch == ';' || (ptr->ch == ',' && !matchCount) || ptr->ch == ':' || ptr->ch == '}')
                                    semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                                else
                                    semiState = NEED_SEMI;
                            }
                            break;
                        case FOUND_BOUND_CONTROL:
                            if (!isalpha(ptr->ch))
                            {
                                if (ptr->ch == '(')
                                {
                                    semiState = FOUND_CONTROL_BINDING;
                                }
                                else if (isspace(ptr->ch))
                                {
                                    semiState = FOUND_BOUND_CONTROL_END;
                                }
                                else
                                    semiState = NEED_SEMI;
                            }
                            break;
                        case FOUND_BOUND_CONTROL_END:
                            if (ptr->ch == '(')
                            {
                                semiState = FOUND_CONTROL_BINDING;
                            }
                            else if (!isspace(ptr->ch))
                            {
                                semiState = NEED_SEMI;
                            }
                            break;
                        case IN_MATCH:
                            if (isspace(ptr->ch))
                                if (ptr->ch == '\n')
                                {
                                    INTERNAL_CHAR* p = ptr + 1;
                                    while (isspace(p->ch))
                                        p++;
                                    if (p->ch == '{')
                                        semiState = IN_MATCH;
                                    else
                                        semiState = NEED_SEMI;
                                }
                                else
                                {
                                    semiState = IN_MATCH;
                                }
                            else if (isalpha(ptr->ch) || ptr->ch == '_')
                                semiState = FOUND_ID_START;
                            else if (ptr->ch == ';' || ptr->ch == ',' || ptr->ch == '{' || ptr->ch == '}' || ptr->ch == ':')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_ELSE:
                            if (!isalpha(ptr->ch))
                            {
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            }
                            break;
                        case FOUND_RETURN:
                            if (!isalpha(ptr->ch))
                            {
                                if (ptr->ch == ';' || ptr->ch == ',' || ptr->ch == '}' || ptr->ch == ':')
                                    semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                                else if (isspace(ptr->ch))
                                    semiState = FOUND_RETURN_END;
                                else
                                    semiState = NEED_SEMI;
                            }
                            break;
                        case FOUND_RETURN_END:
                            if (IsElse(ptr))
                                semiState = FOUND_ELSE;
                            else if (IsReturn(ptr))
                                semiState = NEED_SEMI;
                            else if (IsFreeControl(ptr))
                                semiState = NEED_SEMI;
                            else if (IsBoundControl(ptr))
                                semiState = NEED_SEMI;
                            else if (ptr->ch == '(' || ptr->ch == '[')
                                semiState = IN_MATCH;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_RETURN_END;
                            else if (isalpha(ptr->ch) || ptr->ch == '_')
                                semiState = FOUND_ID_START;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else if (isdigit(ptr->ch))
                                semiState = FOUND_DIGIT;
                            else if (ptr->ch == ';' || ptr->ch == ',' || ptr->ch == ':' || ptr->ch == '}')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_DIGIT:
                            if (isxdigit(ptr->ch) || ptr->ch == 'x' || ptr->ch == 'X')
                                semiState = FOUND_DIGIT;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_DIGIT_END;
                            else if (ptr->ch == ';' || ptr->ch == ',' || ptr->ch == ':' || ptr->ch == '}')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_DIGIT_END:
                            if (ptr->ch == ';' || ptr->ch == ',')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_DIGIT_END;
                            else if (IsOperator(ptr))
                                semiState = FOUND_OPERATOR;
                            else if (ptr->ch == ';' || ptr->ch == ':' || ptr->ch == '}')
                                semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                            else
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_OPERATOR:
                            if (IsReturn(ptr) && ptr->ch == 'n')  // new
                                semiState = FOUND_RETURN;
                            else if (isdigit(ptr->ch))
                                semiState = FOUND_DIGIT;
                            else if (isalpha(ptr->ch) || ptr->ch == '_')
                                semiState = FOUND_ID_START;
                            else if (ptr->ch == '(')
                                semiState = IN_MATCH;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_OPERATOR_END;
                            else if (ptr->ch == ';' || ptr->ch == ',' || ptr->ch == '}')
                            {
                                INTERNAL_CHAR* p1 = ptr - 1;
                                while (p1 > p->cd->text && isspace(p1->ch))
                                    p1--;
                                if (p1->ch != '+' && p1->ch != '-')
                                    semiState = NEED_SEMI;
                                else if (p1[-1].ch != p1->ch)
                                    semiState = NEED_SEMI;
                            }
                            else if (!IsOperator(ptr))
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_OPERATOR_END:
                            if (IsReturn(ptr) && ptr->ch == 'n')  // new
                                semiState = FOUND_RETURN;
                            else if (isdigit(ptr->ch))
                                semiState = FOUND_DIGIT;
                            else if (isalpha(ptr->ch) || ptr->ch == '_')
                                semiState = FOUND_ID_START;
                            else if (ptr->ch == '(')
                                semiState = IN_MATCH;
                            else if (isspace(ptr->ch))
                                semiState = FOUND_OPERATOR_END;
                            else if (ptr->ch == ';' || ptr->ch == ',' || ptr->ch == '}')
                            {
                                INTERNAL_CHAR* p1 = ptr - 1;
                                while (p1 > p->cd->text && isspace(p1->ch))
                                    p1--;
                                if (p1->ch != '+' && p1->ch != '-')
                                    semiState = NEED_SEMI;
                                else if (p1[-1].ch != p1->ch)
                                    semiState = NEED_SEMI;
                            }
                            else if (!IsOperator(ptr))
                                semiState = NEED_SEMI;
                            break;
                        case FOUND_STRING:
                        case FOUND_STRING_END:
                            break;
                        case NEED_SEMI:
                            break;
                    }
                if (ptr->ch == '{' || ptr->ch == '}' || (ptr->ch == ',' && !matchCount) || (ptr->ch == ';' && !inFor))
                {
                    if ((ptr->ch == '{' && functionMatch) || semiState != NEED_SEMI)
                        semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                    if (matchCount && !erred)
                    {
                        p->cd->text[matchIndex[matchCount - 1]].squiggle = TRUE;
                        changed = TRUE;
                    }
                    matchCount = 0;
                    erred = FALSE;
                }
                else if (ptr->ch == '[' || ptr->ch == '(')
                {
                    if (matchCount == 0)
                    {
                        inFor = FALSE;
                        if (ptr->ch == '(')
                        {
                            int n = i - 1;
                            while (n > 2 && isspace(p->cd->text[n].ch))
                                n--;
                            if (n > 1)
                            {
                                if (p->cd->text[n].ch == 'r' && p->cd->text[n - 1].ch == 'o' && p->cd->text[n - 2].ch == 'f')
                                    inFor = TRUE;
                            }
                        }
                    }
                    matchIndex[matchCount] = i;
                    matching[matchCount++] = ptr->ch;
                }
                else if (ptr->ch == ']')
                {
                    if (!matchCount)
                    {
                        ptr->squiggle = TRUE;
                        changed = TRUE;
                        erred = TRUE;
                    }
                    else if (matching[matchCount - 1] != '[')
                    {
                        p->cd->text[matchIndex[matchCount - 1]].squiggle = TRUE;
                        changed = TRUE;
                        erred = TRUE;
                    }
                    else
                    {
                        matchCount--;
                    }
                }
                else if (ptr->ch == ')')
                {
                    if (!matchCount)
                    {
                        ptr->squiggle = TRUE;
                        changed = TRUE;
                        erred = TRUE;
                    }
                    else if (matching[matchCount - 1] != '(')
                    {
                        p->cd->text[matchIndex[matchCount - 1]].squiggle = TRUE;
                        changed = TRUE;
                        erred = TRUE;
                    }
                    else
                    {
                        matchCount--;
                        if (!matchCount)
                            functionMatch = TRUE;
                    }
                }
                else if (ptr->ch == '=')
                    functionMatch = TRUE;
                else if (!isspace(ptr->ch))
                    functionMatch = FALSE;
                if (semiState == NEED_SEMI)
                {
                    semiState = EXPECT_NONEXPRESSION_OR_BRACE_OR_CONTROL;
                    changed = TRUE;
                    p->cd->text[semiPos].squiggle = TRUE;
                }
                if (!isspace(ptr->ch))
                    semiPos = i;
            }
            else
                functionMatch = FALSE;
        }
    }
    if (changed)
        InvalidateRect(hWnd, 0, 0);
}