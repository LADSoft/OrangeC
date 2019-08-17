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
//#include <dir.h>
#include <ctype.h>
#include "idewinconst.h"
#include "header.h"
#define MAX_SAVED_DIRS 10

extern HINSTANCE hInstance;
extern HWND hwndClient;
extern PROJECTITEM* activeProject;
extern PROJECTITEM* workArea;
extern int defaultWorkArea;

char szSourceFilter[] =
    "Source files (*.c,*.cpp,*.h)\0*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.p\0"
    "Assembly files (*.asm, *.asi, *.inc)\0*.asm;*.asi;*.inc\0"
    "Resource files (*.rc,*.dlg,*.bmp,*.cur,*.ico)\0*.rc;*.dlg;*.bmp;*.cur;*.ico\0"
    "All Files (*.*)\0*.*\0";

char szProjectFilter[] = "Project files (*.cpj)\0*.cpj\0";
char szWorkAreaFilter[] = "WorksArea files (*.cwa)\0*.cwa\0";
char szTargetFilter[] =
    "Executables (*.exe)\0*.exe\0"
    "DLLs (*.dll)\0*.dll\0"
    "Librarys (*.lib)\0*.lib\0";

char szNewFileFilter[] =
    "C Source Files(*.c, *.cpp)\0*.c;*.cpp;*.cxx;*.cc\0"
    "C Header Files(*.h)\0*.h;*.hxx;*.hpp\0"
    "Resource Files(*.rc,*.bmp,*.cur,*.ico)\0*.rc;*.bmp;*.cur;*.ico\0"
    "Assembly Language Files(*.asm)\0*.asm\0"
    "All Files(*.*)\0*.*\0";

char szRuleFilter[] =
    "Rule files(*.rul)\0*.rul\0"
    "All Files(*.*)\0*.*\0";

char szCTGFilter[] = "Old Target Files(*.ctg)\0*.ctg\0";
char szCWSFilter[] = "Old Workspace Files(*.cws)\0*.cws\0";

char szBitmapFilter[] = "Bitmap (*.bmp)\0*.bmp\0";
char szCursorFilter[] = "Cursor (*.cur)\0*.cur\0";
char szIconFilter[] = "Icon (*.ico)\0*.ico\0";
char szFontFilter[] = "True Type Font (*.ttf)\0*.ttf\0Font (*.fnt)\0*.fnt\0";
char szMessageTableFilter[] = "Message Table (*.msg)\0*.msg\0";

static char szFileName[10000], szFileTitle[MAX_PATH], szDirPath[MAX_PATH];
static char* szTitle;
static char* savedDirs[MAX_SAVED_DIRS];
static int savedDirCount;
/* hook function gets rid of the toolbar on the left hand size of the
 * explorer dialog box
 */
void ProfileToMRD()
{
    char buf[MAX_PATH], *p;
    int i;
    for (i = 0; i < MAX_SAVED_DIRS; i++)
    {
        sprintf(buf, "MRUD%d", i);
        p = ProfileToString(buf, "");
        if (p[0])
        {
            savedDirs[i] = strdup(p);
            savedDirCount = i + 1;
        }
        else
            savedDirs[i] = 0;
    }
}

//-------------------------------------------------------------------------

void MRDToProfile()
{
    int i;
    char buf[MAX_PATH];
    for (i = 0; i < MAX_SAVED_DIRS; i++)
    {
        sprintf(buf, "MRUD%d", i);
        if (savedDirs[i] != 0)
        {
            StringToProfile(buf, savedDirs[i]);
        }
    }
}

//-------------------------------------------------------------------------

static void AddDirToList(char* buf)
{
    int i;
    for (i = 0; i < savedDirCount; i++)
        if (savedDirs[i] && !stricmp(savedDirs[i], buf))
        {
            if (i != 0)
            {
                char* s = savedDirs[i];
                memmove(&savedDirs[1], &savedDirs[0], i * sizeof(char*));
                savedDirs[0] = s;
            }
            return;
        }

    if (savedDirCount == MAX_SAVED_DIRS)
    {
        savedDirCount--;
        free(savedDirs[9]);
    }
    memmove(&savedDirs[1], &savedDirs[0], (savedDirCount * sizeof(char*)));
    savedDirs[0] = strdup(buf);
    savedDirCount++;
}

//-------------------------------------------------------------------------

