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

#define SQUIGGLE_COLOR 0xff

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

/**********************************************************************
 * drawline draws the current line and everything below it(by invalidating
 * the selection)
 **********************************************************************/
void xdrawline(HWND hwnd, EDITDATA* p, int chpos)
{
    RECT r;
    int pos;
    pos = p->selendcharpos;
    ClientArea(hwnd, p, &r);
    r.top = GetLineOffset(hwnd, p, pos) * p->cd->txtFontHeight;
    SendUpdate(hwnd);
    InvalidateRect(hwnd, &r, 0);
    MoveCaret(hwnd, p);
}
int lineParsed(EDITDATA* p, int line)
{
    if (PropGetInt(NULL, "CODE_COMPLETION") != 0)
    {
        if (p->cd->lineData && !IsBadReadPtr(p->cd->lineData, (p->cd->lineDataMax + 7) / 8))
        {
            if (p->cd->lineDataMax < line)
                line = p->cd->lineDataMax;
            return !!(p->cd->lineData[line / 8] & (1 << (line & 7)));
        }
    }
    return TRUE;
}
int getfragment(EDITDATA* p, int pos, int autoDecoration, int colorizing, char* buf, int bufsz, COLORREF* fcolor, COLORREF* bcolor,
                HFONT* font, int* col, int line)
{
    int initial = pos;
    int count = 0;
    int found = FALSE;
    int attribs = 0;
    int color = (p->cd->defbackground << 5) + C_TEXT;
    int selecting;
    int start, end;
    int matched = FALSE;
    int taboffs = p->leftshownindex % (p->cd->tabs == 0 ? 2 : p->cd->tabs);
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
        if (pos - initial >= bufsz - 10 || pos >= p->cd->textlen || p->cd->text[pos].ch == '\n')
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
                if (!(attribs & CFE_AUTOCOLOR) && p->cd->text[pos].Color != color)
                    break;
            }
        }
        if (p->cd->text[pos].ch == '\t')
        {
            int newpos = ((*col + p->cd->tabs) / (p->cd->tabs ? p->cd->tabs : 2)) * p->cd->tabs;
            int i;
            for (i = *col; i < newpos; i++)
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
        if (attribs & CFE_AUTOCOLOR)
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
    switch (attribs & ~CFE_AUTOCOLOR)
    {
        case CFE_BOLD:
            *font = p->cd->hBoldFont;
            break;
        case CFE_ITALIC:
            *font = p->cd->hItalicFont;
            if (pos >= p->cd->textlen || p->cd->text[pos].ch == '\n')
                buf[count++] = ' ';
            break;
        case CFE_BOLD | CFE_ITALIC:
        default:
            if (color == C_AUTO)
            {
                switch (autoDecoration)
                {
                    case 0:
                    default:
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
void DrawSquiggles(HDC dc, HPEN squigglePen, EDITDATA* p, int row, int start, int end)
{
    squigglePen = SelectObject(dc, squigglePen);
    int initial = start;
    row = row + p->cd->txtFontHeight - 2;
    while (start < end)
    {
        while (!p->cd->text[start].squiggle && start < end)
            start++;
        if (start < end)
        {
            int runstart = start;
            while (p->cd->text[start].squiggle && start < end)
                start++;
            int runend = start;
            runstart = (runstart - initial) * p->cd->txtFontWidth;
            runend = (runend - initial) * p->cd->txtFontWidth;
            for (int i = runstart; i < runend; i += 6)
            {
                MoveToEx(dc, i, row + 1, NULL);
                LineTo(dc, i + 3, row + 1);
                if (i + 3 < runend)
                {
                    MoveToEx(dc, i + 3, row, NULL);
                    LineTo(dc, i + 6, row);
                }
            }
        }
    }
    squigglePen = SelectObject(dc, squigglePen);
}
/**********************************************************************
 * EditPaint is the paint procedure for the window.  It selectively
 * paints only those lines that have been marked as changed
 **********************************************************************/
void EditPaint(HWND hwnd, EDITDATA* p)
{
    char buf[4000];
    PAINTSTRUCT ps;
    HDC dc, dcFinal;
    HBITMAP bmp;
    RECT r, b1, client;
    int lines, i, pos;
    int baseline;
    HBRUSH selBrush = CreateSolidBrush(selectedBackgroundColor);
    HPEN columnbarPen, squigglePen;
    int colMark = PropGetInt(NULL, "COLUMN_MARK");
    int autoDecoration = PropGetInt(NULL, "DECORATE_AUTO");
    int colorizing = PropGetBool(NULL, "COLORIZE");
    if (colMark != 0)
    {
        columnbarPen = CreatePen(PS_SOLID, 0, columnbarColor);
    }
    else
    {
        columnbarPen = CreatePen(PS_NULL, 0, 0);
    }
    squigglePen = CreatePen(PS_SOLID, 0, SQUIGGLE_COLOR);
    ClientArea(hwnd, p, &r);
    GetClientRect(hwnd, &client);
    lines = r.bottom / p->cd->txtFontHeight;
    dcFinal = BeginPaint(hwnd, &ps);
    dc = CreateCompatibleDC(dcFinal);
    bmp = CreateCompatibleBitmap(dcFinal, client.right, client.bottom);
    SelectObject(dc, bmp);
    SetBkMode(dc, OPAQUE);
    pos = p->textshowncharpos;
    baseline = LineFromChar(p, pos) + 1;
    for (i = 0; i < lines; i++)
    {
        int col = 0, leftcol = 0;
        r.top = i * p->cd->txtFontHeight;
        r.bottom = r.top + p->cd->txtFontHeight;
        if (IntersectRect(&b1, &r, &ps.rcPaint))
        {
            while (leftcol < p->leftshownindex && p->cd->text[pos].ch != '\n' && p->cd->text[pos].ch != '\f' &&
                   pos < p->cd->textlen)
            {
                if (p->cd->text[pos].ch == '\t')
                {
                    int v = leftcol;
                    v = v + p->cd->tabs;
                    v = (v / p->cd->tabs) * p->cd->tabs;
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
            int startPos = pos;
            while (p->cd->text[pos].ch != '\n' && pos < p->cd->textlen)
            {
                int selection = FALSE;
                COLORREF fcolor, bcolor;
                HFONT font;
                int n, j, z = 0;
                int dx[sizeof(buf)];
                int ofs = 0;
                pos = getfragment(p, pos, autoDecoration, colorizing, buf, sizeof(buf), &fcolor, &bcolor, &font, &leftcol,
                                  baseline + i);
                if (buf[0] == '\f')
                {
                    strcpy(buf, "+--------- Page Break ---------+");
                }
                SetTextColor(dc, fcolor);
                SetBkColor(dc, bcolor);
                SelectObject(dc, font);
                n = strlen(buf);
                for (j = 0; j < n; j++)
                {
                    dx[j] = p->cd->txtFontWidth;
                }
                // I am not sure why this is necessary...
                if (font == p->cd->hItalicFont)
                {
                    if (col > 1)
                        z = -1;
                }
                r.left = col - z;
                ExtTextOut(dc, col + z, r.top, ETO_OPAQUE, &r, buf, n, dx);
                col += (p->cd->txtFontWidth) * n;
            }
            DrawSquiggles(dc, squigglePen, p, r.top, startPos, pos);
        }
        while (pos < p->cd->textlen)
        {
            if (p->cd->text[pos].ch == '\n')
                break;
            pos++;
        }
        r.left = col;
        if ((pos < p->selendcharpos && pos > p->selstartcharpos) || (pos >= p->selendcharpos && pos < p->selstartcharpos))
        {
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
    BitBlt(dcFinal, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(hwnd, &ps);
    if (p->cd->sendchangeonpaint)
    {
        p->cd->sendchangeonpaint = FALSE;
        SendMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)(EN_CHANGE | (GetWindowLong(hwnd, GWL_ID) << 16)), (LPARAM)hwnd);
    }
    DeleteObject(selBrush);
    DeleteObject(squigglePen);
    DeleteObject(columnbarPen);
}
