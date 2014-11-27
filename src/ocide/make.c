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
#include "header.h"

extern int errcount, warncount;
extern HWND hwndFrame;
extern char makeTempFile[MAX_PATH];
extern char szInstallPath[];
extern BUILDRULE *buildRules;
extern PROJECTITEM *workArea;
extern PROJECTITEM *internalDepends;
extern PROJECTITEM *activeProject;
extern HashTable *tables;

int making;

static BOOL stopBuild;

HANDLE makeSem;

void InitMake(void)
{
    makeSem= CreateSemaphore(0,1,1,0);
}


void EvalMacros(PROJECTITEM *pj, char *parsedCmd, char *rawCmd, PROJECTITEM **lcd);

static BOOL MakeGetFile(PROJECTITEM *pj, char *buf)
{
    if (pj->type == PJ_PROJ)
    {
        char *p = strstr(buf, ".cpj");
        if (p) // get rid of cpj extension
            *p = 0;
    }
    else
    {
        char *value = Lookup("__TARGET_EXTENSION", pj, NULL);
        if (value)
        {
            char *p, *q;
            p = strrchr(value, '.');
            q = strrchr(buf, '.');
            if (p && q)
            {
                strcpy(q, p);
                return TRUE;
            }
            free(value);
        }
    }
    return FALSE;
}
void SetOutputNames(PROJECTITEM *pj, BOOL first)
{
    while (pj)
    {
        AddSymbolTable(pj, FALSE);
        if (pj->type == PJ_FILE)
        {
            char *p;
            p = Lookup("OUTPUTFILE", pj, NULL);
            if (p)
            {
                strcpy(pj->outputName, p);
        free(p);
            }
            if (first)
            {
                RemoveSymbolTable();
                break;
            }
        }
        else if (pj->type == PJ_PROJ)
        {
            char *p = strrchr(pj->realName, '.');
            if (p)
                strcpy(pj->outputExt, p);
        }
        if (pj->children)
        {
            SetOutputNames(pj->children, FALSE);
        }
        RemoveSymbolTable();
        pj = pj->next;
    }
}
void SetOutputExtensions(PROJECTITEM *pj, BOOL first)
{
    while (pj)
    {
        AddSymbolTable(pj, FALSE);
        if (pj->type == PJ_FILE)
        {
            struct _propsData data;
            SETTING *pages = calloc(sizeof(SETTING),1);
            memset(&data, 0, sizeof(data));
            data.prototype = pages;
            SelectRules(pj, &data);
            if (data.protocount)
            {
                SETTING *set = GetSettings(data.prototype[0]);
                if (set->command)
                {
                    SETTING *ext = PropFind(set->command->children, "__TARGET_EXTENSION");

                    if (ext)
                    {
                        char *q = strchr(ext->value, '.');
                        if (q)
                        {
                            strcpy(pj->outputExt, q);
                        }
                    }
                }
            }
            free(pages);
            if (first)
            {
                RemoveSymbolTable();
                break;
            }
        }
        else if (pj->type == PJ_PROJ)
        {
            char *p = strrchr(pj->realName, '.');
            if (p)
                strcpy(pj->outputExt, p);
        }
        if (pj->children)
        {
            SetOutputExtensions(pj->children, FALSE);
        }
        RemoveSymbolTable();
        pj = pj->next;
    }
}
static void GetFileTimes(PROJECTITEM *pj, BOOL clean, BOOL first)
{
    do        
    {
        AddSymbolTable(pj, FALSE);
        if (pj->type == PJ_FILE || pj->type == PJ_PROJ)
        {
            if (pj->outputExt[0])
            {
                char *p = Lookup("OUTPUTFILE", pj, NULL);
                if (p)
                {
                    if (clean || pj->clean)
                    {
                        unlink(p);
                        memset(&pj->outputTime, 0, sizeof(pj->outputTime));
                    }
                    else
                    {
                        FileTime(&pj->outputTime, p);
                    }
                    free(p);
                }
            }
            FileTime(&pj->fileTime, pj->realName);
        }
        if (pj->children)
        {
            GetFileTimes(pj->children, clean || pj->clean, FALSE);
        }
        pj->clean = FALSE;
        RemoveSymbolTable();
        pj = pj->next;
    }
    while (pj && !first);
}
static BOOL CharDepsChanged(char *wd, char *outputFile, char *dependencies)
{
    char path[4096],*p;
    char root[MAX_PATH];
    if (!dependencies)
        return FALSE;
    strcpy(path, wd);
    p = strrchr(path, '\\');
    if (p)
        p[1] = 0;
    strcpy(root, path);
    LoadPath(root, path, "__INCLUDES");
    LoadPath(root, path, "__RCINCLUDES");
    LoadPath(root, path, "__ASMINCLUDES");
    {
        char *current = dependencies;
        FILETIME outTime;
        FILE *fil;
        fil =FindOnPath(outputFile, path);
        if (fil)
        {
            fclose(fil);
            FileTime(&outTime, outputFile);
        }
        else
        {
            return TRUE;
        }        
        
        while (*current)
        {
            char depBuf[MAX_PATH];
            FILETIME depTime;
            current = GetNextFile(depBuf, current);
            fil = FindOnPath(depBuf, path);
            if (fil)
            {
                fclose(fil);
                FileTime(&depTime, depBuf);
                if (CompareTimes(&outTime, &depTime))
                    return TRUE;
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
static BOOL AnythingChanged(PROJECTITEM *pj, PROJECTITEM *fi)
{
    BOOL rv = FALSE;
    while (fi && !rv)
    {
        switch (fi->type)
        {
            case PJ_FILE:
                if (CompareTimes(&pj->outputTime, &fi->outputTime))
                    return TRUE;
                break;
            case PJ_FOLDER:
                rv |= AnythingChanged(pj, fi->children);
                break;
        }
        fi = fi->next;
    }
    return rv;
}
static BOOL DependsChanged(PROJECTITEM *pj)
{
    BOOL rv = FALSE;
    PROJECTITEMLIST *depends = pj->depends;
    PROJECTITEM *proj = pj;
    while (proj->type != PJ_PROJ) proj = proj->parent;
    while (depends)
    {
        if (CompareTimes(&pj->outputTime, &depends->item->fileTime))
            return TRUE;
        depends = depends->next;
    }
    if (pj->type == PJ_PROJ)
    {
        if (AnythingChanged(pj, pj->children))
            return TRUE;
    }
    else if (pj->type == PJ_FILE)
    {
        if (CompareTimes(&pj->outputTime, &pj->fileTime))
            return TRUE;
    }
    if (pj->outputExt[0])
    {
        char *out = Lookup("OUTPUTFILE",pj, NULL);
        char *dep = Lookup("__DEPENDENCIES",pj, NULL);
        rv = CharDepsChanged(proj->realName, out, dep);
        free(out);
        free(dep);
    }
    return rv;
}
static int GenCommand(PROJECTITEM *pj, BOOL always)
{
    BOOL rv = TRUE;
    if (always || pj->outputExt[0] && DependsChanged(pj))
    {
        char *cmd= Lookup("__COMMAND_LINE", pj, NULL);
        if (!cmd)
        {
            rv = 0;
        }
        else
        {
            char *banner = Lookup("__MAKEBANNER", pj, NULL);
            PROJECTITEM *project = pj;
            while (project && project->type != PJ_PROJ)
                project = project->parent;
            if (banner)
                MakeMessage(banner, pj->realName);
            free(banner);
            
            rv = !Execute(cmd, project->realName, ERR_BUILD_WINDOW);
            free(cmd);
            if (rv)
            {
                char *p = Lookup("OUTPUTFILE", pj, NULL);
                if (p)
                    FileTime(&pj->outputTime, p);
                return 1;
            }
            return 2;
        }
    }
    return 0;
}
static int CustomBuildStep(PROJECTITEM *pj)
{
    int rv = 1;
    char *commands = Lookup("__CUSTOM_COMMANDS", pj, NULL);
    if (commands && commands[0])
    {
        char buf[10000], expanded[10000], *p;
        char *output = Lookup("__CUSTOM_OUTPUT", pj, NULL);
        char *outputFile = Lookup("OUTPUTFILE", pj, NULL);
        if (output && output[0])
        {
            char *depends = Lookup("__CUSTOM_DEPENDS",pj, NULL);
            if (depends)
            {
                char *fil = Lookup("OUTPUTFILE", pj, NULL);
                char *newDeps = calloc(1, strlen(depends) + MAX_PATH);
                strcpy(newDeps, depends);
                strcat(newDeps, " ");
                strcat(newDeps, fil);
                free(fil);
                free(depends);
                if (!CharDepsChanged(pj->realName, outputFile, newDeps))
                    rv = FALSE;
                free(newDeps);
            }
            else if (!CharDepsChanged(pj->realName, output, outputFile))
                rv = FALSE;
        }
        free(output);
        free(outputFile);
        if (rv)
        {
            char *msg = Lookup("__CUSTOM_DISPLAY",pj, NULL);
            rv = 0;
            MakeMessage(msg, "");
            free(msg);
            while (*commands)
            {
                char *q ;
                strcpy(buf, "CMD /C ");
                q = buf + strlen(buf);
                p = strrchr(commands, '\n');
                if (!p)
                    p = commands + strlen(commands);
                memcpy(q, commands, p-commands);
                q[p-commands] = 0;
                commands = p;
                if (*commands) commands++;
                EvalMacros(NULL, expanded, buf, NULL);
                if (Execute(expanded, pj->realName, ERR_BUILD_WINDOW))
                    rv |= 1;
                else
                    rv |= 2;
            }
            if (rv & 1)
                MakeMessage("Custom build step failed", "");
            else if (rv & 2)
                MakeMessage("Custom build step succeeded", "");
        }
    }
    return rv;
}
static int BrowseStep(PROJECTITEM *pj)
{
    if (PropGetBool(NULL, "BROWSE_INFORMATION"))
    {
        BUILDRULE *p = buildRules;
        while (p)
        {
            if (p->profiles->debugSettings->select && !strcmp(p->profiles->debugSettings->select, "BROWSE"))
                break;
            p = p->next;
        }
        if (p && p->profiles->debugSettings->command)
        {
            AddAssigns(p->profiles->debugSettings->command->assignments);
            RecursiveAddSymbols(p->profiles->debugSettings->command->children, TRUE);
            if ( GenCommand(pj, TRUE))
            {
                FreeJumpSymbols();
                LoadJumpSymbols();
                return 1;
            }
            return 2;
        }
    }
    return 0;
}
static int Make(PROJECTITEM *pj, BOOL first)
{
    int rv = 0;
    do
    {
        AddSymbolTable(pj, FALSE);
        if (pj->type == PJ_FILE)
        {
            
            rv |= GenCommand(pj, FALSE);
            if (first)
            {
                RemoveSymbolTable();
                break;
            }
        }
        else if (pj->children)
        {
              
            if (pj->type == PJ_PROJ)
                MakeMessage("Building Project %s", pj->displayName);
            rv |= Make(pj->children, FALSE);
            if (!stopBuild)
            {
                if (pj->type == PJ_PROJ)
                {
                    if (!(rv & 2))
                    {
                        rv = GenCommand(pj, FALSE);
                        if (rv & 1)
                        {
                            BrowseStep(pj);
                            rv |= CustomBuildStep(pj);
                        }
                    }
                }
            }
        }
        RemoveSymbolTable();
        pj = pj->next;
    } while (pj && !stopBuild && !first);
    return rv;
}
static BOOL CheckChanged(PROJECTITEM *pj)
{
    BOOL rv = FALSE;
    do
    {
        AddSymbolTable(pj, FALSE);
        if (pj->type == PJ_FILE)
        {
            
            rv = pj->outputExt[0] && DependsChanged(pj);
        }
        else if (pj->children)
        {
                
            rv = CheckChanged(pj->children);
            if (!rv && pj->type == PJ_PROJ)
            {
                rv = pj->outputExt[0] && DependsChanged(pj);
            }
        }
        RemoveSymbolTable();
        pj = pj->next;
    } while (pj && !rv);
    return rv;
}
static BOOL BuildModified(PROJECTITEM *pj)
{
    BOOL rv;
    PROJECTITEM *deps = (PROJECTITEM *)internalDepends;
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    WaitForSingleObject(makeSem, INFINITE);
    AddRootTables(pj, FALSE);
    SetOutputExtensions(pj, TRUE);
    SetOutputNames(pj, TRUE);
    GetFileTimes(pj, pj->clean, TRUE);
    while (deps)
    {
        FileTime(&deps->fileTime, deps->realName);
        deps = deps->internalDependsLink;
    }
    rv = CheckChanged(pj);
    ReleaseSymbolTables();
    ReleaseSemaphore(makeSem, 1, NULL);
    return rv;
}
static DWORD MakerThread(void *p)
{
    PROJECTITEM *pj = (PROJECTITEM *)p;
    PROJECTITEM *deps = (PROJECTITEM *)internalDepends;
    making = TRUE;
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    WaitForSingleObject(makeSem, INFINITE);
    CreateTempFileName();
    AddRootTables(pj, FALSE);
    SetOutputExtensions(pj, TRUE);
    SetOutputNames(pj, TRUE);
    GetFileTimes(pj, pj->clean, TRUE);
    while (deps)
    {
        FileTime(&deps->fileTime, deps->realName);
        deps = deps->internalDependsLink;
    }
    ResetErrorCounts();
    // root table altready added
    SelectInfoWindow(ERR_BUILD_WINDOW);
    Make(pj, TRUE);
    ReleaseSymbolTables();
    if (stopBuild)
    {
        SetInfoColor(ERR_BUILD_WINDOW, 0x0000ff); // red
        SendInfoMessage(ERR_BUILD_WINDOW, "The build was canceled");
    }
    else
    {
        ErrWarnCounts();
    }
    making = FALSE;
    ReleaseSemaphore(makeSem, 1, NULL);
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    TagLinesAdjust(0, TAGM_UPDATEDEBUG);
    return errcount == 0;
}
void StopBuild(void)
{
    stopBuild = TRUE;
}
void Maker(PROJECTITEM *pj, BOOL clean)
{
    DWORD threadhand;
    stopBuild = FALSE;
    SaveDrawAll();
    ResSaveAll();
    SaveAllProjects(workArea, FALSE);
    pj->clean |= clean;
    _beginthread((BEGINTHREAD_FUNC)MakerThread, 0, (LPVOID)pj);
}
void dbgRebuildMainThread(int cmd)
{
    cmd &= 0xffff;
    if (!activeProject)
    {
        ExtendedMessageBox("Debugger", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
            "No project Selected");
    }
    else
    {
        SaveDrawAll();
        ResSaveAll();
        SaveAllProjects(workArea, FALSE);
        if (BuildModified(workArea))
        {
            if (ExtendedMessageBox("Debugger", MB_YESNO, 
                "Project needs to be rebuilt before debugging,  Do you want to rebuild?") == IDYES)
            {
                stopBuild = FALSE;
                if (!MakerThread(workArea))
                    return ;
            }
        }
        initiateDebug(!TagAnyBreakpoints() || cmd == IDM_STEPIN || cmd == IDM_STEPOUT || cmd == IDM_STEPOVER);
    }
}

//-------------------------------------------------------------------------

void dbgRebuildMain(int cmd)
{
    DWORD threadhand;
    _beginthread((BEGINTHREAD_FUNC)dbgRebuildMainThread, 0, (LPVOID)cmd);
}
