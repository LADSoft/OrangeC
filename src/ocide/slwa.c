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
#define WSPVERS 8

#include "header.h"
#include "xml.h"

extern char currentProfileName[256];
extern char findText[256];
extern int profileDebugMode;
extern HWND hwndTbFind;
extern int editFlags;
extern DWINFO* editWindows;
extern HINSTANCE hInstance;
extern struct tagfile* tagFileList;
extern HWND hwndClient, hwndFrame;
extern HWND hwndSrcTab;
extern char* findhist[MAX_COMBO_HISTORY];
extern char* findbrowsehist[MAX_COMBO_HISTORY];
extern char* replacehist[MAX_COMBO_HISTORY];
extern char* replacebrowsehist[MAX_COMBO_HISTORY];
extern char* watchhist[MAX_COMBO_HISTORY];
extern char* funcbphist[MAX_COMBO_HISTORY];
extern char* databphist[MAX_COMBO_HISTORY];
extern char* memhist[MAX_COMBO_HISTORY];
extern char* varinfohist[MAX_COMBO_HISTORY];
extern DWINFO* newInfo;
extern char szDrawClassName[];
extern char szUntitled[];
extern DATABREAK* dataBpList;
extern HDWEBKPT hdwebp[4];
extern PROJECTITEM* activeProject;
extern FILEBROWSE* fileBrowseInfo;
extern int fileBrowseCount;
extern POINT findDlgPos;
extern int findflags;
extern int replaceflags;
extern int findmode;
extern int replacemode;
extern int findext;
extern int replaceext;
extern PROFILENAMELIST* profileNames;
extern char* sysProfileName;
extern BOOL snapToGrid;
extern BOOL showGrid;
extern int gridSpacing;
extern int simpleFindFlags;
extern DOCK_STR debugDocks[100], releaseDocks[100];
extern int debugDockCount, releaseDockCount, dockCount;
extern RECT oldFrame;
extern HWND debugfreeWindows[100], releasefreeWindows[100];
extern int debugfreeWindowCount, releasefreeWindowCount;
extern int freeWindowCount;
extern HWND handles[MAX_HANDLES];
extern HWND hwndToolbarBar;
extern int errorButtons;
extern int toolLayout[100];
extern int toolCount;
extern HWND hwndToolNav, hwndToolBuildType, hwndToolEdit, hwndToolDebug, hwndToolBuild, hwndToolBookmark, hwndToolThreads;
extern int toolLayout[100];
extern int toolCount;

extern int memoryWordSize;

int restoredDocks = FALSE;

static char ipathbuf[2048];

//-------------------------------------------------------------------------

void abspath(char* name, char* path)
{
    char projname[256], *p, *nname = name;
    if (!path)
    {
        path = projname;
        GetCurrentDirectory(256, projname);
        strcat(projname, "\\hi");
    }
    if (!path[0])
        return;
    if (name[0] == 0)
        return;
    if (name[1] == ':')
        return;
    strcpy(projname, path);
    p = strrchr(projname, '\\');
    if (!p)
        return;
    p--;
    if (!strstr(name, "..\\"))
    {
        if (name[0] != '\\')
        {
            strcpy(p + 2, name);
            strcpy(nname, projname);
        }
        return;
    }
    while (!strncmp(name, "..\\", 3))
    {
        while (p > projname && *p-- != '\\')
            ;
        name += 3;
    }
    if (p[0] != '\\')
        *++p = '\\';
    p++;
    strcpy(p, name);
    strcpy(nname, projname);
}

//-------------------------------------------------------------------------

char* relpath(char* name, char* path)
{
    static char projname[MAX_PATH], localname[MAX_PATH];
    char *p = localname, *q = projname, *r, *s;
    if (!path[0])
        return name;
    if (toupper(name[0]) != toupper(path[0]))
        return name;

    strcpy(localname, name);
    strcpy(projname, path);
    r = strrchr(localname, '\\');
    if (r)
        *r++ = 0;
    // r has the point to the file name
    else
        r = localname;
    s = strrchr(projname, '\\');
    if (!s)
        return name;
    if (*s)
        *s = 0;

    while (*p && *q && toupper(*p) == toupper(*q))
    {
        p++, q++;
    }

    if (*p == '\\')
        p++;
    if (*q == '\\')
        q++;

    int count = *q != 0;
    char* t = q;
    while (*t)
        if (*t++ == '\\')
            count++;
    projname[0] = 0;
    while (count--)
        strcat(projname, "..\\");
    if (*p)
    {
        strcat(projname, p);
        strcat(projname, "\\");
    }
    strcat(projname, r);
    return projname;
}
char* relpath2(char* name, char* path)
{
    static char projname[MAX_PATH], localname[MAX_PATH];
    char *p = localname, *q = projname, *r, *s;
    if (!path[0])
        return name;
    if (toupper(name[0]) != toupper(path[0]))
        return name;

    strcpy(localname, name);
    strcpy(projname, path);
    r = strrchr(localname, '\\');
    if (r)
        *r++ = 0;
    // r has the point to the file name
    else
        r = localname;
    s = strrchr(projname, '\\');
    if (!s)
        return name;
    if (*s)
        *s = 0;

    while (*p && *q && toupper(*p) == toupper(*q))
    {
        p++, q++;
    }
    while (q != path && q[-1] != '\\')
        p--, q--;
    if (!(*p | *q))
        return r;
    else if (*(p - 1) == '\\' && *(p - 2) == ':')
        return name;
    else
    {
        int count = 0;
        if (*q != '\\')
            while (p > localname && *p != '\\')
                p--;
        while (*q && (q = strchr(q + 1, '\\')))
            count++;
        projname[0] = 0;
        while (count--)
            strcat(projname, "..\\");
        if (*p)
        {
            strcat(projname, p + 1);
            strcat(projname, "\\");
        }
        strcat(projname, r);
        return projname;
    }
}
//-------------------------------------------------------------------------

void absincludepath(char* name, char* path)
{
    char *p = ipathbuf, *dest1 = name, *dest2 = name;
    strcpy(ipathbuf, name);
    name[0] = 0;
    do
    {
        char* q;
        if (*p == ';')
            p++;
        q = p;
        dest1 = dest2;
        p = strchr(p, ';');
        if (!p)
            p = ipathbuf + strlen(ipathbuf);
        while (q != p)
            *dest2++ = *q++;
        *dest2 = 0;
        abspath(dest1, path);
        dest2 = dest1 + strlen(dest1);
        if (*p)
            *dest2++ = ';';
    } while (*p);
}

//-------------------------------------------------------------------------

char* relincludepath(char* name, char* path)
{
    char *p = name, *dest1 = ipathbuf, *dest2 = ipathbuf;
    ipathbuf[0] = 0;
    do
    {
        char* q;
        if (*p == ';')
            p++;
        q = p;
        dest1 = dest2;
        p = strchr(p, ';');
        if (!p)
            p = name + strlen(name);
        while (q != p)
            *dest2++ = *q++;
        *dest2 = 0;
        strcpy(dest1, relpath(dest1, path));
        dest2 = dest1 + strlen(dest1);
        if (*p)
            *dest2++ = ';';
    } while (*p);
    return ipathbuf;
}

