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
#include <ctype.h>
#include <process.h>
#include "header.h"

extern char szInstallPath[];
extern HWND hwndProject;
extern int making;
extern PROJECTITEM *activeProject;

static char szFileName[256];

char *relpathmake(char *name, char *path)
{
    if (szFileName[0])
    {
        static char buf[MAX_PATH];
        strcpy(buf, name);
        abspath(buf, path);
        return relpath(buf, szFileName);
    }
    else 
        return relpath(name, path);
}
static char *nodotslash(char *p)
{
    char *q = p, *r = p;
    while (*q)
    {
        if (*q == '.' && (q == p || q[-1] != '.') && q[1] == '\\')
            q += 2;
        else
            *r++ = *q++;
    }
    *r = 0;
    return p;
}
static char *Find(char *str, PROJECTITEM *pj, PROJECTITEM **found, int mode)
{
    char buf[10000];
    char *rv = Lookup(str, pj, found);
    char *p = rv;
    if (!p)
        return NULL;
    while (*p)
    {
        if (*p == '$' && p[1] == '(')
        {
            char *q = strchr(p, ')');
            if (q)
            {
                PROJECTITEM *found2 = *found;
                int l;
                char *r;
                memcpy(buf, p+2, q - p-2);
                buf[q-p-2] = 0;
                r = Find(buf, pj, &found2, 0);
                l = strlen(r) - (q - p - 2);
                memmove(q+l, q, strlen(q)+1);
                memcpy(p+2, r, strlen(r));
                *found = LCD(*found, found2);
                p = q + l + 1;
                
            }
            else
            {
                p++;
            }
        }
        else
        {
            p++;
        }
    }
    if (*found)
    {
        NameValuePair *mm;
        char buf2[MAX_PATH];
        char *p = strrchr((*found)->realName, '\\');
        if (p)
            strcpy(buf2, p+1);
        else
            strcpy(buf2, (*found)->realName);
        p = buf2;
        while (*p)
        {
            if (!isalnum(*p))
                *p = '_';
            p++;
        }
        *p++ = '_';
        strcpy(p, str);
        if (mode == 0)
        {
            NameValuePair *p = (*found)->mmacro;
            while (p)
            {
                if (!strcmp(p->name, buf2))
                    return p->name;
                p = p->next;
            }
        }
        mm = calloc(1, sizeof(NameValuePair));
        mm->name = strdup(buf2);
        mm->value = strdup(nodotslash(rv));
        switch(mode)
        {
            case 2:
                pj->mmcustomCmd = mm;
                break;
            case 1:
                pj->mmcmd = mm;
                break;
            case 0:
                mm->next = (*found)->mmacro;
                (*found)->mmacro = mm;
                return mm->name;
        }
    }    
    return NULL;
}
static void GatherCommandMacros(PROJECTITEM *pj)
{
    PROJECTITEM *found = pj;
    while (found && found->type != PJ_PROJ)
        found = found->parent;
    Find("__COMMAND_LINE", pj, &found, 1);
}
static void GatherCustomBuildMacros(PROJECTITEM *pj)
{
    PROJECTITEM *found = pj;
    while (found && found->type != PJ_PROJ)
        found = found->parent;
    Find("__CUSTOM_COMMANDS", pj, &found, 2);
}
static void GatherMacros(PROJECTITEM *pj, BOOL first)
{
    do
    {
        AddSymbolTable(pj, TRUE);
        if (pj->type == PJ_FILE)
        {
            if (pj->outputExt[0])
            {
            
                GatherCommandMacros(pj);
            }
        }
        else if (pj->children)
        {
            GatherMacros(pj->children, FALSE);
            if (pj->type == PJ_PROJ)
            {
                GatherCommandMacros(pj);
                GatherCustomBuildMacros(pj);
            }
        }
        RemoveSymbolTable();
        pj = pj->next;
    } while (pj && !first);
}
static void GenFileMacros(FILE *out, PROJECTITEM *pj)
{
    NameValuePair *p = pj->mmacro;
    while (p)
    {
        fprintf(out, "%s = %s\n", p->name, p->value);
        p = p->next;
    }
}
static int GenProjDepends(FILE *out, PROJECTITEM *pj, PROJECTITEM *fi, int pos)
{
    while (fi)
    {
        AddSymbolTable(fi, TRUE);
        switch (fi->type)
        {
            case PJ_FILE:
            {
                if (fi->outputExt[0])
                {
                    char *outFile = Lookup("OUTPUTFILE",fi, NULL);
                    char *rp;
                    if (pos > 60)
                    {
                        fprintf(out, " \\\n\t");
                        pos = 0;
                    }
                    rp = nodotslash(relpathmake(outFile, pj->realName));
                    pos += strlen(rp);
                    fprintf(out, "%s ", rp);
                    free(outFile);
                }
                break;
            }
            case PJ_FOLDER:
                pos = GenProjDepends(out,pj, fi->children, pos);
                break;
        }
        RemoveSymbolTable();
        fi = fi->next;
    }
    return pos;
}
static void GenFileDepends(FILE *out, PROJECTITEM *pj)
{
    switch (pj->type)
    {
        case PJ_FILE:
        {
            if (pj->outputExt[0])
            {
                int i;
                PROJECTITEM *proj = pj;
                char *outFile = Lookup("OUTPUTFILE",pj, NULL);
                char *deps = Lookup("__DEPENDENCIES",pj, NULL);
                PROJECTITEMLIST *depends = pj->depends;
                char buf[MAX_PATH];
                while (proj && proj->type != PJ_PROJ)
                    proj = proj->parent;
                fprintf(out, "%s: ", nodotslash(relpathmake(pj->realName, proj->realName)));
                for (i=0; depends; depends = depends->next)
                {
                    char *rp;
                    if (i > 60)
                    {
                        fprintf(out, " \\\n\t");
                        i = 0;
                    }
                    strcpy(buf, depends->item->realName);
                    rp = nodotslash(relpathmake(buf, proj->realName));
                    i += strlen(rp);
                    fprintf(out, "%s ", rp);
                }
                if (deps[0])
                {
                    char path[4096],*p;
                    char root[MAX_PATH];
                    if (proj)
                    {
                        strcpy(path, proj->realName);
                        p = strrchr(path, '\\');
                        if (p)
                            p[1] = 0;
                        strcpy(root, path);
                    }
                    LoadPath(root, path, "__INCLUDES");
                    LoadPath(root, path, "__RCINCLUDES");
                    LoadPath(root, path, "__ASMINCLUDES");
                    while (deps[0])
                    {
                        char buf[MAX_PATH], *rp;
                        deps = GetNextFile(buf, deps);
                        if (i > 60)
                        {
                            fprintf(out, " \\\n\t");
                            i = 0;
                        }
                        rp = nodotslash(relpathmake(buf, proj->realName));
                        i += strlen(rp);
                        fprintf(out, "%s ", rp);
                    }
                }
                fprintf(out, "\n\n%s: ", nodotslash(relpathmake(outFile, proj->realName)));
                fprintf(out, "%s\n", nodotslash(relpathmake(pj->realName, proj->realName)));
                free(outFile);
                free(deps);
            }
            break;
        }                        
        case PJ_PROJ:
        {
            char *outFile = Lookup("OUTPUTFILE",pj, NULL);
            char *rp;
            rp = nodotslash(relpathmake(outFile, pj->realName));
            fprintf(out, "%s: ", rp);
            GenProjDepends(out, pj, pj->children, 0);
            fprintf(out, "\n");
            free(outFile);
            break;
        }
    }
}
static void GenFileCommands(FILE *out, PROJECTITEM *pj)
{
    GenFileDepends(out, pj);
    fprintf(out, "\t%s\n", pj->mmcmd->value);
}
static void GenCustomBuildDepends(FILE *out, PROJECTITEM *pj)
{
    char *outFile = Lookup("OUTPUTFILE",pj, NULL);
    char *rp, *q;
    char *deps = Lookup("__CUSTOM_DEPENDENCIES",pj, NULL);
    rp = nodotslash(relpathmake(outFile,pj->realName));
    q = rp;
    while (*q)
    {
        if (*q == '\\' || *q == '.')
            *q = '_';
        q++;
    }
    fprintf(out, "CUSTOM_%s: ", rp);
    if (deps[0])
    {
        char path[4096],*p;
        char root[MAX_PATH];
        int i = 0;
        strcpy(path, pj->realName);
        p = strrchr(path, '\\');
        if (p)
            p[1] = 0;
        strcpy(root, path);
        LoadPath(root, path, "__INCLUDES");
        LoadPath(root, path, "__RCINCLUDES");
        LoadPath(root, path, "__ASMINCLUDES");
        while (deps[0])
        {
            char buf[MAX_PATH];
            deps = GetNextFile(buf, deps);
            if (i > 60)
            {
                fprintf(out, " \\\n\t");
                i = 0;
            }
            i += strlen(buf);
            fprintf(out, "%s ", buf);  
        }
    }
    outFile = Lookup("OUTPUTFILE",pj, NULL);
    fprintf(out,"%s\n", outFile);
    free(deps);
    free(outFile);
}
static void GenCustomBuildCommands(FILE *out, PROJECTITEM *pj)
{
    if (pj->mmcustomCmd->value[0])
    {
        char *p = pj->mmcustomCmd->value;
        GenCustomBuildDepends(out, pj);
        while (*p)
        {
            char *q = strchr(p,'\n');
            if (q)
            {
                *q = 0;
                fprintf(out, "\t%s\n", p);
                *q = '\n';
                p = q + 1;
            }
            else
            {
                fprintf(out, "\t%s\n", p);
                p += strlen(p);
            }
        }
    }
}
static void GenMakeFile(FILE *out, PROJECTITEM *pj, BOOL first)
{
    do
    {
        AddSymbolTable(pj, TRUE);
        GenFileMacros(out, pj);
        if (pj->type == PJ_FILE)
        {
            if (pj->outputExt[0])
            {
                GenFileCommands(out, pj);
            }
        }
        else if (pj->children)
        {
            GenMakeFile(out, pj->children, FALSE);
            if (pj->type == PJ_PROJ)
            {
                GenFileCommands(out, pj);
                GenCustomBuildCommands(out, pj);
            }
        }
        RemoveSymbolTable();
        pj = pj->next;
    } while (pj && !first);
}
static void GenAllRule(FILE *out, PROJECTITEM *pj, BOOL first)
{
    if (first)
        fprintf(out, "all: ");
    do
    {
        if (pj->type == PJ_WS)
        {
            GenAllRule(out, pj->children, FALSE);
        }
        else if (pj->type == PJ_PROJ)
        {
            char *rp;
            char *outFile;
            AddSymbolTable(pj, TRUE);
            outFile = Lookup("OUTPUTFILE",pj, NULL);
            rp = nodotslash(relpathmake(outFile,pj->realName));
            if (pj->mmcustomCmd->value[0])
            {
                char *q = rp;
                while (*q)
                {
                    if (*q == '\\' || *q == '.')
                        *q = '_';
                    q++;
                }
                fprintf(out, "\\\n\tCUSTOM_%s", rp);
            }
            else
            {
                fprintf(out, "\\\n\t%s", rp);
            }
            RemoveSymbolTable();
            free(outFile);
        }
        pj = pj->next;
    } while (pj && !first);
}
static void FreeNVPs(PROJECTITEM *pj)
{
    if (pj->mmcmd)
    {
        free(pj->mmcmd->name);
        free(pj->mmcmd->value);
        free(pj->mmcmd);
        pj->mmcmd = NULL;
    }
    if (pj->mmcustomCmd)
    {
        free(pj->mmcustomCmd->name);
        free(pj->mmcustomCmd->value);
        free(pj->mmcustomCmd);
        pj->mmcustomCmd = NULL;
    }
    while (pj->mmacro)
    {
        NameValuePair *p = pj->mmacro->next;
        free(pj->mmacro->name);
        free(pj->mmacro->value);
        free(pj->mmacro);        
        pj->mmacro = p;
    }
    if (pj->children)
    {
        PROJECTITEM *p = pj->children;
        while (p)
        {
            FreeNVPs(p);
            p = p->next;
        }
    }
}
static DWORD genMake(PROJECTITEM *l)
{
    FILE *out;
    OPENFILENAME ofn;
    PROJECTITEM *pj = activeProject;
    activeProject = NULL;
    if (!SaveFileDialog(&ofn, "makefile", hwndProject, FALSE, 0,
        "Save Makefile As"))
        
    {
        activeProject = pj;
        return 1;
    }
    activeProject = pj;
    strcpy(szFileName, ofn.lpstrFile);
    out = fopen(ofn.lpstrFile, "w");
    if (!out)
    {
        ExtendedMessageBox("Makefile Generation", MB_SETFOREGROUND |
            MB_SYSTEMMODAL, "Could not open %s for write", ofn.lpstrFile);
        return 1;
    }
    making = 1;
    AddRootTables(l, TRUE);
    SetOutputExtensions(l, TRUE);
    SetOutputNames(l, TRUE);
    GatherMacros(l, TRUE);
    GenAllRule(out,l, TRUE);
    fprintf(out, "\n\n");
    GenMakeFile(out, l, TRUE);
    fclose(out);
    FreeNVPs(l);
    szFileName[0] = 0;
    ExtendedMessageBox("Makefile Generation", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
        "Makefile generated");
    making = 0;
    return 0;
}

//-------------------------------------------------------------------------

void genMakeFile(PROJECTITEM *l)
{
    DWORD threadhand;
    _beginthread((BEGINTHREAD_FUNC)genMake, 0, (LPVOID)l);
}
