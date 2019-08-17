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
#include <ctype.h>

#include "header.h"
#include "idewinconst.h"
#include "sqlite3.h"

#define DBVersion 120

extern HINSTANCE hInstance;
extern int changedProject;
extern PROJECTITEM* workArea;
extern HWND hwndFrame, hwndClient;
extern PROJECTITEM* activeProject;

static DWINFO** browsebacklist;
static int browseCount, browseMax;
static int version_ok;

void DBClose(sqlite3* db)
{
    if (db)
        sqlite3_close(db);
}
static int verscallback(void* NotUsed, int argc, char** argv, char** azColName)
{
    if (argc == 1)
    {
        if (atoi(argv[0]) >= DBVersion)
            version_ok = TRUE;
    }
    return 0;
}
sqlite3* BrowseDBOpen(PROJECTITEM* pj)
{
    char name[MAX_PATH], *p;
    sqlite3* rv = NULL;
    strcpy(name, pj->realName);
    p = strrchr(name, '.');
    if (!p)
        p = name + strlen(name);
    strcpy(p, ".obr");
    if (sqlite3_open_v2(name, &rv, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
        char* zErrMsg = NULL;
        version_ok = FALSE;
        if (sqlite3_exec(rv, "SELECT value FROM brPropertyBag WHERE property = \"brVersion\"", verscallback, 0, &zErrMsg) !=
                SQLITE_OK ||
            !version_ok)
        {
            sqlite3_free(zErrMsg);
            DBClose(rv);
            rv = NULL;
        }
        else
        {
            sqlite3_busy_timeout(rv, 800);
        }
    }
    else
    {
        DBClose(rv);
        rv = NULL;
    }

    return rv;
}
sqlite3* BrowseOpenDBByHWND(HWND hwnd, PROJECTITEM** returnProj)
{
    PROJECTITEM* pj;
    char* filname = (char*)SendMessage(hwnd, WM_FILENAME, 0, 0);
    if (filname)
    {
        pj = HasFile(workArea, filname);
        while (pj && pj->type != PJ_PROJ)
            pj = pj->parent;
        if (!pj)
        {
            return NULL;
        }
    }
    else
    {
        pj = activeProject;
        if (!pj)
            return NULL;
    }
    *returnProj = pj;
    return BrowseDBOpen(pj);
}
void InsertBrowse(char* filname, int curline)
{
    DWINFO* info;
    char* p;
    if (browseCount >= browseMax)
    {
        if (browseCount >= 20)
        {
            memmove(browsebacklist, browsebacklist + 1, (--browseCount) * sizeof(void*));
        }
        else
        {
            browsebacklist = realloc(browsebacklist, (browseMax += 20) * sizeof(void*));
            if (!browsebacklist)
            {
                browseMax = 0;
                return;
            }
        }
    }
    info = calloc(sizeof(DWINFO), 1);
    if (!info)
        return;
    strcpy(info->dwName, filname);
    info->dwLineNo = curline;
    info->newFile = FALSE;
    p = strrchr(info->dwName, '\\');
    if (p)
        strcpy(info->dwTitle, p + 1);
    browsebacklist[browseCount++] = info;
}
typedef struct _browselinelist
{
    struct _browselinelist* next;
    char file[MAX_PATH];
    char name[512];
    int type;
    int qual;
    int line;
    int id;
} BROWSELINELIST;
static BROWSELINELIST* FindSymmetric(sqlite3* db, char* file, int line)

{
    BROWSELINELIST* rv = NULL;
    int ids[100];
    int count = 0;
    static char* query = {
        "SELECT LineNumbers.symbolId FROM LineNumbers "
        "    JOIN FileNames ON FileNames.id = LineNumbers.fileId "
        "    WHERE FileNames.name = ?"
        "        AND LineNumbers.startLine == ?;"};
    static char* query1 = {
        "SELECT FileNames.Name, LineNumbers.Startline, Names.name, "
        "                LineNumbers.qual, LineNumbers.type FROM LineNumbers "
        "    JOIN FileNames ON FileNames.id = LineNumbers.fileId "
        "    JOIN NAMES ON Names.id = LineNumbers.symbolId "
        "    Where LineNumbers.symbolId = ?"
        "    ORDER BY LineNumbers.startLine DESC;"};
    char fileName[260];
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    for (i = 0; i < l; i++)
        fileName[i] = tolower(file[i]);
    fileName[i] = 0;
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, fileName, strlen(fileName), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, line);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                {
                    if (count < 100)
                        ids[count++] = sqlite3_column_int(handle, 0);
                    break;
                }
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
        for (i = 0; i < count; i++)
        {
            rc = sqlite3_prepare_v2(db, query1, strlen(query1) + 1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle);
                sqlite3_bind_int(handle, 1, ids[i]);
                while (!done)
                {
                    switch (rc = sqlite3_step(handle))
                    {
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                        {
                            BROWSELINELIST* next = calloc(1, sizeof(BROWSELINELIST));
                            strcpy(next->file, (char*)sqlite3_column_text(handle, 0));
                            next->id = ids[i];
                            next->line = sqlite3_column_int(handle, 1);
                            strcpy(next->name, (char*)sqlite3_column_text(handle, 2));
                            next->qual = sqlite3_column_int(handle, 3);
                            next->type = sqlite3_column_int(handle, 4);
                            next->next = rv;
                            rv = next;
                            break;
                        }
                        default:
                            done = TRUE;
                            break;
                    }
                }
                sqlite3_finalize(handle);
            }
        }
    }
    return rv;
}
static BOOL FindLine(sqlite3* db, char* file, int line, char* name, char* returnedFileName, int* startLine, BOOL toDeclaration)
{
    int qual = -1;
    // this query to attempt to find it in the current file
    static char* query = {
        "SELECT FileNames.Name, LineNumbers.Startline, LineNumbers.qual FROM LineNumbers "
        "    JOIN FileNames ON FileNames.id = LineNumbers.fileId "
        "    JOIN Names ON Names.id = LineNumbers.symbolId "
        "    WHERE FileNames.name = ?"
        "        AND Names.name = ?"
        "        AND LineNumbers.startLine <= ?"
        "    ORDER BY LineNumbers.startLine DESC;"};
    // this query to find it at file scope in any file the current main file includes
    static char* query2 = {
        "SELECT FileNames.name, LineNumbers.Startline, LineNumbers.qual FROM LineNumbers "
        "    JOIN FileNames ON FileNames.id = LineNumbers.fileId"
        "    JOIN Names ON Names.id = LineNumbers.symbolId"
        "    WHERE FileNames.name != ?"
        "        AND Names.name = ?;"};
    char fileName[260];
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    for (i = 0; i < l; i++)
        fileName[i] = tolower(file[i]);
    fileName[i] = 0;
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, fileName, strlen(fileName), SQLITE_STATIC);
        sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
        sqlite3_bind_int(handle, 3, line);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                int newQual;
                int newStartLine;
                char* file;
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    file = (char*)sqlite3_column_text(handle, 0);
                    newStartLine = sqlite3_column_int(handle, 1);
                    newQual = sqlite3_column_int(handle, 2);
                    if (((newQual & 2) && toDeclaration) || (!(newQual & 2) && !toDeclaration))
                    {
                        strcpy(returnedFileName, file);
                        *startLine = newStartLine;
                        qual = newQual;
                    }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
        if (qual == -1)
        {
            rc = sqlite3_prepare_v2(db, query2, strlen(query2) + 1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle);
                sqlite3_bind_text(handle, 1, fileName, strlen(fileName), SQLITE_STATIC);
                sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
                while (!done)
                {
                    switch (rc = sqlite3_step(handle))
                    {
                        int newQual;
                        int newStartLine;
                        char* file;
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                            file = (char*)sqlite3_column_text(handle, 0);
                            newStartLine = sqlite3_column_int(handle, 1);
                            newQual = sqlite3_column_int(handle, 2);
                            if (((newQual & 2) && toDeclaration) || (!(newQual & 2) && !toDeclaration))
                            {
                                strcpy(returnedFileName, file);
                                *startLine = newStartLine;
                                qual = newQual;
                            }
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
            }
        }
        sqlite3_finalize(handle);
    }
    return qual != -1;
}

