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
#include "sqlite3.h"

#define N_EDITDONE -4001

extern HINSTANCE hInstance;
extern HANDLE ewSem;
extern HWND hwndClient, hwndFrame;
extern char szWorkAreaFilter[];
extern PROJECTITEM* activeProject;
extern CRITICAL_SECTION projectMutex;
extern HWND prjTreeWindow;
extern DWINFO* editWindows;

char szWorkAreaName[MAX_PATH] = "Default.cwa";
char szWorkAreaTitle[256];
int defaultWorkArea = TRUE;
PROJECTITEM* workArea;

static int newMode;
static char newName[MAX_PATH];
static char newTitle[MAX_PATH];
static char browsePath[MAX_PATH];

void SetExt(OPENFILENAME* ofn, char* ext);
void ccDBVacuum(sqlite3* db);
void ccDBSetDB(sqlite3* xdb);
sqlite3* ccDBOpen(PROJECTITEM* pj);

//-------------------------------------------------------------------------
void GetDefaultWorkAreaName(char* buf)
{
    GetDefaultProjectsPath(buf);
    //	GetUserDataPath(buf);
    strcat(buf, "\\default.cwa");
}
//-------------------------------------------------------------------------

void SetWorkAreaMRU(PROJECTITEM* workArea)
{
    DWINFO info;
    char buf[MAX_PATH];
    GetDefaultWorkAreaName(buf);
    if (stricmp(buf, workArea->realName))
    {
        strcpy(info.dwName, workArea->realName);
        strcpy(info.dwTitle, workArea->displayName);
        InsertMRU(&info, 1);
        MRUToMenu(1);
    }
}
static DWORD SetColorize(void* v)
{
    DWINFO* ptr;
    Sleep(1000);
    ptr = editWindows;
    while (ptr)
    {
        BOOL ccExistsInDB(char* fileName);
        if (ptr->active)
        {
            if (ccExistsInDB(ptr->dwName))
            {
                if (ptr->active)
                {
                    SendMessage(ptr->dwHandle, EM_LOADLINEDATA, 0, 0);
                }
            }
            else
            {
                if (ptr->active)
                    InstallForParse(ptr->self);
            }
        }
        ptr = ptr->next;
    }
}
void LoadWorkArea(char* name, BOOL existing)
{
    PROJECTITEM* oldWorkArea;
    sqlite3* db;
    if (workArea)
    {
        SaveAllProjects(workArea, TRUE);
    }
    EnterCriticalSection(&projectMutex);
    CloseAll();
    RemoveAllParse();
    ResetInternalAutoDepends();
    TreeView_DeleteAllItems(prjTreeWindow);
    ResDeleteAllItems();
    oldWorkArea = workArea;
    workArea = NULL;
    activeProject = NULL;
    defaultWorkArea = !!stristr(name, "default.cwa");
    workArea = RestoreWorkArea(name);
    if (workArea)
    {
        PROJECTITEM* p = workArea->children;
        while (p)
        {
            RestoreProject(p, FALSE);
            p = p->next;
        }
        if (activeProject && !activeProject->loaded)
            activeProject = NULL;
        if (!activeProject)
        {
            activeProject = workArea->children;
            while (activeProject && !activeProject->loaded)
                activeProject = activeProject->next;
            MarkChanged(activeProject, TRUE);
        }
        RecursiveCreateTree(TVI_ROOT, TVI_FIRST, workArea);
    }
    else if (!existing)
    {
        PROJECTITEM* newWorkArea = calloc(1, sizeof(PROJECTITEM));
        if (newWorkArea)
        {
            char* p;
            newWorkArea->type = PJ_WS;
            strcpy(newWorkArea->realName, name);
            p = strrchr(newWorkArea->realName, '\\');
            if (p)
                p++;
            else
                p = newWorkArea->realName;
            sprintf(newWorkArea->displayName, "WorkArea: %s", p);
            workArea = newWorkArea;
            TVInsertItem(prjTreeWindow, TVI_ROOT, TVI_FIRST, workArea);
            ResAddItem(workArea);
        }
    }
    else
    {
        workArea = oldWorkArea;
        oldWorkArea = NULL;
        if (workArea)
        {
            CalculateProjectDepends(workArea);
            SetStatusMessage("", FALSE);
            RecursiveCreateTree(TVI_ROOT, TVI_FIRST, workArea);
        }
    }
    LeaveCriticalSection(&projectMutex);
    if (oldWorkArea)
        FreeSubTree(oldWorkArea, TRUE);
    FreeJumpSymbols();
    LoadJumpSymbols();
    db = ccDBOpen(workArea);
    ccDBVacuum(db);
    ccDBSetDB(db);
    _beginthread(SetColorize, 0, 0);  // undefined in local context
}
void CloseWorkArea(void)
{
    char buf[MAX_PATH];
    GetDefaultWorkAreaName(buf);
    LoadWorkArea(buf, FALSE);
}