PROJECTITEM* LoadErr(struct xmlNode* root, char* name)
{
    xmlFree(root);
    ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Project File %s is the wrong format", name);
    return 0;
}
PROJECTITEM* NoMemory(struct xmlNode* root)
{
    xmlFree(root);
    ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Out of memory");
    return 0;
}

void NoMemoryWS(void) { ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Out of memory"); }

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void RestoreMemoryWindowSettings(struct xmlNode* node, int version)
{
    struct xmlAttr* attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "WORDSIZE"))
            memoryWordSize = atoi(attribs->value);
        attribs = attribs->next;
    }
}
void RestoreErrorWindowSettings(struct xmlNode* node, int version)
{
    struct xmlAttr* attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "BTNS"))
            errorButtons = atoi(attribs->value);
        attribs = attribs->next;
    }
}

void RestoreHistory(struct xmlNode* node, int version)
{
    int count = 0;
    char** histitem = 0;
    struct xmlAttr* attribs = node->attribs;
    struct xmlNode* children = node->children;
    while (attribs)
    {
        if (IsAttrib(attribs, "TYPE"))
        {
            if (!strcmp(attribs->value, "FIND"))
                histitem = findhist;
            else if (!strcmp(attribs->value, "REPLACE"))
                histitem = replacehist;
            else if (!strcmp(attribs->value, "WATCH"))
                histitem = watchhist;
            else if (!strcmp(attribs->value, "FUNCBP"))
                histitem = funcbphist;
            else if (!strcmp(attribs->value, "DATABP"))
                histitem = databphist;
            else if (!strcmp(attribs->value, "MEMORY"))
                histitem = memhist;
            else if (!strcmp(attribs->value, "VARINFO"))
                histitem = varinfohist;
            else if (!strcmp(attribs->value, "BFINDPATH"))
                histitem = findbrowsehist;
            else if (!strcmp(attribs->value, "BREPLACEPATH"))
                histitem = replacebrowsehist;
        }
        attribs = attribs->next;
    }
    if (!histitem)
        return;
    for (count = 0; count < MAX_COMBO_HISTORY; count++)
        if (histitem[count])
        {
            free(histitem[count]);
            histitem[count] = 0;
        }
    count = 0;
    while (children && count < MAX_COMBO_HISTORY)
    {
        if (IsNode(children, "HISTITEM"))
        {
            attribs = children->attribs;
            while (attribs && count < MAX_COMBO_HISTORY)
            {
                if (IsAttrib(attribs, "VALUE"))
                {
                    histitem[count++] = strdup(attribs->value);
                }
                attribs = attribs->next;
            }
        }
        children = children->next;
    }
    if (histitem == findhist)
    {
        if (findhist[0])
            strcpy(findText, findhist[0]);
        SendMessage((HWND)hwndTbFind, WM_SETHISTORY, 0, (LPARAM)findhist);
    }
}

//-------------------------------------------------------------------------
void RestoreToolBars(struct xmlNode* node, int version)
{
    int id = 0;
    HWND hwnd;
    char* defines = 0;
    struct xmlAttr* attribs = node->attribs;
    if (version != WSPVERS)
        return;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
            id = atoi(attribs->value);
        else if (IsAttrib(attribs, "DEFINE"))
            defines = attribs->value;
        attribs = attribs->next;
    }
    if (id && defines)
    {
        hwnd = GetToolWindow(id);
        if (hwnd)
            SetToolBarData(hwnd, defines);  // undefined in local context
    }
}
void RestoreToolbarLayout(struct xmlNode* node, int version)
{
    int n = 0;
    struct xmlAttr* attribs = node->attribs;
    if (version != WSPVERS)
        return;
    while (attribs)
    {
        if (IsAttrib(attribs, "COUNT"))
            n = atoi(attribs->value);
        attribs = attribs->next;
    }
    if (n)
    {
        char* str = node->textData;
        int pos = 0;
        int i;
        for (i = 0; i < n && i < sizeof(toolLayout) / sizeof(toolLayout[0]); i++)
        {
            int d;
            if (sscanf(str, "%d%n", &d, &pos))
            {
                int visible = d < 10000;
                if (d >= 10000)
                    d -= 10000;
                toolLayout[i] = d;
                if (d > 0)
                {
                    ShowWindow(GetToolWindow(d), visible ? SW_SHOW : SW_HIDE);
                }
                str += pos;
            }
            else
                break;
        }
        toolCount = i;
    }
    SendMessage(hwndToolbarBar, WM_REDRAWTOOLBAR, 0, 0);
    ResizeLayout(NULL);
}
void RestoreDocks(struct xmlNode* node, int version)
{
    struct xmlNode* nodes = node->children;
    DOCK_STR debug[100], release[100];
    HWND debugfrees[100], releasefrees[100];
    int dbgCount = 0;
    int rlsCount = 0;
    int dbgfreeCount = 0;
    int rlsfreeCount = 0;
    while (nodes)
    {
        int ctl = -1;
        int type;
        RECT r;
        int index;
        int sel;
        struct xmlAttr* attribs = nodes->attribs;
        while (attribs)
        {
            if (IsAttrib(attribs, "SEL"))
                sel = atoi(attribs->value);
            if (IsAttrib(attribs, "TYPE"))
                type = atoi(attribs->value);
            if (IsAttrib(attribs, "INDEX"))
                index = atoi(attribs->value);
            if (IsAttrib(attribs, "CTL"))
                ctl = atoi(attribs->value);
            else if (IsAttrib(attribs, "POS"))
            {
                sscanf(attribs->value, "%d %d %d %d", &r.left, &r.top, &r.right, &r.bottom);
                r.right += r.left;
                r.bottom += r.top;
            }
            attribs = attribs->next;
        }
        if (IsNode(nodes, "FRAME"))
        {
            oldFrame = r;
        }
        else if (IsNode(nodes, "DOCKWND"))
        {
            HWND wnd = CreateControlWindow(hwndFrame);
            int current, n;
            char* p = nodes->textData;
            if (type == 2)
            {
                SetWindowPos(wnd, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOACTIVATE | SWP_NOZORDER);
                SendMessage(wnd, TW_MAKEPOPUP, 0, 0);
                //                SetParent(wnd, HWND_DESKTOP);
                debugfrees[dbgfreeCount++] = wnd;
            }
            else if (type == 3)
            {
                SetWindowPos(wnd, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOACTIVATE | SWP_NOZORDER);
                SendMessage(wnd, TW_MAKEPOPUP, 0, 0);
                //                SetParent(wnd, HWND_DESKTOP);
                releasefrees[rlsfreeCount++] = wnd;
            }
            else
            {
                int* count;
                DOCK_STR* docks;
                if (type == 0)
                {
                    count = &dbgCount;
                    docks = &debug[0];
                }
                else
                {
                    count = &rlsCount;
                    docks = &release[0];
                }
                if (index >= *count)
                {
                    *count = index + 1;
                }
                docks[index].ctl = ctl;
                docks[index].r = r;
                docks[index].hwnd = wnd;
            }
            while (sscanf(p, "%d%n", &current, &n) == 1)
            {
                SendMessage(wnd, TW_ADD, 0, (LPARAM)GetWindowHandle(current));
                p += n;
            }
            SendMessage(wnd, TW_SELECT, 1, (LPARAM)GetWindowHandle(sel));
        }
        nodes = nodes->next;
    }
    if (dbgCount || rlsCount || dbgfreeCount || rlsfreeCount)
    {
        int i;
        for (i = 0; i < debugDockCount; i++)
            DestroyWindow(debugDocks[i].hwnd);
        debugDockCount = 0;
        for (i = 0; i < releaseDockCount; i++)
            DestroyWindow(releaseDocks[i].hwnd);
        releaseDockCount = 0;
        for (i = 0; i < debugfreeWindowCount; i++)
            DestroyWindow(debugfreeWindows[i]);
        debugfreeWindowCount = 0;
        for (i = 0; i < releasefreeWindowCount; i++)
            DestroyWindow(releasefreeWindows[i]);
        releasefreeWindowCount = 0;
        memcpy(&debugDocks[0], &debug[0], dbgCount * sizeof(DOCK_STR));
        memcpy(&releaseDocks[0], &release[0], rlsCount * sizeof(DOCK_STR));
        memcpy(&debugfreeWindows[0], &debugfrees[0], dbgfreeCount * sizeof(HWND));
        memcpy(&releasefreeWindows[0], &releasefrees[0], rlsfreeCount * sizeof(HWND));
        debugDockCount = dbgCount;
        dockCount = releaseDockCount = rlsCount;
        debugfreeWindowCount = dbgfreeCount;
        freeWindowCount = releasefreeWindowCount = rlsfreeCount;
        for (i = 0; i < releasefreeWindowCount; i++)
            ShowWindow(releasefreeWindows[i], SW_SHOW);
        SelectDebugWindows(FALSE);
        ResizeLayout(NULL);
    }
}

