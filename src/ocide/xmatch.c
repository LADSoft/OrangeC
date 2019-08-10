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

void CancelParenMatch(HWND hwnd, EDITDATA* p)
{
    if (p->matchingEnd != 0)
    {
        p->matchingEnd = p->matchingStart = 0;
        InvalidateRect(hwnd, NULL, 0);
    }
}
int FindParenMatchBackward(HWND hwnd, EDITDATA* p, int dec)
{
    int skip, match;
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
    while (s && --s)
    {
        if (quotechar == p->cd->text[s].ch && (!s || p->cd->text[s - 1].ch != '\\' || s < 2 || p->cd->text[s - 2].ch == '\\'))
            quotechar = 0;
        else if (!quotechar)
        {
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"') &&
                (!s || p->cd->text[s - 1].ch != '\\' || s < 2 || p->cd->text[s - 2].ch == '\\'))
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
int FindParenMatchForward(HWND hwnd, EDITDATA* p, int dec)
{
    int skip, match;
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
        if (quotechar == p->cd->text[s].ch && (p->cd->text[s - 1].ch != '\\' || s < 2 || p->cd->text[s - 2].ch == '\\'))
            quotechar = 0;
        else if (!quotechar)
        {
            if ((p->cd->text[s].ch == '\'' || p->cd->text[s].ch == '"') &&
                (p->cd->text[s - 1].ch != '\\' || s < 2 || p->cd->text[s - 2].ch == '\\'))
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
void FindParenMatch(HWND hwnd, EDITDATA* p)
{
    if (PropGetBool(NULL, "MATCH_PARENTHESIS"))
        if (!FindParenMatchForward(hwnd, p, TRUE))
            FindParenMatchBackward(hwnd, p, TRUE);
}
void FindBraceMatchForward(HWND hwnd, EDITDATA* p)
{
    int n = 1;
    int x = p->selstartcharpos;
    while (x && isspace(p->cd->text[x].ch))
        x--;
    if (p->cd->text[x].ch == '{')
    {
        x++;
    }
    else
    {
        x = p->selstartcharpos;
        while (isspace(p->cd->text[x].ch))
            x++;
        if (p->cd->text[x].ch == '{')
            x++;
    }
    if (x && p->cd->text[x - 1].ch == '{')
    {
        while (n && p->cd->text[x].ch)
        {
            if (!instring(p->cd->text, p->cd->text + x))
            {
                if (p->cd->text[x].ch == '{')
                    n++;
                if (p->cd->text[x].ch == '}')
                    n--;
            }
            x++;
        }
        p->selstartcharpos = p->selendcharpos = x;
        ScrollCaretIntoView(hwnd, p, TRUE);
        PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0);
    }
}
void FindBraceMatchBackward(HWND hwnd, EDITDATA* p)
{
    int n = 1;
    int x = p->selstartcharpos;
    while (isspace(p->cd->text[x].ch))
        x++;
    if (p->cd->text[x].ch == '}')
    {
        x--;
    }
    else
    {
        x = p->selstartcharpos;
        while (x && isspace(p->cd->text[x - 1].ch))
            x--;
        if (x && p->cd->text[x - 1].ch == '}')
            x -= 2;
        else
            return;
    }
    while (x && n)
    {
        if (!instring(p->cd->text, p->cd->text + x))
        {
            if (p->cd->text[x].ch == '{')
                n--;
            if (p->cd->text[x].ch == '}')
                n++;
        }
        if (!n)
            break;
        x--;
    }
    p->selstartcharpos = p->selendcharpos = x;
    ScrollCaretIntoView(hwnd, p, TRUE);
    PostMessage(GetParent(hwnd), WM_COMMAND, ID_REDRAWSTATUS, 0);
}
void FindBraceMatch(HWND hwnd, EDITDATA* p, int ch)
{
    if (ch == '{')
        FindBraceMatchBackward(hwnd, p);
    else if (ch == '}')
        FindBraceMatchForward(hwnd, p);
}
