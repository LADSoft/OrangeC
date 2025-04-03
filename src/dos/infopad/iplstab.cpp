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

#include <windows.h>
#include <richedit.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TPoint
#define Uses_TView
#define Uses_TEvent
#define Uses_TSortedCollection
#define Uses_TListBox
#define Uses_TButton
#define Uses_MsgBox
#include "tv.h"
#include "infopad.h"
#include "editview.h"


#define MAX_MENU_ROW 15
#define ID_MENUITEMS (2000)

struct _singleTab *CreateItem(char *name, LPARAM lParam)
{
    struct _singleTab *newItem = (struct _singleTab *)calloc(1, sizeof(struct _singleTab));
    if (newItem)
    {
        char *p = strrchr(name, '\\');
        if (p)
            p++;
        else
            p = name;
        strcpy(newItem->text, p);
        newItem->lParam = lParam;
    }
    return newItem;
}
void RemoveFromActive(struct _tabStruct *ptr, struct _singleTab *tabs)
{
    if (tabs->chain.prev)
    {
        if (tabs->chain.next)
            tabs->chain.next->chain.prev = tabs->chain.prev;
        else
            ptr->active.tail = tabs->chain.prev;
        tabs->chain.prev->chain.next = tabs->chain.next;
    }
    else if (tabs->chain.next)
    {
        tabs->chain.next->chain.prev = NULL;
        ptr->active.head = tabs->chain.next;
    }
    else
    {
        ptr->active.head = ptr->active.tail = NULL;
    }
}
void RemoveFromSelected(struct _tabStruct *ptr, struct _singleTab *tabs)
{
    if (tabs->selectedChain.prev)
    {
        if (tabs->selectedChain.next)
            tabs->selectedChain.next->selectedChain.prev = tabs->selectedChain.prev;
        else
            ptr->selected.tail = tabs->selectedChain.prev;
        tabs->selectedChain.prev->selectedChain.next = tabs->selectedChain.next;
    }
    else if (tabs->selectedChain.next)
    {
        tabs->selectedChain.next->selectedChain.prev = NULL;
        ptr->selected.head = tabs->selectedChain.next;
    }
    else
    {
        ptr->selected.head = ptr->selected.tail = NULL;
    }
}
void AddToActive(struct _tabStruct *ptr, struct _singleTab *tabs)
{
    tabs->chain.next = ptr->active.head;
    tabs->chain.prev = NULL;
    if (ptr->active.head)
    {
        ptr->active.head->chain.prev = tabs;
        ptr->active.head = tabs;
    }
    else
    {
        ptr->active.head = ptr->active.tail = tabs;
    }
}
void AddToSelected(struct _tabStruct *ptr, struct _singleTab *tabs)
{
    tabs->selectedChain.next = ptr->selected.head;
    tabs->selectedChain.prev = NULL;
    if (ptr->selected.head)
    {
        ptr->selected.head->selectedChain.prev = tabs;
        ptr->selected.head = tabs;
    }
    else
    {
        ptr->selected.head = ptr->selected.tail = tabs;
    }
}
void _tabStruct::draw()
{
    TRect r = getExtent();
    struct _tabStruct *ptr = this;
    struct _singleTab *tab = ptr->active.head;
    TPoint size;
    int right = r.b.x;
    int x = 0;
    int y = 0;
    int rightInset = 7;
    TDrawBuffer b;
    b.moveChar(0, ' ', 0x70, r.b.x);
    while (tab)
    {
        HFONT xfont;
        int color;
        char *p = strrchr(tab->text, '\\');
        if (p)
            p++;
        else
            p = tab->text;
        
        if (ptr->selected.head == tab)
        {
            color = 0xf0;
        }
        else
        {
            color = 0x70;
        }
        size.x = strlen(p);
        if (x + size.x + 1 + !!tab->modified + rightInset > right)
            break;

        char buf[256];
        strcpy(buf,tab->text);
        if (tab->modified)
            strcat(buf, "*");
        strcat(buf, "\263");
        b.moveCStr(x, buf, color);

        tab->displayRect.a.x = x;
        tab->displayRect.a.y = 0;
        x += size.x + 1 + !!tab->modified;
        b.putAttribute(x-1, 0x70);
        tab->displayRect.b.x = x;
        tab->displayRect.b.y = 1;
        tab->displayed = TRUE;
        tab = tab->chain.next;
    }
    if (x)
        b.moveChar(x-1, ' ', 0x70, 1);
    b.moveCStr(r.b.x - 6, "[ ][ ]", 0x70);
    b.moveChar(r.b.x - 5, '\xfe', 0x72, 1); // block
    b.moveChar(r.b.x -2, '\x19', 0x72, 1); // down arrow
    writeBuf(0, 0, r.b.x, 1, b);
    while (tab)
    {
        tab->displayed = FALSE;
        tab = tab->chain.next;
    }
}
int GetItem(struct _tabStruct *ptr , int index)
{
    struct _singleTab *tabs = ptr->active.head;
    int i;
    for (i = 0; i < index && tabs; i++, tabs = tabs->chain.next);
    if (!tabs)
        return NULL;
    return tabs->lParam;
}
struct _singleTab *AddTab(struct _tabStruct *ptr, char *name, LPARAM lParam)
{
    struct _singleTab *tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
            return NULL;
        tabs = tabs->chain.next;
    }
    tabs = CreateItem(name, lParam);
    if (tabs)
    {
        AddToActive(ptr, tabs);
        AddToSelected(ptr, tabs);
    }
    return tabs;
}
struct _singleTab *RemoveTab(struct _tabStruct *ptr, LPARAM lParam)
{
    
