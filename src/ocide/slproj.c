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
#define PROJVERS 5
#define WSPVERS 5
#define PREFVERS 5

#include "header.h"
#include "xml.h"

extern HINSTANCE hInstance;
extern char szWorkAreaTitle[], szWorkAreaName[];
extern char* sysProfileName;
extern PROJECTITEM* workArea;

static void RestoreOneProp(struct xmlNode* input, SETTING** s)
{
    struct xmlAttr* attribs = input->attribs;
    char* id = NULL;
    while (attribs)
    {
        if (IsAttrib(attribs, "ID"))
            id = attribs->value;
        attribs = attribs->next;
    }
    if (id)
    {
        SETTING* setting = *s;
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
static void RestoreProfileProps(struct xmlNode* input, PROJECTITEM* root, PROJECTITEM* parent)
{
    struct xmlAttr* attribs = input->attribs;
    char* type = NULL;
    char* name = NULL;
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
        char* pname = LookupProfileName(name);
        PROFILE* p = parent->profiles;
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
            PROFILE** q = &parent->profiles;
            while (*q)
                q = &(*q)->next;
            *q = calloc(1, sizeof(PROFILE));
            (*q)->name = pname;
            p = *q;
        }
        if (p)
        {
            SETTING** s = !stricmp(type, "DEBUG") ? &p->debugSettings : &p->releaseSettings;
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
void RestoreProps(struct xmlNode* input, PROJECTITEM* root, PROJECTITEM* parent)
{
    while (input)
    {
        if (IsNode(input, "PROP"))
        {
            // oldformat
            struct xmlAttr* attribs = input->attribs;
            char* id = NULL;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    id = attribs->value;
                attribs = attribs->next;
            }
            if (strcmp(id, "__DEBUG"))
            {
                PROFILE* p = parent->profiles;
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
                    PROFILE** q = &parent->profiles;
                    while (*q)
                        q = &(*q)->next;
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
static void RestorePropsNested(struct xmlNode* input, PROJECTITEM* root, PROJECTITEM* parent)
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
static void RestoreFiles(struct xmlNode* input, PROJECTITEM* root, PROJECTITEM* parent)
{
    while (input)
    {
        if (IsNode(input, "FOLDER"))
        {
            struct xmlAttr* attribs = input->attribs;
            PROJECTITEM* folder = calloc(1, sizeof(PROJECTITEM));
            if (folder)
            {
                PROJECTITEM** ins = &parent->children;
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
            struct xmlAttr* attribs = input->attribs;
            PROJECTITEM* file = calloc(1, sizeof(PROJECTITEM));
            if (file)
            {
                PROJECTITEM** ins = &parent->children;
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

void RestoreProject(PROJECTITEM* project, BOOL loadWA)
{
    int projectVersion;
    struct xmlNode* root;
    struct xmlNode* children;
    struct xmlAttr* attribs;
    FILE* in;
    char name[MAX_PATH];
    sprintf(name, "%s.cpj", project->realName);
    in = fopen(name, "r");
    if (!in)
    {
        ExtendedMessageBox("Load Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Project File %s Not Found", project->displayName);
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
            struct xmlNode* settings = children->children;
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
    SetStatusMessage("", FALSE);
    project->loaded = TRUE;
}
BOOL HasProperties(PROJECTITEM* pj)
{
    BOOL has = FALSE;
    PROFILE* s = pj->profiles;
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
void SaveProfiles(FILE* out, PROJECTITEM* pj, int indent)
{
    int i;
    PROFILE* s;
    for (i = 0; i < indent; i++)
        fprintf(out, "\t");
    fprintf(out, "<PROPERTIES>\n");
    s = pj->profiles;
    while (s)
    {
        if (s->debugSettings || s->releaseSettings)
        {
            if (s->debugSettings)
            {
                for (i = 0; i < indent + 1; i++)
                    fprintf(out, "\t");
                fprintf(out, "<PROFILE NAME=\"%s\" TYPE=\"DEBUG\">\n", s->name);
                SaveProps(out, s->debugSettings, indent + 2);
                for (i = 0; i < indent + 1; i++)
                    fprintf(out, "\t");
                fprintf(out, "</PROFILE>\n");
            }
            if (s->releaseSettings)
            {
                for (i = 0; i < indent + 1; i++)
                    fprintf(out, "\t");
                fprintf(out, "<PROFILE NAME=\"%s\" TYPE=\"RELEASE\">\n", s->name);
                SaveProps(out, s->releaseSettings, indent + 2);
                for (i = 0; i < indent + 1; i++)
                    fprintf(out, "\t");
                fprintf(out, "</PROFILE>\n");
            }
        }
        s = s->next;
    }
    for (i = 0; i < indent; i++)
        fprintf(out, "\t");
    fprintf(out, "</PROPERTIES>\n");
}

void SaveFiles(FILE* out, PROJECTITEM* proj, PROJECTITEM* children, int indent)
{
    while (children)
    {
        int i;
        for (i = 0; i < indent; i++)
            fprintf(out, "\t");
        if (children->type == PJ_FOLDER)
        {
            fprintf(out, "<FOLDER TITLE=\"%s\">\n", children->displayName);
            SaveFiles(out, proj, children->children, indent + 1);
            for (i = 0; i < indent; i++)
                fprintf(out, "\t");
            fprintf(out, "</FOLDER>\n");
        }
        else
        {
            fprintf(out, "<FILE NAME=\"%s\" TITLE=\"%s\" CLEAN=\"%d\"", relpath(children->realName, proj->realName),
                    children->displayName, children->clean);
            if (HasProperties(children))
            {
                fprintf(out, ">\n");
                SaveProfiles(out, children, indent + 1);
                for (i = 0; i < indent; i++)
                    fprintf(out, "\t");
                fprintf(out, "</FILE>\n");
            }
            else
            {
                fprintf(out, "/>\n");
            }
        }
        children = children->next;
    }
}
//-------------------------------------------------------------------------
void SaveProject(PROJECTITEM* project)
{
    FILE* out;
    char name[MAX_PATH];
    sprintf(name, "%s.cpj", project->realName);
    if (PropGetBool(NULL, "BACKUP_PROJECTS"))
        backupFile(name);
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
    SaveFiles(out, project, project->children, 3);
    fprintf(out, "\t\t</FILES>\n");
    fprintf(out, "\t</TARGET>\n");
    fprintf(out, "</CC386PROJECT>\n");
    fclose(out);
    project->changed = FALSE;
}