void RestoreWindows(struct xmlNode* node, int version, PROJECTITEM* wa)
{
    MDICREATESTRUCT mc;
    static DWINFO info;
    struct xmlNode* child = node->children;
    HWND hwnd = NULL;
    ShowWindow(hwndSrcTab, SW_HIDE);
    while (child)
    {
        if (IsNode(child, "WINDOW"))
        {
            struct xmlAttr* attribs = child->attribs;
            int copyId = 0;
            EDITDATA* extra = NULL;
            memset(&info, 0, sizeof(info));
            memset(&mc, 0, sizeof(mc));
            while (attribs)
            {
                if (IsAttrib(attribs, "NAME"))
                {
                    strcpy(info.dwName, attribs->value);
                    abspath(info.dwName, wa->realName);
                }
                else if (IsAttrib(attribs, "TITLE"))
                    strcpy(info.dwTitle, attribs->value);
                else if (IsAttrib(attribs, "X"))
                    mc.x = atoi(attribs->value);
                else if (IsAttrib(attribs, "Y"))
                    mc.y = atoi(attribs->value);
                else if (IsAttrib(attribs, "WIDTH"))
                    mc.cx = atoi(attribs->value);
                else if (IsAttrib(attribs, "HEIGHT"))
                    mc.cy = atoi(attribs->value);
                else if (IsAttrib(attribs, "LINENO"))
                    info.dwLineNo = atoi(attribs->value);
                else if (IsAttrib(attribs, "COPYID"))
                    copyId = atoi(attribs->value);
                attribs = attribs->next;
            }
            newInfo = &info;
            if (copyId)
            {
                DWINFO* ptr = editWindows;
                while (ptr)
                {
                    if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)newInfo))
                    {
                        extra = (EDITDATA*)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0);
                        break;
                    }
                    ptr = ptr->next;
                }
            }

            hwnd = CreateMDIWindow(szDrawClassName, szUntitled,
                                   WS_VISIBLE | WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES |
                                       WS_SIZEBOX | (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU) |
                                       WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                   mc.x, mc.y, mc.cx, mc.cy, hwndClient, hInstance, (LPARAM)extra);
            if (copyId)
            {
                SendMessage(hwnd, WM_SETSIBLINGID, copyId - 1, 0);
            }
        }
        else if (IsNode(child, "ACTIVE"))
        {
            // should be last
            struct xmlAttr* attribs = child->attribs;
            newInfo = &info;
            info.dwLineNo = -1;
            info.logMRU = FALSE;
            info.newFile = FALSE;
            while (attribs)
            {
                if (IsAttrib(attribs, "NAME"))
                {
                    strcpy(info.dwName, attribs->value);
                    abspath(info.dwName, wa->realName);
                }
                else if (IsAttrib(attribs, "TITLE"))
                {
                    strcpy(info.dwTitle, attribs->value);
                }
                attribs = attribs->next;
            }
            CreateDrawWindow(&info, TRUE);
        }
        child = child->next;
    }
    ShowWindow(hwndSrcTab, SW_SHOW);
}

//-------------------------------------------------------------------------

void RestoreChangeLn(struct xmlNode* node, int version, PROJECTITEM* wa)
{
    struct xmlNode* children = node->children;
    struct tagfile *l, **ls = &tagFileList, *lprev = NULL;
    struct xmlAttr* attribs;
    while (*ls)
    {
        lprev = *ls;
        ls = &(*ls)->next;
    }
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            struct xmlNode* fchildren = children->children;
            struct tagchangeln *c = 0, **x = &c;
            *ls = calloc(sizeof(struct tagfile), 1);
            if (!ls)
            {
                NoMemoryWS();
                return;
            }
            attribs = children->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "NAME"))
                {
                    strcpy((*ls)->name, attribs->value);
                    abspath((*ls)->name, wa->realName);
                }
                attribs = attribs->next;
            }
            l = tagFileList;
            while (l)
            {
                if (!strcmp(l->name, (*ls)->name))
                    break;
                l = l->next;
            }
            if (l != *ls)
            {
                free(*ls);
                *ls = 0;
            }
            else
            {
                (*ls)->prev = lprev;
                lprev = (*ls);
                ls = &(*ls)->next;
            }
            while (fchildren)
            {
                if (IsNode(fchildren, "LINE"))
                {
                    // assumes enumerated in order
                    *x = calloc(sizeof(struct tagchangeln), 1);
                    if (!*x)
                    {
                        NoMemoryWS();
                        return;
                    }
                    attribs = fchildren->attribs;
                    while (attribs)
                    {
                        if (IsAttrib(attribs, "NUM"))
                            (*x)->lineno = atoi(attribs->value);
                        else if (IsAttrib(attribs, "DELTA"))
                            (*x)->delta = atoi(attribs->value);
                        attribs = attribs->next;
                    }
                    x = &(*x)->next;
                }
                fchildren = fchildren->next;
            }
            l->changedLines = c;
        }
        children = children->next;
    }
}
void RestoreFileBrowse(struct xmlNode* node, int version, PROJECTITEM* wa)
{
    struct xmlNode* children = node->children;
    struct xmlAttr* attribs;
    FILEBROWSE **p = &fileBrowseInfo, *last = NULL;
    FileBrowseCloseAll();
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            char buf[MAX_PATH];
            int lineno = -1;
            buf[0] = 0;
            attribs = children->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "NAME"))
                {
                    strcpy(buf, attribs->value);
                    abspath(buf, wa->realName);
                }
                else if (IsAttrib(attribs, "LINE"))
                {
                    lineno = atoi(attribs->value);
                }
                attribs = attribs->next;
            }
            if (lineno != -1 && buf[0])
            {
                DWINFO* info = GetFileInfo(buf);
                if (info)
                {
                    FILEBROWSE* newBrowse = calloc(1, sizeof(FILEBROWSE));
                    if (newBrowse)
                    {
                        newBrowse->lineno = lineno;
                        newBrowse->info = info;
                        newBrowse->next = NULL;
                        newBrowse->prev = last;
                        last = newBrowse;
                        *p = newBrowse;
                        p = &(*p)->next;
                        fileBrowseCount++;
                    }
                }
            }
        }
        children = children->next;
    }
}

