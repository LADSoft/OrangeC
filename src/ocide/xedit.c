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
extern HWND hwndFrame;
extern SCOPE* activeScope;
extern LOGFONT systemDialogFont;
extern HWND codecompleteBox;
extern BOOL inStructBox;
extern HWND hwndShowFunc;
extern COLORREF keywordColor;
extern COLORREF numberColor;
extern COLORREF commentColo;
extern COLORREF stringColor;
extern COLORREF directiveColor;
extern COLORREF backgroundColor;
extern COLORREF readonlyBackgroundColor;
extern COLORREF textColor;
extern COLORREF highlightColor;
extern COLORREF selectedTextColor;
extern COLORREF selectedBackgroundColor;
extern COLORREF columnbarColor;

extern COLORREF defineColor;
extern COLORREF functionColor;
extern COLORREF parameterColor;
extern COLORREF typedefColor;
extern COLORREF tagColor;
extern COLORREF autoColor;
extern COLORREF localStaticColor;
extern COLORREF staticColor;
extern COLORREF globalColor;
extern COLORREF externColor;
extern COLORREF labelColor;
extern COLORREF memberColor;

extern COLORREF* colors[];

// This defines the maximum range for the horizontal scroll bar on the window
#define MAX_HSCROLL 256

// The KEYLIST structure holds information about text which should be
// colorized.  It gives the name of a keyword (e.g. 'int') and the color
// to display it in.

LOGFONT EditFont = {-13,
                    0,
                    0,
                    0,
                    FW_NORMAL,
                    FALSE,
                    FALSE,
                    FALSE,
                    ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY,
                    FIXED_PITCH | FF_DONTCARE,
                    CONTROL_FONT};
// For drawing, put a little margin at the left
#define LEFTMARGIN 5

extern HWND hwndFind;
// well we aren't 'totally' independent of the rest of CCIDE...
// the following is used for the tooltip
extern enum DebugState uState;
extern HINSTANCE hInstance;
extern int progLanguage;

static int page_size = -1;

/* using windows to allocate memory because the borland runtime gets confused
 * with large amounts of data
 */
#define SYSALLOC
void getPageSize()
{
    if (page_size == -1)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        page_size = info.dwPageSize;
    }
}
int freemem(EDITDATA* p)
{
#ifdef SYSALLOC
    VirtualFree(p->cd->text, p->cd->textmaxlen, MEM_DECOMMIT);
    VirtualFree(p->cd->text, 0, MEM_RELEASE);
#else
    free(p->cd->text);
#endif
    return 0;
}
int allocmem(EDITDATA* p, int size)
{
    size *= sizeof(INTERNAL_CHAR);
    size += 64 * 1024;
    size = size - size % (64 * 1024);
    if (!p->cd->text)
    {
#ifdef SYSALLOC
        p->cd->text = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
#else
        p->cd->text = calloc(1, size);
#endif
        if (!p->cd->text)
        {
            ExtendedMessageBox("Error", 0, "Out Of Memory");
            return 0;
        }
        p->cd->textmaxlen = size;
        return 1;
    }
    else
    {
        INTERNAL_CHAR* x;
#ifdef SYSALLOC
        x = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
        if (!x || !VirtualAlloc(x, (p->cd->textlen + 1) * sizeof(INTERNAL_CHAR), MEM_COMMIT, PAGE_READWRITE))
#else
        x = calloc(1, size);
        if (!x)
#endif
        {
            ExtendedMessageBox("Error", 0, "Out Of Memory");
            return 0;
        }
        memcpy(x, p->cd->text, (p->cd->textlen + 1) * sizeof(INTERNAL_CHAR));
        freemem(p);
        p->cd->text = x;
        p->cd->textmaxlen = size;
        return 1;
    }
}
int commitmem(EDITDATA* p, int size)
{
    int size1 = size * sizeof(INTERNAL_CHAR);
    getPageSize();
    size1 += page_size;
    size1 = size1 - size1 % page_size;
    if (size1 > p->cd->textmaxlen)
    {
        if (!allocmem(p, size))
            return 0;
    }
#ifdef SYSALLOC
    if (!VirtualAlloc(p->cd->text, size1, MEM_COMMIT, PAGE_READWRITE))
    {
        ExtendedMessageBox("Error", 0, "Cannot commit memory");
        return 0;
    }
#endif
    return 1;
}
void reparse(HWND hwnd, EDITDATA* p)
{
    if (p->cd->reparseTimerID)
        KillTimer(hwnd, p->cd->reparseTimerID);
    p->cd->reparseTimerID = SetTimer(hwnd, 2, 2500, 0);
}
static void UpdateSiblings(EDITDATA* p, int pos, int insert)
{
    void UpdateMarks(EDITDATA * p, int pos, int insert);
    EDLIST* list = p->cd->siblings;
    while (list)
    {
        if (list->data != p)
        {
            SendMessage(list->data->self, EM_UPDATESIBLING, insert, pos);
        }
        else
        {
            UpdateMarks(p, pos, insert);
        }
        list = list->next;
    }
}

/**********************************************************************
 * posfromchar determines the screen position of a given offset in
 * the buffer
 **********************************************************************/
int posfromchar(HWND hwnd, EDITDATA* p, POINTL* point, int pos)
{
    char buf[256], *x = buf;
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
        point->x = (xcol - p->leftshownindex) * p->cd->txtFontWidth;
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
int charfrompos(HWND hwnd, EDITDATA* p, POINTL* point)
{
    RECT r;
    int row, col, xcol = 0;
    int pos = p->textshowncharpos, i = 0;
    char buf[256], *x = buf;
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
int curcol(EDITDATA* p, INTERNAL_CHAR* text, int pos)
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
            rv = (rv / p->cd->tabs) * p->cd->tabs;
        }
        else
            rv++;
        pos++;
    }
    return rv;
}

//-------------------------------------------------------------------------

void setcurcol(EDITDATA* p)
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

