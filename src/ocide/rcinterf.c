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
#include <setjmp.h>

#include "header.h"
#include "rcgui.h"
#include "img.h"

#define ID_NEW 100
#define ID_DELETE 101

#define N_EDITDONE -4001

extern HWND hwndSrcTab;
extern HWND hwndClient;
extern HWND hwndFrame;
extern char szInstallPath[];
extern PROJECTITEM* activeProject;
extern HTREEITEM prjSelectedItem;
extern HINSTANCE hInstance;
extern HIMAGELIST treeIml;
extern PROJECTITEM* workArea;
extern char* rcSearchPath;
extern char szBitmapFilter[];
extern char szCursorFilter[];
extern char szIconFilter[];
extern char szFontFilter[];
extern char szMessageTableFilter[];
extern jmp_buf errjump;
extern LOGFONT systemDialogFont;
extern struct propertyFuncs imgFuncs;
extern char sztreeDoubleBufferName[];

char* rcSearchPath;
RESOURCE_DATA* currentResData;

static HWND hwndRes;
static char* szresEditClassName = "xccresEditClass";
static PROJECTITEM* editItem;
static HWND hwndEdit;
static HTREEITEM resSelection;
static struct resRes* rclickItem;
static HTREEITEM rclickSelection;
static HFONT projFont, boldProjFont, italicProjFont;

static HWND treeWindow;
static char* szResourceClassName = "xccResource";
static char* szResourceTitle = "Resources";
static char srchbuf[1024];
static WNDPROC oldEditProc;
static LIST* resourceWindows;

static char* rcContents = {
    "#include <windows.h>\n"
    "#include <commctrl.h>\n"
    "#include \"%s\"\n\n"
    "LANGUAGE LANG_ENGLISH,SUBLANG_ENGLISH_US\n\n"};
static char* headerContents = {
    "#ifdef __IDE_RC_INVOKED\n"
    "#define __NEXT_CONTROL_ID\t1\n"
    "#define __NEXT_MENU_ID\t\t10000\n"
    "#define __NEXT_RESOURCE_ID\t10\n"
    "#define __NEXT_STRING_ID\t1000\n"
    "#endif\n\n"};
static char* DirNames[] = {
    "", "Accelerators", "Bitmaps",        "Cursors", "Dialogs",       "DlgInclude", "Fonts",        "Icons",
    "", "Menus",        "Message Tables", "RCdata",  "String Tables", "Userdata",   "Version Info",
};

static struct
{
    char* name;
    char* resPrefix;
    int type;
} AddNames[] = {
    {"Acclerator", "IDA_ACCELERATOR", RESTYPE_ACCELERATOR},
    {"Icon", "IDI_ICON", RESTYPE_ICON},
    {"Cursor", "IDC_CURSOR", RESTYPE_CURSOR},
    {"Bitmap", "IDB_BITMAP", RESTYPE_BITMAP},
    {"Dialog", "IDD_DIALOG", RESTYPE_DIALOG},
    {"Font", "IDF_FONT", RESTYPE_FONT},
    {"Menu", "IDM_MENU", RESTYPE_MENU},
    {"Message Table", "IDT_MESSAGETABLE", RESTYPE_MESSAGETABLE},
    {"Version", "VS_VERSION_INFO", RESTYPE_VERSION},
    {"String", "IDS_STRING", RESTYPE_STRING},
    {"RCData", "IDR_RCDATA", RESTYPE_RCDATA},
};

static struct resRes dummies[sizeof(DirNames) / sizeof(DirNames[0])];