//-------------------------------------------------------------------------

void RestoreTags(struct xmlNode* node, int version, PROJECTITEM* wa)
{
    struct tagfile *l, **ls = &tagFileList, *lprev = NULL;
    struct tag **ts, *tprev = NULL;
    struct xmlAttr* attribs = node->attribs;
    struct xmlNode* children = node->children;
    int tagid = -1;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
            tagid = atoi(attribs->value);
        attribs = attribs->next;
    }
    if (tagid == -1 || (tagid != TAG_BP && tagid != TAG_BOOKMARK))
        return;
    while (*ls)
    {
        lprev = *ls;
        ls = &(*ls)->next;
    }
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            struct xmlNode* fchildren = children->children;
            *ls = calloc(sizeof(struct tagfile), 1);
            if (!ls)
            {
                NoMemoryWS();
                return;
            }
            attribs = children->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "NAME"))
                {
                    strcpy((*ls)->name, attribs->value);
                    abspath((*ls)->name, wa->realName);
                }
                attribs = attribs->next;
            }
            l = tagFileList;
            while (l)
            {
                if (!strcmp(l->name, (*ls)->name))
                    break;
                l = l->next;
            }
            if (l != *ls)
            {
                free(*ls);
                *ls = 0;
            }
            else
            {
                (*ls)->prev = lprev;
                lprev = *ls;
                ls = &(*ls)->next;
            }
            ts = &l->tagArray[tagid];
            tprev = NULL;
            while (fchildren)
            {
                if (IsNode(fchildren, "TAGITEM"))
                {
                    *ts = calloc(sizeof(struct tag), 1);
                    if (!*ts)
                    {
                        NoMemoryWS();
                        return;
                    }
                    (*ts)->prev = tprev;
                    tprev = *ts;
                    attribs = fchildren->attribs;
                    while (attribs)
                    {
                        if (IsAttrib(attribs, "LINENO"))
                            (*ts)->drawnLineno = (*ts)->editingLineno = (*ts)->debugLineno = atoi(attribs->value);
                        else if (IsAttrib(attribs, "CP"))
                            (*ts)->charpos = atoi(attribs->value);
                        else if (IsAttrib(attribs, "ENABLED"))
                            (*ts)->enabled = atoi(attribs->value);
                        else if (IsAttrib(attribs, "DISABLED"))
                            (*ts)->disable = atoi(attribs->value);
                        attribs = attribs->next;
                    }
                    if (tagid == TAG_BOOKMARK)
                    {
                        (*ts)->extra = strdup(fchildren->textData);
                    }
                    ts = &(*ts)->next;
                }
                fchildren = fchildren->next;
            }
        }
        children = children->next;
    }
}

//-------------------------------------------------------------------------

void RestoreFind(struct xmlNode* node, int version)
{
    struct xmlAttr* attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "X"))
            findDlgPos.x = atoi(attribs->value);
        else if (IsAttrib(attribs, "Y"))
            findDlgPos.y = atoi(attribs->value);
        else if (IsAttrib(attribs, "FF"))
        {
            sscanf(attribs->value, "%d", &findflags);
        }
        else if (IsAttrib(attribs, "RF"))
        {
            sscanf(attribs->value, "%d", &replaceflags);
        }
        else if (IsAttrib(attribs, "FM"))
            findmode = atoi(attribs->value);
        else if (IsAttrib(attribs, "RM"))
            replacemode = atoi(attribs->value);
        else if (IsAttrib(attribs, "FE"))
            findext = atoi(attribs->value);
        else if (IsAttrib(attribs, "RE"))
            replaceext = atoi(attribs->value);
        attribs = attribs->next;
    }
}
void RestoreSimpleFind(struct xmlNode* node, int version)
{
    struct xmlAttr* attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "FLAGS"))
            simpleFindFlags = atoi(attribs->value);
        attribs = attribs->next;
    }
}
//-------------------------------------------------------------------------

void RestoreWatch(struct xmlNode* node, int version)
{
    struct xmlAttr* attribs = node->attribs;
    int id = 0;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
        {
            // ID MUST BE FIRST!!!!!
            id = atoi(attribs->value) - 1;
            if (id < 0 || id > 3)
                return;
        }
        else if (IsAttrib(attribs, "ENABLE"))
        {
            hdwebp[id].disable = atoi(attribs->value);
        }
        else if (IsAttrib(attribs, "ACTIVE"))
        {
            if (atoi(attribs->value))
                hdwebp[id].active = TRUE;
        }
        else if (IsAttrib(attribs, "NAME"))
        {
            strncpy(hdwebp[id].name, attribs->value, 255);
            hdwebp[id].name[255] = 0;
        }
        else if (IsAttrib(attribs, "MODE"))
        {
            hdwebp[id].mode = atoi(attribs->value);
        }
        else if (IsAttrib(attribs, "SIZE"))
        {
            hdwebp[id].size = atoi(attribs->value);
        }
        attribs = attribs->next;
    }
}
void RestoreDataBreakpoints(struct xmlNode* node, int version)
{
    struct xmlAttr* attribs = node->attribs;
    int id = 0;
    DATABREAK *b = (DATABREAK*)calloc(sizeof(DATABREAK), 1), **search = &dataBpList;
    while (*search)
        search = &(*search)->next;
    *search = b;
    while (attribs)
    {
        if (IsAttrib(attribs, "NAME"))
        {
            strncpy(b->name, attribs->value, 255);
        }
        else if (IsAttrib(attribs, "ACTIVE"))
        {
            b->active = !!atoi(attribs->value);
        }
        else if (IsAttrib(attribs, "DISABLE"))
        {
            b->disable = !!atoi(attribs->value);
        }
        attribs = attribs->next;
    }
}
//-------------------------------------------------------------------------

