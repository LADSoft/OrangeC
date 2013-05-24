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
#define PROJVERS 5
#define WSPVERS 5
#define PREFVERS 5
#define PREFFILE "cc386.prf"

#include "header.h"
#include "xml.h"

extern HINSTANCE hInstance;
extern char szWorkAreaTitle[], szWorkAreaName[];
extern char *sysProfileName;
extern char *currentProfileName;
extern PROJECTITEM *workArea;

static void RestoreOneProp(struct xmlNode *input, SETTING **s)
{
    struct xmlAttr *attribs = input->attribs;
    char *id = NULL;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
            id = attribs->value;
        attribs = attribs->next;
    } 
    if (id)
    {
        SETTING *setting = *s;
        while (setting)
        {
            if (!strcmp(setting->id, id))
                break;
            setting = setting->next;
        }
        if (!setting)
        {
            setting = calloc(1, sizeof(SETTING));
            setting->type = e_text;
            setting->id = strdup(id);
            setting->next = *s;
            *s = setting;
        }
        if (setting)
        {
            free(setting->value);
            setting->value = strdup(input->textData);
        }
    }
}
static void RestoreProfileProps(struct xmlNode *input, PROJECTITEM *root, PROJECTITEM *parent)
{
    struct xmlAttr *attribs = input->attribs;
    char *type = NULL;
    char *name = NULL;
    while (attribs)
    {
        if (IsAttrib(attribs, "NAME"))
            name = attribs->value;
        else if (IsAttrib(attribs, "TYPE"))
            type = attribs->value;
        attribs = attribs->next;
    }
    if (type && name)
    {
        char *pname = LookupProfileName(name);
        PROFILE *p = parent->profiles;
        while (p)
        {
            if (p->name == pname)
            {
                break;
            }
            p = p->next;
        }
        if (!p)
        {
            PROFILE **q = &parent->profiles;
            while (*q) q= &(*q)->next;
            *q = calloc(1, sizeof(PROFILE));
            (*q)->name = pname;
            p = *q;    
        }
        if (p)
        {
            SETTING **s = !stricmp(type,"DEBUG") ? &p->debugSettings : &p->releaseSettings;
            input = input->children;
            while (input)
            {
                if (IsNode(input, "PROP"))
                {
                    RestoreOneProp(input, s);
                }
                input = input->next;
            }
        }
    }
}
void RestoreProps(struct xmlNode *input, PROJECTITEM *root, PROJECTITEM *parent)
{
    while (input)
    {
        if (IsNode(input, "PROP"))
        {
            // oldformat
            struct xmlAttr *attribs = input->attribs;
            char *id = NULL;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    id = attribs->value;
                attribs = attribs->next;
            } 
            if (strcmp(id, "__DEBUG"))
            {
                PROFILE *p = parent->profiles;
                while (p)
                {
                    if (!stricmp(p->name, "WIN32"))
                    {
                        break;
                    }
                    p = p->next;
                }
                if (!p)
                {
                    PROFILE **q = &parent->profiles;
                    while (*q) q= &(*q)->next;
                    *q = calloc(1, sizeof(PROFILE));
                    (*q)->name = sysProfileName;
                    p = *q;    
                }
                if (p)
                {
                    RestoreOneProp(input, &p->debugSettings);
                    RestoreOneProp(input, &p->releaseSettings);
                }
            }
        }
        else if (IsNode(input, "PROFILE"))
        {
            RestoreProfileProps(input, root, parent);
        }
        input = input->next;
    }
}
static void RestorePropsNested(struct xmlNode *input, PROJECTITEM *root, PROJECTITEM *parent)
{
    while (input)
    {
        if (IsNode(input, "PROPERTIES"))
        {
            RestoreProps(input->children, root, parent);
        }
        input = input->next;
    }
}
static void RestoreFiles(struct xmlNode *input, PROJECTITEM *root, PROJECTITEM *parent)
{
    while (input)
    {
        if (IsNode(input, "FOLDER"))
        {
            struct xmlAttr *attribs = input->attribs;
            PROJECTITEM *folder = calloc(1, sizeof(PROJECTITEM));
            if (folder)
            {
                PROJECTITEM **ins = &parent->children;
                while (attribs)
                {
                    if (IsAttrib(attribs, "TITLE"))
                    {
                        strcpy(folder->displayName, attribs->value);
                    }
                    attribs = attribs->next;
                }
                while (*ins && (*ins)->type == PJ_FOLDER && stricmp((*ins)->displayName, folder->displayName) < 0)
                    ins = &(*ins)->next;
                folder->parent = parent;
                folder->type = PJ_FOLDER;
                folder->next = *ins;
                *ins = folder;
                RestoreFiles(input->children, root, folder);
            }
        }
        else if (IsNode(input, "FILE"))
        {
            struct xmlAttr *attribs = input->attribs;
            PROJECTITEM *file = calloc(1, sizeof(PROJECTITEM));
            if (file)
            {
                PROJECTITEM **ins = &parent->children;
                while (attribs)
                {
                    if (IsAttrib(attribs, "TITLE"))
                    {
                        strcpy(file->displayName, attribs->value);
                    }
                    else if (IsAttrib(attribs, "NAME"))
                    {
                        strcpy(file->realName, attribs->value);
                        abspath(file->realName, root->realName);
                    }
                    else if (IsAttrib(attribs, "CLEAN"))
                    {
                        file->clean = atoi(attribs->value);
                    }
                    attribs = attribs->next;
                }
                while (*ins && (*ins)->type == PJ_FOLDER)
                    ins = &(*ins)->next;
                while (*ins && stricmp((*ins)->displayName, file->displayName) < 0)
                    ins = &(*ins)->next;
                file->parent = parent;
                file->type = PJ_FILE;
                file->next = *ins;
                *ins = file;
                RestorePropsNested(input->children, root, file);
            }
        }
        input = input->next;
    }
}
//-------------------------------------------------------------------------