static PROJECTITEM* GetItemInfo(HTREEITEM item);
void InsertRCWindow(HWND hwnd)
{
    LIST* l = malloc(sizeof(LIST));
    l->data = (void*)hwnd;
    l->next = resourceWindows;
    resourceWindows = l;
}
void RemoveRCWindow(HWND hwnd)
{
    LIST** l = &resourceWindows;
    while (*l)
    {
        if ((*l)->data == (void*)hwnd)
        {
            LIST* q = *l;
            *l = (*l)->next;
            free(q);
            break;
        }
        l = &(*l)->next;
    }
}
void CloseAllResourceWindows(void)
{
    MSG msg;
    while (resourceWindows)
    {
        LIST* l = resourceWindows;
        HWND hwnd = (HWND)l->data;
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        // the close will get rid of the item from the list
    }
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        ProcessMessage(&msg);
        if (msg.message == WM_QUIT)
            break;
    }
    resourceWindows = NULL;
}
static void GetProjectPath(char* buf)
{
    PROJECTITEM* data = GetItemInfo(prjSelectedItem);
    if (!data)
    {
        data = activeProject;
    }
    else
    {
        while (data->type == PJ_FOLDER)
            data = data->parent;
    }

    if (data)
    {
        char* p;
        strcpy(buf, data->realName);
        p = strrchr(buf, '\\');
        if (p)
        {
            p[1] = 0;
            return;
        }
    }
    GetDefaultProjectsPath(buf);
}
static void SelectHeaderName(char* dest)
{
    char path[MAX_PATH];
    int i = 0;
    GetProjectPath(path);
    while (TRUE)
    {
        FILE* fil;
        if (i == 0)
        {
            sprintf(dest, "%sResource.h", path);
            i++;
        }
        else
        {
            sprintf(dest, "%sResource%d.h", path, i++);
        }
        if (!(fil = fopen(dest, "rb")))
        {
            return;
        }
        fclose(fil);
    }
}
void CreateNewResourceFile(PROJECTITEM* data, char* name, int flag)
{
    char rc[MAX_PATH];
    char hdr[MAX_PATH];
    char ppath[MAX_PATH];
    FILE* fil;
    GetProjectPath(ppath);
    strcpy(rc, name);
    abspath(rc, ppath);
    fil = fopen(rc, "wb");
    if (fil)
    {
        char* p;
        SelectHeaderName(hdr);
        p = strrchr(hdr, '\\');
        if (p)
            p++;
        else
            p = hdr;
        fprintf(fil, rcContents, p);
        fclose(fil);
        fil = fopen(hdr, "wb");
        if (fil)
        {
            fprintf(fil, "%s", headerContents);
            fclose(fil);
        }
        AddFile(data, rc, flag);
        AddFile(data, hdr, flag);
    }
}
static char* FormatExpInternal(char* buf, EXPRESSION* p, int n)
{
    // well -1 is the outer layer, well this tries to omit the outer parenthesis
    // but it isn't perfect
    buf += strlen(buf);
    if (n == -1)
        n = p->type;
    switch (p->type)
    {
        case hook:
            *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, hook);
            strcpy(buf, " ? ");
            buf = FormatExpInternal(buf, p->right->left, hook);
            strcpy(buf, " : ");
            buf = FormatExpInternal(buf, p->left->left, hook);
            *buf++ = ')';
            *buf = 0;
            break;
        case land:
            buf = FormatExpInternal(buf, p->left, land);
            strcpy(buf, " && ");
            buf = FormatExpInternal(buf, p->right, land);
            *buf = 0;
            break;
        case lor:
            if (n != lor)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, lor);
            strcpy(buf, " || ");
            buf = FormatExpInternal(buf, p->right, lor);
            if (n != lor)
                *buf++ = ')';
            *buf = 0;
            break;
        case or:
            if (n != or)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, or);
            strcpy(buf, " | ");
            buf = FormatExpInternal(buf, p->right, or);
            if (n != or)
                *buf++ = ')';
            *buf = 0;
            break;
        case uparrow:
            if (n != uparrow)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, uparrow);
            strcpy(buf, " ^ ");
            buf = FormatExpInternal(buf, p->right, uparrow);
            if (n != uparrow)
                *buf++ = ')';
            *buf = 0;
            break;
        case and:
            if (n != and)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, and);
            strcpy(buf, " & ");
            buf = FormatExpInternal(buf, p->right, and);
            if (n != and)
                *buf++ = ')';
            *buf = 0;
            break;
        case eq:
            if (n != eq)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, eq);
            strcpy(buf, " == ");
            buf = FormatExpInternal(buf, p->right, eq);
            if (n != eq)
                *buf++ = ')';
            *buf = 0;
            break;
        case neq:
            if (n != eq)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, eq);
            strcpy(buf, " != ");
            buf = FormatExpInternal(buf, p->right, eq);
            if (n != eq)
                *buf++ = ')';
            *buf = 0;
            break;
        case lt:
            if (n != lt)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, lt);
            strcpy(buf, " < ");
            buf = FormatExpInternal(buf, p->right, lt);
            if (n != lt)
                *buf++ = ')';
            *buf = 0;
            break;
        case gt:
            if (n != lt)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, lt);
            strcpy(buf, " > ");
            buf = FormatExpInternal(buf, p->right, lt);
            if (n != lt)
                *buf++ = ')';
            *buf = 0;
            break;
        case leq:
            if (n != lt)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, lt);
            strcpy(buf, " <= ");
            buf = FormatExpInternal(buf, p->right, lt);
            if (n != lt)
                *buf++ = ')';
            *buf = 0;
            break;
        case geq:
            if (n != lt)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, lt);
            strcpy(buf, " >= ");
            buf = FormatExpInternal(buf, p->right, lt);
            if (n != lt)
                *buf++ = ')';
            *buf = 0;
            break;
        case lshift:
            if (n != lshift)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, lshift);
            strcpy(buf, " << ");
            buf = FormatExpInternal(buf, p->right, lshift);
            if (n != lshift)
                *buf++ = ')';
            *buf = 0;
            break;
        case rshift:
            if (n != rshift)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, rshift);
            strcpy(buf, " >> ");
            buf = FormatExpInternal(buf, p->right, rshift);
            if (n != rshift)
                *buf++ = ')';
            *buf = 0;
            break;
        case plus:
            if (n != plus)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, plus);
            strcpy(buf, " + ");
            buf = FormatExpInternal(buf, p->right, plus);
            if (n != plus)
                *buf++ = ')';
            *buf = 0;
            break;
        case minus:
            if (p->right)
            {
                if (n != minus)
                    *buf++ = '(';
                *buf = 0;
                buf = FormatExpInternal(buf, p->left, plus);
                strcpy(buf, " - ");
                buf = FormatExpInternal(buf, p->right, plus);
                if (n != plus)
                    *buf++ = ')';
            }
            else
            {
                *buf++ = '-';
                *buf = 0;
                buf = FormatExpInternal(buf, p->left, minus);
            }
            *buf = 0;
            break;
        case star:
            if (n != star)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, star);
            strcpy(buf, " * ");
            buf = FormatExpInternal(buf, p->right, star);
            if (n != star)
                *buf++ = ')';
            *buf = 0;
            break;
        case divide:
            if (n != star)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, star);
            strcpy(buf, " / ");
            buf = FormatExpInternal(buf, p->right, star);
            if (n != star)
                *buf++ = ')';
            *buf = 0;
            break;
        case modop:
            if (n != star)
                *buf++ = '(';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, star);
            strcpy(buf, " % ");
            buf = FormatExpInternal(buf, p->right, star);
            if (n != star)
                *buf++ = ')';
            *buf = 0;
            break;
        case not:
            *buf++ = '!';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, not);
            *buf = 0;
            break;
        case kw_not:
            strcpy(buf, " NOT ");
            buf = FormatExpInternal(buf, p->left, not);
            *buf = 0;
            break;
        case compl:
            *buf++ = '~';
            *buf = 0;
            buf = FormatExpInternal(buf, p->left, compl);
            *buf = 0;
            break;
        case e_int:
            if (p->rendition)
                sprintf(buf, "%s", p->rendition);
            else
                sprintf(buf, "%d", p->val);
            break;
        default:
            break;
    }

    return buf + strlen(buf);
}
void FormatExp(char* buf, EXPRESSION* exp)
{
    if (!exp)
        sprintf(buf, "0");
    else
    {
        FormatExpInternal(buf, exp, -1);
    }
}
void FormatResId(char* buf, IDENT* id)
{
    if (id->symbolic)
    {
        if (id->origName)
        {
            StringWToA(buf, id->origName, wcslen(id->origName));
        }
        else
        {
            char nbuf[256];
            StringWToA(nbuf, id->u.n.symbol, id->u.n.length);
            sprintf(buf, "%s", nbuf);
        }
    }
    else
    {
        FormatExp(buf, id->u.id);
    }
}
static HTREEITEM InsertResourceTreeItem(TV_INSERTSTRUCT* t)
{
    t->UNNAMED_UNION item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    t->UNNAMED_UNION item.hItem = 0;
    t->UNNAMED_UNION item.cchTextMax = strlen(t->UNNAMED_UNION item.pszText);
    t->UNNAMED_UNION item.iImage = t->UNNAMED_UNION item.iSelectedImage = IL_RC;
    return TreeView_InsertItem(treeWindow, t);
}
void ResShowSubTree(PROJECTITEM* file)
{
    if (file && file->resData)
    {
        struct resDir* dirs = file->resData->dirs;
        while (dirs)
        {
            struct resRes* res = dirs->children;
            TV_INSERTSTRUCT t;
            memset(&t, 0, sizeof(t));
            t.hParent = file->hResTreeItem;
            t.hInsertAfter = TVI_LAST;
            t.UNNAMED_UNION item.pszText = DirNames[dirs->type];
            dummies[dirs->type].type = PJ_RESMENU + dirs->type;
            t.UNNAMED_UNION item.lParam = (LPARAM)&dummies[dirs->type];
            dirs->hTreeItem = InsertResourceTreeItem(&t);
            while (res)
            {
                IDENT* id = &res->resource->id;
                memset(&t, 0, sizeof(t));
                t.hParent = dirs->hTreeItem;
                t.hInsertAfter = TVI_LAST;
                t.UNNAMED_UNION item.pszText = res->name;
                t.UNNAMED_UNION item.lParam = (LPARAM)res;
                res->hTreeItem = InsertResourceTreeItem(&t);
                res = res->next;
            }
            dirs = dirs->next;
        }
    }
}
struct resDir* FindResDir(struct resData* data, int type)
{
    struct resDir** dirs = &data->dirs;
    while (*dirs && type > (*dirs)->type)
        dirs = &(*dirs)->next;
    if (!*dirs || (*dirs)->type != type)
    {
        struct resDir* newDir = calloc(1, sizeof(struct resDir));
        newDir->type = type;
        newDir->next = *dirs;
        *dirs = newDir;
    }
    return *dirs;
}
void ResCreateSubTree(struct resData* data)
{
    RESOURCE* resource = data->resources->resources;
    while (resource)
    {
        if (resource->itype != RESTYPE_LANGUAGE && resource->itype != RESTYPE_PLACEHOLDER)
        {
            struct resDir* dir = FindResDir(data, resource->itype);
            struct resRes** res = &dir->children;
            struct resRes* newRes = calloc(1, sizeof(struct resRes));
            char buf[512];
            memset(buf, 0, sizeof(buf));
            FormatResId(buf, &resource->id);
            newRes->resource = resource;
            newRes->name = strdup(buf);
            newRes->type = PJ_RES;
            while (*res && strcmp(newRes->name, (*res)->name) > 0)
            {
                res = &(*res)->next;
            }
            newRes->next = *res;
            *res = newRes;
        }
        resource = resource->next;
    }
}
void ResFreeSubTree(struct resData* data)
{
    struct resDir* dirs = data->dirs;
    while (dirs)
    {
        struct resDir* next = dirs->next;
        struct resRes* res = dirs->children;
        while (res)
        {
            struct resRes* next = res->next;
            if (res->activeHwnd)
            {
                SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM)res->activeHwnd, 0);
            }
            res = next;
        }
        free(dirs);
        dirs = next;
    }
}
struct resData* ResLoad(char* name)
{
    struct resData* data = calloc(1, sizeof(struct resData));
    data->resources = ReadResources(name);
    if (!data->resources)
    {
        free(data);
        data = NULL;
    }
    return data;
}
void ResRewriteTitle(struct resRes* res, BOOL dirty)
{
    char buf[512], obuf[512];
    GetWindowText(res->activeHwnd, obuf, 512);
    if (dirty)
    {
        sprintf(buf, "%s *", res->name);
    }
    else
    {
        strcpy(buf, res->name);
    }
    SetWindowText(res->activeHwnd, buf);
    SendMessage(res->activeHwnd, WM_NCPAINT, 1, 0);
    SendMessage(hwndSrcTab, TABM_RENAME, (WPARAM)buf, (LPARAM)res->activeHwnd);
}
void ResSetDirty(struct resRes* res)
{
    if (!res->resource->changed)
    {
        ResRewriteTitle(res, TRUE);
        res->resource->changed = TRUE;
    }
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
}
void ResSetClean(struct resRes* res)
{
    if (res->resource->changed && !res->resource->deleted)
    {
        ResRewriteTitle(res, FALSE);
        res->resource->changed = FALSE;
    }
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
}
void ResSave(char* name, struct resData* data)
{
    struct resDir* dirs = data->dirs;
    while (dirs)
    {
        struct resRes* res = dirs->children;
        while (res)
        {
            if (res->activeHwnd)
            {
                SendMessage(res->activeHwnd, WM_RESCOMMIT, 0, 0);
            }
            res->gd.cantClearUndo = !!res->activeHwnd;
            res = res->next;
        }
        dirs = dirs->next;
    }
    WriteResources(name, data->resources);
    FileTime(&data->time, name);
    dirs = data->dirs;
    while (dirs)
    {
        struct resRes* res = dirs->children;
        while (res)
        {
            if (res->activeHwnd)
            {
                if (res->resource->changed)
                {
                    ResSetClean(res);
                }
            }
            if (res->resource)
                res->resource->changed = FALSE;  // force clean in case deleted
            res = res->next;
        }
        dirs = dirs->next;
    }
}
HANDLE ResGetHeap(PROJECTITEM* pj, struct resRes* data)
{
    currentResData = NULL;
    if (pj->type == PJ_FILE)
    {
        if (pj->resData)
        {
            struct resDir* dirs = pj->resData->dirs;
            while (dirs)
            {
                struct resRes* children = dirs->children;
                while (children)
                {
                    if (children == data)
                    {
                        currentResData = pj->resData->resources;
                        SetRCMallocScope(pj->resData->resources);
                        return pj->resData->resources->memHeap;
                    }
                    children = children->next;
                }
                dirs = dirs->next;
            }
        }
    }
    else
    {
        PROJECTITEM* cur = pj->children;
        HANDLE heap;  // reuse instead of allocating again and again.
        while (cur)
        {
            heap = ResGetHeap(cur, data);
            if (heap)
                return heap;
            cur = cur->next;
        }
    }
    return FALSE;
}

