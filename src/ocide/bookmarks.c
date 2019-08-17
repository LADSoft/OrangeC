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

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>

#include "header.h"
#include "idewinconst.h"
extern DWINFO* editWindows;
extern HWND hwndFrame;
extern HINSTANCE hInstance;

HWND hwndBookmark;

DWINFO currentBM;

extern struct tagfile *tagFileList, *last_module;

BOOL AnyBookmarks(void)
{
    struct tagfile* l = tagFileList;
    while (l)
    {
        if (l->tagArray[TAG_BOOKMARK])
            return TRUE;
        l = l->next;
    }
    return FALSE;
}
void ToggleBookMark(int linenum)
{
    HWND hWnd = GetFocus();
    DWINFO* ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && ptr->dwHandle == hWnd)
        {
            char buf[256], *ch;
            if (linenum == -1)
                SendMessage(ptr->dwHandle, EM_GETSEL, (WPARAM)&linenum, 0);

            linenum = SendMessage(ptr->dwHandle, EM_LINEFROMCHAR, linenum, 0) + 1;
            memset(buf, 0, 256);
            *(short*)buf = 256;

            SendMessage(ptr->dwHandle, EM_GETLINE, (WPARAM)linenum - 1, (LPARAM)buf);
            if (Tag(TAG_BOOKMARK, ptr->dwName, linenum, 0, ch = strdup(buf), 0, 0))
            {
                strcpy(currentBM.dwName, ptr->dwName);
                currentBM.dwLineNo = linenum;
            }
            else
                free(ch);
            break;
        }
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

int findbmpos(struct tagfile** l, struct tag** t)
{
    struct tagfile* il;
    (*l) = tagFileList;
    if (currentBM.dwName[0])
    {
        while ((*l))
        {
            if (!stricmp(currentBM.dwName, (*l)->name))
                break;
            (*l) = (*l)->next;
        }
        if (!(*l))
            (*l) = tagFileList;
    }
    if (!(*l))
        return 0;
    il = (*l);

    do
    {
        (*t) = (*l)->tagArray[TAG_BOOKMARK];
        while ((*t))
        {
            if ((*t)->drawnLineno >= currentBM.dwLineNo || (*l) != il)
                return 1;
            (*t) = (*t)->next;
        }
        (*l) = (*l)->next;
        if (!(*l))
            (*l) = tagFileList;
        (*t) = (*l)->tagArray[TAG_BOOKMARK];
    } while ((*l) != il);
    return 0;
}

//-------------------------------------------------------------------------

void NextBookMark(void)
{
    struct tagfile* l;
    struct tag *t, *st;
    if (!findbmpos(&l, &t))
        return;
    st = t;
    t = t->next;
    do
    {
        if (t)
        {
            strcpy(currentBM.dwName, l->name);
            currentBM.dwLineNo = t->drawnLineno;
            currentBM.logMRU = FALSE;
            currentBM.newFile = FALSE;
            CreateDrawWindow(&currentBM, TRUE);
            return;
        }
        else
        {
            l = l->next;
            if (!l)
                l = tagFileList;
            t = l->tagArray[TAG_BOOKMARK];
        }
    } while (t != st);
    strcpy(currentBM.dwName, l->name);
    currentBM.dwLineNo = t->drawnLineno;
    currentBM.logMRU = FALSE;
    currentBM.newFile = FALSE;
    CreateDrawWindow(&currentBM, TRUE);
}

//-------------------------------------------------------------------------

void NextBookMarkFile(void)
{
    struct tagfile* l;
    struct tag* t;
    if (!findbmpos(&l, &t))
        return;
    do
    {
        l = l->next;
    } while (l && !l->tagArray[TAG_BOOKMARK]);

    if (!l)
    {
        l = tagFileList;
        while (l && !l->tagArray[TAG_BOOKMARK])
        {
            l = l->next;
        }
    }
    t = l->tagArray[TAG_BOOKMARK];
    if (t)
    {
        strcpy(currentBM.dwName, l->name);
        currentBM.dwLineNo = t->drawnLineno;
        currentBM.logMRU = FALSE;
        currentBM.newFile = FALSE;
        CreateDrawWindow(&currentBM, TRUE);
        return;
    }
}
//-------------------------------------------------------------------------

