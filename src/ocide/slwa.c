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
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#define WSPVERS 8

#include "header.h"
#include "xml.h"

extern char *currentProfileName;
extern char findText[256];
extern int profileDebugMode;
extern HWND hwndTbFind;
extern int editFlags;
extern DWINFO *editWindows;
extern HINSTANCE hInstance;
extern struct tagfile *tagFileList;
extern HWND hwndToolNav, hwndToolEdit, hwndToolDebug, hwndToolBuild, hwndToolBookmark;
extern HWND hwndClient, hwndFrame;
extern HWND hwndSrcTab;
extern char *findhist[MAX_COMBO_HISTORY];
extern char *findbrowsehist[MAX_COMBO_HISTORY];
extern char *replacehist[MAX_COMBO_HISTORY];
extern char *replacebrowsehist[MAX_COMBO_HISTORY];
extern char *watchhist[MAX_COMBO_HISTORY];
extern char *databphist[MAX_COMBO_HISTORY];
extern char *memhist[MAX_COMBO_HISTORY];
extern DWINFO *newInfo;
extern char szDrawClassName[];
extern char szUntitled[];
extern DATABREAK *dataBpList;
extern char hbpNames[4][256];
extern int hbpAddresses[4];
extern char hbpModes[4], hbpSizes[4], hbpEnables;
extern PROJECTITEM *activeProject;
extern FILEBROWSE *fileBrowseInfo;
extern int fileBrowseCount;
extern POINT findDlgPos;
extern int findflags;
extern int replaceflags;
extern int findmode;
extern int replacemode;
extern int findext;
extern int replaceext;
extern PROFILENAMELIST *profileNames;
extern char *sysProfileName;
extern BOOL snapToGrid;
extern BOOL showGrid;
extern int gridSpacing;

int restoredDocks = FALSE;

static char ipathbuf[2048];

//-------------------------------------------------------------------------

void abspath(char *name, char *path)
{
    char projname[256],  *p,  *nname = name;
    if (!path)
    {
        path = projname;
        GetCurrentDirectory(256, projname);
        strcat(projname,"\\hi");
    }
    if (!path[0])
        return ;
    if (name[0] == 0)
        return ;
    if (name[1] == ':')
        return ;
    strcpy(projname, path);
    p = strrchr(projname, '\\');
    if (!p)
        return ;
    p--;
    if (!strstr(name, "..\\"))
    {
        if (name[0] != '\\')
        {
            strcpy(p + 2, name);
            strcpy(nname, projname);
        }
        return ;
    }
    while (!strncmp(name, "..\\", 3))
    {
        while (p > projname &&  *p-- != '\\')
            ;
        name += 3;
    }
    *++p = '\\';
    p++;
    strcpy(p, name);
    strcpy(nname, projname);
}

//-------------------------------------------------------------------------

char *relpath(char *name, char *path)
{
    static char projname[MAX_PATH], localname[MAX_PATH];
    char *p = localname,  *q = projname,  *r,  *s;
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

    while (*p &&  *q && toupper(*p) == toupper(*q))
    {
        p++, q++;
    }
    if (!(*p |  *q))
        return r;
    else if (*(p - 1) == '\\' && *(p - 2) == ':')
        return name;
    else
    {
        int count =  *q != 0;
        if (*q != '\\')
            while (p > localname &&  *p != '\\')
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

void absincludepath(char *name, char *path)
{
    char *p = ipathbuf,  *dest1 = name,  *dest2 = name;
    strcpy(ipathbuf, name);
    name[0] = 0;
    do
    {
        char *q;
        if (*p == ';')
            p++;
        q = p;
        dest1 = dest2;
        p = strchr(p, ';');
        if (!p)
            p = ipathbuf + strlen(ipathbuf);
        while (q != p)*dest2++ =  *q++;
        *dest2 = 0;
        abspath(dest1, path);
        dest2 = dest1 + strlen(dest1);
        if (*p)
            *dest2++ = ';';
    }
    while (*p)
        ;
}

//-------------------------------------------------------------------------

char *relincludepath(char *name, char *path)
{
    char *p = name,  *dest1 = ipathbuf,  *dest2 = ipathbuf;
    ipathbuf[0] = 0;
    do
    {
        char *q;
        if (*p == ';')
            p++;
        q = p;
        dest1 = dest2;
        p = strchr(p, ';');
        if (!p)
            p = name + strlen(name);
        while (q != p)*dest2++ =  *q++;
        *dest2 = 0;
        strcpy(dest1, relpath(dest1, path));
        dest2 = dest1 + strlen(dest1);
        if (*p)
            *dest2++ = ';';
    }
    while (*p)
        ;
    return ipathbuf;
}

PROJECTITEM *LoadErr(struct xmlNode *root, char *name)
{
    xmlFree(root);
    ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
        "Project File %s is the wrong format", name);
    return 0;
} 
PROJECTITEM *NoMemory(struct xmlNode *root)
{
    xmlFree(root);
    ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
        "Out of memory");
    return 0;
} 

