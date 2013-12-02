/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include "helpid.h"
#include "header.h"
#include "codecomp.h"
#include "regexp.h"
#include "winconst.h"
#include <ctype.h>


#define EDITOR_OFFSET 35

extern HINSTANCE hInstance;
extern HWND hwndClient;
extern HWND hwndTbFind;
extern HWND hwndFrame;
extern char highlightText[256] ;
extern int highlightCaseSensitive;
extern int highlightWholeWord;
extern DWINFO *editWindows;
extern PROJECTITEM *activeProject;
extern PROJECTITEM *workArea;
extern char szInstallPath[];

DWORD BrowseForFile(HWND hwnd, LPSTR pszDisplayName, LPSTR pszPath, UINT cchPath);

HWND hwndFind = NULL, hwndFindInternal = NULL;

char *findhist[MAX_COMBO_HISTORY];
char *replacehist[MAX_COMBO_HISTORY];
char *findbrowsehist[MAX_COMBO_HISTORY];
char *replacebrowsehist[MAX_COMBO_HISTORY];

POINT findDlgPos;

static BOOL infindorreplace;
static BOOL dirty;
static BOOL findFromTB;
BOOL finding = TRUE;
static BOOL replacing = FALSE;
static BOOL usingfif;
static int tempfind;
static int tempreplace;
static int tempfindext;
static int tempreplaceext;
static char browsePath[MAX_PATH];
static char fileType[MAX_PATH];
static BOOL canceled = FALSE;
static BOOL inSetSel;
static int replaceCount;

int findflags[F_M_MAX] =
{
    F_DOWN | F_OPENWINDOW,
    F_DOWN | F_OPENWINDOW,
    F_DOWN | F_OUTPUT1 | F_SUBDIR,
    F_DOWN | F_OUTPUT1 | F_SUBDIR,
    F_DOWN | F_OUTPUT1 | F_SUBDIR,
    F_DOWN | F_OUTPUT1 | F_SUBDIR,
} ;
int replaceflags[F_M_MAX] =
{
    F_DOWN | F_OPENWINDOW,
    F_DOWN | F_OPENWINDOW,
    F_DOWN | F_OPENWINDOW | F_SUBDIR,
    F_DOWN | F_OPENWINDOW | F_SUBDIR,
    F_DOWN | F_OPENWINDOW | F_SUBDIR,
    F_DOWN | F_OPENWINDOW | F_SUBDIR,
} ;
int findmode = F_M_OPENDOCUMENTS;
int replacemode = F_M_CURRENTDOCUMENT;
int fiffindmode = F_M_CURRENTPROJECT;
int fifreplacemode = F_M_CURRENTPROJECT;
int findext = 4;
int replaceext = 0;
int fiffindext = 0;
int fifreplaceext = 0;

typedef struct _bufList
{
    struct _bufList *fwd, *back;
    DWINFO *ptr;
    CHARRANGE range;
} BUFLIST;

static char findText[256];
static char replaceText[256];

static CHARRANGE findPos, replacePos;
static char *currentBuffer;
static DWINFO *currentWindow;
static BUFLIST *opened;
static char *headerExt = ".h;.hxx";

static HANDLE findEvent;
void findInit(void)
{
    findEvent = CreateEvent(0, FALSE, TRUE, 0);
}
//-------------------------------------------------------------------------

