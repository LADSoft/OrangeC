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
#include <stdlib.h>
#include <ctype.h>
#include <process.h>
#include "symtypes.h"
 extern COLORREF keywordColor;
 extern COLORREF numberColor;
 extern COLORREF commentColo;
 extern COLORREF stringColor;
 extern COLORREF directiveColor;
 extern COLORREF backgroundColor;
 extern COLORREF readonlyBackgroundColor;
 extern COLORREF textColor;
 extern COLORREF highlightColor;
 extern COLORREF selectedTextColor ;
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

extern COLORREF *colors[];

/**********************************************************************
 * drawline draws the current line and everything below it(by invalidating
 * the selection)
 **********************************************************************/
void xdrawline(HWND hwnd, EDITDATA *p, int chpos)
{
    RECT r;
    int pos;
    pos = p->selendcharpos;
    ClientArea(hwnd, p, &r);
    r.top = GetLineOffset(hwnd, p, pos) *p->cd->txtFontHeight;
    SendUpdate(hwnd);
    InvalidateRect(hwnd, &r, 0);
    MoveCaret(hwnd, p);
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
        HDC dc, dcFinal;
        HBITMAP bmp;
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
        dcFinal = BeginPaint(hwnd, &ps);
        dc = CreateCompatibleDC(dcFinal);
        bmp = CreateCompatibleBitmap(dcFinal, client.right, client.bottom);
        SelectObject(dc, bmp);
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
        BitBlt(dcFinal,0,0,client.right,client.bottom,dc,0,0,SRCCOPY);
        DeleteObject(bmp);
        DeleteDC(dc);
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