void RestoreProject(PROJECTITEM *project, BOOL loadWA)
{
    int projectVersion;
    struct xmlNode *root;
    struct xmlNode *children;
    struct xmlAttr *attribs;
    FILE *in;
    char name[MAX_PATH];
    sprintf(name, "%s.cpj", project->realName);
    in = fopen(name, "r");
    if (!in)
    {
        ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
            "Project File %s Not Found",project->displayName);
        strcat(project->displayName, " (unable to load)");
        return;
    }
    root = xmlReadFile(in);
    fclose(in);
    if (!root || !IsNode(root, "CC386PROJECT"))
    {
        LoadErr(root, project->displayName);
        strcat(project->displayName, " (unable to load)");
        return;
    }
    if (loadWA)
    {
        children = root->children;
        while (children)
        {
            if (IsNode(children, "WORKAREA"))
            {
                attribs = children->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "NAME"))
                    {
                        LoadWorkArea(attribs->value, TRUE);
                        return;
                    }
                    attribs = attribs->next;
                }
            }
            children = children->next;
        }
    }
    children = root->children;
    while (children)
    {
        if (IsNode(children, "VERSION"))
        {
            attribs = children->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    projectVersion = atoi(attribs->value);
                attribs = attribs->next;
            }
        }
        else if (IsNode(children, "TARGET"))
        {
            struct xmlNode *settings = children->children;
            attribs = children->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "TITLE"))
                {
                    strcpy(project->displayName, attribs->value);
                }
                attribs = attribs->next;
            }
            while (settings)
            {
                if (IsNode(settings, "FILES"))
                {
                    RestoreFiles(settings->children, project, project);
                }
                if (IsNode(settings, "PROPERTIES"))
                {
                    RestoreProps(settings->children, project, project);
                }
                settings = settings->next;
            }
        }
        children = children->next;
    }
    xmlFree(root);
    CalculateProjectDepends(project);
    project->loaded = TRUE;
}
BOOL HasProperties(PROJECTITEM *pj)
{
    BOOL has = FALSE;
    PROFILE *s = pj->profiles;
    while (s)
    {
        if (s->debugSettings || s->releaseSettings)
        {
            has = TRUE;
            break;
        }
        s = s->next;
    }
    return has;
}
void SaveProfiles(FILE *out, PROJECTITEM *pj, int indent)
{
    int i;
    PROFILE *s;
    for (i=0; i < indent; i++)
        fprintf(out, "\t");
    fprintf(out, "<PROPERTIES>\n");
    s = pj->profiles;
    while (s)
    {
        if (s->debugSettings || s->releaseSettings)
        {
            if (s->debugSettings)
            {
                for (i=0; i < indent+1; i++)
                    fprintf(out, "\t");
                fprintf(out, "<PROFILE NAME=\"%s\" TYPE=\"DEBUG\">\n", s->name);
                SaveProps(out, s->debugSettings, indent + 2);
                for (i=0; i < indent+1; i++)
                    fprintf(out, "\t");
                fprintf(out, "</PROFILE>\n");                    
            }
            if (s->releaseSettings)
            {
                for (i=0; i < indent+1; i++)
                    fprintf(out, "\t");
                fprintf(out, "<PROFILE NAME=\"%s\" TYPE=\"RELEASE\">\n", s->name);
                SaveProps(out, s->releaseSettings, indent + 2);
                for (i=0; i < indent+1; i++)
                    fprintf(out, "\t");
                fprintf(out, "</PROFILE>\n");                    
            }
        }
        s = s->next;
    }
    for (i=0; i < indent; i++)
        fprintf(out, "\t");
    fprintf(out, "</PROPERTIES>\n");
}