    struct _singleTab *tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
        {
            RemoveFromActive(ptr, tabs);
            RemoveFromSelected(ptr, tabs);

            free(tabs->text);
            free(tabs);
            if (ptr->selected.head && !ptr->selected.head->displayed)
            {
                RemoveFromActive(ptr, ptr->selected.head);
                AddToActive(ptr, ptr->selected.head);
            }
            return NULL;
        }
        tabs = tabs->chain.next;
    }
    return NULL;
}
struct _singleTab *ModifyTab(struct _tabStruct *ptr, WPARAM modified, LPARAM lParam)
{
    
    struct _singleTab *tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
        {
            tabs->modified = modified;
            break;
        }
        tabs = tabs->chain.next;
    }
    return NULL;
}
void SelectTab(struct _tabStruct *ptr, LPARAM lParam)
{
    struct _singleTab *tabs = ptr->active.head, *cur;
    while (tabs)
    {
        if (lParam == tabs->lParam)
        {
            if (!tabs->displayed)
            {
                RemoveFromActive(ptr, tabs);
                AddToActive(ptr, tabs);				
            }
            RemoveFromSelected(ptr, tabs);
            AddToSelected(ptr, tabs);
                
            return;
        }
        tabs = tabs->chain.next;
    }
}
void RemoveAllTabs(struct _tabStruct *ptr)
{
    while (ptr->active.head)
    {
        struct _singleTab *next =ptr->active.head->chain.next;
        free(ptr->active.head->text);
        free(ptr->active.head);
        ptr->active.head = next;
    }
    ptr->active.head = ptr->active.tail = NULL;
    ptr->selected.head = ptr->selected.tail = NULL;
}
static BOOL PointInRect(TRect &rect, TPoint pt)
{
    if (pt.x >= rect.a.x && pt.x < rect.b.x)
        if (pt.y >= rect.a.y && pt.y < rect.b.y)
            return TRUE;
    return FALSE;
}
struct _singleTab * FindTabByPos( struct _tabStruct *ptr, TPoint pt)
{
    struct _singleTab *tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->displayed)
            if (PointInRect(tabs->displayRect, pt))
            {
                return tabs;
            }
        tabs = tabs->chain.next;
    }
    return NULL;
}
BOOL IsCloseButton(struct _tabStruct *ptr, TPoint pt)
{
    TRect r = ptr->getExtent();
    return pt.x == r.b.x - 5;
}
BOOL IsWindowButton(struct _tabStruct *ptr, TPoint pt)
{
    TRect r = ptr->getExtent();
    return pt.x == r.b.x - 2;
}
BOOL ChangeHighLight( struct _tabStruct *ptr, struct _singleTab *tab)
{
    return FALSE;
}
void Notify(InfoPad *parent, struct _singleTab *tab, int notifyCode)
{
    LSTABNOTIFY notify;
    notify.hdr.code = notifyCode;
    notify.text = tab->text;
    notify.lParam = tab->lParam;
    parent->WinMessage(WM_NOTIFY, 0, (LPARAM)&notify);
}
class TTabMenuCollection : public TSortedCollection
{
public:
    TTabMenuCollection() : TSortedCollection(100, 1) { }
    virtual ~TTabMenuCollection() { }
    virtual int compare( void *key1, void *key2 )
    {
        _singleTab *p = (_singleTab *)key1;
        _singleTab *q = (_singleTab *)key2;
        return strcmp(p->text, q->text);
    }
    virtual void *readItem( ipstream& )  { return NULL; }
    virtual void writeItem( void *xx, opstream&strm ) { }
};