static void nm_matches(char *search, int flags, char *buf, CHARRANGE *pos)
{
    if (flags &F_DOWN)
    {
        if (flags &FR_MATCHCASE)
            pos->cpMin = (int)strstr(buf + pos->cpMin, search);
        else
            pos->cpMin = (int)stristr(buf + pos->cpMin, search);
        if (pos->cpMin)
            pos->cpMin = pos->cpMin - (int)buf;
        else
            pos->cpMin = pos->cpMax;
    }
    else
    {
        int l = strlen(search);
        do
        {
            if (flags &F_MATCHCASE)
            {
                if (!strncmp(buf + pos->cpMax, search, l))
                    break;
            }
            else
                if (!strnicmp(buf + pos->cpMax, search, l))
                    break;
            pos->cpMax--;
        } while (pos->cpMax >= pos->cpMin && !canceled);
    }
}
static int getSegment(char *result, char **source)
{
    int n = 0;
    while (**source && **source != '?' && **source != '*')
    {
        *result ++= *(*source)++;
        n++;
    }
    *result = 0;
    return n;
}
static int matches_wc(char *search, int flags, char *buf, int pos, int *len)
{
    /* this assumes that a '*' will be prepended to whatever they ask for */
    int start=-1;
    char segment[1024];
    char *orgsearch = search;
    char *orgpos = pos;
    
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
                p = strchr(buf+pos, '\n');
                
                if (flags &FR_MATCHCASE)
                    q = (int)strstr(buf+pos, segment);
                else
                    q = (int)stristr(buf+pos, segment);
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
            char *q;
            if (search == orgsearch)
            {
                int l = getSegment(segment, &search);
                if (flags & FR_DOWN)
                {
                    if (flags &FR_MATCHCASE)
                        q = (int)strstr(buf+pos, segment);
                    else
                        q = (int)stristr(buf+pos, segment);
                }
                else
                {
                    q = 0;
                    if (flags &FR_MATCHCASE)
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
                if (flags &F_MATCHCASE)
                {
                    if (strncmp(buf + pos, segment, l))
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
                    if (strncmp(buf + pos, segment, l))
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
static void wc_matches(char *search, int flags, char *buf, CHARRANGE *pos, int *len)
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
static void rex_matches(RE_CONTEXT *context, int flags, char *buf, CHARRANGE *pos, int *len)
{
    if (flags &F_DOWN)
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
static int countLines(char *start, char *end)
{
    int rv = 0;
    char *p = start-1;
    while(p && p < end)
    {
        p = strchr(p+1, '\n');
        if (p && p < end)
            rv++;
        
    }
    return rv;
}
static void SendToOutput(char *fname, int flags, char *start, char *end)
{
    if (flags & (F_OUTPUT1 | F_OUTPUT2))
    {
        char line[1000];
        char buf[1500];
        int fifwindow = (flags & F_OUTPUT2) ? ERR_FIND2_WINDOW : ERR_FIND1_WINDOW;
        int n = countLines(start, end) + 1;
        char *ls = end, *le = end;
        while (ls > start && ls[-1] != '\n')
            ls--;
        while (*le && *le != '\r' && *le != '\n')
            le++;
        if (le - ls > sizeof(line))
            le = ls + sizeof(line)-1;
        memcpy(line, ls, le - ls);
        line[le - ls] = 0;
        sprintf(buf, "%s%7d %s\n", fname, n, line);
        SendInfoMessage(fifwindow, buf);
        Tag(fifwindow == ERR_FIND1_WINDOW ? TAG_FIF1 : TAG_FIF2, fname, n, 0, 0, 0, 0);
    }
    else if (flags & F_OPENWINDOW)
    {
        DWINFO *ptr = GetFileInfo(fname);
        if (!ptr)
        {
            DWINFO *p = calloc(1, sizeof(DWINFO));
            char *q = strrchr(fname, '\\');
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
            while (!GetFileInfo(fname) && !canceled) Sleep(10);
        }
    }
}
static int xfind(char *fname, char *search, int flags, char *buf, int *len, CHARRANGE *pos, 
                 RE_CONTEXT *context)
{
    int i;
    *len = strlen(search);
    while (!canceled)
    {
        int rv;
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
            
        if (flags &F_WHOLEWORD)
        {
            if (flags & FR_DOWN)
            {
            
                if (pos->cpMin && isalnum(buf[pos->cpMin - 1]))
                {
                    pos->cpMin +=  *len;
                }
                else if (!buf[pos->cpMin +  *len] || !isalnum(buf[pos->cpMin +  *len]))
                {
                    SendToOutput(fname, flags, buf, buf + pos->cpMin);
                    return pos->cpMin;
                }
                else
                {
                    pos->cpMin +=  *len;
                }
            }
            else
            {
                if (pos->cpMax && isalnum(buf[pos->cpMax - 1]))
                {
                    pos->cpMax --;
                }
                else if (!buf[pos->cpMax +  *len] || !isalnum(buf[pos->cpMax +  *len]))
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
}
static RE_CONTEXT * getREContext(char *search, int flags)
{
    RE_CONTEXT *rv = NULL;
    if (flags & F_REGULAR)
    {
        rv = re_init(search, RE_F_REGULAR | 
                          ((flags & F_MATCHCASE) ? 0 : RE_F_INSENSITIVE) |
                          ((flags & F_WHOLEWORD) ? RE_F_WORD : 0), NULL);
        if (!rv)
        {
            ExtendedMessageBox("Error",
                MB_SETFOREGROUND | MB_SYSTEMMODAL, "There is a problem with the regular expression");
            rv = (RE_CONTEXT *)-1;
        }
    }
    return rv;
}
static void FindAllInBuffer(char *fname, char *buf, char *search, int flags)
{
    int fifwindow = (flags & F_OUTPUT2) ? ERR_FIND2_WINDOW : ERR_FIND1_WINDOW;
    CHARRANGE pos;
    int len = 0;
    RE_CONTEXT *context = getREContext(search, flags);
    if (context == (RE_CONTEXT *)-1)
        return;
    pos.cpMin = 0;
    pos.cpMax = strlen(buf);
    flags |= F_DOWN;
    while ((pos.cpMin = xfind(fname, search, flags, buf, &len, &pos, context)) >= 0) pos.cpMin += len ;
    if (context)
        re_free(context);
    
}
//-------------------------------------------------------------------------

static int FindNextInDocument(DWINFO *ptr, char *buf, CHARRANGE *pos, int flags, char *search, RE_CONTEXT *context)
{

    BOOL rv = FALSE;
    BOOL val;
    int len = 0;
    SetStatusMessage("", FALSE);
    val = xfind(ptr->dwName, search, flags, buf, &len, pos, context);
    if (val >= 0)
    {
        CHARRANGE r;
        HWND win;
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
        SendMessage(ptr->dwHandle, EM_SETSEL,  - 1, 0);
        SendMessage(ptr->self, WM_COMMAND, ID_REDRAWSTATUS, 0);
    }
    return rv;
}
static BOOL matchesExt(char *name, char *extensions)
{
    char *p = strrchr(name, '.');
    if (p && p[-1] != '.')
    {
        char buf[MAX_PATH];
        while (*extensions)
        {
            char *q = buf;
            while (*extensions && *extensions != ';')
            {
                *q++ = *extensions++;
            }
            *q = 0;
            if (*extensions)
                extensions++;
            if (!strcmp(buf+1, ".*"))
                return TRUE;
            if (!xstricmp(buf+1, p))
                return TRUE;
        } 
        
    }
    return FALSE;
}
static int sortfunc(const void *Left, const void *Right)
{
    const DWINFO **left = Left;
    const DWINFO **right = Right;
    return xstricmp((*left)->dwName, (*right)->dwName);
}
static BUFLIST **AddToTail(DWINFO **files, int count, BUFLIST **tail)
{
    int i;
    qsort(files, count, sizeof(DWINFO *), sortfunc);
    for (i=0; i < count; i++)
    {
        BUFLIST *newBuf = calloc(1,sizeof(BUFLIST));
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
void AddFileToArray(DWINFO ***files, int *count, int *max, char *name)
{
    DWINFO *ptr = GetFileInfo(name);
    if (!ptr)
    {
        HANDLE h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, 
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
            *files = realloc(*files, sizeof(DWINFO *)* (*max));
        }
        if (*files)
            (*files)[(*count)++] = ptr;
    }
}
static BUFLIST **FindLoadProject(PROJECTITEM *pi, BUFLIST **tail, int flags, char *extensions)
{
    static int max, count;
    static DWINFO **files;
    HANDLE handle;
    if (!pi)
        return NULL;
    pi = pi->children;
    while (pi && !canceled)
    {
        if (pi->type == PJ_FOLDER)
        {
            
            if (flags & F_SUBDIR)
            {
                FindLoadProject(pi, NULL, flags, extensions);
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
static void FindLoadPath(char *path, BUFLIST **tail, int flags, char *extensions)
{
    static int max, count;
    static DWINFO **files;
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
                    if (data.cFileName[0] != '.' || (data.cFileName[1] != 0 && (data.cFileName[1] != '.' || data.cFileName[2] != 0)) )
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
        SendMessage(currentWindow->dwHandle, EM_HIDESELECTION, 0,0);
        currentWindow = NULL;
        if (currentBuffer)
            free(currentBuffer);
        currentBuffer = NULL;
        while (opened)
        {
            BUFLIST *next = opened->fwd;
            if (opened->ptr->uninstantiated)
                free(opened->ptr);
            free(opened);
            opened = next;
        }
    }
}
static char *LoadBuffer(DWINFO *ptr)
{
    char *rv;
    if (IsWindow(ptr->self))
    {
        int n = SendMessage(ptr->dwHandle, WM_GETTEXTLENGTH, 0, 0);
        if (n)
        {
            rv = calloc(1, n+1);
            if (rv)
                SendMessage(ptr->dwHandle, WM_GETTEXT, n+1, (LPARAM)rv);
        }
    }
    else
    {
        FILE *fil = fopen(ptr->dwName, "r");
        if (fil)
        {
            int n;
            fseek(fil, 0, SEEK_END);
            n = ftell(fil);
            fseek(fil, 0, SEEK_SET);
            rv = calloc(1, n+1);
            if (rv)
            {
                fread(rv, 1, n+1, fil);
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
        char *p = LoadBuffer(currentWindow);
        if (p)
        {            
            free(currentBuffer);
            currentBuffer = p;
        }
    }
}
void FindResetWindows()
{
    CHARRANGE *findPointer = &findPos;
    int flags = findflags[findmode];
    int mode = findmode;
    if (!finding)
    {
        findPointer = &replacePos;
        flags = replaceflags[replacemode];
        mode = replacemode;
    }
    //select the currently active window as the first thing to search in
    if (currentWindow)
    {
        HWND hwnd;
        CHARRANGE a;
        SendMessage(currentWindow->dwHandle, EM_EXGETSEL, 0, (WPARAM) &a);
        hwnd = (HWND)SendMessage(hwndClient,WM_MDIGETACTIVE, 0, 0);
        if (IsEditWindow(hwnd))
        {
            DWINFO *info = (DWINFO *)GetWindowLong(hwnd, 0);
            if (info != currentWindow)
            {
                if (mode != F_M_OPENDOCUMENTS)
                {
                    EndFind();
                }
                else {
                    BUFLIST *p = opened;
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
                            SendMessage(info->dwHandle, EM_EXGETSEL, 0, (LPARAM)&a);
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
                        SendMessage(info->dwHandle, EM_EXGETSEL, 0, (LPARAM)&a);
                        
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
                    if (findflags[findmode] & F_DOWN)
                    {
//                        a.cpMin += strlen(findText);
                    }
                    else
                    {
                        a.cpMax -= strlen(findText)+1;
                    }
                }
                else
                {
                    if (replaceflags[replacemode] & F_DOWN)
                    {
//                        if (!replacing)
//                            a.cpMin += strlen(replaceText);
                    }
                    else
                    {
                        if (!replacing)
                            a.cpMax -= strlen(findText)+1;
                    }
                }    
            }
        }    
        if (flags & F_DOWN)
        {
            findPointer->cpMin = a.cpMin;
        }
        else
        {
            findPointer->cpMax = a.cpMax == -1 ? SendMessage(currentWindow->dwHandle, WM_GETTEXTLENGTH, 0, 0) : a.cpMax;
        }
    }
    if (dirty) // else if dirty restart the find
    {
        dirty = FALSE;
        RestartFind();
    }
    replacing = FALSE;
}
void FindSetDirty()
{
    dirty = TRUE;
}
static DWINFO *GetNextFindDocument(CHARRANGE *pos, int mode, DWINFO **ptr, char *specifiedPath, char *specifiedExtensions, int flags)
{
    if (!editWindows)
        return NULL;
    if (!*ptr)
    {
        BUFLIST **tail = &opened;
        DWINFO *eq = editWindows;
        switch (mode)
        {
            BUFLIST *newBuf;
            PROJECTITEM *pi;
            char path[MAX_PATH];
            case F_M_CURRENTDOCUMENT:
                newBuf = calloc(1, sizeof(BUFLIST));
                if (newBuf)
                {
                    newBuf->back = *tail;
                    *tail = newBuf;
                    tail = &(*tail)->fwd;
                    *tail = newBuf;
                    newBuf->ptr = editWindows;
                    SendMessage(editWindows->dwHandle, EM_EXGETSEL, (WPARAM)0, (LPARAM) &newBuf->range);
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
                                BUFLIST *newBuf2 = calloc(1, sizeof(BUFLIST));
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
                                BUFLIST *newBuf2 = calloc(1, sizeof(BUFLIST));
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
                        SendMessage(editWindows->dwHandle, EM_EXGETSEL, (WPARAM)0, (LPARAM) &newBuf->range);
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
                        newBuf = calloc(1,sizeof(BUFLIST));
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
                FindLoadProject(activeProject, tail, flags, specifiedExtensions);
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
         BUFLIST *next =opened->fwd;
         char *rv = NULL;
        
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
int FindStringFromToolbar(char *search)
{
    static char last[256];
    RE_CONTEXT *context = getREContext(search, F_WILDCARD | F_DOWN);
    char *buf = currentBuffer;
    DWINFO *ptr = currentWindow;
    if (!search)
        search = last;
    if (!*search)
        return 0;
    if (!findFromTB || strcmp(search, last))
    {
        EndFind();
        if (ptr)
            SendMessage(ptr->dwHandle, EM_SETSEL, -1, 0);
        ptr = NULL;
        strncpy(last, search, sizeof(last)-1);
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
static int FindStringFromFiles(int mode, int flags, char *search, char *specifiedPath, char *specifiedExtension)
{
    RE_CONTEXT *context = getREContext(search, flags);
    DWINFO *ptr = currentWindow;
    char *buf;
    findFromTB = FALSE;
    if (flags & (F_OUTPUT1 | F_OUTPUT2))
    {
        int fifwindow = (flags & F_OUTPUT2) ? ERR_FIND2_WINDOW : ERR_FIND1_WINDOW;
        EndFind();
        TagRemoveAll(fifwindow == ERR_FIND1_WINDOW ? TAG_FIF1 : TAG_FIF2);
        SelectInfoWindow(fifwindow);
        ptr = NULL;
        do
        {
            buf = GetNextFindDocument(&findPos, mode, &ptr, specifiedPath, specifiedExtension, flags);
            if (buf)
                FindAllInBuffer(ptr->dwName, buf, search, flags);
        } while (buf && !canceled); 
        SendInfoMessage(fifwindow, "Done\n");
//        SendInfoDone(fifwindow);
    }
    else
    {
        static char last[256];
        if (strcmp(search, last))
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
        if (!buf && !(flags &FR_REPLACEALL))
        {
            ExtendedMessageBox("Search",0,"Search reached end of region.");
        }
    }
    if (context)
        re_free(context);
}
//-------------------------------------------------------------------------

static void GetReplaceText(char *dest, char *replace, char *dta, int len, int flags, RE_CONTEXT *context)
{
    if (flags & F_REGULAR)
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
                                for (k=0, j = context->matchOffsets[index][0]; j <
                                    context->matchOffsets[index][1] &&  k <
                                    255; j++, k++)
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
static int ReplaceNextInDocument(DWINFO *ptr, char *buf, int *ofs, CHARRANGE *pos, int flags, char *search, char *replace, RE_CONTEXT *context)
{
    BOOL val;
    CHARRANGE find;
    int len = 0;
    val = xfind(ptr->dwName, search, flags, buf, &len, pos, context);
    if (val >= 0)
    {
        char repl[1000];
        int curpos;
        int l;
        CHARRANGE aa;
        if (!ptr->self)
        {
            DWINFO *ptrx = GetFileInfo(ptr->dwName);
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
        *ofs += l;
        inSetSel = TRUE;
        SendMessage(ptr->dwHandle, EM_HIDESELECTION, 1, 0);
        SendMessage(ptr->dwHandle, EM_EXSETSEL, 0, (LPARAM) &aa);
        SendMessage(ptr->dwHandle, EM_REPLACESEL, 1, (LPARAM)repl);
        SendMessage(ptr->dwHandle, EM_HIDESELECTION, 0, 0);
        SendMessage(ptr->self, WM_COMMAND, ID_REDRAWSTATUS, 0);
        inSetSel = FALSE;
        replaceCount++;
        return TRUE;
    }
    return FALSE;
}
static void ReplaceInDocuments(int mode, int flags, char *search, char *replace, char *specifiedPath, char *specifiedExtension)
{
    RE_CONTEXT *context = getREContext(search, flags);
    DWINFO *ptr = currentWindow, *lastPtr = 0;
    int len;
    char *buf = currentBuffer;
    BOOL replaced = FALSE;
    BOOL needsUpdate = FALSE;
    int offset = 0;
    int vis;
    int linepos;
    CHARRANGE lastReplacePos;
    // to guard against the EN_LINECHANGE message which does an EndFind();
    do
    {
        if (buf)
        {
            if (ptr && ptr != lastPtr && (flags & F_REPLACEALL))
            {
                if (needsUpdate)
                {
                    if (lastReplacePos.cpMin >= SendMessage(lastPtr->dwHandle, EM_GETSIZE, 0, 0) - offset)
                        lastReplacePos.cpMin = replacePos.cpMax = vis = offset = 0;
                    lastReplacePos.cpMin += offset;
                    lastReplacePos.cpMax += offset;
                    SendMessage(lastPtr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&lastReplacePos);
                    linepos = SendMessage(lastPtr->dwHandle, EM_LINEFROMCHAR, lastReplacePos.cpMin, 0);
                    vis = linepos - vis;
                    SendMessage(lastPtr->dwHandle, EM_SETFIRSTVISIBLELINE, vis, 0);
                }
                vis  = SendMessage(ptr->dwHandle, EM_GETFIRSTVISIBLELINE, 0, 0);
                linepos = SendMessage(ptr->dwHandle, EM_LINEFROMCHAR, replacePos.cpMin, 0);
                vis = linepos - vis;
                lastPtr = ptr;
                lastReplacePos = replacePos;
                needsUpdate = TRUE;
            }
            offset = 0;
            do 
            {
                replaced = ReplaceNextInDocument(ptr, buf, &offset, &replacePos, flags, search, replace, context);
            }
            while ((flags & F_REPLACEALL) && replaced && !canceled);
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
    } while ((!replaced || (flags & F_REPLACEALL) ) && !canceled &&         
             (buf = GetNextFindDocument(&replacePos, mode, &ptr, specifiedPath, specifiedExtension, flags)));
    if (needsUpdate)
    {
        if (replacePos.cpMin >= SendMessage(lastPtr->dwHandle, EM_GETSIZE, 0, 0) - offset)
            replacePos.cpMin = replacePos.cpMax = vis = offset = 0;
        replacePos.cpMin += offset;
        replacePos.cpMax += offset;
        SendMessage(lastPtr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&replacePos);
        linepos = SendMessage(lastPtr->dwHandle, EM_LINEFROMCHAR, replacePos.cpMin, 0);
        vis = linepos - vis;
        SendMessage(lastPtr->dwHandle, EM_SETFIRSTVISIBLELINE, vis, 0);
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
    EnableWindow(GetDlgItem(hwnd, IDC_RADIOFINDDOCUMENT ), state);
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
DWORD CALLBACK DoFindNext(void *p)
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
        PostMessage(hwndDlg, WM_ENABLEFIND, FALSE ,0);
        canceled = FALSE;
        if (findText[0])
        {
            FindStringFromFiles(findmode, findflags[findmode], findText,
                                browsePath, fileType);
        }
        if (canceled)
            EndFind();
        PostMessage(hwndDlg, WM_ENABLEFIND, TRUE ,0);
    }
    infindorreplace = FALSE;
}
DWORD CALLBACK DoReplaceNext(void *p)
{
    HWND hwndDlg = p;
    PostMessage(hwndDlg, WM_ENABLEFIND, FALSE ,0);
    canceled = FALSE;
    replaceCount = 0;
    replacing = TRUE;
    infindorreplace = TRUE;
    if (findText[0])
        ReplaceInDocuments(replacemode, replaceflags[replacemode], findText, replaceText,
                           browsePath, fileType);
    if (canceled)
        EndFind();
    infindorreplace = FALSE;
    replacing = FALSE;
    dirty = TRUE;
    PostMessage(hwndDlg, WM_ENABLEFIND, TRUE ,0);
    if (!canceled)
    {
        if (replaceflags[replacemode] & F_REPLACEALL)
            ExtendedMessageBox("Replace", 0, "Replaced %d occurrences.", replaceCount);
    }
}
void SetFlags(HWND hwndDlg, DLGHDR *pHdr)
{
    if (!pHdr->iSel)
    {
        int sel = SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_GETCURSEL, findmode, 0);
        if (sel >= 0)
        {
            int flags = findflags[sel];
            if (flags & F_REGULAR)
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_SETCURSEL, 1 , 0);
            }
            else if (flags & F_WILDCARD)
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_SETCURSEL, 2 , 0);
            }
            else
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_SETCURSEL, 0 , 0);
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
    }
    else
    {
        int sel = SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_GETCURSEL, findmode, 0);
        if (sel >= 0)
        {
            int flags = replaceflags[sel];
            if (flags & F_REGULAR)
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_SETCURSEL, 1 , 0);
            }
            else if (flags & F_WILDCARD)
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_SETCURSEL, 2 , 0);
            }
            else
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_SETCURSEL, 0 , 0);
            }
            CheckDlgButton(hwndDlg, IDC_CHECKREPLACEWHOLE, (flags & F_WHOLEWORD) ? BST_CHECKED : BST_UNCHECKED); 
            CheckDlgButton(hwndDlg, IDC_CHECKREPLACECASE, (flags & F_MATCHCASE) ? BST_CHECKED : BST_UNCHECKED); 
            CheckDlgButton(hwndDlg, IDC_CHECKREPLACERECURSIVE, (flags & F_SUBDIR) ? BST_CHECKED : BST_UNCHECKED); 
        }
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
DLGTEMPLATE * WINAPI DoLockDlgRes(LPCSTR lpszResName)

{
    HRSRC hrsrc = FindResource(NULL, lpszResName, RT_DIALOG);
    HGLOBAL hglb = LoadResource(hInstance, hrsrc);
    return (DLGTEMPLATE *) LockResource(hglb);
}

VOID WINAPI OnSelChanged(HWND hwndDlg);
LRESULT CALLBACK FindChildDlgProc(HWND hwndDlg, UINT iMessage, WPARAM wParam, LPARAM
                                  lParam)
{
    HWND hwndParent;
    DLGHDR *pHdr;
    switch (iMessage)
    {
        case WM_INITDIALOG:
            hwndFindInternal = hwndDlg;
            hwndParent = GetParent(hwndDlg);
            pHdr = (DLGHDR *) GetWindowLong(hwndParent, GWL_USERDATA);
            SetWindowLong(hwndDlg, GWL_USERDATA, (long)pHdr);
            SetWindowPos(hwndDlg, HWND_TOP,
                         pHdr->rcDisplay.left, pHdr->rcDisplay.top,
                         0, 0, SWP_NOSIZE);
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
                    SendMessage(child, WM_SETHISTORY,0, (LPARAM)findbrowsehist);
                }
                SendDlgItemMessage(hwndDlg, IDC_ICONFINDOPTIONS, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)((pHdr->findOffs & 2) ? pHdr->minusBmp : pHdr->plusBmp));
                SendDlgItemMessage(hwndDlg, IDC_ICONFINDOUTPUT, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)((pHdr->findOffs & 4) ? pHdr->minusBmp : pHdr->plusBmp));
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM)"*.c;*.cpp;*.h");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM)"*.asm;*.nas");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM)"*.def;*.rc");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM)"*.txt");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_ADDSTRING, 0, (LPARAM)"*.*");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDTYPE, CB_SETCURSEL, findext, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM)"Current Document");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM)"Open Documents");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM)"Current Project");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM)"All Projects");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM)"Orange C Include Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_ADDSTRING, 0, (LPARAM)"User-specified Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDWHERE, CB_SETCURSEL, findmode, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_ADDSTRING, 0, (LPARAM)"Direct Match");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_ADDSTRING, 0, (LPARAM)"Regular Expression");
                SendDlgItemMessage(hwndDlg, IDC_COMBOFINDHOW, CB_ADDSTRING, 0, (LPARAM)"Wildcard");
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
                    SendMessage(child, WM_SETHISTORY,0, (LPARAM)replacebrowsehist);
                }
                    
                SendDlgItemMessage(hwndDlg, IDC_ICONREPLACEOPTIONS, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)((pHdr->replaceOffs & 2) ? pHdr->minusBmp : pHdr->plusBmp));
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_ADDSTRING, 0, (LPARAM)"*.c;*.cpp;*.h");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_ADDSTRING, 0, (LPARAM)"*.def;*.rc");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_ADDSTRING, 0, (LPARAM)"*.*");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACETYPE, CB_SETCURSEL, replaceext, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM)"Current Document");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM)"Open Documents");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM)"Current Project");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM)"All Projects");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM)"Orange C Include Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_ADDSTRING, 0, (LPARAM)"User-specified Path");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEWHERE, CB_SETCURSEL, replacemode, 0);
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_ADDSTRING, 0, (LPARAM)"Direct Match");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_ADDSTRING, 0, (LPARAM)"Regular Expression");
                SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_ADDSTRING, 0, (LPARAM)"Wildcard");
                finding = FALSE;
            }
            EnableWindows(hwndDlg, pHdr->findEnabled);
            SetFlags(hwndDlg, pHdr);
            return 1;
        case WM_ENABLEFIND:
            pHdr = (DLGHDR *) GetWindowLong(hwndDlg, GWL_USERDATA);
            pHdr->findEnabled = wParam;
            EnableWindows(hwndDlg, wParam);
            SetFocus((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0,0));
            break;
        case WM_USER:
            pHdr = (DLGHDR *) GetWindowLong(hwndParent, GWL_USERDATA);
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
        case WM_COMMAND:
            pHdr = (DLGHDR *) GetWindowLong(hwndDlg, GWL_USERDATA);
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
                    findflags[findmode] &= ~(F_REGULAR | F_WILDCARD);
                    switch (n)
                    {
                        case 0:
                            break;
                        case 1:
                            findflags[findmode] |= F_REGULAR;
                            break;
                        case 2:
                            findflags[findmode] |= F_WILDCARD;
                            break;
                    }
                    break;
                case IDC_COMBOREPLACEHOW:
                    n = SendDlgItemMessage(hwndDlg, IDC_COMBOREPLACEHOW, CB_GETCURSEL, 0, 0);
                    replaceflags[replacemode] &= ~(F_REGULAR | F_WILDCARD);
                    switch (n)
                    {
                        case 0:
                            break;
                        case 1:
                            replaceflags[replacemode] |= F_REGULAR;
                            break;
                        case 2:
                            replaceflags[replacemode] |= F_WILDCARD;
                            break;
                    }
                    break;
                case IDC_CHECKFINDWHOLE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags[findmode] ^= F_WHOLEWORD;
                    }
                    break;
                case IDC_CHECKREPLACEWHOLE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        replaceflags[replacemode] ^= F_WHOLEWORD;
                    }
                    break;
                case IDC_CHECKFINDCASE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags[findmode] ^= F_MATCHCASE;
                    }
                    break;
                case IDC_CHECKREPLACECASE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        replaceflags[replacemode] ^= F_MATCHCASE;
                    }
                    break;
                case IDC_CHECKFINDRECURSIVE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags[findmode] ^= F_SUBDIR;
                    }
                    break;
                case IDC_CHECKREPLACERECURSIVE:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        replaceflags[replacemode] ^= F_SUBDIR;
                    }
                    break;
                case IDC_CHECKFINDUP:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags[findmode] ^= F_DOWN;
                    }
                    break;
                case IDC_RADIOFINDDOCUMENT:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags[findmode] &= ~(F_OUTPUT1 | F_OUTPUT2 | F_OPENWINDOW);
                        findflags[findmode] |= F_OPENWINDOW;
                    }
                    break;
                case IDC_RADIOFIND1:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags[findmode] &= ~(F_OUTPUT1 | F_OUTPUT2 | F_OPENWINDOW);
                        findflags[findmode] |= F_OUTPUT1;
                    }
                    break;
                case IDC_RADIOFIND2:
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        findflags[findmode] &= ~(F_OUTPUT1 | F_OUTPUT2 | F_OPENWINDOW);
                        findflags[findmode] |= F_OUTPUT2;
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
                    DWORD id;
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
                    CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DoFindNext, hwndDlg, 0, &id));
                }
                    break;
                case IDC_REPLACENEXT:
                {
                    HWND child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEFIND);
                    DWORD id;
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
                    child = GetDlgItem(hwndDlg, IDC_COMBOREPLACE);
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)replaceText);
                    findFromTB = FALSE;
                    replaceflags[replacemode] &= ~F_REPLACEALL;
                    GetBrowsePath(hwndDlg, IDC_COMBOREPLACEPATH);
                    xGetFileType(hwndDlg, IDC_COMBOREPLACETYPE);
                    CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DoReplaceNext, hwndDlg, 0, &id));
                }
                    break;
                case IDC_REPLACEALL:
                {
                    HWND child = GetDlgItem(hwndDlg, IDC_COMBOREPLACEFIND);
                    DWORD id;
                    EndFind();
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)findText);
                    child = GetDlgItem(hwndDlg, IDC_COMBOREPLACE);
                    SendMessage(child, WM_GETTEXT, 256, (LPARAM)replaceText);
                    if (findFromTB)
                    {
                        findFromTB = FALSE;
                        EndFind();
                    }
                    replaceflags[replacemode] |= F_REPLACEALL;
                    GetBrowsePath(hwndDlg, IDC_COMBOREPLACEPATH);
                    xGetFileType(hwndDlg, IDC_COMBOREPLACETYPE);
                    CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DoReplaceNext, hwndDlg, 0, &id));
                }
                    break;
            }
            break;
    }
    return 0;
}
VOID WINAPI OnSelChanged(HWND hwndDlg)