void MoveCaret(HWND hwnd, EDITDATA* p)
{
    int x = 0, y = 0;
    POINTL pt;
    if (posfromchar(hwnd, p, &pt, p->selstartcharpos != p->selendcharpos ? p->selendcharpos : p->selstartcharpos))
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
void scrollleft(HWND hwnd, EDITDATA* p, int cols)
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

void scrollup(HWND hwnd, EDITDATA* p, int lines)
{
    RECT r, update;
    int totallines, movelines = lines;
    int pos = p->textshowncharpos, len = 0;
    ClientArea(hwnd, p, &r);
    totallines = r.bottom / p->cd->txtFontHeight;
    if (lines < 0)
    {
        lines = -lines;
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
            if (-movelines - lines)
                ScrollWindowEx(hwnd, 0, (-movelines - lines) * p->cd->txtFontHeight, &r, &r, 0, &update, SW_INVALIDATE);
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
                ScrollWindowEx(hwnd, 0, -(movelines - lines) * p->cd->txtFontHeight, &r, &r, 0, &update, SW_INVALIDATE);
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
void ScrollCaretIntoView(HWND hwnd, EDITDATA* p, BOOL middle)
{
    POINTL pt;
    int lines, cols, colpos = 0;
    RECT r;
    int pos1, pos;
    pos1 = p->selendcharpos;
    if (posfromchar(hwnd, p, &pt, pos1))
    {
        MoveCaret(hwnd, p);
        return;
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
                scrollup(hwnd, p, xlines - lines / 2);
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
                    scrollup(hwnd, p, -xlines - lines / 2);
                else
                    scrollup(hwnd, p, -xlines);
            }
        }
    }
    pos = pos1;
    while (pos > 0 && p->cd->text[pos - 1].ch != '\n')
        pos--;
    while (pos != pos1)
    {
        if (pos && p->cd->text[pos - 1].ch == '\t')
            colpos = ((colpos + p->cd->tabs) / p->cd->tabs) * p->cd->tabs;
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
void TrackVScroll(HWND hwnd, EDITDATA* p, int end)
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
void TrackHScroll(HWND hwnd, EDITDATA* p, int end)
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
int lfchars(INTERNAL_CHAR* c, int start, int end)
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
int LineFromChar(EDITDATA* p, int pos)
{
    int rv = 0;
    INTERNAL_CHAR* ic = p->cd->text;
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
static int SelLine(EDITDATA* p, int pos)
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
void Replace(HWND hwnd, EDITDATA* p, char* s, int lens)
{
    UNDO* u = 0;
    int i, temp, changed;
    char* s1;
    int len = 0;

    i = p->selendcharpos - p->selstartcharpos;
    changed = lens - i;
    u = undo_deletesel(hwnd, p);
    if (i > 0)
    {
        len -= lfchars(p->cd->text, p->selstartcharpos, p->selendcharpos);
        SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selstartcharpos), len);
        memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p->selendcharpos,
               (p->cd->textlen - p->selendcharpos + 1) * sizeof(INTERNAL_CHAR));
        p->selendcharpos = p->selstartcharpos;
        p->cd->textlen -= i;
        UpdateSiblings(p, p->selstartcharpos, -i);
    }
    else if (i < 0)
    {
        temp = lfchars(p->cd->text, p->selendcharpos, p->selstartcharpos);
        SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selendcharpos), -temp);
        VScrollPos(hwnd, -temp, FALSE);
        len -= temp;
        memcpy(p->cd->text + p->selendcharpos, p->cd->text + p->selstartcharpos,
               (p->cd->textlen - p->selstartcharpos + 1) * sizeof(INTERNAL_CHAR));
        p->selstartcharpos = p->selendcharpos;
        p->cd->textlen += i;
        UpdateSiblings(p, p->selstartcharpos, i);
    }
    if (u)
    {
        u->postselstart = u->postselend = p->selstartcharpos;
    }
    if (!commitmem(p, p->cd->textlen + lens))
    {
        p->selendcharpos = p->selstartcharpos;
        SendUpdate(hwnd);
        p->cd->sendchangeonpaint = TRUE;
        InvalidateRect(hwnd, 0, 0);
        return;
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
    SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selstartcharpos), temp);
    memmove(p->selstartcharpos + lens + p->cd->text, p->cd->text + p->selstartcharpos,
            (p->cd->textlen - p->selstartcharpos + 1) * sizeof(INTERNAL_CHAR));
    memset(p->selstartcharpos + p->cd->text, 0, (lens) * sizeof(INTERNAL_CHAR));
    UpdateSiblings(p, p->selstartcharpos, lens);
    i = 0;
    while (*s && i < lens)
    {
        if (*s == '\n')
            len++;
        p->cd->text[p->selstartcharpos + i].ch = *s++;
        p->cd->text[p->selstartcharpos + i].Color = (p->cd->defbackground << 5) + p->cd->defforeground;
        i++;
        p->cd->textlen++;
    }
    VScrollLen(hwnd, len, FALSE);
    FormatBufferFromScratch(p->colorizeEntries, p->cd->text, p->selstartcharpos - 1, p->selstartcharpos + lens + 1, p->cd->language,
                            p->cd->defbackground);
    SendUpdate(hwnd);
    p->cd->sendchangeonpaint = TRUE;
    if (p->selstartcharpos < p->textshowncharpos)
    {
        p->textshowncharpos += changed;
        if (p->textshowncharpos < 0)
            p->textshowncharpos = 0;
        while (p->textshowncharpos && p->cd->text[p->textshowncharpos - 1].ch != '\n')
            p->textshowncharpos--;
    }
    InvalidateRect(hwnd, 0, 0);
    reparse(hwnd, p);
}

/**********************************************************************
 * GetLineOffset returns the line number, this time as an offset from
 * the first line shown at the top of the window
 **********************************************************************/
int GetLineOffset(HWND hwnd, EDITDATA* p, int chpos)
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
 * insertchar handles the functionality of inserting a character
 * will also cut a previous selection
 **********************************************************************/