static int ParseNewWorkAreaData(HWND hwnd)
{
    int rv = 1;
    GetWindowText(GetDlgItem(hwnd, IDC_FILENEWPROJECT), newTitle, sizeof(newTitle));
    GetWindowText(GetDlgItem(hwnd, IDC_PROJECTNEWPROJECT), newName, sizeof(newName));
    newMode = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_LVNEWPROJECT));
    if (newTitle[0] == 0 || newName[0] == 0)
    {
        ExtendedMessageBox("Add New Work Area", 0, "Please specify a new work area name");
        rv = 0;
    }
    else
    {
        WIN32_FIND_DATA aa;
        HANDLE ff;
        ff = FindFirstFile(newName, &aa);
        if (ff != INVALID_HANDLE_VALUE)
        {
            FindClose(ff);
            if (aa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                int foundone = FALSE;
                char nbuf[MAX_PATH];
                strcpy(nbuf, newName);
                if (!strlen(newName) || newName[strlen(newName) - 1] != '\\')
                    strcat(nbuf, "\\");
                strcat(nbuf, "*.*");
                ff = FindFirstFile(nbuf, &aa);
                if (ff != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        if (strcmp(aa.cFileName, ".") && strcmp(aa.cFileName, ".."))
                        {
                            foundone = TRUE;
                            break;
                        }
                    } while (FindNextFile(ff, &aa));
                    FindClose(ff);
                }
                if (foundone)
                {
                    if (ExtendedMessageBox("Exists", MB_YESNO,
                                           "Project directory exists and is not empty.\nCreate Work Area anyway?") != IDYES)
                        rv = 0;
                }
            }
            else
            {
                ExtendedMessageBox("Invalid File", 0, "Project directory cannot be created");
                rv = 0;
            }
        }
        else
        {
            CreateFullPath(newName);
        }
        if (rv == 1)
        {
            if (newName[strlen(newName) - 1] != '\\')
                strcat(newName, "\\");
            strcat(newName, newTitle);
            strcat(newName, ".cwa");
        }
    }
    if (rv)
        rv = VerifyPath(newName);
    return rv;
}
static int CreateNewWorkAreaData(HWND hwnd)
{

    newTitle[0] = 0;
    if (!defaultWorkArea)
    {
        char* p;
        strcpy(newName, workArea->realName);
        p = strrchr(newName, '\\');
        if (p)
            p[0] = 0;
        // directory above last work area
        p = strrchr(newName, '\\');
        if (p)
            p[1] = 0;
        else
            strcat(newName, "\\");
    }
    else
    {
        GetDefaultProjectsPath(newName);
        strcat(newName, "\\");
    }
    SendDlgItemMessage(hwnd, IDC_FILENEWPROJECT, WM_SETTEXT, 0, (LPARAM)newTitle);
    SendDlgItemMessage(hwnd, IDC_FILENEWPROJECT, EM_SETLIMITTEXT, MAX_PATH, 0);
    SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, WM_SETTEXT, 0, (LPARAM)newName);
    SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, EM_SETLIMITTEXT, MAX_PATH, 0);
    strcpy(browsePath, newName);
    return 1;
}
//-------------------------------------------------------------------------

static long APIENTRY NewWorkAreaProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            if (!CreateNewWorkAreaData(hwnd))
            {
                EndDialog(hwnd, 1);
            }
            else
            {
                CenterWindow(hwnd);
            }
            return 1;
        case WM_COMMAND:
            switch (wParam & 0xffff)
            {
                case IDC_PROJECTNEWPROJECT:
                    SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, WM_GETTEXT, sizeof(newName), (LPARAM)newName);
                    break;
                case IDC_FILENEWPROJECT:
                    if ((wParam >> 16) == EN_CHANGE)
                    {
                        char bf[256], *p;
                        int changeIt = -1;
                        strcpy(bf, newTitle);
                        p = strrchr(bf, '.');
                        if (p)
                            *p = 0;
                        if (strlen(bf) < strlen(newName))
                        {
                            if (strlen(bf) == 0 || !stricmp(newName + strlen(newName) - strlen(bf), bf))
                            {
                                changeIt = strlen(bf);
                                if (newName[strlen(newName) - changeIt - 1] != '\\')
                                    changeIt = -1;
                            }
                        }
                        SendDlgItemMessage(hwnd, IDC_FILENEWPROJECT, WM_GETTEXT, sizeof(newTitle), (LPARAM)newTitle);
                        if (changeIt >= 0)
                        {
                            strcpy(bf, newTitle);
                            p = strrchr(bf, '.');
                            if (p)
                                *p = 0;
                            strcpy(newName + strlen(newName) - changeIt, bf);
                            SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, WM_SETTEXT, 0, (LPARAM)newName);
                        }
                    }
                    break;
                case IDC_NEWPROJECTBROWSE:
                    SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, WM_GETTEXT, sizeof(browsePath), (LPARAM)browsePath);
                    BrowseForFile(hwndFrame, "New Work Area Directory", browsePath, MAX_PATH);
                    strcat(browsePath, "\\");
                    SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, WM_SETTEXT, 0, (LPARAM)browsePath);
                    break;
                case IDOK:
                    if (ParseNewWorkAreaData(hwnd))
                        EndDialog(hwnd, IDOK);
                    break;
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    break;
                case IDHELP:
                    ContextHelp(IDH_NEW_WORKAREA_DIALOG);
                    break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
    }
    return 0;
}
void ProjectNewWorkArea(void)
{
    PROJECTITEM* data = workArea;
    if (data && data->type == PJ_WS)
    {
        SaveDrawAll();
        ResSaveAll();
        SaveAllProjects(workArea, TRUE);
        if (DialogBox(hInstance, "IDD_NEWWORKAREA", hwndFrame, (DLGPROC)NewWorkAreaProc) == IDOK)
        {
            LoadWorkArea(newName, FALSE);
            SetWorkAreaMRU(workArea);
        }
    }
}

void ProjectExistingWorkArea(void)
{
    OPENFILENAME ofn;
    SaveDrawAll();
    ResSaveAll();
    SaveAllProjects(workArea, TRUE);
    if (!(OpenFileDialog(&ofn, 0, hwndClient, FALSE, FALSE, szWorkAreaFilter, "Open Work Area")))
    {
        return;
    }
    SetExt(&ofn, ".cwa");
    LoadWorkArea(ofn.lpstrFile, TRUE);
    SetWorkAreaMRU(workArea);
}