void NoMemoryWS(void)
{
    ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
        "Out of memory");
}

//-------------------------------------------------------------------------

void RestoreDocks(struct xmlNode *node, int version)
{
    CCD_params d[20];
    int ids[20];
    int count = 0;
    struct xmlNode *dchildren = node->children;
    while (dchildren && count < 20)
    {
        if (IsNode(dchildren, "DOCK"))
        {
            struct xmlAttr *attribs = dchildren->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    ids[count] = atoi(attribs->value);
                else if (IsAttrib(attribs, "VALUE"))
                {
                    sscanf(attribs->value, 
                        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d>\n",  
                        &d[count].flags, &d[count].flexparams,  
                        &d[count].rowindex, &d[count].colindex,  
                        &d[count].hidden, &d[count].hiddenwidth,  
                        &d[count].oldsize.left, &d[count].oldsize.top, 
                        &d[count].oldsize.right, &d[count].oldsize.bottom,  
                        &d[count].position.left, &d[count].position.top, 
                        &d[count].position.right, &d[count].position.bottom,  
                        &d[count].lastposition.left, &d[count].lastposition.top, 
                        &d[count].lastposition.right, &d[count].lastposition.bottom);						
                } attribs = attribs->next;
            }
            adjustforbadplacement(ids[count], &d[count]);
            count++;
        }
        dchildren = dchildren->next;
    }
    if (count)
    {
        dmgrSetInfo(ids, &d[0], count);
        restoredDocks = TRUE;
    }
}

//-------------------------------------------------------------------------

void RestoreHistory(struct xmlNode *node, int version)
{
    int count = 0;
    char **histitem = 0;
    struct xmlAttr *attribs = node->attribs;
    struct xmlNode *children = node->children;
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
            else if (!strcmp(attribs->value, "DATABP"))
                histitem = databphist;
            else if (!strcmp(attribs->value, "MEMORY"))
                histitem = memhist;
            else if ( !strcmp(attribs->value, "BFINDPATH"))
                histitem = findbrowsehist;
            else if ( !strcmp(attribs->value, "BREPLACEPATH"))
                histitem = replacebrowsehist;
        } 
        attribs = attribs->next;
    }
    if (!histitem)
        return ;
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

