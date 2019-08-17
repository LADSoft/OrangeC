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
#include <process.h>
#include "helpid.h"
#include "header.h"
#include "regexp.h"
#include "idewinconst.h"
#include <ctype.h>

#define EDITOR_OFFSET 35

extern HINSTANCE hInstance;
extern HWND hwndClient;
extern HWND hwndTbFind;
extern HWND hwndFrame;
extern char highlightText[256];
extern int highlightCaseSensitive;
extern int highlightWholeWord;
extern DWINFO* editWindows;
extern PROJECTITEM* activeProject;
extern PROJECTITEM* workArea;
extern char szInstallPath[];

DWORD BrowseForFile(HWND hwnd, LPSTR pszDisplayName, LPSTR pszPath, UINT cchPath);

HWND hwndFind = NULL, hwndFindInternal = NULL;
int modifiedFind;

char* findhist[MAX_COMBO_HISTORY];
char* replacehist[MAX_COMBO_HISTORY];
char* findbrowsehist[MAX_COMBO_HISTORY];
char* replacebrowsehist[MAX_COMBO_HISTORY];

POINT findDlgPos;
BOOL finding = TRUE;

static BOOL infindorreplace;
static BOOL dirty;
static BOOL findFromTB;
static BOOL replacing = FALSE;
static BOOL usingfif;
static char browsePath[MAX_PATH];
static char fileType[MAX_PATH] = "*.*";
static BOOL canceled = FALSE;
static BOOL inSetSel;
static int replaceCount;
static int matchCount;

int findflags = F_DOWN | F_OPENWINDOW | F_SUBDIR;
int replaceflags = F_DOWN | F_OPENWINDOW | F_SUBDIR;
int findmode = F_M_OPENDOCUMENTS;
int replacemode = F_M_CURRENTDOCUMENT;
int findext = 4;
int replaceext = 0;

typedef struct _bufList
{
    struct _bufList *fwd, *back;
    DWINFO* ptr;
    CHARRANGE range;
} BUFLIST;

char findText[256];

static char replaceText[256];

static CHARRANGE findPos, replacePos;
static char* currentBuffer;
static DWINFO* currentWindow;
static BUFLIST* opened;
static char* headerExt = ".h;.hxx";

static HANDLE findEvent;
void findInit(void) { findEvent = CreateEvent(0, FALSE, TRUE, 0); }
//-------------------------------------------------------------------------

