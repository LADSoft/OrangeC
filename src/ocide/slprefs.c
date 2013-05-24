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
#define PREFVERS 5
#define PREFFILE "ui.prf"

#include "header.h"
#include "xml.h"

extern PROJECTITEM generalProject;

extern int findflags[F_M_MAX];
extern int replaceflags[F_M_MAX];
extern int findmode;
extern int replacemode;
extern int fiffindmode;
extern int fifreplacemode;

extern int memoryWordSize;

extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern int custColors[16];

//-------------------------------------------------------------------------

char *nocr(char *s)
{
    static char buf[1024];
    char *p = buf;
    while (*s)
    {
        if (*s != '\r')
            *p++ =  *s;
        s++;
    }
    *p = 0;
    return buf;
}
//-------------------------------------------------------------------------

int IsNode(struct xmlNode *node, char *name)
{
    return (!strcmp(node->elementType, name));
} 
int IsAttrib(struct xmlAttr *attr, char *name)
{
    return (!strcmp(attr->name, name));
} 
void addcr(char *buf)
{
    char buf2[2048],  *p = buf2;
    strcpy(buf2, buf);
    while (*p)
    {
        if (*p == '\n')
            *p++ = '\r';
        *buf++ =  *p++;
    }
    *buf = 0;
}

//-------------------------------------------------------------------------

void RestorePlacement(struct xmlNode *node, int version)
{
    struct xmlAttr *attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "VALUE"))
        {
            WINDOWPLACEMENT wp;
            wp.length = sizeof(WINDOWPLACEMENT);
            sscanf(attribs->value, "%d %d %d %d %d %d %d %d %d %d",  &wp.flags,
                &wp.showCmd,  &wp.ptMinPosition.x, &wp.ptMinPosition.y, 
                &wp.ptMaxPosition.x, &wp.ptMaxPosition.y, 
                &wp.rcNormalPosition.left, &wp.rcNormalPosition.top, 
                &wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom);
            wp.flags = 0;
            SetWindowPlacement(hwndFrame, &wp);
        } 
        attribs = attribs->next;
    }
}




void RestoreMemoryWindowSettings(struct xmlNode *node, int version)
{
    struct xmlAttr *attribs = node->attribs;
    while (attribs)
    {
        if (IsAttrib(attribs, "WORDSIZE"))
            memoryWordSize = atoi(attribs->value);
        attribs = attribs->next;
    } 
}
//-------------------------------------------------------------------------