void SaveFiles(FILE *out, PROJECTITEM *proj, PROJECTITEM *children, int indent)
{
    while (children)
    {
        int i;
        for (i=0; i < indent; i++)
            fprintf(out, "\t");
        if (children->type == PJ_FOLDER)
        {
            fprintf(out, "<FOLDER TITLE=\"%s\">\n", children->displayName);
            SaveFiles(out, proj, children->children, indent + 1);
            for (i=0; i < indent; i++)
                fprintf(out, "\t");
            fprintf(out, "</FOLDER>\n");
        }
        else
        {
            fprintf(out, "<FILE NAME=\"%s\" TITLE=\"%s\" CLEAN=\"%d\"",relpath(children->realName, proj->realName), children->displayName, children->clean);
            if (HasProperties(children))
            {
                fprintf(out, ">\n");
                SaveProfiles(out, children, indent+1);
                for (i=0; i < indent; i++)
                    fprintf(out, "\t");
                fprintf(out, "</FILE>\n");
            }
            else
            {
                fprintf(out, "/>\n" );
            }
        }
        children = children->next;
    }
}
//-------------------------------------------------------------------------
void SaveProject(PROJECTITEM *project)
{
    FILE *out ;
    char name[MAX_PATH];
    sprintf(name, "%s.cpj", project->realName);
    if (PropGetBool(NULL, "BACKUP_PROJECTS"))
        backup(name);	
    out = fopen(name, "w");
    if (!out)
    {
        ExtendedMessageBox("Save Error", 0, "Could not save project %s", project->displayName);
        return;
    }
    fprintf(out, "<CC386PROJECT>\n");
    fprintf(out, "\t<VERSION ID=\"%d\"/>\n", PROJVERS);
    fprintf(out, "\t<WORKAREA NAME=\"%s\"/>\n", workArea->realName);
    fprintf(out, "\t<TARGET TITLE=\"%s\">\n", project->displayName);
    SaveProfiles(out, project, 2);    
    fprintf(out, "\t\t<FILES>\n");
    SaveFiles(out, project, project->children,3);
    fprintf(out, "\t\t</FILES>\n");
    fprintf(out, "\t</TARGET>\n");
    fprintf(out, "</CC386PROJECT>\n");
    fclose(out);
    project->changed = FALSE;
}