void RestoreWindows(struct xmlNode *node, int version, PROJECTITEM *wa)
{
    MDICREATESTRUCT mc;
    static DWINFO info;
    int i;
    struct xmlNode *child = node->children;
    HWND hwnd = NULL;
    ShowWindow(hwndSrcTab, SW_HIDE);
    while (child)
    {
        if (IsNode(child, "WINDOW"))
        {
            struct xmlAttr *attribs = child->attribs;
            int copyId = 0;
            EDITDATA *extra = NULL;
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
                DWINFO *ptr = editWindows;
                while (ptr)
                {
                    if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, 
                        (LPARAM)newInfo))
                    {
                        extra = (EDITDATA *)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0);
                        break;
                    }
                    ptr = ptr->next;
                }
            }
                
            hwnd = CreateMDIWindow(szDrawClassName, szUntitled,  WS_VISIBLE |
                WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES | 
                WS_SIZEBOX | (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU) |
                WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                mc.x,mc.y,mc.cx,mc.cy, hwndClient, hInstance, (LPARAM)extra); 
            if (copyId)
            {
                SendMessage(hwnd, WM_SETSIBLINGID, copyId-1, 0);
            }
        }
        else if (IsNode(child, "ACTIVE"))
        {
            // should be last
            struct xmlAttr *attribs = child->attribs;
            newInfo = &info;
            info.dwLineNo =  - 1;
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

void RestoreChangeLn(struct xmlNode *node, int version, PROJECTITEM *wa)
{
    struct xmlNode *children = node->children;
    struct tagfile *l,  **ls = &tagFileList, *lprev = NULL;
    struct xmlAttr *attribs;
    while (*ls)
    {
        lprev = *ls;
        ls = &(*ls)->next;
    }
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            struct xmlNode *fchildren = children->children;
            struct tagchangeln *c = 0,  **x = &c;
            *ls = calloc(sizeof(struct tagfile), 1);
            if (!ls)
            {
                NoMemoryWS();
                return ;
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
            if (l !=  *ls)
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
                    if (! *x)
                    {
                        NoMemoryWS();
                        return ;
                    } attribs = fchildren->attribs;
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
void RestoreFileBrowse(struct xmlNode *node, int version, PROJECTITEM *wa)
{
    struct xmlNode *children = node->children;
    struct xmlAttr *attribs;
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
                DWINFO *info = GetFileInfo(buf);
                if (info)
                {
                    FILEBROWSE *newBrowse = calloc(1, sizeof(FILEBROWSE));
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

void RestoreTags(struct xmlNode *node, int version, PROJECTITEM *wa)
{
    struct tagfile *l,  **ls = &tagFileList, *lprev = NULL;
    struct tag **ts, *tprev = NULL;
    struct xmlAttr *attribs = node->attribs;
    struct xmlNode *children = node->children;
    int tagid =  - 1;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
            tagid = atoi(attribs->value);
        attribs = attribs->next;
    } 
    if (tagid ==  - 1 || tagid != TAG_BP && tagid != TAG_BOOKMARK)
        return ;
    while (*ls)
    {
        lprev = *ls;
        ls = &(*ls)->next;
    }
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            struct xmlNode *fchildren = children->children;
            *ls = calloc(sizeof(struct tagfile), 1);
            if (!ls)
            {
                NoMemoryWS();
                return ;
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
            if (l !=  *ls)
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
                    if (! *ts)
                    {
                        NoMemoryWS();
                        return ;
                    } 
                    (*ts)->prev = tprev;
                    tprev = *ts;
                    attribs = fchildren->attribs;
                    while (attribs)
                    {
                        if (IsAttrib(attribs, "LINENO"))
                            (*ts)->drawnLineno = (*ts)->editingLineno = (*ts)
                                ->debugLineno = atoi(attribs->value);
                        else if (IsAttrib(attribs, "CP"))
                            (*ts)->charpos = atoi(attribs->value);
                        else if (IsAttrib(attribs, "ENABLED"))
                            (*ts)->enabled = atoi(attribs->value);
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

void RestoreFind(struct xmlNode *node, int version)
{
    struct xmlAttr *attribs = node->attribs;
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
//-------------------------------------------------------------------------

void RestoreWatch(struct xmlNode *node, int version)
{
    struct xmlAttr *attribs = node->attribs;
    int id = 0;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
        {
            // ID MUST BE FIRST!!!!!
            id = atoi(attribs->value) - 1;
            if (id < 0 || id > 3)
                return ;
        } 
        else if (IsAttrib(attribs, "ENABLE"))
        {
            if (atoi(attribs->value))
                hbpEnables |= (1 << id);
        }
        else if (IsAttrib(attribs, "NAME"))
        {
            strncpy(hbpNames[id], attribs->value, 255);
            hbpNames[id][255] = 0;
        }
        else if (IsAttrib(attribs, "MODE"))
        {
            hbpModes[id] = atoi(attribs->value);
        }
        else if (IsAttrib(attribs, "SIZE"))
        {
            hbpSizes[id] = atoi(attribs->value);
        }
        attribs = attribs->next;
    }
}
void RestoreDataBreakpoints(struct xmlNode *node, int version)
{
    struct xmlAttr *attribs = node->attribs;
    int id = 0;
    DATABREAK *b = (DATABREAK *)calloc(sizeof(DATABREAK), 1) ,** search = &dataBpList;
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
        attribs = attribs->next;
    }
}
//-------------------------------------------------------------------------

void RestoreToolBars(struct xmlNode *node, int version)
{
    int id = 0;
    HWND hwnd;
    char *horiz = 0,  *vert = 0;
    struct xmlAttr *attribs = node->attribs;
    if (version != WSPVERS)
        return;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
            id = atoi(attribs->value);
        else if (IsAttrib(attribs, "HORIZ"))
            horiz = attribs->value;
        else if (IsAttrib(attribs, "VERT"))
            vert = attribs->value;
        attribs = attribs->next;
    } 
    if (id && horiz && vert)
    {
        switch (id)
        {
            case DID_NAVTOOL:
                hwnd = hwndToolNav;
                break;
            case DID_BOOKMARKTOOL:
                hwnd = hwndToolBookmark;
                break;
            case DID_EDITTOOL:
                hwnd = hwndToolEdit;
                break;
            case DID_BUILDTOOL:
                hwnd = hwndToolBuild;
                break;
            case DID_DEBUGTOOL:
                hwnd = hwndToolDebug;
                break;
            default:
                return ;
        }
        SetToolBarData(hwnd, horiz, vert);
    }
}

//-------------------------------------------------------------------------

void RestoreProjectNames(struct xmlNode *node, int version, PROJECTITEM *wa)
{
    struct xmlNode *children = node->children;
    while (children)
    {
        if (IsNode(children, "FILE"))
        {
            PROJECTITEM **ins = &wa->children;
            PROJECTITEM *p = calloc(1, sizeof(PROJECTITEM));
            struct xmlAttr * attribs = children->attribs ;
        char *q;
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
                attribs = attribs->next ;
            }
            q = strrchr(p->realName,'\\');
            if (q)
                q++;
            else
                q= p->realName;
            strcpy(p->displayName, q);
            while (*ins && stricmp((*ins)->displayName, p->displayName) < 0)
                ins = &(*ins)->next;
            p->parent = wa;
            p->type = PJ_PROJ;
            p->next = *ins;
            *ins = p;
        }
        children = children->next ;
    }
}
void RestoreProfileNames(struct xmlNode *node, int version, PROJECTITEM *wa)
{
    char *selected = NULL;
    struct xmlAttr *attribs = node->attribs;
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
        free(currentProfileName);
        currentProfileName = strdup(LookupProfileName(selected));
    }
    node = node->children;
    while (node)
    {
        if (IsNode(node, "NAME"))
        {
            struct xmlAttr *attribs = node->attribs;
            char *name = NULL;
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
}
//-------------------------------------------------------------------------

PROJECTITEM *RestoreWorkArea(char *name)
{
    PROJECTITEM *wa;
    int version;
    FILE *in;
    char activeName[MAX_PATH];
    struct xmlNode *root;
    struct xmlNode *nodes;
    activeName[0] = 0;
    in = fopen(name, "r");
    if (!in)
    {
        return NULL;
    }
    root = xmlReadFile(in);
    fclose(in);
    if (!root || !IsNode(root, "CC386WORKAREA"))
    {
        ExtendedMessageBox("Load Error", 0, "WorkArea is invalid file format");
        if (root)
            xmlFree(root);
        return 0;
    }
    wa = calloc(1,sizeof(PROJECTITEM));
    if (!wa)
    {
        xmlFree(root);
        ExtendedMessageBox("Load Error", 0, "Out of memory");
        return 0;
    }
    ClearProfileNames();
    wa->type = PJ_WS;
    strcpy(wa->realName, name);
    strcpy(wa->displayName, relpath(name, wa->realName));
    
    TagEraseAllEntries();
    CloseAll();
    hbpInit();
    databpInit();
    nodes = root->children;
    while (nodes)
    {
        if (IsNode(nodes, "VERSION"))
        {
            struct xmlAttr *attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    version = atoi(attribs->value);
                attribs = attribs->next;
            } 
        }
        else if (IsNode(nodes, "FLAGS"))
        {
            struct xmlAttr *attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "EXPANDED"))
                    wa->expanded = atoi(attribs->value);
                if (IsAttrib(attribs, "DEBUGVIEW"))
                    wa->dbgview = atoi(attribs->value);
                attribs = attribs->next;
            } 
        }
        else if (IsNode(nodes, "DIALOG"))
        {
            struct xmlAttr *attribs = nodes->attribs;
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
            RestoreProfileNames(nodes, version,wa);
        else if (IsNode(nodes, "HISTORY"))
            RestoreHistory(nodes, version);
        else if (IsNode(nodes, "EDITWINDOWS"))
            RestoreWindows(nodes, version, wa);
        else if (IsNode(nodes, "TAG"))
            RestoreTags(nodes, version, wa);
        else if (IsNode(nodes, "DOCKS"))
            RestoreDocks(nodes, version);
        else if (IsNode(nodes, "TOOLBAR"))
            RestoreToolBars(nodes, version);
        else if (IsNode(nodes, "FIND"))
            RestoreFind(nodes, version);
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
        else if (IsNode(nodes, "PROJECTS"))
        {
            struct xmlAttr *attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "SELECTED"))
                {
                    strcpy(activeName, attribs->value);
                }
                attribs = attribs->next;
            } 
            RestoreProjectNames(nodes, version, wa);
        }
        nodes = nodes->next;
    }
    xmlFree(root);
    if (activeName[0])
    {
        PROJECTITEM *project = wa->children;
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
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    FreeJumpSymbols();
    LoadJumpSymbols();
    return wa;
}

