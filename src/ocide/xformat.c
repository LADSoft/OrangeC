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

/**********************************************************************
 * tab to the current line position
 **********************************************************************/
void insertcrtabs(HWND hwnd, EDITDATA* p)
{
    int oldinsert = p->cd->inserting;
    int parencount = 0;
    if (!p->cd->language)
    {
        p->insertcursorcolumn = 0;
        return;
    }
    if (!PropGetBool(NULL, "AUTO_INDENT"))
    {
        p->insertcursorcolumn = 0;
        return;
    }
    p->cd->inserting = TRUE;
    while (p->insertcursorcolumn >= p->cd->tabs)
    {
        inserttab(hwnd, p);
        p->insertcursorcolumn -= p->cd->tabs;
    }
    while (p->insertcursorcolumn)
    {
        p->insertcursorcolumn--;
        insertchar(hwnd, p, ' ');
    }
    p->cd->inserting = oldinsert;
    //    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
}
//-------------------------------------------------------------------------

int spacedend(EDITDATA* p, int pos)
{
    int rv = 0;
    while (pos && p->cd->text[pos - 1].ch != '\n')
        if (!isspace(p->cd->text[--pos].ch))
            return 0;
    rv = pos;
    while (p->cd->text[pos].ch && isspace(p->cd->text[pos].ch) && p->cd->text[pos].ch != '\n')
        pos++;
    if (p->cd->text[pos].ch == '}')
        return rv;
    else
        return 0;
}

//-------------------------------------------------------------------------

int preprocline(EDITDATA* p, int pos)
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
        return -1;
}

/**********************************************************************
 * tab to the current line position
 **********************************************************************/
