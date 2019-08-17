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

#include "helpid.h"
#include "header.h"
#include <ctype.h>
#include <stdlib.h>

extern PROJECTITEM* activeProject;
extern HWND hwndFrame;
extern HINSTANCE hInstance;
extern PROJECTITEM* workArea;

static BOOL hasDepend(PROJECTITEM* parent, PROJECTITEM* depend)
{
    PROJECTITEMLIST* list = parent->depends;
    while (list)
    {
        if (depend == list->item)
            return TRUE;
        list = list->next;
    }
    return FALSE;
}
static BOOL CircularDepends(PROJECTITEM* parent, PROJECTITEM* depend)
{
    PROJECTITEMLIST* list = depend->depends;
    while (list)
    {
        if (parent == list->item)
            return TRUE;
        if (CircularDepends(parent, list->item))
            return TRUE;
        list = list->next;
    }
    return FALSE;
}
static void PopulateProject(HWND hwnd, PROJECTITEM* proj)
{
    PROJECTITEM* temp;
    HWND hwndLV = GetDlgItem(hwnd, IDC_DEPENDS);
    LV_COLUMN lvC;
    RECT r;
    int rows = 0;
    ListView_DeleteAllItems(hwndLV);
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    ListView_DeleteColumn(hwndLV, 1);
    ListView_DeleteColumn(hwndLV, 0);
    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = 20;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = r.right - r.left - 24;
    lvC.iSubItem = 1;
    ListView_InsertColumn(hwndLV, 1, &lvC);
    temp = workArea->children;
    while (temp)
    {
        if (temp != proj)
        {
            int v;
            LV_ITEM item;
            memset(&item, 0, sizeof(item));
            item.iItem = rows++;
            item.iSubItem = 0;
            item.mask = LVIF_PARAM;
            item.lParam = (LPARAM)temp;
            item.pszText = "";  // LPSTR_TEXTCALLBACK ;
            v = ListView_InsertItem(hwndLV, &item);
            if (hasDepend(proj, temp))
            {
                ListView_SetCheckState(hwndLV, v, TRUE);
            }
        }
        temp = temp->next;
    }
}
static int CustomDraw(HWND hwnd, BOOLEAN showingBuildOrder, PROJECTITEM* proj, LPNMLVCUSTOMDRAW draw)
{
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            if (!showingBuildOrder && CircularDepends(proj, (PROJECTITEM*)draw->nmcd.lItemlParam))
            {
                draw->clrText = 0x888888;
                draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
            }
            else
            {
                draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
            }
            return CDRF_NEWFONT;
        default:
            return CDRF_DODEFAULT;
    }
}
static void ModifyProjectDepends(HWND hwnd, BOOL showingBuildInfo, PROJECTITEM* proj, LV_DISPINFO* plvdi)
{
    if (plvdi->hdr.idFrom == IDC_DEPENDS)
    {
        NMITEMACTIVATE* nmia;
        LVHITTESTINFO lvhtti;
        int row;
        switch (plvdi->hdr.code)
        {
            case LVN_GETDISPINFO:
                plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                plvdi->item.mask &= ~LVIF_STATE;
                switch (plvdi->item.iSubItem)
                {
                    PROJECTITEM* temp;
                    case 1:  // for dependencies
                        temp = (PROJECTITEM*)plvdi->item.lParam;
                        plvdi->item.pszText = temp->displayName;
                        break;
                    default:
                        plvdi->item.pszText = "";
                        break;
                }
                break;
            case NM_CLICK:
            case NM_DBLCLK:
                nmia = (NMITEMACTIVATE*)plvdi;
                memset(&lvhtti, 0, sizeof(lvhtti));
                lvhtti.pt = nmia->ptAction;
                row = ListView_SubItemHitTest(GetDlgItem(hwnd, IDC_DEPENDS), &lvhtti);
                if (row != -1)
                {
                    LV_ITEM item;
                    PROJECTITEMLIST** list = &proj->depends;
                    PROJECTITEM* depend;
                    memset(&item, 0, sizeof(item));
                    item.iItem = row;
                    item.iSubItem = 0;
                    item.mask = LVIF_PARAM;
                    ListView_GetItem(GetDlgItem(hwnd, IDC_DEPENDS), &item);
                    depend = (PROJECTITEM*)item.lParam;
                    if (CircularDepends(proj, depend))
                    {
                        ListView_SetCheckState(GetDlgItem(hwnd, IDC_DEPENDS), row,
                                               !ListView_GetCheckState(GetDlgItem(hwnd, IDC_DEPENDS), row));
                    }
                    else
                    {
                        MarkChanged(workArea, TRUE);
                        if (!ListView_GetCheckState(GetDlgItem(hwnd, IDC_DEPENDS), row))
                        {
                            // add
                            while (*list)
                                list = &(*list)->next;
                            (*list) = (PROJECTITEMLIST*)calloc(1, sizeof(PROJECTITEMLIST));
                            (*list)->item = depend;
                        }
                        else
                        {
                            // remove
                            while (*list)
                            {
                                if ((*list)->item == depend)
                                {
                                    PROJECTITEMLIST* found = (*list);
                                    (*list) = found->next;
                                    free(found);
                                    break;
                                }
                                list = &(*list)->next;
                            }
                        }
                    }
                }
                break;
        }
    }
}
static void PopulateBuildOrder(HWND hwnd)
{
    HWND hwndLV = GetDlgItem(hwnd, IDC_DEPENDS);
    LV_COLUMN lvC;
    RECT r;
    PROJECTITEM* temp;
    int rows = 0;

    CreateProjectDependenciesList();

    ListView_DeleteAllItems(hwndLV);
    ListView_DeleteColumn(hwndLV, 1);
    ListView_DeleteColumn(hwndLV, 0);
    ListView_SetExtendedListViewStyle(hwndLV, 0);

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = r.right - r.left - 10;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);

    temp = workArea->projectBuildList;
    while (temp)
    {
        LV_ITEM item;
        memset(&item, 0, sizeof(item));
        item.iItem = rows++;
        item.iSubItem = 0;
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.lParam = (LPARAM)temp;
        item.pszText = temp->displayName;
        ListView_InsertItem(hwndLV, &item);
        temp = temp->projectBuildList;
    }
}
static void SelectProject(HWND hwnd, PROJECTITEM* proj)
{
    int id = -1;
    while ((id = SendDlgItemMessage(hwnd, IDC_SELECTPROJECT, CB_SELECTSTRING, id + 1, (LPARAM)proj->displayName)) != CB_ERR)
    {
        char buf[256];
        buf[SendDlgItemMessage(hwnd, IDC_SELECTPROJECT, CB_GETLBTEXT, id, (LPARAM)buf)] = 0;
        if (!strcmp(buf, proj->displayName))
            break;
    }
}
LRESULT CALLBACK ProjectDependsProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

    static PROJECTITEM* proj;
    static BOOLEAN showingBuildOrder;
    PROJECTITEM* temp;
    int id;
    switch (iMessage)
    {
        case WM_INITDIALOG:
            // IDC_DEPENDENCIESLABEL, IDC_DEPENDS, IDC_SELECTPROJECT
            proj = (PROJECTITEM*)lParam;
            // go through project list and add to combo
            temp = workArea->children;
            while (temp)
            {
                id = SendDlgItemMessage(hwnd, IDC_SELECTPROJECT, CB_ADDSTRING, 0, (LPARAM)temp->displayName);
                SendDlgItemMessage(hwnd, IDC_SELECTPROJECT, CB_SETITEMDATA, id, (LPARAM)temp);
                temp = temp->next;
            }
            SelectProject(hwnd, proj);
            PopulateProject(hwnd, proj);
            CenterWindow(hwnd);
            return TRUE;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                SetWindowLong(hwnd, DWL_MSGRESULT, CustomDraw(hwnd, showingBuildOrder, proj, (LPNMLVCUSTOMDRAW)lParam));
                return TRUE;
            }
            else
            {
                ModifyProjectDepends(hwnd, showingBuildOrder, proj, (LV_DISPINFO*)lParam);
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_SELECTPROJECT:
                    if (HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        id = SendDlgItemMessage(hwnd, IDC_SELECTPROJECT, CB_GETCURSEL, 0, 0);
                        proj = (PROJECTITEM*)SendDlgItemMessage(hwnd, IDC_SELECTPROJECT, CB_GETITEMDATA, id, 0);
                        PopulateProject(hwnd, proj);
                    }
                    break;
                case IDOK:
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    showingBuildOrder = FALSE;
                    break;
                case IDHELP:
                    ContextHelp(IDH_PROJECT_DEPENDENCIES_DIALOG);
                    break;
                case IDC_BUILDORDER:
                    showingBuildOrder = !showingBuildOrder;
                    if (showingBuildOrder)
                    {
                        SendDlgItemMessage(hwnd, IDC_SELECTPROJECT, CB_SETCURSEL, -1, 0);
                        PopulateBuildOrder(hwnd);
                        SetWindowText(GetDlgItem(hwnd, IDC_DEPENDENCIESLABEL), "Calculated Build Order:");
                        SetWindowText(GetDlgItem(hwnd, IDC_BUILDORDER), "View Dependencies");
                        EnableWindow(GetDlgItem(hwnd, IDC_SELECTPROJECT), FALSE);
                    }
                    else
                    {
                        SelectProject(hwnd, proj);
                        PopulateProject(hwnd, proj);
                        SetWindowText(GetDlgItem(hwnd, IDC_DEPENDENCIESLABEL), "Select Dependencies: ");
                        SetWindowText(GetDlgItem(hwnd, IDC_BUILDORDER), "View Build Order");
                        EnableWindow(GetDlgItem(hwnd, IDC_SELECTPROJECT), TRUE);
                    }
                    break;
            }
            break;
    }
    return 0;
}
static PROJECTITEM* NextChild(PROJECTITEM* proj)
{
    PROJECTITEMLIST* list = proj->depends;
    if (!proj->visited)
    {
        while (list)
        {
            if (!list->item->visited)
            {
                PROJECTITEM* rv = NextChild(list->item);
                if (rv)
                {
                    return rv;
                }
            }
            list = list->next;
        }
        proj->visited = TRUE;
        return proj;
    }
    return NULL;
}
static PROJECTITEM* ExtractLeaf(void)
{
    PROJECTITEM* list = workArea->children;
    while (list)
    {
        if (!list->visited)
        {
            PROJECTITEM* rv = NextChild(list);
            if (rv)
            {
                return rv;
            }
            else
            {
                list->visited = TRUE;
                return list;
            }
        }
        list = list->next;
    }
    return NULL;
}
void CreateProjectDependenciesList(void)
{
    PROJECTITEM* temp = workArea;
    if (temp && temp->children)
    {
        PROJECTITEM* list = workArea;
        temp->depends = NULL;
        temp = temp->children;
        while (temp)
        {
            temp->projectBuildList = NULL;
            temp->visited = FALSE;
            temp = temp->next;
        }
        while ((temp = ExtractLeaf()))
        {
            list = list->projectBuildList = temp;
        }
    }
}
void EditProjectDependencies(PROJECTITEM* pj)
{
    if (!pj || pj->type == PJ_WS)
    {
        pj = activeProject;
    }
    DialogBoxParam(hInstance, "IDD_PROJDEPENDS", hwndFrame, (DLGPROC)ProjectDependsProc, (LPARAM)pj);
}