//-------------------------------------------------------------------------

void onehistsave(FILE *out, char **hist, char *name)
{
    int i;
    fprintf(out, "\t<HISTORY TYPE=\"%s\">\n", name);
    for (i = 0; i < MAX_COMBO_HISTORY; i++)
        if (hist[i])
            fprintf(out, "\t\t<HISTITEM VALUE=\"%s\"/>\n", xmlConvertString(hist[i],TRUE));
    fprintf(out, "\t</HISTORY>\n");
}

//-------------------------------------------------------------------------

void SaveHistory(FILE *out)
{
    onehistsave(out, findhist, "FIND");
    onehistsave(out, findbrowsehist, "BFINDPATH");
    onehistsave(out, replacehist, "REPLACE");
    onehistsave(out, replacebrowsehist, "BREPLACEPATH");
    onehistsave(out, watchhist, "WATCH");
    onehistsave(out, databphist, "DATABP");
    onehistsave(out, memhist, "MEMORY");
}

//-------------------------------------------------------------------------

void SaveChangeLn(FILE *out, PROJECTITEM *wa)
{
    struct tagfile *list = tagFileList;
    fprintf(out, "\t<CHANGELN>\n");
    while (list)
    {
        if (list->changedLines)
        {
            struct tagchangeln *data = list->changedLines;
            fprintf(out, "\t\t<FILE NAME=\"%s\">\n", relpath(list->name, wa->realName));
            while (data)
            {
                fprintf(out, "\t\t\t<LINE NUM=\"%d\" DELTA=\"%d\"/>\n", data
                    ->lineno, data->delta);
                data = data->next;
            } fprintf(out, "\t\t</FILE>\n");
        }
        list = list->next;
    }
    fprintf(out, "\t</CHANGELN>\n");
}
void SaveFileBrowse(FILE *out, PROJECTITEM *wa)
{
    FILEBROWSE *p = fileBrowseInfo;
    fprintf(out, "\t<FILEBROWSE>\n");
    while (p)
    {
        fprintf(out, "\t\t<FILE NAME=\"%s\" LINE=\"%d\"/>\n", relpath(p->info->dwName, wa->realName),
                p->lineno);
        p = p->next;
    }
    fprintf(out, "\t</FILEBROWSE>\n");
}
//-------------------------------------------------------------------------