void RestoreProjectNames(struct xmlNode* node, int version, PROJECTITEM* wa)
{
    struct xmlNode* children = node->children;
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            PROJECTITEM** ins = &wa->children;
            PROJECTITEM* p = calloc(1, sizeof(PROJECTITEM));
            struct xmlAttr* attribs = children->attribs;
            char* q;
            if (!p)
                return;
            while (attribs)
            {
                if (IsAttrib(attribs, "NAME"))
                {
                    strcpy(p->realName, attribs->value);
                    abspath(p->realName, wa->realName);
                }
                else if (IsAttrib(attribs, "CLEAN"))
                {
                    p->clean = atoi(attribs->value);
                }
                attribs = attribs->next;
            }
            q = strrchr(p->realName, '\\');
            if (q)
                q++;
            else
                q = p->realName;
            strcpy(p->displayName, q);
            while (*ins && stricmp((*ins)->displayName, p->displayName) < 0)
                ins = &(*ins)->next;
            p->parent = wa;
            p->type = PJ_PROJ;
            p->next = *ins;
            *ins = p;
        }
        children = children->next;
    }
}
void RestoreProjectDependencies(struct xmlNode* node, int version, PROJECTITEM* wa)
{
    struct xmlNode *children = node->children, *dependsChildren;
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            char parentName[MAX_PATH], dependsName[MAX_PATH];
            struct xmlAttr* attribs = children->attribs;
            parentName[0] = 0;
            while (attribs)
            {
                if (IsAttrib(attribs, "NAME"))
                {
                    strcpy(parentName, attribs->value);
                    abspath(parentName, wa->realName);
                }
                attribs = attribs->next;
            }
            dependsChildren = children->children;
            while (dependsChildren)
            {
                attribs = dependsChildren->attribs;
                dependsName[0] = 0;
                while (attribs)
                {
                    if (IsAttrib(attribs, "NAME"))
                    {
                        strcpy(dependsName, attribs->value);
                        abspath(dependsName, wa->realName);
                    }
                    attribs = attribs->next;
                }
                if (parentName[0] && dependsName[0])
                {
                    PROJECTITEM *parent = wa->children, *depends = wa->children;
                    while (parent)
                    {
                        if (!xstricmpz(parent->realName, parentName))
                            break;
                        parent = parent->next;
                    }
                    while (depends)
                    {
                        if (!xstricmpz(depends->realName, dependsName))
                            break;
                        depends = depends->next;
                    }
                    if (parent && depends)
                    {
                        PROJECTITEMLIST** list = &parent->depends;
                        while (*list)
                            list = &(*list)->next;
                        *list = (PROJECTITEMLIST*)calloc(1, sizeof(PROJECTITEMLIST));
                        (*list)->item = depends;
                    }
                }
                dependsChildren = dependsChildren->next;
            }
        }
        children = children->next;
    }
}
void RestoreProfileNames(struct xmlNode* node, int version, PROJECTITEM* wa)
{
    char* selected = NULL;
    struct xmlAttr* attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "SELECTED"))
            selected = attribs->value;
        if (IsAttrib(attribs, "TYPE"))
            profileDebugMode = !strcmp(attribs->value, "DEBUG");
        attribs = attribs->next;
    }
    if (selected)
    {
        strcpy(currentProfileName, LookupProfileName(selected));
    }
    node = node->children;
    while (node)
    {
        if (IsNode(node, "NAME"))
        {
            struct xmlAttr* attribs = node->attribs;
            char* name = NULL;
            while (attribs)
            {
                if (IsAttrib(attribs, "VALUE"))
                    name = attribs->value;
                attribs = attribs->next;
            }
            if (name)
                LookupProfileName(name);
        }
        node = node->next;
    }
    SendDIDMessage(DID_PROJWND, WM_COMMAND, IDM_RESETPROFILECOMBOS, 0);
}

// this is in here because sometimes the IDE corrupts the docks portion of the file
// detect it on power up and leave the windows in their default state if it is corrupt
BOOLEAN ValidateDocks(struct xmlNode* node, int version)
{
    node = node->children;
    while (node)
    {
        if (IsNode(node, "DOCKWND"))
        {
            struct xmlAttr* attribs = node->attribs;
            char* name = NULL;

            while (attribs)
            {
                if (IsAttrib(attribs, "POS"))
                    if (strchr(attribs->value, '-'))
                    {
                        return FALSE;
                    }
                attribs = attribs->next;
            }
        }
        node = node->next;
    }
    return TRUE;
}
//-------------------------------------------------------------------------

