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

#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <richedit.h>
#include <prsht.h>
#include <stdio.h>
#include <ctype.h>
#include "idewinconst.h"
#include "helpid.h"
#include "header.h"

#define GENERALPROPS "general.props"

#define IDC_AUXBUTTON 10000
#define IDC_LISTBOX 10001
#define IDC_SETFONTNAME 10002
#define IDC_RELOAD 10003

extern char szInstallPath[];
extern PROJECTITEM* workArea;
extern LOGFONT systemDialogFont;
extern HWND hwndFrame;
extern HINSTANCE hInstance;
extern BUILDRULE* buildRules;
extern unsigned int helpMsg;
#define MyMessageBox(one, two, three, four)  // ExtendedMessageBox(one,two,three,four)

COLORREF custColors[16];
PROJECTITEM generalProject;
char* sysProfileName = "WIN32";
char currentProfileName[256];
int profileDebugMode = 1;
HWND hwndGeneralProps;

PROFILENAMELIST* profileNames;

static PROFILE* generalPrototype;
static char* szGeneralPropsClassName = "xccGeneralProps";
static char* szAccept = "Accept";
static char* szApply = "Apply";
static char* szClose = "Close";
static char* szHelp = "Help";
static HBITMAP menuButtonBM, comboButtonBM;
static CRITICAL_SECTION propsMutex;
static SETTING* current;
static HWND hwndTree;
static HWND hwndLV;
static BOOL populating;

static struct _propsData generalProps = {
    "General Properties",
    1,
    &generalPrototype,
    &generalProject,
};

static SETTING* PropFindAll(PROFILE** list, int count, char* id);
static void PopulateItems(HWND parent, HWND hwnd, SETTING* settings);

char* LookupProfileName(char* name)
{
    if (!strcmp(name, sysProfileName))
    {
        return sysProfileName;
    }
    else
    {
        PROFILENAMELIST** pf = &profileNames;
        while (*pf)
        {
            if (!strcmp((*pf)->name, name))
                return (*pf)->name;
            pf = &(*pf)->next;
        }
        *pf = calloc(1, sizeof(PROFILENAMELIST));
        strcpy((*pf)->name, name);
        return (*pf)->name;
    }
}
void ClearProfileNames()
{
    PROFILENAMELIST* pf = profileNames;
    profileNames = NULL;
    while (pf)
    {
        PROFILENAMELIST* next = pf->next;
        free(pf);
        pf = next;
    }
}
void InsertSetting(PROJECTITEM* pj, SETTING* set)
{
    SETTING** s = GetSettingsAddress(pj);
    if (s)
    {
        set->next = *s;
        *s = set;
    }
}
SETTING** GetSettingsAddress(PROJECTITEM* pj)
{
    if (pj == &generalProject)
    {
        return &generalProject.profiles->debugSettings;
    }
    else
    {
        PROFILE* p = pj->profiles;
        while (p)
        {
            if (!p->name || !strcmp(p->name, currentProfileName))
                break;
            p = p->next;
        }
        if (!p)
        {
            PROFILE** q = &pj->profiles;
            p = calloc(1, sizeof(PROFILE));
            p->name = strdup(currentProfileName);
            while (*q)
                q = &(*q)->next;
            p->next = *q;
            *q = p;
        }
        if (profileDebugMode)
        {
            return &p->debugSettings;
        }
        else
        {
            return &p->releaseSettings;
        }
    }
}
SETTING* GetSettings(PROFILE* pf)
{
    if (pf)
    {
        if (pf == generalProject.profiles)
        {
            return generalProject.profiles->debugSettings;
        }
        else
        {
            PROFILE* p = pf;
            while (p)
            {
                if (!p->name || !strcmp(p->name, currentProfileName))
                    break;
                p = p->next;
            }
            if (p)
            {
                if (profileDebugMode)
                    return p->debugSettings;
                else
                    return p->releaseSettings;
            }
        }
    }
    return NULL;
}
LRESULT CALLBACK SelectProfileDlgProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HWND hProfileCombo;
    HWND hReleaseTypeCombo;
    HWND htemp;
    PROFILENAMELIST* pf;
    POINT pt;
    int selected, n;
    int count;
    switch (iMessage)
    {
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            pf = profileNames;
            selected = 0;
            count = 0;
            hProfileCombo = GetDlgItem(hwnd, IDC_PROFILENAME);
            hReleaseTypeCombo = GetDlgItem(hwnd, IDC_RELEASETYPE);
            SendMessage(hProfileCombo, CB_ADDSTRING, 0, (LPARAM)sysProfileName);
            while (pf)
            {
                count++;
                if (!strcmp(pf->name, currentProfileName))
                    selected = count;
                SendMessage(hProfileCombo, CB_ADDSTRING, 0, (LPARAM)pf->name);

                pf = pf->next;
            }
            SendMessage(hProfileCombo, CB_SETCURSEL, selected, 0);
            SendMessage(hReleaseTypeCombo, CB_ADDSTRING, 0, (LPARAM) "Debug");
            SendMessage(hReleaseTypeCombo, CB_ADDSTRING, 0, (LPARAM) "Release");
            SendMessage(hReleaseTypeCombo, CB_SETCURSEL, profileDebugMode ? 0 : 1, 0);

            pt.x = 5;
            pt.y = 5;
            htemp = ChildWindowFromPoint(hProfileCombo, pt);
            SendMessage(htemp, EM_SETREADONLY, 1, 0);
            htemp = ChildWindowFromPoint(hReleaseTypeCombo, pt);
            SendMessage(htemp, EM_SETREADONLY, 1, 0);
            return 1;
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    hProfileCombo = GetDlgItem(hwnd, IDC_PROFILENAME);
                    hReleaseTypeCombo = GetDlgItem(hwnd, IDC_RELEASETYPE);
                    n = SendMessage(hProfileCombo, CB_GETCURSEL, 0, 0);
                    if (n != -1)
                    {
                        if (n == 0)
                        {
                            strcpy(currentProfileName, sysProfileName);
                        }
                        else
                        {
                            PROFILENAMELIST* pf = profileNames;
                            while (pf && --n)
                                pf = pf->next;
                            if (pf)
                            {
                                strcpy(currentProfileName, pf->name);
                            }
                        }
                        MarkChanged(workArea, TRUE);
                    }
                    n = SendMessage(hReleaseTypeCombo, CB_GETCURSEL, 0, 0);
                    if (n != -1)
                    {
                        profileDebugMode = n == 0 ? 1 : 0;
                        MarkChanged(workArea, TRUE);
                    }
                    EndDialog(hwnd, 1);
                    break;
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;
                case IDHELP:
                    ContextHelp(IDH_SELECT_PROFILE_DIALOG);
                    break;
            }
            break;
    }
    return 0;
}
void SelectProfileDialog(void) { DialogBox(hInstance, "SELECTPROFILEDLG", hwndFrame, (DLGPROC)SelectProfileDlgProc); }
void InitProps(void)
{
    PROFILE* pf;
    char name[MAX_PATH];
    strcpy(currentProfileName, sysProfileName);
    strcpy(name, szInstallPath);
    strcat(name, "\\bin\\");
    strcat(name, GENERALPROPS);
    pf = LoadRule(name);
    generalPrototype = pf;

    if (generalPrototype)
        generalProps.protocount = 1;
    generalProps.title = "General Properties";
    generalProps.saveTo = &generalProject;
    generalProps.prototype = &generalPrototype;
    InitializeCriticalSection(&propsMutex);
}