BOOL ResSaveCurrent(PROJECTITEM* pj, struct resRes* res)
{
    if (pj->type == PJ_FILE)
    {
        if (pj->resData)
        {
            struct resDir* dirs = pj->resData->dirs;
            while (dirs)
            {
                struct resRes* children = dirs->children;
                while (children)
                {
                    if (children == res)
                    {
                        ResSave(pj->realName, pj->resData);
                        return TRUE;
                    }
                    children = children->next;
                }
                dirs = dirs->next;
            }
        }
    }
    else
    {
        PROJECTITEM* cur = pj->children;
        while (cur)
        {
            if (ResSaveCurrent(cur, res))
                return TRUE;
            cur = cur->next;
        }
    }
    return FALSE;
}

PROJECTITEM* GetResData(PROJECTITEM* cur)
{

    if (cur->type == PJ_FILE)
    {
        if (cur->resData)
            return cur;
        return NULL;
    }
    else
    {
        cur = cur->children;
        while (cur)
        {
            PROJECTITEM* rd;
            if ((rd = GetResData(cur)))
                return rd;
            cur = cur->next;
        }
    }
    return NULL;
}
static void ResSaveAllInternal(PROJECTITEM* cur)
{
    if (cur->type == PJ_FILE)
    {
        if (cur->resData)
            ResSave(cur->realName, cur->resData);
    }
    else
    {
        cur = cur->children;
        while (cur)
        {
            ResSaveAllInternal(cur);
            cur = cur->next;
        }
    }
}
void ResSaveAll(void) { ResSaveAllInternal(workArea); }
void ResOpen(PROJECTITEM* file)
{
    if (file->type == PJ_FILE && !file->resData)
    {
        static char buf[1024];
        strcpy(buf, szInstallPath);
        strcat(buf, "\\include;");
        GetProjectPath(buf + strlen(buf));
        rcSearchPath = buf;
        TreeView_Expand(treeWindow, workArea->hResTreeItem, TVE_COLLAPSE);
        file->resData = ResLoad(file->realName);
        if (file->resData)
        {
            FileTime(&file->resData->time, file->realName);
            ResCreateSubTree(file->resData);
            ResShowSubTree(file);
        }
        TreeView_Expand(treeWindow, workArea->hResTreeItem, TVE_EXPAND);
        TreeView_Expand(treeWindow, file->hResTreeItem, TVE_EXPAND);
    }
}
static PROJECTITEM* GetItemInfo(HTREEITEM item)
{
    TV_ITEM xx;
    if (!item)
        return NULL;
    xx.hItem = item;
    xx.mask = TVIF_PARAM;
    if (SendMessage(treeWindow, TVM_GETITEM, 0, (LPARAM)&xx))
        return (PROJECTITEM*)xx.lParam;
    return NULL;
}
void HandleDblClick(HTREEITEM item, BOOL err)
{
    PROJECTITEM* p = GetItemInfo(item);
    if (p)
    {
        if (p->type == PJ_RES)
        {
            struct resRes* d = (struct resRes*)p;
            if (d->activeHwnd)
            {
                HWND active = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (d->activeHwnd == active)
                {
                    SetFocus(active);
                }
                else
                {
                    PostMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)d->activeHwnd, 0);
                }
            }
            else
            {
                switch (d->resource->itype)
                {
                    case RESTYPE_ACCELERATOR:
                        CreateAcceleratorDrawWindow(d);
                        break;
                    case RESTYPE_ICON:
                    case RESTYPE_CURSOR:
                    case RESTYPE_BITMAP:
                        CreateImageDrawWindow(d);
                        break;
                    case RESTYPE_DIALOG:
                        CreateDlgDrawWindow(d);
                        break;
                    case RESTYPE_MENU:
                        CreateMenuDrawWindow(d);
                        break;
                    case RESTYPE_VERSION:
                        CreateVersionDrawWindow(d);
                        break;
                    case RESTYPE_STRING:
                        CreateStringTableDrawWindow(d);
                        break;
                    case RESTYPE_RCDATA:
                        CreateRCDataDrawWindow(d);
                        break;
                    case RESTYPE_FONT:
                    case RESTYPE_MESSAGETABLE:
                        SetResourceProperties(d, &imgFuncs);
                        break;
                    default:
                        if (err)
                            ExtendedMessageBox("Unimplemented", 0, "There is no editor for this type of resource");
                        break;
                }
                if (d->activeHwnd)
                    SetWindowLong(d->activeHwnd, 4, RESSIG);
            }
        }
        else
        {
            prjSelectedItem = p->hTreeItem;
            ResOpen(p);
        }
    }
}