int LookupSymbolBrowse(sqlite3* db, char* name)
{
    static char* query = "SELECT id, type From Names where name = ? ";
    int id = 0, type;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    id = sqlite3_column_int(handle, 0);
                    type = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return id;
}
BROWSELIST* LookupCPPNamesBrowse(sqlite3* db, char* name, BROWSELIST* in)
{
    static char* query = {
        "SELECT id From Names"
        "     Where name = ?"};
    static char* query1 = {
        "SELECT complexId FROM CPPNameMapping"
        "    WHERE simpleId = ?"};
    static char* query2 = {

        "SELECT name From Names"
        "     Where id = ?"};
    int id = -1;
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    id = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    if (id >= 0)
    {
        rc = sqlite3_prepare_v2(db, query1, strlen(query1) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_bind_int(handle, 1, id);
            while (!done)
            {
                switch (rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        done = TRUE;
                        rc = SQLITE_OK;
                        break;
                    case SQLITE_ROW:
                    {
                        BROWSELIST* next = calloc(sizeof(BROWSELIST), 1);
                        next->id = sqlite3_column_int(handle, 0);
                        next->next = in;
                        in = next;
                        break;
                    }
                    default:
                        done = TRUE;
                        break;
                }
            }
            sqlite3_finalize(handle);
        }
        if (rc == SQLITE_OK)
        {
            BROWSELIST* scan = in;
            while (scan)
            {
                rc = sqlite3_prepare_v2(db, query2, strlen(query2) + 1, &handle, NULL);
                if (rc == SQLITE_OK)
                {
                    int done = FALSE;
                    rc = SQLITE_DONE;
                    sqlite3_bind_int(handle, 1, scan->id);
                    while (!done)
                    {
                        switch (rc = sqlite3_step(handle))
                        {
                            case SQLITE_BUSY:
                                done = TRUE;
                                break;
                            case SQLITE_DONE:
                                done = TRUE;
                                break;
                            case SQLITE_ROW:
                                strcpy(scan->name, (char*)sqlite3_column_text(handle, 0));
                                rc = SQLITE_OK;
                                done = TRUE;
                                break;
                            default:
                                done = TRUE;
                                break;
                        }
                    }
                    sqlite3_finalize(handle);
                }
                scan = scan->next;
            }
        }
    }

    return in;
}
static BROWSELIST* GetBrowseList(sqlite3* db, char* name, char* filename, int curline, BOOL toDeclaration)
{
    BROWSELIST *rv = NULL, **scan;
    if (!strrchr(name + 1, '@'))
    {
        int id;
        name[0] = '_';
        if ((id = LookupSymbolBrowse(db, name)))
        {
            BROWSELIST* next = calloc(sizeof(BROWSELIST), 1);
            next->next = rv;
            rv = next;
            strcpy(next->name, name + 1);
            next->id = id;
        }
        else if ((id = LookupSymbolBrowse(db, name + 1)))  // might be a #define
        {
            BROWSELIST* next = calloc(sizeof(BROWSELIST), 1);
            next->next = rv;
            rv = next;
            strcpy(next->name, name + 1);
            next->id = id;
        }
        rv = LookupCPPNamesBrowse(db, name, rv);
        name[0] = '@';
    }
    rv = LookupCPPNamesBrowse(db, name, rv);
    scan = &rv;
    while (*scan)
    {
        if (!FindLine(db, filename, curline, &(*scan)->name, &(*scan)->file, &(*scan)->line, toDeclaration))
            if (!FindLine(db, filename, curline, &(*scan)->name, &(*scan)->file, &(*scan)->line, !toDeclaration))
            {
                BROWSELIST* next = (*scan)->next;
                free(*scan);
                (*scan) = next;
                continue;
            }
        scan = &(*scan)->next;
    }
    return rv;
}
static LRESULT CALLBACK BrowseInfoSelectProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    BROWSELIST* list;
    LV_COLUMN lvC;
    LV_ITEM item;
    RECT r;
    HWND hwndlb;
    int i;
    switch (iMessage)
    {
        case WM_NOTIFY:
            if (wParam == IDC_FBPLISTBOX)
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
                EndDialog(hwnd, 0);
            }
            else if (wParam == IDC_BMGOTO)
            {

                int sel = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_FBPLISTBOX));
                if (sel == -1)
                {
                    EndDialog(hwnd, 0);
                    break;
                }
                item.iItem = sel;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM;
                ListView_GetItem(GetDlgItem(hwnd, IDC_FBPLISTBOX), &item);
                EndDialog(hwnd, item.lParam);
            }
            break;
        case WM_INITDIALOG:
            list = (FUNCTIONLIST*)lParam;
            CenterWindow(hwnd);
            hwndlb = GetDlgItem(hwnd, IDC_FBPLISTBOX);
            ApplyDialogFont(hwndlb);
            GetWindowRect(hwndlb, &r);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
            lvC.cx = 1000;
            lvC.iSubItem = 0;
            ListView_InsertColumn(hwndlb, 0, &lvC);
            i = 0;
            while (list)
            {
                char buf[2048];
                char unmang[2048];
                unmangle(unmang, list->name);
                sprintf(buf, "%s:  %s", unmang, list->file);
                item.iItem = i++;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM | LVIF_TEXT;
                item.lParam = (LPARAM)list;
                item.pszText = buf;
                ListView_InsertItem(hwndlb, &item);
                list = list->next;
            }
            break;
    }
    return 0;
}
static BROWSELIST* FindBrowseToName(sqlite3* db, char* name, char* filname, int curline, BOOL toDeclaration)
{
    BROWSELIST* list = GetBrowseList(db, name, filname, curline, toDeclaration);
    if (list && list->next)
    {
        BROWSELIST* selected = (BROWSELIST*)DialogBoxParam(hInstance, "BROWSEINFOSELECTDIALOG", hwndFrame,
                                                           (DLGPROC)BrowseInfoSelectProc, (LPARAM)list);
        while (list)
        {
            BROWSELIST* next;
            if (list == selected)
                list = list->next;
            if (list)
            {
                next = list->next;
                free(list);
                list = next;
            }
        }
        return selected;
    }
    else if (list)
    {
        return list;
    }
    return NULL;
}
static BROWSELIST* LookupSymmetricBrowse(sqlite3* db, char* name, char* filname, int curline, BOOL toDeclaration)
{
    BROWSELIST* rv = NULL;
    BROWSELINELIST *ids = FindSymmetric(db, filname, curline), *scan;
    int currentType = 0;
    scan = ids;
    while (scan)
    {
        if (scan->type & 2)
            currentType = 2;
        else if ((scan->type & 1) && currentType != 2)
            currentType = 1;
        scan = scan->next;
    }
    if (currentType)
    {
        BROWSELINELIST* current = NULL;
        scan = ids;
        while (scan)
        {
            if (scan->type & currentType)
            {
                if ((toDeclaration && !(scan->qual & 2)) || (!toDeclaration && (scan->qual & 2)))
                {
                    if (current)
                        break;
                    current = scan;
                }
            }
            scan = scan->next;
        }
        if (!scan && current)
        {
            scan = ids;
            while (scan)
            {
                if ((scan->type & currentType) && scan->id == current->id)
                {
                    if ((toDeclaration && (scan->qual & 2)) || (!toDeclaration && !(scan->qual & 2)))
                    {
                        rv = calloc(1, sizeof(BROWSELINELIST));
                        strcpy(rv->file, scan->file);
                        rv->line = scan->line;

                        break;
                    }
                }
                scan = scan->next;
            }
        }
    }
    while (ids)
    {
        BROWSELINELIST* next = ids->next;
        free(ids);
        ids = next;
    }
    return rv;
}
// I usually don't mix accesses between multiple databases, but it just seemed
// so promising to be able to filter the function list based on a structure
// being accessed
BROWSELIST* LookupBrowseStruct(sqlite3* db, char* name, char* filename, int curline, BOOL toDeclaration)
{
    BROWSELIST* rv = NULL;
    char hold;
    char* b = name + strlen(name);
    CCSTRUCTDATA* structData;
    while ((b > name && isalnum(b[-1])) || b[-1] == '_')
        b--;
    if (isdigit(b[0]))
        return NULL;
    hold = *b;
    *b = 0;
    structData = GetStructType(&name, curline, filename, NULL, NULL);
    *b = hold;
    if (structData && name[0])
    {
        int i;
        size_t len = strlen(name);
        BROWSELIST *selected, **scan;
        for (i = 0; i < structData->fieldCount; i++)
        {
            BOOL found = FALSE;
            char testfor[2048];
            int sz = strlen(structData->data[i].fieldName);
            if (len < sz)
                if (!strncmp(structData->data[i].fieldName + sz - len, name, len))
                {
                    found = TRUE;
                }
            if (!found)
            {
                sprintf(testfor, "@%s$", name);
                if (strstr(structData->data[i].fieldName, testfor))
                    found = TRUE;
            }
            if (found)
            {
                int id;
                if ((id = LookupSymbolBrowse(db, structData->data[i].fieldName)))
                {
                    BROWSELIST* next = calloc(sizeof(BROWSELIST), 1);
                    next->next = rv;
                    rv = next;
                    strcpy(next->name, structData->data[i].fieldName);
                    next->id = id;
                }
            }
        }
        scan = &rv;
        while (*scan)
        {
            if (!FindLine(db, filename, curline, &(*scan)->name, &(*scan)->file, &(*scan)->line, toDeclaration))
                if (!FindLine(db, filename, curline, &(*scan)->name, &(*scan)->file, &(*scan)->line, !toDeclaration))
                {
                    BROWSELIST* next = (*scan)->next;
                    free(*scan);
                    (*scan) = next;
                    continue;
                }
            scan = &(*scan)->next;
        }
        if (rv && rv->next)
        {
            selected = (BROWSELIST*)DialogBoxParam(hInstance, "BROWSEINFOSELECTDIALOG", hwndFrame, (DLGPROC)BrowseInfoSelectProc,
                                                   (LPARAM)rv);
            while (rv)
            {
                BROWSELIST* next;
                if (rv == selected)
                    rv = rv->next;
                if (rv)
                {
                    next = rv->next;
                    free(rv);
                    rv = next;
                }
            }
            rv = selected;
        }
    }
    return rv;
}
//-------------------------------------------------------------------------