BOOL MatchesExt(char* name, char* exts)
{
    char* p = strrchr(name, '.');
    if (p)
    {
        int n = strlen(p);
        if (p && p[1] != '\\')
        {
            char* q = exts;
            while (q && *q)
            {
                char* r = strchr(q, '.');
                if (r)
                {
                    if (!strnicmp(r, p, n) && (r[n] == 0 || r[n] == ' '))
                    {
                        return TRUE;
                    }
                }
                q = strchr(q, ' ');
                if (q)
                    while (isspace(*q))
                        q++;
            }
        }
    }
    return FALSE;
}
static BOOL MatchesExtExt(char* source, char* exts)
{
    while (source && *source)
    {
        char buf[MAX_PATH];
        char* p = strchr(source, '.');
        if (p)
        {
            char* q = strchr(p, ' ');
            if (!q)
                q = p + strlen(p);
            strncpy(buf, p, q - p);
            buf[q - p] = 0;
            if (MatchesExt(buf, exts))
                return TRUE;
            p = q;
            while (isspace(*p))
                p++;
        }
        source = p;
    }
    return FALSE;
}
static int SortRules(const void* left, const void* right)
{
    SETTING* lleft = GetSettings(*(PROFILE**)left);
    SETTING* lright = GetSettings(*(PROFILE**)right);
    if ((lleft)->order < (lright)->order)
        return -1;
    return ((lleft)->order > (lright)->order);
}
static void LookupDependentRules(struct _propsData* data, char* name)
{
    BUILDRULE* br = buildRules;
    while (br)
    {
        if (br->profiles->debugSettings->select && !strcmp(name, br->profiles->debugSettings->select))
        {
            data->prototype[data->protocount++] = br->profiles;
            break;
        }
        br = br->next;
    }
}
static char* EvalDepsFind(SETTING* choice, PROJECTITEM* item, struct _propsData* data)
{
    SETTING* s = NULL;
    while (item && !s)
    {
        s = PropFind(GetSettings(item->profiles), choice->id);
        item = item->parent;
    }
    if (!s)
    {
        s = PropFindAll(data->prototype, data->protocount, choice->id);
    }
    if (s)
        return s->value;
    return NULL;
}
void EvalDependentRules(SETTING* depends, PROJECTITEM* item, struct _propsData* data)
{
    while (depends)
    {
        if (depends->type == e_choose)
        {
            char* buf = EvalDepsFind(depends, item, data);
            if (buf && !strcmp(buf, depends->value))
            {
                EvalDependentRules(depends->children, item, data);
            }
        }
        else if (depends->type == e_assign)
        {
            LookupDependentRules(data, depends->id);
        }
        depends = depends->next;
    }
}
void SelectRules(PROJECTITEM* item, struct _propsData* data)
{
    BUILDRULE* p = buildRules;
    char* cls = NULL;
    while (p)
    {
        switch (item->type)
        {
            case PJ_FILE:
                cls = "FILE";
                break;
            case PJ_PROJ:
                cls = "PROJECT";
                break;
            case PJ_WS:
                cls = "WORKAREA";
                break;
        }
        if (cls)
        {
            if (!strcmp(p->profiles->debugSettings->cls, cls))
            {
                SETTING* s = NULL;
                if (p->profiles->debugSettings->command)
                {
                    s = PropFind(p->profiles->debugSettings->command->children, "__SOURCE_EXTENSION");
                }
                if (!s || MatchesExt(item->realName, s->value))
                {
                    int i;
                    for (i = 0; i < data->protocount; i++)
                        if (data->prototype[i] == p->profiles)
                            break;
                    if (i >= data->protocount)
                    {
                        data->prototype[data->protocount++] = p->profiles;
                    }
                }
            }
        }
        p = p->next;
    }
}
static void CullBasicRules(PROJECTITEM* item, struct _propsData* data, int first)
{
    do
    {
        if (item->children)
            CullBasicRules(item->children, data, FALSE);
        SelectRules(item, data);
        item = item->next;
    } while (item && !first);
}
void GetActiveRules(PROJECTITEM* item, struct _propsData* data)
{
    BOOL done = FALSE;
    int i;
    data->protocount = 0;
    CullBasicRules(item, data, TRUE);
    for (i = 0; i < data->protocount; i++)
    {
        SETTING* set = GetSettings(data->prototype[i]);
        if (set->depends)
            EvalDependentRules(set->depends, item, data);
    }
    qsort(data->prototype, data->protocount, sizeof(PROFILE*), SortRules);
}
static void PropGetPrototype(PROJECTITEM* item, struct _propsData* data, PROFILE** arr)
{
    memset(data, 0, sizeof(*data));
    if (item == NULL || item == &generalProject)
        memcpy(data, &generalProps, sizeof(*data));
    else
    {

        data->title = "Project Properties";
        data->prototype = arr;
        data->protocount = 0;
        data->saveTo = item;
        GetActiveRules(item, data);
    }
}
static SETTING* FindSetting(SETTING* settings, SETTING* old)
{
    SETTING* rv = NULL;
    while (settings)
    {
        if (settings->type == e_tree)
        {
            if (!strcmp(settings->displayName, old->displayName))
                rv = settings;
            else
                rv = FindSetting(settings->children, old);
            if (rv)
                break;
        }
        settings = settings->next;
    }
    return rv;
}
static void PopulateTree(HWND hwnd, HTREEITEM hParent, SETTING* settings, int* first)
{
    while (settings)
    {
        if (settings->type == e_tree)
        {
            int xx = first ? *first : 0;
            TV_INSERTSTRUCT t;
            memset(&t, 0, sizeof(t));
            t.hParent = hParent;
            t.hInsertAfter = TVI_LAST;
            t.UNNAMED_UNION item.mask = TVIF_TEXT | TVIF_PARAM;
            t.UNNAMED_UNION item.hItem = 0;
            t.UNNAMED_UNION item.pszText = settings->displayName;
            t.UNNAMED_UNION item.cchTextMax = strlen(settings->displayName);
            t.UNNAMED_UNION item.lParam = (LPARAM)settings;
            settings->hTreeItem = TreeView_InsertItem(hwnd, &t);
            PopulateTree(hwnd, settings->hTreeItem, settings->children, first);
            if (first && *first && settings->children && settings->children->type != e_tree)
            {
                *first = FALSE;
                current = settings;
                populating = TRUE;
                PopulateItems(hwnd, hwndLV, current);
                populating = FALSE;
            }
            else if (xx)
            {
                TreeView_Expand(hwnd, settings->hTreeItem, TVE_EXPAND);
            }
        }
        settings = settings->next;
    }
}
static void CreateLVColumns(HWND hwnd, int width)
{
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwnd, /*LVS_EX_FULLROWSELECT | */ LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    ListView_DeleteColumn(hwnd, 1);
    ListView_DeleteColumn(hwnd, 0);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = width * 2 / 5;
    lvC.iSubItem = 0;
    lvC.pszText = "Property";
    lvC.cchTextMax = strlen("Property");
    ListView_InsertColumn(hwnd, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
    lvC.cx = width * 3 / 5;
    lvC.iSubItem = 1;
    lvC.pszText = "Value";
    lvC.cchTextMax = strlen("Value");
    ListView_InsertColumn(hwnd, 1, &lvC);
}
static void ParseFont(LOGFONT* lf, char* text)
{
    BYTE* p = (BYTE*)lf;
    int size = sizeof(*lf);
    while (size)
    {
        *p++ = strtoul(text, &text, 10);
        size--;
    }
}
static void CreateItemWindow(HWND parent, HWND lv, RECT* r, SETTING* current)
{
    switch (current->type)
    {
        SETTINGCOMBO* sc;
        char* sel;
        int i;
        case e_combo:
            sc = current->combo;
            i = 0;
            while (sc)
            {
                i++;
                sc = sc->next;
            }
            current->hWnd = CreateWindow("xccCombo", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | CBS_DROPDOWNLIST, r->left + 1,
                                         r->top, r->right - r->left - 1, r->bottom - r->top - 1, parent, 0, hInstance, 0);
            SendMessage(current->hWnd, WM_SETFONT, (WPARAM)SendMessage(lv, WM_GETFONT, 0, 0), 1);
            sc = current->combo;
            sel = NULL;
            while (sc)
            {
                SendMessage(current->hWnd, CB_ADDSTRING, 0, (LPARAM)sc->displayName);
                if (!strcmp(sc->value, current->tentative))
                    sel = sc->displayName;
                sc = sc->next;
            }
            if (sel)
                SendMessage(current->hWnd, CB_SELECTSTRING, 0, (LPARAM)sel);
            break;
        case e_text:
        case e_prependtext:
        case e_separatedtext:
        case e_multitext:
            current->hWnd = CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | ES_AUTOHSCROLL, r->left + 1, r->top,
                                         r->right - r->left - 1, r->bottom - r->top - 1, parent, 0, hInstance, 0);
            SendMessage(current->hWnd, WM_SETFONT, (WPARAM)SendMessage(lv, WM_GETFONT, 0, 0), 1);
            SendMessage(current->hWnd, WM_SETTEXT, 0, (LPARAM)current->tentative);
            SendMessage(current->hWnd, EM_SETSEL, strlen(current->tentative), strlen(current->tentative));
            break;
        case e_numeric:
            current->hWnd = CreateWindow("edit", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | ES_NUMBER, r->left + 1, r->top,
                                         r->right - r->left - 1, r->bottom - r->top - 1, parent, 0, hInstance, 0);
            SendMessage(current->hWnd, WM_SETFONT, (WPARAM)SendMessage(lv, WM_GETFONT, 0, 0), 1);
            SendMessage(current->hWnd, WM_SETTEXT, 0, (LPARAM)current->tentative);
            SendMessage(current->hWnd, EM_SETSEL, strlen(current->tentative), strlen(current->tentative));
            SendMessage(current->hWnd, EM_LIMITTEXT, 32, 0);
            break;
        case e_color:
            current->hWnd = CreateWindow("xccColor", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, r->left + 1, r->top,
                                         r->right - r->left - 1, r->bottom - r->top - 1, parent, 0, hInstance, 0);
            {
                DWORD val = strtoul(current->tentative, NULL, 0);
                SendMessage(current->hWnd, WM_SETCOLOR, 0, (LPARAM)val);
            }
            break;
        case e_font:
            current->hWnd = CreateWindow("xccFont", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, r->left + 1, r->top,
                                         r->right - r->left - 1, r->bottom - r->top - 1, parent, 0, hInstance, 0);
            {
                LOGFONT lf;
                HFONT font;
                ParseFont(&lf, current->tentative);
                font = CreateFontIndirect(&lf);
                SendMessage(current->hWnd, WM_SETFONT, (WPARAM)font, 0);
            }
            break;
        case e_printformat:
            current->hWnd = CreateWindow("xccPrintFormat", "", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS, r->left + 1, r->top,
                                         r->right - r->left - 1, r->bottom - r->top - 1, parent, 0, hInstance, 0);
            SendMessage(current->hWnd, WM_SETTEXT, 0, (LPARAM)current->tentative);
            SendMessage(current->hWnd, EM_SETSEL, strlen(current->tentative), strlen(current->tentative));
            SendMessage(current->hWnd, WM_SETFONT, (WPARAM)SendMessage(lv, WM_GETFONT, 0, 0), 1);
            break;
        default:
            break;
    }
    if (current->hWnd)
    {
        SetParent(current->hWnd, lv);
    }
}

