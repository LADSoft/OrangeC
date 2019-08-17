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
#include <stdlib.h>
#include <ctype.h>
/**********************************************************************
 * getundo creates an UNDO structure based on the user's last operation
 **********************************************************************/
UNDO* getundo(HWND hwnd, EDITDATA* p, int type)
{
    int x;
    UNDO* u;
    if (p->cd->undoing)
        return 0;
    if (p->cd->undohead != p->cd->undotail)
    {
        x = p->cd->redopos;
        if (x >= UNDO_MAX)
            x = 0;
        if (x != p->cd->undotail)
        {
            x = p->cd->redopos;
            if (--x < 0)
                x += UNDO_MAX;
            u = &p->cd->undolist[x];
            switch (u->type)
            {
                case UNDO_INSERT:
                    u->len = u->undotemp;
                    break;
                case UNDO_MODIFY:
                case UNDO_BACKSPACE:
                case UNDO_DELETE:
                    memcpy(u->data, u->data + u->undotemp, u->len - u->undotemp);
                    u->len -= u->undotemp;
                    u->undotemp = 0;
                    break;
                default:
                    u->undotemp = 0;
                    break;
            }
            if (type != UNDO_DELETESELECTION && type != UNDO_INSERTSELECTION && type != UNDO_CASECHANGE && type != UNDO_AUTOBEGIN &&
                type != UNDO_AUTOEND && type != UNDO_AUTOCHAINBEGIN && p->cd->undohead != p->cd->undotail)
            {
                if (u->type == type)
                {
                    if (type != UNDO_BACKSPACE)
                    {
                        if (p->selstartcharpos == u->postselstart)
                        {
                            /*
                            p->cd->undohead = x;
                            if (++p->cd->undohead >= UNDO_MAX)
                                p->cd->undohead -= UNDO_MAX;
                            p->cd->redopos = p->cd->undohead;
                            */
                            p->cd->modified = TRUE;
                            return u;
                        }
                    }
                    else
                    {
                        if (p->selstartcharpos + 1 == u->postselstart)
                        {
                            /*
                            p->cd->undohead = x;
                            if (++p->cd->undohead >= UNDO_MAX)
                                p->cd->undohead -= UNDO_MAX;
                            p->cd->redopos = p->cd->undohead;
                            */
                            return u;
                        }
                    }
                }
            }
        }
    }

    p->cd->undohead = p->cd->redopos;
    u = &p->cd->undolist[p->cd->redopos];
    if (++p->cd->undohead >= UNDO_MAX)
        p->cd->undohead = 0;
    p->cd->redopos = p->cd->undohead;
    if ((p->cd->undohead + 1) % UNDO_MAX == p->cd->undotail)
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

void insertautoundo(HWND hwnd, EDITDATA* p, int type)
{
    UNDO* u = getundo(hwnd, p, type);
    if (u)
    {
        u->postselstart = p->selstartcharpos;
        u->postselend = p->selendcharpos;
    }
}
/**********************************************************************
 * undo_deletesel gets the undo structure for a CUT operation
 **********************************************************************/
UNDO* undo_deletesel(HWND hwnd, EDITDATA* p)
{
    UNDO* u;
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
        BYTE* temp = (BYTE*)realloc(u->data, end - start);
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

UNDO* undo_casechange(HWND hwnd, EDITDATA* p)
{
    UNDO* x = undo_deletesel(hwnd, p);
    x->type = UNDO_CASECHANGE;
    return x;
}

/**********************************************************************
 * undo_insertsel gets the undo structure for an operation which pasts
 **********************************************************************/
UNDO* undo_insertsel(HWND hwnd, EDITDATA* p, char* s)
{
    UNDO* u = getundo(hwnd, p, UNDO_INSERTSELECTION);
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
UNDO* undo_deletechar(HWND hwnd, EDITDATA* p, int ch, int type)
{
    UNDO* u = getundo(hwnd, p, type);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        BYTE* temp = realloc(u->data, u->max + 64);
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
UNDO* undo_modifychar(HWND hwnd, EDITDATA* p)
{
    UNDO* u = getundo(hwnd, p, UNDO_MODIFY);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        BYTE* temp = realloc(u->data, u->max + 64);
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
UNDO* undo_insertchar(HWND hwnd, EDITDATA* p, int ch)
{
    UNDO* u = getundo(hwnd, p, UNDO_INSERT);
    if (!u)
        return u;
    if (u->max <= u->len)
    {
        BYTE* temp = realloc(u->data, u->max + 64);
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
 * undo_pchar undoes a delete character operation
 **********************************************************************/
void undo_pchar(HWND hwnd, EDITDATA* p, int ch)
{
    insertchar(hwnd, p, ch);
    FormatLine(hwnd, p->cd->text, p->cd->language, p->cd->defbackground);
}
/**********************************************************************
 * doundo is the primary routine to traverse the undo list and perform
 * an undo operation
 **********************************************************************/

int doundo(HWND hwnd, EDITDATA* p)
{
    int rv = 1;
    if (SendMessage(hwnd, EM_CANUNDO, 0, 0))
    {
        UNDO* u;
        int oldinsert = p->cd->inserting;
        int start;
        int x = p->cd->redopos;
        if (--x < 0)
            x += UNDO_MAX;
        u = &p->cd->undolist[x];
        p->cd->undoing++;
        p->cd->inserting = TRUE;
        rv = 0;
        switch (u->type)
        {
            case UNDO_INSERT:
                start = u->undotemp;
                start--;
                if (keysym(u->data[start]))
                    while (start > 0 && keysym(u->data[start - 1]))
                        start--;
                p->selstartcharpos = u->preselstart + start;
                p->selendcharpos = p->selstartcharpos + u->undotemp - start;
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
                u->postselstart = u->postselend += start - u->undotemp;
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
                        p->selstartcharpos = p->selendcharpos = u->postselstart - start - 1;
                        n = p->cd->text[p->selstartcharpos].ch;
                        undo_pchar(hwnd, p, u->data[start]);
                        u->data[start++] = n;
                    }
                else
                {
                    int n;
                    p->selstartcharpos = p->selendcharpos = u->postselstart - start - 1;
                    n = p->cd->text[p->selstartcharpos].ch;
                    undo_pchar(hwnd, p, u->data[start]);
                    u->data[start++] = n;
                    if (u->data[start] == '\n')
                    {
                        p->selstartcharpos = p->selendcharpos = u->postselstart - start;
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
                Replace(hwnd, p, (char*)u->data, u->len);
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

int doredo(HWND hwnd, EDITDATA* p)
{
    int rv = 1;
    int start;
    if (SendMessage(hwnd, EM_CANREDO, 0, 0))
    {
        int oldinsert = p->cd->inserting;
        UNDO* u;
        int x = p->cd->redopos;
        if (x >= UNDO_MAX)
            x -= UNDO_MAX;
        if (x == p->cd->undotail)
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
                if (keysym(u->data[start - 1]))
                {
                    while (start > 0 && keysym(u->data[start - 1]))
                    {
                        start--;
                    }
                }
                else
                {
                    start--;
                }
                p->selstartcharpos = u->preselstart;
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
                if (keysym(u->data[start - 1]))
                {
                    while (start > 0 && keysym(u->data[start - 1]))
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

                if (keysym(u->data[start - 1]))
                    while (start > 0 && keysym(u->data[start - 1]))
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
                Replace(hwnd, p, (char*)u->data, u->len);
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
