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

#include "header.h"
#include "xml.h"
#include "helpid.h"

extern char szInstallPath[];
extern char szRuleFilter[];
extern HWND hwndFrame;
extern HINSTANCE hInstance;

BUILDRULE* buildRules;

void LoadDefaultRules(void)
{
    BUILDRULE** next = &buildRules;
    WIN32_FIND_DATA data;
    HANDLE hndl;
    char name[MAX_PATH];
    strcpy(name, szInstallPath);
    strcat(name, "\\rule\\*.rul");
    hndl = FindFirstFile(name, &data);
    if (hndl != INVALID_HANDLE_VALUE)
    {
        do
        {
            BUILDRULE* p = calloc(1, sizeof(BUILDRULE));
            strcpy(p->name, szInstallPath);
            strcat(p->name, "\\rule\\");
            strcat(p->name, data.cFileName);
            p->active = TRUE;
            p->defaultRule = TRUE;
            p->profiles = LoadRule(p->name);
            if (!p->profiles)
            {
                ExtendedMessageBox("Missing Rule File", 0, "Rule file %s cannot be loaded", p->name);
                p->active = FALSE;
            }
            *next = p;
            next = &p->next;
        } while (FindNextFile(hndl, &data));
        FindClose(hndl);
    }
}
void RestoreBuildRules(struct xmlNode* node, int version)
{
    node = node->children;
    if (!node)
        LoadDefaultRules();  // Safety
    else
        while (node)
        {
            if (IsNode(node, "FILE"))
            {
                struct xmlAttr* attribs = node->attribs;
                BUILDRULE* p = calloc(1, sizeof(BUILDRULE));
                while (attribs)
                {
                    if (IsAttrib(attribs, "NAME"))
                    {
                        strcpy(p->name, attribs->value);
                    }
                    if (IsAttrib(attribs, "ACTIVE"))
                    {
                        p->active = !!atoi(attribs->value);
                    }
                    if (IsAttrib(attribs, "DEFAULT"))
                    {
                        p->defaultRule = !!atoi(attribs->value);
                    }
                    attribs = attribs->next;
                }
                p->profiles = LoadRule(p->name);
                if (!p->profiles)
                {
                    ExtendedMessageBox("Missing Rule File", 0, "Rule file %s cannot be loaded", p->name);
                    p->active = FALSE;
                }
                p->next = buildRules;
                buildRules = p;
            }
            node = node->next;
        }
}
void SaveBuildRules(FILE* out)
{
    BUILDRULE* p = buildRules;
    while (p)
    {
        fprintf(out, "\t\t<FILE NAME=\"%s\" ACTIVE=\"%d\" DEFAULT=\"%d\"/>\n", p->name, p->active, p->defaultRule);
        p = p->next;
    }
}
static int CreateBuildRuleData(HWND hwnd)
{
    int items = 0;
    LV_ITEM item;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_BRLISTBOX);
    LV_COLUMN lvC;
    BUILDRULE* br;
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    ListView_DeleteAllItems(hwndLV);

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

    br = buildRules;
    while (br)
    {
        if (!br->remove)
        {
            int v;
            item.iItem = items++;
            item.iSubItem = 0;
            item.mask = LVIF_PARAM;
            item.lParam = (LPARAM)br;
            item.pszText = "";  // LPSTR_TEXTCALLBACK ;
            v = ListView_InsertItem(hwndLV, &item);
            ListView_SetCheckState(hwndLV, v, br->active);
        }
        br = br->next;
    }
    if (items)
    {
        ListView_SetSelectionMark(hwndLV, 0);
        ListView_SetItemState(hwndLV, 0, LVIS_SELECTED, LVIS_SELECTED);
    }
    return items;
}
static void ParseBuildRuleData(HWND hwnd)
{
    LV_ITEM item;
    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
    int i;
    BUILDRULE** br = &buildRules;
    for (i = 0; *br;)
    {
        if ((*br)->remove)
        {
            BUILDRULE* p = *br;
            *br = (*br)->next;
            free(p);
        }
        else
        {
            BUILDRULE* xx = (BUILDRULE*)item.lParam;  // item is uninitialized, is it a good idea to use it here?
            (*br)->add = FALSE;
            item.iItem = i;
            item.iSubItem = 0;
            item.mask = LVIF_PARAM;
            if (!ListView_GetItem(hwndLV, &item))
            {
                (xx)->active = ListView_GetCheckState(hwndLV, i);
                if (xx->active && !xx->profiles)
                {
                    xx->profiles = LoadRule(xx->name);
                    if (!xx->profiles)
                    {
                        ExtendedMessageBox("Missing Rule File", 0, "Rule file %s cannot be loaded", xx->name);
                        xx->active = FALSE;
                    }
                }
            }
            br = &(*br)->next;
            i++;
        }
    }
}
static void ResetBuildRuleData()
{
    BUILDRULE** br = &buildRules;
    while (*br)
    {
        if ((*br)->add)
        {
            BUILDRULE* p = *br;
            *br = (*br)->next;
            free(p);
        }
        else
        {
            (*br)->remove = FALSE;
            br = &(*br)->next;
        }
    }
}
static void AddOneRule(HWND hwnd, char* name)
{
    BUILDRULE* br = calloc(1, sizeof(BUILDRULE));
    strcpy(br->name, name);
    br->active = TRUE;
    br->add = TRUE;
    br->profiles = LoadRule(br->name);
    if (!br->profiles)
    {
        ExtendedMessageBox("Missing Rule File", 0, "Rule file %s cannot be loaded", br->name);
        br->active = FALSE;
    }
    br->next = buildRules;
    buildRules = br;
    CreateBuildRuleData(hwnd);
}
static void AddRule(HWND hwnd)
{
    OPENFILENAME ofn;
    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
    int i = ListView_GetItemCount(hwndLV);
    if (OpenFileDialog(&ofn, "", hwnd, FALSE, TRUE, szRuleFilter, "Open Rule File"))
    {
        char *q = ofn.lpstrFile, path[MAX_PATH];
        strcpy(path, ofn.lpstrFile);
        q += strlen(q) + 1;
        if (!*q)
        {
            AddOneRule(hwnd, path);
        }
        else
        {
            while (*q)
            {
                char thisPath[MAX_PATH];
                sprintf(thisPath, "%s\\%s", path, q);
                AddOneRule(hwnd, thisPath);
                q += strlen(q) + 1;
            }
        }
        ListView_SetSelectionMark(hwndLV, i);
        ListView_SetItemState(hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
    }
}
static void RemoveRule(HWND hwnd)
{
    HWND hwndLV = GetDlgItem(hwnd, IDC_BRLISTBOX);
    int i = ListView_GetSelectionMark(hwndLV);
    if (i != -1)
    {
        LV_ITEM item;
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        if (ListView_GetItem(hwndLV, &item))
        {
            BUILDRULE* br = (BUILDRULE*)item.lParam;
            br->remove = TRUE;
            CreateBuildRuleData(hwnd);
            ListView_SetSelectionMark(hwndLV, i);
            ListView_SetItemState(hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}
static void EditRule(HWND hwnd) {}
static int CustomDraw(HWND hwnd, LPNMLVCUSTOMDRAW draw)
{
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            if (draw->nmcd.uItemState & (CDIS_SELECTED))
            {
                draw->clrText = RetrieveSysColor(COLOR_HIGHLIGHTTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_HIGHLIGHT);
            }
            else
            {
                BUILDRULE* p = (BUILDRULE*)draw->nmcd.lItemlParam;
                if (p->defaultRule)
                    draw->clrText = RGB(0, 0xee, 0xff);
                else
                    draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
            }
            return CDRF_NEWFONT;
        default:
            return CDRF_DODEFAULT;
    }
}
static int FAR PASCAL brDlgProc(HWND hwnd, UINT wmsg, WPARAM wParam, LPARAM lParam)

{
    switch (wmsg)
    {
        case WM_INITDIALOG:
        {
            HWND hwndLV = GetDlgItem(hwnd, IDC_BRLISTBOX);
            CenterWindow(hwnd);
            CreateBuildRuleData(hwnd);
            EnableWindow(GetDlgItem(hwnd, IDC_BUILDRULEADD), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_BUILDRULEREMOVE), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_BUILDRULEEDIT), FALSE);
        }
        break;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                SetWindowLong(hwnd, DWL_MSGRESULT, CustomDraw(hwnd, (LPNMLVCUSTOMDRAW)lParam));
                return TRUE;
            }
            if (wParam == IDC_BRLISTBOX)
            {
                if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO)
                {
                    BUILDRULE* br = buildRules;
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                        case 2:
                            br = (BUILDRULE*)plvdi->item.lParam;
                            plvdi->item.pszText = br->profiles->debugSettings->displayName;
                            break;
                        default:
                            plvdi->item.pszText = "";
                            break;
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
                {
                    LPNMLISTVIEW p = (LPNMLISTVIEW)lParam;
                    if (p->uChanged & LVIF_STATE)
                    {
                        if (p->uNewState & LVIS_SELECTED)
                        {
                            BUILDRULE* br = (BUILDRULE*)p->lParam;
                            EnableWindow(GetDlgItem(hwnd, IDC_BUILDRULEREMOVE), !br->defaultRule);
                        }
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
                {
                    switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                    {
                        case VK_INSERT:
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_BRLISTBOX);
                                ListView_SetCheckState(hwndLV, -1, TRUE);
                            }
                            else
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_BRLISTBOX);
                                int i = ListView_GetSelectionMark(hwndLV);
                                ListView_SetCheckState(hwndLV, i, TRUE);
                            }
                            break;
                        case VK_DELETE:
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_BRLISTBOX);
                                ListView_SetCheckState(hwndLV, -1, FALSE);
                            }
                            else
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_BRLISTBOX);
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
                case IDOK:
                    ParseBuildRuleData(hwnd);
                    SavePreferences();
                    EndDialog(hwnd, 1);
                    break;
                case IDCANCEL:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDHELP:
                    ContextHelp(IDH_BUILD_RULE_DIALOG);
                    break;
                case IDC_BUILDRULEADD:
                    AddRule(hwnd);
                    break;
                case IDC_BUILDRULEREMOVE:
                    RemoveRule(hwnd);
                    break;
                case IDC_BUILDRULEEDIT:
                    EditRule(hwnd);
                    break;
            }
            break;
        case WM_CLOSE:
            ResetBuildRuleData();
            EndDialog(hwnd, 0);
            break;
    }
    return 0;
}
void BuildRulesCustomize(void) { DialogBox(hInstance, "DLG_BUILDRULE", hwndFrame, (DLGPROC)&brDlgProc); }