void saveonetag(FILE *out, int tag, PROJECTITEM *wa)
{
    struct tagfile *list = tagFileList;
    fprintf(out, "\t<TAG ID=\"%d\">\n", tag);
    while (list)
    {
        if (list->tagArray[tag])
        {
            struct tag *data = list->tagArray[tag];
            fprintf(out, "\t\t<FILE NAME=\"%s\">\n", relpath(list->name, wa->realName));
            while (data)
            {
                fprintf(out, 
                    "\t\t\t<TAGITEM LINENO=\"%d\" CP=\"%d\" ENABLED=\"%d\"",
                    data->drawnLineno, data->charpos, data->enabled);

                if (tag == TAG_BOOKMARK)
                    fprintf(out, ">\n%s\n\t\t\t</TAGITEM>\n", xmlConvertString
                        (data->extra, FALSE));
                else
                    fprintf(out, "/>\n");
                data = data->next;
            } fprintf(out, "\t\t</FILE>\n");
        }
        list = list->next;
    }
    fprintf(out, "\t</TAG>\n");
}

//-------------------------------------------------------------------------

void SaveTags(FILE *out, PROJECTITEM *wa)
{
    saveonetag(out, TAG_BP, wa );
    saveonetag(out, TAG_BOOKMARK, wa);
}