void insertchar(HWND hwnd, EDITDATA* p, int ch)
{
    int len = 0, temp;
    UNDO* u = 0;
    if (p->cd->inserting || p->cd->text[p->selstartcharpos].ch == '\n')
    {
        int i = p->selendcharpos - p->selstartcharpos;
        u = undo_deletesel(hwnd, p);
        if (i > 0)
        {
            len -= lfchars(p->cd->text, p->selstartcharpos, p->selendcharpos);
            SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selstartcharpos), len);
            memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p->selendcharpos,
                   (p->cd->textlen - p->selendcharpos + 1) * sizeof(INTERNAL_CHAR));
            p->cd->textlen -= i;
            UpdateSiblings(p, p->selstartcharpos, -i);
        }
        else if (i < 0)
        {
            temp = lfchars(p->cd->text, p->selendcharpos, p->selstartcharpos);
            SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selendcharpos), -temp);
            VScrollPos(hwnd, -temp, FALSE);
            len -= temp;
            memcpy(p->cd->text + p->selendcharpos, p->cd->text + p->selstartcharpos,
                   (p->cd->textlen - p->selstartcharpos + 1) * sizeof(INTERNAL_CHAR));
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
            SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selstartcharpos), 1);
        }
        if (!commitmem(p, p->cd->textlen + 2))
        {
            SendUpdate(hwnd);
            p->cd->sendchangeonpaint = TRUE;
            InvalidateRect(hwnd, 0, 0);
            return;
        }
        u = undo_insertchar(hwnd, p, ch);
        memmove(p->cd->text + p->selstartcharpos + 1, p->cd->text + p->selstartcharpos,
                (p->cd->textlen - p->selstartcharpos + 1) * sizeof(INTERNAL_CHAR));
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
void insertcrtabs(HWND hwnd, EDITDATA* p);
void insertcr(HWND hwnd, EDITDATA* p, BOOL tabs)
{
    int temp;
    xdrawline(hwnd, p, p->selstartcharpos);  // undefined in local context
    if (p->selstartcharpos > p->selendcharpos)
    {
        temp = -p->selstartcharpos;
        p->selstartcharpos = p->selendcharpos;
    }
    else
    {
        temp = p->selendcharpos;
        p->selendcharpos = p->selstartcharpos;
    }
    if (PropGetBool(NULL, "AUTO_INDENT"))
    {
        BOOL realinsert = p->cd->text[p->selstartcharpos].ch != '\n';
        int pos = 0;
        int n = p->selstartcharpos, parenpos = -1;
        POINTL pt;
        int first = 0, hasend = 0, noend = 0, hasbegin = 0, nesting = 0;
        insertautoundo(hwnd, p, UNDO_AUTOEND);
        insertchar(hwnd, p, '\n');
        if (temp < 0)
        {
            p->selstartcharpos = -temp + 1;
        }
        else
        {
            p->selendcharpos = temp + 1;
        }
        while (n > 0 && p->cd->text[n].ch == '\n')
            first++, n--;
        while (n > 0 && (p->cd->text[n].ch != '\n' || nesting))
        {

            if (p->cd->text[n].ch == ')')
                nesting++;
            else if (p->cd->text[n].ch == '(')
            {
                if (nesting)
                    nesting--;
                else
                    parenpos = n;
            }
            if (!isspace(p->cd->text[n].ch) && !noend)
            {
                if (p->cd->text[n].ch == '}')
                    hasend = TRUE;
                else if (p->cd->text[n].ch == '{')
                    hasbegin = TRUE;
                else
                    noend = TRUE;
            }
            n--;
        }
        if (n > 0)
            n++;
        while (n < p->cd->textlen && isspace(p->cd->text[n].ch) && p->cd->text[n].ch != '\n')
        {
            if (p->cd->text[n].ch == '\t')
            {
                if (p->cd->tabs)
                {
                    pos += p->cd->tabs;
                    pos /= p->cd->tabs;
                    pos *= p->cd->tabs;
                }
                else
                    pos++;
            }
            else
            {
                pos++;
            }
            n++;
        }

        if (hasbegin && !hasend)
        {
            if (p->cd->tabs)
            {
                pos += p->cd->tabs;
                pos /= p->cd->tabs;
                pos *= p->cd->tabs;
            }
        }
        else if (first)
        {
            while (n < p->cd->textlen && p->cd->text[n].ch != '\n')
            {
                if (!isspace(p->cd->text[n].ch))
                    break;
                n++;
            }
            if (parenpos != -1 && n < parenpos)
            {
                while (n < parenpos)
                {
                    if (p->cd->text[n].ch == '\t')
                    {
                        if (p->cd->tabs)
                        {
                            pos += p->cd->tabs;
                            pos /= p->cd->tabs;
                            pos *= p->cd->tabs;
                        }
                        else
                            pos++;
                    }
                    else
                    {
                        pos++;
                    }
                    n++;
                }
            }

            else if (firstword(p->cd->text + n, "if") || firstword(p->cd->text + n, "else") || firstword(p->cd->text + n, "do") ||
                     firstword(p->cd->text + n, "for") || firstword(p->cd->text + n, "while") ||
                     firstword(p->cd->text + n, "case") || firstword(p->cd->text + n, "default"))

            {
                if (p->cd->tabs)
                {
                    pos += p->cd->tabs;
                    pos /= p->cd->tabs;
                    pos *= p->cd->tabs;
                }
            }
        }
        if (posfromchar(hwnd, p, &pt, p->selstartcharpos != p->selendcharpos ? p->selendcharpos : p->selstartcharpos))
        {
            p->insertcursorcolumn = pos;
            pt.x += p->cd->txtFontWidth * pos;
            SetCaretPos(pt.x, pt.y);
            if (realinsert)
                insertcrtabs(hwnd, p);
        }
        //        insertcrtabs(hwnd, p);
        insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
    }
    else
    {
        insertchar(hwnd, p, '\n');
        if (temp < 0)
        {
            p->selstartcharpos = -temp + 1;
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
void inserttab(HWND hwnd, EDITDATA* p)
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

int firstword(INTERNAL_CHAR* pos, char* name)
{
    int l = strlen(name);
    int i;
    for (i = 0; i < l; i++)
        if (!pos[i].ch || pos[i].ch != name[i])
            return FALSE;
    if (isalnum(pos[i].ch) || pos[i].ch == '_')
        return FALSE;
    return TRUE;
}

/**********************************************************************
 * go backwards to the last tab position
 **********************************************************************/
void backtab(HWND hwnd, EDITDATA* p)
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

void RemoveCurrentLine(HWND hwnd, EDITDATA* p)
{
    int end = p->selendcharpos;
    int start = p->selendcharpos;
    int oldend = p->selendcharpos;
    int oldstart = p->selendcharpos;
    int n;
    UNDO* u;
    while (end < p->cd->textlen && p->cd->text[end].ch != '\n')
        end++;
    if (end < p->cd->textlen && p->cd->text[end].ch == '\n')
        end++;
    while (start > 0 && p->cd->text[start - 1].ch != '\n')
        start--;
    p->selstartcharpos = start;
    p->selendcharpos = end;
    Replace(hwnd, p, "", 0);
    n = p->cd->undohead - 1;
    if (n < 0)
        n += UNDO_MAX;
    u = &p->cd->undolist[n];
    u->preselstart = oldstart;
    u->preselend = oldend;
}
void RemoveNextWord(HWND hwnd, EDITDATA* p)
{
    int end = p->selendcharpos;
    int start = p->selendcharpos;
    int oldend = p->selendcharpos;
    int oldstart = p->selendcharpos;
    int n;
    UNDO* u;
    if (end < p->cd->textlen && isspace(p->cd->text[end].ch))
    {
        while (end < p->cd->textlen && isspace(p->cd->text[end].ch))
        {
            end++;
        }
    }
    else if ((end < p->cd->textlen && isalpha(p->cd->text[end].ch)) || p->cd->text[end].ch == '_')
    {
        while ((end < p->cd->textlen && isalnum(p->cd->text[end].ch)) || p->cd->text[end].ch == '_')
        {
            end++;
        }
    }
    else if (end < p->cd->textlen)
    {
        end++;
    }
    p->selstartcharpos = start;
    p->selendcharpos = end;

    Replace(hwnd, p, "", 0);
    n = p->cd->undohead - 1;
    if (n < 0)
        n += UNDO_MAX;
    u = &p->cd->undolist[n];
    u->preselstart = oldstart;
    u->preselend = oldend;
}
/**********************************************************************
 * removechar cuts a character from the text (delete or back arrow)
 **********************************************************************/
void removechar(HWND hwnd, EDITDATA* p, int utype)
{
    if (p->insertcursorcolumn)
    {
        if (utype == UNDO_BACKSPACE)
        {
            return;
        }
        else
        {
            insertcrtabs(hwnd, p);
        }
    }
    if (p->cd->inserting && p->selstartcharpos != p->selendcharpos)
    {
        Replace(hwnd, p, "", 0);
        ScrollCaretIntoView(hwnd, p, FALSE);
    }
    else
    {
        int del;
        if (p->selstartcharpos == p->cd->textlen)
            return;
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
                SendMessage(GetParent(hwnd), EN_LINECHANGE, SelLine(p, p->selstartcharpos) + 1, -1);
            }
        }
        p->cd->sendchangeonpaint = TRUE;
        memcpy(p->cd->text + p->selstartcharpos, p->cd->text + p->selstartcharpos + del,
               (p->cd->textlen - p->selstartcharpos - del + 1) * sizeof(INTERNAL_CHAR));
        p->cd->textlen -= del;
        UpdateSiblings(p, p->selstartcharpos, -del);
        ScrollCaretIntoView(hwnd, p, FALSE);
        xdrawline(hwnd, p, p->selstartcharpos);
        reparse(hwnd, p);
    }
    p->selendcharpos = p->selstartcharpos;
}
/**********************************************************************
 * upline scrolls the display down one line
 **********************************************************************/
void upline(HWND hwnd, EDITDATA* p, int lines)
{
    int ilines = lines;
    int pos;
    int col, index = 0;
    p->insertcursorcolumn = 0;
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
            index = (index / p->cd->tabs) * p->cd->tabs;
        }
        else
            index++;
        pos++;
    }
    if (pos && p->cd->text[pos - 1].ch == '\f')
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
/**********************************************************************
 * eol indexes to the last character in a line
 **********************************************************************/