static LRESULT CALLBACK filedlghook(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    int i;
    RECT r1, r2;
    if (iMessage == WM_NOTIFY)
    {
        OFNOTIFY* msg = (OFNOTIFY*)lParam;
        if (msg->hdr.code == CDN_INITDONE)
        {
            HWND hcombo = GetDlgItem(hwnd, IDC_EAI_DIRCOMBO);
            for (i = 0; i < savedDirCount; i++)
                if (savedDirs[i])
                    SendMessage(hcombo, CB_ADDSTRING, 0, (LPARAM)savedDirs[i]);
            CenterWindow(GetParent(hwnd));
            GetWindowRect(GetParent(hwnd), &r1);
            GetWindowRect(hcombo, &r2);
            r2.right = r1.right - 30;
            GetWindowRect(hwnd, &r1);
            r2.right -= r1.left;
            r2.bottom -= r1.top;
            r2.left -= r1.left;
            r2.top -= r1.top;
            MoveWindow(hcombo, r2.left, r2.top, r2.right - r2.left, r2.bottom - r2.top, 1);
        }
    }
    else if (iMessage == WM_INITDIALOG)
    {
        //        SendMessage(hwnd, CDM_HIDECONTROL, 1184, 0); // hides the left-hand junk
        // win50 has another way to do it, but we won't for backward compatibility
        if (szTitle)
            SendMessage(GetParent(hwnd), WM_SETTEXT, 0, (LPARAM)szTitle);
    }
    else if (iMessage == WM_COMMAND)
    {
        if (LOWORD(wParam) == IDC_EAI_DIRCOMBO)
        {
            if (HIWORD(wParam) == CBN_SELENDOK)
            {
                HWND wnd = GetDlgItem(hwnd, IDC_EAI_DIRCOMBO);
                i = SendMessage(wnd, CB_GETCURSEL, 0, 0);
                if (i != CB_ERR)
                {
                    char buf[270];
                    buf[0] = 0;
                    if (SendMessage(wnd, CB_GETLBTEXT, i, (LPARAM)buf) != CB_ERR)
                    {
                        SendMessage(GetParent(hwnd), CDM_SETCONTROLTEXT, 0x480, (LPARAM)buf);
                        SendMessage(GetParent(hwnd), WM_COMMAND, (BN_PUSHED << 16) | IDOK,
                                    (LPARAM)GetDlgItem(GetParent(hwnd), IDOK));
                        SendMessage(GetParent(hwnd), CDM_SETCONTROLTEXT, 0x480, (LPARAM) "");
                        SetFocus(GetDlgItem(GetParent(hwnd), 0x480));
                    }
                }
            }
        }
    }

    return 0;
}

//-------------------------------------------------------------------------

void setofndata(OPENFILENAME* ofn, char* name, HWND handle, char* filter)
{
    if (name && name[0])
    {
        char* p;
        strcpy(szDirPath, name);
        p = strrchr(szDirPath, '\\');
        if (p)
            p[1] = 0;
    }
    else if (activeProject)
    {
        char* p;
        strcpy(szDirPath, activeProject->realName);
        p = strrchr(szDirPath, '\\');
        if (p)
            p[1] = 0;
    }
    else
    {
        HWND win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
        if (IsWindow(win) && IsEditWindow(win))
        {
            DWINFO* q = (DWINFO*)GetWindowLong(win, 0);
            char* p;
            strcpy(szDirPath, q->dwName);
            p = strrchr(szDirPath, '\\');
            if (p)
                p[1] = 0;
        }
        else if (!defaultWorkArea)
        {
            char* p;
            strcpy(szDirPath, workArea->realName);
            p = strrchr(szDirPath, '\\');
            if (p)
                p[1] = 0;
        }
        else
        {
            GetDefaultProjectsPath(szDirPath);
        }
    }
    memset(ofn, 0, sizeof(*ofn));
    if (name)
        strcpy(szFileName, name);
    else
        szFileName[0] = 0;
    szFileTitle[0] = 0;
    //    #ifdef OPENFILENAME_SIZE_VERSION_400
    // needed because the real size confuses older windows
    //        ofn->lStructSize = OPENFILENAME_SIZE_VERSION_400;
    //    #else
    ofn->lStructSize = sizeof(OPENFILENAME);
    //    #endif
    ofn->hwndOwner = handle;
    ofn->hInstance = hInstance;
    ofn->lpstrFilter = filter;
    ofn->nFilterIndex = 0;
    ofn->lpstrFile = szFileName;
    ofn->nMaxFile = sizeof(szFileName);
    ofn->lpstrFileTitle = szFileTitle;
    ofn->nMaxFileTitle = sizeof(szFileTitle);
    ofn->lpstrInitialDir = szDirPath;
    ofn->Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
    ofn->lpfnHook = (LPOFNHOOKPROC)filedlghook;
    ofn->lpstrDefExt = NULL;
    ofn->lpTemplateName = "EXPADDIN";

    memset(szFileName, 0, sizeof(szFileName));
}

