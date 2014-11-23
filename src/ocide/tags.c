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
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>

#include "header.h"
#include "winconst.h"
extern DWINFO *editWindows;
extern HWND hwndFrame;
extern HINSTANCE hInstance;

HWND hwndBookmark;

static char bpModule[256];
static int bpLine;

DWINFO currentBM;

struct tagfile *tagFileList,  *last_module;

int oldline(struct tagfile *, int, int);

int IsTagged(char *module, int line)
{
    struct tagfile *l = tagFileList;
    if (last_module && !stricmp(module, last_module->name))
    {
        l = last_module;
        goto join;
    } 
    while (l)
    {
        if (!stricmp(l->name, module))
        {
            int i;
            last_module = l;
            join: for (i = 0; i < TAG_MAX; i++)
            {
                struct tag *t = l->tagArray[i];
                while (t)
                {
                    int tl = t->drawnLineno;
                    if (line == tl)
                        if (t->enabled)
                            if (t->enabled &TAGF_GRAYED)
                                return TAG_BPGRAYED;
                            else
                                return i;
                            else
                                break;
                    t = t->next;
                } 
            }
            return  - 1;
        }
        l = l->next;
    }
    return  - 1;
}
int TagAnyBreakpoints(void)
{
    struct tagfile *l = tagFileList;
    while (l)
    {
        if (l->tagArray[TAG_BP])
            return TRUE;
        l = l->next;
    }
    return FALSE;
}
//-------------------------------------------------------------------------

int Tag(int type, char *name, int drawnLineno, int charpos, void *extra, int
    freeextra, int alwaysadd)
{
    struct tagfile **l = &tagFileList, *prev = NULL;
    struct tag *t,  **oldt , *tprev = NULL;
    while (*l)
    {
        if (!stricmp((*l)->name, name))
            break;
        prev = *l;
        l = &(*l)->next;
    } 
    if (! *l)
    {
        *l = calloc(sizeof(struct tagfile), 1);
        if (! *l)
            return FALSE ;
        (*l)->prev = prev;
        strcpy((*l)->name, name);
    }
    t = (*l)->tagArray[type];
    oldt = &(*l)->tagArray[type];
    while (t)
    {
        int tl = t->drawnLineno;
        if (drawnLineno == tl)
        {
            if (alwaysadd)
            {
                t->enabled = TAGF_ENABLED;
                if (!t->enabled && type == TAG_BP)
                {
                    int tl = GetBreakpointNearestLine((*l)->name, t->debugLineno, TRUE);
                    if (!dbgSetBreakPoint(name, tl, extra))
                        t->enabled |= TAGF_GRAYED;
                }
                InvalidateByName((*l)->name);
                return TRUE;
            } 

            if (t->enabled && type == TAG_BP && (!t->extra || freeextra) ||
                type != TAG_BP)
            {
                if (t->next)
                    t->next->prev = (*oldt)->prev;
                *oldt = t->next;
                if (type == TAG_BP)
                {
                    int tl = GetBreakpointNearestLine((*l)->name, t->debugLineno, TRUE);
                    dbgClearBreakPoint((*l)->name, tl);
                }
                free(t->extra);
                free(t);
                if (type == TAG_BP)
                    SendMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                InvalidateByName((*l)->name);
            }
            else
            {
                t->enabled = !t->enabled;
                if (t->enabled)
                {
                    int tl = GetBreakpointNearestLine((*l)->name, t->debugLineno, TRUE);
                    if (!dbgSetBreakPoint((*l)->name, tl, t->extra))
                        t->enabled |= TAGF_GRAYED;
                }
                else
                {
                    int tl = GetBreakpointNearestLine((*l)->name, t->debugLineno, TRUE);
                    dbgClearBreakPoint((*l)->name, tl);
                }
                InvalidateByName((*l)->name);
                return t->enabled;
            }
            return FALSE;

        }
        if (drawnLineno < t->drawnLineno)
            break;
        tprev = t;
        oldt = &t->next;
        t = t->next;
    }
    t = calloc(sizeof(struct tag), 1);
    if (!t)
        return FALSE ;
    t->editingLineno = t->drawnLineno = drawnLineno;
    t->debugLineno = oldline(l, drawnLineno, FALSE);
    t->charpos = charpos;
    t->extra = extra;
    t->enabled = TAGF_ENABLED;
    if (*oldt)
    {
        (*oldt)->prev = t;
    }
    t->prev = tprev;
    t->next =  *oldt;
    *oldt = t;
    if (type == TAG_BP)
    {
        int tl = GetBreakpointNearestLine(name, t->debugLineno, TRUE);
        if (!dbgSetBreakPoint(name, tl, extra))
            t->enabled |= TAGF_GRAYED;
        SendMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    }
    InvalidateByName((*l)->name);
    return TRUE;
}

//-------------------------------------------------------------------------

