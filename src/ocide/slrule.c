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
#include "header.h"
#include "xml.h"

SETTINGCOMBO *LoadCombo(struct xmlNode *node, int version)
{
    SETTINGCOMBO *rv = NULL, **rvl = &(rv);
    while (node)
    {
        if (IsNode(node, "SETTING"))
        {
            *rvl = calloc(1, sizeof(SETTINGCOMBO));
            if (*rvl)
            {
                struct xmlAttr *attribs = node->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "NAME"))
                    {
                        (*rvl)->displayName = strdup(attribs->value);
                    }
                    else if (IsAttrib(attribs, "VALUE"))
                    {
                        (*rvl)->value = strdup(attribs->value);
                    }
                    attribs = attribs->next;
                }
                
                rvl = &(*rvl)->next;
            }
        }
        node = node->next;
    }
    return rv;
}
SETTING *LoadItem(struct xmlNode *node, int version, BOOL debug)
{
    struct xmlAttr *attribs = node->attribs;
    char *type = NULL;
    while (attribs)
    {
        if (IsAttrib(attribs, "MODE"))
        {
            type =  attribs->value;
            break;
        }
        attribs = attribs->next;
    }
    if (!type || (debug && !stricmp(type, "DEBUG")) || (!debug && !stricmp(type, "RELEASE")))
    {
        SETTING *rv = calloc(1, sizeof(SETTING));
        attribs = node->attribs;
        while (attribs)
        {
            if (IsAttrib(attribs, "NAME"))
                rv->displayName = strdup(attribs->value);
            else if (IsAttrib(attribs, "ID"))
                rv->id = strdup(attribs->value);
            else if (IsAttrib(attribs, "TYPE"))
            {
                if (!strcmp(attribs->value, "COMBO"))
                    rv->type = e_combo;
                else if (!strcmp(attribs->value, "TEXT"))
                    rv->type = e_text;
                else if (!strcmp(attribs->value, "NUMERIC"))
                    rv->type = e_numeric;
                else if (!strcmp(attribs->value, "COLOR"))
                    rv->type = e_color;
                else if (!strcmp(attribs->value, "FONT"))
                    rv->type = e_font;
                else if (!strcmp(attribs->value, "PRINTFORMAT"))
                    rv->type = e_printformat;
                else if (!strcmp(attribs->value, "SEPARATOR"))
                    rv->type = e_separator;
                else if (!strcmp(attribs->value, "MULTITEXT"))
                    rv->type = e_multitext;
                else if (!strncmp(attribs->value, "PREPENDTEXT", 11))
                {
                    rv->type = e_prependtext;
                    rv->aux = strdup(attribs->value + 12);
                }
                else if (!strncmp(attribs->value, "SEPARATEDTEXT", 13))
                {
                    rv->type = e_separatedtext;
                    rv->aux = strdup(attribs->value + 14);
                }
                switch (rv->type)
                {
                    case e_combo:
                        rv->combo = LoadCombo(node->children, version);
                        break;
                    case e_text:
                    case e_font:
                    case e_printformat:
                    case e_prependtext:
                    case e_separatedtext:
                    case e_multitext:
                        rv->deflt = strdup(node->textData);
                        free(rv->value);
                        if (node->textData)
                            rv->value = strdup(node->textData);
                        else
                            rv->value = strdup("");
                        break;
                    default:
                        break;
                }
            }
            else if (IsAttrib(attribs, "LOW"))
                rv->lowLimit = strdup(attribs->value);
            else if (IsAttrib(attribs, "HIGH"))
                rv->highLimit = strdup(attribs->value);
            else if (IsAttrib(attribs, "DEFAULT"))
            {
                rv->deflt = strdup(attribs->value);
                free(rv->value);
                rv->value = strdup(attribs->value);
            }
                
            attribs = attribs->next;
        } 
        return rv;
    }
    return NULL;
}
SETTING *LoadPropItems(struct xmlNode *node, int version, BOOL debug)
{
    SETTING *rv = NULL, **rvl = &rv;
    while (node)
    {
        if (IsNode(node, "PROPS"))
        {
            *rvl = calloc(1, sizeof(SETTING));
            if (*rvl)
            {
                struct xmlAttr *attribs = node->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "NAME"))
                        (*rvl)->displayName = strdup(attribs->value);
                    else if (IsAttrib(attribs, "HELPID"))
                        (*rvl)->helpid = atoi(attribs->value);
                    attribs = attribs->next;
                }
                    
                (*rvl)->type = e_tree;
                (*rvl)->children = LoadPropItems(node->children, version, debug);
                rvl = &(*rvl)->next;
            }
        }
        else if (IsNode(node, "ITEM"))
        {
            *rvl = LoadItem(node, version, debug);
            if (*rvl)
                rvl = &(*rvl)->next;
        }
        node = node->next;
    }
    return rv;
}
SETTING *LoadAssignmentItems(struct xmlNode *node, int version)
{
    SETTING *rv = NULL, **rvl = &rv;
    while (node)
    {
        if (IsNode(node, "ASSIGN"))
        {
            *rvl = calloc(1, sizeof(SETTING));
            if (*rvl)
            {
                struct xmlAttr *attribs = node->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "ID"))
                        (*rvl)->id = strdup(attribs->value);
                    attribs = attribs->next;
                }
                    
                (*rvl)->type = e_assign;
                (*rvl)->value = strdup(node->textData);
                rvl = &(*rvl)->next;
            }
        }
        else if (IsNode(node, "CHOOSE"))
        {
            *rvl = calloc(1, sizeof(SETTING));
            if (*rvl)
            {
                struct xmlAttr *attribs = node->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "ID"))
                    {
                        (*rvl)->id = strdup(attribs->value);
                    }
                    else if (IsAttrib(attribs, "SELECT"))
                    {
                        (*rvl)->value = strdup(attribs->value);
                    }
                    attribs = attribs->next;
                }
                (*rvl)->type = e_choose;
                (*rvl)->children = LoadAssignmentItems(node->children, version);
                rvl = &(*rvl)->next;
            }
        }
        node = node->next;
    }
    return rv;
}
SETTING *LoadCommandItems(struct xmlNode *node, int version)
{
    SETTING *rv = calloc(1, sizeof(SETTING));
    if (rv)
    {
        SETTING **rvl = &rv->children;
        while (node)
        {
            if (IsNode(node, "ASSIGNMENTS"))
            {
                rv->assignments = LoadAssignmentItems(node->children, version);
            }
            else if (IsNode(node, "ITEM"))
            {
                *rvl = LoadItem(node, version, FALSE);
                if (*rvl)
                    rvl = &(*rvl)->next;
            }
            node = node->next;
        }
    }
    return rv;
}
SETTING *LoadDependsItems(struct xmlNode *node, int version)
{
    SETTING *rv = NULL, **rvl = &rv;
    while (node)
    {
        if (IsNode(node, "CHOOSE"))
        {
            *rvl = calloc(1, sizeof(SETTING));
            if (*rvl)
            {
                struct xmlAttr *attribs = node->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "ID"))
                    {
                        (*rvl)->id = strdup(attribs->value);
                    }
                    else if (IsAttrib(attribs, "SELECT"))
                    {
                        (*rvl)->value = strdup(attribs->value);
                    }
                    attribs = attribs->next;
                }
                (*rvl)->type = e_choose;
                (*rvl)->children = LoadDependsItems(node->children, version);
                rvl = &(*rvl)->next;
            }
        }
        else if (IsNode(node, "LOAD"))
        {
            *rvl = calloc(1, sizeof(SETTING));
            (*rvl)->type = e_assign;
            if (*rvl)
            {
                struct xmlAttr *attribs = node->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "SELECT"))
                    {
                        (*rvl)->id = strdup(attribs->value);
                    }
                    attribs = attribs->next;
                }
                rvl = &(*rvl)->next;
            }
        }
        node = node->next;
    }
    return rv;
}
void FreeCombo(SETTINGCOMBO *c)
{
    while (c)
    {
        SETTINGCOMBO *next = c->next;
        free(c->displayName);
        free(c->value);
        free(c);
        c = next;
    }
}
void FreeRule(SETTING *rule)
{
    if (rule)
    {
        FreeRule(rule->next);
        FreeRule(rule->children);
        FreeRule(rule->command);
        FreeRule(rule->assignments);
        FreeRule(rule->depends);
        free(rule->displayName);
        free(rule->select);
        free(rule->cls);
        free(rule->id);
        FreeCombo(rule->combo);
        free(rule->lowLimit);
        free(rule->highLimit);
        free(rule->deflt);
        free(rule->value);
        free(rule);
    }
}
void LoadChildren(struct xmlNode *node, int version, SETTING *rvp, BOOL debug)
{	
    SETTING **rvx = &rvp->children;
    while (node)
    {
        if (IsNode(node, "PROPS"))
        {
            *rvx = calloc(1, sizeof(SETTING));
            if (*rvx)
            {
                struct xmlAttr *attribs = node->attribs;
                while (attribs)
                {
                    if (IsAttrib(attribs, "NAME"))
                        (*rvx)->displayName = strdup(attribs->value);
                    else if (IsAttrib(attribs, "HELPID"))
                        (*rvx)->helpid = atoi(attribs->value);
                    attribs = attribs->next;
                }
                    
                (*rvx)->type = e_tree;
                (*rvx)->children = LoadPropItems(node->children, version, debug);
                rvx = &(*rvx)->next;
            }
        }
        else if (IsNode(node, "COMMAND"))
            rvp->command = LoadCommandItems(node->children, version);
        else if (IsNode(node, "DEPENDS"))
            rvp->depends = LoadDependsItems(node->children, version);
        node = node->next;
    }
}
PROFILE *LoadRule(char *fileName)
{
    PROFILE *rv;
    SETTING **rvpd;
    SETTING **rvpr;
    int version=0;
    FILE *in;
    struct xmlNode *root;
    struct xmlNode *nodes;
    struct xmlAttr *attribs;
    in = fopen(fileName, "r");
    if (!in)
    {
        return 0;
    }
    root = xmlReadFile(in);
    fclose(in);
    if (!root || !IsNode(root, "OCRULE"))
    {
        return 0;
    }
    rv = calloc(1, sizeof(PROFILE));
    rvpd = & rv->debugSettings;
    rvpr = & rv->releaseSettings;
    attribs = root->attribs;
    
    nodes = root->children;
    while (nodes)
    {
        if (IsNode(nodes, "PROPGROUP"))
        {
            struct xmlAttr *attribs = nodes->attribs;
            (*rvpd) = calloc(1, sizeof(SETTING));
            (*rvpr) = calloc(1, sizeof(SETTING));
            if ((*rvpd) && (&rvpr))
            {
                char *select = NULL;
                int order = 0;
                (*rvpd)->type = (*rvpr)->type = e_tree;
                while (attribs)
                {
                    if (IsAttrib(attribs, "CLASS"))
                    {
                        (*rvpd)->cls = strdup(attribs->value);
                        (*rvpr)->cls = strdup(attribs->value);
                    }
                    if (IsAttrib(attribs, "NAME"))
                    {
                        (*rvpd)->displayName = strdup(attribs->value);
                        (*rvpr)->displayName = strdup(attribs->value);
                    }
                    if (IsAttrib(attribs, "VERSION"))
                        version = atoi(attribs->value);
                    if (IsAttrib(attribs, "SELECT"))
                        select = strdup(attribs->value);
                    if (IsAttrib(attribs, "ORDER"))
                        order = atoi(attribs->value);
                        
                    attribs = attribs->next;
                } 
                (*rvpd)->select = (*rvpr)->select = select;
                (*rvpd)->order = (*rvpr)->order = order;
                LoadChildren(nodes->children, version, *rvpd, TRUE);
                LoadChildren(nodes->children, version, *rvpr, FALSE);
                rvpd = &(*rvpd)->next;
                rvpr = &(*rvpr)->next;
            }
        }
        nodes = nodes->next;
    }
    xmlFree(root);
    return rv;
}