static void SaveItemData(SETTING* current)
{
    if (current->hWnd)
    {
        switch (current->type)
        {
            DWORD color;
            HFONT font;
            LOGFONT lf;
            BYTE* p;
            char* q;
            int i;
            char buf[1024];
            SETTINGCOMBO* sc;

            case e_combo:
                buf[SendMessage(current->hWnd, WM_GETTEXT, 1024, (LPARAM)buf)] = 0;
                sc = current->combo;
                while (sc)
                {
                    if (!strcmp(sc->displayName, buf))
                    {
                        q = strdup(sc->value);
                        if (q)
                        {
                            free(current->tentative);
                            current->tentative = q;
                        }
                        break;
                    }
                    sc = sc->next;
                }
                break;
            case e_printformat:
            case e_text:
            case e_numeric:
            case e_prependtext:
            case e_separatedtext:
            case e_multitext:
                i = SendMessage(current->hWnd, WM_GETTEXTLENGTH, 0, 0);
                q = malloc(i + 1);
                if (q)
                {
                    SendMessage(current->hWnd, WM_GETTEXT, i + 1, (LPARAM)q);
                    q[i] = 0;
                    free(current->tentative);
                    current->tentative = q;
                }
                break;
            case e_font:
                font = (HFONT)SendMessage(current->hWnd, WM_GETFONT, 0, 0);
                GetObject(font, sizeof(lf), &lf);
                p = (BYTE*)&lf;
                buf[0] = 0;
                for (i = 0; i < sizeof(lf); i++)
                    sprintf(buf + strlen(buf), "%d ", p[i]);
                free(current->tentative);
                current->tentative = strdup(buf);
                break;
            case e_color:
                color = SendMessage(current->hWnd, WM_RETRIEVECOLOR, 0, 0);
                sprintf(buf, "%d", color);
                free(current->tentative);
                current->tentative = strdup(buf);
                break;
            default:
                break;
        }
    }
}
static void DestroyItemWindow(SETTING* current)
{
    if (current->hWnd)
    {
        SaveItemData(current);
        DestroyWindow(current->hWnd);
        current->hWnd = 0;
    }
}
static void SaveItemDatas(SETTING* current)
{
    if (current)
    {
        current = current->children;
        while (current)
        {
            SaveItemData(current);
            current = current->next;
        }
    }
}
static void DestroyItemWindows(SETTING* current)
{
    if (current)
    {
        current = current->children;
        while (current)
        {
            DestroyItemWindow(current);
            current = current->next;
        }
    }
}
static void PopulateItems(HWND parent, HWND hwnd, SETTING* settings)
{
    LV_ITEM item;
    int items = 0;
    ListView_DeleteAllItems(hwnd);
    settings = settings->children;
    while (settings)
    {
        memset(&item, 0, sizeof(item));
        if (settings->type != e_tree)
        {
            if (settings->type == e_separator)
            {
                item.iItem = items++;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM | LVIF_TEXT;
                item.lParam = (LPARAM)settings;
                item.pszText = "";
                item.cchTextMax = 0;
                ListView_InsertItem(hwnd, &item);
            }
            else
            {
                RECT r;
                item.iItem = items++;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM | LVIF_TEXT;
                item.lParam = (LPARAM)settings;
                item.pszText = settings->displayName;
                item.cchTextMax = strlen(settings->displayName);
                ListView_InsertItem(hwnd, &item);
                ListView_GetSubItemRect(hwnd, item.iItem, 1, LVIR_BOUNDS, &r);
                CreateItemWindow(parent, hwnd, &r, settings);
            }
        }
        settings = settings->next;
    }
}
static int TreeCustomDraw(HWND hwnd, LPNMTVCUSTOMDRAW draw)
{
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;
        case CDDS_ITEMPREPAINT:
            if (draw->nmcd.uItemState & (CDIS_SELECTED))
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
            return CDRF_NEWFONT;
        default:
            return CDRF_DODEFAULT;
    }
}
static int LVCustomDraw(HWND hwnd, LPNMLVCUSTOMDRAW draw)
{
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;
        case CDDS_ITEMPREPAINT:
            draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
            draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
            return CDRF_NEWFONT;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
            draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
            return CDRF_NEWFONT;
        default:
            return CDRF_DODEFAULT;
    }
}
void SetupCurrentValues(SETTING* setting, PROJECTITEM* saveTo)
{
    while (setting)
    {
        if (setting->type == e_tree)
        {
            SetupCurrentValues(setting->children, saveTo);
        }
        else if (setting->type != e_separator)
        {
            SETTING* value;
            EnterCriticalSection(&propsMutex);
            PropSearchProtos(saveTo, setting->id, &value);
            free(setting->tentative);
            setting->tentative = strdup(value->value);
            LeaveCriticalSection(&propsMutex);
        }
        setting = setting->next;
    }
}
void ApplyCurrentValues(SETTING* setting, PROJECTITEM* saveTo)
{
    while (setting)
    {
        if (setting->type == e_tree)
        {
            ApplyCurrentValues(setting->children, saveTo);
        }
        else if (setting->type != e_separator)
        {
            SETTING *value, *empty;
            EnterCriticalSection(&propsMutex);
            value = PropSearchProtos(saveTo, setting->id, &empty);
            if (value && !strcmp(value->value, setting->tentative))
            {
                SETTING** s = GetSettingsAddress(saveTo);
                while (*s)
                {
                    if (!strcmp((*s)->id, setting->id))
                    {
                        SETTING* p = *s;
                        *s = (*s)->next;
                        MarkChanged(saveTo, FALSE);
                        saveTo->clean = iscleanable(p->id);
                        free(p);
                        break;
                    }
                    s = &(*s)->next;
                }
            }
            else
            {
                SETTING* s = PropFind(GetSettings(saveTo->profiles), setting->id);
                if (!s)
                {
                    s = calloc(sizeof(SETTING), 1);
                    s->type = e_text;
                    s->id = strdup(setting->id);
                    InsertSetting(saveTo, s);
                }
                if (s && (!s->value || strcmp(s->value, setting->tentative)))
                {
                    free(s->value);
                    s->value = strdup(setting->tentative);
                    MarkChanged(saveTo, FALSE);
                    saveTo->clean = iscleanable(s->id);
                }
            }
            LeaveCriticalSection(&propsMutex);
        }
        setting = setting->next;
    }
}
static void PerformListViewScroll(HWND hwndLV, SETTING* setting, int delta)
{
    RECT r;
    ListView_GetSubItemRect(hwndLV, 1, 1, LVIR_BOUNDS, &r);
    setting = setting->children;
    delta *= -(r.bottom - r.top);
    while (setting)
    {
        if (setting->hWnd)
        {
            RECT pr;
            RECT r;
            GetWindowRect(hwndLV, &pr);
            GetWindowRect(setting->hWnd, &r);
            r.left -= pr.left + 1;
            r.top -= pr.top + 1;
            r.top += delta;
            MoveWindow(setting->hWnd, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
        }
        setting = setting->next;
    }
}
void RemoveCurrentValues(SETTING* setting)
{
    while (setting)
    {
        if (setting->type == e_tree)
        {
            RemoveCurrentValues(setting->children);
        }
        else if (setting->type != e_separator)
        {
            EnterCriticalSection(&propsMutex);
            free(setting->tentative);
            setting->tentative = NULL;
            LeaveCriticalSection(&propsMutex);
        }
        setting = setting->next;
    }
}
long APIENTRY NewProfileProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char* name;
    switch (message)
    {
        case WM_INITDIALOG:
            SendDlgItemMessage(hwnd, IDC_NEWPROFILE, EM_LIMITTEXT, 256, 0);
            name = (char*)lParam;
            CenterWindow(hwnd);
            return TRUE;
        case WM_COMMAND:
            switch (wParam & 0xffff)
            {
                case IDOK:
                    name[0] = 0;
                    name[SendDlgItemMessage(hwnd, IDC_NEWPROFILE, WM_GETTEXT, 256, (LPARAM)name)] = 0;
                    if (name[0])
                    {
                        SendDIDMessage(DID_PROJWND, WM_COMMAND, IDM_RESETPROFILECOMBOS, 0);
                        EndDialog(hwnd, 1);
                    }
                    else
                    {
                        ExtendedMessageBox("Error", 0, "Please enter a new profile name");
                    }
                    break;
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
    }
    return 0;
}
static BOOL GetNewProfileName(HWND hwndCombo, char* name)
{
    BOOL rv = FALSE;
    BOOL done = FALSE;
    while (!done)
    {
        if (!DialogBoxParam(hInstance, "NEWPROFILEDLG", 0, (DLGPROC)NewProfileProc, (LPARAM)name))
        {
            done = TRUE;
        }
        else
        {
            PROFILENAMELIST* pf = profileNames;
            while (pf)
            {
                if (!strcmp(pf->name, name))
                    break;
                pf = pf->next;
            }
            if (!pf)
            {
                rv = !!LookupProfileName(name);
                done = TRUE;
            }
            else
            {
                ExtendedMessageBox("Profile Name Exists", 0, "Profile name already exists.");
            }
        }
    }
    return rv;
}
static LRESULT CALLBACK GeneralWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HWND hStaticProfile;
    static HWND hStaticReleaseType;
    static HWND hProfileCombo;
    static HWND hReleaseTypeCombo;
    static HWND hAcceptBtn;
    static HWND hApplyBtn;
    static HWND hCancelBtn;
    static HWND hHelpBtn;
    static BOOL populating;
    static HWND hwndTTip;
    static int oldProtoCount;
    static SETTING* oldSettings[100];
    static int addProfileIndex;
    static char lastProfileName[256];
    static int lastDebugMode;
    int y;
    SETTING* lastSetting;
    LPCREATESTRUCT cs;
    RECT r;
    static SIZE acceptSz;
    static SIZE cancelSz;
    static SIZE helpSz;
    static POINT pt;
    static SIZE sz;
    HDC dc;
    struct _propsData* pd;
    int i;

    switch (iMessage)
    {
        int first;
        case WM_CTLCOLORSTATIC:
            //        case WM_CTLCOLORLISTBOX:
            //        case WM_CTLCOLORMSGBOX:
            //        case WM_CTLCOLOREDIT:
            //        case WM_CTLCOLORBTN:
            //        case WM_CTLCOLORDLG:
            {
                if ((HWND)lParam != hStaticProfile && (HWND)lParam != hStaticReleaseType)
                    return (LRESULT)(HBRUSH)(COLOR_WINDOW + 1);
            }
            break;
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
                NM_TREEVIEW* nm;
                TV_ITEM xx;
                LPNMCUSTOMDRAW cd;
                NMLVSCROLL* sc;
                case HDN_BEGINTRACK:
                    return TRUE;
                case LVN_BEGINSCROLL:
                    sc = (NMLVSCROLL*)lParam;
                    break;
                case LVN_ENDSCROLL:
                    sc = (NMLVSCROLL*)lParam;
                    PerformListViewScroll(hwndLV, current, sc->dy);
                    break;
                case LVN_ITEMCHANGING:
                    return TRUE;  // disable selection
                case NM_CUSTOMDRAW:
                    cd = (LPNMCUSTOMDRAW)lParam;
                    if (cd->hdr.hwndFrom == hwndTree)
                    {
                        return TreeCustomDraw(hwndTree, (LPNMTVCUSTOMDRAW)lParam);
                    }
                    else
                    {
                        return LVCustomDraw(hwndLV, (LPNMLVCUSTOMDRAW)lParam);
                    }
                case TVN_SELCHANGED:
                    nm = (NM_TREEVIEW*)lParam;
                    xx.hItem = nm->itemNew.hItem;
                    xx.mask = TVIF_PARAM;
                    if (SendMessage(hwndTree, TVM_GETITEM, 0, (LPARAM)&xx))
                    {
                        DestroyItemWindows(current);
                        current = (SETTING*)xx.lParam;
                        EnableWindow(hHelpBtn, current->helpid != 0);

                        populating = TRUE;
                        PopulateItems(hwnd, hwndLV, current);
                        populating = FALSE;
                    }
                    return 0;
            }
            break;
        case WM_COMMAND:
            pd = (struct _propsData*)GetWindowLong(hwnd, GWL_USERDATA);
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                if (LOWORD(wParam) == 1000)  // profile type
                {
                    int n = SendMessage(hProfileCombo, CB_GETCURSEL, 0, 0);
                    if (n != -1)
                    {
                        if (n == 0)
                        {
                            strcpy(currentProfileName, sysProfileName);
                        }
                        else if (n < addProfileIndex)
                        {
                            PROFILENAMELIST* pf = profileNames;
                            while (pf && --n)
                                pf = pf->next;
                            if (pf)
                            {
                                strcpy(currentProfileName, pf->name);
                            }
                        }
                        else if (n == addProfileIndex)
                        {
                            char name[MAX_PATH];
                            if (!GetNewProfileName(hwnd, name))
                            {
                                int count = 0;
                                if (strcmp(currentProfileName, sysProfileName) != 0)
                                {
                                    PROFILENAMELIST* pf = profileNames;
                                    count++;
                                    while (pf)
                                    {
                                        if (!strcmp(pf->name, currentProfileName))
                                            break;
                                        count++;
                                        pf = pf->next;
                                    }
                                    if (!pf)
                                        count = 0;
                                }
                                SendMessage(hProfileCombo, CB_SETCURSEL, count, 0);
                                break;
                            }
                            else
                            {
                                strcpy(currentProfileName, name);
                            }
                            SendMessage(hProfileCombo, CB_INSERTSTRING, addProfileIndex, (LPARAM)name);
                            SendMessage(hProfileCombo, CB_SETCURSEL, addProfileIndex++, 0);
                        }
                        if (SendMessage(hwnd, WM_COMMAND, ID_QUERYSAVE, 0))
                        {
                            MarkChanged(workArea, TRUE);
                            PropagateAllProjectTypes();
                            PostMessage(hwnd, WM_COMMAND, IDC_RELOAD, 0);
                        }
                    }
                }
                else if (LOWORD(wParam) == 1001)  // release type
                {
                    int n = SendMessage(hReleaseTypeCombo, CB_GETCURSEL, 0, 0);
                    if (n != -1)
                    {
                        if (SendMessage(hwnd, WM_COMMAND, ID_QUERYSAVE, 0))
                        {
                            profileDebugMode = n == 0 ? 1 : 0;
                            MarkChanged(workArea, TRUE);
                            PostMessage(hwnd, WM_COMMAND, IDC_RELOAD, 0);
                        }
                    }
                }

                break;
            }
            switch (LOWORD(wParam))
            {
                case IDHELP:
                    ContextHelp(current->helpid);
                    break;
                case IDOK:
                    SaveItemDatas(current);
                    for (i = 0; i < pd->protocount; i++)
                        ApplyCurrentValues(GetSettings(pd->prototype[i]), pd->saveTo);
                    if (pd == &generalProps)
                    {
                        FreeJumpSymbols();
                        LoadJumpSymbols();
                        SavePreferences();
                        ApplyEditorSettings();
                    }
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDCANCEL:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case IDC_AUXBUTTON:
                    SaveItemDatas(current);
                    for (i = 0; i < pd->protocount; i++)
                        ApplyCurrentValues(GetSettings(pd->prototype[i]), pd->saveTo);
                    if (pd == &generalProps)
                    {
                        FreeJumpSymbols();
                        LoadJumpSymbols();
                        SavePreferences();
                        ApplyEditorSettings();
                    }
                    EnableWindow(hAcceptBtn, FALSE);
                    EnableWindow(hApplyBtn, FALSE);
                    break;
                case ID_CLOSEWINDOW:
                    if (SendMessage(hwnd, WM_COMMAND, ID_QUERYSAVE, 0) != IDCANCEL)
                        SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case ID_QUERYSAVE:
                    if (IsWindowEnabled(hAcceptBtn))
                    {
                        switch (ExtendedMessageBox("Save", MB_YESNOCANCEL, "Settings have changed.  Save them?"))
                        {
                            case IDYES:
                                SaveItemDatas(current);
                                for (i = 0; i < pd->protocount; i++)
                                    ApplyCurrentValues(GetSettings(pd->prototype[i]), pd->saveTo);
                                if (pd == &generalProps)
                                {
                                    FreeJumpSymbols();
                                    LoadJumpSymbols();
                                    SavePreferences();
                                    ApplyEditorSettings();
                                }
                                break;
                            case IDNO:
                                break;
                            case IDCANCEL:
                                return 0;
                        }
                    }
                    EnableWindow(hAcceptBtn, FALSE);
                    EnableWindow(hApplyBtn, FALSE);
                    EnableWindow(hHelpBtn, FALSE);
                    return 1;
                case IDC_RELOAD:
                    DestroyItemWindows(current);
                    lastSetting = current;
                    ListView_DeleteAllItems(hwndTree);
                    TreeView_DeleteAllItems(hwndTree);
                    for (i = 0; i < oldProtoCount; i++)
                    {
                        RemoveCurrentValues(oldSettings[i]);
                    }
                    for (i = 0; i < pd->protocount; i++)
                        SetupCurrentValues(GetSettings(pd->prototype[i]), pd->saveTo);
                    for (i = 0; i < pd->protocount; i++)
                    {
                        SETTING* set = GetSettings(pd->prototype[i]);
                        PopulateTree(hwndTree, TVI_ROOT, set->children, NULL);
                    }
                    if (lastSetting)
                    {
                        for (i = 0; i < pd->protocount; i++)
                        {
                            SETTING* set = GetSettings(pd->prototype[i]);
                            set = FindSetting(set->children, lastSetting);
                            if (set)
                            {
                                current = set;
                                populating = TRUE;
                                PopulateItems(hwnd, hwndLV, current);
                                populating = FALSE;
                                break;
                            }
                        }
                    }
                    return 0;
                default:
                    if (!populating && HIWORD(wParam) == EN_CHANGE)
                    {
                        EnableWindow(hAcceptBtn, TRUE);
                        EnableWindow(hApplyBtn, TRUE);
                    }
                    else if (HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        int style = GetWindowLong((HWND)lParam, GWL_STYLE);
                        if (style & ES_NUMBER)
                        {
                            char buf[256];
                            SETTING* s = current->children;
                            GetWindowText((HWND)lParam, buf, 256);
                            while (s)
                            {
                                if (s->hWnd == (HWND)lParam)
                                    break;
                                s = s->next;
                            }
                            if (s)
                            {
                                if (buf[0])
                                {
                                    int low, high;
                                    int n = strtoul(buf, NULL, 0);
                                    low = strtoul(s->lowLimit, NULL, 0);
                                    high = strtoul(s->highLimit, NULL, 0);
                                    if (n < low || n > high)
                                    {
                                        TOOLINFO t;
                                        static char text[256];
                                        RECT r, r1;
                                        SetFocus((HWND)lParam);
                                        sprintf(text, "The numeric range of this tool is %d to %d", low, high);
                                        GetWindowRect(s->hWnd, &r1);
                                        GetWindowRect(hwndLV, &r);
                                        OffsetRect(&r1, -r.left, -r.top);
                                        memset(&t, 0, sizeof(t));
                                        t.cbSize = sizeof(t);
                                        t.uFlags = TTF_SUBCLASS | TTF_ABSOLUTE | TTF_TRACK;
                                        t.hwnd = hwndLV;
                                        t.uId = 1000;
                                        t.rect = r1;
                                        //                                                 t.rect.left = 0;
                                        //                                                 t.rect.right = 100;
                                        //                                                 t.rect.top = 0;
                                        //                                                 t.rect.bottom = 32;
                                        t.lpszText = text;
                                        SendMessage(hwndTTip, TTM_DELTOOL, 0, (LPARAM)&t);
                                        SendMessage(hwndTTip, TTM_ADDTOOL, 0, (LPARAM)&t);
                                        SendMessage(hwndTTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 1000);
                                        SendMessage(hwndTTip, TTM_SETDELAYTIME, TTDT_INITIAL, 0);
                                        SendMessage(hwndTTip, TTM_ACTIVATE, 0, 0);
                                        SendMessage(hwndTTip, TTM_TRACKACTIVATE, TRUE, 0);
                                        SendMessage(hwndTTip, TTM_TRACKPOSITION, 0, MAKELPARAM(r1.bottom, r1.left + 10));
                                        SendMessage(hwndTTip, TTM_POPUP, 0, 0);
                                    }
                                }
                                else
                                {
                                    SetWindowText((HWND)lParam, s->value);
                                }
                            }
                        }
                    }
                    break;
            }
            break;
        case WM_CREATE:
            cs = ((LPCREATESTRUCT)lParam);
            pd = (struct _propsData*)cs->lpCreateParams;
            strcpy(lastProfileName, currentProfileName);
            lastDebugMode = profileDebugMode;
            SetWindowLong(hwnd, GWL_USERDATA, (long)pd);
            populating = FALSE;
            current = NULL;
            hwndTTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_BALLOON, CW_USEDEFAULT,
                                      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL,
                                      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

            SetWindowPos(hwndTTip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            CenterWindow(hwnd);
            GetClientRect(hwnd, &r);
            if (pd == &generalProps)
            {
                y = 0;
            }
            else
            {
                y = 24;
            }
            if (y)
            {
                HWND htemp;
                POINT pt = {5, 5};
                PROFILENAMELIST* pf = profileNames;
                int selected = 0, count = 0;
                hStaticProfile =
                    CreateWindowEx(0, "static", "Profile Name:", WS_VISIBLE | WS_CHILD, 10, 4, 100, 16, hwnd, 0, hInstance, NULL);
                ApplyDialogFont(hStaticProfile);
                hStaticReleaseType =
                    CreateWindowEx(0, "static", "Build Type:", WS_VISIBLE | WS_CHILD, 280, 4, 100, 16, hwnd, 0, hInstance, NULL);
                ApplyDialogFont(hStaticReleaseType);
                hProfileCombo = CreateWindowEx(0, "combobox", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_AUTOHSCROLL, 110, 0,
                                               150, 20, hwnd, (HMENU)1000, hInstance, NULL);
                ApplyDialogFont(hProfileCombo);
                hReleaseTypeCombo = CreateWindowEx(0, "combobox", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_AUTOHSCROLL, 380,
                                                   0, 150, 20, hwnd, (HMENU)1001, hInstance, NULL);
                ApplyDialogFont(hReleaseTypeCombo);
                SendMessage(hProfileCombo, CB_ADDSTRING, 0, (LPARAM)sysProfileName);
                count++;
                while (pf)
                {
                    if (!strcmp(pf->name, currentProfileName))
                        selected = count;
                    SendMessage(hProfileCombo, CB_ADDSTRING, 0, (LPARAM)pf->name);

                    count++;
                    pf = pf->next;
                }
                addProfileIndex = count;
                SendMessage(hProfileCombo, CB_ADDSTRING, 0, (LPARAM) "Add New Profile...");
                SendMessage(hProfileCombo, CB_SETCURSEL, selected, 0);
                SendMessage(hReleaseTypeCombo, CB_ADDSTRING, 0, (LPARAM) "Debug");
                SendMessage(hReleaseTypeCombo, CB_ADDSTRING, 0, (LPARAM) "Release");
                SendMessage(hReleaseTypeCombo, CB_SETCURSEL, profileDebugMode ? 0 : 1, 0);

                htemp = ChildWindowFromPoint(hProfileCombo, pt);
                SendMessage(htemp, EM_SETREADONLY, 1, 0);
                htemp = ChildWindowFromPoint(hReleaseTypeCombo, pt);
                SendMessage(htemp, EM_SETREADONLY, 1, 0);
            }
            hwndTree = CreateWindowEx(0, WC_TREEVIEW, "",
                                      WS_VISIBLE | WS_BORDER | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |
                                          TVS_TRACKSELECT,
                                      0, y, (r.right - r.left) * 3 / 10, r.bottom - r.top - 32 - 24 - y, hwnd, 0, hInstance, NULL);
            hwndLV = CreateWindowEx(0, WC_LISTVIEW, "", WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | WS_CHILD,
                                    (r.right - r.left) * 3 / 10, y, (r.right - r.left) * 7 / 10, r.bottom - r.top - 32 - 24 - y,
                                    hwnd, 0, hInstance, NULL);
            dc = GetDC(hwnd);
            GetTextExtentPoint32(dc, szAccept, strlen(szAccept), &acceptSz);
            GetTextExtentPoint32(dc, szClose, strlen(szClose), &cancelSz);
            GetTextExtentPoint32(dc, szApply, strlen(szApply), &sz);
            GetTextExtentPoint32(dc, szHelp, strlen(szClose), &helpSz);
            ReleaseDC(hwnd, dc);
            if (acceptSz.cx > sz.cx)
                sz.cx = acceptSz.cx;
            if (acceptSz.cy > sz.cy)
                sz.cy = acceptSz.cy;
            if (cancelSz.cx > sz.cx)
                sz.cx = cancelSz.cx;
            if (cancelSz.cy > sz.cy)
                sz.cy = cancelSz.cy;
            if (helpSz.cx > sz.cx)
                sz.cx = helpSz.cx;
            if (helpSz.cy > sz.cy)
                sz.cy = helpSz.cy;
            pt.x = sz.cx;
            pt.y = sz.cy;
            hAcceptBtn = CreateWindowEx(0, "button", szAccept, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, r.right - 1 * (sz.cx + 32),
                                        r.bottom - sz.cy - 12, sz.cx + 24, sz.cy + 8, hwnd, (HMENU)IDOK, hInstance, NULL);
            SendMessage(hAcceptBtn, WM_SETFONT, (WPARAM)SendMessage(hwndLV, WM_GETFONT, 0, 0), 1);
            hCancelBtn = CreateWindowEx(0, "button", szClose, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, r.right - 2 * (sz.cx + 32),
                                        r.bottom - sz.cy - 12, sz.cx + 24, sz.cy + 8, hwnd, (HMENU)IDCANCEL, hInstance, NULL);
            SendMessage(hCancelBtn, WM_SETFONT, (WPARAM)SendMessage(hwndLV, WM_GETFONT, 0, 0), 1);
            hApplyBtn = CreateWindowEx(0, "button", szApply, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, r.right - 3 * (sz.cx + 32),
                                       r.bottom - sz.cy - 12, sz.cx + 24, sz.cy + 8, hwnd, (HMENU)IDC_AUXBUTTON, hInstance, NULL);
            SendMessage(hApplyBtn, WM_SETFONT, (WPARAM)SendMessage(hwndLV, WM_GETFONT, 0, 0), 1);
            hHelpBtn = CreateWindowEx(0, "button", szHelp, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, r.right - 5 * (sz.cx + 32),
                                      r.bottom - sz.cy - 12, sz.cx + 24, sz.cy + 8, hwnd, (HMENU)IDHELP, hInstance, NULL);
            SendMessage(hHelpBtn, WM_SETFONT, (WPARAM)SendMessage(hwndLV, WM_GETFONT, 0, 0), 1);
            EnableWindow(hAcceptBtn, FALSE);
            EnableWindow(hApplyBtn, FALSE);
            EnableWindow(hHelpBtn, FALSE);
            oldProtoCount = pd->protocount;
            for (i = 0; i < pd->protocount; i++)
                SetupCurrentValues(oldSettings[i] = GetSettings(pd->prototype[i]), pd->saveTo);
            CreateLVColumns(hwndLV, (r.right - r.left) * 7 / 10);
            first = TRUE;
            for (i = 0; i < pd->protocount; i++)
            {
                SETTING* set = GetSettings(pd->prototype[i]);
                PopulateTree(hwndTree, TVI_ROOT, set->children, &first);
            }
            return 0;
        case WM_CLOSE:
            pd = (struct _propsData*)GetWindowLong(hwnd, GWL_USERDATA);
            if (pd != &generalProps)
            {
                CalculateProjectDepends(pd->saveTo);
                SetStatusMessage("", FALSE);
            }
            ResyncProjectIcons();
            break;
        case WM_DESTROY:
            pd = (struct _propsData*)GetWindowLong(hwnd, GWL_USERDATA);
            DestroyItemWindows(current);
            for (i = 0; i < pd->protocount; i++)
            {
                RemoveCurrentValues(GetSettings(pd->prototype[i]));
            }
            if (pd != &generalProps)
            {
                free(pd->prototype);
                free(pd);
            }
            DestroyWindow(hwndLV);
            DestroyWindow(hwndTree);
            DestroyWindow(hAcceptBtn);
            DestroyWindow(hCancelBtn);
            DestroyWindow(hApplyBtn);
            if (pd == &generalProps)
            {
                DestroyWindow(hStaticProfile);
                DestroyWindow(hStaticReleaseType);
                DestroyWindow(hProfileCombo);
                DestroyWindow(hReleaseTypeCombo);
            }
            strcpy(currentProfileName, lastProfileName);
            profileDebugMode = lastDebugMode;
            hwndGeneralProps = NULL;
            break;
        case WM_SIZE:
            if (current)
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                MoveWindow(hwndTree, 0, 24, x * 3 / 10, y - 32 - 24, 1);
                MoveWindow(hwndLV, x * 3 / 10, 24, x * 7 / 10, y - 32 - 24, 1);
                MoveWindow(hAcceptBtn, x - 3 * (pt.x + 20), y - pt.y - 12, pt.x + 12, pt.y + 8, 1);
                MoveWindow(hCancelBtn, x - 2 * (pt.x + 20), y - pt.y - 12, pt.x + 12, pt.y + 8, 1);
                MoveWindow(hApplyBtn, x - 1 * (pt.x + 20), y - pt.y - 12, pt.x + 12, pt.y + 8, 1);
                CreateLVColumns(hwndLV, x * 7 / 10);
                DestroyItemWindows(current);
                populating = TRUE;
                PopulateItems(hwnd, hwndLV, current);
                populating = FALSE;
            }
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
struct buttonWindow
{
    HWND parent;
    HWND button;
    HWND edit;
    HWND list;
    HBRUSH brush;
    HFONT font;
    int lfHeight;
    DWORD color;
};
struct buttonWindow* CreateButtonWnd(HWND parent, BOOL staticText, BOOL combo)
{
    struct buttonWindow* ptr = calloc(1, sizeof(struct buttonWindow));
    if (ptr)
    {
        RECT r;
        GetClientRect(parent, &r);
        ptr->button = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_FLAT, r.right - 18, r.top, 18,
                                   r.bottom - r.top, parent, (HMENU)IDC_AUXBUTTON, hInstance, NULL);
        if (combo)
            SendMessage(ptr->button, BM_SETIMAGE, 0, (LPARAM)comboButtonBM);
        else
            SendMessage(ptr->button, BM_SETIMAGE, 0, (LPARAM)menuButtonBM);
        if (staticText)
        {
            ptr->edit = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, r.left, r.top, r.right - r.left - 18, r.bottom - r.top,
                                     parent, 0, hInstance, NULL);
        }
        else
        {
            ptr->edit = CreateWindow("edit", "", WS_CHILD | WS_VISIBLE, r.left, r.top, r.right - r.left - 18, r.bottom - r.top,
                                     parent, 0, hInstance, NULL);
        }
        ApplyDialogFont(ptr->edit);
        ptr->parent = GetParent(parent);
    }
    return ptr;
}
static LRESULT CALLBACK ColorWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    struct buttonWindow* ptr;
    if (iMessage == helpMsg)
        ContextHelp(IDH_CHOOSE_COLOR_DIALOG);
    else
        switch (iMessage)
        {
            CHOOSECOLOR c;
            case WM_LBUTTONUP:
                SendMessage(hwnd, WM_COMMAND, IDC_AUXBUTTON, 0);
                break;
            case WM_COMMAND:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                if (LOWORD(wParam) == IDC_AUXBUTTON)
                {
                    memset(&c, 0, sizeof(c));
                    c.lStructSize = sizeof(CHOOSECOLOR);
                    c.hwndOwner = hwnd;
                    c.rgbResult = GetWindowLong(hwnd, 0);
                    c.lpCustColors = custColors;
                    c.Flags = CC_RGBINIT | CC_SHOWHELP;
                    if (ChooseColor(&c))
                    {
                        ptr->color = c.rgbResult;
                        InvalidateRect(ptr->edit, 0, 0);
                        SendMessage(ptr->parent, WM_COMMAND, EN_CHANGE << 16, 0);
                    }
                    else
                        CommDlgExtendedError();
                }
                break;
            case WM_CTLCOLORBTN:
            {
                HBRUSH br = CreateSolidBrush(RetrieveSysColor(COLOR_WINDOW));
                return (LRESULT)br;
            }
            case WM_CTLCOLORSTATIC:
            {
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                if (ptr->brush)
                    DeleteObject(ptr->brush);
                ptr->brush = CreateSolidBrush(ptr->color);
                SetBkColor((HDC)wParam, ptr->color);
                return (LRESULT)ptr->brush;
            }
            case WM_SETCOLOR:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                ptr->color = lParam;
                InvalidateRect(hwnd, 0, 0);
                break;
            case WM_RETRIEVECOLOR:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                return ptr->color;
            case WM_CREATE:
                ptr = CreateButtonWnd(hwnd, TRUE, FALSE);
                SetWindowLong(hwnd, 0, (long)ptr);
                InvalidateRect(hwnd, 0, 0);
                break;
            case WM_DESTROY:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                DeleteObject(ptr->brush);
                break;
        }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static LRESULT CALLBACK FontWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    struct buttonWindow* ptr;
    if (iMessage == helpMsg)
        ContextHelp(IDH_CHOOSE_FONT_DIALOG);
    else
        switch (iMessage)
        {
            case WM_CREATE:
                ptr = CreateButtonWnd(hwnd, TRUE, FALSE);
                SetWindowLong(hwnd, 0, (long)ptr);
                ptr->font = CreateFontIndirect(&systemDialogFont);
                ptr->lfHeight = systemDialogFont.lfHeight;
                SendMessage(hwnd, WM_COMMAND, IDC_SETFONTNAME, 0);
                break;
            case WM_COMMAND:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                switch (LOWORD(wParam))
                {
                    CHOOSEFONT ch;
                    LOGFONT lf;
                    case IDC_AUXBUTTON:
                        memset(&ch, 0, sizeof(ch));
                        GetObject(ptr->font, sizeof(lf), &lf);
                        ch.lStructSize = sizeof(ch);
                        ch.hwndOwner = hwnd;
                        ch.lpLogFont = &lf;
                        ch.Flags = CF_FIXEDPITCHONLY | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_SHOWHELP;
                        if (ChooseFont(&ch))
                        {
                            ptr->font = CreateFontIndirect(&lf);
                            SendMessage(ptr->parent, WM_COMMAND, EN_CHANGE << 16, 0);
                            SendMessage(hwnd, WM_COMMAND, IDC_SETFONTNAME, 0);
                        }
                        break;
                    case IDC_SETFONTNAME:
                    {
                        LOGFONT lf;
                        HFONT font, oldFont;
                        char buf[256];
                        HDC dc;
                        GetObject(ptr->font, sizeof(lf), &lf);
                        dc = GetDC(hwnd);
                        sprintf(buf, "%s (%d)", lf.lfFaceName, MulDiv(-lf.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSX)));
                        ReleaseDC(hwnd, dc);
                        lf.lfHeight = ptr->lfHeight;
                        font = CreateFontIndirect(&lf);
                        oldFont = (HFONT)SendMessage(ptr->edit, WM_GETFONT, 0, 0);
                        SendMessage(ptr->edit, WM_SETFONT, (WPARAM)font, 0);
                        DeleteObject(oldFont);
                        SendMessage(ptr->edit, WM_SETTEXT, 0, (LPARAM)buf);
                        break;
                    }
                }
                break;
            case WM_SETFONT:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                ptr->font = (HFONT)wParam;
                SendMessage(hwnd, WM_COMMAND, IDC_SETFONTNAME, 0);
                break;
            case WM_GETFONT:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                return (LRESULT)ptr->font;
            case WM_CTLCOLORBTN:
            case WM_CTLCOLORSTATIC:
            {
                HBRUSH br = CreateSolidBrush(RetrieveSysColor(COLOR_WINDOW));
                SetBkColor((HDC)wParam, RetrieveSysColor(COLOR_WINDOW));
                SetTextColor((HDC)wParam, RetrieveSysColor(COLOR_WINDOWTEXT));
                return (LRESULT)br;
            }
            case WM_DESTROY:
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                DestroyWindow(ptr->edit);
                DestroyWindow(ptr->button);
                DeleteObject(ptr->font);
                free(ptr);
                break;
        }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