void PreviousBookMark(void)
{
    struct tagfile* l;
    struct tag *t, *st;
    if (!findbmpos(&l, &t))
        return;
    st = t;
    t = t->prev;
    do
    {
        if (t)
        {
            strcpy(currentBM.dwName, l->name);
            currentBM.dwLineNo = t->drawnLineno;
            currentBM.logMRU = FALSE;
            currentBM.newFile = FALSE;
            CreateDrawWindow(&currentBM, TRUE);
            return;
        }
        else
        {
            do
            {
                l = l->prev;
            } while (l && !l->tagArray[TAG_BOOKMARK]);

            if (!l)
            {
                l = tagFileList;
                while (l->next)
                {
                    l = l->next;
                }
                while (l && !l->tagArray[TAG_BOOKMARK])
                {
                    l = l->prev;
                }
            }
            t = l->tagArray[TAG_BOOKMARK];
            while (t->next)
                t = t->next;
        }
    } while (t != st);
    strcpy(currentBM.dwName, l->name);
    currentBM.dwLineNo = t->drawnLineno;
    currentBM.logMRU = FALSE;
    currentBM.newFile = FALSE;
    CreateDrawWindow(&currentBM, TRUE);
}

//-------------------------------------------------------------------------

void PreviousBookMarkFile(void)
{
    struct tagfile* l;
    struct tag* t;
    if (!findbmpos(&l, &t))
        return;
    do
    {
        l = l->prev;
    } while (l && !l->tagArray[TAG_BOOKMARK]);

    if (!l)
    {
        l = tagFileList;
        while (l->next)
        {
            l = l->next;
        }
        while (l && !l->tagArray[TAG_BOOKMARK])
        {
            l = l->prev;
        }
    }
    t = l->tagArray[TAG_BOOKMARK];
    while (t->next)
        t = t->next;
    if (t)
    {
        strcpy(currentBM.dwName, l->name);
        currentBM.dwLineNo = t->drawnLineno;
        currentBM.logMRU = FALSE;
        currentBM.newFile = FALSE;
        CreateDrawWindow(&currentBM, TRUE);
        return;
    }
}
//-------------------------------------------------------------------------

LRESULT CALLBACK BMProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    LPMEASUREITEMSTRUCT mi;
    LV_COLUMN lvC;
    LV_ITEM item;
    RECT r;
    struct tagfile* l = tagFileList;
    struct tag *t, *t1;
    HWND hwndlb;
    int i;
    switch (iMessage)
    {
        case WM_NOTIFY:
            if (wParam == IDC_BMLISTBOX)
            {
                if (((LPNMHDR)lParam)->code == NM_DBLCLK)
                {
                    SendMessage(hwnd, WM_COMMAND, IDC_BMGOTO, 0);
                }
            }
            break;
        case WM_COMMAND:
            if (wParam == IDCANCEL)
            {
                DestroyWindow(hwnd);
                hwndBookmark = 0;
            }
            else if (wParam == IDC_BMGOTO)
            {
                int sel = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_BMLISTBOX));
                if (sel == -1)
                {
                    EndDialog(hwnd, 1);
                    break;
                }
                item.iItem = sel;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM;
                ListView_GetItem(GetDlgItem(hwnd, IDC_BMLISTBOX), &item);
                t1 = (struct tag*)item.lParam;
                while (l)
                {
                    t = l->tagArray[TAG_BOOKMARK];
                    while (t && t != t1)
                        t = t->next;
                    if (t)
                    {
                        strcpy(currentBM.dwName, l->name);
                        currentBM.dwLineNo = t->drawnLineno;
                        currentBM.logMRU = FALSE;
                        currentBM.newFile = FALSE;
                        CreateDrawWindow(&currentBM, TRUE);
                        break;
                    }
                    l = l->next;
                }
                EndDialog(hwnd, 1);
            }
            break;
        case WM_MEASUREITEM:
            mi = (LPMEASUREITEMSTRUCT)lParam;
            hwndlb = GetDlgItem(hwnd, IDC_BMLISTBOX);
            GetClientRect(hwndlb, &r);
            mi->itemWidth = r.right;
            mi->itemHeight = 14;
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            hwndlb = GetDlgItem(hwnd, IDC_BMLISTBOX);
            ApplyDialogFont(hwndlb);
            GetWindowRect(hwndlb, &r);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
            lvC.cx = r.right - r.left;
            lvC.iSubItem = 0;
            ListView_InsertColumn(hwndlb, 0, &lvC);
            i = 0;
            while (l)
            {
                t = l->tagArray[TAG_BOOKMARK];
                while (t)
                {
                    char buf[256];
                    sprintf(buf, "%s(%d): %s", l->name, t->drawnLineno, t->extra);
                    item.iItem = i++;
                    item.iSubItem = 0;
                    item.mask = LVIF_PARAM | LVIF_TEXT;
                    item.lParam = (LPARAM)t;
                    item.pszText = strdup(buf);
                    ListView_InsertItem(hwndlb, &item);
                    t = t->next;
                }
                l = l->next;
            }
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

void ShowBookMarks(void) { hwndBookmark = CreateDialog(hInstance, "BOOKMARKDLG", hwndFrame, (DLGPROC)BMProc); }