PROJECTITEM* RestoreWorkArea(char* name)
{
    PROJECTITEM* wa;
    int version;
    FILE* in;
    char activeName[MAX_PATH], buf[MAX_PATH], *p;
    struct xmlNode* root;
    struct xmlNode* nodes;
    activeName[0] = 0;

    in = fopen(name, "r");
    if (!in)
    {
        return NULL;
    }
    root = xmlReadFile(in);
    fclose(in);
    if (!root || (!IsNode(root, "CC386WORKAREA") && !IsNode(root, "OCCWORKAREA")))
    {
        ExtendedMessageBox("Load Error", 0, "WorkArea is invalid file format");
        if (root)
            xmlFree(root);
        return 0;
    }
    wa = calloc(1, sizeof(PROJECTITEM));
    if (!wa)
    {
        xmlFree(root);
        ExtendedMessageBox("Load Error", 0, "Out of memory");
        return 0;
    }
    ClearProfileNames();
    wa->type = PJ_WS;
    strcpy(wa->realName, name);
    p = relpath(name, wa->realName);
    if (strrchr(p, '\\'))
        p = strrchr(p, '\\') + 1;
    strcpy(wa->displayName, p);

    TagEraseAllEntries();
    CloseAll();
    hbpInit();
    databpInit();
    nodes = root->children;
    while (nodes)
    {
        if (IsNode(nodes, "VERSION"))
        {
            struct xmlAttr* attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    version = atoi(attribs->value);
                attribs = attribs->next;
            }
        }
        else if (IsNode(nodes, "FLAGS"))
        {
            struct xmlAttr* attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "EXPANDED"))
                    wa->expanded = atoi(attribs->value);
                attribs = attribs->next;
            }
        }
        else if (IsNode(nodes, "DIALOG"))
        {
            struct xmlAttr* attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "GRIDSPACING"))
                    gridSpacing = atoi(attribs->value);
                if (IsAttrib(attribs, "SHOWGRID"))
                    showGrid = !!atoi(attribs->value);
                if (IsAttrib(attribs, "SNAPTOGRID"))
                    snapToGrid = !!atoi(attribs->value);
                attribs = attribs->next;
            }
            if (gridSpacing <= 0)
                gridSpacing = 4;
        }
        else if (IsNode(nodes, "PROFILELIST"))
            RestoreProfileNames(nodes, version, wa);
        else if (IsNode(nodes, "HISTORY"))
            RestoreHistory(nodes, version);
        else if (IsNode(nodes, "EDITWINDOWS"))
            RestoreWindows(nodes, version, wa);
        else if (IsNode(nodes, "TAG"))
            RestoreTags(nodes, version, wa);
        else if (IsNode(nodes, "FIND"))
            RestoreFind(nodes, version);
        else if (IsNode(nodes, "SIMPLEFIND"))
            RestoreSimpleFind(nodes, version);
        else if (IsNode(nodes, "WATCH"))
            RestoreWatch(nodes, version);
        else if (IsNode(nodes, "DATABP"))
            RestoreDataBreakpoints(nodes, version);
        else if (IsNode(nodes, "CHANGELN"))
            RestoreChangeLn(nodes, version, wa);
        else if (IsNode(nodes, "FILEBROWSE"))
            RestoreFileBrowse(nodes, version, wa);
        else if (IsNode(nodes, "PROPERTIES"))
            RestoreProps(nodes->children, wa, wa);
        else if (IsNode(nodes, "MEMWND"))
            RestoreMemoryWindowSettings(nodes, version);
        else if (IsNode(nodes, "ERRWND"))
            RestoreErrorWindowSettings(nodes, version);
        else if (IsNode(nodes, "TOOLBAR"))
            RestoreToolBars(nodes, version);
        else if (IsNode(nodes, "TOOLLAYOUT"))
            RestoreToolbarLayout(nodes, version);
        else if (IsNode(nodes, "DOCKS"))
        {
            if (ValidateDocks(nodes, version))
                RestoreDocks(nodes, version);
        }
        else if (IsNode(nodes, "PROJECTS"))
        {
            struct xmlAttr* attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "SELECTED"))
                {
                    strcpy(activeName, attribs->value);
                }
                attribs = attribs->next;
            }
            RestoreProjectNames(nodes, version, wa);
            RestoreProjectDependencies(nodes, version, wa);
        }
        nodes = nodes->next;
    }
    xmlFree(root);
    sprintf(buf, "%s.user", name);
    in = fopen(buf, "r");
    if (in)
    {
        root = xmlReadFile(in);
        fclose(in);
        if (!root || !IsNode(root, "OCCWORKAREAUSERPREF"))
        {
            if (root)
                xmlFree(root);
            root = NULL;
        }
        if (root)
        {
            nodes = root->children;
            while (nodes)
            {
                if (IsNode(nodes, "VERSION"))
                {
                    struct xmlAttr* attribs = nodes->attribs;
                    while (attribs)
                    {
                        if (IsAttrib(attribs, "ID"))
                            version = atoi(attribs->value);
                        attribs = attribs->next;
                    }
                }
                else if (IsNode(nodes, "FLAGS"))
                {
                    struct xmlAttr* attribs = nodes->attribs;
                    while (attribs)
                    {
                        if (IsAttrib(attribs, "EXPANDED"))
                            wa->expanded = atoi(attribs->value);
                        attribs = attribs->next;
                    }
                }
                else if (IsNode(nodes, "DIALOG"))
                {
                    struct xmlAttr* attribs = nodes->attribs;
                    while (attribs)
                    {
                        if (IsAttrib(attribs, "GRIDSPACING"))
                            gridSpacing = atoi(attribs->value);
                        if (IsAttrib(attribs, "SHOWGRID"))
                            showGrid = !!atoi(attribs->value);
                        if (IsAttrib(attribs, "SNAPTOGRID"))
                            snapToGrid = !!atoi(attribs->value);
                        attribs = attribs->next;
                    }
                    if (gridSpacing <= 0)
                        gridSpacing = 4;
                }
                else if (IsNode(nodes, "PROFILELIST"))
                    RestoreProfileNames(nodes, version, wa);
                else if (IsNode(nodes, "HISTORY"))
                    RestoreHistory(nodes, version);
                else if (IsNode(nodes, "EDITWINDOWS"))
                    RestoreWindows(nodes, version, wa);
                else if (IsNode(nodes, "TAG"))
                    RestoreTags(nodes, version, wa);
                else if (IsNode(nodes, "FIND"))
                    RestoreFind(nodes, version);
                else if (IsNode(nodes, "SIMPLEFIND"))
                    RestoreSimpleFind(nodes, version);
                else if (IsNode(nodes, "WATCH"))
                    RestoreWatch(nodes, version);
                else if (IsNode(nodes, "DATABP"))
                    RestoreDataBreakpoints(nodes, version);
                else if (IsNode(nodes, "CHANGELN"))
                    RestoreChangeLn(nodes, version, wa);
                else if (IsNode(nodes, "FILEBROWSE"))
                    RestoreFileBrowse(nodes, version, wa);
                else if (IsNode(nodes, "PROPERTIES"))
                    RestoreProps(nodes->children, wa, wa);
                else if (IsNode(nodes, "MEMWND"))
                    RestoreMemoryWindowSettings(nodes, version);
                else if (IsNode(nodes, "ERRWND"))
                    RestoreErrorWindowSettings(nodes, version);
                else if (IsNode(nodes, "TOOLBAR"))
                    RestoreToolBars(nodes, version);
                else if (IsNode(nodes, "TOOLLAYOUT"))
                    RestoreToolbarLayout(nodes, version);
                else if (IsNode(nodes, "DOCKS"))
                {
                    if (ValidateDocks(nodes, version))
                        RestoreDocks(nodes, version);
                }
                nodes = nodes->next;
            }
            xmlFree(root);
        }
    }
    if (activeName[0])
    {
        PROJECTITEM* project = wa->children;
        while (project)
        {
            int n = strlen(activeName);
            if (n <= strlen(project->displayName))
            {
                if (!strnicmp(project->displayName, activeName, n))
                {
                    if (project->displayName[n] == '.')
                        activeProject = project;
                    break;
                }
            }
            project = project->next;
        }
    }
    SendMessage(hwndToolbarBar, WM_REDRAWTOOLBAR, 0, 0);
    FreeJumpSymbols();
    LoadJumpSymbols();
    PostDIDMessage(DID_BREAKWND, WM_RESTACK, 0, 0);
    PostDIDMessage(DID_BROWSEWND, WM_RESETHISTORY, 0, 0);
    PostDIDMessage(DID_MEMWND, WM_RESETHISTORY, 0, 0);
    PostDIDMessage(DID_MEMWND + 1, WM_RESETHISTORY, 0, 0);
    PostDIDMessage(DID_MEMWND + 2, WM_RESETHISTORY, 0, 0);
    PostDIDMessage(DID_MEMWND + 3, WM_RESETHISTORY, 0, 0);
    return wa;
}

//-------------------------------------------------------------------------

void onehistsave(FILE* out, char** hist, char* name)
{
    int i;
    fprintf(out, "\t<HISTORY TYPE=\"%s\">\n", name);
    for (i = 0; i < MAX_COMBO_HISTORY; i++)
        if (hist[i])
            fprintf(out, "\t\t<HISTITEM VALUE=\"%s\"/>\n", xmlConvertString(hist[i], TRUE));
    fprintf(out, "\t</HISTORY>\n");
}

//-------------------------------------------------------------------------

