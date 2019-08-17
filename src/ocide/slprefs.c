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
#define PREFVERS 5
#define PREFFILE "ui.prf"

#include "header.h"
#include "xml.h"

extern PROJECTITEM generalProject;

extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern COLORREF custColors[16];

//-------------------------------------------------------------------------

char* nocr(char* s)
{
    static char buf[1024];
    char* p = buf;
    while (*s)
    {
        if (*s != '\r')
            *p++ = *s;
        s++;
    }
    *p = 0;
    return buf;
}
//-------------------------------------------------------------------------

int IsNode(struct xmlNode* node, char* name) { return (!strcmp(node->elementType, name)); }
int IsAttrib(struct xmlAttr* attr, char* name) { return (!strcmp(attr->name, name)); }
void addcr(char* buf)
{
    char buf2[2048], *p = buf2;
    strcpy(buf2, buf);
    while (*p)
    {
        if (*p == '\n')
            *p++ = '\r';
        *buf++ = *p++;
    }
    *buf = 0;
}

//-------------------------------------------------------------------------

void RestorePlacement(struct xmlNode* node, int version)
{
    struct xmlAttr* attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "VALUE"))
        {
            WINDOWPLACEMENT wp;
            wp.length = sizeof(WINDOWPLACEMENT);
            sscanf(attribs->value, "%d %d %d %d %d %d %d %d %d %d", &wp.flags, &wp.showCmd, &wp.ptMinPosition.x,
                   &wp.ptMinPosition.y, &wp.ptMaxPosition.x, &wp.ptMaxPosition.y, &wp.rcNormalPosition.left,
                   &wp.rcNormalPosition.top, &wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom);
            wp.flags = 0;
            SetWindowPlacement(hwndFrame, &wp);
        }
        attribs = attribs->next;
    }
}

void RestoreCustomColors(struct xmlNode* node, int version)
{
    char* p = node->textData;
    int i;
    for (i = 0; i < 16; i++)
    {
        custColors[i] = strtoul(p, &p, 0);
    }
}
void RestoreGeneralProps(struct xmlNode* node, int version)
{
    node = node->children;
    while (node)
    {
        if (IsNode(node, "PROP"))
        {
            struct xmlAttr* attribs = node->attribs;
            char* id = NULL;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    id = attribs->value;
                attribs = attribs->next;
            }
            if (id)
            {
                SETTING* setting = PropFind(NULL, id);
                if (!setting)
                {
                    setting = calloc(1, sizeof(SETTING));
                    setting->type = e_text;
                    setting->id = strdup(id);
                    InsertSetting(&generalProject, setting);
                }
                if (setting)
                {
                    free(setting->value);
                    setting->value = strdup(node->textData);
                }
            }
        }
        node = node->next;
    }
}
int RestorePreferences(void)
{
    static int pass = 0;
    int version;
    FILE* in;
    struct xmlNode* root;
    struct xmlNode* nodes;
    char name[256];
    if (!generalProject.profiles)
        generalProject.profiles = calloc(1, sizeof(PROFILE));
    GetUserDataPath(name);
    strcat(name, PREFFILE);
    in = fopen(name, "r");
    if (!in)
    {
        LoadDefaultRules();
        return 0;
    }
    root = xmlReadFile(in);
    fclose(in);
    if (!root || !IsNode(root, "UIPREFS"))
    {
        LoadDefaultRules();
        return 0;
    }
    nodes = root->children;
    while (nodes)
    {
        if (IsNode(nodes, "VERSION"))
        {
            struct xmlAttr* attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    version = atoi(attribs->value);
                attribs = attribs->next;
            }
        }
        else if (!pass && IsNode(nodes, "PROPERTIES"))
            RestoreGeneralProps(nodes, version);
        else if (!pass && IsNode(nodes, "PLACEMENT"))
            RestorePlacement(nodes, version);
        else if (!pass && IsNode(nodes, "CUSTOMCOLORS"))
            RestoreCustomColors(nodes, version);
        else if (!pass && IsNode(nodes, "RULES"))
            RestoreBuildRules(nodes, version);
        nodes = nodes->next;
    }
    pass++;
    xmlFree(root);
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    return 1;
}
void SaveProps(FILE* out, SETTING* settings, int indent)
{
    while (settings)
    {

        switch (settings->type)
        {
            int i;
            case e_tree:
                SaveProps(out, settings->children, indent);
                break;
            case e_separator:
                break;
            default:
                for (i = 0; i < indent; i++)
                    fputc('\t', out);
                fprintf(out, "<PROP ID=\"%s\">%s</PROP>\n", settings->id, settings->value);
                break;
        }
        settings = settings->next;
    }
}
//-------------------------------------------------------------------------
void SavePreferences(void)
{
    int i;
    char name[256];
    WINDOWPLACEMENT wp;
    FILE* out;
    GetUserDataPath(name);
    strcat(name, PREFFILE);
    out = fopen(name, "w");
    if (!out)
        return;
    fprintf(out, "<UIPREFS>\n");
    fprintf(out, "\t<VERSION ID=\"%d\"/>\n", PREFVERS);
    wp.length = sizeof(wp);
    GetWindowPlacement(hwndFrame, &wp);
    fprintf(out, "\t<PLACEMENT VALUE=\"%d %d %d %d %d %d %d %d %d %d\"/>\n", wp.flags, wp.showCmd, wp.ptMinPosition.x,
            wp.ptMinPosition.y, wp.ptMaxPosition.x, wp.ptMaxPosition.y, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
            wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
    fprintf(out, "\t<CUSTOMCOLORS>\n\t\t");
    for (i = 0; i < 16; i++)
    {
        fprintf(out, "%d ", custColors[i]);
        if (i == 7)
            fprintf(out, "\n\t\t");
    }
    fprintf(out, "\n\t</CUSTOMCOLORS>\n");
    fprintf(out, "\t<PROPERTIES>\n");
    SaveProps(out, generalProject.profiles->debugSettings, 2);
    fprintf(out, "\t</PROPERTIES>\n");
    fprintf(out, "\t<RULES>\n");
    SaveBuildRules(out);
    fprintf(out, "\t</RULES>\n");
    fprintf(out, "</UIPREFS>\n");
    fclose(out);
}