//-------------------------------------------------------------------------

void savedir(char* name)
{
    char buf[260], *p;
    strcpy(buf, name);
    p = name + strlen(name) + 1;
    if (!p[0])
    {
        p = strrchr(buf, '\\');
        if (p)
            *p = 0;
        else
        {
            p = strrchr(buf, ':');
            if (p)
            {
                *(++p) = '\\';
                *(++p) = 0;
            }
            else
                buf[0] = 0;
        }
    }
    AddDirToList(buf);
    ResetEditTitles();
}

//-------------------------------------------------------------------------

void AppendExtension(OPENFILENAME* ofn)
{
    if (ofn->lpstrFilter)
    {
        // has a filter
        char* p = ofn->lpstrFile + strlen(ofn->lpstrFile) + 1;
        if (!*p)
        {
            // they didn't multiselect

            p = strrchr(ofn->lpstrFile, '.');
            if (!p || *(p - 1) == '.')
            {
                // has no  extension
                int n = ofn->nFilterIndex * 2 - 1;
                const char* s = ofn->lpstrFilter;
                char* q = ofn->lpstrFileTitle + strlen(ofn->lpstrFileTitle);
                p = ofn->lpstrFile + strlen(ofn->lpstrFile);
                while (n--)
                    s = s + strlen(s) + 1;
                s = strchr(s, '.');
                if (s[1] != '*')
                {
                    while (*s && *s != ';')
                    {
                        *p++ = *s;
                        *q++ = *s++;
                    }
                    *p = 0;
                    *q = 0;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------

int OpenFileDialogWithCancel(OPENFILENAME* ofn, char* name, HWND handle, int new, int multiple, char* filter, char* title)
{
    szTitle = title;
    setofndata(ofn, name, handle, filter);
    if (!new)
        ofn->Flags |= OFN_FILEMUSTEXIST;
    if (multiple)
        ofn->Flags |= OFN_ALLOWMULTISELECT;
    if (GetOpenFileName(ofn))
    {
        FILE* fil;
        if (ofn->lpstrFile[strlen(ofn->lpstrFile) + 1])  // multiselect
            return 3;
        AppendExtension(ofn);
        fil = fopen(ofn->lpstrFile, "rb");
        if (fil)
        {
            fclose(fil);
            savedir(szFileName);
            return 1;
        }
        return 0;
    }
    else
    {
        if (CommDlgExtendedError() == 0)  // cancel
            return 2;
        return 0;
    }
}
int OpenFileDialog(OPENFILENAME* ofn, char* name, HWND handle, int new, int multiple, char* filter, char* title)
{
    int n = OpenFileDialogWithCancel(ofn, name, handle, new, multiple, filter, title);
    if (n == 2)
        n = 0;
    return n;
}
//-------------------------------------------------------------------------

int SaveFileDialog(OPENFILENAME* ofn, char* name, HWND handle, int saveas, char* filter, char* title)
{
    szTitle = title;
    setofndata(ofn, name, handle, filter);
    if (saveas)
        ofn->Flags |= OFN_OVERWRITEPROMPT;
    while (GetSaveFileName(ofn))
    {
        FILE* fil;
        ofn->lpstrFile[strlen(ofn->lpstrFile) + 1] = 0;
        AppendExtension(ofn);
        if (saveas && (fil = fopen(ofn->lpstrFile, "r")))
        {
            fclose(fil);
            if (ExtendedMessageBox("File Exists", MB_YESNO, "File '%s' exists, overwrite it?", ofn->lpstrFileTitle) == IDYES)
            {
                savedir(szFileName);
                return 1;
            }
        }
        else
        {
            savedir(szFileName);
            return 1;
        }
    }
    return 0;
}