void InsertBeginTabs(HWND hwnd, EDITDATA* p)
{
    int pos, n;
    int storepos, parencount = 0;
    int solpos, eospos;
    int oldinsert = p->cd->inserting;
    if (p->cd->language != LANGUAGE_C && p->cd->language != LANGUAGE_CPP && p->cd->language != LANGUAGE_RC)
        return;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return;
    p->cd->inserting = TRUE;
    pos = p->selstartcharpos - 1;
    solpos = pos;
    while (solpos && p->cd->text[solpos - 1].ch != '\n')
    {
        if (!isspace(p->cd->text[solpos - 1].ch))
            return;
        solpos--;
    }
    if (solpos)
        pos = solpos - 1;
    eospos = solpos;
    while (eospos < p->cd->textlen && isspace(p->cd->text[eospos].ch) && p->cd->text[eospos].ch != '\n')
        eospos++;
    while (1)
    {
        int pos2 = pos;
        while (pos && p->cd->text[pos - 1].ch != '\n')
            pos--;
        while (p->cd->text[pos].ch && isspace(p->cd->text[pos].ch) && p->cd->text[pos].ch != '\n')
            pos++;
        if (p->cd->text[pos].ch != '#')
        {
            while (pos2-- > pos && p->cd->text[pos2].ch && p->cd->text[pos2].ch != '\n')
            {
                if (p->cd->text[pos2].ch == '(')
                {
                    storepos = pos2;
                    if (++parencount == 0)
                        break;
                }
                else if (p->cd->text[pos2].ch == ')')
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
    while (pos && p->cd->text[pos - 1].ch != '\n')
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
    p->selstartcharpos = ++p->selendcharpos;  // skip past '}'
    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    p->cd->inserting = oldinsert;
}
/**********************************************************************
 * tab to the current line position
 **********************************************************************/
void InsertEndTabs(HWND hwnd, EDITDATA* p, int newend)
{
    int pos, n;
    int eospos;
    int lsolpos, leospos;
    int oldinsert = p->cd->inserting;
    if (p->cd->language != LANGUAGE_C && p->cd->language != LANGUAGE_CPP && p->cd->language != LANGUAGE_RC)
        return;
    if (!newend)
        return;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return;
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
            if (pos1 != -1)
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
        p->selstartcharpos = p->selendcharpos;  // position to '}'
        insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    }
    p->cd->inserting = oldinsert;
}

//-------------------------------------------------------------------------

void SelectIndent(HWND hwnd, EDITDATA* p, int insert)
{
    int olds = p->selstartcharpos;
    int olde = p->selendcharpos;
    int start = p->selstartcharpos;
    int end = p->selendcharpos;
    int oldinsert = p->cd->inserting;
    int oldselect = p->cd->selecting;
    int oldte = p->cd->textlen;
    int decd = FALSE;
    int inverted = FALSE;
    p->cd->inserting = TRUE;
    if (start == end)
    {
        int x;
        int adjustPos = start;
        int uVal = p->cd->undohead;
        while (start && p->cd->text[start - 1].ch != '\n')
            start--;
        while (p->cd->text[end].ch && p->cd->text[end].ch != '\n')
            end++;
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
            inverted = TRUE;
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
void SelectComment(HWND hwnd, EDITDATA* p, int insert)
{
    int olds = p->selstartcharpos;
    int olde = p->selendcharpos;
    int start = p->selstartcharpos;
    int end = p->selendcharpos;
    int oldinsert = p->cd->inserting;
    int oldselect = p->cd->selecting;
    int decd = FALSE;
    int inverted = FALSE;
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
        while (start && isspace(p->cd->text[start].ch) && p->cd->text[start].ch != '\n')
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
                    p->selstartcharpos++;
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
                        p->selstartcharpos--;
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
            inverted = TRUE;
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

void DeletePound(HWND hwnd, EDITDATA* p)
{
    int n, m;
    if (p->cd->language != LANGUAGE_C && p->cd->language != LANGUAGE_CPP && p->cd->language != LANGUAGE_RC)
        return;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return;
    if (p->selstartcharpos && p->cd->text[p->selstartcharpos - 1].ch != '#')
        return;
    n = p->selstartcharpos - 1;
    while (n && p->cd->text[n - 1].ch != '\n')
        n--;
    m = n;
    while (isspace(p->cd->text[m].ch))
        m++;
    if (p->cd->text[m].ch != '#' || m == n)
        return;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINEND);
    p->selstartcharpos = n;
    p->selendcharpos = m;
    Replace(hwnd, p, "", 0);
    p->selstartcharpos = p->selendcharpos = n + 1;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    ScrollCaretIntoView(hwnd, p, FALSE);
}

//-------------------------------------------------------------------------

void DeletePercent(HWND hwnd, EDITDATA* p)
{
    int n, m;
    if (p->cd->language != LANGUAGE_ASM)
        return;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return;
    if (p->selstartcharpos && p->cd->text[p->selstartcharpos - 1].ch != '%')
        return;
    n = p->selstartcharpos - 1;
    while (n && p->cd->text[n - 1].ch != '\n')
        n--;
    m = n;
    while (isspace(p->cd->text[m].ch))
        m++;
    if (p->cd->text[m].ch != '%' || m == n)
        return;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINEND);
    p->selstartcharpos = n;
    p->selendcharpos = m;
    Replace(hwnd, p, "", 0);
    p->selstartcharpos = p->selendcharpos = n + 1;
    insertautoundo(hwnd, p, UNDO_AUTOCHAINBEGIN);
    ScrollCaretIntoView(hwnd, p, FALSE);
}

void upperlowercase(HWND hwnd, EDITDATA* p, int ucase)
{
    int oldsel = -1;
    int i;
    int s, e;
    UNDO* u;
    u = undo_casechange(hwnd, p);
    if (p->selstartcharpos == p->selendcharpos)
    {
        e = s = p->selstartcharpos;
        if (!isalnum(p->cd->text[e].ch) && p->cd->text[e].ch != '_')
            return;
        e++;
        while (p->cd->text[e].ch && (isalnum(p->cd->text[e].ch) || p->cd->text[e].ch == '_'))
            e++;
        while (s && (isalnum(p->cd->text[s - 1].ch) || p->cd->text[s - 1].ch == '_'))
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
        p->cd->text[i].ch = ucase ? toupper(p->cd->text[i].ch) : tolower(p->cd->text[i].ch);
    }
    InvalidateRect(hwnd, 0, 0);
}
int DeleteColonSpaces(HWND hwnd, EDITDATA* p)
{
    int n = p->selstartcharpos - 1, s, b = n + 1;
    if (p->cd->language != LANGUAGE_ASM)
        return 0;
    if (!PropGetBool(NULL, "AUTO_FORMAT"))
        return 0;
    while (n && isspace(p->cd->text[n - 1].ch) && p->cd->text[n - 1].ch != '\n')
        n--;
    if (n && (isalnum(p->cd->text[n - 1].ch) || p->cd->text[n - 1].ch == '_'))
    {
        while (n && (isalnum(p->cd->text[n - 1].ch) || p->cd->text[n - 1].ch == '_'))
            n--;
        s = n;
        while (s && isspace(p->cd->text[s - 1].ch) && p->cd->text[s - 1].ch != '\n')
            s--;
        if ((!s || p->cd->text[s - 1].ch == '\n') && s != n)
        {
            p->selstartcharpos = s;
            p->selendcharpos = n;
            Replace(hwnd, p, "", 0);
            p->selstartcharpos += b - n;
            p->selendcharpos += b - n;
            return n - s;
        }
    }
    return 0;
}