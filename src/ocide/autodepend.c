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

extern HANDLE hInstance;
extern char szInstallPath[];
extern PROJECTITEM* workArea;

PROJECTITEM* internalDepends;

static char* filedResourceTypes[] = {"cursor", "font", "bitmap", "icon", "messagetable", "rcdata"};
static char* sourceTypes[] = {".c", ".cpp", ".cxx", ".cc", ".h", ".hpp", ".hxx", ".rc"};

static void DependsGetPath(PROJECTITEM* pj, PROJECTITEM* fi, char* path, int len)
{
    char buf[10000], *p, *q;
    PropGetString(fi, "__INCLUDE", path, len);
    p = path;
    buf[0] = 0;
    while (*p)
    {
        q = strchr(p, ';');
        if (!q)
            q = p + strlen(p);
        if (p != q)
        {
            char exp[MAX_PATH];
            strncpy(exp, p, q - p);
            exp[q - p] = '\0';
            abspath(exp, pj->realName);
            if (buf[0])
                strcat(buf, ";");
            strcat(buf, exp);
            p = q;
            if (*p)
                p++;
        }
        else
        {
            p++;
        }
    }
    strncpy(path, buf, len - 1);
    path[len - 1] = 0;
}
static void InsertDepend(PROJECTITEM* fi, PROJECTITEM* dep)
{
    PROJECTITEMLIST** li = &fi->depends;
    for (li = &fi->depends; (*li) && (*li)->item != dep; li = &(*li)->next)
        ;
    if (!*li)
    {
        *li = calloc(1, sizeof(PROJECTITEMLIST));
        if (*li)
        {
            (*li)->item = dep;
        }
    }
}
//-------------------------------------------------------------------------

