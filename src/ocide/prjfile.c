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

#define N_EDITDONE -4001

extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern char szNewFileFilter[];
extern char szInstallPath[];
extern HWND prjTreeWindow;
extern HIMAGELIST treeIml;
extern HTREEITEM prjSelectedItem;

static int newMode;
static char newTitle[MAX_PATH];

static char* extensionMap[][2] = {
    ".c",   "Source Files",   ".cpp", "Source Files",   ".cxx", "Source Files",   ".cc",  "Source Files",
    ".asm", "Source Files",   ".nas", "Source Files",   ".s",   "Source Files",   ".def", "Source Files",
    ".h",   "Include Files",  ".hpp", "Include Files",  ".hxx", "Include Files",  ".p",   "Include Files",
    ".inc", "IncludeFiles",   ".rc",  "Resource Files", ".dlg", "Resource Files", ".bmp", "Resource Files",
    ".cur", "Resource Files", ".ico", "Resource Files", ".txt", "Text Files"};

PROJECTITEM* GetItemInfo(HTREEITEM item);
int VerifyPath(char* path)
{
    char err[MAX_PATH + 100];
    WIN32_FIND_DATA aa;
    HANDLE ff;
    ff = FindFirstFile(path, &aa);
    if (ff != INVALID_HANDLE_VALUE)
    {
        FindClose(ff);
        return 1;
    }
    else
    {
        // try to create it...
        FILE* fil = fopen(path, "wb");
        if (fil)
        {
            fclose(fil);
            unlink(path);
            return 1;
        }
    }
    sprintf(err, "Could not create file %s", path);
    ExtendedMessageBox("Invalid file", MB_OK, "Could not create file %s", path);
    return 0;
}
void SetExt(OPENFILENAME* ofn, char* ext)
{
    char* p = strrchr(ofn->lpstrFileTitle, '.');
    if (!p || stricmp(p, ext))
        strcat(ofn->lpstrFileTitle, p);
}