{
    DLGHDR *pHdr = (DLGHDR *) GetWindowLong(hwndDlg, GWL_USERDATA);
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

    SetWindowPos(pHdr->hwndTab, NULL, rcTab.left, rcTab.top,
        rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, 
        SWP_NOZORDER);

    SetWindowPos(hwndDlg, NULL, findDlgPos.x, findDlgPos.y,
        rcTab.right + 2 * cxMargin + 2 * GetSystemMetrics(SM_CXDLGFRAME),
        rcTab.bottom + 2 * cyMargin + 2 * GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYCAPTION),
        SWP_NOZORDER);
    pHdr->hwndDisplay = CreateDialogIndirect(hInstance, pHdr->apRes[iSel], hwndDlg, 
                                             (DLGPROC)FindChildDlgProc);
}
LRESULT CALLBACK FindDlgProc(HWND hwndDlg, UINT iMessage, WPARAM wParam, LPARAM
    lParam)
{
    DLGHDR *pHdr;
    TC_ITEM tie;
    HWND hwndButton;
    RECT rcButton;
    int i;
    switch (iMessage)
    {
        case WM_INITDIALOG:
            inSetSel = FALSE;
            if (usingfif)
            {
                tempfind = findmode;
                tempreplace = replacemode;
                tempfindext = findext;
                tempreplaceext = replaceext;
                findmode = fiffindmode;
                replacemode = fifreplacemode;
                findext = fiffindext;
                replaceext = fifreplaceext;
            }
            hwndFind = hwndDlg;
            pHdr = (DLGHDR *) calloc(1, sizeof(DLGHDR));
            SetWindowLong(hwndDlg, GWL_USERDATA, (LONG) pHdr);
            pHdr->hwndTab = CreateWindow(WC_TABCONTROL, "",
                                WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                                0, 0, 100, 100,
                                hwndDlg, NULL, hInstance, NULL);
            tie.mask = TCIF_TEXT | TCIF_IMAGE;
            tie.iImage = -1;
            tie.pszText = "Find";
            TabCtrl_InsertItem(pHdr->hwndTab, 0, &tie);
            tie.pszText = "Replace";
            TabCtrl_InsertItem(pHdr->hwndTab, 1, &tie);
            for (i=0; i < 8; i++)
                pHdr->apRes[i] = DoLockDlgRes(MAKEINTRESOURCE(FINDDLG_0 + i));
            for (i=0; i < 4; i++)
                pHdr->apRes[i+8] = DoLockDlgRes(MAKEINTRESOURCE(REPLACEDLG_0 + i));
            pHdr->plusBmp = LoadBitmap(hInstance, "ID_PLUS99");
            pHdr->minusBmp = LoadBitmap(hInstance, "ID_MINUS99");
            pHdr->findEnabled = TRUE;
            if (lParam)
                TabCtrl_SetCurSel(pHdr->hwndTab, 1);
            if (findmode == F_M_SPECIFIEDPATH)
                pHdr->findOffs = 1; // select the path dialog
            if (replacemode == F_M_SPECIFIEDPATH)
                pHdr->replaceOffs = 1;
            OnSelChanged(hwndDlg);
            return TRUE;
        case WM_DESTROY:
            pHdr = (DLGHDR *) GetWindowLong(hwndDlg, GWL_USERDATA);
            DestroyWindow(pHdr->hwndTab);
            DestroyWindow(pHdr->hwndDisplay);
            free(pHdr);
            hwndFind = NULL;
            hwndFindInternal = NULL;
            break;
        case WM_FOCUSFIND:
            pHdr = (DLGHDR *) GetWindowLong(hwndDlg, GWL_USERDATA);
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
                    pHdr = (DLGHDR *) GetWindowLong(hwndDlg, GWL_USERDATA);
                    return FindChildDlgProc(pHdr->hwndDisplay, iMessage, wParam, lParam);
                case IDCANCEL:
                    if (usingfif)
                    {
                        usingfif = FALSE;
                        fiffindmode = findmode;
                        fifreplacemode = replacemode;
                        fiffindext = tempfindext;
                        fifreplaceext = tempreplaceext;
                        findmode = tempfind;
                        replacemode = tempreplace;
                        findext = tempfindext;
                        replaceext = tempreplaceext;
                    }
                    SetEvent(findEvent);
                    SetFocus((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0));
                    EndDialog(hwndDlg, 0);
                    pHdr = (DLGHDR *) GetWindowLong(hwndDlg, GWL_USERDATA);
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