static void EraseDepends(PROJECTITEM* fi)
{
    while (fi->depends)
    {
        PROJECTITEMLIST* list = fi->depends;
        fi->depends = fi->depends->next;
        free(list);
    }
}
static void FindDepends(PROJECTITEM* pj, PROJECTITEM* fi)
{
    BOOL rcFile = FALSE;
    FILE* in = fopen(fi->realName, "r");
    if (!in)
        return;
    if (strlen(fi->realName) > 3 && !stricmp(fi->realName + strlen(fi->realName) - 3, ".rc"))
    {
        rcFile = TRUE;
    }
    while (!feof(in))
    {
        char buf[4096], *p = buf;
        buf[0] = 0;
        fgets(buf, sizeof(buf), in);
        while (*p && (*p == ' ' || *p == '\t'))
            p++;
        if (*p == '#')
        {
            p++;
            while (*p && (*p == ' ' || *p == '\t'))
                p++;
            if (!strncmp(p, "include", 7))
            {
                p += 7;
                while (*p && (*p == ' ' || *p == '\t'))
                    p++;
                if (*p == '"' || *p == '<')
                {
                    BOOL created = FALSE;
                    int i = 0;
                    char path[4096];
                    FILE* fil;
                    path[0] = 0;
                    p++;
                    DependsGetPath(pj, fi, path, sizeof(path));
                    while (*p && *p != '"' && *p != '>')
                        buf[i++] = *p++;
                    buf[i] = 0;
                    if ((fil = FindOnPath(buf, path)))
                    {
                        PROJECTITEM* newItem;
                        fclose(fil);
                        if (!(newItem = HasFile(pj, buf)))
                        {
                            newItem = RetrieveInternalDepend(buf);
                            if (!newItem)
                            {
                                newItem = calloc(1, sizeof(PROJECTITEM));
                                if (newItem)
                                {
                                    // deliberately not filling in all fields...
                                    newItem->type = PJ_FILE;
                                    strcpy(newItem->realName, buf);
                                    newItem->internalDependsLink = internalDepends;
                                    internalDepends = newItem;
                                    created = TRUE;
                                }
                            }
                        }
                        InsertDepend(fi, newItem);
                        if (created)
                        {
                            FindDepends(pj, newItem);
                        }
                        else
                        {
                            PROJECTITEMLIST* pil = newItem->depends;
                            while (pil)
                            {
                                InsertDepend(fi, pil->item);
                                pil = pil->next;
                            }
                        }
                    }
                }
            }
        }
        else if (rcFile)
        {
            int i;
            char* q = NULL;
            for (i = 0; i < sizeof(filedResourceTypes) / sizeof(filedResourceTypes[0]); i++)
            {
                if ((q = stristr(p, filedResourceTypes[i])))
                {
                    if (q == p || isspace(q[-1]))
                    {
                        q += strlen(filedResourceTypes[i]);
                        if (isspace(*q))
                        {
                            p = q;
                            while (isspace(*p))
                                p++;

                            if (*p == '"' || *p == '<')
                            {
                                int i = 0;
                                char path[4096];
                                FILE* fil;
                                path[0] = 0;
                                p++;
                                DependsGetPath(pj, fi, path, sizeof(path));
                                while (*p && *p != '"' && *p != '>')
                                    buf[i++] = *p++;
                                buf[i] = 0;
                                if ((fil = FindOnPath(buf, path)))
                                {
                                    PROJECTITEM* newItem;
                                    fclose(fil);
                                    if (!(newItem = HasFile(pj, buf)))
                                    {
                                        newItem = RetrieveInternalDepend(buf);
                                        if (!newItem)
                                        {
                                            newItem = calloc(1, sizeof(PROJECTITEM));
                                            if (newItem)
                                            {
                                                // deliberately not filling in all fields...
                                                newItem->type = PJ_FILE;
                                                strcpy(newItem->realName, buf);
                                                newItem->internalDependsLink = internalDepends;
                                                internalDepends = newItem;
                                            }
                                        }
                                    }
                                    InsertDepend(fi, newItem);
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    fclose(in);
}

static BOOL IsSourceFile(char* name)
{
    int i;
    for (i = 0; i < sizeof(sourceTypes) / sizeof(sourceTypes[0]); i++)
        if (strlen(name) > strlen(sourceTypes[i]) && !stricmp(name + strlen(name) - strlen(sourceTypes[i]), sourceTypes[i]))
            return TRUE;
    return FALSE;
}
static int CountFiles(PROJECTITEM* pj)
{
    int rv = 0;
    pj = pj->children;
    while (pj)
    {
        pj->visited = FALSE;
        if (pj->type == PJ_FOLDER || pj->type == PJ_PROJ || pj->type == PJ_WS)
            rv += CountFiles(pj);
        else if (pj->type == PJ_FILE)
            if (IsSourceFile(pj->realName))
                rv++;
        pj = pj->next;
    }
    return rv;
}
static void CalculateFiles(PROJECTITEM* pj, PROJECTITEM* fi)
{
    fi = fi->children;
    while (fi)
    {
        if (fi->type == PJ_FOLDER || fi->type == PJ_PROJ || fi->type == PJ_WS)
            CalculateFiles(pj, fi);
        else if (fi->type == PJ_FILE)
            if (!fi->visited && IsSourceFile(fi->realName))
            {
                char buf[MAX_PATH + 100];
                sprintf(buf, "Scanning dependencies for %s...", fi->displayName);
                SetStatusMessage(buf, FALSE);
                EraseDepends(fi);
                FindDepends(pj, fi);
            }
        fi = fi->next;
    }
}
//-------------------------------------------------------------------------

void CalculateProjectDepends(PROJECTITEM* pj)
{
    int count = pj->type == PJ_FILE ? 1 : CountFiles(pj);
    if (count != 0)
    {
        SetBusy(1);
        switch (pj->type)
        {
            PROJECTITEM* item;
            case PJ_PROJ:
                CalculateFiles(pj, pj);
                break;
            case PJ_WS:
                pj = pj->children;
                while (pj)
                {
                    CalculateFiles(pj, pj);
                    pj = pj->next;
                }
                break;
            case PJ_FILE:
                item = pj;
                while (item && item->type != PJ_PROJ)
                    item = item->parent;
                EraseDepends(pj);
                FindDepends(item, pj);
                break;
            case PJ_FOLDER:
                item = pj;
                while (item && item->type != PJ_PROJ)
                    item = item->parent;
                CalculateFiles(item, pj);
                break;
            case PJ_RES:
            case PJ_RESMENU:
                break;
        }
        SetBusy(0);
    }
}
void CalculateFileAutoDepends(char* fileName)
{
    PROJECTITEM* item = HasFile(workArea, fileName);
    if (item)
        CalculateProjectDepends(item);
    SetStatusMessage("", FALSE);
}
PROJECTITEM* RetrieveInternalDepend(char* fileName)
{
    PROJECTITEM* internals = internalDepends;
    while (internals)
    {
        if (!stricmp(fileName, internals->realName))
        {
            return internals;
        }
        internals = internals->internalDependsLink;
    }
    return NULL;
}
void ResetInternalAutoDepends(void)
{
    PROJECTITEM* internals = internalDepends;
    internalDepends = NULL;
    while (internals)
    {
        PROJECTITEM* next = internals->internalDependsLink;
        free(internals);
        internals = next;
    }
}