void TagRemoveAll(int type)
{
    struct tagfile *l = &tagFileList;
    struct tag *t;
    while (l)
    {
        t = l->tagArray[type];
        l->tagArray[type] = NULL;
        while (t)
        {
            struct tag *nt = t->next;
            if (type == TAG_BP)
            {
                int tl = GetBreakpointNearestLine(l->name, t->debugLineno, TRUE);
                dbgClearBreakPoint(l->name, tl);
            }
            free(t->extra);
            free(t);
            t = nt;
        } InvalidateByName(l->name);
        l = l->next;
    }
}
// used after BP are restored from data file
void TagRegenBreakPoints(void)
{
    struct tagfile *l = tagFileList;
    struct tag *t;
    int flag = FALSE;
    while (l)
    {
        t = l->tagArray[TAG_BP];
        while (t)
        {
            if (t->enabled == TAGF_ENABLED)
            {
                int tl;
                tl = GetBreakpointNearestLine(l->name, t->debugLineno, TRUE);
                if (!dbgSetBreakPoint(l->name,tl, t->extra))
                {
                    flag = TRUE;
                    t->enabled |= TAGF_GRAYED;
                    InvalidateByName(l->name);
                } 
            }
            t = t->next;
        }
        l = l->next;
    }
    if (flag)
        ExtendedMessageBox("Breakpoints", 0, 
            "One or more breakpoints were not positioned on valid lines and were disabled");
}

//-------------------------------------------------------------------------

static int oldline(struct tagfile *l, int lineno, int allowmiddle)
{
    struct tagchangeln *c = l->changedLines;
    while (c && lineno > c->lineno)
    {
        if (lineno < c->lineno + c->delta)
        {
            if (allowmiddle)
                return c->lineno;
            else
                return 0;
        } 
        lineno -= c->delta;
        c = c->next;
    }
    return lineno;
}

//-------------------------------------------------------------------------

int TagOldLine(char *name, int lineno)
{
    struct tagfile *l = tagFileList;
    int rv = lineno;
    while (l)
    {
        if (!stricmp(l->name, name))
        {
            return oldline(l, lineno, FALSE);
        } l = l->next;
    }
    return lineno;
}

//-------------------------------------------------------------------------

static int newline(struct tagfile *l, int line)
{
    int delta = 0;
    struct tagchangeln *c = l->changedLines;
    while (c && line >= c->lineno)
    {
        delta += c->delta;
        c = c->next;
    } return delta + line;
}

//-------------------------------------------------------------------------

int TagNewLine(char *name, int lineno)
{
    struct tagfile *l = tagFileList;
    int rv = lineno;
    while (l)
    {
        if (!stricmp(l->name, name))
        {
            return newline(l, lineno);
        } 
        l = l->next;
    }
    return lineno;
}

//-------------------------------------------------------------------------

void TagRemoveOld(struct tagfile *l)
{
    struct tagchangeln *c = l->changedLines;
    while (c)
    {
        struct tagchangeln *x = c->next;
        free(c);
        c = x;
    } 
    l->changedLines = 0;
}

//-------------------------------------------------------------------------

static void TagInsertChange(struct tagfile *l, int lineno, int delta)
{
    struct tagchangeln **c = &l->changedLines,  *newone,  **insert = 0;
    int found = FALSE;
    lineno = oldline(l, lineno, TRUE);
    while (*c)
    {
        if ((*c)->lineno >= lineno)
        {
            if ((*c)->lineno == lineno)
            {
                found = TRUE;
                (*c)->delta += delta;
            }
            else
                if (!insert)
                    insert = c;
        } 
        c = &(*c)->next;
    }
    if (!found)
    {
        if (!insert)
            insert = c;
        newone = calloc(1, sizeof(struct tagchangeln));
        if (!newone)
            return ;
        newone->next =  *insert;
        newone->lineno = lineno;
        newone->delta = delta;
        (*insert) = newone; // relies on link being first field
    }
}

//-------------------------------------------------------------------------

void TagLineChange(char *name, int drawnLineno, int delta)
{
    struct tagfile *l = tagFileList;

    int i;
    if (delta == 0)
        return;
    while (l)
    {
        if (!stricmp(l->name, name))
        {
            TagInsertChange(l, drawnLineno, delta);
            for (i = 0; i < TAG_MAX; i++)
            {
                struct tag *t = l->tagArray[i];
                while (t)
                {
                    if (t->drawnLineno >= drawnLineno)                        
                        t->drawnLineno += delta;
                    t = t->next;
                } 
            }
            break;
        }
        l = l->next;
    }
    if (!l)
    {
        l = calloc(sizeof(struct tagfile), 1);
        if (l)
        {
            struct tagfile **ls = &tagFileList;
            while (*ls)
                ls = &(*ls)->next;
            *ls = l;
            strcpy(l->name, name);
            TagInsertChange(l, drawnLineno, delta);
        } 
    }
}

//-------------------------------------------------------------------------

void TagLinesAdjust(char *name, int mode)
{
    struct tagfile *l = tagFileList;
    int i;
    while (l)
    {
        if (!name || !stricmp(l->name, name))
        {
            if (mode == TAGM_DISCARDFILE || mode == TAGM_UPDATEDEBUG)
                TagRemoveOld(l);
            for (i = 0; i < TAG_MAX; i++)
            {
                struct tag *t = l->tagArray[i];
                while (t)
                {
                    switch (mode)
                    {
                        case TAGM_SAVEFILE:
                            t->editingLineno = t->drawnLineno;
                            break;
                        case TAGM_DISCARDFILE:
                            t->drawnLineno = t->editingLineno;
                            break;
                        case TAGM_UPDATEDEBUG:
                            t->debugLineno = t->drawnLineno;
                            break;
                    }
                    t = t->next;
                }
            }
        }
        l = l->next;
    }
}

