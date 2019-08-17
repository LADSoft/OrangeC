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
#include <stdio.h>
#include <process.h>
#include <richedit.h>
#include "header.h"
#include "helpid.h"

extern HMENU hMenuMain;
extern HANDLE hInstance;
extern HWND hwndFrame, hwndClient;
extern char szWorkAreaName[];
extern PROJECTITEM* workArea;

typedef struct _tool
{
    struct _tool* next;
    char* cmd;
    char* wd;
    char* name;
    char* args;
    BOOL CommandWindow;
    BOOL enabled;
    BOOL removing;
    BOOL adding;

} TOOL;

static TOOL* tools;
static BOOL start;

DWORD RunExternalToolThread(void* p)
{
    int i;
    int id = (int)p;
    TOOL* curTools = tools;
    for (i = 0; i < id - ID_EXTERNALTOOLS && curTools; curTools = curTools->next, i++)
        ;
    if (curTools)
    {
        if (curTools->CommandWindow)
        {
            char buf[MAX_PATH];
            sprintf(buf, "%s\\hi", curTools->wd);
            DosWindow(buf, curTools->cmd, curTools->args, curTools->name, "Command Complete.");
        }
        else
        {
            DWORD bRet;
            char name[256];
            STARTUPINFO stStartInfo;
            PROCESS_INFORMATION stProcessInfo;
            DWORD retCode;
            char cmd[10000];
            strcpy(name, curTools->name);
            sprintf(cmd, "\"%s\" %s", curTools->cmd, curTools->args);
            memset(&stStartInfo, 0, sizeof(STARTUPINFO));
            memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

            stStartInfo.cb = sizeof(STARTUPINFO);
            bRet = CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, 0, curTools->wd, &stStartInfo, &stProcessInfo);
            if (!bRet)
            {
                ExtendedMessageBox("Command Execution", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Could not execute %s.", cmd);
                return 0;
            }
            WaitForSingleObject(stProcessInfo.hProcess, INFINITE);
            GetExitCodeProcess(stProcessInfo.hProcess, &retCode);
            if (retCode)
            {
                ExtendedMessageBox("Command Execution", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Command %s returned %d", name, retCode);
            }
            CloseHandle(stProcessInfo.hProcess);
            CloseHandle(stProcessInfo.hThread);
        }
    }
    return 0;
}
void RunExternalTool(int id) { _beginthread((BEGINTHREAD_FUNC)RunExternalToolThread, 0, (LPVOID)id); }
void ExternalToolsToMenu(void)
{
    TOOL* curTools = tools;
    int base, base1;
    MENUITEMINFO mi;
    int i, currentOffset;
    HMENU hMRUSubMenu;
    int maxed;
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maxed);
    base = ID_EXTERNALTOOLS;
    base1 = 3;  // menu index.  Must change if RC file changes

    hMRUSubMenu = GetSubMenu(hMenuMain, ToolsMenuItem + maxed);
    hMRUSubMenu = GetSubMenu(hMRUSubMenu, base1);
    currentOffset = GetMenuItemCount(hMRUSubMenu);
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA;
    mi.fType = MFT_STRING;

    for (i = 0; curTools; curTools = curTools->next)
    {
        if (curTools->enabled)
        {
            mi.wID = i + base;
            mi.dwTypeData = curTools->name;
            if (i >= currentOffset - 2)
                InsertMenuItem(hMRUSubMenu, i, TRUE, &mi);
            else
                SetMenuItemInfo(hMRUSubMenu, i, TRUE, &mi);
            i++;
        }
    }
    while (i < currentOffset - 2)
    {
        DeleteMenu(hMRUSubMenu, i, MF_BYPOSITION);
        currentOffset--;
    }
}
void ExternalToolsToProfile(void)
{
    TOOL* curTools = tools;
    int i;
    for (i = 0; curTools; curTools = curTools->next, i++)
    {
        char buf[256];
        sprintf(buf, "EXTOOL_NAME_%d", i);
        StringToProfile(buf, curTools->name);
        sprintf(buf, "EXTOOL_CMDLINE_%d", i);
        StringToProfile(buf, curTools->cmd);
        sprintf(buf, "EXTOOL_WD_%d", i);
        StringToProfile(buf, curTools->wd);
        sprintf(buf, "EXTOOL_ARGS_%d", i);
        StringToProfile(buf, curTools->args);
        sprintf(buf, "EXTOOL_CMDWND_%d", i);
        IntToProfile(buf, curTools->CommandWindow);
        sprintf(buf, "EXTOOL_ENABLED_%d", i);
        IntToProfile(buf, curTools->enabled);
    }
    IntToProfile("EXTOOL_COUNT", i);
}
void ProfileToExternalTools(void)
{
    TOOL *curTools = tools, **add = &tools;
    int i;
    int max = ProfileToInt("EXTOOL_COUNT", 0);
    while (curTools)
    {
        TOOL* next = curTools->next;
        free(curTools->name);
        free(curTools->cmd);
        free(curTools->wd);
        free(curTools->args);
        free(curTools);
        curTools = next;
    }
    for (i = 0; i < max; i++)
    {
        char buf[4096], dflt[256];
        TOOL* newTool = calloc(1, sizeof(TOOL));
        sprintf(buf, "EXTOOL_NAME_%d", i);
        sprintf(dflt, " Tool %d", i);
        newTool->name = strdup(ProfileToString(buf, dflt));
        sprintf(buf, "EXTOOL_CMDLINE_%d", i);
        dflt[0] = 0;
        newTool->cmd = strdup(ProfileToString(buf, dflt));
        sprintf(buf, "EXTOOL_WD_%d", i);
        dflt[0] = 0;
        newTool->wd = strdup(ProfileToString(buf, dflt));
        sprintf(buf, "EXTOOL_ARGS_%d", i);
        dflt[0] = 0;
        newTool->args = strdup(ProfileToString(buf, dflt));
        sprintf(buf, "EXTOOL_CMDWND_%d", i);
        newTool->CommandWindow = ProfileToInt(buf, 0);
        sprintf(buf, "EXTOOL_ENABLED_%d", i);
        newTool->enabled = ProfileToInt(buf, 1);
        *add = newTool;
        add = &newTool->next;
    }
}
LRESULT CALLBACK ExToolsEditProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static TOOL* item = NULL;
    char buf[4096];
    switch (iMessage)
    {
        case WM_INITDIALOG:
        {
            item = (TOOL*)lParam;
            SetDlgItemText(hwnd, IDC_EXT_NAME, item->name);
            SetDlgItemText(hwnd, IDC_EXT_COMMAND, item->cmd);
            SetDlgItemText(hwnd, IDC_EXT_ARG, item->args);
            SetDlgItemText(hwnd, IDC_EXT_WD, item->wd);
            CheckDlgButton(hwnd, IDC_EXT_CW, item->CommandWindow ? BST_CHECKED : BST_UNCHECKED);
            break;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDOK:
                    GetDlgItemText(hwnd, IDC_EXT_NAME, buf, sizeof(buf));
                    if (buf[0])
                    {
                        free(item->name);
                        item->name = strdup(buf);
                    }
                    GetDlgItemText(hwnd, IDC_EXT_COMMAND, buf, sizeof(buf));
                    free(item->cmd);
                    item->cmd = strdup(buf);
                    GetDlgItemText(hwnd, IDC_EXT_ARG, buf, sizeof(buf));
                    free(item->args);
                    item->args = strdup(buf);
                    GetDlgItemText(hwnd, IDC_EXT_WD, buf, sizeof(buf));
                    free(item->wd);
                    item->wd = strdup(buf);
                    item->CommandWindow = IsDlgButtonChecked(hwnd, IDC_EXT_CW) == BST_CHECKED;
                    EndDialog(hwnd, 0);
                    break;
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;
                case IDHELP:
                    ContextHelp(IDH_CUSTOM_TOOLS_EDITOR);
                    break;
            }
        }
        break;
    }
    return 0;
}
static int GetSelected(HWND hwnd)
{

    int n;
    int i;
    hwnd = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
    n = ListView_GetItemCount(hwnd);
    for (i = 0; i < n; i++)
    {
        int v = ListView_GetItemState(hwnd, i, LVIS_SELECTED);
        if (v)
            return i;
    }
    return 0;
}
static int CreateNewExTool(HWND hwnd)
{
    static int pos = 0;
    TOOL** current = &tools;
    TOOL* newTool = calloc(1, sizeof(TOOL));
    int count = 0;
    char buf[MAX_PATH];
    while (*current)
    {
        if (!(*current)->removing)
            count++;
        current = &(*current)->next;
    }
    if (count >= MAX_EXTERNALTOOLS)
    {
        free(newTool);
        return GetSelected(hwnd);
    }
    if (workArea && workArea->children)
    {
        char* p;
        strcpy(buf, szWorkAreaName);
        p = strrchr(buf, '\\');
        if (p)
            *p = 0;
        else
            GetCurrentDirectory(sizeof(buf), buf);
    }
    else
    {
        GetCurrentDirectory(sizeof(buf), buf);
    }

    newTool->adding = TRUE;
    newTool->cmd = strdup("");
    newTool->args = strdup("");
    newTool->wd = strdup(buf);
    sprintf(buf, "New Tool %d", pos++);
    newTool->name = strdup(buf);
    newTool->enabled = TRUE;
    newTool->CommandWindow = FALSE;
    *current = newTool;
    return count;
}
static int RemoveExTool(HWND hwnd)
{
    int n = GetSelected(hwnd), len;
    int i = 0;
    TOOL* curTool;
    hwnd = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
    len = ListView_GetItemCount(hwnd);
    for (curTool = tools; (i < n || curTool->removing) && curTool; curTool = curTool->next)
        if (!curTool->removing)
            i++;
    if (curTool)
        curTool->removing = TRUE;
    if (n >= len - 1)
        n--;
    return n;
}
static void Edit(HWND hwnd)
{
    int n = GetSelected(hwnd);
    int i = 0;
    TOOL* curTool;
    hwnd = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
    for (curTool = tools; (i < n || curTool->removing) && curTool; curTool = curTool->next)
        if (!curTool->removing)
            i++;
    if (curTool)
        DialogBoxParam(hInstance, "DLG_EXTOOLSEDIT", hwnd, (DLGPROC)ExToolsEditProc, (DWORD)curTool);
}
static int MoveExTool(HWND hwnd, BOOL up)
{
    int n = GetSelected(hwnd);
    TOOL **current, *curTool;
    int i = 0;
    for (current = &tools; *current && (i < n || (*current)->removing); current = &(*current)->next)
        if (!(*current)->removing)
            i++;
    curTool = *current;
    if (*current)
    {
        if (up)
        {
            if (n <= 0)
            {
                return n;
            }
            n--;
        }
        else
        {
            if (!(*current)->next)
                return n;
            n++;
        }
        *current = (*current)->next;
        i = 0;
        for (current = &tools; *current && i < n; current = &(*current)->next)
            if (!(*current)->removing)
                i++;
        curTool->next = *current;
        *current = curTool;
    }
    return n;
}
static void PopulateExToolsView(HWND hwnd, int sel, BOOL first)
{
    TOOL* curTools = tools;
    int items = 0;
    int i;
    LV_ITEM item;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);

    if (first)
    {
        LV_COLUMN lvC;
        ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

        GetWindowRect(hwndLV, &r);
        lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
        lvC.cx = 20;
        lvC.iSubItem = 0;
        ListView_InsertColumn(hwndLV, 0, &lvC);
        lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
        lvC.cx = 32;
        lvC.iSubItem = 1;
        ListView_InsertColumn(hwndLV, 1, &lvC);
        lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
        lvC.fmt = LVCFMT_LEFT;
        lvC.cx = r.right - r.left - 56;
        lvC.iSubItem = 2;
        ListView_InsertColumn(hwndLV, 2, &lvC);
    }
    else
    {
        ListView_DeleteAllItems(hwndLV);
    }

    start = TRUE;
    for (i = 0; curTools; curTools = curTools->next, i++)
    {
        if (!curTools->removing)
        {
            int v;
            item.iItem = items++;
            item.iSubItem = 0;
            item.mask = LVIF_PARAM;
            item.lParam = (LPARAM)i;
            item.pszText = "";  // LPSTR_TEXTCALLBACK ;
            v = ListView_InsertItem(hwndLV, &item);
            ListView_SetCheckState(hwndLV, v, curTools->enabled ? 1 : 0);
        }
    }
    start = FALSE;
    if (items)
    {
        ListView_SetSelectionMark(hwndLV, sel);
        ListView_SetItemState(hwndLV, sel, LVIS_SELECTED, LVIS_SELECTED);
    }
}
LRESULT CALLBACK ExToolsCustomizeProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static int oldCount;
    static TOOL** oldOrder;
    switch (iMessage)
    {
        case WM_NOTIFY:
            if (wParam == IDC_EXTOOLCUSTOM)
            {
                if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO)
                {
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                        TOOL* curTools;
                        int i;
                        case 2:
                            curTools = tools;
                            for (i = plvdi->item.iItem; curTools && (i || curTools->removing); curTools = curTools->next)
                                if (!curTools->removing)
                                    i--;
                            if (curTools)
                                plvdi->item.pszText = curTools->name;
                            break;
                        default:
                            plvdi->item.pszText = "";
                            break;
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
                {
                    if (!start)
                    {
                        LPNMLISTVIEW lp = (LPNMLISTVIEW)lParam;
                        TOOL* curTools = tools;
                        int i;
                        for (i = lp->iItem; curTools && (i || curTools->removing); curTools = curTools->next)
                            if (!curTools->removing)
                                i--;
                        if (curTools)
                            curTools->enabled = ListView_GetCheckState(GetDlgItem(hwnd, IDC_EXTOOLCUSTOM), lp->iItem) ? 1 : 0;
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
                {
                    switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                    {
                        case VK_INSERT:
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
                                ListView_SetCheckState(hwndLV, -1, TRUE);
                            }
                            else
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
                                int i = ListView_GetSelectionMark(hwndLV);
                                ListView_SetCheckState(hwndLV, i, TRUE);
                            }
                            break;
                        case VK_DELETE:
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
                                ListView_SetCheckState(hwndLV, -1, FALSE);
                            }
                            else
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
                                int i = ListView_GetSelectionMark(hwndLV);
                                ListView_SetCheckState(hwndLV, i, FALSE);
                            }
                            break;
                    }
                }
            }
            return 0;
        case WM_COMMAND:
            switch (wParam & 0xffff)
            {
                case IDC_EXTOOLSADD:
                    PopulateExToolsView(hwnd, CreateNewExTool(hwnd), FALSE);
                    EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                    EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSEDIT), TRUE);
                    EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSREMOVE), TRUE);
                    EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSMOVEUP), TRUE);
                    EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSMOVEDOWN), TRUE);
                    SendMessage(hwnd, WM_COMMAND, IDC_EXTOOLSEDIT, 0);
                    break;
                case IDC_EXTOOLSEDIT:
                    Edit(hwnd);
                    PopulateExToolsView(hwnd, GetSelected(hwnd), FALSE);
                    EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                    break;
                case IDC_EXTOOLSREMOVE:
                    PopulateExToolsView(hwnd, RemoveExTool(hwnd), FALSE);
                    EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                    if (!ListView_GetItemCount(GetDlgItem(hwnd, IDC_EXTOOLCUSTOM)))
                    {
                        EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSEDIT), FALSE);
                        EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSREMOVE), FALSE);
                        EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSMOVEUP), FALSE);
                        EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSMOVEDOWN), FALSE);
                    }
                    break;
                case IDC_EXTOOLSMOVEUP:
                    PopulateExToolsView(hwnd, MoveExTool(hwnd, TRUE), FALSE);
                    EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                    break;
                case IDC_EXTOOLSMOVEDOWN:
                    PopulateExToolsView(hwnd, MoveExTool(hwnd, FALSE), FALSE);
                    EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                    break;
                case IDOK:
                {
                    TOOL** curTools = &tools;
                    while (*curTools)
                    {
                        (*curTools)->adding = FALSE;
                        if ((*curTools)->removing)
                        {
                            TOOL* cur = *curTools;
                            TOOL* next = cur->next;
                            free(cur->args);
                            free(cur->cmd);
                            free(cur->name);
                            free(cur->wd);
                            free(cur);
                            *curTools = next;
                        }
                        else
                        {
                            curTools = &(*curTools)->next;
                        }
                    }
                    free(oldOrder);
                    ExternalToolsToMenu();
                    EndDialog(hwnd, IDOK);
                    break;
                }
                case IDCANCEL:
                {
                    int i;
                    TOOL** curTools = &tools;
                    while (*curTools)
                    {
                        (*curTools)->removing = FALSE;
                        if ((*curTools)->adding)
                        {
                            TOOL* cur = *curTools;
                            TOOL* next = cur->next;
                            free(cur->args);
                            free(cur->cmd);
                            free(cur->name);
                            free(cur->wd);
                            free(cur);
                            *curTools = next;
                        }
                        else
                        {
                            curTools = &(*curTools)->next;
                        }
                    }
                    curTools = &tools;
                    for (i = 0; i < oldCount; i++)
                    {
                        *curTools = oldOrder[i];
                        (*curTools)->next = NULL;
                        curTools = &((*curTools)->next);
                    }
                    EndDialog(hwnd, IDCANCEL);
                    break;
                }
                case IDHELP:
                    ContextHelp(IDH_CUSTOM_TOOLS_DIALOG);
                    break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
        case WM_INITDIALOG:
        {
            TOOL* curTools = tools;
            HWND hwndLV = GetDlgItem(hwnd, IDC_EXTOOLCUSTOM);
            oldCount = 0;
            for (oldCount = 0; curTools; curTools = curTools->next, oldCount++)
                ;
            if (oldCount)
            {
                oldOrder = calloc(oldCount, sizeof(TOOL*));
                oldCount = 0;
                for (curTools = tools; curTools; curTools = curTools->next, oldCount++)
                {
                    oldOrder[oldCount] = curTools;
                }
            }
            else
            {
                oldOrder = NULL;
                EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSEDIT), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSREMOVE), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSMOVEUP), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_EXTOOLSMOVEDOWN), FALSE);
            }
            CenterWindow(hwnd);

            PopulateExToolsView(hwnd, 0, TRUE);

            break;
        }
    }
    return 0;
}
void EditExternalTools(void) { DialogBox(hInstance, "DLG_EXTOOLS", hwndFrame, (DLGPROC)ExToolsCustomizeProc); }