//-------------------------------------------------------------------------

void SaveDocks(FILE *out)
{
    int i;
    CCW_params p[20];
    CCD_params d[20];
    int len;
    len = dmgrGetInfo(&p[0], &d[0]);
    fprintf(out, "\t<DOCKS>\n");
    for (i = 0; i < len; i++)
    {
        fprintf(out, 
            "\t\t<DOCK ID=\"%d\" VALUE=\"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\"/>\n", 
                p[i].id, 
                d[i].flags, d[i].flexparams, 
                d[i].rowindex, d[i].colindex, 
                d[i].hidden, d[i].hiddenwidth, 
                d[i].oldsize.left, d[i].oldsize.top, 
                d[i].oldsize.right, d[i].oldsize.bottom, 
                d[i].position.left, d[i].position.top, 
                d[i].position.right, d[i].position.bottom, 
                d[i].lastposition.left, d[i].lastposition.top, 
                d[i].lastposition.right, d[i].lastposition.bottom);
    }
    fprintf(out, "\t</DOCKS>\n");
}

//-------------------------------------------------------------------------
void SaveWindows(FILE *out, PROJECTITEM *wa)
{
    int count;
    int i;
    HWND wnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
    DWINFO *info = (DWINFO *)GetWindowLong(wnd, 0);
    fprintf(out, "\t<EDITWINDOWS>\n");
    for (i=0; i < 10000; i++)
        if (!SendMessage(hwndSrcTab, TABM_GETITEM, i, 0))
            break;
    count = i;
    for (i=count-1; i >= 0; i--)
    {
        HWND hwnd = (HWND)SendMessage(hwndSrcTab, TABM_GETITEM, i, 0);
        if (IsEditWindow(hwnd))
        {
            DWINFO *ptr = (DWINFO *)GetWindowLong(hwnd, 0);
            EDITDATA *dt = (EDITDATA *)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0);
            POINT p;
            int startpos;
            WINDOWPLACEMENT placement;
            CHARRANGE a;
            SendMessage(ptr->dwHandle, EM_EXGETSEL, 0, (WPARAM) &a);
            startpos = SendMessage(ptr->dwHandle, EM_EXLINEFROMCHAR, 0, a.cpMin);
            placement.length = sizeof(placement);
            GetWindowPlacement(ptr->self, &placement);
            p.x = placement.rcNormalPosition.left;
            p.y = placement.rcNormalPosition.top;
            fprintf(out, 
                "\t\t<WINDOW NAME=\"%s\" TITLE=\"%s\" X=\"%d\" Y=\"%d\" WIDTH=\"%d\" HEIGHT=\"%d\" LINENO=\"%d\" COPYID=\"%d\"/>\n", 
                relpath(ptr->dwName, wa->realName), ptr->dwTitle, p.x, p.y, 
                placement.rcNormalPosition.right - placement.rcNormalPosition.left, 
                placement.rcNormalPosition.bottom - placement.rcNormalPosition.top, 
                startpos + 1, dt->cd->siblings ? dt->id + 1 : 0);
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

void SaveToolBarA(FILE *out, HWND hwnd)
{
    char horiz[512], vert[512];
    int id = GetToolBarData(hwnd, horiz, vert);
    fprintf(out, "\t<TOOLBAR ID=\"%d\" HORIZ=\"%s\" VERT=\"%s\"/>\n", id, horiz,
        vert);
}

//-------------------------------------------------------------------------

void SaveToolBars(FILE *out)
{
    SaveToolBarA(out, hwndToolNav);
    SaveToolBarA(out, hwndToolEdit);
    SaveToolBarA(out, hwndToolBuild);
    SaveToolBarA(out, hwndToolDebug);
    SaveToolBarA(out, hwndToolBookmark);
}

//-------------------------------------------------------------------------

void SaveWatchpoints(FILE *out)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        fprintf(out, 
            "\t<WATCH ID=\"%d\" ENABLE=\"%d\" NAME=\"%s\" MODE=\"%d\" SIZE=\"%d\"/>\n", i + 1, !!(hbpEnables &(1 << i)), xmlConvertString(hbpNames[i], TRUE), hbpModes[i], hbpSizes[i]);
    }
}