LRESULT CALLBACK NewResourceProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    POINT pt = {5, 5};
    switch (iMessage)
    {
        int i;
        case WM_INITDIALOG:
            CenterWindow(hWnd);
            for (i = 0; i < sizeof(AddNames) / sizeof(AddNames[0]); i++)
            {
                int v = SendDlgItemMessage(hWnd, IDC_NEWRESOURCE, LB_ADDSTRING, 0, (LPARAM)AddNames[i].name);
                if (v != CB_ERR)
                {
                    SendDlgItemMessage(hWnd, IDC_NEWRESOURCE, LB_SETITEMDATA, v, i);
                }
            }
            SendDlgItemMessage(hWnd, IDC_NEWRESOURCE, LB_SETCURSEL, 0, 0);
            SendDlgItemMessage(hWnd, IDC_EDITNEWRESOURCE, WM_SETTEXT, 0, (LPARAM)AddNames[0].name);
            return TRUE;
        case WM_COMMAND:
            if (HIWORD(wParam) == LBN_SELCHANGE)
            {
                int v = SendDlgItemMessage(hWnd, IDC_NEWRESOURCE, LB_GETCURSEL, 0, 0);
                v = SendDlgItemMessage(hWnd, IDC_NEWRESOURCE, LB_GETITEMDATA, v, 0);
                SendDlgItemMessage(hWnd, IDC_EDITNEWRESOURCE, WM_SETTEXT, 0, (LPARAM)AddNames[v].name);
            }
            else
                switch (LOWORD(wParam))
                {
                    int v;
                    case IDC_NEWRESOURCE:
                        if (HIWORD(wParam) == LBN_DBLCLK)
                            PostMessage(hWnd, WM_COMMAND, IDOK, 0);
                        break;
                    case IDOK:
                        v = SendDlgItemMessage(hWnd, IDC_NEWRESOURCE, LB_GETCURSEL, 0, 0);
                        if (v != LB_ERR)
                        {
                            v = SendDlgItemMessage(hWnd, IDC_NEWRESOURCE, LB_GETITEMDATA, v, 0);
                            v = AddNames[v].type;
                            EndDialog(hWnd, v);
                        }
                        else
                        {
                            EndDialog(hWnd, 0);
                        }
                        break;
                    case IDCANCEL:
                        EndDialog(hWnd, 0);
                        break;
                }
            break;
    }
    return 0;
}
void HandleRightClick(HWND hWnd, HTREEITEM item)
{
    PROJECTITEM* p = GetItemInfo(item);
    if (p)
    {
        if (p->type == PJ_FILE && p->resData)
        {
            int v = DialogBox(hInstance, "DLG_NEWRESOURCE", hWnd, (DLGPROC)NewResourceProc);
            if (v != 0)
            {
                rclickSelection = item;
                dummies[v].type = PJ_RESMENU + v;
                rclickItem = &dummies[v];
                PostMessage(hWnd, WM_COMMAND, ID_NEW, 0);
            }
        }
        else if (p->type == PJ_RES || (p->type >= PJ_RESMENU && p->type < PJ_RESMENU + 100))
        {
            struct resRes* d = (struct resRes*)p;
            char* newItem = NULL;
            char* delItem = NULL;
            HMENU popup;
            POINT pos;
            int n = p->type == PJ_RES ? d->resource->itype : p->type - PJ_RESMENU;

            rclickItem = d;
            rclickSelection = item;
            switch (n)
            {
                case RESTYPE_ACCELERATOR:
                    newItem = "New Accelerator";
                    delItem = "Remove Accelerator";
                    break;
                case RESTYPE_ICON:
                    newItem = "New Icon";
                    delItem = "Remove Icon";
                    break;
                case RESTYPE_CURSOR:
                    newItem = "New Cursor";
                    delItem = "Remove Cursor";
                    break;
                case RESTYPE_BITMAP:
                    newItem = "New Bitmap";
                    delItem = "Remove Bitmap";
                    break;
                case RESTYPE_DIALOG:
                    newItem = "New Dialog";
                    delItem = "Remove Dialog";
                    break;
                case RESTYPE_FONT:
                    newItem = "New Font";
                    delItem = "Remove Font";
                    break;
                case RESTYPE_MENU:
                    newItem = "New Menu";
                    delItem = "Remove Menu";
                    break;
                case RESTYPE_VERSION:
                    newItem = "New Version Info";
                    delItem = "Remove Version Info";
                    break;
                case RESTYPE_STRING:
                    newItem = "New String Info";
                    delItem = "Remove String Info";
                    break;
                case RESTYPE_RCDATA:
                    newItem = "New RCDATA";
                    delItem = "Remove RCDATA";
                    break;
                case RESTYPE_MESSAGETABLE:
                    newItem = "New Message Table";
                    delItem = "Remove Message Table";
                    break;
                default:
                    delItem = "Remove Item";
                    break;
            }
            if (p->type != PJ_RES)
                delItem = NULL;
            else
                newItem = NULL;
            if (newItem || delItem)
            {
                popup = CreatePopupMenu();
                GetCursorPos(&pos);
                if (newItem)
                {
                    AppendMenu(popup, MF_STRING, ID_NEW, newItem);
                }
                else if (delItem)
                {
                    AppendMenu(popup, MF_STRING, ID_DELETE, delItem);
                    //                    AppendMenu(popup, MF_STRING, IDM_RENAME, "Rename");
                }
                TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTBUTTON, pos.x, pos.y, hWnd, NULL);
                DestroyMenu(popup);
            }
        }
    }
}
BOOL ResCheckChanged(RESOURCE* resources)
{
    while (resources)
    {
        if (resources->changed)
        {
            return TRUE;
        }
        resources = resources->next;
    }
    return FALSE;
}
void ResFree(PROJECTITEM* file, BOOL toSave)
{
    CloseAllResourceWindows();
    if (file->resData)
    {
        if (toSave && (ResCheckChanged(file->resData->resources->resources)))
            if (ExtendedMessageBox("Resources Changed", MB_YESNO, "Resources have changed.  Save them?") == IDYES)
                ResSave(file->realName, file->resData);
        ResFreeSubTree(file->resData);
        HeapDestroy(file->resData->resources->memHeap);
        free(file->resData);
        file->resData = NULL;
    }
}
void ResReload(PROJECTITEM* pj)
{
    ResFree(pj, FALSE);
    ResOpen(pj);
}
void NavigateToResource(PROJECTITEM* file)
{
    SelectWindow(DID_RESWND);
    TreeView_SelectItem(treeWindow, file->hResTreeItem);
    ResOpen(file);
    SelectWindow(DID_PROPSWND);
}
void ResAddItem(PROJECTITEM* file)
{
    if (file->type != PJ_FILE || (strlen(file->realName) >= 3 && !stricmp(file->realName + strlen(file->realName) - 3, ".rc")))
    {
        char buf[256];
        PROJECTITEM* parent = file->parent;
        HTREEITEM before = TVI_FIRST;
        TV_INSERTSTRUCT t;
        char* name = file->displayName;
        if (file->type == PJ_WS)
        {
            strcpy(buf, "(Resources) ");
            strcat(buf, file->displayName);
            name = buf;
        }
        if (parent)
        {
            PROJECTITEM* p = parent->children;
            while (p && p != file)
            {
                before = p->hResTreeItem;
                p = p->next;
            }
        }
        memset(&t, 0, sizeof(t));
        t.hParent = parent ? parent->hResTreeItem : TVI_ROOT;
        t.hInsertAfter = before;
        t.UNNAMED_UNION item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        t.UNNAMED_UNION item.hItem = 0;
        t.UNNAMED_UNION item.pszText = name;
        t.UNNAMED_UNION item.cchTextMax = strlen(file->displayName);
        t.UNNAMED_UNION item.iImage = t.UNNAMED_UNION item.iSelectedImage =
            file->type == PJ_WS ? IL_RC : imageof(file, file->realName);
        t.UNNAMED_UNION item.lParam = (LPARAM)file;
        file->hResTreeItem = TreeView_InsertItem(treeWindow, &t);
        ResShowSubTree(file);
    }
}
void ResDeleteItem(PROJECTITEM* data)
{
    if (data->hResTreeItem)
    {
        TreeView_DeleteItem(treeWindow, data->hResTreeItem);
        ResFree(data, TRUE);
        data->resData = 0;
        data->hResTreeItem = NULL;
    }
}
void ResDeleteAllItems(void) { TreeView_DeleteAllItems(treeWindow); }
void ResGetMenuItemName(EXPRESSION* id, char* text)
{
    if (id->rendition && !strnicmp(id->rendition, "IDM_MENUITEM", 12))
    {
        char name[512], *p = name;
        sprintf(name, "IDM_%s", text);
        while (*p)
        {
            if (*p == '&')
            {
                strcpy(p, p + 1);
                continue;
            }
            if (*p & 0x80)
                *p &= 0x7f;
            if (!isalnum(*p))
                *p = '_';
            p++;
        }
        id->rendition = rcStrdup(name);
        ResAddNewDef(name, id->val);
    }
}
void ResGetStringItemName(EXPRESSION* id, char* text)
{
    if (id->rendition && !strnicmp(id->rendition, "IDS_STRING", 9))
    {
        char name[512], *p = name;
        sprintf(name, "IDS_%s", text);
        while (*p)
        {
            if (*p & 0x80)
                *p &= 0x7f;
            if (!isalnum(*p))
                *p = '_';
            p++;
        }
        id->rendition = ResAddNewDef(name, id->val);
    }
}
int ResChangeId(char* name, int id)
{
    SYM* s = search(name, &currentResData->syms);
    if (s)
    {
        DEFSTRUCT* d = (DEFSTRUCT*)s->value.s;
        WCHAR buf[256];
        wsprintfW(buf, L"%d", id);
        if (wcscmp(buf, d->string))
            d->string = prcStrdup(buf);
        return 1;
    }
    return 0;
}
char* ResAddNewDef(char* name, int id)
{
    if (!ResChangeId(name, id))
    {
        SYM* sp;
        DEFSTRUCT* def;
        WCHAR wbuf[256];

        sp = rcAlloc(sizeof(SYM));
        sp->name = rcStrdup(name);
        def = rcAlloc(sizeof(DEFSTRUCT));
        sp->value.s = (char*)def;

        def->args = 0;
        def->argcount = 0;
        wsprintfW(wbuf, L"%d", id);
        def->string = prcStrdup(wbuf);

        insert(sp, &currentResData->syms);
        sp->xref = currentResData->newDefinitions;
        currentResData->newDefinitions = sp;
        return sp->name;
    }
    else
    {
        return rcStrdup(name);
    }
}
EXPRESSION* ResAllocateMenuId()
{
    char name[256];
    EXPRESSION* id = rcAlloc(sizeof(EXPRESSION));
    id->type = e_int;
    id->val = currentResData->nextMenuId++;
    sprintf(name, "IDM_MENUITEM_%d", id->val);
    id->rendition = ResAddNewDef(name, id->val);
    return id;
}
EXPRESSION* ResAllocateStringId()
{
    char name[256];
    EXPRESSION* id = rcAlloc(sizeof(EXPRESSION));
    id->type = e_int;
    id->val = currentResData->nextStringId++;
    sprintf(name, "IDS_STRING_%d", id->val);
    id->rendition = ResAddNewDef(name, id->val);
    return id;
}
int ResNextCtlId(CONTROL* ctls)
{
    int max = 100;
    while (ctls)
    {
        int n = Eval(ctls->id);
        if (n >= max)
            max = n + 1;
        ctls = ctls->next;
    }
    return max;
}