void SaveHistory(FILE* out)
{
    onehistsave(out, findhist, "FIND");
    onehistsave(out, findbrowsehist, "BFINDPATH");
    onehistsave(out, replacehist, "REPLACE");
    onehistsave(out, replacebrowsehist, "BREPLACEPATH");
    onehistsave(out, watchhist, "WATCH");
    onehistsave(out, funcbphist, "FUNCBP");
    onehistsave(out, databphist, "DATABP");
    onehistsave(out, memhist, "MEMORY");
    onehistsave(out, varinfohist, "VARINFO");
}

//-------------------------------------------------------------------------

void SaveChangeLn(FILE* out, PROJECTITEM* wa)
{
    struct tagfile* list = tagFileList;
    fprintf(out, "\t<CHANGELN>\n");
    while (list)
    {
        if (list->changedLines)
        {
            struct tagchangeln* data = list->changedLines;
            fprintf(out, "\t\t<FILE NAME=\"%s\">\n", relpath(list->name, wa->realName));
            while (data)
            {
                fprintf(out, "\t\t\t<LINE NUM=\"%d\" DELTA=\"%d\"/>\n", data->lineno, data->delta);
                data = data->next;
            }
            fprintf(out, "\t\t</FILE>\n");
        }
        list = list->next;
    }
    fprintf(out, "\t</CHANGELN>\n");
}
void SaveFileBrowse(FILE* out, PROJECTITEM* wa)
{
    FILEBROWSE* p = fileBrowseInfo;
    fprintf(out, "\t<FILEBROWSE>\n");
    while (p)
    {
        fprintf(out, "\t\t<FILE NAME=\"%s\" LINE=\"%d\"/>\n", relpath(p->info->dwName, wa->realName), p->lineno);
        p = p->next;
    }
    fprintf(out, "\t</FILEBROWSE>\n");
}
//-------------------------------------------------------------------------

void saveonetag(FILE* out, int tag, PROJECTITEM* wa)
{
    struct tagfile* list = tagFileList;
    fprintf(out, "\t<TAG ID=\"%d\">\n", tag);
    while (list)
    {
        if (list->tagArray[tag])
        {
            struct tag* data = list->tagArray[tag];
            fprintf(out, "\t\t<FILE NAME=\"%s\">\n", relpath(list->name, wa->realName));
            while (data)
            {
                fprintf(out, "\t\t\t<TAGITEM LINENO=\"%d\" CP=\"%d\" ENABLED=\"%d\" DISABLED=\"%d\"", data->drawnLineno,
                        data->charpos, data->enabled, data->disable);

                if (tag == TAG_BOOKMARK)
                    fprintf(out, ">\n%s\n\t\t\t</TAGITEM>\n", xmlConvertString(data->extra, FALSE));
                else
                    fprintf(out, "/>\n");
                data = data->next;
            }
            fprintf(out, "\t\t</FILE>\n");
        }
        list = list->next;
    }
    fprintf(out, "\t</TAG>\n");
}

//-------------------------------------------------------------------------

void SaveTags(FILE* out, PROJECTITEM* wa)
{
    saveonetag(out, TAG_BP, wa);
    saveonetag(out, TAG_BOOKMARK, wa);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void SaveWindows(FILE* out, PROJECTITEM* wa)
{
    int count;
    int i;
    HWND wnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
    DWINFO* info = (DWINFO*)GetWindowLong(wnd, 0);
    fprintf(out, "\t<EDITWINDOWS>\n");
    for (i = 0; i < 10000; i++)
        if (!SendMessage(hwndSrcTab, TABM_GETITEM, i, 0))
            break;
    count = i;
    for (i = count - 1; i >= 0; i--)
    {
        HWND hwnd = (HWND)SendMessage(hwndSrcTab, TABM_GETITEM, i, 0);
        if (IsEditWindow(hwnd))
        {
            DWINFO* ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            EDITDATA* dt = (EDITDATA*)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0);
            POINT p;
            int startpos;
            WINDOWPLACEMENT placement;
            CHARRANGE a;
            SendMessage(ptr->dwHandle, EM_EXGETSEL, 0, (WPARAM)&a);
            startpos = SendMessage(ptr->dwHandle, EM_EXLINEFROMCHAR, 0, a.cpMin);
            placement.length = sizeof(placement);
            GetWindowPlacement(ptr->self, &placement);
            p.x = placement.rcNormalPosition.left;
            p.y = placement.rcNormalPosition.top;
            fprintf(
                out,
                "\t\t<WINDOW NAME=\"%s\" TITLE=\"%s\" X=\"%d\" Y=\"%d\" WIDTH=\"%d\" HEIGHT=\"%d\" LINENO=\"%d\" COPYID=\"%d\"/>\n",
                relpath(ptr->dwName, wa->realName), ptr->dwTitle, p.x, p.y,
                placement.rcNormalPosition.right - placement.rcNormalPosition.left,
                placement.rcNormalPosition.bottom - placement.rcNormalPosition.top, startpos + 1,
                dt->cd->siblings ? dt->id + 1 : 0);
        }
    }
    // must be last
    if (IsEditWindow(wnd))
    {
        fprintf(out, "\t\t<ACTIVE NAME=\"%s\" TITLE=\"%s\"/>\n", info->dwName, info->dwTitle);
    }
    fprintf(out, "\t</EDITWINDOWS>\n");
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void SaveWatchpoints(FILE* out)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        fprintf(out, "\t<WATCH ID=\"%d\" ACTIVE=\"%d\" DISABLE=\"%d\" NAME=\"%s\" MODE=\"%d\" SIZE=\"%d\"/>\n", i + 1,
                !!(hdwebp[i].active), !!(hdwebp[i].disable), xmlConvertString(hdwebp[i].name, TRUE), hdwebp[i].mode,
                hdwebp[i].size);
    }
}

void SaveDataBreakpoints(FILE* out)
{
    DATABREAK* search = dataBpList;
    while (search)
    {
        fprintf(out, "\t<DATABP NAME=\"%s\" ACTIVE=\"%d\" DISABLE=\"%d\"/>\n", search->name, search->active, search->disable);
        search = search->next;
    }
}
void SaveProjectNames(FILE* out, PROJECTITEM* wa)
{
    PROJECTITEM* l = wa->children;
    fprintf(out, "\t<PROJECTS SELECTED=\"%s\">\n", activeProject ? activeProject->displayName : "");
    while (l)
    {
        if (l->depends)
        {
            PROJECTITEMLIST* depends = l->depends;
            fprintf(out, "\t\t<FILE NAME=\"%s\" CLEAN=\"%x\">\n", relpath(l->realName, wa->realName), l->clean);
            while (depends)
            {
                fprintf(out, "\t\t\t<DEPENDENCY NAME=\"%s\" />\n", relpath(depends->item->realName, wa->realName));
                depends = depends->next;
            }
            fprintf(out, "\t\t</FILE>\n");
        }
        else
        {
            fprintf(out, "\t\t<FILE NAME=\"%s\" CLEAN=\"%x\"/>\n", relpath(l->realName, wa->realName), l->clean);
        }
        l = l->next;
    }
    fprintf(out, "\t</PROJECTS>\n");
}
void SaveToolBarA(FILE* out, HWND hwnd)
{
    char data[512];
    int id = GetToolBarData(hwnd, data);  // undefined in local context
    fprintf(out, "\t<TOOLBAR ID=\"%d\" DEFINE=\"%s\"/>\n", id, data);
}