//-------------------------------------------------------------------------

void TagBreakpoint(char *module, int line)
{
    if (module)
    {
        strcpy(bpModule, module);
    }
    else
    {
        bpModule[0] = 0;
    }
    bpLine = line;
}

//-------------------------------------------------------------------------

int BPLine(char *module)
{
    if (bpModule[0] && !stricmp(bpModule, module))
        return TagNewLine(module, bpLine);
    return 0;
}

//-------------------------------------------------------------------------

void TagEraseAllEntries(void)
{
    int i;
    struct tagfile *l = tagFileList;
    for (i = 0; i < TAG_MAX; i++)
        TagRemoveAll(i);
    while (l)
    {
        struct tagfile *s = l->next;
        TagRemoveOld(l);
        free(l);
        l = s;
    } 
    tagFileList = 0;
    last_module = 0;
}

//-------------------------------------------------------------------------
BOOL AnyBookmarks(void)
{
    struct tagfile *l = tagFileList;
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
    DWINFO *ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && ptr->dwHandle == hWnd)
        {
            char buf[256],  *ch;
            int addr;
            if (linenum == -1)
                SendMessage(ptr->dwHandle, EM_GETSEL, (WPARAM) &linenum, 0);

            linenum = SendMessage(ptr->dwHandle, EM_LINEFROMCHAR, linenum, 0) +
                1;
            memset(buf, 0, 256);
            *(short*)buf = 256;

            SendMessage(ptr->dwHandle, EM_GETLINE, (WPARAM)linenum - 1, (LPARAM)
                buf);
            if (Tag(TAG_BOOKMARK, ptr->dwName, linenum, 0, ch = strdup(buf), 0,
                0))
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

int findbmpos(struct tagfile **l, struct tag **t)
{
    struct tagfile *il;
    (*l) = tagFileList;
    if (currentBM.dwName[0])
    {
        while ((*l))
        {
            if (!stricmp(currentBM.dwName, (*l)->name))
                break;
            (*l) = (*l)->next;
        } if (!(*l))
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
            if ((*t)->drawnLineno >= currentBM.dwLineNo || (*l) != il)return 1;
            (*t)
                 = (*t)->next;
        }
        (*l) = (*l)->next;
        if (!(*l))
            (*l) = tagFileList;
        (*t) = (*l)->tagArray[TAG_BOOKMARK];
    }
    while ((*l) != il)
        ;
    return 0;
}

//-------------------------------------------------------------------------

void NextBookMark(void)
{
    struct tagfile *l;
    struct tag *t,  *st;
    if (!findbmpos(&l, &t))
        return ;
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
            return ;
        } 
        else
        {
            l = l->next;
            if (!l)
                l = tagFileList;
            t = l->tagArray[TAG_BOOKMARK];
        }
    }
    while (t != st);
    strcpy(currentBM.dwName, l->name);
    currentBM.dwLineNo = t->drawnLineno;
    currentBM.logMRU = FALSE;
    currentBM.newFile = FALSE;
    CreateDrawWindow(&currentBM, TRUE);

}

//-------------------------------------------------------------------------

void NextBookMarkFile(void)
{
    struct tagfile *l;
    struct tag *t;
    if (!findbmpos(&l, &t))
        return ;
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
        return ;
    } 

}
//-------------------------------------------------------------------------

void PreviousBookMark(void)
{
    struct tagfile *l;
    struct tag *t, *st;
    if (!findbmpos(&l, &t))
        return ;
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
            return ;
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
    }
    while (t != st);
    strcpy(currentBM.dwName, l->name);
    currentBM.dwLineNo = t->drawnLineno;
    currentBM.logMRU = FALSE;
    currentBM.newFile = FALSE;
    CreateDrawWindow(&currentBM, TRUE);
}

//-------------------------------------------------------------------------

void PreviousBookMarkFile(void)
{
    struct tagfile *l;
    struct tag *t;
    if (!findbmpos(&l, &t))
        return ;
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
        return ;
    } 
}
//-------------------------------------------------------------------------

LRESULT CALLBACK BMProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam)
{
    LPMEASUREITEMSTRUCT mi;
    LV_COLUMN lvC;
    LV_ITEM item;
    RECT r;
    struct tagfile *l = &tagFileList;
    struct tag *t,  *t1;
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
                int sel = ListView_GetSelectionMark(GetDlgItem(hwnd,
                    IDC_BMLISTBOX));
                if (sel ==  - 1)
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
                    sprintf(buf, "%s(%d): %s", l->name, t->drawnLineno, t
                        ->extra);
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

void ShowBookMarks(void)
{
    hwndBookmark = CreateDialog(hInstance, "BOOKMARKDLG", hwndFrame, (DLGPROC)
        BMProc);
}