void RestoreFindflags(struct xmlNode *node, int version)
{
    struct xmlAttr *attribs = node->attribs;
    struct xmlNode *children;
    while (attribs)
    {
        if (IsAttrib(attribs, "FMODE"))
            findmode = atoi(attribs->value);
        if (IsAttrib(attribs, "RMODE"))
            replacemode = atoi(attribs->value);
        if (IsAttrib(attribs, "FIFFINDMODE"))
            fiffindmode = atoi(attribs->value);
        if (IsAttrib(attribs, "FIFREPLACEMODE"))
            fifreplacemode = atoi(attribs->value);
        attribs = attribs->next;
    } 
    children = node->children;
    while (children)
    {
        int index = -1;
        int fflags = -1;
        int rflags = -1;
        attribs = children->attribs;
        while (attribs)
        {
            if (IsAttrib(attribs, "INDEX"))
                index = atoi(attribs->value);
            else if (IsAttrib(attribs, "FIND"))
                index = atoi(attribs->value);
            else if (IsAttrib(attribs, "REPLACE"))
                index = atoi(attribs->value);
            attribs = attribs->next;
        }
        if (index != -1 && fflags != -1 && rflags != -1)
        {
            findflags[index] = fflags;
            replaceflags[index] = rflags;
        }
        children = children->next;
    }
}
void RestoreCustomColors(struct xmlNode *node, int version)
{
    char *p = node->textData;
    int i;
    for (i=0; i < 16; i++)
    {
        custColors[i] = strtoul(p, &p, 0);
    }
}
void RestoreGeneralProps(struct xmlNode *node, int version)
{
    node = node->children;
    while (node)
    {
        if (IsNode(node, "PROP"))
        {
            struct xmlAttr *attribs = node->attribs;
            char *id = NULL;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    id = attribs->value;
                attribs = attribs->next;
            } 
            if (id)
            {
                SETTING *setting = PropFind(NULL, id);
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
    int version;
    FILE *in;
    struct xmlNode *root;
    struct xmlNode *nodes,  *children;
    struct xmlAttr *attribs;
    char buf[256],  *p; 
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
            struct xmlAttr *attribs = nodes->attribs;
            while (attribs)
            {
                if (IsAttrib(attribs, "ID"))
                    version = atoi(attribs->value);
                attribs = attribs->next;
            } 
        }
        else if (IsNode(nodes, "PROPERTIES"))
            RestoreGeneralProps(nodes, version);
        else if (IsNode(nodes, "MEMWND"))
            RestoreMemoryWindowSettings(nodes, version);
        else if (IsNode(nodes, "FIND"))
            RestoreFindflags(nodes, version);
        else if (IsNode(nodes, "PLACEMENT"))
            RestorePlacement(nodes, version);
        else if (IsNode(nodes, "CUSTOMCOLORS"))
            RestoreCustomColors(nodes, version);
        else if (IsNode(nodes, "RULES"))
            RestoreBuildRules(nodes, version);
        nodes = nodes->next;
    }
    xmlFree(root);
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    return 1;
}
void SaveProps(FILE *out, SETTING *settings, int indent)
{
    while (settings)
    {
        
        switch(settings->type)
        {
            int i;
            case e_tree:
                SaveProps(out, settings->children, indent);
                break;
            case e_separator:
                break;
            default:
                for (i=0; i < indent; i++)
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
    FILE *out;
    GetUserDataPath(name);
    strcat(name, PREFFILE);
    out = fopen(name, "w");
    if (!out)
        return;
    fprintf(out, "<UIPREFS>\n");
    fprintf(out, "\t<VERSION ID=\"%d\"/>\n", PREFVERS);
    wp.length = sizeof(wp);
    GetWindowPlacement(hwndFrame, &wp);
    fprintf(out, "\t<PLACEMENT VALUE=\"%d %d %d %d %d %d %d %d %d %d\"/>\n",
        wp.flags, wp.showCmd, wp.ptMinPosition.x, wp.ptMinPosition.y,
        wp.ptMaxPosition.x, wp.ptMaxPosition.y, wp.rcNormalPosition.left,
        wp.rcNormalPosition.top, wp.rcNormalPosition.right,
        wp.rcNormalPosition.bottom);
    fprintf(out, "\t<CUSTOMCOLORS>\n\t\t");
    for (i = 0; i < 16; i++)
    {
        fprintf(out, "%d ", custColors[i]);
        if (i == 7)
            fprintf(out, "\n\t\t");
    }
    fprintf(out, "\n\t</CUSTOMCOLORS>\n");
    fprintf(out, "\t<MEMWND WORDSIZE=\"%d\"/>\n", memoryWordSize);
    fprintf(out, "\t<FIND FMODE=\"%d\" RMODE=\"%d\" FIFFINDMODE=\"%d\" FIFREPLACEMODE=\"%d\">\n", findmode, replacemode, fiffindmode, fifreplacemode);
    for (i=0; i < F_M_MAX; i++)
    {
        fprintf(out, "\t\t<MODE INDEX=\"%d\" FIND=\"%d\" REPLACE=\"%d\"/>\n", i, findflags[i], replaceflags[i]);
    }			
    fprintf(out, "\t</FIND>\n");
    fprintf(out, "\t<PROPERTIES>\n");
    SaveProps(out, generalProject.profiles->debugSettings, 2);
    fprintf(out, "\t</PROPERTIES>\n");
    fprintf(out, "\t<RULES>\n");
    SaveBuildRules(out);
    fprintf(out, "\t</RULES>\n");
    fprintf(out, "</UIPREFS>\n");
    fclose(out);
}