//-------------------------------------------------------------------------

void SaveToolBars(FILE* out)
{
    int i;
    fprintf(out, "\t<TOOLLAYOUT COUNT=\"%d\">\n", toolCount);
    for (i = 0; i < toolCount; i++)
    {
        int n = toolLayout[i];
        if (n > 0)
        {
            HWND h = GetToolWindow(n);
            if (!IsWindowVisible(h))
                n += 10000;
        }
        fprintf(out, " %d ", n);
    }
    fprintf(out, "\n\t</TOOLLAYOUT>\n");
    SaveToolBarA(out, hwndToolNav);
    SaveToolBarA(out, hwndToolBuildType);
    SaveToolBarA(out, hwndToolEdit);
    SaveToolBarA(out, hwndToolBuild);
    SaveToolBarA(out, hwndToolDebug);
    SaveToolBarA(out, hwndToolBookmark);
    SaveToolBarA(out, hwndToolThreads);
}
static void SaveDock(FILE* out, HWND hwnd, RECT* r, int ctl, int type, int index)
{
    int sel;
    int *list, *list1;
    HWND xx = (HWND)SendMessage(hwnd, TW_GETACTIVE, 0, 0);
    for (sel = 0; sel < MAX_HANDLES; sel++)
        if (handles[sel] == xx)
            break;
    fprintf(out, "\t\t<DOCKWND TYPE=\"%d\" INDEX=\"%d\" SEL=\"%d\" CTL=\"%d\" POS=\"%d %d %d %d\">\n", type, index, sel, ctl,
            r->left, r->top, r->right - r->left, r->bottom - r->top);
    fprintf(out, "\t\t\t");
    list1 = list = (int*)SendMessage(hwnd, WM_GETDOCKLIST, 0, 0);
    while (list && *list)
        fprintf(out, "%d ", *list++);
    free(list1);
    fprintf(out, "\n\t\t</DOCKWND>\n");
}
void SaveDocks(FILE* out)
{
    int i;
    Select(0);
    fprintf(out, "\t<DOCKS>\n");
    fprintf(out, "\t\t<FRAME POS=\"%d %d %d %d\"/>\n", oldFrame.left, oldFrame.top, oldFrame.right - oldFrame.left,
            oldFrame.bottom - oldFrame.top);
    for (i = 0; i < debugDockCount; i++)
        SaveDock(out, debugDocks[i].hwnd, &debugDocks[i].r, debugDocks[i].ctl, 0, i);
    for (i = 0; i < releaseDockCount; i++)
        SaveDock(out, releaseDocks[i].hwnd, &releaseDocks[i].r, releaseDocks[i].ctl, 1, i);
    for (i = 0; i < debugfreeWindowCount; i++)
    {
        RECT r;
        GetWindowRect(debugfreeWindows[i], &r);
        MapWindowPoints(HWND_DESKTOP, hwndFrame, (LPPOINT)&r, 2);
        SaveDock(out, debugfreeWindows[i], &r, 0, 2, i);
    }
    for (i = 0; i < releasefreeWindowCount; i++)
    {
        RECT r;
        GetWindowRect(releasefreeWindows[i], &r);
        MapWindowPoints(HWND_DESKTOP, hwndFrame, (LPPOINT)&r, 2);
        SaveDock(out, releasefreeWindows[i], &r, 0, 3, i);
    }
    fprintf(out, "\t</DOCKS>\n");
}
void SaveWorkArea(PROJECTITEM* wa)
{
    FILE* out;
    PROFILENAMELIST* pf;
    char buf[MAX_PATH];
    if (PropGetBool(NULL, "BACKUP_PROJECTS"))
        backupFile(wa->realName);
    out = fopen(wa->realName, "w");
    if (!out)
    {
        ExtendedMessageBox("Save Error", 0, "Could not save WorkArea");
        return;
    }
    fprintf(out, "<OCCWORKAREA>\n");
    fprintf(out, "\t<VERSION ID=\"%d\"/>\n", WSPVERS);
    SaveProjectNames(out, wa);
    fprintf(out, "</OCCWORKAREA>\n");
    fclose(out);

    sprintf(buf, "%s.user", wa->realName);
    if (PropGetBool(NULL, "BACKUP_PROJECTS"))
        backupFile(buf);
    out = fopen(buf, "w");
    if (!out)
    {
        ExtendedMessageBox("Save Error", 0, "Could not save WorkArea");
        return;
    }
    fprintf(out, "<OCCWORKAREAUSERPREF>\n");
    fprintf(out, "\t<VERSION ID=\"%d\"/>\n", WSPVERS);
    fprintf(out, "\t<FLAGS EXPANDED=\"%d\"/>\n", wa->expanded ? 1 : 0);
    fprintf(out, "\t<PROFILELIST SELECTED=\"%s\" TYPE=\"%s\">\n", currentProfileName, profileDebugMode ? "DEBUG" : "RELEASE");
    fprintf(out, "\t\t<NAME VALUE=\"%s\"/>\n", sysProfileName);
    pf = profileNames;
    while (pf)
    {
        fprintf(out, "\t\t<NAME VALUE=\"%s\"/>\n", pf->name);
        pf = pf->next;
    }
    fprintf(out, "\t</PROFILELIST>\n");
    fprintf(out, "\t<DIALOG GRIDSPACING=\"%d\" SHOWGRID=\"%d\" SNAPTOGRID=\"%d\"/>\n", gridSpacing, showGrid, snapToGrid);

    fprintf(out, "\t<FIND X=\"%d\" Y=\"%d\"", findDlgPos.x, findDlgPos.y);
    fprintf(out, "\n\t\tFF=\"%d\" RF=\"%d\" FM=\"%d\" RM=\"%d\" FE=\"%d\" RE=\"%d\"/>\n", findflags, replaceflags, findmode,
            replacemode, findext, replaceext);
    fprintf(out, "\t<SIMPLEFIND FLAGS=\"%d\"/>\n", simpleFindFlags);
    SaveWatchpoints(out);
    fprintf(out, "\t<MEMWND WORDSIZE=\"%d\"/>\n", memoryWordSize);
    fprintf(out, "\t<ERRWND BTNS=\"%d\"/>\n", errorButtons);
    SaveDataBreakpoints(out);
    SaveWindows(out, wa);
    SaveHistory(out);
    SaveTags(out, wa);
    SaveChangeLn(out, wa);
    SaveFileBrowse(out, wa);
    SaveProfiles(out, wa, 1);
    SaveToolBars(out);
    SaveDocks(out);

    fprintf(out, "</OCCWORKAREAUSERPREF>\n");
    fclose(out);
    wa->changed = FALSE;
}