void SaveDataBreakpoints(FILE *out)
{
    DATABREAK *search = dataBpList;
    while (search)
    {
        fprintf(out,"\t<DATABP NAME=\"%s\" ACTIVE=\"%d\"/>\n", search->name, search->active);
        search = search->next;
    }
}
void SaveProjectNames(FILE *out, PROJECTITEM *wa)
{
    PROJECTITEM *l = wa->children;
    fprintf(out,"\t<PROJECTS SELECTED=\"%s\">\n", 
            activeProject ? activeProject->displayName : "");
    while (l)
    {
        fprintf(out,
            "\t\t<FILE NAME=\"%s\" CLEAN=\"%x\"/>\n", 
            relpath(l->realName, wa->realName), l->clean);
        l = l->next ;
    }
    fprintf(out,"\t</PROJECTS>\n");
}
void SaveWorkArea(PROJECTITEM *wa)
{
    FILE *out;
    int i;
    PROFILENAMELIST *pf;
    
    if (PropGetBool(NULL, "BACKUP_PROJECTS"))
        backup(wa->realName);	
    out = fopen(wa->realName, "w");
    if (!out)
    {
        ExtendedMessageBox("Save Error", 0, "Could not save WorkArea");
        return ;
    }
    fprintf(out, "<CC386WORKAREA>\n");
    fprintf(out, "\t<VERSION ID=\"%d\"/>\n", WSPVERS);
    fprintf(out, "\t<FLAGS EXPANDED=\"%d\" DEBUGVIEW=\"%d\"/>\n", wa->expanded ? 1 : 0, wa->dbgview);
    fprintf(out, "\t<PROFILELIST SELECTED=\"%s\" TYPE=\"%s\">\n", currentProfileName,
            profileDebugMode ? "DEBUG" : "RELEASE");
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
    fprintf(out, "\n\t\tFF=\"%d\" RF=\"%d\" FM=\"%d\" RM=\"%d\" FE=\"%d\" RE=\"%d\"/>\n",
            findflags, replaceflags, findmode, replacemode, findext, replaceext);
    SaveWatchpoints(out);
    SaveDataBreakpoints(out);
    SaveToolBars(out);
    SaveDocks(out);
    SaveProjectNames(out, wa);
    SaveWindows(out, wa);
    SaveHistory(out);
    SaveTags(out, wa );
    SaveChangeLn(out, wa);
    SaveFileBrowse(out, wa);
    SaveProfiles(out, wa, 1);
    fprintf(out, "</CC386WORKAREA>\n");
    fclose(out);
    wa->changed = FALSE;
}