EXPRESSION* ResAllocateControlId(struct resRes* dlgData, char* sel)
{
    char name[256];
    EXPRESSION* rv = rcAlloc(sizeof(EXPRESSION));
    rv->type = e_int;
    if (!strcmp(sel, "STATICTEXT"))
    {
        rv->val = -1;
    }
    else
    {
        rv->val = ResNextCtlId(dlgData->resource->u.dialog->controls);
        sprintf(name, "IDC_%s_%d", sel, currentResData->nextControlId++);
        rv->rendition = ResAddNewDef(name, rv->val);
    }
    return rv;
}
EXPRESSION* ResAllocateResourceId(int type)
{
    char* prefix = "IDR_RESOURCE";  // fallback
    char name[256];
    int i;
    EXPRESSION* rv = rcAlloc(sizeof(EXPRESSION));
    rv->type = e_int;
    rv->val = currentResData->nextResourceId++;
    for (i = 0; i < sizeof(AddNames) / sizeof(AddNames[0]); i++)
        if (AddNames[i].type == type)
        {
            prefix = AddNames[i].resPrefix;
            break;
        }
    sprintf(name, "%s_%d", prefix, rv->val);
    rv->rendition = ResAddNewDef(name, rv->val);
    return rv;
}

EXPRESSION* ResReadExp(struct resRes* data, char* buf)
{
    EXPRESSION* id;
    ResGetHeap(workArea, data);
    id = rcAlloc(sizeof(EXPRESSION));
    id->type = e_int;
    id->val = atoi(buf);
    return id;
}
static int CustomDraw(HWND hwnd, LPNMTVCUSTOMDRAW draw)
{
    PROJECTITEM* p;
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;
        case CDDS_ITEMPREPAINT:
            p = (PROJECTITEM*)draw->nmcd.lItemlParam;
            if (!p)
            {
                draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                SelectObject(draw->nmcd.hdc, projFont);
                return CDRF_NEWFONT;
            }
            else if (p->type == PJ_PROJ && !p->loaded)
            {
                draw->clrText = RetrieveSysColor(COLOR_GRAYTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                SelectObject(draw->nmcd.hdc, italicProjFont);
                return CDRF_NEWFONT;
            }
            else
            {
                if (activeProject == p)
                {
                    SelectObject(draw->nmcd.hdc, boldProjFont);
                    return CDRF_NEWFONT;
                }
                else if (draw->nmcd.uItemState & (CDIS_SELECTED))
                {
                    draw->clrText = RetrieveSysColor(COLOR_HIGHLIGHT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                }
                else if (draw->nmcd.uItemState & (CDIS_HOT))
                {
                    draw->clrText = RetrieveSysColor(COLOR_INFOTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_INFOBK);
                }
                else if (draw->nmcd.uItemState == 0)
                {
                    draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                }
                SelectObject(draw->nmcd.hdc, projFont);
                if (hwndEdit)
                    InvalidateRect(hwndEdit, 0, TRUE);
                return CDRF_NEWFONT;
            }
            return CDRF_DODEFAULT;
        default:
            return CDRF_DODEFAULT;
    }
}
static EXPRESSION* ResNewExp(int val, char* name)
{
    EXPRESSION* temp = rcAlloc(sizeof(EXPRESSION));
    temp->type = e_int;
    temp->rendition = name ? rcStrdup(name) : name;
    temp->val = val;
    return temp;
}
static EXPRESSION* ResOrExp(EXPRESSION* left, EXPRESSION* right)
{
    EXPRESSION* temp = rcAlloc(sizeof(EXPRESSION));
    temp->type = or ;
    temp->left = left;
    temp->right = right;
    return temp;
}
WCHAR* WStrDup(WCHAR* s)
{
    WCHAR* rv = rcAlloc((wcslen(s) + 1) * sizeof(WCHAR));
    wcscpy(rv, s);
    return rv;
}
static void ResPrependDialogButton(CONTROL** ctl, char* text, int id, char* idname, int x, int y, int width, int height)
{
    EXPRESSION* curExp = NULL;
    CONTROL* rv = rcAlloc(sizeof(CONTROL));
    rv->next = *ctl;
    *ctl = rv;
    rv->generic = TRUE;
    rv->class.u.id = ResNewExp(CTL_BUTTON, 0);
    rv->text = rcAlloc(sizeof(IDENT));
    rv->text->symbolic = TRUE;
    rv->text->u.n.length = StringAsciiToWChar(&rv->text->u.n.symbol, text, strlen(text));
    rv->id = ResNewExp(id, idname);
    rv->x = ResNewExp(x, 0);
    rv->y = ResNewExp(y, 0);
    rv->width = ResNewExp(width, 0);
    rv->height = ResNewExp(height, 0);
    rv->style = ResNewExp(BS_PUSHBUTTON, "BS_PUSHBUTTON");
    rv->style = ResOrExp(rv->style, ResNewExp(BS_CENTER, "BS_CENTER"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_CHILD, "WS_CHILD"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_VISIBLE, "WS_VISIBLE"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_TABSTOP, "WS_TABSTOP"));
}
static DIALOG* ResNewDialog()
{
    DIALOG* rv = rcAlloc(sizeof(DIALOG));
    rv->exstyle =
        ResOrExp(ResNewExp(WS_EX_DLGMODALFRAME, "WS_EX_DLGMODALFRAME"), ResNewExp(WS_EX_CONTEXTHELP, "WS_EX_CONTEXTHELP"));
    rv->style = ResNewExp(WS_VISIBLE, "DS_MODALFRAME");
    rv->style = ResOrExp(rv->style, ResNewExp(WS_VISIBLE, "DS_3DLOOK"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_VISIBLE, "DS_CONTEXTHELP"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_VISIBLE, "WS_POPUP"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_VISIBLE, "WS_VISIBLE"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_CAPTION, "WS_CAPTION"));
    rv->style = ResOrExp(rv->style, ResNewExp(WS_CAPTION, "WS_SYSMENU"));
    rv->x = ResNewExp(0, 0);
    rv->y = ResNewExp(0, 0);
    rv->width = ResNewExp(240, 0);
    rv->height = ResNewExp(120, 0);
    rv->caption = WStrDup(L"New Dialog");
    rv->pointsize = ResNewExp(8, 0);
    rv->font = WStrDup(L"MS Shell Dlg");
    rv->ex.charset = ResNewExp(DEFAULT_CHARSET, "DEFAULT_CHARSET");
    ResPrependDialogButton(&rv->controls, "Help", IDHELP, "IDHELP", 186, 46, 50, 14);
    ResPrependDialogButton(&rv->controls, "Cancel", IDCANCEL, "IDCANCEL", 186, 26, 50, 14);
    ResPrependDialogButton(&rv->controls, "OK", IDOK, "IDOK", 186, 6, 50, 14);
    return rv;
}
static void ResPrependVerString(struct ver_stringinfo** s, char* name, char* val)
{
    char buf[1024];
    struct ver_stringinfo* rv;
    rv = rcAlloc(sizeof(struct ver_stringinfo));
    rv->next = *s;
    *s = rv;
    StringAsciiToWChar(&rv->key, name, strlen(name));
    strcpy(buf, val);
    strcat(buf, "\0");
    rv->length = StringAsciiToWChar(&rv->value, buf, strlen(buf) + 1);
}
static VERSIONINFO* ResNewVersionInfo(PROJECTITEM* data)
{
    VERSIONINFO* rv = rcAlloc(sizeof(VERSIONINFO));
    struct variable* string;
    struct variable* var;
    char name[256], *p;
    while (data->type != PJ_PROJ)
    {
        data = data->parent;
    }
    p = strrchr(data->realName, '\\');
    if (!p)
        p = data->realName;
    else
        p++;
    strcpy(name, p);
    rv->fixed = rcAlloc(sizeof(struct fixed));
    rv->fixed->file_version_ms = 0x10000;
    rv->fixed->file_version_ls = 0x1;
    rv->fixed->product_version_ms = 0x10000;
    rv->fixed->product_version_ls = 0x1;
    rv->fixed->file_flags_mask = ResNewExp(VS_FFI_FILEFLAGSMASK, "VS_FFI_FILEFLAGSMASK");
    rv->fixed->file_os = ResNewExp(VOS_NT_WINDOWS32, "VOS_NT_WINDOWS32");
    rv->fixed->file_type = ResNewExp(VFT_APP, "VFT_APP");
    string = rcAlloc(sizeof(struct variable));
    string->type = VERINFO_STRING;
    string->u.string.language = WStrDup(L"040904E4");
    ResPrependVerString(&string->u.string.strings, "LegalCopyright", "Copyright (C)");
    ResPrependVerString(&string->u.string.strings, "ProductVersion", "1, 0, 0, 1");
    ResPrependVerString(&string->u.string.strings, "ProductName", data->displayName);
    ResPrependVerString(&string->u.string.strings, "OriginalFileName", name);
    ResPrependVerString(&string->u.string.strings, "InternalName", name);
    ResPrependVerString(&string->u.string.strings, "FileVersion", "1, 0, 0, 1");
    ResPrependVerString(&string->u.string.strings, "FileDescription", name);
    ResPrependVerString(&string->u.string.strings, "CompanyName", "Unknown");
    rv->var = string;
    var = rcAlloc(sizeof(struct variable));
    var->type = VERINFO_VAR;
    rv->var->next = var;
    var->u.var.var = rcAlloc(sizeof(struct ver_varinfo));
    var->u.var.var->key = WStrDup(L"Translation");
    var->u.var.var->intident = rcAlloc(sizeof(struct ver_varlangchar));
    var->u.var.var->intident->language = ResNewExp(0x409, 0);
    var->u.var.var->intident->charset = ResNewExp(0x4e4, 0);
    return rv;
}
static int ResGetImageName(PROJECTITEM* pj, char* name, char* title, char* filter)
{
    OPENFILENAME ofn;
    switch (OpenFileDialogWithCancel(&ofn, pj->realName, hwndFrame, TRUE, FALSE, filter, title))
    {
        case 0:  // file doesn't exist
            strcpy(name, ofn.lpstrFile);
            if (ExtendedMessageBox("New Image File", MB_YESNO, "File %s does not exist.  It will be created.", name) == IDYES)
                return 2;
            else
                return 0;
        case 1:
            // file exists
            strcpy(name, ofn.lpstrFile);
            return 1;
        case 2:  // cancel
            return 0;
        default:
            return 0;
    }
}
static FONT* ResNewFont(PROJECTITEM* pj, RESOURCE* newRes)
{
    OPENFILENAME ofn;
    if (OpenFileDialog(&ofn, pj->realName, hwndFrame, FALSE, FALSE, szFontFilter, "Load Font"))
    {
        // file exists
        newRes->filename = rcStrdup(ofn.lpstrFile);
        return RCLoadFont(newRes->filename);
    }
    return 0;
}
static BYTE* ResNewMessageTable(PROJECTITEM* pj, RESOURCE* newRes, int* size)
{
    OPENFILENAME ofn;
    if (OpenFileDialog(&ofn, pj->realName, hwndFrame, FALSE, FALSE, szMessageTableFilter, "Load Message Table"))
    {
        // file exists
        newRes->filename = rcStrdup(ofn.lpstrFile);
        return RCLoadMessageTable(newRes->filename, size);
    }
    return 0;
}
static BITMAP_* ResNewBitmap(PROJECTITEM* pj, RESOURCE* newRes)
{
    char name[MAX_PATH];
    BITMAP_* nbmp;
    switch (ResGetImageName(pj, name, "Load Bitmap", szBitmapFilter))
    {
        default:
        case 0:
            return NULL;
        case 1:
            newRes->filename = rcStrdup(name);
            return RCLoadBitmap(name);
        case 2:
            newRes->filename = rcStrdup(name);
            nbmp = rcAlloc(sizeof(BITMAP_));
            {
                BITMAPINFOHEADER* lpbih;
                nbmp->headerSize = sizeof(BITMAPINFOHEADER);  //+ (1 << 4) * sizeof(RGBQUAD);
                nbmp->headerData = (BYTE*)(lpbih = rcAlloc(nbmp->headerSize));
                memset(nbmp->headerData + sizeof(BITMAPINFOHEADER), 0xff, 3);  // COLOR WHITE
                lpbih->biBitCount = 24;                                        // 24-bit colors
                lpbih->biHeight = 32;
                lpbih->biWidth = 32;
                lpbih->biPlanes = 1;
                lpbih->biSize = sizeof(BITMAPINFOHEADER);
                lpbih->biSizeImage = (((((DWORD)lpbih->biWidth * lpbih->biBitCount) + 31) & 0xffffffe0) >> 3) * lpbih->biHeight;
                nbmp->pixelSize = lpbih->biSizeImage;
                nbmp->pixelData = rcAlloc(nbmp->pixelSize);
                memset(nbmp->pixelData, 0xff, nbmp->pixelSize);  // set to white
            }
            return nbmp;
    }
}
static CURSOR* ResNewCursor(PROJECTITEM* pj, RESOURCE* newRes)
{
    char name[MAX_PATH];
    CURSOR* ncrs;
    int colorbits;
    BYTE* p;
    switch (ResGetImageName(pj, name, "Load Cursor", szCursorFilter))
    {
        default:
        case 0:
            return NULL;
        case 1:
            newRes->filename = rcStrdup(name);
            return RCLoadCursor(name);
        case 2:
            colorbits = 1;
            newRes->filename = rcStrdup(name);
            ncrs = rcAlloc(sizeof(CURSOR));
            ncrs->count = 1;
            ncrs->cursors = rcAlloc(sizeof(CURSORDATA));
            ncrs->cursors->height = 32;
            ncrs->cursors->width = 32;
            ncrs->cursors->colorcount = colorbits == 8 ? 8 : 0;
            ncrs->cursors->xhotspot = 0;
            ncrs->cursors->yhotspot = 0;
            ncrs->cursors->bytes = sizeof(BITMAPINFOHEADER) + (1 << colorbits) * sizeof(RGBQUAD) +
                                   (((((DWORD)ncrs->cursors->width * colorbits) + 31) & 0xffffffe0) >> 3) * ncrs->cursors->height +
                                   (((((DWORD)ncrs->cursors->width) + 31) & 0xffffffe0) >> 3) * ncrs->cursors->height;
            ncrs->cursors->data = rcAlloc(ncrs->cursors->bytes);
            p = ncrs->cursors->data + sizeof(BITMAPINFOHEADER);
            p[3] = p[4] = p[5] = 0xff;
            p += (1 << colorbits) * sizeof(RGBQUAD);
            //            memset (p, 0xff, (((((DWORD)ncrs->cursors->width * colorbits) + 31) & 0xffffffe0) >> 3) *
            //            ncrs->cursors->height);
            p += (((((DWORD)ncrs->cursors->width * colorbits) + 31) & 0xffffffe0) >> 3) * ncrs->cursors->height;
            memset(p, 0xff, (((((DWORD)ncrs->cursors->width) + 31) & 0xffffffe0) >> 3) * ncrs->cursors->height);

            {
                BITMAPINFOHEADER* lpbih = (BITMAPINFOHEADER*)ncrs->cursors->data;
                lpbih->biBitCount = colorbits;  // 2 colors
                lpbih->biHeight = ncrs->cursors->height * 2;
                lpbih->biWidth = ncrs->cursors->width;
                lpbih->biPlanes = 1;
                lpbih->biSize = sizeof(BITMAPINFOHEADER);
                // just the size of the XOR mask...
                lpbih->biSizeImage = (((((DWORD)lpbih->biWidth * colorbits) + 31) & 0xffffffe0) >> 3) * lpbih->biHeight / 2;
            }
            return ncrs;
            break;
    }
}
static ICON* ResNewIcon(PROJECTITEM* pj, RESOURCE* newRes)
{
    char name[MAX_PATH];
    ICON* nico;
    char* p;
    int colorbits;
    switch (ResGetImageName(pj, name, "Load Icon", szIconFilter))
    {
        default:
        case 0:
            return NULL;
        case 1:
            newRes->filename = rcStrdup(name);
            return RCLoadIcon(name);
            break;
        case 2:
            colorbits = 4;
            newRes->filename = rcStrdup(name);
            nico = rcAlloc(sizeof(ICON));
            nico->count = 1;
            nico->icons = rcAlloc(sizeof(ICONDATA));
            nico->icons->height = 32;
            nico->icons->width = 32;
            nico->icons->colorcount = 0;
            nico->icons->bits = colorbits;
            nico->icons->bytes =
                sizeof(BITMAPINFOHEADER) + (1 << nico->icons->bits) * sizeof(RGBQUAD) +
                (((((DWORD)nico->icons->width * nico->icons->bits) + 31) & 0xffffffe0) >> 3) * nico->icons->height +
                (((((DWORD)nico->icons->width) + 31) & 0xffffffe0) >> 3) * nico->icons->height;
            nico->icons->data = rcAlloc(nico->icons->bytes);
            p = (char*)nico->icons->data + sizeof(BITMAPINFOHEADER);
            p += (1 << nico->icons->bits) * sizeof(RGBQUAD);
            p += (((((DWORD)nico->icons->width * nico->icons->bits) + 31) & 0xffffffe0) >> 3) * nico->icons->height;
            // this and the blackness will make it see through by default
            memset(p, 0xff, (((((DWORD)nico->icons->width) + 31) & 0xffffffe0) >> 3) * nico->icons->height);
            {
                LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)nico->icons->data;
                lpbih->biBitCount = colorbits;  // 16 colors
                lpbih->biHeight = nico->icons->height * 2;
                lpbih->biWidth = nico->icons->width;
                lpbih->biPlanes = 1;
                lpbih->biSize = sizeof(BITMAPINFOHEADER);
                lpbih->biSizeImage = (((((DWORD)lpbih->biWidth * lpbih->biBitCount) + 31) & 0xffffffe0) >> 3) * lpbih->biHeight / 2;
            }
            return nico;
    }
}
static MENU* ResNewMenu(void)
{
    MENU* menu = rcAlloc(sizeof(MENU));
    menu->items = rcAlloc(sizeof(MENUITEM));
    StringAsciiToWChar(&menu->items->text, "New Item", 8);
    menu->items->id = ResAllocateMenuId();
    return menu;
}
static int ResSetData(PROJECTITEM* data, RESOURCE* newRes)
{
    if (!setjmp(errjump))
    {
        switch (newRes->itype)
        {
            case RESTYPE_FONT:
                newRes->u.font = ResNewFont(data, newRes);
                if (!newRes->u.font)
                    return 0;
                break;
            case RESTYPE_BITMAP:
                newRes->u.bitmap = ResNewBitmap(data, newRes);
                if (!newRes->u.bitmap)
                    return 0;
                break;
            case RESTYPE_CURSOR:
                newRes->u.cursor = ResNewCursor(data, newRes);
                if (!newRes->u.cursor)
                    return 0;
                break;
            case RESTYPE_ICON:
                newRes->u.icon = ResNewIcon(data, newRes);
                if (!newRes->u.icon)
                    return 0;
                break;
            case RESTYPE_DIALOG:
                newRes->extended = TRUE;
                newRes->u.dialog = ResNewDialog();
                break;
            case RESTYPE_MENU:
                newRes->extended = TRUE;
                newRes->u.menu = ResNewMenu();
                break;
            case RESTYPE_VERSION:
                newRes->u.versioninfo = ResNewVersionInfo(data);
                break;
            case RESTYPE_MESSAGETABLE:
                newRes->u.data.data = ResNewMessageTable(data, newRes, &newRes->u.data.length);
                if (!newRes->u.data.data)
                    return 0;
                break;
        }
        newRes->changed = TRUE;
    }
    else
    {
        // return from FATAL comes here...
        return 0;
    }
    return 1;
}
void ResSetTreeName(struct resRes* data, char* buf)
{
    TVITEM xx;
    memset(&xx, 0, sizeof(xx));
    data->name = rcStrdup(buf);
    xx.mask = TVIF_TEXT;
    xx.hItem = data->hTreeItem;
    xx.pszText = data->name;
    xx.cchTextMax = strlen(buf);
    TreeView_SetItem(treeWindow, &xx);
    ResRewriteTitle(data, data->resource->changed);
}
static void ResInsertNew(PROJECTITEM* pj, RESOURCE* newRes)
{
    TV_INSERTSTRUCT t;
    struct resData* resData = pj->resData;
    RESOURCE** curs = &resData->resources->resources;
    struct resDir* dirs = FindResDir(resData, newRes->itype);
    struct resRes *res = rcAlloc(sizeof(struct resRes)), **res2;
    res->type = PJ_RES;
    res->name = newRes->id.u.id->rendition;
    res->resource = newRes;
    while (*curs)
    {
        if ((*curs)->itype == newRes->itype)
        {
            while ((*curs)->next && (*curs)->next->itype == newRes->itype)
                curs = &(*curs)->next;
            newRes->next = (*curs);
            (*curs) = newRes;
            newRes = NULL;
            break;
        }
        curs = &(*curs)->next;
    }
    // not found, tack it on the end
    if (!*curs && newRes)
        *curs = newRes;
    if (!dirs->hTreeItem)
    {
        struct resDir* dir2;
        memset(&t, 0, sizeof(t));
        dir2 = resData->dirs;
        t.hParent = pj->hResTreeItem;
        t.hInsertAfter = TVI_FIRST;
        dummies[dirs->type].type = PJ_RESMENU + dirs->type;
        t.UNNAMED_UNION item.lParam = (LPARAM)&dummies[dirs->type];
        t.UNNAMED_UNION item.pszText = DirNames[dirs->type];
        while (dir2 && dir2 != dirs)
        {
            t.hInsertAfter = dir2->hTreeItem;
            dir2 = dir2->next;
        }
        dirs->hTreeItem = InsertResourceTreeItem(&t);
    }
    res2 = &dirs->children;
    memset(&t, 0, sizeof(t));
    t.hParent = dirs->hTreeItem;
    t.hInsertAfter = TVI_FIRST;
    t.UNNAMED_UNION item.lParam = (LPARAM)res;
    t.UNNAMED_UNION item.pszText = res->name;
    while (*res2 && stricmp((*res2)->name, res->name) < 0)
    {
        t.hInsertAfter = (*res2)->hTreeItem;
        res2 = &(*res2)->next;
    }
    res->next = *res2;
    *res2 = res;
    res->hTreeItem = InsertResourceTreeItem(&t);
    TreeView_EnsureVisible(treeWindow, res->hTreeItem);
    HandleDblClick(res->hTreeItem, FALSE);
}
PROJECTITEM* GetResProjectFile(HTREEITEM sel)
{
    PROJECTITEM* data = GetItemInfo(sel);
    if (data)
    {
        while (data && data->type != PJ_FILE)
        {
            sel = TreeView_GetParent(treeWindow, sel);
            data = GetItemInfo(sel);
        }
    }
    return data;
}
static void NewResource(HTREEITEM sel, struct resRes* res)
{
    PROJECTITEM* data = GetResProjectFile(sel);
    if (data)
    {
        // fix filename in insert
        RESOURCE* newRes;
        currentResData = ((PROJECTITEM*)data)->resData->resources;
        SetRCMallocScope(((PROJECTITEM*)data)->resData->resources);
        newRes = rcAlloc(sizeof(RESOURCE));
        newRes->itype = res->type - PJ_RESMENU;
        if (ResSetData(data, newRes))
        {
            if (newRes->itype == RESTYPE_VERSION)
            {
                newRes->id.u.id = ResNewExp(1, "VS_VERSION_INFO");
                ResAddNewDef(newRes->id.u.id->rendition, newRes->id.u.id->val);
            }
            else if (newRes->itype == RESTYPE_STRING)
            {
                newRes->id.u.id = ResNewExp(1, "String Table");
            }
            else
            {
                newRes->id.u.id = ResAllocateResourceId(newRes->itype);
            }
            newRes->id.symbolic = FALSE;
            newRes->id.origName = NULL;
            ResInsertNew(data, newRes);
        }
        // this leaks memory but only into the file's local heap.
    }
}
void ResRename(void)
{
    PROJECTITEM* data = GetItemInfo(resSelection);
    if (data && data->type == PJ_RES)
    {
        RECT r, s;
        editItem = data;
        TreeView_GetItemRect(treeWindow, data->hTreeItem, &r, TRUE);
        TreeView_GetItemRect(treeWindow, data->hTreeItem, &s, FALSE);
        hwndEdit = CreateWindow(szresEditClassName, ((struct resRes*)data)->name, WS_CHILD | ES_AUTOHSCROLL | WS_BORDER, r.left,
                                r.top, s.right - r.left, r.bottom - r.top, hwndRes, (HMENU)449,
                                (HINSTANCE)GetWindowLong(GetParent(hwndRes), GWL_HINSTANCE), 0);
        SendMessage(hwndEdit, EM_SETSEL, 0, (LPARAM)-1);
        SendMessage(hwndEdit, EM_SETLIMITTEXT, 64, 0);
        ShowWindow(hwndEdit, SW_SHOW);
        SetFocus(hwndEdit);
    }
}
void ResDoneRenaming(void)
{
    char buf[MAX_PATH];
    buf[0] = 0;
    GetWindowText(hwndEdit, buf, MAX_PATH);
    if (buf[0] != 0)
    {
        if (strcmp(buf, ((struct resRes*)editItem)->name))
        {
            PropSetIdName((struct resRes*)editItem, buf, &((struct resRes*)editItem)->resource->id.u.id, NULL, TRUE);
        }
    }
    DestroyWindow(hwndEdit);
    hwndEdit = NULL;
}
LRESULT CALLBACK ResourceProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    int i;
    NM_TREEVIEW* nm;
    RECT rs;
    TVHITTESTINFO hittest;
    switch (iMessage)
    {
        LOGFONT lf;
        case WM_NOTIFY:
            nm = (NM_TREEVIEW*)lParam;
            switch (nm->hdr.code)
            {
                LPNMTVKEYDOWN key;
                case N_EDITDONE:
                    ResDoneRenaming();
                    break;
                case NM_CUSTOMDRAW:
                    return CustomDraw(hwnd, (LPNMTVCUSTOMDRAW)nm);
                case NM_DBLCLK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(treeWindow, &hittest.pt);
                    PostMessage(hwnd, WM_HANDLEDBLCLICK, 0, (LPARAM)TreeView_HitTest(treeWindow, &hittest));
                    return 0;
                case NM_RCLICK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(treeWindow, &hittest.pt);
                    resSelection = TreeView_HitTest(treeWindow, &hittest);
                    if (resSelection)
                    {
                        TreeView_SelectItem(treeWindow, resSelection);
                        HandleRightClick(hwnd, resSelection);
                    }
                    break;
                case TVN_SELCHANGED:
                    nm = (NM_TREEVIEW*)lParam;
                    resSelection = nm->itemNew.hItem;
                    if (resSelection == 0)
                    {
                        resSelection = workArea->hResTreeItem;
                    }
                    break;
                case TVN_KEYDOWN:
                    key = (LPNMTVKEYDOWN)lParam;
                    switch (key->wVKey)
                    {
                        case VK_INSERT:
                            if (!(GetKeyState(VK_CONTROL) & 0x80000000) && !(GetKeyState(VK_SHIFT) & 0x8000000))
                            {
                                PROJECTITEM* data = GetItemInfo(resSelection);
                                if (data && (data->type == PJ_RES))
                                    PostMessage(hwnd, WM_COMMAND, IDM_RENAME, 0);
                            }
                            break;
                        case VK_DELETE:
                            if (!(GetKeyState(VK_CONTROL) & 0x80000000) && !(GetKeyState(VK_SHIFT) & 0x8000000))
                            {
                                PROJECTITEM* data = GetItemInfo(resSelection);
                                if (data && (data->type == PJ_RES))
                                {
                                    rclickItem = (struct resRes*)data;
                                    PostMessage(hwnd, WM_COMMAND, ID_DELETE, 0);
                                }
                            }
                            break;
                        case VK_RETURN:
                            if (resSelection)
                            {
                                HandleDblClick(resSelection, TRUE);
                            }
                            break;
                    }
                    break;
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_NEWRESOURCE:
                {
                    PROJECTITEM* pj = GetResData(activeProject);
                    HandleRightClick(hwnd, pj->hResTreeItem);
                }
                break;
                case ID_NEW:
                    NewResource(rclickSelection, rclickItem);
                    break;
                case ID_DELETE:
                    if (ExtendedMessageBox("Resource Remove Item", MB_OKCANCEL, "This resource wil be removed.") == IDOK)
                    {
                        if (rclickItem)
                        {
                            struct resRes* res = rclickItem;
                            if (IsWindow(res->activeHwnd))
                            {
                                SendMessage(res->activeHwnd, WM_CLOSE, 0, 0);
                            }

                            TreeView_DeleteItem(treeWindow, res->hTreeItem);
                            res->resource->deleted = TRUE;
                            res->resource->changed = TRUE;
                            // no memory has to be deleted because the memory is in a separate
                            // per-file heap that will be deleted when the file is closed.
                        }
                    }
                    break;
                case IDM_RENAME:
                    ResRename();
                    break;
            }
            break;
        case WM_HANDLEDBLCLICK:
            HandleDblClick((HTREEITEM)lParam, TRUE);
            break;
        case WM_CREATE:
            GetClientRect(hwnd, &rs);
            treeWindow = CreateWindowEx(0, sztreeDoubleBufferName, "",
                                        WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_TRACKSELECT,
                                        0, 0, rs.right, rs.bottom, hwnd, (HMENU)ID_TREEVIEW, hInstance, NULL);
            i = GetWindowLong(treeWindow, GWL_STYLE);
            TreeView_SetImageList(treeWindow, ImageList_Duplicate(treeIml), TVSIL_NORMAL);
            lf = systemDialogFont;
            projFont = CreateFontIndirect(&lf);
            lf.lfItalic = TRUE;
            italicProjFont = CreateFontIndirect(&lf);
            lf.lfItalic = FALSE;
            lf.lfWeight = FW_BOLD;
            boldProjFont = CreateFontIndirect(&lf);
            break;
        case WM_SIZE:
            MoveWindow(treeWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 0);
            break;
        case WM_DESTROY:
            DestroyWindow(treeWindow);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static LRESULT CALLBACK extEditWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    NMHDR nm;
    switch (iMessage)
    {
        case WM_CHAR:
            if (wParam == 13)
            {
                nm.code = N_EDITDONE;
                nm.hwndFrom = hwnd;
                nm.idFrom = GetWindowLong(hwnd, GWL_ID);
                SendMessage(GetParent(hwnd), WM_NOTIFY, 0, (LPARAM)&nm);
                return 0;
            }
            if (wParam == 27)
            {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        case WM_KILLFOCUS:
            DestroyWindow(hwnd);
            return 0;
    }
    return CallWindowProc(oldEditProc, hwnd, iMessage, wParam, lParam);
}
void RegisterResourceWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &ResourceProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szResourceClassName;
    RegisterClass(&wc);

    GetClassInfo(0, "edit", &wc);
    oldEditProc = wc.lpfnWndProc;
    wc.lpfnWndProc = &extEditWndProc;
    wc.lpszClassName = szresEditClassName;
    wc.hInstance = hInstance;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreateResourceWindow(void)
{
    hwndRes = CreateInternalWindow(DID_RESWND, szResourceClassName, szResourceTitle);
    return hwndRes;
}