static void nm_matches(char* search, int flags, char* buf, CHARRANGE* pos)
{
    if (flags & F_DOWN)
    {
        char* found;
        if (flags & FR_MATCHCASE)
            found = strstr(buf + pos->cpMin, search);
        else
            found = stristr(buf + pos->cpMin, search);
        if (found)
            pos->cpMin = found - buf;
        else
            pos->cpMin = pos->cpMax;
    }
    else
    {
        int l = strlen(search);
        do
        {
            if (flags & F_MATCHCASE)
            {
                if (!strncmp(buf + pos->cpMax, search, l))
                    break;
            }
            else if (!strnicmp(buf + pos->cpMax, search, l))
                break;
            pos->cpMax--;
        } while (pos->cpMax >= pos->cpMin && !canceled);
    }
}
static int getSegment(char* result, char** source)
{
    int n = 0;
    while (**source && **source != '?' && **source != '*')
    {
        *result++ = *(*source)++;
        n++;
    }
    *result = 0;
    return n;
}
static int matches_wc(char* search, int flags, char* buf, int pos, int* len)
{
    /* this assumes that a '*' will be prepended to whatever they ask for */
    int start = -1;
    char segment[1024];
    char* orgsearch = search;
    int orgpos = pos;

    while (pos != -1 && *search && !canceled)
    {
        if (search == orgsearch)
        {
            orgpos = pos + 1;
            start = -1;
        }
        if (*search == '*')
        {
            // this is a simplification, probably we ought to count the '?'
            // chars and make sure the match is at least that many...
            while (*search == '*' || *search == '?')
                search++;
            if (*search)
            {
                int l = getSegment(segment, &search);
                char *q, *p;
                p = strchr(buf + pos, '\n');

                if (flags & FR_MATCHCASE)
                    q = strstr(buf + pos, segment);
                else
                    q = stristr(buf + pos, segment);
                if (q)
                {
                    if (p && q > p)
                    {
                        pos = p - buf;
                        search = orgsearch;
                        if (!(flags & F_DOWN))
                            break;
                    }
                    else
                    {
                        if (start == -1)
                            start = pos;
                        pos = q - buf + l;
                    }
                }
                else
                {
                    pos = -1;
                }
            }
        }
        else if (*search == '?')
        {
            if (start == -1)
                start = pos;
            if (!buf[pos++])
                pos = -1;
            else
                search++;
        }
        else
        {
            char* q;
            if (search == orgsearch)
            {
                int l = getSegment(segment, &search);
                if (flags & FR_DOWN)
                {
                    if (flags & FR_MATCHCASE)
                        q = strstr(buf + pos, segment);
                    else
                        q = stristr(buf + pos, segment);
                }
                else
                {
                    q = 0;
                    if (flags & FR_MATCHCASE)
                    {
                        if (!strncmp(buf + pos, segment, l))
                            q = buf + pos;
                    }
                    else
                    {
                        if (!strnicmp(buf + pos, segment, l))
                            q = buf + pos;
                    }
                }
                if (!q)
                    pos = -1;
                else
                {
                    pos = q - buf;
                    if (start == -1)
                        start = pos;
                    orgpos = pos + 1;
                    pos += l;
                }
            }
            else
            {
                int l = getSegment(segment, &search);
                if (flags & F_MATCHCASE)
                {
                    if (strncmp(buf + pos, segment, l) != 0)
                    {
                        search = orgsearch;
                        pos = orgpos;
                        if (!(flags & F_DOWN))
                            break;
                    }
                    else
                    {
                        pos += l;
                    }
                }
                else
                {
                    if (strnicmp(buf + pos, segment, l) != 0)
                    {
                        search = orgsearch;
                        pos = orgpos;
                        if (!(flags & F_DOWN))
                            break;
                    }
                    else
                    {
                        pos += l;
                    }
                }
            }
        }
    }
    if (!*search)
    {
        *len = pos - start;
        return start;
    }
    return -1;
}
static void wc_matches(char* search, int flags, char* buf, CHARRANGE* pos, int* len)
{
    if (flags & FR_DOWN)
    {
        pos->cpMin = matches_wc(search, flags, buf, pos->cpMin, len);
        if (pos->cpMin == -1)
            pos->cpMin = pos->cpMax;
    }
    else
    {
        int l = strlen(search);
        do
        {
            int n = matches_wc(search, flags, buf, pos->cpMax, len);
            if (n >= 0)
            {
                pos->cpMax = n;
                return;
            }
            pos->cpMax--;
        } while (pos->cpMax >= pos->cpMin && !canceled);
    }
}
static void rex_matches(RE_CONTEXT* context, int flags, char* buf, CHARRANGE* pos, int* len)
{
    if (flags & F_DOWN)
    {
        int i = re_matches(context, buf, pos->cpMin, pos->cpMax);
        if (!i)
            pos->cpMin = pos->cpMax;
        else
            pos->cpMin += context->m_so;
    }
    else
    {
        int i = re_matches(context, buf, pos->cpMax, pos->cpMin);
        if (!i)
            pos->cpMax = -1;
        else
            pos->cpMax += context->m_so;
    }
    *len = context->m_eo - context->m_so;
}
static int countLines(char* start, char* end)
{
    int rv = 0;
    char* p = start - 1;
    while (p && p < end)
    {
        p = strchr(p + 1, '\n');
        if (p && p < end)
            rv++;
    }
    return rv;
}
static void SendToOutput(char* fname, int flags, char* start, char* end)
{
    if (flags & (F_OUTPUT1 | F_OUTPUT2))
    {
        char line[1000];
        char buf[1500];
        int n = countLines(start, end) + 1;
        char *ls = end, *le = end;
        while (ls > start && ls[-1] != '\n')
            ls--;
        while (*le && *le != '\r' && *le != '\n')
            le++;
        if (le - ls > sizeof(line))
            le = ls + sizeof(line) - 1;
        memcpy(line, ls, le - ls);
        line[le - ls] = 0;
        sprintf(buf, "%s:%7d %s\n", fname, n, line);
        SendFindMessage(!!(flags & F_OUTPUT2), buf);
        Tag((flags & F_OUTPUT1) ? TAG_FIF1 : TAG_FIF2, fname, n, 0, 0, 0, 0);
    }
    else if (flags & F_OPENWINDOW)
    {
        DWINFO* ptr = GetFileInfo(fname);
        if (!ptr)
        {
            DWINFO* p = calloc(1, sizeof(DWINFO));
            char* q = strrchr(fname, '\\');
            if (q)
                q++;
            else
                q = fname;

            strcpy(p->dwName, fname);
            strcpy(p->dwTitle, q);
            p->dwLineNo = countLines(start, end) + 1;
            p->logMRU = FALSE;
            p->newFile = FALSE;
            PostMessage(hwndFrame, WM_COMMAND, IDM_DEFERREDOPEN, (LPARAM)p);
            while (!GetFileInfo(fname) && !canceled)
                Sleep(10);
        }
    }
}
static int xfind(char* fname, char* search, int flags, char* buf, int* len, CHARRANGE* pos, RE_CONTEXT* context)
{
    *len = strlen(search);
    while (!canceled)
    {
        if (flags & F_REGULAR)
        {
            rex_matches(context, flags, buf, pos, len);
        }
        else if (flags & F_WILDCARD)
        {
            wc_matches(search, flags, buf, pos, len);
        }
        else
        {
            nm_matches(search, flags, buf, pos);
        }
        if (pos->cpMin >= pos->cpMax)
            return -1;

        if (flags & F_WHOLEWORD)
        {
            if (flags & FR_DOWN)
            {

                if (pos->cpMin && isalnum(buf[pos->cpMin - 1]))
                {
                    pos->cpMin += *len;
                }
                else if (!buf[pos->cpMin + *len] || !isalnum(buf[pos->cpMin + *len]))
                {
                    SendToOutput(fname, flags, buf, buf + pos->cpMin);
                    return pos->cpMin;
                }
                else
                {
                    pos->cpMin += *len;
                }
            }
            else
            {
                if (pos->cpMax && isalnum(buf[pos->cpMax - 1]))
                {
                    pos->cpMax--;
                }
                else if (!buf[pos->cpMax + *len] || !isalnum(buf[pos->cpMax + *len]))
                {
                    SendToOutput(fname, flags, buf, buf + pos->cpMax);
                    return pos->cpMax;
                }
                else
                {
                    pos->cpMax--;
                }
            }
        }
        else
        {

            if (flags & FR_DOWN)
            {
                SendToOutput(fname, flags, buf, buf + pos->cpMin);
                return pos->cpMin;
            }
            else
            {
                SendToOutput(fname, flags, buf, buf + pos->cpMax);
                return pos->cpMax;
            }
        }
    }
    return -1;  // canceled
}
static RE_CONTEXT* getREContext(char* search, int flags)
{
    RE_CONTEXT* rv = NULL;
    if (flags & F_REGULAR)
    {
        rv = re_init(search,
                     RE_F_REGULAR | ((flags & F_MATCHCASE) ? 0 : RE_F_INSENSITIVE) | ((flags & F_WHOLEWORD) ? RE_F_WORD : 0), NULL);
        if (!rv)
        {
            ExtendedMessageBox("Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "There is a problem with the regular expression");
            rv = (RE_CONTEXT*)-1;
        }
    }
    return rv;
}
static void FindAllInBuffer(char* fname, char* buf, char* search, int flags)
{
    CHARRANGE pos;
    int len = 0;
    RE_CONTEXT* context = getREContext(search, flags);
    if (context == (RE_CONTEXT*)-1)
        return;
    pos.cpMin = 0;
    pos.cpMax = strlen(buf);
    flags |= F_DOWN;
    while ((pos.cpMin = xfind(fname, search, flags, buf, &len, &pos, context)) >= 0)
    {
        matchCount++;
        pos.cpMin += len;
    }
    if (context)
        re_free(context);
}
//-------------------------------------------------------------------------

static int FindNextInDocument(DWINFO* ptr, char* buf, CHARRANGE* pos, int flags, char* search, RE_CONTEXT* context)
{

    BOOL rv = FALSE;
    BOOL val;
    int len = 0;
    val = xfind(ptr->dwName, search, flags, buf, &len, pos, context);
    if (val >= 0)
    {
        CHARRANGE r;
        HWND win;
        matchCount++;
        if (flags & F_DOWN)
        {
            r.cpMax = pos->cpMin;
            r.cpMin = pos->cpMin + len;
        }
        else
        {
            r.cpMin = pos->cpMax;
            r.cpMax = pos->cpMax + len;
        }
        inSetSel = TRUE;
        SendMessage(ptr->dwHandle, EM_HIDESELECTION, 1, 0);
        SendMessage(ptr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&r);
        SendMessage(ptr->dwHandle, EM_SCROLLCARET, 0, 1);
        SendMessage(ptr->dwHandle, EM_HIDESELECTION, 0, 0);
        inSetSel = FALSE;
        UpdateWindow(ptr->dwHandle);
        SendMessage(ptr->self, WM_COMMAND, ID_REDRAWSTATUS, 0);
        win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
        if (win != ptr->self)
        {
            inSetSel = TRUE;
            SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)ptr->self, 0);
            inSetSel = FALSE;
        }
        rv = TRUE;
    }
    else
    {
        SendMessage(ptr->dwHandle, EM_SETSEL, -1, 0);
        SendMessage(ptr->self, WM_COMMAND, ID_REDRAWSTATUS, 0);
    }
    return rv;
}
static BOOL matchesExt(char* name, char* extensions)
{
    char* p = strrchr(name, '.');
    if (p && p[-1] != '.')
    {
        char buf[MAX_PATH];
        while (*extensions)
        {
            char* q = buf;
            while (*extensions && *extensions != ';')
            {
                *q++ = *extensions++;
            }
            *q = 0;
            if (*extensions)
                extensions++;
            if (!strcmp(buf + 1, ".*"))
                return TRUE;
            if (!xstricmp(buf + 1, p))
                return TRUE;
        }
    }
    else if (!p)
    {
        char buf[MAX_PATH];
        while (*extensions)
        {
            char* q = buf;
            while (*extensions && *extensions != ';')
            {
                *q++ = *extensions++;
            }
            *q = 0;
            if (*extensions)
                extensions++;
            if (!strcmp(buf + 1, ".*"))
                return TRUE;
        }
    }
    return FALSE;
}
static int sortfunc(const void* Left, const void* Right)
{
    const DWINFO** left = (const DWINFO**)Left;
    const DWINFO** right = (const DWINFO**)Right;
    return xstricmp((*left)->dwName, (*right)->dwName);
}
static BUFLIST** AddToTail(DWINFO** files, int count, BUFLIST** tail)
{
    int i;
    qsort(files, count, sizeof(DWINFO*), sortfunc);
    for (i = 0; i < count; i++)
    {
        BUFLIST* newBuf = calloc(1, sizeof(BUFLIST));
        if (newBuf)
        {
            newBuf->back = *tail;
            *tail = newBuf;
            tail = &(*tail)->fwd;
            *tail = newBuf;
            newBuf->range.cpMin = 0;
            newBuf->ptr = files[i];
            newBuf->range.cpMax = files[i]->temp;
        }
    }
    return tail;
}
void AddFileToArray(DWINFO*** files, int* count, int* max, char* name)
{
    DWINFO* ptr = GetFileInfo(name);
    if (!ptr)
    {
        HANDLE h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE)
        {
            DWORD hi;
            ptr = calloc(sizeof(DWINFO), 1);
            strcpy(ptr->dwName, name);
            ptr->uninstantiated = TRUE;
            ptr->temp = GetFileSize(h, &hi);
            CloseHandle(h);
        }
    }
    else
    {
        ptr->temp = SendMessage(ptr->dwHandle, WM_GETTEXTLENGTH, 0, 0);
    }
    if (ptr)
    {
        if (*count >= *max)
        {
            if (*max == 0)
                *max = 100;
            else
                *max *= 2;
            *files = realloc(*files, sizeof(DWINFO*) * (*max));
        }
        if (*files)
            (*files)[(*count)++] = ptr;
    }
}
static BUFLIST** FindLoadProject(PROJECTITEM* pi, BUFLIST** tail, int flags, char* extensions)
{
    static int max, count;
    static DWINFO** files;
    if (!pi)
        return tail;
    pi = pi->children;
    while (pi && !canceled)
    {
        if (pi->type == PJ_FOLDER)
        {

            if (flags & F_SUBDIR)
            {
                tail = FindLoadProject(pi, tail, flags, extensions);
            }
        }
        else if (matchesExt(pi->realName, extensions))
        {
            AddFileToArray(&files, &count, &max, pi->realName);
        }
        pi = pi->next;
    }
    if (tail)
    {
        tail = AddToTail(files, count, tail);
        free(files);
        max = 0, count = 0, files = 0;
    }
    return tail;
}
static void FindLoadPath(char* path, BUFLIST** tail, int flags, char* extensions)
{
    static int max, count;
    static DWINFO** files;
    char xpath[MAX_PATH];
    WIN32_FIND_DATA data;
    HANDLE hndl;
    strcpy(xpath, path);
    strcat(xpath, "\\*.*");
    hndl = FindFirstFile(xpath, &data);
    if (hndl != INVALID_HANDLE_VALUE)
    {
        do
        {
            char xpath[MAX_PATH];
            sprintf(xpath, "%s\\%s", path, data.cFileName);
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (flags & F_SUBDIR)
                {
                    if (data.cFileName[0] != '.' ||
                        (data.cFileName[1] != 0 && (data.cFileName[1] != '.' || data.cFileName[2] != 0)))
                        FindLoadPath(xpath, NULL, flags, extensions);
                }
            }
            else if (matchesExt(data.cFileName, extensions))
            {
                AddFileToArray(&files, &count, &max, xpath);
            }
        } while (FindNextFile(hndl, &data) && !canceled);
        FindClose(hndl);
    }
    if (tail)
    {
        AddToTail(files, count, tail);
        free(files);
        max = 0, count = 0, files = 0;
    }
}
void EndFind(void)
{
    if (currentWindow && !inSetSel)
    {
        SendMessage(currentWindow->dwHandle, EM_HIDESELECTION, 0, 0);
        currentWindow = NULL;
        free(currentBuffer);
        currentBuffer = NULL;
        while (opened)
        {
            BUFLIST* next = opened->fwd;
            if (opened->ptr->uninstantiated)
                free(opened->ptr);
            free(opened);
            opened = next;
        }
    }
}
static char* LoadBuffer(DWINFO* ptr)
{
    char* rv;
    if (IsWindow(ptr->self))
    {
        int n = SendMessage(ptr->dwHandle, WM_GETTEXTLENGTH, 0, 0);
        if (n)
        {
            rv = calloc(1, n + 1);
            if (rv)
                SendMessage(ptr->dwHandle, WM_GETTEXT, n + 1, (LPARAM)rv);
        }
    }
    else
    {
        FILE* fil = fopen(ptr->dwName, "r");
        if (fil)
        {
            int n;
            fseek(fil, 0, SEEK_END);
            n = ftell(fil);
            fseek(fil, 0, SEEK_SET);
            rv = calloc(1, n + 1);
            if (rv)
            {
                fread(rv, 1, n + 1, fil);
            }
            fclose(fil);
        }
    }
    return rv;
}
static void RestartFind()
{
    if (currentWindow && currentBuffer)
    {
        char* p = LoadBuffer(currentWindow);
        if (p)
        {
            free(currentBuffer);
            currentBuffer = p;
        }
    }
}
static void GetSelectionPos(HWND hwnd, CHARRANGE* cr)
{
    SendMessage(hwnd, EM_EXGETSEL, 0, (LPARAM)cr);
    if (cr->cpMax < cr->cpMin)
    {
        int n = cr->cpMax;
        cr->cpMax = cr->cpMin;
        cr->cpMin = n;
    }
}
void FindResetWindows()
{
    CHARRANGE* findPointer = &findPos;
    int flags = findflags;
    int mode = findmode;
    if (!finding)
    {
        findPointer = &replacePos;
        flags = replaceflags;
        mode = replacemode;
    }
    // select the currently active window as the first thing to search in
    if (currentWindow)
    {
        HWND hwnd;
        CHARRANGE a;
        GetSelectionPos(currentWindow->dwHandle, &a);
        hwnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
        if (IsEditWindow(hwnd))
        {
            DWINFO* info = (DWINFO*)GetWindowLong(hwnd, 0);
            if (info != currentWindow)
            {
                if (mode != F_M_OPENDOCUMENTS)
                {
                    EndFind();
                }
                else
                {
                    BUFLIST* p = opened;
                    BOOL found = FALSE;
                    while (p)
                    {
                        if (p->ptr == info)
                        {
                            if (p->fwd)
                                p->fwd->back = p->back;
                            if (p->back)
                                p->back->fwd = p->fwd;
                            if (p == opened)
                            {
                                opened = p->fwd;
                                if (opened)
                                    opened->back = NULL;
                            }
                            p->fwd = opened;
                            p->back = NULL;
                            if (opened)
                                opened->back = p;
                            opened = p;
                            p->range = *findPointer;
                            p->ptr = currentWindow;
                            GetSelectionPos(info->dwHandle, &a);
                            free(currentBuffer);
                            currentBuffer = LoadBuffer(info);
                            currentWindow = info;
                            found = TRUE;
                            break;
                        }
                        p = p->fwd;
                    }
                    if (!found)
                    {
                        p = calloc(1, sizeof(BUFLIST));
                        p->fwd = opened;
                        p->back = NULL;
                        if (opened)
                            opened->back = p;
                        opened = p;
                        p->ptr = currentWindow;
                        p->range = *findPointer;
                        GetSelectionPos(info->dwHandle, &a);

                        free(currentBuffer);
                        currentBuffer = LoadBuffer(info);
                        currentWindow = info;
                    }
                    findPointer->cpMin = 0;
                    findPointer->cpMax = SendMessage(info->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                    dirty = FALSE;
                }
            }
            else if (!dirty)
            {
                if (finding)
                {
                    if (findflags & F_DOWN)
                    {
                        //                        a.cpMin += strlen(findText);
                    }
                    else
                    {
                        a.cpMax -= strlen(findText) + 1;
                    }
                }
                else
                {
                    if (replaceflags & F_DOWN)
                    {
                        //                        if (!replacing)
                        //                            a.cpMin += strlen(replaceText);
                    }
                    else
                    {
                        if (!replacing)
                            a.cpMax -= strlen(findText) + 1;
                    }
                }
            }
        }
        if (flags & F_DOWN)
        {
            findPointer->cpMin = a.cpMax;
        }
        else
        {
            findPointer->cpMax = a.cpMax == -1 ? SendMessage(currentWindow->dwHandle, WM_GETTEXTLENGTH, 0, 0) : a.cpMax;
        }
    }
    if (dirty)  // else if dirty restart the find
    {
        dirty = FALSE;
        RestartFind();
    }
    replacing = FALSE;
}
void FindSetDirty() { dirty = TRUE; }
static char* GetNextFindDocument(CHARRANGE* pos, int mode, DWINFO** ptr, char* specifiedPath, char* specifiedExtensions, int flags)
{
    if (!editWindows)
        return NULL;
    if (!*ptr)
    {
        BUFLIST** tail = &opened;
        DWINFO* eq = editWindows;
        switch (mode)
        {
            BUFLIST* newBuf;
            PROJECTITEM* pi;
            char path[MAX_PATH];
            case F_M_CURRENTDOCUMENT:
            case F_M_SELECTION:
                newBuf = calloc(1, sizeof(BUFLIST));
                if (newBuf)
                {
                    newBuf->back = *tail;
                    *tail = newBuf;
                    tail = &(*tail)->fwd;
                    *tail = newBuf;
                    newBuf->ptr = editWindows;
                    GetSelectionPos(editWindows->dwHandle, &newBuf->range);
                    if (newBuf->range.cpMin == newBuf->range.cpMax)
                    {
                        if (flags & (F_OUTPUT1 | F_OUTPUT2))
                        {
                            newBuf->range.cpMin = 0;
                            newBuf->range.cpMax = SendMessage(editWindows->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                        }
                        else if (flags & F_DOWN)
                        {
                            newBuf->range.cpMax = SendMessage(editWindows->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                            if (newBuf->range.cpMin != 0)
                            {
                                BUFLIST* newBuf2 = calloc(1, sizeof(BUFLIST));
                                if (newBuf2)
                                {
                                    newBuf2->back = *tail;
                                    *tail = newBuf2;
                                    tail = &(*tail)->fwd;
                                    *tail = newBuf2;
                                    newBuf2->ptr = editWindows;
                                    newBuf2->range.cpMin = 0;
                                    newBuf2->range.cpMax = newBuf->range.cpMin;
                                }
                            }
                        }
                        else
                        {
                            int n = SendMessage(editWindows->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                            if (newBuf->range.cpMax != n)
                            {
                                BUFLIST* newBuf2 = calloc(1, sizeof(BUFLIST));
                                if (newBuf2)
                                {
                                    newBuf2->back = *tail;
                                    *tail = newBuf2;
                                    tail = &(*tail)->fwd;
                                    *tail = newBuf2;
                                    newBuf2->ptr = editWindows;
                                    newBuf2->range.cpMin = newBuf->range.cpMax;
                                    newBuf2->range.cpMax = n;
                                }
                            }
                            newBuf->range.cpMin = 0;
                            newBuf->range.cpMax--;
                        }
                    }
                }
                break;
            case F_M_OPENDOCUMENTS:
                if (matchesExt(editWindows->dwName, specifiedExtensions))
                {
                    newBuf = calloc(1, sizeof(BUFLIST));
                    if (newBuf)
                    {
                        newBuf->back = *tail;
                        *tail = newBuf;
                        tail = &(*tail)->fwd;
                        *tail = newBuf;
                        GetSelectionPos(editWindows->dwHandle, &newBuf->range);
                        *pos = newBuf->range;
                        if (newBuf->range.cpMin == newBuf->range.cpMax)
                        {
                            if (flags & (F_OUTPUT1 | F_OUTPUT2))
                            {
                                newBuf->range.cpMin = 0;
                                newBuf->range.cpMax = SendMessage(editWindows->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                            }
                            else if (flags & F_DOWN)
                            {
                                newBuf->range.cpMax = SendMessage(editWindows->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                            }
                            else
                            {
                                newBuf->range.cpMin = 0;
                                newBuf->range.cpMax--;
                            }
                        }
                        newBuf->ptr = eq;
                        eq = eq->next;
                    }
                }
                while (eq && !canceled)
                {
                    if (matchesExt(eq->dwName, specifiedExtensions))
                    {
                        newBuf = calloc(1, sizeof(BUFLIST));
                        if (newBuf)
                        {
                            newBuf->back = *tail;
                            *tail = newBuf;
                            tail = &(*tail)->fwd;
                            *tail = newBuf;
                            newBuf->range.cpMax = SendMessage(eq->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                            newBuf->ptr = eq;
                        }
                    }
                    eq = eq->next;
                }
                if (!(flags & (F_OUTPUT1 | F_OUTPUT2)))
                {
                    if (matchesExt(editWindows->dwName, specifiedExtensions))
                    {
                        newBuf = calloc(1, sizeof(BUFLIST));
                        if (newBuf)
                        {
                            newBuf->back = *tail;
                            *tail = newBuf;
                            tail = &(*tail)->fwd;
                            *tail = newBuf;
                            newBuf->ptr = opened->ptr;
                            if (flags & F_DOWN)
                            {
                                newBuf->range.cpMin = 0;
                                newBuf->range.cpMax = pos->cpMin;
                            }
                            else
                            {
                                newBuf->range.cpMin = pos->cpMin;
                                newBuf->range.cpMax = SendMessage(editWindows->dwHandle, WM_GETTEXTLENGTH, 0, 0);
                            }
                        }
                    }
                }
                break;
            case F_M_CURRENTPROJECT:
                tail = FindLoadProject(activeProject, tail, flags, specifiedExtensions);
                break;
            case F_M_ALLPROJECTS:
                pi = workArea->children;
                while (pi)
                {
                    tail = FindLoadProject(pi, tail, flags, specifiedExtensions);
                    pi = pi->next;
                }
                break;
            case F_M_INCLUDEDIR:
                sprintf(path, "%s\\include", szInstallPath);
                FindLoadPath(path, tail, flags, headerExt);
                break;
            case F_M_SPECIFIEDPATH:
                FindLoadPath(specifiedPath, tail, flags, specifiedExtensions);
                break;
        }
        *tail = NULL;
    }
    if (opened)
    {
        BUFLIST* next = opened->fwd;
        char* rv = NULL;

        *pos = opened->range;
        *ptr = opened->ptr;
        rv = LoadBuffer(opened->ptr);
        if (currentWindow && currentWindow->uninstantiated)
        {
            free(currentWindow);
        }
        currentBuffer = rv;
        currentWindow = opened->ptr;
        free(opened);
        opened = next;
        if (opened)
        {
            opened->back = NULL;
        }
        return currentBuffer;
    }
    else
    {
        EndFind();
        return NULL;
    }
}
void FindStringFromToolbar(char* search)
{
    static char last[256];
    RE_CONTEXT* context = getREContext(search, F_WILDCARD | F_DOWN);
    char* buf = currentBuffer;
    DWINFO* ptr = currentWindow;
    if (!search)
        search = last;
    if (!*search)
        return;
    if (!findFromTB || strcmp(search, last) != 0)
    {
        EndFind();
        if (ptr)
            SendMessage(ptr->dwHandle, EM_SETSEL, -1, 0);
        ptr = NULL;
        strncpy(last, search, sizeof(last) - 1);
    }
    finding = TRUE;
    findFromTB = TRUE;

    if (!ptr || !buf || !FindNextInDocument(ptr, buf, &findPos, F_WILDCARD | F_DOWN, search, context))
        do
        {
            buf = GetNextFindDocument(&findPos, F_M_CURRENTDOCUMENT, &ptr, "", "*.*", F_WILDCARD | F_DOWN);
        } while (buf && !FindNextInDocument(ptr, buf, &findPos, F_WILDCARD | F_DOWN, search, context) && !canceled);
    if (context)
        re_free(context);
}
static void FindStringFromFiles(int mode, int flags, char* search, char* specifiedPath, char* specifiedExtension)
{
    RE_CONTEXT* context = getREContext(search, flags);
    DWINFO* ptr = currentWindow;
    char* buf;
    findFromTB = FALSE;
    if (flags & (F_OUTPUT1 | F_OUTPUT2))
    {
        char msgbuf[256];
        EndFind();
        TagRemoveAll((flags & F_OUTPUT1) ? TAG_FIF1 : TAG_FIF2);
        matchCount = 0;
        ptr = NULL;
        do
        {
            buf = GetNextFindDocument(&findPos, mode, &ptr, specifiedPath, specifiedExtension, flags);
            if (buf)
                FindAllInBuffer(ptr->dwName, buf, search, flags);
        } while (buf && !canceled);
        sprintf(msgbuf, "\nFinished.  %d Matches\n", matchCount);
        SendFindMessage(!!(flags & F_OUTPUT2), msgbuf);
    }
    else
    {
        static char last[256];
        if (strcmp(search, last) != 0)
        {
            EndFind();
            if (ptr)
                SendMessage(ptr->dwHandle, EM_SETSEL, -1, 0);
            ptr = NULL;
            strcpy(last, search);
        }
        buf = currentBuffer;
        if (!ptr || !buf || !FindNextInDocument(ptr, buf, finding ? &findPos : &replacePos, flags, search, context))
        {
            do
            {
                buf = GetNextFindDocument(finding ? &findPos : &replacePos, mode, &ptr, specifiedPath, specifiedExtension, flags);
            } while (buf && !FindNextInDocument(ptr, buf, finding ? &findPos : &replacePos, flags, search, context) && !canceled);
        }
        if (!buf && !(flags & FR_REPLACEALL))
        {
            SetStatusMessage("Search reached end of region.", FALSE);
        }
    }
    if (context)
        re_free(context);
}
//-------------------------------------------------------------------------

static void GetReplaceText(char* dest, char* replace, char* dta, int len, int flags, RE_CONTEXT* context)
{
    if (replaceflags & F_REGULAR)
    {
        while (*replace)
        {
            if (*replace == '\\')
            {
                if (++*replace)
                {
                    int index = *replace;
                    if (isdigit(index))
                    {
                        index -= '0';
                        if (dta && index <= 10)
                        {
                            if (context->matchOffsets[index][1] > 0)
                            {
                                int j, k;
                                for (k = 0, j = context->matchOffsets[index][0]; j < context->matchOffsets[index][1] && k < 255;
                                     j++, k++)
                                    dest[k] = dta[j];
                            }
                        }
                    }
                    else
                    {
                        *dest++ = *replace++;
                    }
                }
            }
            else
            {
                *dest++ = *replace++;
            }
        }
        *dest = 0;
    }
    else
    {
        strcpy(dest, replace);
    }
}
static int ReplaceNextInDocument(DWINFO* ptr, char* buf, int* ofs, CHARRANGE* pos, CHARRANGE* selection, int flags, char* search,
                                 char* replace, RE_CONTEXT* context)
{
    BOOL val;
    int len = 0;
    val = xfind(ptr->dwName, search, flags, buf, &len, pos, context);
    if (val >= 0)
    {
        char repl[1000];
        int curpos;
        int l;
        CHARRANGE aa;
        matchCount++;
        if (!ptr->self)
        {
            DWINFO* ptrx = GetFileInfo(ptr->dwName);
            if (ptrx)
            {
                ptr->self = ptrx->self;
                ptr->dwHandle = ptrx->dwHandle;
            }
        }
        if (flags & F_DOWN)
            curpos = pos->cpMin;
        else
            curpos = pos->cpMax;
        GetReplaceText(repl, replace, buf, curpos, len, context);
        aa.cpMin = curpos + *ofs;
        aa.cpMax = curpos + *ofs + len;
        if (flags & F_DOWN)
        {
            //            if (flags & F_REPLACEALL)
            {
                pos->cpMin += len;
            }
        }
        l = strlen(repl) - strlen(search);
        if (curpos + *ofs < selection->cpMin)
        {
            if (curpos + *ofs + l > selection->cpMin)
                selection->cpMin += selection->cpMin - curpos - *ofs;
            else
                selection->cpMin += l;
        }
        if (curpos + *ofs < selection->cpMax)
        {
            if (curpos + *ofs + l > selection->cpMax)
                selection->cpMax += selection->cpMax - curpos - *ofs;
            else
                selection->cpMax += l;
        }
        *ofs += l;
        inSetSel = TRUE;
        SendMessage(ptr->dwHandle, EM_HIDESELECTION, 1, 0);
        SendMessage(ptr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&aa);
        SendMessage(ptr->dwHandle, EM_REPLACESEL, 1, (LPARAM)repl);
        SendMessage(ptr->dwHandle, EM_HIDESELECTION, 0, 0);
        SendMessage(ptr->self, WM_COMMAND, ID_REDRAWSTATUS, 0);
        inSetSel = FALSE;
        replaceCount++;
        return TRUE;
    }
    return FALSE;
}
static void ReplaceInDocuments(int mode, int flags, char* search, char* replace, char* specifiedPath, char* specifiedExtension)
{
    RE_CONTEXT* context = getREContext(search, flags);
    DWINFO *ptr = currentWindow, *lastPtr = 0;
    char* buf = currentBuffer;
    BOOL replaced = FALSE;
    BOOL needsUpdate = FALSE;
    int offset = 0, lastoffset = 0;
    int vis;
    int linepos;
    CHARRANGE lastReplacePos = {0, 0};
    CHARRANGE selection = {0, 0};

    // to guard against the EN_LINECHANGE message which does an EndFind();
    do
    {
        if (buf)
        {
            if (ptr && ptr != lastPtr && (flags & F_REPLACEALL))
            {
                if (selection.cpMin != selection.cpMax)
                {
                    if (lastPtr)
                    {
                        SendMessage(lastPtr->dwHandle, EM_SETFIRSTVISIBLELINE, vis, 0);
                        SendMessage(lastPtr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&selection);
                    }
                    lastPtr = ptr;
                    vis = SendMessage(ptr->dwHandle, EM_GETFIRSTVISIBLELINE, 0, 0);
                    SendMessage(ptr->dwHandle, EM_EXGETSEL, 0, (WPARAM)&selection);
                }
                else
                {
                    if (needsUpdate)
                    {
                        offset = lastoffset;
                        if (lastReplacePos.cpMin >= SendMessage(lastPtr->dwHandle, EM_GETSIZE, 0, 0) - offset)
                            lastReplacePos.cpMin = replacePos.cpMax = vis = offset = 0;
                        lastReplacePos.cpMin += offset;
                        lastReplacePos.cpMax += offset;
                        SendMessage(lastPtr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&lastReplacePos);
                        linepos = SendMessage(lastPtr->dwHandle, EM_LINEFROMCHAR, lastReplacePos.cpMin, 0);
                        vis = linepos - vis;
                        SendMessage(lastPtr->dwHandle, EM_SETFIRSTVISIBLELINE, vis, 0);
                    }
                    vis = SendMessage(ptr->dwHandle, EM_GETFIRSTVISIBLELINE, 0, 0);
                    lastPtr = ptr;
                    GetSelectionPos(ptr->dwHandle, &lastReplacePos);
                    linepos = SendMessage(ptr->dwHandle, EM_LINEFROMCHAR, lastReplacePos.cpMin, 0);
                    vis = linepos - vis;
                    needsUpdate = TRUE;
                }
            }
            offset = 0;
            lastoffset = 0;
            if ((flags & F_REPLACEALL) && ptr)
                insertautoundo(ptr->dwHandle, (EDITDATA*)GetWindowLong(ptr->dwHandle, 0), UNDO_AUTOCHAINEND);
            do
            {
                BOOL doupdate = FALSE;
                doupdate = replacePos.cpMin < lastReplacePos.cpMin;
                replaced = ReplaceNextInDocument(ptr, buf, &offset, &replacePos, &selection, flags, search, replace, context);
                if (doupdate)
                    lastoffset = offset;
            } while ((flags & F_REPLACEALL) && replaced && !canceled);
            if ((flags & F_REPLACEALL) && ptr)
                insertautoundo(ptr->dwHandle, (EDITDATA*)GetWindowLong(ptr->dwHandle, 0), UNDO_AUTOCHAINBEGIN);
            if (replaced && !(flags & F_REPLACEALL))
            {
                RestartFind();
                buf = currentBuffer;
                if (!FindNextInDocument(ptr, buf, &replacePos, flags, search, context))
                {
                    do
                    {
                        buf = GetNextFindDocument(&replacePos, mode, &ptr, specifiedPath, specifiedExtension, flags);
                    } while (buf && !FindNextInDocument(ptr, buf, &replacePos, flags, search, context) && !canceled);
                }
            }
        }
    } while ((!replaced || (flags & F_REPLACEALL)) && !canceled &&
             (buf = GetNextFindDocument(&replacePos, mode, &ptr, specifiedPath, specifiedExtension, flags)));
    if (selection.cpMin != selection.cpMax)
    {
        if (lastPtr)
        {
            SendMessage(lastPtr->dwHandle, EM_SETFIRSTVISIBLELINE, vis, 0);
            SendMessage(lastPtr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&selection);
        }
    }
    else
    {
        if (needsUpdate)
        {
            offset = lastoffset;
            if (replacePos.cpMin >= SendMessage(lastPtr->dwHandle, EM_GETSIZE, 0, 0) - offset)
                replacePos.cpMin = replacePos.cpMax = vis = offset = 0;
            replacePos.cpMin += offset;
            replacePos.cpMax += offset;
            SendMessage(lastPtr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&replacePos);
            linepos = SendMessage(lastPtr->dwHandle, EM_LINEFROMCHAR, replacePos.cpMin, 0);
            vis = linepos - vis;
            SendMessage(lastPtr->dwHandle, EM_SETFIRSTVISIBLELINE, vis, 0);
        }
    }
    if (context)
        re_free(context);
}
static void EnableWindows(HWND hwnd, BOOL state)
{
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOFINDFIND), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOFINDTYPE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOFINDWHERE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOFINDPATH), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOFINDHOW), state);
    EnableWindow(GetDlgItem(hwnd, IDC_FINDNEXT), state);
    EnableWindow(GetDlgItem(hwnd, IDC_FINDBROWSE), state);

    EnableWindow(GetDlgItem(hwnd, IDC_CHECKFINDWHOLE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_CHECKFINDCASE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_CHECKFINDRECURSIVE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_CHECKFINDUP), state);
    EnableWindow(GetDlgItem(hwnd, IDC_RADIOFINDDOCUMENT), state);
    EnableWindow(GetDlgItem(hwnd, IDC_RADIOFIND1), state);
    EnableWindow(GetDlgItem(hwnd, IDC_RADIOFIND2), state);

    EnableWindow(GetDlgItem(hwnd, IDC_COMBOREPLACEFIND), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOREPLACE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOREPLACETYPE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOREPLACEWHERE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOREPLACEPATH), state);
    EnableWindow(GetDlgItem(hwnd, IDC_COMBOREPLACEHOW), state);
    EnableWindow(GetDlgItem(hwnd, IDC_REPLACEFINDNEXT), state);
    EnableWindow(GetDlgItem(hwnd, IDC_REPLACENEXT), state);
    EnableWindow(GetDlgItem(hwnd, IDC_REPLACEALL), state);
    EnableWindow(GetDlgItem(hwnd, IDC_REPLACEBROWSE), state);

    EnableWindow(GetDlgItem(hwnd, IDC_CHECKREPLACEWHOLE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_CHECKREPLACECASE), state);
    EnableWindow(GetDlgItem(hwnd, IDC_CHECKREPLACERECURSIVE), state);
}
void DoFindNext(void* p)
{
    if (findflags & (F_OUTPUT1 | F_OUTPUT2))
    {
        SendFindMessage(!!(findflags & F_OUTPUT2), NULL);
    }
    if (!infindorreplace)
    {
        HWND hwndDlg = p;
        FindResetWindows();
        infindorreplace = TRUE;
        if (findFromTB)
        {
            FindStringFromToolbar(NULL);
        }
        else
        {
            PostMessage(hwndDlg, WM_ENABLEFIND, FALSE, 0);
            canceled = FALSE;
            if (findText[0])
            {
                FindStringFromFiles(findmode, findflags, findText, browsePath, fileType);
            }
            if (canceled)
                EndFind();
            PostMessage(hwndDlg, WM_ENABLEFIND, TRUE, 0);
        }
        rehighlight(findText, findflags & FR_MATCHCASE, findflags & F_WHOLEWORD);
        infindorreplace = FALSE;
    }
}
void DoReplaceNext(void* p)
{
    if (!infindorreplace)
    {
        HWND hwndDlg = p;
        PostMessage(hwndDlg, WM_ENABLEFIND, FALSE, 0);
        canceled = FALSE;
        replaceCount = 0;
        replacing = TRUE;
        infindorreplace = TRUE;
        if (findText[0])
            ReplaceInDocuments(replacemode, replaceflags, findText, replaceText, browsePath, fileType);
        if (canceled)
            EndFind();
        infindorreplace = FALSE;
        replacing = FALSE;
        dirty = TRUE;
        if (!canceled)
        {
            if (replaceflags & F_REPLACEALL)
            {
                char buf[256];
                sprintf(buf, "Replaced %d occurrences.", replaceCount);
                SetStatusMessage(buf, FALSE);
            }
        }
        PostMessage(hwndDlg, WM_ENABLEFIND, TRUE, 0);
    }
}
void SetFlags(HWND hwndDlg, DLGHDR* pHdr)
{
    if (!pHdr->iSel)
    {
        int flags = findflags;
        if (flags & F_REGULAR)
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_SETCURSEL, 1, 0);
        }
        else if (flags & F_WILDCARD)
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_SETCURSEL, 2, 0);
        }
        else
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_SETCURSEL, 0, 0);
        }
        CheckDlgButton(hwndDlg, IDC_CHECKFINDUP, (flags & F_DOWN) ? BST_UNCHECKED : BST_CHECKED);
        CheckDlgButton(hwndDlg, IDC_CHECKFINDWHOLE, (flags & F_WHOLEWORD) ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwndDlg, IDC_CHECKFINDCASE, (flags & F_MATCHCASE) ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwndDlg, IDC_CHECKFINDRECURSIVE, (flags & F_SUBDIR) ? BST_CHECKED : BST_UNCHECKED);
        if (flags & F_OUTPUT1)
        {
            CheckRadioButton(hwndDlg, IDC_RADIOFINDDOCUMENT, IDC_RADIOFIND2, IDC_RADIOFIND1);
        }
        else if (flags & F_OUTPUT2)
        {
            CheckRadioButton(hwndDlg, IDC_RADIOFINDDOCUMENT, IDC_RADIOFIND2, IDC_RADIOFIND2);
        }
        else if (flags & F_OPENWINDOW)
        {
            CheckRadioButton(hwndDlg, IDC_RADIOFINDDOCUMENT, IDC_RADIOFIND2, IDC_RADIOFINDDOCUMENT);
        }
    }
    else
    {
        int flags = replaceflags;
        if (flags & F_REGULAR)
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_SETCURSEL, 1, 0);
        }
        else if (flags & F_WILDCARD)
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_SETCURSEL, 2, 0);
        }
        else
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_SETCURSEL, 0, 0);
        }
        CheckDlgButton(hwndDlg, IDC_CHECKREPLACEWHOLE, (flags & F_WHOLEWORD) ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwndDlg, IDC_CHECKREPLACECASE, (flags & F_MATCHCASE) ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwndDlg, IDC_CHECKREPLACERECURSIVE, (flags & F_SUBDIR) ? BST_CHECKED : BST_UNCHECKED);
    }
}
void LoadBrowsePath(HWND hwndDlg, int id)
{
    if (browsePath[0])
    {
        SendDlgItemMessage(hwndDlg, id, WM_SETTEXT, 0, (LPARAM)browsePath);
        SendDlgItemMessage(hwndDlg, id, WM_SAVEHISTORY, 0, 0);
    }
}
void GetBrowsePath(HWND hwndDlg, int id)
{
    int n = SendDlgItemMessage(hwndDlg, id, CB_GETCURSEL, 0, 0);
    if (n == CB_ERR)
        browsePath[0] = 0;
    else
        SendDlgItemMessage(hwndDlg, id, CB_GETLBTEXT, n, (LPARAM)browsePath);
}
void xGetFileType(HWND hwndDlg, int id)
{
    int n = SendDlgItemMessage(hwndDlg, id, CB_GETCURSEL, 0, 0);
    if (n == CB_ERR)
        fileType[0] = 0;
    else
        SendDlgItemMessage(hwndDlg, id, CB_GETLBTEXT, n, (LPARAM)fileType);
}
DLGTEMPLATE* WINAPI DoLockDlgRes(LPCSTR lpszResName)