static void DisplayTabMenu( struct _tabStruct *ptr)
{
    int n =0;
    struct _singleTab *h = ptr->active.head;
    while (h)
    {
        n++;
        h = h->chain.next;
    }
    int columns = (n + 17)/17;
    int rows = (n + columns-1)/columns;
    TRect bounds(0, 0, columns * 14+2, rows+7);
    bounds.move(TProgram::deskTop->size.x - bounds.b.x-1, 2);

    TDialog *dialog = new TDialog(bounds, "");
    TRect r = TRect(1,1, columns * 14, rows+1);
    dialog->insert( new TListBox(r, columns, NULL));
    
    r = TRect( dialog->size.x - 14, dialog->size.y - 5,
               dialog->size.x - 2, dialog->size.y - 3);
    dialog->insert( new TButton(r, MsgBoxText::okText, cmOK, bfDefault));
    r.a.y += 2;
    r.b.y += 2;
    dialog->insert( new TButton(r, MsgBoxText::cancelText, cmCancel, bfDefault));

    TListBoxRec rec;
    rec.items = new TTabMenuCollection;
    rec.selection = 0;
    h = ptr->active.head;
    while (h)
    {
        struct _singleTab *x = new _singleTab;
        *x = *h;
        rec.items->insert(x);
        h = h->chain.next;
    }
    dialog->setData(&rec);

    ushort c = TProgram::application->execView(dialog);
    if (c == cmOK)
    {
        dialog->getData(&rec);
        _singleTab *p = (_singleTab *)rec.items->at(rec.selection);
        if (p)
        {
            Notify(ptr->parent, p, TABN_SELECTED);
        }
    }
    TObject::destroy(rec.items);
    TObject::destroy( dialog );
}
static void tabRename( struct _tabStruct *ptr, char *newName, LPARAM lParam)
{
    struct _singleTab *tabs = ptr->active.head;
    while (tabs)
    {
        if (tabs->lParam == lParam)
        {
            char *p = strrchr(newName, '\\');
            if (p)
                p++;
            else
                p = newName;
            strcpy(tabs->text, p);
            if (tabs->displayed)
                ptr->drawView();
        }
        tabs = tabs->chain.next;
    }
}
static void DragTo( struct _tabStruct *ptr, struct _singleTab *tab)
{
    struct _singleTab *find = ptr->dragSrc;
    BOOL left;
    while (find)
    {
        if (find == tab)
            break;
        find = find->chain.next;
    }
    left = find == NULL;
    RemoveFromActive(ptr, ptr->dragSrc);
    if (left)
    {
        if (tab->chain.prev == NULL)
        {
            AddToActive(ptr, ptr->dragSrc);
        }
        else
        {
            ptr->dragSrc->chain.next = tab->chain.prev->chain.next;
            ptr->dragSrc->chain.prev = tab->chain.prev;
            tab->chain.prev->chain.next = ptr->dragSrc;
            tab->chain.prev = ptr->dragSrc;
        }
    }
    else
    {
        ptr->dragSrc->chain.next = tab->chain.next;
        ptr->dragSrc->chain.prev = tab;
        if (tab->chain.next)
            tab->chain.next->chain.prev = ptr->dragSrc;
        tab->chain.next = ptr->dragSrc;
    }
    ptr->drawView();
}
void _tabStruct::handleEvent(TEvent &event)
{
    TView::handleEvent(event);
    _tabStruct *ptr = this;
    switch(event.what)
    {
        case evMouseDown:
#if 0
            if (event.mouse.buttons & 1)
            {
                tab = FindTabByPos(ptr, lParam);
                if (tab && (GetWindowLong(GWL_STYLE) & TABS_DRAGABLE))
                {
                    ptr->dragging = TRUE;
                    ptr->dragSrc = tab;
                    if (!ptr->captured)
                    {
                        SetCapture(hwnd);
                        ptr->captured = TRUE;
                    }
                }
            }
#endif
            return;
        case evMouseUp:
        {
            TPoint pt = makeLocal(event.mouse.where);
            struct _singleTab *tab = FindTabByPos(ptr, pt);
#if 0
            if (ptr->dragging && ptr->oldCursor)
            {
                if (tab && tab != ptr->dragSrc)
                {
                    DragTo(ptr, tab);
                }
                SetCursor(ptr->oldCursor);
                ptr->oldCursor = NULL;
            }
            else 
#endif
            if (tab)
            {
                SelectTab(ptr, tab->lParam);
                Notify(parent, tab, TABN_SELECTED);
                ptr->drawView();
            }
            else if (IsCloseButton(ptr, pt))
            {
                Notify(parent, ptr->selected.head, TABN_CLOSED);
            }
            else if (IsWindowButton(ptr, pt))
            {
                DisplayTabMenu(ptr);
            }
            ptr->dragging = FALSE;
#if 0
            if (ptr->captured)
            {
                ptr->captured = FALSE;
                ReleaseCapture();
            }
#endif
            return;
        }
        case evMouseMove:
#if 0
            if (event.mouse.buttons & 1)
            {
                tab = FindTabByPos(ptr, lParam);
                
                if (ptr->dragging)
                {
                    HCURSOR old;
                    if (tab)
                    {
                        old = SetCursor(dragCur);
                    }
                    else
                    {
                        old = SetCursor(noCur);
                    }
                    if (!ptr->oldCursor)
                        ptr->oldCursor = old;
                }
            }
#endif
            break;
    }
}
DWORD _tabStruct::WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam)
{
    TRect r;
    LRESULT rv;
    struct _tabStruct *ptr = this;
    struct _singleTab *tab;
    switch (iMessage)
    {
        TOOLTIPTEXT *lpnhead;
        case TABM_GETITEM:
            return GetItem(ptr, wParam);
        case TABM_ADD:
            AddTab(ptr, (char *)wParam, lParam);
            ptr->drawView();
            break;
        case TABM_REMOVE:
            RemoveTab(ptr, lParam);
            ptr->drawView();
            break;
        case TABM_REMOVEALL:
            RemoveAllTabs(ptr);
            ptr->drawView();
            break;
        case TABM_SELECT:
            SelectTab(ptr, lParam);
            ptr->drawView();
            break;
        case TABM_GETCURSEL:
            return (LRESULT)ptr->selected.head;
        case TABM_RENAME:
            tabRename(ptr, (char *)wParam, lParam);
            return 0;
        case TABM_SETMODIFY:
            ModifyTab(ptr, wParam, lParam);
            ptr->drawView();
            return 0;
        case WM_CREATE:
            active.head = active.tail = NULL;
            selected.head = selected.tail = NULL;
            highlighted = NULL;
            fontHeight  = 0;
            captured = 0;
            flat = 0;
            displayMenu = 0;
            windowBtnMode = 0;
            dragging = 0;
            dragSrc = NULL;
            menuList = NULL;
            return rv;
        case WM_DESTROY:
            RemoveAllTabs(ptr);
            break;
        case WM_CLOSE:
            WinMessage(WM_DESTROY, 0, 0);
            break;
        case WM_SIZE:
            break;
        case WM_MOVE:
            break;
        default:
            break;
    }
    return 0;
}