void BrowseTo(HWND hwnd, char* msg, BOOL toDeclaration)
{
    static char name[2048];
    static char mangled[2048];
    BOOL browsing = FALSE;

    int ofs;
    if (msg)
    {
        strcpy(name, msg);
        browsing = TRUE;
        hwnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
    }
    else
    {
        browsing = SendMessage(hwnd, WM_WORDUNDERCURSOR, 2, (LPARAM)name);
    }
    if (!PropGetBool(NULL, "BROWSE_INFORMATION") && browsing)
    {
        ExtendedMessageBox("Browse Info Alert", MB_OK, "Browse information not enabled");
        browsing = FALSE;
    }
    if (browsing)
    {
        BROWSELIST* selected;
        sqlite3* db = NULL;
        CHARRANGE charrange;
        char* pName = name;
        int curline;
        char* filname;
        PROJECTITEM* pj;
        if (msg)
        {
            curline = -2;
            filname = "";
        }
        else
        {
            SendDlgItemMessage(hwnd, ID_EDITCHILD, EM_EXGETSEL, (WPARAM)0, (LPARAM)&charrange);
            curline = SendDlgItemMessage(hwnd, ID_EDITCHILD, EM_EXLINEFROMCHAR, 0, (LPARAM)charrange.cpMin) + 1;
            filname = (char*)SendMessage(hwnd, WM_FILENAME, 0, 0);
        }
        db = BrowseOpenDBByHWND(hwnd, &pj);
        if (!db)
        {
            return;
        }
        selected = LookupBrowseStruct(db, pName, filname, curline, toDeclaration);
        if (!selected)
        {
            GetQualifiedName(mangled, &pName, FALSE, FALSE);
            selected = LookupSymmetricBrowse(db, mangled, filname, curline, toDeclaration);
            if (!selected)
                selected = FindBrowseToName(db, mangled, filname, curline, toDeclaration);
        }
        if (selected)
        {
            DWINFO info, *ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            char* p;
            memset(&info, 0, sizeof(info));
            strcpy(info.dwName, selected->file);
            info.dwLineNo = selected->line;
            p = strrchr(info.dwName, '\\');
            if (p)
                strcpy(info.dwTitle, p + 1);
            info.logMRU = FALSE;
            info.newFile = FALSE;
            InsertBrowse(ptr->dwName, curline);
            CreateDrawWindow(&info, TRUE);
            free(selected);
        }
        DBClose(db);
    }
    browsing = FALSE;
}

//-------------------------------------------------------------------------

void BrowseBack(void)
{
    if (!browseCount)
        return;
    browsebacklist[--browseCount]->logMRU = FALSE;
    CreateDrawWindow(browsebacklist[browseCount], TRUE);
    free(browsebacklist[browseCount]);
}