{
    HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG);
    HGLOBAL hglb = LoadResource(hInstance, hrsrc);
    return (DLGTEMPLATE*)LockResource(hglb);
}

static void SetFindNextButton(HWND hwndDlg)
{
    if (findflags & (F_OUTPUT1 | F_OUTPUT2))
    {
        SetDlgItemText(hwndDlg, IDC_FINDNEXT, "Find All");
        SetDlgItemText(hwndDlg, IDC_REPLACEFINDNEXT, "Find All");
    }
    else
    {
        SetDlgItemText(hwndDlg, IDC_FINDNEXT, "Find Next");
        SetDlgItemText(hwndDlg, IDC_REPLACEFINDNEXT, "Find Next");
    }
}
VOID WINAPI OnSelChanged(HWND hwndDlg);
LRESULT CALLBACK FindChildDlgProc(HWND hwndDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndParent;
    DLGHDR* pHdr;
    switch (iMessage)
    {
        case WM_INITDIALOG:
            if (modifiedFind)
            {
                int n = GetWindowText(hwndTbFind, findText, 256);
                if (n < 0)
                    n = 0;
                findText[n] = 0;
            }
            else
            {
                HWND hwnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (IsWindow(hwnd))
                    SendMessage(hwnd, WM_WORDUNDERCURSOR, 0, (LPARAM)findText);
                findText[sizeof(findText) - 1] = 0;
            }
            hwndFindInternal = hwndDlg;
            hwndParent = GetParent(hwndDlg);
            pHdr = (DLGHDR*)GetWindowLong(hwndParent, GWL_USERDATA);
            SetWindowLong(hwndDlg, GWL_USERDATA, (long)pHdr);
            SetWindowPos(hwndDlg, HWND_TOP, pHdr->rcDisplay.left, pHdr->rcDisplay.top, 0, 0, SWP_NOSIZE);
            SetFindNextButton(hwndDlg);
            if (!pHdr->iSel)
            {
                HWND child = GetDlgItem(hwndDlg, IDC_COMBOFINDFIND);
                SubClassHistoryCombo(child);
                SendMessage(child, WM_SETHISTORY, 0, (LPARAM)findhist);
                if (findText[0])
                    SendMessage(child, WM_SETTEXT, 0, (LPARAM)findText);
                child = GetDlgItem(hwndDlg, IDC_COMBOFINDPATH);
                if (IsWindow(child))
                {
                    SubClassHistoryCombo(child);
                    SendMessage(child, WM_SETHISTORY, 0, (LPARAM)findbrowsehist);
                }
                SendDlgItemMessage(hwndDlg, IDC_ICONFINDOPTIONS, STM_SETIMAGE, IMAGE_BITMAP,
                                   (LPARAM)((pHdr->findOffs & 2) ? pHdr->minusBmp : pHdr->plusBmp));
                SendDlgItemMessage(hwndDlg, IDC_ICONFINDOUTPUT, STM_SETIMAGE, IMAGE_BITMAP,
                                   (LPARAM)((pHdr->findOffs & 4) ? pHdr->minusBmp : pHdr->plusBmp));
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM) "*.c;*.cpp;*.cxx;*.cc;*.h");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM) "*.asm;*.nas;*.s");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM) "*.def;*.rc");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM) "*.txt");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM) "*.*");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_SETCURSEL, findext, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM) "Current Document");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM) "Open Documents");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM) "Current Project");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM) "All Projects");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM) "Orange C Include Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM) "User-specified Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_SETCURSEL, findmode, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_ADDSTRING, 0, (LPARAM) "Direct Match");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_ADDSTRING, 0, (LPARAM) "Regular Expression");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_ADDSTRING, 0, (LPARAM) "Wildcard");
                finding = TRUE;
            }
            else
            {
                HWND child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEFIND);
                SubClassHistoryCombo(child);
                SendMessage(child, WM_SETHISTORY, 0, (LPARAM)findhist);
                if (findText[0])
                    SendMessage(child, WM_SETTEXT, 0, (LPARAM)findText);
                child = GetDlgItem(hwndDlg, IDC_COMBOREPLACE);
                SubClassHistoryCombo(child);
                SendMessage(child, WM_SETHISTORY, 0, (LPARAM)replacehist);
                if (replaceText[0])
                    SendMessage(child, WM_SETTEXT, 0, (LPARAM)replaceText);
                child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEPATH);
                if (IsWindow(child))
                {
                    SubClassHistoryCombo(child);
                    SendMessage(child, WM_SETHISTORY, 0, (LPARAM)replacebrowsehist);
                }
                child = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (IsWindow(child))
                {
                    CHARRANGE range;
                    memset(&range, 0, sizeof(range));
                    SendMessage(child, EM_EXGETSEL, 0, (LPARAM)&range);
                    if (range.cpMax != range.cpMin)
                    {
                        replacemode = F_M_SELECTION;
                    }
                    else if (replacemode == F_M_SELECTION)
                    {
                        replacemode = F_M_CURRENTDOCUMENT;
                    }
                }
                SendDlgItemMessage(hwndDlg, IDC_ICONREPLACEOPTIONS, STM_SETIMAGE, IMAGE_BITMAP,
                                   (LPARAM)((pHdr->replaceOffs & 2) ? pHdr->minusBmp : pHdr->plusBmp));
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_ADDSTRING, 0, (LPARAM) "*.c;*.cpp;*.cxx;*.cc;*.h");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_ADDSTRING, 0, (LPARAM) "*.def;*.rc");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_ADDSTRING, 0, (LPARAM) "*.*");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_SETCURSEL, replaceext, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM) "Current Document");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM) "Open Documents");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM) "Current Project");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM) "All Projects");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM) "Orange C Include Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM) "User-specified Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM) "Selection");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_SETCURSEL, replacemode, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_ADDSTRING, 0, (LPARAM) "Direct Match");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_ADDSTRING, 0, (LPARAM) "Regular Expression");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_ADDSTRING, 0, (LPARAM) "Wildcard");
                finding = FALSE;
            }
            EnableWindows(hwndDlg, pHdr->findEnabled);
            SetFlags(hwndDlg, pHdr);
            modifiedFind = FALSE;
            return 1;
        case WM_ENABLEFIND:
            pHdr = (DLGHDR*)GetWindowLong(hwndDlg, GWL_USERDATA);
            pHdr->findEnabled = wParam;
            EnableWindows(hwndDlg, wParam);
            SetFocus((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0));
            break;
        case WM_USER:
            pHdr = (DLGHDR*)GetWindowLong(hwndParent, GWL_USERDATA);
            if (!pHdr->iSel)
            {
                HWND child = GetDlgItem(hwndDlg, IDC_COMBOFINDFIND);
                SendMessage(child, WM_SAVEHISTORY, 0, 0);
                SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
            }
            else
            {
                HWND child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEFIND);
                SendMessage(child, WM_SAVEHISTORY, 0, 0);
                SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
                child = GetDlgItem(hwndDlg, IDC_COMBOREPLACE);
                SendMessage(child, WM_SAVEHISTORY, 0, 0);
                SendMessage(child, WM_GETTEXT, 256, (LPARAM)replaceText);
            }
            break;
        case WM_KEYDOWN:
            MessageBeep(0);
            break;
        case WM_COMMAND:
            pHdr = (DLGHDR*)GetWindowLong(hwndDlg, GWL_USERDATA);
            // this is needed to handle the default behavior
            if (LOWORD(wParam) == IDOK)
            {
                dirty = TRUE;
                if (pHdr->iSel)
                    wParam = IDC_REPLACENEXT;
                else
                    wParam = IDC_FINDNEXT;
            }
            switch (LOWORD(wParam))
            {
                int n;
                case IDHELP:
                    ContextHelp(IDH_FIND_REPLACE_DIALOG);
                    break;
                case IDC_FINDBROWSE:
                    GetBrowsePath(hwndDlg, IDC_COMBOFINDPATH);
                    BrowseForFile(hwndFrame, "Find", browsePath, MAX_PATH);
                    LoadBrowsePath(hwndDlg, IDC_COMBOFINDPATH);
                    EndFind();
                    break;
                case IDC_REPLACEBROWSE:
                    GetBrowsePath(hwndDlg, IDC_COMBOREPLACEPATH);
                    BrowseForFile(hwndFrame, "Replace", browsePath, MAX_PATH);
                    LoadBrowsePath(hwndDlg, IDC_COMBOREPLACEPATH);
                    EndFind();
                    break;
                case IDC_ICONFINDOPTIONS:
                    pHdr->findOffs ^= 2;
                    OnSelChanged(GetParent(hwndDlg));
                    break;
                case IDC_ICONFINDOUTPUT:
                    pHdr->findOffs ^= 4;
                    OnSelChanged(GetParent(hwndDlg));
                    break;
                case IDC_ICONREPLACEOPTIONS:
                    pHdr->replaceOffs ^= 2;
                    OnSelChanged(GetParent(hwndDlg));
                    break;
                case IDC_COMBOFINDTYPE:
                    findext = SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_GETCURSEL, 0, 0);
                    break;
                case IDC_COMBOREPLACETYPE:
                    replaceext = SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_GETCURSEL, 0, 0);
                    break;
                case IDC_COMBOFINDWHERE:
                    if ((findmode = SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_GETCURSEL, 0, 0)) == 5)
                    {
                        if (!(pHdr->findOffs & 1))
                        {
                            pHdr->findOffs |= 1;
                            OnSelChanged(GetParent(hwndDlg));
                        }
                    }
                    else
                    {
                        if (pHdr->findOffs & 1)
                        {
                            pHdr->findOffs &= ~1;
                            OnSelChanged(GetParent(hwndDlg));
                        }
                    }
                    SetFlags(hwndDlg, pHdr);
                    EndFind();
                    break;
                case IDC_COMBOREPLACEWHERE:
                    if ((replacemode = SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_GETCURSEL, 0, 0)) == 5)
                    {
                        if (!(pHdr->replaceOffs & 1))
                        {
                            pHdr->replaceOffs |= 1;
                            OnSelChanged(GetParent(hwndDlg));
                        }
                    }
                    else
                    {
                        if (pHdr->replaceOffs & 1)
                        {
                            pHdr->replaceOffs &= ~1;
                            OnSelChanged(GetParent(hwndDlg));
                        }
                    }
                    SetFlags(hwndDlg, pHdr);
                    EndFind();
                    break;
                case IDC_COMBOFINDHOW:
                    n = SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_GETCURSEL, 0, 0);
                    findflags &= ~(F_REGULAR | F_WILDCARD);
                    switch (n)
                    {
                        case 0:
                            break;
                        case 1:
                            findflags |= F_REGULAR;
                            break;
                        case 2:
                            findflags |= F_WILDCARD;
                            break;
                    }
                    break;
                case IDC_COMBOREPLACEHOW:
                    n = SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_GETCURSEL, 0, 0);
                    replaceflags &= ~(F_REGULAR | F_WILDCARD);
                    switch (n)
                    {
                        case 0:
                            break;
                        case 1:
                            replaceflags |= F_REGULAR;
                            break;
                        case 2:
                            replaceflags |= F_WILDCARD;
                            break;
                    }
                    break;
                case IDC_CHECKFINDWHOLE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags ^= F_WHOLEWORD;
                    }
                    break;
                case IDC_CHECKREPLACEWHOLE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        replaceflags ^= F_WHOLEWORD;
                    }
                    break;
                case IDC_CHECKFINDCASE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags ^= F_MATCHCASE;
                    }
                    break;
                case IDC_CHECKREPLACECASE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        replaceflags ^= F_MATCHCASE;
                    }
                    break;
                case IDC_CHECKFINDRECURSIVE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags ^= F_SUBDIR;
                    }
                    break;
                case IDC_CHECKREPLACERECURSIVE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        replaceflags ^= F_SUBDIR;
                    }
                    break;
                case IDC_CHECKFINDUP:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags ^= F_DOWN;
                    }
                    break;
                case IDC_RADIOFINDDOCUMENT:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags &= ~(F_OUTPUT1 | F_OUTPUT2 | F_OPENWINDOW);
                        findflags |= F_OPENWINDOW;
                        SetFindNextButton(hwndDlg);
                    }
                    break;
                case IDC_RADIOFIND1:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags &= ~(F_OUTPUT1 | F_OUTPUT2 | F_OPENWINDOW);
                        findflags |= F_OUTPUT1;
                        SetFindNextButton(hwndDlg);
                    }
                    break;
                case IDC_RADIOFIND2:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags &= ~(F_OUTPUT1 | F_OUTPUT2 | F_OPENWINDOW);
                        findflags |= F_OUTPUT2;
                        SetFindNextButton(hwndDlg);
                    }
                    break;
                case IDCANCEL:
                    SendMessage(hwndDlg, WM_USER, 0, 0);
                    canceled = TRUE;
                    SendMessage(GetParent(hwndDlg), WM_COMMAND, IDCANCEL, 0);
                    break;
                case IDC_FINDNEXT:
                case IDC_REPLACEFINDNEXT:
                {
                    HWND child;
                    if (LOWORD(wParam) == IDC_FINDNEXT)
                    {
                        child = GetDlgItem(hwndDlg, IDC_COMBOFINDFIND);
                        GetBrowsePath(hwndDlg, IDC_COMBOFINDPATH);
                        xGetFileType(hwndDlg, IDC_COMBOFINDTYPE);
                    }
                    else
                    {
                        child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEFIND);
                        GetBrowsePath(hwndDlg, IDC_COMBOREPLACEPATH);
                        xGetFileType(hwndDlg, IDC_COMBOREPLACETYPE);
                    }
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
                    SendMessage(child, EM_SETMODIFY, 0, 0);
                    SendMessage(child, WM_SAVEHISTORY, 0, 0);
                    if (findFromTB)
                    {
                        findFromTB = FALSE;
                        EndFind();
                    }
                    _beginthread((BEGINTHREAD_FUNC)DoFindNext, 0, (LPVOID)hwndDlg);
                }
                break;
                case IDC_REPLACENEXT:
                {
                    HWND child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEFIND);
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
                    child = GetDlgItem(hwndDlg, IDC_COMBOREPLACE);
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)replaceText);
                    findFromTB = FALSE;
                    replaceflags &= ~F_REPLACEALL;
                    GetBrowsePath(hwndDlg, IDC_COMBOREPLACEPATH);
                    xGetFileType(hwndDlg, IDC_COMBOREPLACETYPE);
                    _beginthread((BEGINTHREAD_FUNC)DoReplaceNext, 0, (LPVOID)hwndDlg);
                }
                break;
                case IDC_REPLACEALL:
                {
                    HWND child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEFIND);
                    EndFind();
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
                    child = GetDlgItem(hwndDlg, IDC_COMBOREPLACE);
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)replaceText);
                    if (findFromTB)
                    {
                        findFromTB = FALSE;
                        EndFind();
                    }
                    replaceflags |= F_REPLACEALL;
                    GetBrowsePath(hwndDlg, IDC_COMBOREPLACEPATH);
                    xGetFileType(hwndDlg, IDC_COMBOREPLACETYPE);
                    _beginthread((BEGINTHREAD_FUNC)DoReplaceNext, 0, (LPVOID)hwndDlg);
                }
                break;
            }
            break;
    }
    return 0;
}
VOID WINAPI OnSelChanged(HWND hwndDlg)