void endofline(HWND hwnd, EDITDATA* p)
{
    int pos;
    p->insertcursorcolumn = 0;
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
void sol(HWND hwnd, EDITDATA* p)
{
    int pos;
    int encns = FALSE;
    p->insertcursorcolumn = 0;
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
void left(HWND hwnd, EDITDATA* p)
{
    if (p->insertcursorcolumn)
    {
        POINT pt;
        p->insertcursorcolumn--;
        if (posfromchar(hwnd, p, &pt, p->selstartcharpos != p->selendcharpos ? p->selendcharpos : p->selstartcharpos))
        {
            pt.x += p->cd->txtFontWidth * p->insertcursorcolumn;
            SetCaretPos(pt.x, pt.y);
        }
        return;
    }
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
            if (pos && p->cd->text[pos - 1].ch == '\f')
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
void right(HWND hwnd, EDITDATA* p)
{
    p->insertcursorcolumn = 0;
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
void leftword(HWND hwnd, EDITDATA* p)
{
    int pos;
    int flag = 0;
    p->insertcursorcolumn = 0;
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
void rightword(HWND hwnd, EDITDATA* p)
{
    int pos;
    int flag = 0;
    p->insertcursorcolumn = 0;
    pos = p->selendcharpos;
    while (pos < p->cd->textlen && (isalnum(p->cd->text[pos].ch) || p->cd->text[pos].ch == '_'))
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
    EDITDATA* p = (EDITDATA*)GetWindowLong(hwnd, 0);
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
void StartAutoScroll(HWND hwnd, EDITDATA* p, int dir)
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
void EndAutoScroll(HWND hwnd, EDITDATA* p)
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
void HilightWord(HWND hwnd, EDITDATA* p)
{
    if (keysym(p->cd->text[p->selstartcharpos].ch))
    {
        while (p->selstartcharpos && keysym(p->cd->text[p->selstartcharpos - 1].ch))
            p->selstartcharpos--;
        while (p->selendcharpos < p->cd->textlen && keysym(p->cd->text[p->selendcharpos].ch))
            p->selendcharpos++;
    }
}
void verticalCenter(HWND hwnd, EDITDATA* p)
{
    RECT bounds;
    int rows, pos, lines = 0;
    ClientArea(hwnd, p, &bounds);
    rows = (bounds.bottom - bounds.top) / p->cd->txtFontHeight; /* find center */
    if (p->selstartcharpos <= p->selendcharpos)
        pos = p->selendcharpos;
    else
        pos = p->selstartcharpos;
    while (--pos > p->textshowncharpos)
    {
        if (p->cd->text[pos].ch == '\n')
            lines++;
    }

    pos = p->textshowncharpos;

    lines = rows / 2 - lines;

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
static BOOL multilineSelect(EDITDATA* p)
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
void UpdateMarks(EDITDATA* p, int pos, int insert)
{
    int i;
    for (i = 0; i < 10; i++)
    {
        if (p->marks[i] >= pos)
            p->marks[i] += insert;
        else if (pos + insert < p->marks[i])
            p->marks[i] = pos + insert;
    }
}
void MarkOrGoto(HWND hwnd, EDITDATA* p, int index, BOOL mark)
{
    if (index >= 0 && index < 10)
    {
        if (mark)
        {
            p->marks[index] = p->selendcharpos;
        }
        else
        {
            if (p->marks[index] >= 0)
            {
                p->selstartcharpos = p->selendcharpos = p->marks[index];
                ScrollCaretIntoView(hwnd, p, TRUE);
            }
        }
    }
}

/**********************************************************************
 * exeditProc is the window proc for the edit control
 **********************************************************************/

LRESULT CALLBACK exeditProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    CREATESTRUCT* lpCreate;
    static char buffer[1024];
    EDITDATA* p;
    DWINFO* x;
    int stop;
    LRESULT rv;
    int i, start, end;
    TOOLTIPTEXT* lpnhead;
    int l;
    DEBUG_INFO* dbg;
    VARINFO* var;
    NMHDR nm;
    int charpos;
    RECT r;
    HDC dc;
    HFONT xfont;
    TEXTMETRIC t;
    INTERNAL_CHAR* ic;
    POINTL pt;
    LOGFONT lf;
    CHARRANGE* ci;
    if (GetWindowLong(hwnd, 0) || iMessage == WM_CREATE)
    {
        switch (iMessage)
        {
            case WM_TIMER:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                KillTimer(hwnd, p->cd->reparseTimerID);
                p->cd->reparseTimerID = 0;
                InstallForParse(GetParent(hwnd));
                SyntaxCheck(hwnd, p);
                break;
                /*
            case WM_NCPAINT:
            {
                HDC dc;
                RECT r;
                HBRUSH brush = GetStockObject(NULL_BRUSH);
                HPEN pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
                DefWindowProc(hwnd, iMessage, wParam, lParam);
                GetWindowRect(hwnd, &r);
                r.right -= r.left;
                r.left -= r.left;
                r.bottom -= r.top;
                r.top -= r.top;
                dc = GetWindowDC(hwnd);
                pen = SelectObject(dc, pen);
                brush = SelectObject(dc, brush);
                Rectangle(dc, r.left, r.top, r.right, r.bottom);
                brush = SelectObject(dc, brush);
                pen = SelectObject(dc, pen);
                DeleteObject(pen);
                ReleaseDC(hwnd, dc);
                return 0;
            }
            */
            case WM_ERASEBKGND:
                return 1;
            case WM_PAINT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->colorizing)
                    EditPaint(hwnd, p);
                return 0;
            case EM_GETINSERTSTATUS:
                return ((EDITDATA*)GetWindowLong(hwnd, 0))->cd->inserting;
            case WM_CODECOMPLETE:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (wParam == '.' || wParam == '>' || wParam == ':')
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
                else
                    switch (lpnhead->hdr.code)
                    {
                        case TTN_SHOW:
                        case TTN_POP:
                            break;
                        case TTN_NEEDTEXT:
                            // tooltip
                            if ((uState == notDebugging && PropGetBool(NULL, "EDITOR_HINTS")) ||
                                ((uState == atException || uState == atBreakpoint) && PropGetBool(NULL, "DEBUGGER_HINTS")))
                            {
                                char name[1000];
                                DWINFO* info = (DWINFO*)GetWindowLong(GetParent(hwnd), 0);
                                int lineno;
                                POINT pt;
                                name[0] = 0;
                                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                                GetCursorPos(&pt);
                                ScreenToClient(hwnd, &pt);
                                charpos = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&pt);
                                GetWordFromPos(hwnd, name, FALSE, charpos, &lineno, &start, &end);
                                lineno++;
                                if (name[0])
                                {
                                    x = (DWINFO*)GetWindowLong(GetParent(hwnd), 0);
                                    if (uState == notDebugging)
                                    {
                                        int flags;
                                        if (ccLookupType(buffer, name, info->dwName, lineno, &flags, NULL))
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
                break;
            case WM_KEYDOWN:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (IsWindowVisible(hwndShowFunc) && !SendMessage(hwndShowFunc, WM_USER + 3, wParam, lParam))
                    break;
                CancelParenMatch(hwnd, p);
                switch (wParam)
                {
                    case VK_ESCAPE:
                        SendMessage(hwndFrame, WM_COMMAND, IDM_CLOSEFIND, 0);
                        if (IsWindowVisible(hwndShowFunc))
                            ShowWindow(hwndShowFunc, SW_HIDE);
                        if (IsWindow(codecompleteBox))
                            SendMessage(codecompleteBox, WM_CLOSE, 0, 0);
                        break;
                    case VK_INSERT:
                        if (!p->cd->readonly)
                        {
                            if (GetKeyState(VK_SHIFT) & 0x80000000)
                            {
                                SendMessage(hwnd, WM_PASTE, 0, 0);
                            }
                            else if (GetKeyState(VK_CONTROL) & 0x80000000)
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
                            int n = p->insertcursorcolumn;
                            if (p->selstartcharpos == 0)
                                break;
                            left(hwnd, p);
                            if (n)
                                break;
                        }
                    case VK_DELETE:
                        if (!p->cd->readonly)
                        {
                            if (GetKeyState(VK_SHIFT) & 0x80000000)
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
                                removechar(hwnd, p, wParam == VK_DELETE ? UNDO_DELETE : UNDO_BACKSPACE);
                                FormatBufferFromScratch(p->colorizeEntries, p->cd->text, p->selstartcharpos, p->selstartcharpos,
                                                        p->cd->language, p->cd->defbackground);
                            }
                            setcurcol(p);
                        }
                        break;
                    case VK_RETURN:
                        if (!p->cd->readonly)
                        {
                            insertcr(hwnd, p, TRUE);
                            FormatBufferFromScratch(p->colorizeEntries, p->cd->text, p->selstartcharpos - 1, p->selstartcharpos,
                                                    p->cd->language, p->cd->defbackground);
                            setcurcol(p);
                            if (IsWindowVisible(hwndShowFunc))
                                scrollup(hwnd, p, 1);
                        }

                        break;
                    case VK_TAB:
                        if (GetKeyState(VK_SHIFT) & 0x80000000)
                        {
                            if (!p->cd->readonly)
                            {
                                if (multilineSelect(p))
                                    SelectIndent(hwnd, p, FALSE);
                                else
                                {
                                    backtab(hwnd, p);
                                    xdrawline(hwnd, p, p->selstartcharpos);
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
                                if (p->insertcursorcolumn)
                                    insertcrtabs(hwnd, p);
                                if (multilineSelect(p))
                                    SelectIndent(hwnd, p, TRUE);
                                else
                                {
                                    inserttab(hwnd, p);
                                    InsertEndTabs(hwnd, p, FALSE);
                                    xdrawline(hwnd, p, p->selstartcharpos);
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
                        upline(hwnd, p, -1);
                        SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                        break;
                    case VK_PRIOR:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
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
                            upline(hwnd, p, 2 - i);
                        }
                        SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);

                        break;
                    case VK_NEXT:
                        ClientArea(hwnd, p, &r);
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
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
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
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
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
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
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                            leftword(hwnd, p);
                        else
                            left(hwnd, p);
                        setcurcol(p);
                        SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                        break;
                    case VK_RIGHT:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                            rightword(hwnd, p);
                        else
                            right(hwnd, p);
                        setcurcol(p);
                        SendMessage(GetParent(hwnd), EN_SETCURSOR, 0, 0);
                        break;
                    case 'A':
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            p->selstartcharpos = 0;
                            p->selendcharpos = p->cd->textlen;
                            InvalidateRect(hwnd, 0, 0);
                        }
                        break;
                    case 'X':
                        if (!p->cd->readonly && GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            SendMessage(hwnd, WM_CUT, 0, 0);
                        }
                        break;
                    case 'C':
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            SendMessage(hwnd, WM_COPY, 0, 0);
                        }
                        break;
                    case 'V':
                        if (!p->cd->readonly && GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            SendMessage(hwnd, WM_PASTE, 0, 0);
                        }
                        break;
                    case 'Y':
                        if (!p->cd->readonly && GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            RemoveCurrentLine(hwnd, p);
                            return 0;
                        }
                        break;
                    case 'T':
                        if (!p->cd->readonly && GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            RemoveNextWord(hwnd, p);
                        }
                        break;
                    case 'R':
                        if (!p->cd->readonly)
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                SendMessage(hwnd, WM_REDO, 0, 0);
                                if (IsWindowVisible(hwndShowFunc))
                                    ShowWindow(hwndShowFunc, SW_HIDE);
                            }
                        break;
                    case 'Z':
                        if (!p->cd->readonly)
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                SendMessage(hwnd, WM_UNDO, 0, 0);
                                if (IsWindowVisible(hwndShowFunc))
                                    ShowWindow(hwndShowFunc, SW_HIDE);
                            }
                        break;
                    case 'S':
                        if (!p->cd->readonly && GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            if (p->cd->modified)
                                SendMessage(GetParent(hwnd), WM_COMMAND, IDM_SAVE, 0);
                        }
                        break;
                    case 'L':
                        if (!(GetKeyState(VK_CONTROL) & 0x80000000))
                            break;
                        if (!p->cd->readonly && p->cd->inserting)
                        {
                            insertautoundo(hwnd, p, UNDO_AUTOEND);
                            insertchar(hwnd, p, '\f');
                            insertcr(hwnd, p, FALSE);
                            insertautoundo(hwnd, p, UNDO_AUTOBEGIN);
                        }
                        break;
                    case 'W':
                        if (!(GetKeyState(VK_CONTROL) & 0x80000000))
                            break;
                        InstallForParse(GetParent(hwnd));
                        break;

                    case VK_SHIFT:
                        p->cd->selecting = TRUE;
                        break;
                    default:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            switch (KeyboardToAscii(wParam, lParam, FALSE))
                            {
                                case '[':
                                    PopupFullScreen(hwnd, p);
                                    return 0;
                                case ']':
                                    ReleaseFullScreen(hwnd, p);
                                    return 0;
                                case '\\':
                                    verticalCenter(hwnd, p);
                                    break;
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    // on german keyboards still honor the rcontrol
                                    switch (KeyboardToAscii(wParam, lParam, TRUE))
                                    {
                                        case '[':
                                            if (GetKeyState(VK_RCONTROL) & 0x80000000)
                                            {
                                                PopupFullScreen(hwnd, p);
                                                return 0;
                                            }
                                            break;
                                        case ']':
                                            if (GetKeyState(VK_RCONTROL) & 0x80000000)
                                            {
                                                ReleaseFullScreen(hwnd, p);
                                                return 0;
                                            }
                                            break;
                                        case '{':
                                        case '}':
                                        case 0xb3:  // superscript 3
                                        case 0xb2:  // superscript 2
                                            break;
                                        case '\\':
                                            if (GetKeyState(VK_RCONTROL) & 0x80000000)
                                            {
                                                verticalCenter(hwnd, p);
                                            }
                                            break;
                                        default:
                                            MarkOrGoto(hwnd, p, wParam - '0', !!(GetKeyState(VK_SHIFT) & 0x80000000));
                                            break;
                                    }
                                    break;
                            }
                        }
                }
                if (p->cd->selecting)
                    InvalidateRect(hwnd, 0, 0);
                PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0);
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                FindParenMatch(hwnd, p);  // undefined in local context
                break;
            case WM_SYSKEYUP:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (wParam == VK_SHIFT)
                    p->cd->selecting = FALSE;
                else
                    switch (wParam = KeyboardToAscii(wParam, lParam, FALSE))
                    {
                        case '[':
                        case ']':
                            if (!(GetKeyState(VK_CONTROL) & 0x80000000) && (GetKeyState(VK_SHIFT) & 0x80000000))
                                if (lParam & 0x20000000)
                                {  // alt key
                                    return 0;
                                }
                            break;
                    }
                break;
            case WM_SYSKEYDOWN:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (wParam == VK_SHIFT)
                    p->cd->selecting = TRUE;
                else
                    switch (wParam = KeyboardToAscii(wParam, lParam, FALSE))
                    {
                        case '[':
                        case ']':
                            if (!(GetKeyState(VK_CONTROL) & 0x80000000) && (GetKeyState(VK_SHIFT) & 0x80000000))
                                if (lParam & 0x20000000)
                                {                                                        // alt key
                                    FindBraceMatch(hwnd, p, wParam == '[' ? '{' : '}');  // undefined in local context
                                    return 0;
                                }
                            break;
                    }
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
                if ((wParam >= ' ' && wParam < 256) && !(GetKeyState(VK_RCONTROL) & 0x80000000))  // german kb support
                {
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    if (p->insertcursorcolumn)
                        insertcrtabs(hwnd, p);
                    CancelParenMatch(hwnd, p);
                    if (!p->cd->readonly)
                    {
                        if (wParam == ':')
                        {
                            if (p->lastWasColon)
                            {
                                int n = p->selstartcharpos;
                                doundo(hwnd, p);
                                p->selstartcharpos = p->selendcharpos = n + p->lastWasColon;
                                p->lastWasColon = 0;
                                insertchar(hwnd, p, wParam);
                            }
                            else
                            {
                                insertchar(hwnd, p, wParam);
                                p->lastWasColon = DeleteColonSpaces(hwnd, p);
                            }
                        }
                        else
                        {
                            insertchar(hwnd, p, wParam);
                            p->lastWasColon = 0;
                        }
                        if (wParam == '.' || wParam == '>' || wParam == ':' || isalnum(wParam) || wParam == '_')
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
                        xdrawline(hwnd, p, p->selstartcharpos);
                    }
                    else
                        right(hwnd, p);
                    FindParenMatch(hwnd, p);
                    setcurcol(p);
                    PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0);
                    break;
                }
                break;
            case WM_SETFOCUS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->hasfocus = TRUE;
                p->hiddenCaret = TRUE;
                CreateCaret(hwnd, 0, p->cd->inserting ? 2 : p->cd->txtFontWidth, p->cd->txtFontHeight);
                MoveCaret(hwnd, p);
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_SETFOCUS | (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
                break;
            case WM_KILLFOCUS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                p->hasfocus = FALSE;
                DestroyCaret();
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_KILLFOCUS | (GetWindowLong(hwnd, GWL_ID) << 16)),
                            (LPARAM)hwnd);
                break;
            case WM_SETTEXT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                freemem(p);
                p->cd->text = 0;
                p->cd->textlen = 0;
                p->cd->textmaxlen = 0;
                if (p->cd->language)
                    SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, p->cd->readonly ? C_READONLYBACKGROUND : C_BACKGROUND);
                i = strlen((char*)lParam);

                if (allocmem(p, i * 5) && commitmem(p, i + 1))
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
                return (LRESULT)(EDITDATA*)GetWindowLong(hwnd, 0);
            case EM_UPDATESIBLING:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                UpdateMarks(p, lParam, wParam);
                if (lParam <= p->textshowncharpos)
                {
                    if (wParam > 0)
                        p->textshowncharpos += wParam;
                    else if (lParam - wParam < p->textshowncharpos)
                        p->textshowncharpos += wParam;
                    else
                        p->textshowncharpos -= p->textshowncharpos - lParam;
                }
                if (lParam <= p->selstartcharpos)
                    p->selstartcharpos += wParam;
                if (lParam <= p->selendcharpos)
                    p->selendcharpos += wParam;
                InvalidateRect(hwnd, 0, 0);
                break;
            case EM_LOADLINEDATA:
                if (PropGetInt(NULL, "CODE_COMPLETION") != 0)
                {
                    p = (EDITDATA*)GetWindowLong(hwnd, 0);
                    if (p)
                    {
                        if (p->cd)
                        {
                            free(p->cd->lineData);
                            p->cd->lineData = NULL;
                            p->cd->lineDataMax = 0;
                            if (!p->cd->lineData)
                            {
                                HWND t = GetParent(hwnd);
                                DWINFO* x = (DWINFO*)GetWindowLong(t, 0);
                                BYTE* ld = ccGetLineData(x->dwName, &p->cd->lineDataMax);
                                // the above call call takes a while and meanwhile they may
                                // have closed the window... if they did 'p' is invalid now
                                // so bail..
                                if (!IsWindow(hwnd))
                                    return 0;
                                p->cd->lineData = ld;
                            }
                        }
                        FreeColorizeEntries(p->colorizeEntries);
                        {
                            HWND t = GetParent(hwnd);
                            DWINFO* x = (DWINFO*)GetWindowLong(t, 0);
                            ccGetColorizeData(x->dwName, p->colorizeEntries);  // undefined in local context
                            FullColorize(hwnd, p, FALSE);
                        }
                    }
                }
                break;
            case WM_CREATE:
                p = (EDITDATA*)calloc(1, sizeof(EDITDATA));
                SetWindowLong(hwnd, 0, (int)p);
                p->self = hwnd;
                lpCreate = (LPCREATESTRUCT)lParam;
                if (lpCreate->lpCreateParams)
                {
                    p->cd = (COREDATA*)((EDITDATA*)(lpCreate->lpCreateParams))->cd;
                    if (GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP)
                    {
                        p->selstartcharpos = ((EDITDATA*)(lpCreate->lpCreateParams))->selstartcharpos;
                        p->selendcharpos = ((EDITDATA*)(lpCreate->lpCreateParams))->selendcharpos;
                        p->textshowncharpos = ((EDITDATA*)(lpCreate->lpCreateParams))->textshowncharpos;
                        p->popupDerivedFrom = ((EDITDATA*)(lpCreate->lpCreateParams));
                        SetWindowLong(hwnd, GWL_HWNDPARENT, (long)GetParent(((EDITDATA*)(lpCreate->lpCreateParams))->self));
                    }
                }
                else
                {
                    LOGFONT editFont;
                    OSVERSIONINFO osvi;
                    p->cd = (COREDATA*)calloc(1, sizeof(COREDATA));
                    p->cd->inserting = TRUE;

                    memcpy(&editFont, &EditFont, sizeof(editFont));
                    memset(&osvi, 0, sizeof(osvi));
                    osvi.dwOSVersionInfoSize = sizeof(osvi);
                    GetVersionEx(&osvi);  // ascii version of this function is deprecated
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

                    for (int i = 0; i < 10; i++)
                        p->marks[i] = -1;
                    getPageSize();
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
                    EDLIST* l = (EDLIST*)calloc(1, sizeof(EDLIST));
                    if (l)
                    {
                        EDLIST** l1 = &p->cd->siblings;
                        while (*l1)
                        {
                            if (p->id <= (*l1)->data->id)
                                p->id = (*l1)->data->id + 1;
                            l1 = &(*l1)->next;
                        }
                        *l1 = l;
                        l->data = p;
                    }
                }
                SendMessage(hwnd, EM_LOADLINEDATA, 0, 0);
                SetScrollRange(hwnd, SB_HORZ, 0, MAX_HSCROLL, TRUE);
                break;
            case WM_SETEDITORSETTINGS:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (p->cd->language)
                    SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, p->cd->readonly ? C_READONLYBACKGROUND : C_BACKGROUND);
                p->cd->defbackground = p->cd->readonly ? C_READONLYBACKGROUND : C_BACKGROUND;
                if (p->cd->language)
                {
                    stop = p->cd->tabs * 4;
                    SendMessage(hwnd, EM_SETTABSTOPS, 1, (LPARAM)&stop);
                    Colorize(p->cd->text, 0, p->cd->textlen, (p->cd->defbackground << 5) + p->cd->defforeground, FALSE);
                    FullColorize(hwnd, p, FALSE);
                }
                InvalidateRect(hwnd, 0, 0);
                return 0;
            case WM_DESTROY:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                SetWindowLong(hwnd, 0, 0);
                if (p->popupDerivedFrom)
                {
                    p->popupDerivedFrom->selstartcharpos = p->selstartcharpos;
                    p->popupDerivedFrom->selendcharpos = p->selendcharpos;
                    ScrollCaretIntoView(p->popupDerivedFrom->self, p->popupDerivedFrom, TRUE);
                }
                while (p->cd->colorizing)
                    Sleep(10);
                DestroyWindow(p->tooltip);
                {
                    EDLIST** l = &p->cd->siblings;
                    while (*l)
                    {
                        if ((*l)->data == p)
                        {
                            EDLIST* l2 = (*l);
                            (*l) = (*l)->next;
                            free((void*)l2);
                            break;
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
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_VSCROLL | (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
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
                        scrollup(hwnd, p, -1);
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
                        scrollup(hwnd, p, 1 - i);
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
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_HSCROLL | (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
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
                        scrollleft(hwnd, p, -1);
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
                        scrollleft(hwnd, p, 1 - i);
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
                if (i <= 0)
                    i = 1;
                SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_VSCROLL | (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
                p->wheelIncrement += GET_WHEEL_DELTA_WPARAM(wParam);
                if (p->wheelIncrement < 0)
                {
                    while (p->wheelIncrement <= -WHEEL_DELTA)
                    {
                        scrollup(hwnd, p, 1);
                        p->wheelIncrement += WHEEL_DELTA / i;
                    }
                }
                else
                {
                    while (p->wheelIncrement >= WHEEL_DELTA)
                    {
                        scrollup(hwnd, p, -1);
                        p->wheelIncrement -= WHEEL_DELTA / i;
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
                        StartAutoScroll(hwnd, p, -1);
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
                SendMessage(hwndShowFunc, WM_USER + 3, 0, 0);
                return 0;
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (GetFocus() != hwnd)
                    SendMessage(GetParent(hwnd), WM_COMMAND, EN_NEEDFOCUS, 0);
                //                    SetFocus(hwnd);
                nm.code = NM_RCLICK;
                SendMessage(GetParent(hwnd), WM_NOTIFY, 0, (LPARAM)&nm);
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
                if (GetKeyState(VK_SHIFT) & 0x80000000)
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
                        while (p->selstartcharpos && p->cd->text[p->selstartcharpos - 1].ch != '\n')
                            p->selstartcharpos--;
                        while (p->cd->text[p->selendcharpos].ch && p->cd->text[p->selendcharpos + 1].ch != '\n')
                            p->selendcharpos++;
                        if (p->cd->text[p->selendcharpos].ch)
                            p->selendcharpos++;
                    }
                }
                else
                {

                    p->selstartcharpos = p->selendcharpos = charfrompos(hwnd, p, &pt);
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
                    p->cd->selecting = !!(GetKeyState(VK_SHIFT) & 0x80000000);
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
                xdrawline(hwnd, p, 0);
                break;
            case WM_WORDUNDERCURSOR:
                return GetWordFromPos(hwnd, (char*)lParam, wParam, -1, 0, 0, 0);
            case WM_WORDUNDERPOINT:
            {
                int charpos, start, end, linepos;
                POINT pt = *(POINT*)wParam;
                charpos = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&pt);
                return GetWordFromPos(hwnd, (char*)lParam, wParam, charpos, &linepos, &start, &end);
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
                InvalidateRect(hwnd, 0, 0);
                break;
            case WM_GETTEXT:
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (!p->cd->textlen)
                    return 0;
                i = p->cd->textlen;
                if (wParam <= i)
                    i = wParam - 1;
                {
                    INTERNAL_CHAR* x = p->cd->text;
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
                if (wParam == 0 && lParam == -1)
                {
                    p->selstartcharpos = 0;
                    p->selendcharpos = p->cd->textlen;
                }
                else if (wParam == -1)
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
                return -1;
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
                if (ic[-1].ch != '\n')
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
                p->cd->leftmargin = lParam & 0xffff;
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
                return SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, p->textshowncharpos);
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
                    if (p->cd->undohead != p->cd->undotail)
                    {
                        return p->cd->undotail != p->cd->redopos;
                    }
                }
                return FALSE;
            case EM_CANREDO:
            {
                p = (EDITDATA*)GetWindowLong(hwnd, 0);
                if (p->cd->undohead != p->cd->undotail)
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
                p->cd->txtFontHeight = t.tmHeight + 1;  // the +1 is necessary for italic underline to be visible on some fonts...
                p->cd->txtFontWidth = t.tmAveCharWidth;
                GetObject(p->cd->hFont, sizeof(LOGFONT), &lf);
                lf.lfWidth = t.tmAveCharWidth - 1;
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
                    int start, end;
                    char* pt = (char*)lParam;
                    INTERNAL_CHAR* q;
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
                    q = p->cd->text + start;
                    while (q < p->cd->text + end)
                        *pt++ = q++->ch;
                    *pt = 0;
                }
                break;
            default:
                break;
        }
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
    wc.hCursor = 0;  // LoadCursor(0, IDC_ARROW);
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