static LRESULT CALLBACK PrintFormatWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    struct buttonWindow* ptr;
    switch (iMessage)
    {
        char* insert;
        case WM_CREATE:
            SetWindowLong(hwnd, 0, (long)CreateButtonWnd(hwnd, FALSE, FALSE));
            break;
        case WM_COMMAND:
            insert = NULL;
            switch (LOWORD(wParam))
            {
                case IDC_AUXBUTTON:
                {
                    HMENU menu = LoadMenuGeneric(hInstance, "PRINTFORMATMENU"), popup;
                    RECT r;
                    GetWindowRect(hwnd, &r);
                    popup = GetSubMenu(menu, 0);
                    InsertBitmapsInMenu(popup);
                    TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON, r.right, r.top, hwnd, NULL);
                    DestroyMenu(menu);
                }
                break;
                case IDM_PF12HR:
                    insert = "%T";
                    break;
                case IDM_PF24HR:
                    insert = "%t";
                    break;
                case IDM_PFUSADATE:
                    insert = "%D";
                    break;
                case IDM_PFEURODATE:
                    insert = "%d";
                    break;
                case IDM_PFCURPAGE:
                    insert = "%P";
                    break;
                case IDM_PFNUMPAGE:
                    insert = "%#";
                    break;
                case IDM_PFFILENAME:
                    insert = "%F";
                    break;
                case IDM_PFCENTER:
                    insert = "%C";
                    break;
                case IDM_PFLEFT:
                    insert = "%L";
                    break;
                case IDM_PFRIGHT:
                    insert = "%R";
                    break;
                default:
                    if (HIWORD(wParam) == EN_CHANGE)
                    {
                        ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                        SendMessage(ptr->parent, WM_COMMAND, EN_CHANGE << 16, 0);
                    }
                    break;
            }
            if (insert)
            {
                ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
                SendMessage(ptr->edit, EM_REPLACESEL, 1, (LPARAM)insert);
                SendMessage(ptr->parent, WM_COMMAND, EN_CHANGE << 16, 0);
            }
            break;
        case WM_SETTEXT:
        case WM_GETTEXT:
        case WM_GETTEXTLENGTH:
        case EM_SETSEL:
        case WM_SETFONT:
        case WM_GETFONT:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            return SendMessage(ptr->edit, iMessage, wParam, lParam);

        case WM_CTLCOLORBTN:
        {
            HBRUSH br = CreateSolidBrush(RetrieveSysColor(COLOR_WINDOW));
            return (LRESULT)br;
        }
        case WM_DESTROY:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            DestroyWindow(ptr->edit);
            DestroyWindow(ptr->button);
            DeleteObject(ptr->font);
            free(ptr);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static LRESULT CALLBACK ComboWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    struct buttonWindow* ptr;
    switch (iMessage)
    {
        RECT r, rc;
        int n;
        TEXTMETRIC t;
        HDC hDC;
        case WM_CREATE:
            ptr = CreateButtonWnd(hwnd, TRUE, TRUE);
            SetWindowLong(hwnd, 0, (long)ptr);
            GetClientRect(hwnd, &r);
            ptr->list = CreateWindow("listbox", 0, WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER, r.left, r.bottom, 16,
                                     r.right - r.left, hwnd, (HMENU)IDC_LISTBOX, hInstance, 0);
            break;
        case WM_COMMAND:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case IDC_LISTBOX:
                    if (HIWORD(wParam) == LBN_SELCHANGE)
                    {
                        int n = SendMessage(ptr->list, LB_GETCURSEL, 0, 0);
                        char buf[256];
                        SendMessage(ptr->list, LB_GETTEXT, n, (LPARAM)buf);
                        SendMessage(ptr->edit, WM_SETTEXT, 0, (LPARAM)buf);
                        SendMessage(ptr->parent, WM_COMMAND, EN_CHANGE << 16, 0);
                    }
                    else
                        break;
                    // fallthrough
                case IDC_AUXBUTTON:
                {
                    GetClientRect(hwnd, &r);
                    GetClientRect(ptr->list, &rc);
                    if (IsWindowVisible(ptr->list))
                    {
                        SetWindowPos(hwnd, 0, r.left, r.top, r.right - r.left,
                                     r.bottom - r.top - rc.bottom + rc.top - GetSystemMetrics(SM_CYBORDER) * 2,
                                     SWP_NOACTIVATE | SWP_NOMOVE);
                    }
                    else
                    {
                        SetWindowPos(hwnd, 0, r.left, r.top, r.right - r.left,
                                     r.bottom - r.top + rc.bottom - rc.top + GetSystemMetrics(SM_CYBORDER) * 2,
                                     SWP_NOACTIVATE | SWP_NOMOVE);
                    }
                    SetWindowPos(ptr->list, HWND_TOP, 0, 0, 0, 0,
                                 SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE |
                                     (IsWindowVisible(ptr->list) ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));
                    break;
                }
            }
            break;
        case WM_LBUTTONDOWN:
            SendMessage(hwnd, WM_COMMAND, IDC_AUXBUTTON, 0);
            break;
        case CB_ADDSTRING:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            SendMessage(ptr->list, LB_ADDSTRING, wParam, lParam);
            n = SendMessage(ptr->list, LB_GETCOUNT, 0, 0);
            hDC = GetDC(hwnd);
            GetTextMetrics(hDC, &t);
            GetClientRect(hwnd, &r);
            GetClientRect(ptr->list, &rc);
            rc.bottom = (t.tmHeight) * n;
            SetWindowPos(ptr->list, NULL, 0, 0, r.right - r.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            ReleaseDC(hwnd, hDC);
            break;
        case CB_SELECTSTRING:
        case WM_SETTEXT:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            SendMessage(ptr->edit, WM_SETTEXT, wParam, lParam);
            break;
        case WM_GETTEXT:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            return SendMessage(ptr->edit, WM_GETTEXT, wParam, lParam);
        case WM_GETTEXTLENGTH:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            return SendMessage(ptr->edit, WM_GETTEXTLENGTH, wParam, lParam);
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORSTATIC:
        {
            HBRUSH br = CreateSolidBrush(RetrieveSysColor(COLOR_WINDOW));
            SetBkColor((HDC)wParam, RetrieveSysColor(COLOR_WINDOW));
            SetTextColor((HDC)wParam, RetrieveSysColor(COLOR_WINDOWTEXT));
            return (LRESULT)br;
        }
        case WM_DESTROY:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            DestroyWindow(ptr->edit);
            DestroyWindow(ptr->button);
            DestroyWindow(ptr->list);
            free(ptr);
            break;
        case WM_SETFONT:
            ptr = (struct buttonWindow*)GetWindowLong(hwnd, 0);
            SendMessage(ptr->edit, iMessage, wParam, lParam);
            SendMessage(ptr->list, iMessage, wParam, lParam);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

void RegisterPropWindows(HINSTANCE hInstance)
{

    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &GeneralWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(LPVOID);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szGeneralPropsClassName;
    RegisterClass(&wc);

    wc.lpfnWndProc = ColorWndProc;
    wc.lpszClassName = "xccColor";
    RegisterClass(&wc);
    wc.lpfnWndProc = FontWndProc;
    wc.lpszClassName = "xccFont";
    RegisterClass(&wc);
    wc.lpfnWndProc = PrintFormatWndProc;
    wc.lpszClassName = "xccPrintFormat";
    RegisterClass(&wc);
    wc.lpfnWndProc = ComboWndProc;
    wc.lpszClassName = "xccCombo";
    RegisterClass(&wc);
    menuButtonBM = LoadBitmap(hInstance, "ID_MENUBM");
    menuButtonBM = ConvertToTransparent(menuButtonBM, 0xc0c0c0);
    comboButtonBM = LoadBitmap(hInstance, "ID_COMBOBM");
    comboButtonBM = ConvertToTransparent(comboButtonBM, 0xc0c0c0);
}
void ShowGeneralProperties(void)
{
    if (!hwndGeneralProps)
    {
        hwndGeneralProps = CreateWindow(szGeneralPropsClassName, generalProps.title,
                                        WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU | WS_DLGFRAME, 0, 0, 700, 500, 0, 0,
                                        hInstance, (LPVOID)&generalProps);
    }
}
void ShowBuildProperties(PROJECTITEM* projectItem)
{
    if (hwndGeneralProps)
        SendMessage(hwndGeneralProps, WM_COMMAND, ID_CLOSEWINDOW, 0);
    if (!hwndGeneralProps)
    {
        PROFILE** arr = calloc(sizeof(PROFILE*), 100);
        if (arr)
        {
            static char title[512];
            struct _propsData* data = calloc(sizeof(struct _propsData), 1);
            if (!data)
            {
                free(arr);
                return;
            }
            PropGetPrototype(projectItem, data, arr);
            if (data->title != generalProps.title)
            {
                data->title = title;
                sprintf(title, "Build properties for %s", projectItem->displayName);
            }
            hwndGeneralProps = CreateWindow(szGeneralPropsClassName, data->title,
                                            WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, 0, 0, 724, 500,
                                            0, 0, hInstance, (LPVOID)data);
        }
    }
}
SETTING* PropFind(SETTING* props, char* id)
{
    SETTING* rv = NULL;
    while (props && !rv)
    {
        switch (props->type)
        {
            case e_tree:
                rv = PropFind(props->children, id);

                break;
            case e_separator:
                break;
            default:
                if (props->type >= e_values)
                    if (!strcmp(props->id, id))
                    {
                        rv = props;
                    }
                break;
        }
        props = props->next;
    }
    return rv;
}
static SETTING* PropFindAll(PROFILE** list, int count, char* id)
{
    int i;
    SETTING* setting = NULL;
    for (i = 0; i < count; i++)
        if ((setting = PropFind(GetSettings(list[i]), id)) != NULL)
            break;
    return setting;
}
SETTING* PropSearchProtos(PROJECTITEM* item, char* id, SETTING** value)
{
    PROFILE* arr[100];
    struct _propsData data;
    SETTING* setting;
    PropGetPrototype(item, &data, arr);
    EnterCriticalSection(&propsMutex);
    *value = NULL;
    if (!item)
    {
        *value = PropFind(GetSettings(data.saveTo->profiles), id);
    }
    else
    {
        while (item && !*value)
        {
            *value = PropFind(GetSettings(item->profiles), id);
            item = item->parent;
        }
    }
    setting = PropFindAll(data.prototype, data.protocount, id);
    if (!*value)
    {
        *value = setting;
    }
    LeaveCriticalSection(&propsMutex);
    return setting;
}
void PropGetFont(PROJECTITEM* item, char* id, LPLOGFONT lplf)
{
    SETTING* value;
    SETTING* setting = PropSearchProtos(item, id, &value);
    if (value)
    {
        ParseFont(lplf, value->value);
    }
    else
    {
        MyMessageBox("Unknown setting", 0, "unknown setting %s", id);
    }
}
BOOL PropGetBool(PROJECTITEM* item, char* id)
{
    SETTING* value;
    SETTING* setting = PropSearchProtos(item, id, &value);
    if (value)
    {
        return atoi(value->value);
    }
    else
    {
        MyMessageBox("Unknown setting", 0, "unknown setting %s", id);
    }
    return 0;
}
int PropGetInt(PROJECTITEM* item, char* id)
{
    SETTING* value;
    SETTING* setting = PropSearchProtos(item, id, &value);
    if (value)
    {
        return strtoul(value->value, NULL, 0);
    }
    else
    {
        MyMessageBox("Unknown setting", 0, "unknown setting %s", id);
    }
    return 0;
}
void PropGetString(PROJECTITEM* item, char* id, char* string, int len)
{
    SETTING* value;
    SETTING* setting = PropSearchProtos(item, id, &value);
    if (value)
    {
        strncpy(string, value->value, len - 1);
        string[len - 1] = 0;
    }
    else
    {
        MyMessageBox("Unknown setting", 0, "unknown setting %s", id);
    }
}
COLORREF PropGetColor(PROJECTITEM* item, char* id)
{
    SETTING* value;
    SETTING* setting = PropSearchProtos(item, id, &value);
    if (value)
    {
        return strtoul(value->value, NULL, 0);
    }
    else
    {
        MyMessageBox("Unknown setting", 0, "unknown setting %s", id);
    }
    return 0;
}