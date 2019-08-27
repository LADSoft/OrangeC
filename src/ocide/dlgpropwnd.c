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
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "header.h"
#include "rc.h"
//#include "props.h"
#include "rcgui.h"
extern PROJECTITEM* workArea;
extern HINSTANCE hInstance;
extern RESOURCE_DATA* currentResData;

static HWND hwndProps;

static WNDPROC oldEditProc, oldComboProc;
static HWND lvwindow;
static struct propertyFuncs* currentPropertyFuncs;
static void* currentPropertyData;
static int currentPropertyRow;
static HWND currentPropertyWindow;

// static PROPDESC *desc;

static char* szPropsClassName = "xccProps";
static char* szPropsTitle = "Properties";

static void SetListViewColumns(RECT* r)
{
    LV_COLUMN lvC;
    ListView_EnableGroupView(lvwindow, TRUE);
    ListView_SetExtendedListViewStyle(lvwindow, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER);

    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = (r->right - r->left) / 2;
    lvC.iSubItem = 0;
    lvC.pszText = "Property";
    ListView_InsertColumn(lvwindow, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = (r->right - r->left) / 2;
    lvC.iSubItem = 1;
    lvC.pszText = "Value";
    ListView_InsertColumn(lvwindow, 1, &lvC);
}
static void ResizeListViewColumns(void)
{
    RECT r;
    LV_COLUMN lvC;

    GetClientRect(lvwindow, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = (r.right - r.left) / 2;
    lvC.iSubItem = 0;
    ListView_SetColumn(lvwindow, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = (r.right - r.left) / 2;
    lvC.iSubItem = 1;
    ListView_SetColumn(lvwindow, 1, &lvC);
}
static LRESULT CALLBACK PropsEditorSubclassProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    if (iMessage == WM_KEYDOWN && wParam == VK_RETURN)
    {
        PostMessage((HWND)GetWindowLong(hwnd, GWL_USERDATA), WM_KILLPROPSEDITOR, 0, 0);
        return 0;
    }
    return CallWindowProc(oldEditProc, hwnd, iMessage, wParam, lParam);
}
static void SubclassPropsEditWindow(HWND hwnd)
{
    oldEditProc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
    SetWindowLong(hwnd, GWL_WNDPROC, (long)PropsEditorSubclassProc);
    SetWindowLong(hwnd, GWL_USERDATA, (long)hwndProps);
}
static LRESULT CALLBACK PropsComboSubclassProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    if (iMessage == WM_KEYDOWN && wParam == VK_RETURN)
    {
        PostMessage((HWND)GetWindowLong(hwnd, GWL_USERDATA), WM_KILLPROPSEDITOR, 0, 0);
        return 0;
    }
    return CallWindowProc(oldComboProc, hwnd, iMessage, wParam, lParam);
}
static void SubclassPropsComboWindow(HWND hwnd)
{
    oldComboProc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
    SetWindowLong(hwnd, GWL_WNDPROC, (long)PropsComboSubclassProc);
    SetWindowLong(hwnd, GWL_USERDATA, (long)hwndProps);
}
HWND PropGetHWNDCombobox(HWND parent, BOOL vscroll)
{
    HWND rv = CreateWindow("combobox", "", WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_SORT | CBS_DROPDOWN | (vscroll ? WS_VSCROLL : 0), CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, 0, hInstance, NULL);
    SubclassPropsComboWindow(rv);
    return rv;
}
HWND PropGetHWNDNumeric(HWND parent)
{
    HWND rv = CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT, parent, 0, hInstance, NULL);
    SubclassPropsEditWindow(rv);
    ApplyDialogFont(rv);
    return rv;
}
HWND PropGetHWNDText(HWND parent)
{
    HWND rv = CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT, parent, 0, hInstance, NULL);
    SubclassPropsEditWindow(rv);
    ApplyDialogFont(rv);
    return rv;
}
void PropSetExp(struct resRes* data, char* buf, EXPRESSION** exp)
{
    ResSetDirty(data);
    while (isspace(*buf))
        buf++;
    (*exp) = LookupWithTranslation(buf);
}
EXPRESSION* GetBaseId(EXPRESSION* exp)
{
    if (!exp)
        return NULL;
    if (exp->type == e_int && exp->rendition)
    {
        return exp;
    }
    else
    {
        EXPRESSION* rv = GetBaseId(exp->left);
        if (rv)
            return rv;
        rv = GetBaseId(exp->right);
        return rv;
    }
}
static int GetId(EXPRESSION* exp)
{
    EXPRESSION* x = GetBaseId(exp);
    if (x)
        return x->val;
    if (exp->type == e_int)
        return exp->val;
    return 100;
}
static void SetId(struct resRes* data, EXPRESSION* exp, int id, CONTROL* ctls)
{
    EXPRESSION* x = GetBaseId(exp);
    if (x)
    {
        SYM* sp = search(x->rendition, &currentResData->syms);
        if (!sp)
        {
            x->val = id;
            if (ctls && id < 100)
                x->val = ResNextCtlId(ctls);  // if it is a sys ctl id assign a new one...
                                              // otherwise reuse what was there...
            ResAddNewDef(x->rendition, x->val);
        }
    }
}
void PropSetIdName(struct resRes* data, char* buf, EXPRESSION** exp, CONTROL* ctls, BOOL changeTree)
{
    WCHAR name[512], *p = name;
    char xx[256], *q;
    int id = GetId(*exp);
    while (isspace(*buf))
        buf++;
    q = buf;
    if (!isalpha(*q) && *q != '_')
        return;
    while (*q)
    {
        if (!isalnum(*q) && (*q) != '_')
            return;
        q++;
    }
    ResGetHeap(workArea, data);
    ResSetDirty(data);
    if (changeTree)
        ResSetTreeName(data, buf);
    q = buf;
    while (*q)
        *p++ = *q++;
    *p = 0;
    *exp = ReadExpFromString(name);
    if (!*exp)
    {
        *exp = rcAlloc(sizeof(EXPRESSION));
        (*exp)->type = e_int;
        (*exp)->rendition = rcStrdup(buf);
        SetId(data, *exp, id, ctls);
    }
    else
    {
        sprintf(xx, "%d", (*exp)->val);
        if (!strcmp(xx, (*exp)->rendition))
            (*exp)->rendition = NULL;
        else
            SetId(data, *exp, id, ctls);
    }
}
void PropSetItem(HWND lv, int index, int group, char* label)
{
    LVITEMA_x item;
    memset(&item, 0, sizeof(LVITEMA_x));
    item.iItem = index;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_GROUPID | LVIF_COLUMNS | LVIF_PARAM;
    item.pszText = label;
    item.iGroupId = group;
    item.lParam = index;
    ListView_InsertItem(lv, &item);
    item.iSubItem = 1;
    item.mask = LVIF_GROUPID | LVIF_COLUMNS | LVIF_PARAM;
    ListView_SetItem(lv, &item);
}
void PropSetGroup(HWND lv, int group, WCHAR* label)
{
    LVGROUP lvG;
    ListView_RemoveGroup(lv, group);
    lvG.cbSize = sizeof(lvG);
    lvG.mask = LVGF_HEADER | LVGF_GROUPID;
    lvG.iGroupId = group;
    lvG.pszHeader = label;
    ListView_InsertGroup(lv, -1, &lvG);
}
void SetResourceProperties(void* data, struct propertyFuncs* funcs)
{
    currentPropertyFuncs = funcs;
    currentPropertyData = data;
    ListView_DeleteAllItems(lvwindow);
    if (funcs)
    {
        funcs->draw(lvwindow, data);
        ResizeListViewColumns();
    }
    if (GetWindowIndex(DID_PROPSWND) >= 0)
        SelectWindow(DID_PROPSWND);
}
static int CustomDraw(HWND hwnd, LPNMLVCUSTOMDRAW draw)
{
    switch (draw->nmcd.dwDrawStage)
    {
        POINT pt;
        HPEN oldPen;
        case CDDS_PREPAINT:
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
        case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
            // well you can't have groups and grids both, so we opt for groups
            // then draw the grids by hand...
            oldPen = SelectObject(draw->nmcd.hdc, CreatePen(PS_SOLID, 0, 0xe0e0e0));
            MoveToEx(draw->nmcd.hdc, draw->iSubItem == 0 ? 0 : draw->nmcd.rc.left, draw->nmcd.rc.bottom, &pt);
            LineTo(draw->nmcd.hdc, draw->nmcd.rc.right, draw->nmcd.rc.bottom);
            MoveToEx(draw->nmcd.hdc, draw->iSubItem == 0 ? 0 : draw->nmcd.rc.left, draw->nmcd.rc.top - 1, NULL);
            LineTo(draw->nmcd.hdc, draw->nmcd.rc.right, draw->nmcd.rc.top - 1);
            if (draw->iSubItem == 1)
            {
                MoveToEx(draw->nmcd.hdc, draw->nmcd.rc.left - 1, draw->nmcd.rc.top - 1, NULL);
                LineTo(draw->nmcd.hdc, draw->nmcd.rc.left - 1, draw->nmcd.rc.bottom);
            }
            MoveToEx(draw->nmcd.hdc, pt.x, pt.y, NULL);
            oldPen = SelectObject(draw->nmcd.hdc, oldPen);
            DeleteObject(oldPen);
            return CDRF_SKIPDEFAULT;
        default:
            return CDRF_DODEFAULT;
    }
}
void PropsWndRedraw(void)
{
    int n = ListView_GetItemCount(lvwindow);
    int i;
    for (i = 0; i < n; i++)
    {
        ListView_SetItemText(lvwindow, i, 1, LPSTR_TEXTCALLBACK);
    }
}
void PropsWndClearEditBox(MSG* msg)
{
    if (currentPropertyWindow && currentPropertyWindow != msg->hwnd && !IsChild(currentPropertyWindow, msg->hwnd))
    {
        char buf[256] = "";
        GetClassName(msg->hwnd, buf, sizeof(buf) - 2);
        if (stricmp(buf, "COMBOLBOX") != 0)
        {            
            SendMessage(hwndProps, WM_KILLPROPSEDITOR, 0, 0);
        }
        else
        {
            RECT wrect;
            GetWindowRect(msg->hwnd, &wrect);
            if (wrect.right - GetSystemMetrics(SM_CXVSCROLL) > msg->pt.x)
                SendMessage(hwndProps, WM_KILLPROPSEDITOR, 0, 0);
        }
    }
}
LRESULT CALLBACK PropsProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    NM_TREEVIEW* nm;
    RECT rs;
    LVHITTESTINFO_x hittest;
    switch (iMessage)
    {
        case WM_NOTIFY:
            nm = (NM_TREEVIEW*)lParam;
            switch (nm->hdr.code)
            {
                case LVN_GETDISPINFO:
                {
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_IMAGE;
                    switch (plvdi->item.iSubItem)
                    {
                        case 1:
                            if (currentPropertyFuncs)
                            {
                                char buf[256];
                                LV_ITEM item;
                                buf[0] = 0;
                                memset(&item, 0, sizeof(item));
                                item.iItem = plvdi->item.iItem;
                                item.mask = LVIF_PARAM;
                                ListView_GetItem(lvwindow, &item);
                                currentPropertyFuncs->getText(buf, lvwindow, currentPropertyData, item.lParam);
                                plvdi->item.pszText = buf;
                            }
                            else
                            {
                                plvdi->item.pszText = "";
                            }
                            break;
                        default:
                            break;
                    }
                }
                case NM_CUSTOMDRAW:
                    return CustomDraw(hwnd, (LPNMLVCUSTOMDRAW)lParam);
                case NM_CLICK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(lvwindow, &hittest.pt);
                    if (currentPropertyWindow)
                    {
                        PostMessage(hwnd, WM_KILLPROPSEDITOR, 0, 0);
                    }
                    else
                    {
                        ListView_SubItemHitTest(lvwindow, &hittest);
                        if (hittest.flags & LVHT_ONITEM)
                        {
                            if (hittest.iSubItem == 1)
                            {
                                RECT r;
                                LV_ITEM item;
                                memset(&item, 0, sizeof(item));
                                item.iItem = hittest.iItem;
                                item.mask = LVIF_PARAM;
                                ListView_GetItem(lvwindow, &item);
                                currentPropertyRow = item.lParam;
                                currentPropertyWindow =
                                    currentPropertyFuncs->startEdit(lvwindow, currentPropertyRow, currentPropertyData);
                                if (currentPropertyWindow)
                                {
                                    SendMessage(currentPropertyWindow, WM_SETFONT, SendMessage(lvwindow, WM_GETFONT, 0, 0), 0);
                                    ListView_GetSubItemRect(lvwindow, hittest.iItem, 1, LVIR_BOUNDS, &r);
                                    SetWindowPos(currentPropertyWindow, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top,
                                                 SWP_NOZORDER | SWP_SHOWWINDOW);
                                    SetFocus(currentPropertyWindow);
                                    // well it might not be an edit window in which case this should be ignored
                                    SendMessage(currentPropertyWindow, EM_SETSEL, 0, -1);
                                    // well it might not be a combo box in which case this should be ignored
                                    SendMessage(currentPropertyWindow, CB_SHOWDROPDOWN, 1, 0);
                                }
                            }
                        }
                    }
                    break;
                case NM_DBLCLK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(lvwindow, &hittest.pt);
                    return 0;
                case NM_RCLICK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(lvwindow, &hittest.pt);
                    break;
            }
            break;
        case WM_KILLPROPSEDITOR:
            if (currentPropertyFuncs)
            {
                currentPropertyFuncs->finishEdit(lvwindow, currentPropertyRow, currentPropertyWindow, currentPropertyData);
                currentPropertyWindow = NULL;
            }
            break;
        case WM_CREATE:
            GetClientRect(hwnd, &rs);
            lvwindow = CreateWindowEx(0, WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL, 0, 0, rs.right,
                                      rs.bottom, hwnd, (HMENU)ID_TREEVIEW, hInstance, NULL);
            SetListViewColumns(&rs);
            break;
        case WM_SIZE:
            MoveWindow(lvwindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 0);
            ResizeListViewColumns();
            break;
        case WM_DESTROY:
            //            ReleasePropInfo();
            DestroyWindow(lvwindow);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
void RegisterPropsWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &PropsProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szPropsClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreatePropsWindow(void)
{
    hwndProps = CreateInternalWindow(DID_PROPSWND, szPropsClassName, szPropsTitle);
    return hwndProps;
}