{
    DLGHDR* pHdr = (DLGHDR*)GetWindowLong(hwndDlg, GWL_USERDATA);
    int iSel = pHdr->iSel = TabCtrl_GetCurSel(pHdr->hwndTab);
    DWORD dwDlgBase = GetDialogBaseUnits();
    int cxMargin = LOWORD(dwDlgBase) / 4;
    int cyMargin = HIWORD(dwDlgBase) / 8;
    RECT rcTab;
    if (iSel == 0)
    {
        if (pHdr->hwndDisplay)
        {
            HWND child = GetDlgItem(pHdr->hwndDisplay, IDC_COMBOFINDFIND);
            SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
        }
        iSel = pHdr->findOffs;
    }
    else
    {
        if (pHdr->hwndDisplay)
        {
            HWND child = GetDlgItem(pHdr->hwndDisplay, IDC_COMBOREPLACEFIND);
            SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
            child = GetDlgItem(pHdr->hwndDisplay, IDC_COMBOREPLACE);
            SendMessage(child, WM_GETTEXT, 256, (LPARAM)replaceText);
        }
        iSel = 8 + pHdr->replaceOffs;
    }

    if (pHdr->hwndDisplay != NULL)
        DestroyWindow(pHdr->hwndDisplay);

    SetRectEmpty(&rcTab);
    rcTab.right = pHdr->apRes[iSel]->cx;
    rcTab.bottom = pHdr->apRes[iSel]->cy;
    rcTab.right = rcTab.right * LOWORD(dwDlgBase) / 4;
    rcTab.bottom = rcTab.bottom * HIWORD(dwDlgBase) / 8;

    TabCtrl_AdjustRect(pHdr->hwndTab, TRUE, &rcTab);
    OffsetRect(&rcTab, cxMargin - rcTab.left, cyMargin - rcTab.top);

    CopyRect(&pHdr->rcDisplay, &rcTab);
    TabCtrl_AdjustRect(pHdr->hwndTab, FALSE, &pHdr->rcDisplay);

    SetWindowPos(pHdr->hwndTab, NULL, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, SWP_NOZORDER);

    SetWindowPos(hwndDlg, NULL, findDlgPos.x, findDlgPos.y, rcTab.right + 2 * cxMargin + 2 * GetSystemMetrics(SM_CXDLGFRAME),
                 rcTab.bottom + 2 * cyMargin + 2 * GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYCAPTION), SWP_NOZORDER);
    pHdr->hwndDisplay = CreateDialogIndirect(hInstance, pHdr->apRes[iSel], hwndDlg, (DLGPROC)FindChildDlgProc);
}
LRESULT CALLBACK FindDlgProc(HWND hwndDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    DLGHDR* pHdr;
    TC_ITEM tie;
    int i;
    switch (iMessage)
    {
        case WM_INITDIALOG:
            inSetSel = FALSE;
            if (usingfif)
            {
                usingfif = FALSE;
                if (findflags & F_OPENWINDOW)
                {
                    findflags &= ~F_OPENWINDOW;
                    if (!(findflags & (F_OUTPUT1 | F_OUTPUT2)))
                        findflags |= F_OUTPUT1;
                    findmode = F_M_CURRENTPROJECT;
                    replacemode = F_M_CURRENTPROJECT;
                }
                else if (!(findflags & (F_OUTPUT1 | F_OUTPUT2)))
                    findflags |= F_OUTPUT1;
            }
            else
            {
                if (!(findflags & F_OPENWINDOW))
                {
                    findflags &= ~(F_OUTPUT1 | F_OUTPUT2);
                    findflags |= F_OPENWINDOW;
                    findmode = F_M_CURRENTDOCUMENT;
                }
                replacemode = F_M_CURRENTDOCUMENT;
            }
            hwndFind = hwndDlg;
            pHdr = (DLGHDR*)calloc(1, sizeof(DLGHDR));
            SetWindowLong(hwndDlg, GWL_USERDATA, (LONG)pHdr);
            pHdr->hwndTab = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 100, 100, hwndDlg, NULL,
                                         hInstance, NULL);
            tie.mask = TCIF_TEXT | TCIF_IMAGE;
            tie.iImage = -1;
            tie.pszText = "Find";
            TabCtrl_InsertItem(pHdr->hwndTab, 0, &tie);
            tie.pszText = "Replace";
            TabCtrl_InsertItem(pHdr->hwndTab, 1, &tie);
            for (i = 0; i < 8; i++)
                pHdr->apRes[i] = DoLockDlgRes(MAKEINTRESOURCE(FINDDLG_0 + i));
            for (i = 0; i < 4; i++)
                pHdr->apRes[i + 8] = DoLockDlgRes(MAKEINTRESOURCE(REPLACEDLG_0 + i));
            pHdr->plusBmp = LoadBitmap(hInstance, "ID_PLUS99");
            pHdr->minusBmp = LoadBitmap(hInstance, "ID_MINUS99");
            pHdr->findEnabled = TRUE;
            if (lParam)
                TabCtrl_SetCurSel(pHdr->hwndTab, 1);
            if (findmode == F_M_SPECIFIEDPATH)
                pHdr->findOffs = 1;  // select the path dialog
            if (replacemode == F_M_SPECIFIEDPATH)
                pHdr->replaceOffs = 1;
            OnSelChanged(hwndDlg);
            return TRUE;
        case WM_DESTROY:
            pHdr = (DLGHDR*)GetWindowLong(hwndDlg, GWL_USERDATA);
            DestroyWindow(pHdr->hwndTab);
            DestroyWindow(pHdr->hwndDisplay);
            free(pHdr);
            hwndFind = NULL;
            hwndFindInternal = NULL;
            break;
        case WM_FOCUSFIND:
            pHdr = (DLGHDR*)GetWindowLong(hwndDlg, GWL_USERDATA);
            inSetSel = FALSE;
            if (wParam)
            {
                if (!pHdr->iSel)
                {
                    TabCtrl_SetCurSel(pHdr->hwndTab, 1);
                    OnSelChanged(hwndDlg);
                }
                SetFocus(GetDlgItem(pHdr->hwndDisplay, IDC_COMBOREPLACEFIND));
            }
            else
            {
                if (pHdr->iSel)
                {
                    TabCtrl_SetCurSel(pHdr->hwndTab, 0);
                    OnSelChanged(hwndDlg);
                }
                SetFocus(GetDlgItem(pHdr->hwndDisplay, IDC_COMBOFINDFIND));
            }
            break;
        case WM_NOTIFY:
            switch (((LPNMHDR)lParam)->code)
            {
                case TCN_SELCHANGING:
                    SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
                    break;
                case TCN_SELCHANGE:
                    OnSelChanged(hwndDlg);
                    break;
            }
            break;
        case WM_MOVE:
            findDlgPos.x = LOWORD(lParam);
            findDlgPos.y = HIWORD(lParam);
            //            ScreenToClient(hwndFrame, &findDlgPos);
            findDlgPos.x -= GetSystemMetrics(SM_CXDLGFRAME);
            findDlgPos.y -= GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                    pHdr = (DLGHDR*)GetWindowLong(hwndDlg, GWL_USERDATA);
                    return FindChildDlgProc(pHdr->hwndDisplay, iMessage, wParam, lParam);
                case IDCANCEL:
                    SetEvent(findEvent);
                    SetFocus((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0));
                    rehighlight("", 0, 0);
                    EndDialog(hwndDlg, 0);
                    pHdr = (DLGHDR*)GetWindowLong(hwndDlg, GWL_USERDATA);
                    FindChildDlgProc(pHdr->hwndDisplay, WM_USER, 0, 0);
                    break;
                case IDHELP:
                    ContextHelp(IDH_FIND_REPLACE_DIALOG);
                    break;
            }
            break;
    }
    return 0;
}

void OpenFindDialog(void)
{
    if (WaitForSingleObject(findEvent, 0) == WAIT_OBJECT_0)
    {
        CreateDialogParam(hInstance, "FINDSTUBDLG", hwndFrame, (DLGPROC)FindDlgProc, 0);
    }
    else
    {
        SendMessage(hwndFind, WM_FOCUSFIND, FALSE, 0);
    }
}
void OpenReplaceDialog(void)
{
    if (WaitForSingleObject(findEvent, 0) == WAIT_OBJECT_0)
    {
        CreateDialogParam(hInstance, "FINDSTUBDLG", hwndFrame, (DLGPROC)FindDlgProc, 1);
    }
    else
    {
        SendMessage(hwndFind, WM_FOCUSFIND, TRUE, 0);
    }
}
void OpenFindInFilesDialog(void)
{
    if (WaitForSingleObject(findEvent, 0) == WAIT_OBJECT_0)
    {
        usingfif = 1;
        CreateDialogParam(hInstance, "FINDSTUBDLG", hwndFrame, (DLGPROC)FindDlgProc, 0);
    }
    else
    {
        SendMessage(hwndFindInternal, WM_FOCUSFIND, FALSE, 0);
    }
}