PROJECTITEM* HasFile(PROJECTITEM* data, char* name)
{
    PROJECTITEM* rv = NULL;
    data = data->children;
    while (data && !rv)
    {
        if (data->type == PJ_FOLDER || data->type == PJ_PROJ)
            rv = HasFile(data, name);
        else if (data->type == PJ_FILE)
        {
            if (!stricmp(data->realName, name))
                rv = data;
        }
        data = data->next;
    }
    return rv;
}
PROJECTITEM* AddFile(PROJECTITEM* data, char* name, BOOL automatic)
{
    PROJECTITEM* file;
    PROJECTITEM** ins;
    HTREEITEM pos = TVI_FIRST;
    char* p;
    if ((file = HasFile(data, name)))
        return file;
    p = strrchr(name, '\\');
    if (p)
        p++;
    else
        p = name;
    if (data->type == PJ_PROJ && automatic)
    {
        //        if (!strnicmp(szInstallPath, name, strlen(szInstallPath)))
        //        {
        //            return NULL;
        //        }
        //        else
        {
            int i;
            for (i = 0; i < sizeof(extensionMap) / sizeof(extensionMap[0]); i++)
            {
                if (strlen(extensionMap[i][0]) < strlen(name) &&
                    !stricmp(name + strlen(name) - strlen(extensionMap[i][0]), extensionMap[i][0]))
                {
                    data = CreateFolder(data, extensionMap[i][1], FALSE);
                    break;
                }
            }
        }
    }
    ins = &data->children;
    while (*ins && (*ins)->type == PJ_FOLDER)
    {
        pos = (*ins)->hTreeItem;
        ins = &(*ins)->next;
    }
    while (*ins && stricmp((*ins)->displayName, p) < 0)
    {
        pos = (*ins)->hTreeItem;
        ins = &(*ins)->next;
    }
    file = RetrieveInternalDepend(name);
    if (!file)
        file = calloc(1, sizeof(PROJECTITEM));
    if (file)
    {
        file->type = PJ_FILE;
        file->parent = data;
        strcpy(file->realName, name);
        strcpy(file->displayName, p);

        file->next = *ins;
        *ins = file;
        TVInsertItem(prjTreeWindow, data->hTreeItem, pos, file);
        ExpandParents(file);
        MarkChanged(file, FALSE);
        ResAddItem(file);
        CalculateFileAutoDepends(name);
    }
    return file;
}
static int ParseNewFileData(HWND hwnd)
{
    int rv = 1;
    GetWindowText(GetDlgItem(hwnd, IDC_FILENEWFILE), newTitle, sizeof(newTitle));

    if (newTitle[0] == 0)
    {
        ExtendedMessageBox("Add New File", 0, "Please specify a new file name");
        rv = 0;
    }
    else
    {
        LVITEM item;
        memset(&item, 0, sizeof(item));
        item.mask = LVIF_PARAM;
        item.iItem = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_LVNEWFILE));
        ListView_GetItem(GetDlgItem(hwnd, IDC_LVNEWFILE), &item);
        newMode = item.lParam;
    }
    return rv;
}
static int CreateNewFileData(HWND hwnd)
{
    int items = 0;
    LV_ITEM item;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_LVNEWPROJECT);
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    ListView_SetImageList(hwndLV, ImageList_Duplicate(treeIml), LVSIL_SMALL);

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = r.right - r.left;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);

    memset(&item, 0, sizeof(item));
    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iImage = IL_C;
    item.pszText = "C Program File";
    item.lParam = 0;
    ListView_InsertItem(hwndLV, &item);

    memset(&item, 0, sizeof(item));
    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iImage = IL_CPP;
    item.pszText = "C++ Program File";
    item.lParam = 1;
    ListView_InsertItem(hwndLV, &item);

    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iImage = IL_H;
    item.pszText = "Header File";
    item.lParam = 8;
    ListView_InsertItem(hwndLV, &item);

    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iImage = IL_RES;
    item.pszText = "Resource File";
    item.lParam = 13;
    ListView_InsertItem(hwndLV, &item);

    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iImage = IL_FILES;
    item.pszText = "Module Definition File";
    item.lParam = 7;
    ListView_InsertItem(hwndLV, &item);

    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iImage = IL_ASM;
    item.pszText = "Assembly Language File";
    item.lParam = 5;
    ListView_InsertItem(hwndLV, &item);

    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    item.iImage = IL_FILES;
    item.pszText = "Text File";
    item.lParam = 18;
    ListView_InsertItem(hwndLV, &item);

    ListView_SetSelectionMark(hwndLV, 0);
    ListView_SetItemState(hwndLV, 0, LVIS_SELECTED, LVIS_SELECTED);

    SendDlgItemMessage(hwnd, IDC_FILENEWFILE, WM_SETTEXT, 0, (LPARAM) "");
    SendDlgItemMessage(hwnd, IDC_FILENEWFILE, EM_SETLIMITTEXT, MAX_PATH, 0);
    return items;
}
static int CustomDrawNewFile(HWND hwnd, LPNMLVCUSTOMDRAW draw)
{
    HWND hwndLV;
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            hwndLV = GetDlgItem(hwnd, IDC_LVNEWPROJECT);
            // we do it this way so the selection won't go away when we focus another control
            if (ListView_GetItemState(hwndLV, draw->nmcd.dwItemSpec, LVIS_SELECTED) & LVIS_SELECTED)
            {
                draw->clrText = RetrieveSysColor(COLOR_HIGHLIGHTTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_HIGHLIGHT);
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
//-------------------------------------------------------------------------

long APIENTRY NewFileProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            if (!CreateNewFileData(hwnd))
            {
                EndDialog(hwnd, 1);
            }
            else
            {
                CenterWindow(hwnd);
            }
            return 1;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                SetWindowLong(hwnd, DWL_MSGRESULT, CustomDrawNewFile(hwnd, (LPNMLVCUSTOMDRAW)lParam));
                return TRUE;
            }
            else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
            {
                switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                {
                    case VK_INSERT:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_LVNEWFILE);
                            ListView_SetSelectionMark(hwndLV, -1);
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_LVNEWFILE);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetSelectionMark(hwndLV, i);
                            ListView_SetItemState(hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
                        }
                        break;
                }
            }
            return 0;
        case WM_COMMAND:
            switch (wParam & 0xffff)
            {
                case IDOK:
                    if (ParseNewFileData(hwnd))
                        EndDialog(hwnd, IDOK);
                    break;
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    break;
                case IDHELP:
                    ContextHelp(IDH_NEW_FILE_DIALOG);
                    break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
    }
    return 0;
}
void ProjectNewFile(void)
{
    PROJECTITEM* data = GetItemInfo(prjSelectedItem);
    if (data && (data->type == PJ_PROJ || data->type == PJ_FOLDER))
    {
        if (DialogBox(hInstance, "IDD_NEWFILE", hwndFrame, (DLGPROC)NewFileProc) == IDOK)
        {
            char* p;
            char buf[MAX_PATH];
            PROJECTITEM* pj = data;
            BOOL writeFile = FALSE;
            FILE* fil;
            while (pj->type == PJ_FOLDER)
                pj = pj->parent;
            strcpy(buf, pj->realName);
            p = strrchr(buf, '\\');
            if (!p)
                p = buf;
            else
                p++;
            strcpy(p, newTitle);

            if (xstricmp(p + strlen(p) - strlen(extensionMap[newMode][0]), extensionMap[newMode][0]))
                strcat(p, extensionMap[newMode][0]);
            fil = fopen(buf, "rb");
            if (fil)
            {
                if (ExtendedMessageBox("Add New File", MB_YESNO, "File exists.  Overwrite?") == IDYES)
                    writeFile = TRUE;
            }
            else
            {
                writeFile = VerifyPath(buf);
            }
            if (writeFile)
            {
                if (!stricmp(p + strlen(p) - 3, ".rc"))
                {
                    CreateNewResourceFile(data, buf, TRUE);
                }
                else
                {
                    fil = fopen(buf, "wb");
                    if (fil)
                        fclose(fil);
                    AddFile(data, buf, TRUE);
                }
            }
        }
    }
}
void ProjectExistingFile(void)
{
    PROJECTITEM* data = GetItemInfo(prjSelectedItem);
    if (data && (data->type == PJ_PROJ || data->type == PJ_FOLDER))
    {
        OPENFILENAME ofn;
        if (OpenFileDialog(&ofn, data->realName, GetWindowHandle(DID_PROJWND), FALSE, TRUE, szNewFileFilter, "Open existing file"))
        {
            char* path = ofn.lpstrFile;
            if (path[strlen(path) + 1])  // multiselect
            {
                char buf[MAX_PATH];
                char* q = path + strlen(path) + 1;
                while (*q)
                {
                    sprintf(buf, "%s\\%s", path, q);
                    AddFile(data, buf, TRUE);
                    q += strlen(q) + 1;
                }
            }
            else
            {
                AddFile(data, path, TRUE);
            }
        }
    }
}
